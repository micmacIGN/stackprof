#include <QGraphicsItem>

#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "CustomGraphicsItemType_v2.h"

#include "CustomGraphicsItem_coloredGripSquare.h"

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

CustomGraphicsItem_coloredGripSquare::CustomGraphicsItem_coloredGripSquare(QColor QColor_unselected, QColor QColor_selected,
                                                                           double handlerWidth, double handlerHeight,
                                                                           bool bCanBeMouseMoved,
                                                                           bool bFilledAlsoWhenUnselected,
                                                                           QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),
    _pUnselected(QColor_unselected),
    _pSelected(QColor_selected),
    _bUnselected(QColor_unselected, Qt::NoBrush),
    _bSelected(QColor_selected, Qt::SolidPattern),


    _x(0.0),
    _y(0.0),
    _handlerWidth(handlerWidth),
    _handlerHeight(handlerHeight),
    _dBorderThickness(1.0),
    _bCanBeMouseMoved(bCanBeMouseMoved),
    _bFilledAlsoWhenUnselected(bFilledAlsoWhenUnselected),
    _bLocationInAdjustement(false),
    _ptrPenInUse(nullptr),
    _ptrBrushInUse(nullptr),
    _bSelectedState(false)
{

    _qrectfhandler.setTopLeft({.0 , .0});
    _qrectfhandler.setSize({_handlerWidth, _handlerHeight});

    if (_bFilledAlsoWhenUnselected) {
        _bUnselected.setStyle(Qt::SolidPattern);
    }

    _ptrPenInUse    = &_pUnselected;
    _ptrBrushInUse  = &_bUnselected;
    _bSelectedState = false;

    setAcceptHoverEvents(true);

    setPos(_x, _y);

    setPensStyle();
    setBorderThickness(1.0);

    _offsetMouseClickToPointCenterPos = {.0, .0};

    qDebug() << __FUNCTION__ << "_x = " << _x;
    qDebug() << __FUNCTION__ << "(constructor) @ " << (void*)this;

}


void CustomGraphicsItem_coloredGripSquare::set_canBeMouseMoved(bool bState) {
    _bCanBeMouseMoved = bState;
}


void CustomGraphicsItem_coloredGripSquare::setHandlerPositionAndSize(double x, double y, double handlerWidth, double handlerHeight) {
    prepareGeometryChange();
    _x = x;
    _y = y;
    _handlerWidth = handlerWidth;
    _handlerHeight = handlerHeight;

    _qrectfhandler.setSize({_handlerWidth, _handlerHeight});

    computeBoundingBox();

    setPos(_x, _y);
}

void CustomGraphicsItem_coloredGripSquare::setXPos(double x) {
    setHandlerPositionAndSize(x, _y, _handlerWidth, _handlerHeight);
}

void CustomGraphicsItem_coloredGripSquare::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(*_ptrPenInUse);
    painter->setBrush(*_ptrBrushInUse);
    painter->drawRect(_qrectfhandler);

    //@#LP debug:
    /*
    QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,128,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectfBoundingBox);
    */

}

QRectF CustomGraphicsItem_coloredGripSquare::boundingRect() const {
    return(_qrectfBoundingBox);
}


void CustomGraphicsItem_coloredGripSquare::setSelected(bool bNewState) {

    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) received bNewState = " << bNewState;

    if (bNewState == _bSelectedState) {
        qDebug() << __FUNCTION__ << "(CustomLineMouseOver) if (bNewState == _bSelectedState) return!";
        return;
    }
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) use received bNewState ( " << bNewState << " )";

    _bSelectedState = bNewState;
    if (_bSelectedState) {
        _ptrPenInUse = &_pSelected;
        _ptrBrushInUse = &_bSelected;
    } else {
        _ptrPenInUse = &_pUnselected;
        _ptrBrushInUse = &_bUnselected;
    }
    qDebug() << __FUNCTION__ << "(CustomLineMouseOver) emit now signal_selectStateChanged(_bSelectedState)";

    emit signal_selectStateChanged(_bSelectedState);
}


CustomGraphicsItem_coloredGripSquare::~CustomGraphicsItem_coloredGripSquare() {
    qDebug() << __FUNCTION__  << "@ " << (void*)this;
}

int CustomGraphicsItem_coloredGripSquare::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType_part2::e_cgit_coloredSquareMouseOver);
}


