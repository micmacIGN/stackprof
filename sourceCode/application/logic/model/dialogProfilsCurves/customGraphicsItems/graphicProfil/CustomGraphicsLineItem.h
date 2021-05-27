#ifndef TESTCUSTOMGRAPHICSLINEITEM_H
#define TESTCUSTOMGRAPHICSLINEITEM_H

#include <QGraphicsItem>
#include <QPen>

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

//use to display the line about the linear regression model

class CustomGraphicsLineItem: public QGraphicsItem {

public:
    CustomGraphicsLineItem(QGraphicsItem *parent=nullptr);

    //QPainterPath shape() const override ;
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void setGraphicsParameters(const S_Color_Thickness_Antialiasing& sCTA_computedLine);

    ~CustomGraphicsLineItem() override;

    int type() const override;

    void setLine(qreal x1, qreal y1, qreal x2, qreal y2);

private:
    void setPensStyle();
    void setPensWidthF(double dWidth);
    void computeBoundingBox();
private:
    QLineF _qlineF;
    double _dThickness;
    bool _bUseAntialising;

    QPen _qPen;

    QRectF _qRectBoundingBox;
};

#endif // TESTCUSTOMGRAPHICSLINEITEM_H
