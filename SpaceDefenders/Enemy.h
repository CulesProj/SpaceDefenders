#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include <QPainter>
#include <QPointF>

enum class EnemyType { Basic = 0, Shooter = 1, Diver = 2 };
enum class EnemyState { InFormation = 0, Diving = 1, Returning = 2, Dead = 3 };

struct Enemy {
    Enemy() = default;
    Enemy(EnemyType t, int r, int c)
        : type(t), row(r), col(c) {}

    EnemyType type = EnemyType::Basic;
    EnemyState state = EnemyState::InFormation;
    bool alive = true;
    int row = 0;
    int col = 0;

    // world position (computed by manager for InFormation, directly modified for Diving/Returning)
    QPointF pos{0.0, 0.0};

    // formation-local offset (col * spacingX, row * spacingY)
    double localX = 0.0;
    double localY = 0.0;

    // timers & state params
    double shootTimer = 0.0;    // seconds until next shot
    double diveT = 0.0;         // interpolation param for dive/return (0..1)
    QPointF diveStart;
    QPointF diveTarget;

    // draw helper
    void draw(QPainter &p, double enemyW, double enemyH) const;
};

#endif // ENEMY_H
