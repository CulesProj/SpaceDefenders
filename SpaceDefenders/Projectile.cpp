#include "Projectile.h"
#include <QRect>

Projectile::Projectile(double x, double y, double w, double h, double speed)
    : m_x(x), m_y(y), m_w(w), m_h(h), m_speed(speed)
{}

void Projectile::update(double dt)
{
    m_y -= m_speed * dt;
}

void Projectile::draw(QPainter &p) const
{
    QRectF r(m_x - m_w/2.0, m_y - m_h, m_w, m_h);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRect(r);
}

bool Projectile::isOffscreen(double windowHeight) const
{
    return (m_y+0.0) < 0.0;
}
