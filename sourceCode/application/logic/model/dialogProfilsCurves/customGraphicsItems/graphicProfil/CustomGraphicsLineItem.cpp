
#include <QGraphicsItem>

#include <QPen>
#include <QColor>

#include <QDebug>

#include "CustomGraphicsLineItem.h"

#include <QPainter>

#include "CustomGraphicsItemType_v2.h"

CustomGraphicsLineItem::CustomGraphicsLineItem(QGraphicsItem *parent): QGraphicsItem(parent), _qlineF(.0,.0,.0,.0) {
    setPensStyle();
    setGraphicsParameters({Qt::darkGreen, 1.0, false});
    setPensWidthF(1.0);
    computeBoundingBox();
}

void getMinMaxFromTwoValues(qreal v1, qreal v2, qreal &vMin, qreal &vMax) {
    vMin = v1;
    vMax = v1;
    if (v2 < vMin) { vMin = v2; }
    if (v2 > vMax) { vMax = v2; }
}


void CustomGraphicsLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2) {
    qDebug() << __FUNCTION__ <<"(CustomGraphicsLineItem) x1, y1 => x2, y2 : " << x1 << "," << y1 << " => " << x2 << ", " << y2;
    prepareGeometryChange();
    _qlineF = {x1, y1, x2, y2};
    computeBoundingBox();
}

void CustomGraphicsLineItem::computeBoundingBox() {

    qreal xmin = .0, xmax = .0;
    getMinMaxFromTwoValues(_qlineF.x1(), _qlineF.x2(), xmin, xmax);

    qreal ymin = .0, ymax = .0;
    getMinMaxFromTwoValues(_qlineF.y1(), _qlineF.y2(), ymin, ymax);

    //@LP a little bit more large than requiered:
    xmin-=_dThickness;
    ymin-=_dThickness;

    xmax+=_dThickness;
    ymax+=_dThickness;

    _qRectBoundingBox.setTopLeft({xmin, ymin});
    _qRectBoundingBox.setBottomRight({xmax, ymax});

    qDebug() << __FUNCTION__ << "(CustomGraphicsLineItem) _qRectBoundingBox = " << _qRectBoundingBox;
}


void CustomGraphicsLineItem::setPensStyle() {   
    _qPen.setCapStyle(Qt::FlatCap);  
}

void CustomGraphicsLineItem::setPensWidthF(double dWidth) {
    _qPen.setWidthF(dWidth);
    _qPen.setCosmetic(false);
}

void CustomGraphicsLineItem::setGraphicsParameters(const S_Color_Thickness_Antialiasing& sCTA_computedLine) {
    prepareGeometryChange();
    _dThickness = sCTA_computedLine._dThickness;
    setPensWidthF(_dThickness);
    _qPen.setColor(sCTA_computedLine._qColor);
    _bUseAntialising = sCTA_computedLine._bAntialiasing;
    computeBoundingBox();
}

void CustomGraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,  QWidget *widget) {

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, _bUseAntialising);

    painter->setPen(_qPen);
    painter->drawLine(_qlineF);

/*
    //@#LP debug
    QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,0,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qRectBoundingBox);
*/

/*
    QRectF qRectBoundingBoxExtendedToLocateWhen_0_0_0_0;
    qRectBoundingBoxExtendedToLocateWhen_0_0_0_0.setTop(_qRectBoundingBox.top() - 10.);
    qRectBoundingBoxExtendedToLocateWhen_0_0_0_0.setBottom(_qRectBoundingBox.bottom() + 10.);
    qRectBoundingBoxExtendedToLocateWhen_0_0_0_0.setLeft(_qRectBoundingBox.left() - 10.);
    qRectBoundingBoxExtendedToLocateWhen_0_0_0_0.setRight(_qRectBoundingBox.right() + 10.);
    painter->drawRect(qRectBoundingBoxExtendedToLocateWhen_0_0_0_0);
*/

    painter->restore();

}

QRectF CustomGraphicsLineItem::boundingRect() const {
    return(_qRectBoundingBox);
}

CustomGraphicsLineItem::~CustomGraphicsLineItem() {
    qDebug() << __FUNCTION__;
}

int CustomGraphicsLineItem::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsLineItem);
}
