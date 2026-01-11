#include "Player.h"
#include <algorithm>

Player::Player(double x, double w, double h, double speed)
    : m_x(x), m_w(w), m_h(h), m_speed(speed)
{}

void Player::update(double dt, int direction, double windowWidth)
{
    // direction: -1, 0, +1
    m_x += direction * m_speed * dt;

    // clamp within [0, windowWidth - playerWidth]
    double minX = 0.0;
    double maxX = std::max(0.0, windowWidth - m_w);
    if (m_x < minX) m_x = minX;
    if (m_x > maxX) m_x = maxX;
}

void Player::draw(QPainter &p, double windowHeight) const
{
    QRectF r(m_x, windowHeight - 40.0 - m_h, m_w, m_h);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRect(r);
}