void CustomGraphicsItem_coloredGripSquare::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }
    qDebug() << __FUNCTION__;
   _bLocationInAdjustement = true;

   QPointF qpointf_location {_x, _y};
   _offsetMouseClickToPointCenterPos = (event->scenePos()) - qpointf_location;
   qDebug() << __FUNCTION__ << "offsetMouseClickToPointCenter = " << _offsetMouseClickToPointCenterPos;

   qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare) emit now signal_locationInAdjustement(true)";
   emit signal_locationInAdjustement(true); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)
}

void CustomGraphicsItem_coloredGripSquare::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }
    _bLocationInAdjustement = false;

    //@LP be sure to call signal_locationInAdjustement before signal_coloredSquare_mouseReleased (this CustomGraphicsItem_coloredGripSquare instance will be deleted when the doubleadjuster will be 'deleteLater()'
    qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare) emit now signal_locationInAdjustement(false)";
    qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare) @ " << (void*)this;
    emit signal_locationInAdjustement(false); //to inform the window containing all (the vertical layout and all inside), //to inform the window containing all (the vertical layout and all inside), that switching L/R vs Center can now be accepted instead of rejected)

    qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare)" << " will emit now signal_coloredSquare_mouseReleased()";
    emit signal_coloredSquare_mouseReleased();
}



void CustomGraphicsItem_coloredGripSquare::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    qDebug() << __FUNCTION__;

    //try taking into account the offset between the mouse cursor and the grip square center to avoid to "move" the square when clicking on it
    if (_bLocationInAdjustement) {

        QPointF qpscenePos = event->scenePos();

        qpscenePos -= _offsetMouseClickToPointCenterPos;

        qDebug() << __FUNCTION__ << qpscenePos.x();
        qreal newxlocation = qpscenePos.x();

        prepareGeometryChange();

        _x = newxlocation;

        computeBoundingBox();

        setPos(_x, _y);
        emit signal_coloredSquare_movedTo(_x);
    }
}

void CustomGraphicsItem_coloredGripSquare::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {

    if (!_bCanBeMouseMoved) {
        QGraphicsItem::hoverEnterEvent(event);
        return;
    }
    qDebug() << "CustomGraphicsItem_coloredGripSquare::hoverEnterEvent";
    setSelected(true);
}

void CustomGraphicsItem_coloredGripSquare::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (!_bCanBeMouseMoved) {
        QGraphicsItem::hoverLeaveEvent(event);
        return;
    }
    qDebug() << __FUNCTION__ << "CustomGraphicsItem_coloredGripSquare::hoverLeaveEvent";

    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
        setSelected(false);
    //}
}

void CustomGraphicsItem_coloredGripSquare::slot_selectStateChangedFromOutside(bool selectedState) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare) received selectedState = " << selectedState;
    qDebug() << __FUNCTION__ << "(CustomGraphicsItem_coloredGripSquare) call now setSelected(selectedState)";

    setSelected(selectedState);
}

void CustomGraphicsItem_coloredGripSquare::setPensStyle() {

    _pUnselected.setCapStyle(Qt::FlatCap);
    _pUnselected.setJoinStyle(Qt::MiterJoin);

    _pSelected.setCapStyle(Qt::FlatCap);
    _pSelected.setJoinStyle(Qt::MiterJoin);
}

void CustomGraphicsItem_coloredGripSquare::setBorderThickness(double dBorderThickness) {

    _dBorderThickness = dBorderThickness;

    _pUnselected.setCosmetic(false);
    _pUnselected.setWidthF(_dBorderThickness);

    _pSelected.setCosmetic(false);
    _pSelected.setWidthF(_dBorderThickness);

    prepareGeometryChange();
    if (_dBorderThickness <= 1.0) {
        _qrectfhandler.setTopLeft({.0, .0});
    } else {
        _qrectfhandler.setTopLeft({.0, _dBorderThickness/2.0});
    }

    computeBoundingBox();

    setPos(_x, _y);
}

void CustomGraphicsItem_coloredGripSquare::computeBoundingBox() {

    _qrectfBoundingBox.setTopLeft({ .0 - _dBorderThickness, .0});
    _qrectfBoundingBox.setSize({ _handlerWidth  + 2*_dBorderThickness,
                                 _handlerHeight + 2*_dBorderThickness});
}

void CustomGraphicsItem_coloredGripSquare::setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster) {
    _pUnselected.setColor(sCT_xAdjuster._qColor);
    _bUnselected.setColor(sCT_xAdjuster._qColor);
    setBorderThickness(sCT_xAdjuster._dThickness);
}
