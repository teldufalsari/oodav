#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    //setStyleSheet("background-color:black");
    resize(BOARD_WIDTH, BOARD_HEIGHT);
    LoadImages();
    board_ = new GameBoard(this);
    score_counter_ = new QLCDNumber(this);
    score_counter_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    shit_eaten_counter_ = new QLCDNumber(this);
    shit_eaten_counter_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    speed_counter_ = new QLCDNumber(this);
    speed_counter_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //speed_knob_ = new QDial(this);
    toxic_bar_ = new QProgressBar;
    toxic_bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toxic_bar_->setMinimum(0);
    toxic_bar_->setMaximum(MAX_TOXICITY);

    QLabel* score_label = new QLabel("SCORE", this);
    QLabel* shit_label = new QLabel("SHIT", this);
    QLabel* toxic_label = new QLabel("TOXICITY", this);
    QLabel* speed_label = new QLabel("SPEED", this);

    QVBoxLayout* score_box = new QVBoxLayout;
    score_box->addWidget(score_label, 0, Qt::AlignHCenter);
    score_box->addWidget(score_counter_);

    QVBoxLayout* shit_box = new QVBoxLayout;
    shit_box->addWidget(shit_label, 0, Qt::AlignHCenter);
    shit_box->addWidget(shit_eaten_counter_);

    QVBoxLayout* toxic_box = new QVBoxLayout;
    toxic_box->addWidget(toxic_label, 0, Qt::AlignHCenter);
    toxic_box->addWidget(toxic_bar_);

    QVBoxLayout* speed_box = new QVBoxLayout;
    speed_box->addWidget(speed_label, 0, Qt::AlignHCenter);
    speed_box->addWidget(speed_counter_);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(board_, 0, Qt::AlignCenter);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addLayout(score_box);
    hbox->addLayout(shit_box);
    hbox->addLayout(toxic_box);
    hbox->addLayout(speed_box);
    //hbox->addWidget(speed_knob_);
    vbox->addLayout(hbox, Qt::AlignBottom);
    setLayout(vbox);
    offset_ = board_->mapToGlobal(board_->rect().topLeft());
    QDateTime seed_time = QDateTime::currentDateTime();
    rannd.seed(seed_time.toTime_t());
    StartGame();
}

MainWindow::~MainWindow()
{
}

void MainWindow::LoadImages()
{
    head.load("Resources/head.png");
    segment.load("Resources/segment.png");
    tomato.load("Resources/tomato.png");
    fat.load("Resources/thick.png");
    shit.load("Resources/shit.png");
}

void MainWindow::StartGame()
{
    score_ = 0;
    toxicity_ = 0;
    toxic_bar_->setValue(toxicity_);
    score_counter_->display(score_);
    shit_eaten_counter_->display(0);
    speed_counter_->display(1000 / period_);
    segments.clear();
    shit_list.clear();
    direction_ = D_LEFT;
    segments.push_back(Segment(350, 300, Segment::head));
    segments.push_back(Segment(360, 300, Segment::body));
    segments.push_back(Segment(370, 300, Segment::body));
    PlaceTomato();
    state_ = ST_IN_GAME;
    timer_ = startTimer(period_);
}

void MainWindow::PlaceTomato()
{
    tomato_location.x = (rannd.generate() % 80) * 10; //magic numbers!!1!
    tomato_location.y = (rannd.generate() % 60) * 10;
}

void MainWindow::CheckTomato()
{
    Segment head = segments.first();
    if (head.x == tomato_location.x && head.y == tomato_location.y) {
        PlaceTomato();
        segments[1].type = Segment::fat;
        score_++;
        score_counter_->display(score_);
    } else {
        if (segments.last().type == Segment::fat)
            shit_list.push_back(segments.last());
        segments.pop_back();
    }
}

void MainWindow::CheckShit()
{
    Segment head = segments.first();
    for (auto& v : shit_list) {
        if (v.x == head.x && v.y == head.y) {
            shit_list.removeOne(v);
            shit_eaten_counter_ += 1;
            toxicity_ += 10000;
            if (toxicity_ >= MAX_TOXICITY) {
                state_ = ST_GAMOVER;
                killTimer(timer_);
            }
        }
    }
}

