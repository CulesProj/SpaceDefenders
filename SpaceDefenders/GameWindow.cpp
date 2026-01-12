// GameWindow.cpp (fixed)
#include "GameWindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <algorithm>
#include <iostream>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
    m_player(300.0, 80.0, 20.0, 350.0)
{
    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_timer, &QTimer::timeout, this, &GameWindow::onLoop);
    m_timer.start(16); // ~60 Hz

    m_elapsed.start();

    // Allow the first shot immediately
    m_timeSinceLastShot = m_shotCooldownSeconds;
}

void GameWindow::paintEvent(QPaintEvent * /*ev*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // clear background
    p.fillRect(rect(), Qt::black);

    // draw player
    m_player.draw(p, static_cast<double>(height()));

    // draw projectiles
    for (const auto &proj : m_projectiles) {
        proj.draw(p);
    }
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
    case Qt::Key_Space:
        m_spaceShoot = true;
        tryShoot(); // attempt an immediate shot on press
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
    case Qt::Key_Space:
        m_spaceShoot = false;
        break;
    default:
        QWidget::keyReleaseEvent(ev);
    }
}

void GameWindow::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        tryShoot();
    } else {
        QWidget::mousePressEvent(ev);
    }
}

void GameWindow::tryShoot()
{
    // if cooldown elapsed, spawn projectile at player's muzzle
    if (m_timeSinceLastShot >= m_shotCooldownSeconds) {
        QPointF muzzle = m_player.muzzlePosition(static_cast<double>(height()));
        Projectile p(muzzle.x(), muzzle.y());
        m_projectiles.push_back(std::move(p));
        m_timeSinceLastShot = 0.0;

        // debug logging (newline so it flushes reliably)
        std::cout << "shot\n";
    }
}

void GameWindow::onLoop()
{
    qint64 ms = m_elapsed.restart();
    double dt = ms / 1000.0;

    // update cooldown timer first so holding Space behaves as expected
    m_timeSinceLastShot += dt;

    // update movement
    int dir = 0;
    if (m_leftDown && !m_rightDown) dir = -1;
    if (m_rightDown && !m_leftDown) dir = 1;

    // if player is holding space, attempt to shoot (cooldown controls rate)
    if (m_spaceShoot) {
        tryShoot();
    }

    m_player.update(dt, dir, static_cast<double>(width()));

    // update projectiles
    for (auto &proj : m_projectiles) {
        proj.update(dt);
    }

    // remove off-screen projectiles
    auto it = std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                             [this](const Projectile &p) { return p.isOffscreen(static_cast<double>(height())); });
    m_projectiles.erase(it, m_projectiles.end());

    update(); // schedule repaint
}
