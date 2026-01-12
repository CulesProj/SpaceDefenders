#include "Projectile.h"
#include <QRectF>

Projectile::Projectile(double x, double y, double w, double h, double speed)
    : m_x(x), m_y(y), m_w(w), m_h(h), m_speed(speed)
{}

void Projectile::update(double dt)
{
    // previous convention: m_y -= m_speed * dt (positive speed moves projectile upward).
    m_y -= m_speed * dt;
}

void Projectile::draw(QPainter &p) const
{
    QRectF r = rect();
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRect(r);
}

bool Projectile::isOffscreen(double /*windowHeight*/) const
{
    // offscreen above top
    return (m_y) < 0.0;
}
