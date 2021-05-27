#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsRectangularHighlightItem.h"

#include "CustomGraphicsItemTypes.h"

#include "math.h"

//this item is used to highligt the area where it's possible to add a point in a segment
CustomGraphicsRectangularHighlightItem::CustomGraphicsRectangularHighlightItem(
   const QVector<QPointF>& qvectqpf_boxCornersNoZLI,
   QGraphicsItem *parent):
   CustomGraphicsRectangularHighlightItem(Qt::green, Qt::yellow, parent) {

    //@LP: remember: points in counterclockwise for the QpolygonF give better result for enter and leave hoverEvent
    _qvectqpf_boxCornersNoZLI = qvectqpf_boxCornersNoZLI;
    adjustLocationAndSizeToZLI(1);
}

CustomGraphicsRectangularHighlightItem::CustomGraphicsRectangularHighlightItem(
                                        Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
                                        QGraphicsItem *parent): /*QGraphicsItem*/ QGraphicsPolygonItem(parent),
                                        _bHover {false} {

    _pForUnselectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselectedPixel.setColor(colorForUnselected);
    _pForUnselectedPixel.setCosmetic(true);

    _pForSelectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForSelectedPixel.setColor(colorForSelected);
    _pForSelectedPixel.setCosmetic(true);

    setSelected(false);
    
    _qvectqpf_boxCornersZLI.resize(4); 

    setAcceptHoverEvents(true);

    setVisible(true);

    setPen(*_PtrUsedPen);

    adjustLocationAndSizeToZLI(1);

}

void CustomGraphicsRectangularHighlightItem::adjustLocationAndSizeToZLI(int ZLI) {

    //qDebug() << __FUNCTION__;

    _currentZLI = ZLI;

    prepareGeometryChange(); //@#LP best location ?

     _polygonF.clear();
    int idxqpf = 0;
       
    for(auto& iterqpf : _qvectqpf_boxCornersNoZLI) {
        //qDebug() << __FUNCTION__ << "looping on _qvectqpf_boxCornersNoZLI";
        _qvectqpf_boxCornersZLI[idxqpf] = iterqpf / static_cast<qreal>(_currentZLI);
        _polygonF << _qvectqpf_boxCornersZLI[idxqpf];
        idxqpf++;
    }
    setPolygon(_polygonF);
    
}

CustomGraphicsRectangularHighlightItem::~CustomGraphicsRectangularHighlightItem() {
    //qDebug() << __FUNCTION__;
}

int CustomGraphicsRectangularHighlightItem::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return( e_customGraphicsItemType::e_cgit_CustomGraphicsRectangularHighlightItem);
}

void CustomGraphicsRectangularHighlightItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    setSelected(true);
    setPen(*_PtrUsedPen);
    event->accept();
}

void CustomGraphicsRectangularHighlightItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    setSelected(false);
    setPen(*_PtrUsedPen);   
    event->accept();       
}

void CustomGraphicsRectangularHighlightItem::setSelected(bool bNewState) {
    _bHover = bNewState;
    if (_bHover) {
        _PtrUsedPen = &_pForSelectedPixel;
    } else {
        _PtrUsedPen = &_pForUnselectedPixel;
    }    
}


