#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include "Player.h"
#include "Projectile.h"
#include "EnemyManager.h"    // NEW

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    QSize sizeHint() const override { return {800, 600}; }

protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;

private slots:
    void onLoop();

private:
    void tryShoot();

    QTimer m_timer;
    QElapsedTimer m_elapsed;
    Player m_player;

    // input state
    bool m_leftDown{false};
    bool m_rightDown{false};
    bool m_spaceShoot{false};

    std::vector<Projectile> m_projectiles;

    // shooting cooldown (seconds)
    const double m_shotCooldownSeconds = 0.25;
    double m_timeSinceLastShot{0.0}; // seconds

    // NEW: enemy manager, score, lives
    EnemyManager m_enemyManager;
    int m_score{0};
    int m_lives{3};
};
