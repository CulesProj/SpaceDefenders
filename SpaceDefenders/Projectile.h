#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QPainter>
#include <QRectF>

class Projectile
{
public:
    Projectile(double x, double y, double w = 6.0, double h = 12.0, double speed = 600.0);

    void update(double dt);

    void draw(QPainter &p) const;

    bool isOffscreen(double windowHeight) const;

    double x() const { return m_x; }
    double y() const { return m_y; }

    // NEW: accessors for collision logic
    double speed() const { return m_speed; }
    QRectF rect() const { return QRectF(m_x - m_w/2.0, m_y - m_h, m_w, m_h); }
    double width() const { return m_w; }
    double height() const { return m_h; }

private:
    double m_x;
    double m_y;
    double m_w;
    double m_h;
    double m_speed;
};

#endif // PROJECTILE_H
