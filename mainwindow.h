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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Point
{
    int x, y;
    Point();
    Point(int new_x, int new_y);
    ~Point() = default;
};

struct Segment : public Point
{
    int type;
    enum types {none = 0, head, body, fat};
    Segment();
    Segment(int new_x, int new_y, int new_type);
    ~Segment() = default;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    const int DOT_SIZE = 10;
    int period = 100;
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
    void CheckCollision();
    void Draw();
    void Move();
    void ShowGameOver(QPainter& qp);
    void DrawSegment(QPainter& qp, const Segment& seg);

protected:
    void timerEvent(QTimerEvent* event);
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);
};
#endif // MAINWINDOW_H
