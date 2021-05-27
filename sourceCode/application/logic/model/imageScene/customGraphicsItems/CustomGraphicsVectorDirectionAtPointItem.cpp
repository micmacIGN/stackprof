#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsVectorDirectionAtPointItem.h"

#include "CustomGraphicsItemTypes.h"

#include "math.h"

CustomGraphicsVectorDirectionAtPointItem::CustomGraphicsVectorDirectionAtPointItem(
        int idPoint, QPointF startingPointLocationNoZLI, QPointF unitvectorDirectionNoZLI, QGraphicsItem *parent):
    CustomGraphicsVectorDirectionAtPointItem( idPoint, startingPointLocationNoZLI, unitvectorDirectionNoZLI,
        Qt::yellow, Qt::red, parent) {

}


CustomGraphicsVectorDirectionAtPointItem::CustomGraphicsVectorDirectionAtPointItem(int idPoint,
                                        QPointF startingPointLocationNoZLI,
                                        QPointF unitvectorDirectionNoZLI,
                                        Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
                                        QGraphicsItem *parent):
    QGraphicsObject(parent),
    _id(idPoint),
    _currentZLI(-1),
    _qpf_startingPointLocationNoZLI {startingPointLocationNoZLI},
    _qpf_unitvectorDirectionNoZLI {unitvectorDirectionNoZLI},

    _qrectf_boundingBox {.0,.0,.0,.0},
    _bHover {false} {

    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_startingPointLocationNoZLI =" << _qpf_startingPointLocationNoZLI;
    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_unitvectorDirectionNoZLI =" << _qpf_unitvectorDirectionNoZLI;

    _qpf_endingPointLocationNoZLI = { _qpf_startingPointLocationNoZLI.x() + _qpf_unitvectorDirectionNoZLI.x(),
                                      _qpf_startingPointLocationNoZLI.y() + _qpf_unitvectorDirectionNoZLI.y()};

    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_endingPointLocationNoZLI =" << _qpf_endingPointLocationNoZLI;

    _pForUnselectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselectedPixel.setColor(colorForUnselected);
    _pForUnselectedPixel.setCosmetic(true);

    _pForSelectedPixel.setStyle(Qt::PenStyle::SolidLine);
    _pForSelectedPixel.setColor(colorForSelected);
    _pForSelectedPixel.setCosmetic(true);

    setSelected(false);

    //Compute points location to draw arrow segments:
    //There are computed using lines from point A, distance from point A and angle around the vector direction
    double cosAlphaUnitVect = _qpf_unitvectorDirectionNoZLI.x();
    double gradToDegMulFactor = 180.0/M_PI;
    double alphaUnitVectDegree = gradToDegMulFactor*acos(cosAlphaUnitVect);
    //qDebug() << __FUNCTION__ << "vectorGraph " << "cosAlphaUnitVect = " << cosAlphaUnitVect;
    //qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegree = " << alphaUnitVectDegree;

    double sinAlphaUnitVect = _qpf_unitvectorDirectionNoZLI.y();
    double mulSigny = (sinAlphaUnitVect<0.0)?(-1.0):(+1.0);

    double alphaUnitVectDegreeMinus30 = alphaUnitVectDegree-15.0;
    double alphaUnitVectDegreePlus30 = alphaUnitVectDegree+15.0;

    //qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegreeMinus30 = " << alphaUnitVectDegreeMinus30;
    //qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegreePlus30 = " << alphaUnitVectDegreePlus30;

    qreal lengthToLocateArrowSegmentsFromStartingPointNoZLI = 0.60;

    //qDebug() << __FUNCTION__ << "vectorGraph " << "lengthToLocateArrowSegmentsromStartingPointNoZLI = " << lengthToLocateArrowSegmentsFromStartingPointNoZLI;

    double degToGradMulFactor = M_PI/180.0;
    _arrowExtremitySide1NoZLI = {
        _qpf_startingPointLocationNoZLI.x() + lengthToLocateArrowSegmentsFromStartingPointNoZLI * cos(degToGradMulFactor*alphaUnitVectDegreeMinus30),
        _qpf_startingPointLocationNoZLI.y() + mulSigny * lengthToLocateArrowSegmentsFromStartingPointNoZLI * sin(degToGradMulFactor*alphaUnitVectDegreeMinus30) };

    //qDebug() << __FUNCTION__ << "vectorGraph " << " lengthToLocateArrowSegmentsFromStartingPointNoZLI =" << lengthToLocateArrowSegmentsFromStartingPointNoZLI;
    //qDebug() << __FUNCTION__ << "vectorGraph " << " degToGradMulFactor*alphaUnitVectDegreeMinus30 =" << degToGradMulFactor*alphaUnitVectDegreeMinus30;

    //qDebug() << __FUNCTION__ << "vectorGraph " << " cos(degToGradMulFactor*alphaUnitVectDegreeMinus30) =" << cos(degToGradMulFactor*alphaUnitVectDegreeMinus30);
    //qDebug() << __FUNCTION__ << "vectorGraph " << " sin(degToGradMulFactor*alphaUnitVectDegreeMinus30) =" << sin(degToGradMulFactor*alphaUnitVectDegreeMinus30);

    _arrowExtremitySide2NoZLI = {
        _qpf_startingPointLocationNoZLI.x() + lengthToLocateArrowSegmentsFromStartingPointNoZLI * cos(degToGradMulFactor*alphaUnitVectDegreePlus30),
        _qpf_startingPointLocationNoZLI.y() + mulSigny * lengthToLocateArrowSegmentsFromStartingPointNoZLI * sin(degToGradMulFactor*alphaUnitVectDegreePlus30) };


    //-------
    // perpendicularDirectionsAtPoint
    //avoiding to use IMath Vec2 in pure Qt widget
    QPointF qpf_vecNormalized_perpendicularToSegmentCounterclockwise { -_qpf_unitvectorDirectionNoZLI.y() , _qpf_unitvectorDirectionNoZLI.x() };
    QPointF qpf_vecNormalized_perpendicularToSegmentClockwise = -qpf_vecNormalized_perpendicularToSegmentCounterclockwise;

    qreal lengthForOnePerpendicularSegmentNoZLI = 1.5;

    _perpendicularExtermitySegment1NoZLI = _qpf_startingPointLocationNoZLI + lengthForOnePerpendicularSegmentNoZLI*qpf_vecNormalized_perpendicularToSegmentCounterclockwise;
    _perpendicularExtermitySegment2NoZLI = _qpf_startingPointLocationNoZLI + lengthForOnePerpendicularSegmentNoZLI*qpf_vecNormalized_perpendicularToSegmentClockwise;

    _bShowPerpendicularDirectionsAtPoint = true;
    //------

    QVector<qreal> qvect_xMinMaxCandidates {
        _qpf_startingPointLocationNoZLI.x(),
        _qpf_endingPointLocationNoZLI.x(),
        _arrowExtremitySide1NoZLI.x(),
        _arrowExtremitySide2NoZLI.x()
    };

    QVector<qreal> qvect_yMinMaxCandidates {
        _qpf_startingPointLocationNoZLI.y(),
        _qpf_endingPointLocationNoZLI.y(),
        _arrowExtremitySide1NoZLI.y(),
        _arrowExtremitySide2NoZLI.y()
    };

    if (_bShowPerpendicularDirectionsAtPoint) {
        qvect_xMinMaxCandidates.push_back(_perpendicularExtermitySegment1NoZLI.x());
        qvect_xMinMaxCandidates.push_back(_perpendicularExtermitySegment2NoZLI.x());
        qvect_yMinMaxCandidates.push_back(_perpendicularExtermitySegment1NoZLI.y());
        qvect_yMinMaxCandidates.push_back(_perpendicularExtermitySegment2NoZLI.y());
   }

    _qrectf_boundingBoxNoZLI.setTop   (*std::min_element(qvect_yMinMaxCandidates.constBegin(), qvect_yMinMaxCandidates.constEnd()));
    _qrectf_boundingBoxNoZLI.setBottom(*std::max_element(qvect_yMinMaxCandidates.constBegin(), qvect_yMinMaxCandidates.constEnd()));

    _qrectf_boundingBoxNoZLI.setLeft (*std::min_element(qvect_xMinMaxCandidates.constBegin(), qvect_xMinMaxCandidates.constEnd()));
    _qrectf_boundingBoxNoZLI.setRight(*std::max_element(qvect_xMinMaxCandidates.constBegin(), qvect_xMinMaxCandidates.constEnd()));

    adjustLocationAndSizeToZLI(1);

    setAcceptHoverEvents(true);

    setVisible(true);
}

