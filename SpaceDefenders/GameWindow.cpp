#include "GameWindow.h"
#include <QPainter>
#include <QDateTime>
#include <cmath>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), m_playerPos(400, 500)
{
    setFixedSize(m_width, m_height);
    connect(&m_timer, &QTimer::timeout, this, &GameWindow::onLoop);
    m_timer.start(16); // ~60 FPS
}

void GameWindow::onLoop() {
    // advance bullets
    for (auto &b : m_bullets) {
        b.pos += b.vel;
    }
    // remove off-screen bullets
    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(),
                                   [this](const Bullet &b){
                                       return b.pos.y() < 0 || b.pos.y() > m_height
                                              || b.pos.x() < 0 || b.pos.x() > m_width;
                                   }), m_bullets.end());
    update(); // trigger paintEvent
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    // draw player (simple triangle)
    QPointF p1(m_playerPos.x(), m_playerPos.y()-10);
    QPointF p2(m_playerPos.x()-10, m_playerPos.y()+10);
    QPointF p3(m_playerPos.x()+10, m_playerPos.y()+10);
    QVector<QPointF> pts = {p1,p2,p3};
    p.setBrush(Qt::white);
    p.drawPolygon(pts);

    // draw bullets
    for (auto &b : m_bullets) {
        p.drawRect(QRectF(b.pos.x()-2, b.pos.y()-4, 4, 8));
    }
}

void GameWindow::keyPressEvent(QKeyEvent *ev) {
    const int step = 10;
    if (ev->key() == Qt::Key_Left)   m_playerPos.rx() -= step;
    if (ev->key() == Qt::Key_Right)  m_playerPos.rx() += step;
    if (ev->key() == Qt::Key_Space) {
        // shoot bullet
        Bullet b;
        b.pos = m_playerPos + QPointF(0, -16);
        b.vel = QPointF(0, -8);
        m_bullets.push_back(b);
    }
    QWidget::keyPressEvent(ev);
}
