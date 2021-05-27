#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsTinyTargetPointItem_notDragrable2.h"

#include "CustomGraphicsItemTypes.h"

CustomGraphicsTinyTargetPointItem_notDragrable2::CustomGraphicsTinyTargetPointItem_notDragrable2(int idPoint,
                                               QPointF location, qreal sideSizeNoZLI,
                                               Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
                                               bool bVisible, QGraphicsItem *parent):
    QGraphicsObject(parent),
    _id(idPoint),
    _currentZLI(-1),
    _qpointf_locationNoZLI(location),
    _qsizef_sideSizeNoZLI {sideSizeNoZLI, sideSizeNoZLI},
    _qrectf_locationAndSize {.0,.0,.0,.0},
    _qrectf_boundingBoxZLI {.0,.0,.0,.0},
    _bHover {false}
    {

    _widthBorderNoZLI = 1.0;

    _bUseCosmeticPen = false; //true;
    _bUseAntialiasing = false;//!_bUseCosmeticPen;

    _pForUnselectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselectedPixel.setColor(colorForUnselected);
    _pForUnselectedPixel.setCosmetic(_bUseCosmeticPen);
    _pForUnselectedPixel.setWidthF(_widthBorderNoZLI);
    //_pForUnselectedPixel.setJoinStyle(Qt::MiterJoin);

    _pForSelectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForSelectedPixel.setColor(colorForSelected);
    _pForSelectedPixel.setCosmetic(_bUseCosmeticPen);
    _pForSelectedPixel.setWidthF(_widthBorderNoZLI);
    //_pForSelectedPixel.setJoinStyle(Qt::MiterJoin);

    _bVisible = bVisible;
    setSelected(false);

    _scaleOfViewSideForCurrentZLI = 1.0;
    adjustGraphicsItemWidth(_scaleOfViewSideForCurrentZLI);

    adjustLocationAndSizeToZLI(1);

    //qDebug() << __FUNCTION__;

    setAcceptHoverEvents(true);

    setVisible(_bVisible);

}

void CustomGraphicsTinyTargetPointItem_notDragrable2::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsTinyTargetPointItem_notDragrable) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    QPointF qpointf_location = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);
    QSizeF qsizef_sideSize   = _qsizef_sideSizeNoZLI/static_cast<qreal>(_currentZLI);

    qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI = " << _qpointf_locationNoZLI;
    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI   = " << _qsizef_sideSizeNoZLI;

    qDebug() << __FUNCTION__ << "qpointf_location = " << qpointf_location;
    qDebug() << __FUNCTION__ << "qsizef_sideSize   = " << qsizef_sideSize;

    /*
    .-------.
    |       |
    |   +   |
    |       |
    .-------.

    + = center
    side size = width = height of the shape

    */
    qreal widthDiv2 = qsizef_sideSize.width()/2.0;

    _qrectf_locationAndSize.setLeft  (qpointf_location.x()-widthDiv2);
    _qrectf_locationAndSize.setRight (qpointf_location.x()+widthDiv2);

    qreal heightDiv2 = qsizef_sideSize.height()/2.0;
    _qrectf_locationAndSize.setTop   (qpointf_location.y()-heightDiv2);
    _qrectf_locationAndSize.setBottom(qpointf_location.y()+heightDiv2);

    prepareGeometryChange();

    qreal penWidthDiv2 = _widthBorderNoZLI/2.0; //.0; //1.0/2.0;

    _qrectf_boundingBoxZLI.setTop   (_qrectf_locationAndSize.top ()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setLeft  (_qrectf_locationAndSize.left()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setBottom(_qrectf_locationAndSize.bottom()+penWidthDiv2);
    _qrectf_boundingBoxZLI.setRight (_qrectf_locationAndSize.right() +penWidthDiv2);

    qDebug() << __FUNCTION__ << "(CustomGraphicsTinyTargetPointItem_notDragrable2) _qrectf_boundingBoxZLI top left bottom right = " <<
                _qrectf_boundingBoxZLI.top() <<
                _qrectf_boundingBoxZLI.left() <<
                _qrectf_boundingBoxZLI.bottom() <<
                _qrectf_boundingBoxZLI.right();

    qDebug() << __FUNCTION__ << "(CustomGraphicsTinyTargetPointItem_notDragrable2) _qrectf_boundingBoxZLI width height = " <<
                _qrectf_boundingBoxZLI.width() << _qrectf_boundingBoxZLI.height();


}


QRectF CustomGraphicsTinyTargetPointItem_notDragrable2::boundingRect() const {
    //qDebug() << __FUNCTION__;
    return(_qrectf_boundingBoxZLI);
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();

    //---@#LP debug
    /*QBrush brush_debug(Qt::SolidPattern);
    //brush_debug.setColor({0,128,0,128});
    brush_debug.setColor({128,0,0,128});
    painter->setBrush(brush_debug);
    painter->setPen(Qt::PenStyle::NoPen);
    painter->drawRect(static_cast<QRectF>(_qrectf_boundingBoxZLI));
    */
    //---

    painter->setPen(*_PtrUsedPen);

    painter->drawRect(_qrectf_locationAndSize);

    painter->drawEllipse(_qrectf_locationAndSize);

    QPointF qpointf_location = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);

    painter->drawLine(QPointF(_qrectf_locationAndSize.left  (), qpointf_location.y()),
                      QPointF(_qrectf_locationAndSize.right (), qpointf_location.y()));
    painter->drawLine(QPointF(qpointf_location.x(), _qrectf_locationAndSize.top   ()),
                      QPointF(qpointf_location.x(), _qrectf_locationAndSize.bottom()));

    painter->restore();

}

