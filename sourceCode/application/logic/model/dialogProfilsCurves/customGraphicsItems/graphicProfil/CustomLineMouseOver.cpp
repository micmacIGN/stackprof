#include <QGraphicsItem>

#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "CustomLineMouseOver.h"

#include <QPainter>
#include <QPainterPath>

#include "CustomGraphicsItemType_v2.h"

CustomLineMouseOver::CustomLineMouseOver(QColor QColor_unselected, QColor QColor_selected,
                                         qreal x,
                                         double xrange_minValue, double xrange_maxValue,
                                         qreal y, qreal height,
                                         bool bCanBeMouseMoved,
                                         QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),

    _x(x),
   _xrange_minValue(xrange_minValue),
   _xrange_maxValue(xrange_maxValue),
   _y(y),
   _height(height),
   _bCanBeMouseMoved(bCanBeMouseMoved),
   _bLocationInAdjustement(false),
   _bSelectedState(false),//mouse hover the item
   _pUnselected(QColor_unselected),
   _pSelected(QColor_selected),
   _ptrPenInUse(nullptr),
   _dThickness(1.0),
   _offsetMouseClickToPointCenterPos{.0, .0} {

    setPensStyle();
    setPensWidthF(1.0);

    //--------------

    _ptrPenInUse = &_pUnselected;

    setAcceptHoverEvents(true);

    //prepareGeometryChange(); //@#LP useful here ?
    setXYPos();

    qDebug() << __FUNCTION__ << "(constructor) @ " << (void*)this;

}

void CustomLineMouseOver::set_canBeMouseMoved(bool bState) {
    _bCanBeMouseMoved = bState;
}

void CustomLineMouseOver::setPensStyle() {
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

void CustomLineMouseOver::setPensWidthF(double dWidth) {
    _pUnselected.setWidthF(dWidth);
    _pUnselected.setCosmetic(false);

    _pSelected.setWidthF(dWidth);
    _pSelected.setCosmetic(false);
}

void CustomLineMouseOver::setXYPos() {
    prepareGeometryChange();
    computeBoundingBox();
    setPos(_x, _y);
    qDebug() << __FUNCTION__ << "done: setPos(_x, _y) with _x =" << _x << ", _y = " << _y;
}

void CustomLineMouseOver::setXPos(double x) {
    prepareGeometryChange();
    _x = x;
    computeBoundingBox();
    setX(_x);
    qDebug() << __FUNCTION__ << "done: setX(_x) with _x =" << _x;
}

void CustomLineMouseOver::set_lineHeight(double lineHeight) {
    prepareGeometryChange();
    _height = lineHeight;
    setXYPos(); // this method call computeBoundingBox();
}

void CustomLineMouseOver::setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster) {
    _dThickness = sCT_xAdjuster._dThickness;
    setPensWidthF(_dThickness);
    _pUnselected.setColor(sCT_xAdjuster._qColor);
    prepareGeometryChange();
    computeBoundingBox();
}

void CustomLineMouseOver::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,  QWidget *widget) {

    qDebug() << __FUNCTION__ << "CustomLineMouseOver::paint(...) _height = " << _height;

    painter->setPen(*_ptrPenInUse);

    QLineF qlf(.0, .0, .0,  _height);
    painter->drawLine(qlf);

    //@#LP debug:
    /*
    QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({0,128,0,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectFBBox);
    */

}


void CustomLineMouseOver::computeBoundingBox() {

    //qDebug() << __FUNCTION__;
    qreal dThicknessDiv2 = _dThickness/2.0;

    qreal pixelXSpaceAround = 2.0;
    qreal x_left  = .0 - dThicknessDiv2 - pixelXSpaceAround;
    qreal x_right = .0 + dThicknessDiv2 + pixelXSpaceAround;
    qreal y_top = .0 - dThicknessDiv2;

    //int iThickness = static_cast<int>(_dThickness); //try to improve the asymetric reactive area, adding 1 pixel at right
    //if (iThickness%2) {
    //   x_right += dThicknessDiv2;
    //}

    _qrectFBBox.setTopLeft({x_left, y_top});
    _qrectFBBox.setSize({ x_right - x_left,
                         _height + _dThickness});

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: pixelXSpaceAround = "<< pixelXSpaceAround;
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: _dThickness = "<< _dThickness;
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: dThicknessDiv2 = "<< dThicknessDiv2;
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: x_left = "<< x_left;
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: x_right = "<< x_right;
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: y_top = "<< y_top;

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) dbg: _qrectFBBox = "<< _qrectFBBox;


}

