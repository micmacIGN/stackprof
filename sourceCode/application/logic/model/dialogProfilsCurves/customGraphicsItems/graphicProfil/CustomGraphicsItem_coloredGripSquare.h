#ifndef CustomGraphicsItem_coloredGripSquare_H
#define CustomGraphicsItem_coloredGripSquare_H

#include <QGraphicsItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

class CustomGraphicsItem_coloredGripSquare: //public QObject, public QGraphicsItem {
                                              public QGraphicsObject {

    Q_OBJECT

public:
    CustomGraphicsItem_coloredGripSquare(QColor QColor_unselected, QColor QColor_selected,
                                         /*double x, double y,*/
                                         double handlerWidth, double handlerHeight,
                                         bool bCanBeMouseMoved,
                                         bool bFilledAlsoWhenUnselected,
                                         QGraphicsItem *parent=nullptr);

    void setHandlerPositionAndSize(double x, double y, double handlerWidth, double handlerHeight);

    ~CustomGraphicsItem_coloredGripSquare() override ;

    int type() const override;

    void setSelected(bool bNewState);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void setXPos(double x);

    void setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster);

    void set_canBeMouseMoved(bool bState);

signals:
    void signal_coloredSquare_movedTo(double x);
    void signal_selectStateChanged(bool selectedState);
    void signal_coloredSquare_mouseReleased();

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
    void setBorderThickness(double dBorderThickness);
    void computeBoundingBox();

private:
    QPen _pUnselected;
    QPen _pSelected;

    QBrush _bUnselected;
    QBrush _bSelected;

    qreal _x;
    qreal _y;
    double _handlerWidth;
    double _handlerHeight;
    double _dBorderThickness;

    bool _bCanBeMouseMoved;
    bool _bFilledAlsoWhenUnselected;

    bool _bLocationInAdjustement;

    QPen  * _ptrPenInUse;
    QBrush* _ptrBrushInUse;

    bool _bSelectedState; //mouse hover the item

    QRectF _qrectfhandler;

    QRectF _qrectfBoundingBox;

    QPointF _offsetMouseClickToPointCenterPos; //stores the offset between the mouse cursor location when clicking for location adjustement and then point center

};


#endif // CustomGraphicsItem_coloredGripSquare_H

