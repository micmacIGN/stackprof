#include <QGraphicsItem>

#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

#include "CustomLineNoEvent.h"

#include <QPainter>
#include <QPainterPath>

#include "CustomGraphicsItemType_v2.h"

CustomLineNoEvent::CustomLineNoEvent(QColor QColor_unselected, QColor QColor_selected,
                                         qreal x,
                                         qreal y, qreal height,
                                         QGraphicsItem *parent): QGraphicsItem(parent),
    _x_qcs(x),
   _y(y),
   _lineHeight_qcs(height),
   _pUnselected(QColor_unselected),
   _pSelected(QColor_selected),
   _ptrPenInUse(nullptr),
   _dThickness(1.0)
   {

    _height = _lineHeight_qcs;
    _x = _x_qcs;

    setPensStyle();
    setPensWidthF(1.0);

    //--------------

    _ptrPenInUse = &_pUnselected;

    setAcceptHoverEvents(true);

    //prepareGeometryChange(); //@#LP useful here ?
    setXYPos();

    qDebug() << __FUNCTION__ << "(constructor) @ " << (void*)this;

}


void CustomLineNoEvent::setPensStyle() {
    //custom dash: "----    ----    ----"
    QVector<qreal> dashes;
    qreal space_length = 4;
    qreal line_length  = 4;
    dashes << space_length << line_length;

    _pUnselected.setDashPattern(dashes);
    _pUnselected.setStyle(Qt::CustomDashLine);
    _pUnselected.setCapStyle(Qt::FlatCap);

    _pSelected.setDashPattern(dashes);
    _pSelected.setStyle(Qt::CustomDashLine);
    _pSelected.setCapStyle(Qt::FlatCap);

}

void CustomLineNoEvent::setPensWidthF(double dWidth) {
    _pUnselected.setWidthF(dWidth);
    _pUnselected.setCosmetic(false);

    _pSelected.setWidthF(dWidth);
    _pSelected.setCosmetic(false);
}

void CustomLineNoEvent::setXYPos() {
    prepareGeometryChange();
    computeBoundingBox();
    setPos(_x, _y);
    qDebug() << __FUNCTION__ << "done: setPos(_x, _y) with _x =" << _x << ", _y = " << _y;
}

void CustomLineNoEvent::setXPos(double x) {
    prepareGeometryChange();
    _x = x;
    computeBoundingBox();
    setX(_x);
    qDebug() << __FUNCTION__ << "done: setX(_x) with _x =" << _x;
}

void CustomLineNoEvent::set_lineHeight(double lineHeight) {
    prepareGeometryChange();
    _height = lineHeight;
    setXYPos(); // this method call computeBoundingBox();
}

void CustomLineNoEvent::setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster) {
    _dThickness = sCT_xAdjuster._dThickness;
    setPensWidthF(_dThickness);
    _pUnselected.setColor(sCT_xAdjuster._qColor);
    prepareGeometryChange();
    computeBoundingBox();
}

void CustomLineNoEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,  QWidget *widget) {

    qDebug() << __FUNCTION__ << "CustomLineMouseOver::paint(...) _height = " << _height;

    painter->setPen(*_ptrPenInUse);

    QLineF qlf(.0, .0, .0,  _height);
    painter->drawLine(qlf);

    //qDebug() << __FUNCTION__ << "_height = " <<_height;

    //@#LP debug:
    /*
    QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,128,0,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectFBBox);
    */

}


void CustomLineNoEvent::computeBoundingBox() {

    //qDebug() << __FUNCTION__;
    qreal dThicknessDiv2 = _dThickness/2.0;

    qreal pixelXSpaceAround = 2.0;
    qreal x_left  = .0 - dThicknessDiv2 - pixelXSpaceAround;
    qreal x_right = .0 + dThicknessDiv2 + pixelXSpaceAround;
    qreal y_top = .0 - dThicknessDiv2;

    _qrectFBBox.setTopLeft({x_left, y_top});
    _qrectFBBox.setSize({ x_right - x_left,
                         _height + _dThickness});

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: _qrectFBBox = "<< _qrectFBBox;

}

QRectF CustomLineNoEvent::boundingRect() const {
    return(_qrectFBBox);
}


CustomLineNoEvent::~CustomLineNoEvent() {
    qDebug() << __FUNCTION__  << "@ " << (void*)this;
}

int CustomLineNoEvent::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType_part2::e_cgit_CustomLineNoEvent);
}

void CustomLineNoEvent::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc) {
    _height = converterPixToqsc.qsc_y_to_pixel(_lineHeight_qcs);
    set_lineHeight(_height);
    _x = converterPixToqsc.qsc_x_to_pixel(_x_qcs);
    setXPos(_x);
}