void MainWindow::CheckCollision()
{
    Segment head(segments.first());
    for (int i = segments.count() - 1; i > 4; i--)
        if(head.x == segments[i].x && head.y == segments[i].y) {
            state_ = ST_GAMOVER;
            killTimer(timer_);
        }
    if (head.y >= BOARD_HEIGHT || head.y < 0 || head.x < 0 || head.x >= BOARD_WIDTH) {
        state_ = ST_GAMOVER;
        killTimer(timer_);
    }
}

void MainWindow::Draw()
{
    QPainter qp(this);
    if (state_ != ST_GAMOVER) {
        qp.drawImage(offset_.x() + tomato_location.x, offset_.y() + tomato_location.y, tomato);
        for (auto& v : shit_list) {
            qp.drawImage(offset_.x() + v.x, offset_.y() + v.y, shit);
        }
        for (auto& v : segments) {
            DrawSegment(qp, v);
        }
    } else {
        ShowGameOver(qp);
    }
}

void MainWindow::Move()
{
    Segment new_head(segments.first());
    segments.first().type = Segment::body;
    switch(direction_) {

    case D_UP:
        new_head.y -= DOT_SIZE;
        break;
    case D_DOWN:
        new_head.y += DOT_SIZE;
        break;
    case D_LEFT:
        new_head.x -= DOT_SIZE;
        break;
    case D_RIGHT:
        new_head.x += DOT_SIZE;
        break;
    default:
        ; // unreachable
    }
    segments.push_front(new_head);
}

void MainWindow::ShowGameOver(QPainter& qp)
{
    QString message = "GAME OVER";
    QFont font("Source Code Variable", 20, QFont::Bold);
    QFontMetrics fm(font);
    int textWidth = fm.width(message);

    qp.setPen(QColor(Qt::red));
    qp.setFont(font);
    int h = height();
    int w = width();
    qp.translate(QPoint(w/2, h/2));
    qp.drawText(-textWidth/2, 0, message);

    qp.setPen(QColor(Qt::white));
    qp.setFont(QFont("Source Code Variable", 10, QFont::Bold));
    message = "Press ESC to start again";
    textWidth = fm.width(message);

    qp.drawText(-textWidth/2, 30, message);
}

void MainWindow::DrawSegment(QPainter &qp, const Segment &seg)
{
    switch(seg.type) {
    case Segment::body:
        qp.drawImage(offset_.x() +  seg.x, offset_.y() +  seg.y, segment);
        break;
    case Segment::head:
        qp.drawImage(offset_.x() +  seg.x, offset_.y() +  seg.y, head);
        break;
    case Segment::fat:
        qp.drawImage(offset_.x() +  seg.x - 1, offset_.y() +  seg.y - 1, fat);
        break;
    default:
        ;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if (state_ == ST_IN_GAME) {
        Move();
        CheckTomato();
        CheckShit();
        CheckCollision();
        if (toxicity_ >= 0)
            toxicity_ -= 8;
        toxic_bar_->setValue(toxicity_);
    }
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Draw();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        if (direction_ != D_RIGHT)
            direction_ = D_LEFT;
        break;

    case Qt::Key_Right:
        if (direction_ != D_LEFT)
            direction_ = D_RIGHT;
        break;

    case Qt::Key_Up:
        if (direction_ != D_DOWN)
            direction_ = D_UP;
        break;

    case Qt::Key_Down:
        if (direction_ != D_UP)
            direction_ = D_DOWN;
        break;

    case Qt::Key_Escape:
        switch (state_) {
        case ST_IN_GAME:
            killTimer(timer_);
            state_ = ST_PAUSED;
            break;
        case ST_PAUSED:
            timer_ = startTimer(period_);
            state_ = ST_IN_GAME;
            break;
        case ST_GAMOVER:
            StartGame();
            break;
        default:
            ;
        }

    default:
        ;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    offset_ = board_->mapTo(this, board_->rect().topLeft());
}

Point::Point() :
    x(0), y(0)
{
}

Point::Point(int new_x, int new_y) :
    x(new_x), y(new_y)
{
}

Segment::Segment() :
    type(none)
{
}

Segment::Segment(int new_x, int new_y, int new_type) :
    Point(new_x, new_y),
    type(new_type)
{
}

GameBoard::GameBoard(QWidget *parent) : QFrame(parent)
{
    setFixedSize(BOARD_WIDTH, BOARD_HEIGHT);
    setLineWidth(2);
    setMidLineWidth(2);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Raised);
}
