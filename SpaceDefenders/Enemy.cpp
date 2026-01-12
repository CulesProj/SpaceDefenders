#include "Enemy.h"
#include <QRectF>

void Enemy::draw(QPainter &p, double enemyW, double enemyH) const
{
    if (!alive) return;

    QColor color;
    switch (type) {
    case EnemyType::Basic:  color = QColor(200, 200, 255); break; // pale blue
    case EnemyType::Shooter: color = QColor(255, 200, 200); break; // pale red
    case EnemyType::Diver:   color = QColor(200, 255, 200); break; // pale green
    }

    p.setPen(Qt::NoPen);
    p.setBrush(color);
    QRectF r(pos.x(), pos.y(), enemyW, enemyH);
    p.drawRect(r);

    // optional small eye for aesthetic
    p.setBrush(Qt::black);
    p.drawRect(QRectF(pos.x() + enemyW*0.4, pos.y() + enemyH*0.2, enemyW*0.2, enemyH*0.2));
}
