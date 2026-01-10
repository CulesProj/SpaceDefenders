#pragma once
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <vector>

struct Bullet { QPointF pos; QPointF vel; };

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
private slots:
    void onLoop();
private:
    QTimer m_timer;
    QPointF m_playerPos;
    std::vector<Bullet> m_bullets;
    int m_width = 800;
    int m_height = 600;
};
