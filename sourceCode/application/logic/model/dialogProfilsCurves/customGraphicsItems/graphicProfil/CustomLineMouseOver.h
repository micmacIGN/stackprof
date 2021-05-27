#ifndef TESTCUSTOMLINEMOUSEOVER_H
#define TESTCUSTOMLINEMOUSEOVER_H

#include <QGraphicsLineItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

class CustomLineMouseOver: //public QObject, public QGraphicsItem {
                           public QGraphicsObject {

    Q_OBJECT

public:
    //@#LP align param type name (double;qreal)
    CustomLineMouseOver(QColor QColor_unselected, QColor QColor_selected,
                        qreal x,
                        double xrange_minValue, double xrange_maxValue,
                        qreal y, qreal height,
                        bool bCanBeMouseMoved,
                        QGraphicsItem *parent=nullptr);

    void setSelected(bool newState);
    void setAsMouseOver(bool newState);

    //QPainterPath shape() const override ;
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void setXPos(double x);
    void setXYPos();

    void set_lineHeight(double lineHeight);

    void setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster);

    void set_canBeMouseMoved(bool bState);

    ~CustomLineMouseOver() override;

    int type() const override;

signals:
    void signal_customLineMouseOver_movedTo(double x);
    void signal_selectStateChanged(bool selectedState);
    void signal_customLineMouseOver_mouseReleased();

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

public slots:
    void slot_selectStateChangedFromOutside(bool selectedState);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void setPensStyle();
    void setPensWidthF(double dWidth);

    void computeBoundingBox();

private:
    qreal _x;

    double _xrange_minValue;
    double _xrange_maxValue;

    qreal _y;
    qreal _height;

    bool _bCanBeMouseMoved;

    bool _bLocationInAdjustement;

    bool _bSelectedState; //mouse hover the item

    QPen _pUnselected;
    QPen _pSelected;
    QPen *_ptrPenInUse;

    double _dThickness;

    QPointF _offsetMouseClickToPointCenterPos; //stores the offset between the mouse cursor location when clicking for location adjustement and then point center

    QRectF _qrectFBBox;

};

#endif // TESTCUSTOMLINEMOUSEOVER_H