void CustomGraphicsVectorDirectionAtPointItem::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsVectorDirectionAtPointItem) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    prepareGeometryChange();



    _qrectf_boundingBox.setTop   (_qrectf_boundingBoxNoZLI.top()   /static_cast<qreal>(_currentZLI));
    _qrectf_boundingBox.setLeft  (_qrectf_boundingBoxNoZLI.left()  /static_cast<qreal>(_currentZLI));
    _qrectf_boundingBox.setBottom(_qrectf_boundingBoxNoZLI.bottom()/static_cast<qreal>(_currentZLI));
    _qrectf_boundingBox.setRight (_qrectf_boundingBoxNoZLI.right() /static_cast<qreal>(_currentZLI));

    //qDebug() << __FUNCTION__ << "_qrectf_boundingBoxNoZLI   = " << _qrectf_boundingBoxNoZLI;
    //qDebug() << __FUNCTION__ << "_qrectf_boundingBox = " << _qrectf_boundingBox;
}

QRectF CustomGraphicsVectorDirectionAtPointItem::boundingRect() const {
    //qDebug() << __FUNCTION__;
    return(_qrectf_boundingBox);
}

void CustomGraphicsVectorDirectionAtPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->save();

    painter->setPen(*_PtrUsedPen);

    QPointF _qpf_startingPointLocationZLI = _qpf_startingPointLocationNoZLI/static_cast<qreal>(_currentZLI);
    QPointF _qpf_endingPointLocationZLI = _qpf_endingPointLocationNoZLI/static_cast<qreal>(_currentZLI);

    //qDebug() << __FUNCTION__ << "_qpf_startingPointLocationZLI   = " << _qpf_startingPointLocationZLI;
    //qDebug() << __FUNCTION__ << "_qpf_endingPointLocationZLI   = " << _qpf_endingPointLocationZLI;

    painter->drawLine(_qpf_startingPointLocationZLI, _qpf_endingPointLocationZLI);


    QPointF _arrowExtremitySide1ZLI = _arrowExtremitySide1NoZLI/static_cast<qreal>(_currentZLI);
    QPointF _arrowExtremitySide2ZLI = _arrowExtremitySide2NoZLI/static_cast<qreal>(_currentZLI);

    //qDebug() << __FUNCTION__ << "_arrowExtremitySide1ZLI   = " << _arrowExtremitySide1ZLI;
    //qDebug() << __FUNCTION__ << "_arrowExtremitySide2ZLI   = " << _arrowExtremitySide2ZLI;

    painter->drawLine(_qpf_endingPointLocationZLI, _arrowExtremitySide1ZLI);
    painter->drawLine(_qpf_endingPointLocationZLI, _arrowExtremitySide2ZLI);

    //--
    if (_bShowPerpendicularDirectionsAtPoint) {
        QPointF _perpendicularExtremitySegment1ZLI = _perpendicularExtermitySegment1NoZLI/static_cast<qreal>(_currentZLI);
        QPointF _perpendicularExtremitySegment2ZLI = _perpendicularExtermitySegment2NoZLI/static_cast<qreal>(_currentZLI);

        QPen _pForperpendicularLine;
        _pForperpendicularLine.setStyle(Qt::PenStyle::SolidLine);
        _pForperpendicularLine.setColor(Qt::blue);
        _pForperpendicularLine.setCosmetic(true);

        painter->setPen(_pForperpendicularLine);

        painter->drawLine(
            _perpendicularExtremitySegment1ZLI,
            _perpendicularExtremitySegment2ZLI
        );
    }
    //---

    painter->restore();

}

int CustomGraphicsVectorDirectionAtPointItem::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsVectorDirectionAtPointItem);
}

CustomGraphicsVectorDirectionAtPointItem::~CustomGraphicsVectorDirectionAtPointItem() {
    //qDebug() << __FUNCTION__;
}

void CustomGraphicsVectorDirectionAtPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    qDebug() << __FUNCTION__;
    setSelected(true);
    event->accept();
}

void CustomGraphicsVectorDirectionAtPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    qDebug() << __FUNCTION__;
    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
    setSelected(false);
    //}
    event->accept();
}

void CustomGraphicsVectorDirectionAtPointItem::setSelected(bool bNewState) {
    _bHover = bNewState;
    if (_bHover) {
        _PtrUsedPen = &_pForSelectedPixel;
    } else {
        _PtrUsedPen = &_pForUnselectedPixel;
    }
    update();
}
