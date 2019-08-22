#pragma once

#include <QGraphicsItem>


class OperatorView;


class BaseDataView : public QGraphicsItem
{

public:

    BaseDataView(OperatorView& parent);


    QRectF boundingRect() const override;

    void set_bounds(double x, double y, double w, double h);


    //virtual void draw(int res_x, int res_y) = 0;

private:

    // Arbitrary initial bounding box, will be changed by OperatorView calling set_bounds().
    QRectF bounds{ 0, 0, 10, 10 };


};
