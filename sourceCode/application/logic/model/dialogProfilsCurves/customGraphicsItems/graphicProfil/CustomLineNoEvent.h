#ifndef TESTCUSTOMLINENOEVENT_H
#define TESTCUSTOMLINENOEVENT_H

#include <QGraphicsLineItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

#include "../../ConverterPixelToQsc.h"

class CustomLineNoEvent:  public QGraphicsItem {

public:
    //@#LP align param type name (double;qreal)
    CustomLineNoEvent( qreal x,
                        qreal y, qreal height,
                        QGraphicsItem *parent=nullptr);

    //@#LP align param type name (double;qreal)
    CustomLineNoEvent(QColor QColor_unselected, QColor QColor_selected,
                        qreal x,
                        qreal y, qreal height,
                        QGraphicsItem *parent=nullptr);

    void setSelected(bool newState);


    //QPainterPath shape() const override ;
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void setXPos(double x);
    void setXYPos();

    void set_lineHeight(double lineHeight);

    void setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster);

    ~CustomLineNoEvent() override;

    int type() const override;

    void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);

protected:

    void setPensStyle();
    void setPensWidthF(double dWidth);

    void computeBoundingBox();

private:
    qreal _x_qcs;
    qreal _x; //pixel

    qreal _y;
    qreal _lineHeight_qcs;
    qreal _height;//pixel

    QPen _pUnselected;
    QPen _pSelected;
    QPen *_ptrPenInUse;

    double _dThickness;

    QRectF _qrectFBBox;

};

#endif // TESTCUSTOMLINENOEVENT_H
