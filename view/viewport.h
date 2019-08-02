#pragma once

#include <QWidget>
#include <QGraphicsView>

#include "view/programview.h"



class Viewport : public QGraphicsView
{
    Q_OBJECT

public:

    Viewport(QWidget* parent = nullptr);

    void set_view(ProgramView* view_model /* , ProgramScope scope */);


    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;


signals:


public slots:


private:

    void zoom_in();

    void zoom_out();

    double zoom_exponent = 1.2;
    bool zoom_inverted = false;

    double current_zoom = 1.0;
    double zoom_limit = 0.12;

    ProgramView* view_model;

};
