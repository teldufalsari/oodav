#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QObject>
#include <QTime>
#include <QList>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QLabel>
#include <QLCDNumber>
#include <QSizePolicy>
#include <QProgressBar>
#include <QDial>

const int BOARD_WIDTH = 800;
const int BOARD_HEIGHT = 600;
const int MAX_TOXICITY = 30000;

struct Point
{
    int x, y;
    Point();
    Point(int new_x, int new_y);
    ~Point() = default;
    friend inline bool operator ==(const Point& lhs, const Point& rhs) {
        return (lhs.x == rhs.x && lhs.y == rhs.y);
    }
};



class GameBoard : public QFrame
{
    Q_OBJECT
public:
    GameBoard(QWidget* parent = nullptr);
};


struct Segment : public Point
{
    int type;
    enum types {none = 0, head, body, fat};
    Segment();
    Segment(int new_x, int new_y, int new_type);
    ~Segment() = default;
};

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    GameBoard* board_;
    QLCDNumber* shit_eaten_counter_;
    QLCDNumber* score_counter_;
    QLCDNumber* speed_counter_;
    //QDial* speed_knob_;
    QProgressBar* toxic_bar_;
    const int DOT_SIZE = 10;
    int period_ = 100;
    QList<Segment> segments;
    QList<Point> shit_list;
    Point tomato_location;
    QRandomGenerator rannd;
    QImage head;
    QImage segment;
    QImage tomato;
    QImage fat;
    QImage shit;
    int timer_;
    int state_;
    int score_;
    int toxicity_;
    QPoint offset_;
    enum states {ST_IN_GAME = 1, ST_PAUSED, ST_GAMOVER};
    int direction_;
    enum directions {D_UP = 1, D_DOWN, D_LEFT, D_RIGHT};

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void LoadImages();
    void StartGame();
    void PlaceTomato();
    void CheckTomato();
    void CheckShit();
    void CheckCollision();
    void Draw();
    void Move();
    void ShowGameOver(QPainter& qp);
    void DrawSegment(QPainter& qp, const Segment& seg);

protected:
    void timerEvent(QTimerEvent* event);
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent* event);
};
#endif // MAINWINDOW_H