QRectF CustomLineMouseOver::boundingRect() const {
    return(_qrectFBBox);
}

CustomLineMouseOver::~CustomLineMouseOver() {
    qDebug() << __FUNCTION__  << "@ " << (void*)this;
}

int CustomLineMouseOver::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType_part2::e_cgit_CustomLineMouseOver);
}


void CustomLineMouseOver::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    qDebug() << __FUNCTION__;
    _bLocationInAdjustement = true;

    QPointF qpointf_location {_x, _y};
    _offsetMouseClickToPointCenterPos = (event->scenePos()) - qpointf_location;
    qDebug() << __FUNCTION__ << "offsetMouseClickToPointCenter = " << _offsetMouseClickToPointCenterPos;

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) emit now signal_locationInAdjustement(true)";
    emit signal_locationInAdjustement(true); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)
}

void CustomLineMouseOver::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }

    _bLocationInAdjustement = false;

    //@LP be sure to call signal_locationInAdjustement before signal_coloredSquare_mouseReleased (this CustomLineMouseOver instance will be deleted when the doubleadjuster will be 'deleteLater()'
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) emit now signal_locationInAdjustement(false)";
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) @ " << (void*)this;
    emit signal_locationInAdjustement(false); //to inform the window containing all (the vertical layout and all inside), that that switching L/R vs Center can now be accepted instead of rejected)

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver)" << "will now emit signal_customLineMouseOver_mouseReleased();";
    emit signal_customLineMouseOver_mouseReleased();
}

void CustomLineMouseOver::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    qDebug() << __FUNCTION__;
    if (_bLocationInAdjustement) {
        QPointF qpscenePos = event->scenePos();

        qpscenePos -= _offsetMouseClickToPointCenterPos;

        qDebug() << __FUNCTION__ << qpscenePos.x();

        qreal newxlocation = qpscenePos.x();

        prepareGeometryChange();
        _x = newxlocation;
        computeBoundingBox();
        setPos(_x, _y);
        emit signal_customLineMouseOver_movedTo(_x);
    }
}

void CustomLineMouseOver::setSelected(bool bNewState) {

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) received bNewState = " << bNewState;

    if (bNewState == _bSelectedState) {
        qDebug() << __FUNCTION__ << "(CustomLineMouseOver) if (bNewState == _bSelectedState) return!";
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) use received bNewState ( " << bNewState << " )";

    _bSelectedState = bNewState;
    if (_bSelectedState) {
        _ptrPenInUse = &_pSelected;
    } else {
        _ptrPenInUse = &_pUnselected;
    }

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) emit now signal_selectStateChanged(bNewState)";
    emit signal_selectStateChanged(_bSelectedState);
}

void CustomLineMouseOver::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    qDebug() << __FUNCTION__ << "CustomGraphicsItem_coloredGripSquare::hoverEnterEvent _bCanBeMouseMoved = " << _bCanBeMouseMoved;
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::hoverEnterEvent(event);

        return;
    }
    qDebug() << "CustomLineMouseOver::hoverEnterEvent";
    setSelected(true);

}

void CustomLineMouseOver::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (!_bCanBeMouseMoved) {       
        QGraphicsItem::hoverLeaveEvent(event);
        return;
    }
    qDebug() << "CustomLineMouseOver::hoverLeaveEvent";
    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
        setSelected(false);
    //}
}

void CustomLineMouseOver::slot_selectStateChangedFromOutside(bool selectedState) {
    qDebug() << "CustomLineMouseOver::slot_selectStateChangedFromOutside";
    setSelected(selectedState);
}
