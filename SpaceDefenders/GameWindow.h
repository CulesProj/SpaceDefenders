#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "Player.h"

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    QSize sizeHint() const override { return {800, 600}; }

protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

private slots:
    void onLoop();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsed;
    Player m_player;

    // input state
    bool m_leftDown{false};
    bool m_rightDown{false};
};
