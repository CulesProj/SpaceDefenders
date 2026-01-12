#include "EnemyManager.h"
#include <algorithm>
#include <cmath>
#include <random>

EnemyManager::EnemyManager()
{
    std::random_device rd;
    rng.seed(rd());
}

void EnemyManager::initGrid(int rows, int cols,
                            double startX, double startY,
                            double sX, double sY)
{
    enemies.clear();
    originX = startX;
    originY = startY;
    spacingX = sX;
    spacingY = sY;

    enemies.reserve(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            // for variety: make top rows basic, middle shooters, bottom divers
            EnemyType type = EnemyType::Basic;
            if (r >= rows/3 && r < 2*rows/3) type = EnemyType::Shooter;
            if (r >= 2*rows/3) type = EnemyType::Diver;

            Enemy e(type, r, c);
            e.localX = c * spacingX;
            e.localY = r * spacingY;
            e.pos = QPointF(originX + e.localX, originY + e.localY);
            // initial shoot timers randomized a bit
            std::uniform_real_distribution<double> dist(0.0,  (type == EnemyType::Shooter ? shooterCooldown : basicCooldown));
            e.shootTimer = dist(rng);
            enemies.push_back(std::move(e));
        }
    }
}

void EnemyManager::recomputeFormationBounds(double &minX, double &maxX) const
{
    minX = std::numeric_limits<double>::infinity();
    maxX = -std::numeric_limits<double>::infinity();

    for (const auto &e : enemies) {
        if (!e.alive) continue;
        if (e.state != EnemyState::InFormation) continue;
        double x = originX + e.localX;
        minX = std::min(minX, x);
        maxX = std::max(maxX, x + enemyW);
    }
    if (minX == std::numeric_limits<double>::infinity()) {
        // no in-formation alive enemies
        minX = 0.0;
        maxX = 0.0;
    }
}

void EnemyManager::update(double dt, double windowW, double playerX, std::vector<Projectile> &outProjectiles)
{
    // 1) move formation origin and bounce on edges
    originX += dir * formationSpeed * dt;

    double minX, maxX;
    recomputeFormationBounds(minX, maxX);

    // if formation hits edge, reverse & descend
    if (minX < 0.0) {
        dir = 1;
        originY += descendStep;
    } else if (maxX > windowW) {
        dir = -1;
        originY += descendStep;
    }

    // random helper
    std::uniform_real_distribution<double> uniform01(0.0, 1.0);

    // 2) per-enemy update
    for (auto &e : enemies) {
        if (!e.alive) continue;

        if (e.state == EnemyState::InFormation) {
            // world pos = formation origin + local offsets
            e.pos = QPointF(originX + e.localX, originY + e.localY);

            // diving chance (only diver type)
            if (e.type == EnemyType::Diver) {
                double chanceThisFrame = diverChancePerSecond * dt;
                if (uniform01(rng) < chanceThisFrame) {
                    // start dive: set start/target and switch state
                    e.state = EnemyState::Diving;
                    e.diveT = 0.0;
                    e.diveStart = e.pos;
                    // aim at player's x and a Y deeper than player (or near bottom)
                    double targetY = std::min(windowW * 0.6, originY + 300.0); // don't go too far
                    e.diveTarget = QPointF(playerX - enemyW*0.5, originY + 200.0); // target based on playerX
                }
            }
        }
        else if (e.state == EnemyState::Diving) {
            // advance t
            e.diveT += dt / diveDuration;
            if (e.diveT >= 1.0) {
                // reached dive target, switch to Returning
                e.diveT = 1.0;
                e.state = EnemyState::Returning;
                // set up return interpolation
                e.diveStart = e.pos;
                // target is formation position at time of return (approx)
                QPointF formationPos(originX + e.localX, originY + e.localY);
                e.diveTarget = formationPos;
                e.diveT = 0.0;
            } else {
                // simple quadratic curve for a nicer arc: p(t) = (1-t)^2*start + 2(1-t)t*mid + t^2*end
                // define mid control a bit below start->end for an arc
                QPointF mid((e.diveStart.x() + e.diveTarget.x()) * 0.5, std::max(e.diveStart.y(), e.diveTarget.y()) + 80.0);
                double t = e.diveT;
                QPointF p = (1-t)*(1-t)*e.diveStart + 2*(1-t)*t*mid + t*t*e.diveTarget;
                e.pos = p;
            }
        }
        else if (e.state == EnemyState::Returning) {
            e.diveT += dt / returnDuration;
            if (e.diveT >= 1.0) {
                e.diveT = 1.0;
                e.state = EnemyState::InFormation;
                // snap back to formation
                e.pos = QPointF(originX + e.localX, originY + e.localY);
            } else {
                double t = e.diveT;
                // linear interpolation from diveStart to diveTarget
                e.pos = e.diveStart * (1.0 - t) + e.diveTarget * t;
            }
        }

        // Shooting logic: decrease timer; when <= 0 try to fire (probability check)
        e.shootTimer -= dt;
        if (e.shootTimer <= 0.0) {
            // Decide cooldown reset depending on type
            double cooldown = (e.type == EnemyType::Shooter) ? shooterCooldown : basicCooldown;

            // Compute chance to actually shoot (to avoid all shooting simultaneously).
            // We'll use per-frame chance scaled by dt and base probability.
            double shootChance = shootProbabilityPerSecond * dt;
            if (uniform01(rng) < shootChance) {
                // spawn projectile: enemy shots travel downward, so pass negative speed
                // spawn at enemy center
                double px = e.pos.x() + enemyW * 0.5;
                double py = e.pos.y() + enemyH;
                // negative speed to move downwards (Projectile::update subtracts m_speed from y)
                double enemyShotSpeed = -300.0; // pixels/sec downward
                Projectile shot(px, py, 6.0, 12.0, enemyShotSpeed);
                outProjectiles.push_back(std::move(shot));
                // Reset timer to cooldown (with small jitter)
                std::uniform_real_distribution<double> jitter(0.0, 0.4 * cooldown);
                e.shootTimer = cooldown + jitter(rng);
            } else {
                // no shot, try again after a short randomized interval
                std::uniform_real_distribution<double> small(0.05, 0.5);
                e.shootTimer = small(rng);
            }
        }
    } // end for enemies

    // dynamic difficulty: increase formation speed as enemies die (classic)
    int aliveCount = 0;
    for (auto &e : enemies) if (e.alive) ++aliveCount;
    int total = static_cast<int>(enemies.size());
    if (total > 0) {
        double aliveRatio = double(aliveCount) / double(total);
        // speed rises as fewer enemies remain
        formationSpeed = 40.0 * (1.0 + (1.0 - aliveRatio) * 2.0); // up to 3x speed
    }
}

void EnemyManager::draw(QPainter &p) const
{
    for (const auto &e : enemies) {
        if (!e.alive) continue;
        e.draw(const_cast<QPainter&>(p), enemyW, enemyH); // draw expects painter + size
    }
}

bool EnemyManager::allDead() const
{
    for (const auto &e : enemies) {
        if (e.alive) return false;
    }
    return true;
}

void EnemyManager::killEnemy(size_t index)
{
    if (index < enemies.size()) {
        enemies[index].alive = false;
        enemies[index].state = EnemyState::Dead;
    }
}
