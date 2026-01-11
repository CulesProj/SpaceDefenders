#include "GameWindow.h"
#include <QPainter>
#include <QKeyEvent>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
    m_player(300.0, 80.0, 20.0, 350.0)
{
    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_timer, &QTimer::timeout, this, &GameWindow::onLoop);
    m_timer.start(16); // ~60 Hz

    m_elapsed.start();
}

void GameWindow::paintEvent(QPaintEvent * /*ev*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // clear background
    p.fillRect(rect(), Qt::black);

    // draw player
    m_player.draw(p, static_cast<double>(height()));
}

void GameWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->isAutoRepeat()) {
        QWidget::keyPressEvent(ev);
        return;
    }

    switch (ev->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        m_leftDown = true;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        m_rightDown = true;
        break;
    default:
        QWidget::keyPressEvent(ev);
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *ev)
{
    if (ev->isAutoRepeat()) {
        QWidget::keyReleaseEvent(ev);
        return;
    }

    switch (ev->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        m_leftDown = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        m_rightDown = false;
        break;
    default:
        QWidget::keyReleaseEvent(ev);
    }
}

void GameWindow::onLoop()
{
    qint64 ms = m_elapsed.restart();
    double dt = ms / 1000.0;

    int dir = 0;
    if (m_leftDown && !m_rightDown) dir = -1;
    if (m_rightDown && !m_leftDown) dir = 1;

    m_player.update(dt, dir, static_cast<double>(width()));

    update(); // schedule repaint
}
