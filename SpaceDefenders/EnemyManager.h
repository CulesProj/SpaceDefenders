#pragma once
#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include "Enemy.h"
#include "Projectile.h"
#include <vector>
#include <random>
#include <QPainter>

class EnemyManager {
public:
    EnemyManager();

    // initialize a regular grid: specify counts and formation origin/spacing
    void initGrid(int rows, int cols,
                  double startX, double startY,
                  double spacingX, double spacingY);

    // update formation and enemies; supply playerX so diver can aim
    // outProjectiles will get any enemy shots appended
    void update(double dt, double windowW, double playerX, std::vector<Projectile> &outProjectiles);

    void draw(QPainter &p) const;

    // returns true if no alive enemies remain
    bool allDead() const;

    // optional: kill enemy at index (useful after collision)
    void killEnemy(size_t index);

    // expose enemies (read-only) for collision checks
    const std::vector<Enemy>& getEnemies() const { return enemies; }

private:
    std::vector<Enemy> enemies;

    // formation origin and movement
    double originX = 100.0;
    double originY = 50.0;
    int dir = 1;                    // +1 moving right, -1 left
    double formationSpeed = 40.0;   // pixels per second
    double descendStep = 20.0;
    double spacingX = 64.0;
    double spacingY = 48.0;

    // enemy size for drawing / collision
    double enemyW = 40.0;
    double enemyH = 28.0;

    // dive parameters
    double diverChancePerSecond = 0.15;   // per-diver chance to start dive (while in formation)
    double diveDuration = 0.9;            // seconds to complete dive
    double returnDuration = 0.9;          // seconds to return

    // shooting parameters
    double basicCooldown = 3.0;   // seconds between shots for basic
    double shooterCooldown = 1.2; // faster shooter
    double shootProbabilityPerSecond = 0.6; // base chance (scaled per dt)

    // random engine
    std::mt19937 rng;

    // recompute bounding box used for edge detection (only considers in-formation, alive enemies)
    void recomputeFormationBounds(double &minX, double &maxX) const;
};

#endif // ENEMYMANAGER_H
