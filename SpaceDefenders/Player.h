#pragma once

#include <QRectF>
#include <QPainter>

class Player {
public:
    Player(double x = 0.0, double w = 80.0, double h = 20.0, double speed = 350.0);

    // update player position based on input (-1 left, 0 none, +1 right)
    void update(double dt, int direction, double windowWidth);

    // draw the player at the bottom of the window
    void draw(QPainter &p, double windowHeight) const;

    // accessors
    double x() const { return m_x; }
    void setX(double x) { m_x = x; }
    double width() const { return m_w; }
    double height() const { return m_h; }
    void setSize(double w, double h) { m_w = w; m_h = h; }
    void setSpeed(double s) { m_speed = s; }

private:
    double m_x;
    double m_w;
    double m_h;
    double m_speed; // pixels per second
};