int CustomGraphicsTinyTargetPointItem_notDragrable2::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsTinyTargetPointItem_notDraggable);
}

CustomGraphicsTinyTargetPointItem_notDragrable2::~CustomGraphicsTinyTargetPointItem_notDragrable2() {
    //qDebug() << __FUNCTION__;
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    //qDebug() << __FUNCTION__;
    setSelected(true);
    event->accept();
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    //qDebug() << __FUNCTION__;
    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
    setSelected(false);
    //}
    event->accept();
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::setSelected(bool bNewState) {
    _bHover = bNewState;
    if (_bHover) {
        _PtrUsedPen = &_pForSelectedPixel;
    } else {
        _PtrUsedPen = &_pForUnselectedPixel;
    }
    update();
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::setSize(qreal sideSizeNoZLI) {
    if (_currentZLI == -1) {
        return;
    }
    _qsizef_sideSizeNoZLI = {sideSizeNoZLI, sideSizeNoZLI};
    adjustLocationAndSizeToZLI(_currentZLI);
}

void CustomGraphicsTinyTargetPointItem_notDragrable2::setLocation(QPointF location) {
    _qpointf_locationNoZLI = location;
    adjustLocationAndSizeToZLI(_currentZLI);
}


void CustomGraphicsTinyTargetPointItem_notDragrable2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

    prepareGeometryChange(); //@#LP best location ?

    _scaleOfViewSideForCurrentZLI = scaleOfViewSideForCurrentZLI;

    if (_bUseCosmeticPen) {
        _widthBorderZLI = 1.0;
    } else {
        _widthBorderZLI = _widthBorderNoZLI / _scaleOfViewSideForCurrentZLI;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _scaleOfViewSideForCurrentZLI: " << _scaleOfViewSideForCurrentZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderNoZLI: " << _widthBorderNoZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderZLI => " << _widthBorderZLI;

//#define DEF_LP_debug_trace_file_cgbox
#ifdef DEF_LP_debug_trace_file_cgbox

    QString qstrDbg0 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- start -----\n\r";
    QString qstrDbg1 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _currentZLI: " + QString::number(_currentZLI) + "\n\r";
    QString qstrDbg2 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _scaleOfViewSideForCurrentZLI: " + QString::number(_scaleOfViewSideForCurrentZLI) + "\n\r";
    QString qstrDbg3 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _widthBorderNoZLI: " + QString::number(_widthBorderNoZLI) + "\n\r";
    QString qstrDbg4 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _widthBorderZLI: " + QString::number(_widthBorderZLI) + "\n\r";
    QString qstrDbg5 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- end -----\n\r";


    QFile qfdebug("/tmp/cgbox_log.txt");
    if (qfdebug.open(QIODevice::Append)) {
        qfdebug.write(qstrDbg0.toStdString().c_str());
        qfdebug.write(qstrDbg1.toStdString().c_str());
        qfdebug.write(qstrDbg2.toStdString().c_str());
        qfdebug.write(qstrDbg3.toStdString().c_str());
        qfdebug.write(qstrDbg4.toStdString().c_str());
        qfdebug.write(qstrDbg5.toStdString().c_str());
        qfdebug.close();
    }
#endif

    //bool bUseCosmeticPen = true; //false;

    _pForUnselectedPixel.setWidthF(_widthBorderZLI);
    _pForUnselectedPixel.setCosmetic(_bUseCosmeticPen);

    _pForSelectedPixel.setWidthF(_widthBorderZLI);
    _pForSelectedPixel.setCosmetic(_bUseCosmeticPen);

}
