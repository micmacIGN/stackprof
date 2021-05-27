#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsTinyTargetedPointItem.h"

#include "CustomGraphicsItemTypes.h"

CustomGraphicsTinyTargetPointItem::CustomGraphicsTinyTargetPointItem(
        int idPoint,
        QPointF location,
        qreal sideSizeNoZLI,
        Qt::GlobalColor colorForUnselected,
        Qt::GlobalColor colorForSelected,
        QGraphicsItem *parent):
    QGraphicsObject(parent),

    _id(idPoint),
    _currentZLI(-1),
    _qpointf_locationNoZLI(location),
    _qsizef_sideSizeNoZLI {sideSizeNoZLI, sideSizeNoZLI},
    _qrectf_locationAndSize {.0,.0,.0,.0},
    _qrectf_boundingBox {.0,.0,.0,.0},
    _bHover {false}

{

    _pForUnselectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselectedPixel.setColor(colorForUnselected);
    _pForUnselectedPixel.setCosmetic(true);

    _pForSelectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForSelectedPixel.setColor(colorForSelected);
    _pForSelectedPixel.setCosmetic(true);

    setSelected(false);

    adjustLocationAndSizeToZLI(1);

    //qDebug() << __FUNCTION__;

    setAcceptHoverEvents(true);

    setVisible(true);

}

CustomGraphicsTinyTargetPointItem::CustomGraphicsTinyTargetPointItem(int idPoint, QPointF location, qreal sideSizeNoZLI, QGraphicsItem *parent):
    CustomGraphicsTinyTargetPointItem(idPoint, location, sideSizeNoZLI, Qt::green, Qt::red, parent) {

}

void CustomGraphicsTinyTargetPointItem::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsTinyTargetPointItem) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    QPointF qpointf_location = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);
    QSizeF qsizef_sideSize   = _qsizef_sideSizeNoZLI/static_cast<qreal>(_currentZLI);

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

    qreal penWidthDiv2 = .0; //1.0/2.0;

    _qrectf_boundingBox.setTop   (_qrectf_locationAndSize.top ()  -penWidthDiv2);
    _qrectf_boundingBox.setLeft  (_qrectf_locationAndSize.left()  -penWidthDiv2);
    _qrectf_boundingBox.setBottom(_qrectf_locationAndSize.bottom()+penWidthDiv2);
    _qrectf_boundingBox.setRight (_qrectf_locationAndSize.right() +penWidthDiv2);

    //qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI = " << _qpointf_locationNoZLI;
    //qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI   = " << _qsizef_sideSizeNoZLI;

    //qDebug() << __FUNCTION__ << "qpointf_location = " << qpointf_location;
    //qDebug() << __FUNCTION__ << "qsizef_sideSize   = " << qsizef_sideSize;

}

QRectF CustomGraphicsTinyTargetPointItem::boundingRect() const {
    //qDebug() << __FUNCTION__;
    return(_qrectf_boundingBox);
}

void CustomGraphicsTinyTargetPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();

    painter->setPen(*_PtrUsedPen);

    QPointF qpointf_location = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);

    painter->drawLine(QPointF(_qrectf_boundingBox.left  (), qpointf_location.y()),
                      QPointF(_qrectf_boundingBox.right (), qpointf_location.y()));
    painter->drawLine(QPointF(qpointf_location.x(), _qrectf_boundingBox.top   ()),
                      QPointF(qpointf_location.x(), _qrectf_boundingBox.bottom()));

    painter->restore();

}

int CustomGraphicsTinyTargetPointItem::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsTinyTargetPointItem);
}

CustomGraphicsTinyTargetPointItem::~CustomGraphicsTinyTargetPointItem() {
    //qDebug() << __FUNCTION__;
}

void CustomGraphicsTinyTargetPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    //qDebug() << __FUNCTION__;
    setSelected(true);
    event->accept();
}

void CustomGraphicsTinyTargetPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    //qDebug() << __FUNCTION__;
    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
    setSelected(false);
    //}
    event->accept();
}

void CustomGraphicsTinyTargetPointItem::setSelected(bool bNewState) {
    _bHover = bNewState;
    if (_bHover) {
        _PtrUsedPen = &_pForSelectedPixel;
    } else {
        _PtrUsedPen = &_pForUnselectedPixel;
    }
    update();
}
