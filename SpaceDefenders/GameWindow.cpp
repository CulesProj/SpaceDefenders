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

    // Initialize enemies (rows, cols, startX, startY, spacingX, spacingY)
    m_enemyManager.initGrid(5, 11, 80.0, 40.0, 56.0, 44.0);
}

void GameWindow::paintEvent(QPaintEvent * /*ev*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // clear background
    p.fillRect(rect(), Qt::black);

    // draw enemies first
    m_enemyManager.draw(p);

    // draw player
    m_player.draw(p, static_cast<double>(height()));

    // draw projectiles (both player and enemy shots)
    for (const auto &proj : m_projectiles) {
        proj.draw(p);
    }

    // optional: draw HUD (score / lives)
    p.setPen(Qt::white);
    p.drawText(8, 16, QString("Score: %1").arg(m_score));
    p.drawText(8, 32, QString("Lives: %1").arg(m_lives));
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
        tryShoot(); // immediate shot on press
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
    if (m_timeSinceLastShot >= m_shotCooldownSeconds) {
        QPointF muzzle = m_player.muzzlePosition(static_cast<double>(height()));
        Projectile p(muzzle.x(), muzzle.y()); // positive speed -> moves up
        m_projectiles.push_back(std::move(p));
        m_timeSinceLastShot = 0.0;
        std::cout << "player shot\n";
    }
}

void GameWindow::onLoop()
{
    qint64 ms = m_elapsed.restart();
    double dt = ms / 1000.0;

    // update cooldown first
    m_timeSinceLastShot += dt;

    // update enemies; this may append enemy projectiles to m_projectiles
    double playerCenterX = m_player.x() + m_player.width() * 0.5;
    m_enemyManager.update(dt, static_cast<double>(width()), playerCenterX, m_projectiles);

    // update movement
    int dir = 0;
    if (m_leftDown && !m_rightDown) dir = -1;
    if (m_rightDown && !m_leftDown) dir = 1;
    m_player.update(dt, dir, static_cast<double>(width()));

    // if holding space, attempt to shoot (cooldown controls rate)
    if (m_spaceShoot) tryShoot();

    // update projectiles positions
    for (auto &proj : m_projectiles) {
        proj.update(dt);
    }

    // --- COLLISIONS ---
    // We'll remove projectiles that hit something. Iterate backwards so erase is safe.
    // Projectile "ownership": positive speed => player's bullet (moves up), negative => enemy bullet (moves down).
    const auto &enemies = m_enemyManager.getEnemies();
    // iterate projectiles from end -> start
    for (int i = static_cast<int>(m_projectiles.size()) - 1; i >= 0; --i) {
        Projectile &proj = m_projectiles[i];
        QRectF projRect = proj.rect();

        bool removed = false;

        if (proj.speed() > 0.0) {
            // Player bullet — check collision with enemies
            const auto &enemyList = m_enemyManager.getEnemies(); // read-only
            for (size_t ei = 0; ei < enemyList.size(); ++ei) {
                const Enemy &e = enemyList[ei];
                if (!e.alive) continue;
                QRectF enemyRect(e.pos.x(), e.pos.y(), /*enemyW*/ 40.0, /*enemyH*/ 28.0);
                if (projRect.intersects(enemyRect)) {
                    // hit: kill enemy and remove projectile
                    m_enemyManager.killEnemy(ei);
                    m_score += 100; // reward
                    removed = true;
                    break;
                }
            }
        } else {
            // Enemy bullet — check collision with player
            double playerTopY = static_cast<double>(height()) - 40.0 - m_player.height();
            QRectF playerRect(m_player.x(), playerTopY, m_player.width(), m_player.height());
            if (projRect.intersects(playerRect)) {
                // player hit
                m_lives -= 1;
                std::cout << "player hit, lives=" << m_lives << "\n";
                // optional: reset player position, or trigger invincibility frames
                // e.g. m_player.setX( (width() - m_player.width()) * 0.5 );
                removed = true;
            }
        }

        if (removed) {
            // erase projectile at i
            m_projectiles.erase(m_projectiles.begin() + i);
            continue;
        }

        // remove off-screen projectiles as usual
        if (proj.isOffscreen(static_cast<double>(height()))) {
            m_projectiles.erase(m_projectiles.begin() + i);
        }
    } // end projectile loop

    update(); // schedule repaint
}
