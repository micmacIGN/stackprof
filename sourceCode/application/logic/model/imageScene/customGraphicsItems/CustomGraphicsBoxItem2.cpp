#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QPainterPath>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsBoxItem2.h"

#include "CustomGraphicsItemTypes.h"

#include "math.h"


CustomGraphicsBoxItem2::CustomGraphicsBoxItem2(
   const S_ProfilsBoxParameters& profilsBoxParameters, int boxID, qreal devicePixelRatioF, QGraphicsItem *parent):
   CustomGraphicsBoxItem2( profilsBoxParameters,
                          boxID,
                          Qt::green,
                          Qt::yellow,
                          Qt::red,
                          devicePixelRatioF,
                          parent) {  
}

int CustomGraphicsBoxItem2::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsBoxItem2);
}


CustomGraphicsBoxItem2::CustomGraphicsBoxItem2(const S_ProfilsBoxParameters& profilsBoxParameters,
                                        int boxID,
                                        Qt::GlobalColor colorForUnselected,
                                        Qt::GlobalColor colorForSelected,
                                        Qt::GlobalColor colorForHighlight,
                                        qreal devicePixelRatioF,
                                        QGraphicsItem *parent): QGraphicsObject(parent), //// QGraphicsItem(parent) ////QGraphicsPolygonItem(parent) /
_boxID(boxID),
_profilsBoxParametersNoZLI(profilsBoxParameters),
//_qrectf_locationAndSize {.0,.0,.0,.0},
//_qrectf_boundingBox {.0,.0,.0,.0},
_bHover {false},
_bHighlight {false},
_bSelectable {true} {

    //setDevicePixelRatioF(devicePixelRatioF);

    //to have keypressEvent called
    /*setFocus();*/
    /*setFlags(this->flags() | QGraphicsItem::ItemIsFocusable );
    setFlag(QGraphicsItem::ItemIsMovable,false);*/

    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_startingPointLocationNoZLI =" << _qpf_startingPointLocationNoZLI;
    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_unitvectorDirectionNoZLI =" << _qpf_unitvectorDirectionNoZLI;

    //_qpf_endingPointLocationNoZLI = { _qpf_startingPointLocationNoZLI.x() + _qpf_unitvectorDirectionNoZLI.x(),
    //                                  _qpf_startingPointLocationNoZLI.y() + _qpf_unitvectorDirectionNoZLI.y()};

    //qDebug() << __FUNCTION__ << "vectorGraph " << "_qpf_endingPointLocationNoZLI =" << _qpf_endingPointLocationNoZLI;

    _widthBorderNoZLI = 1.0; //1.0;
    /*if (_devicePixelRatioF > 1.0) {
        if (_devicePixelRatioF < 3.0) {
          _widthBorderNoZLI = 0.75;
        }// else {
         //   _widthBorderNoZLI = .. / _devicePixelRatioF; //2.0; //1.5;
        //
     }*/

    _bUseCosmeticPen = false; //true;
    _bUseAntialiasing = !_bUseCosmeticPen;


    _pForUnselected.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselected.setColor(colorForUnselected);
    //_pForUnselected.setColor({0,255,0,128});//@#LP testing bbox location and size
    _pForUnselected.setCosmetic(_bUseCosmeticPen);
    _pForUnselected.setWidthF(_widthBorderNoZLI);
    _pForUnselected.setJoinStyle(Qt::MiterJoin);

    _pForSelected.setStyle(Qt::PenStyle::SolidLine);
    _pForSelected.setColor(colorForSelected);
    _pForSelected.setCosmetic(_bUseCosmeticPen);
    _pForSelected.setWidthF(_widthBorderNoZLI);
    _pForSelected.setJoinStyle(Qt::MiterJoin);

    _pForOverflowOnPrevious.setStyle(Qt::PenStyle::SolidLine);
    _pForOverflowOnPrevious.setColor(Qt::cyan);
    _pForOverflowOnPrevious.setCosmetic(_bUseCosmeticPen);
    _pForOverflowOnPrevious.setWidthF(_widthBorderNoZLI);
    _pForOverflowOnPrevious.setJoinStyle(Qt::MiterJoin);

    _pForFinalAfterAdjustement.setStyle(Qt::PenStyle::SolidLine);
    _pForFinalAfterAdjustement.setColor(Qt::white);
    _pForFinalAfterAdjustement.setCosmetic(_bUseCosmeticPen);
    _pForFinalAfterAdjustement.setWidthF(_widthBorderNoZLI);
    _pForFinalAfterAdjustement.setJoinStyle(Qt::MiterJoin);

    _pForHighlight.setStyle(Qt::PenStyle::SolidLine);
    _pForHighlight.setColor(colorForHighlight);
    _pForHighlight.setCosmetic(_bUseCosmeticPen);
    _pForHighlight.setWidthF(_widthBorderNoZLI);
    _pForHighlight.setJoinStyle(Qt::MiterJoin);

    _pFor_tooCloseBox_rejected_needGoFurther.setStyle(Qt::PenStyle::SolidLine);
    _pFor_tooCloseBox_rejected_needGoFurther.setColor(Qt::red);
    _pFor_tooCloseBox_rejected_needGoFurther.setCosmetic(_bUseCosmeticPen);
    _pFor_tooCloseBox_rejected_needGoFurther.setWidthF(_widthBorderNoZLI);
    _pFor_tooCloseBox_rejected_needGoFurther.setJoinStyle(Qt::MiterJoin);

    _pFor_tooFarBox_rejected_needGoCloser.setStyle(Qt::PenStyle::SolidLine);
    _pFor_tooFarBox_rejected_needGoCloser.setColor(Qt::blue);
    _pFor_tooFarBox_rejected_needGoCloser.setCosmetic(_bUseCosmeticPen);
    _pFor_tooFarBox_rejected_needGoCloser.setWidthF(_widthBorderNoZLI);
    _pFor_tooFarBox_rejected_needGoCloser.setJoinStyle(Qt::MiterJoin);

    setSelected(false);//_PtrUsedPen = &_pForUnselectedPixel;

    //setGraphicsParameters({1.0, false});

/*
    //Compute points location to draw arrow segments:
    //There are computed using lines from point A, distance from point A and angle around the vector direction
    double cosAlphaUnitVect = _qpf_unitvectorDirectionNoZLI.x();
    double gradToDegMulFactor = 180.0/M_PI;
    double alphaUnitVectDegree = gradToDegMulFactor*acos(cosAlphaUnitVect);
    qDebug() << __FUNCTION__ << "vectorGraph " << "cosAlphaUnitVect = " << cosAlphaUnitVect;
    qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegree = " << alphaUnitVectDegree;

    double sinAlphaUnitVect = _qpf_unitvectorDirectionNoZLI.y();
    double mulSigny = (sinAlphaUnitVect<0.0)?(-1.0):(+1.0);

    double alphaUnitVectDegreeMinus30 = alphaUnitVectDegree-15.0;
    double alphaUnitVectDegreePlus30 = alphaUnitVectDegree+15.0;

    qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegreeMinus30 = " << alphaUnitVectDegreeMinus30;
    qDebug() << __FUNCTION__ << "vectorGraph " << "alphaUnitVectDegreePlus30 = " << alphaUnitVectDegreePlus30;

    qreal lengthToLocateArrowSegmentsFromStartingPointNoZLI = 0.60;

    qDebug() << __FUNCTION__ << "vectorGraph " << "lengthToLocateArrowSegmentsromStartingPointNoZLI = " << lengthToLocateArrowSegmentsFromStartingPointNoZLI;

    double degToGradMulFactor = M_PI/180.0;
    _arrowExtremitySide1NoZLI = {
        _qpf_startingPointLocationNoZLI.x() + lengthToLocateArrowSegmentsFromStartingPointNoZLI * cos(degToGradMulFactor*alphaUnitVectDegreeMinus30),
        _qpf_startingPointLocationNoZLI.y() + mulSigny * lengthToLocateArrowSegmentsFromStartingPointNoZLI * sin(degToGradMulFactor*alphaUnitVectDegreeMinus30) };

    qDebug() << __FUNCTION__ << "vectorGraph " << " lengthToLocateArrowSegmentsFromStartingPointNoZLI =" << lengthToLocateArrowSegmentsFromStartingPointNoZLI;
    qDebug() << __FUNCTION__ << "vectorGraph " << " degToGradMulFactor*alphaUnitVectDegreeMinus30 =" << degToGradMulFactor*alphaUnitVectDegreeMinus30;

    qDebug() << __FUNCTION__ << "vectorGraph " << " cos(degToGradMulFactor*alphaUnitVectDegreeMinus30) =" << cos(degToGradMulFactor*alphaUnitVectDegreeMinus30);
    qDebug() << __FUNCTION__ << "vectorGraph " << " sin(degToGradMulFactor*alphaUnitVectDegreeMinus30) =" << sin(degToGradMulFactor*alphaUnitVectDegreeMinus30);

    _arrowExtremitySide2NoZLI = {
        _qpf_startingPointLocationNoZLI.x() + lengthToLocateArrowSegmentsFromStartingPointNoZLI * cos(degToGradMulFactor*alphaUnitVectDegreePlus30),
        _qpf_startingPointLocationNoZLI.y() + mulSigny * lengthToLocateArrowSegmentsFromStartingPointNoZLI * sin(degToGradMulFactor*alphaUnitVectDegreePlus30) };


    //-------
    // perpendicularDirectionsAtPoint
    //avoiding to uss IMath Vec2 in pure Qt widget
    QPointF qpf_vecNormalized_perpendicularToSegmentCounterclockwise { -_qpf_unitvectorDirectionNoZLI.y() , _qpf_unitvectorDirectionNoZLI.x() };
    QPointF qpf_vecNormalized_perpendicularToSegmentClockwise = -qpf_vecNormalized_perpendicularToSegmentCounterclockwise;

    qreal lengthForOnePerpendicularSegmentNoZLI = 1.5;

    _perpendicularExtermitySegment1NoZLI = _qpf_startingPointLocationNoZLI + lengthForOnePerpendicularSegmentNoZLI*qpf_vecNormalized_perpendicularToSegmentCounterclockwise;
    _perpendicularExtermitySegment2NoZLI = _qpf_startingPointLocationNoZLI + lengthForOnePerpendicularSegmentNoZLI*qpf_vecNormalized_perpendicularToSegmentClockwise;

    _bShowPerpendicularDirectionsAtPoint = false; //true;
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
*/

    /*

    *-------*            *-<-.->-*
    |       |            |   |   |
    |       |            |   ^   |
    |       |            |   |   |  find the four corners (*)
    |   +-> |            |   +   |  from the center
    |       |            |   |   |
    |       |            |   v   |
    |       |            |   |   |
    *-------*            *-<-.->-*
    box with
    center point (+)
    and unit vector
    direction (->)

    *---Width---*
    |           |
    L           |
    e           |
    n           |
    g     +     |
    t           |
    h           |
    |           |
    |           |
    *-----------*

    */

    QPointF unitVectorPerpendicularToDirectionCounterClockWise {
        -_profilsBoxParametersNoZLI._unitVectorDirection.y(),
         _profilsBoxParametersNoZLI._unitVectorDirection.x()};

    int lengthDiv2 = static_cast<int>(_profilsBoxParametersNoZLI._oddPixelLength)/2; //remove the odd part
    //box sides center point at length from the center point
    QPointF qpf_pointAtLengthFromCenter_counterClockWideSide {
        _profilsBoxParametersNoZLI._qpfCenterPoint.x() + lengthDiv2 * unitVectorPerpendicularToDirectionCounterClockWise.x(),
        _profilsBoxParametersNoZLI._qpfCenterPoint.y() + lengthDiv2 * unitVectorPerpendicularToDirectionCounterClockWise.y()
    };

    QPointF unitVectorPerpendicularToDirectionClockWise = - unitVectorPerpendicularToDirectionCounterClockWise;

    QPointF qpf_pointAtLengthFromCenter_clockWideSide {
        _profilsBoxParametersNoZLI._qpfCenterPoint.x() + lengthDiv2 * unitVectorPerpendicularToDirectionClockWise.x(),
        _profilsBoxParametersNoZLI._qpfCenterPoint.y() + lengthDiv2 * unitVectorPerpendicularToDirectionClockWise.y()
    };

    QPointF unitVectorToOppositeDirection = - _profilsBoxParametersNoZLI._unitVectorDirection;

    int widthDiv2 = static_cast<int>(_profilsBoxParametersNoZLI._oddPixelWidth)/2; //remove the odd part

    //box corners
    QPointF corner1FromCenterAtLength_counterClockWideSide {
            qpf_pointAtLengthFromCenter_counterClockWideSide.x() + widthDiv2 * unitVectorToOppositeDirection.x(),
            qpf_pointAtLengthFromCenter_counterClockWideSide.y() + widthDiv2 * unitVectorToOppositeDirection.y()
    };

    QPointF corner2FromCenterAtLength_counterClockWideSide {
            qpf_pointAtLengthFromCenter_counterClockWideSide.x() + widthDiv2 * _profilsBoxParametersNoZLI._unitVectorDirection.x(),
            qpf_pointAtLengthFromCenter_counterClockWideSide.y() + widthDiv2 * _profilsBoxParametersNoZLI._unitVectorDirection.y()
    };

    QPointF corner3FromCenterAtLength_clockWideSide {
            qpf_pointAtLengthFromCenter_clockWideSide.x() + widthDiv2 * _profilsBoxParametersNoZLI._unitVectorDirection.x(),
            qpf_pointAtLengthFromCenter_clockWideSide.y() + widthDiv2 * _profilsBoxParametersNoZLI._unitVectorDirection.y()
    };

    QPointF corner4FromCenterAtLength_clockWideSide {
            qpf_pointAtLengthFromCenter_clockWideSide.x() + widthDiv2 * unitVectorToOppositeDirection.x(),
            qpf_pointAtLengthFromCenter_clockWideSide.y() + widthDiv2 * unitVectorToOppositeDirection.y()
    };

    //points in counterclockwise for the QpolygonF give better result for enter and leave hoverEvent
    _qvectqpf_boxCornersNoZLI.push_back(corner4FromCenterAtLength_clockWideSide);
    _qvectqpf_boxCornersNoZLI.push_back(corner3FromCenterAtLength_clockWideSide);
    _qvectqpf_boxCornersNoZLI.push_back(corner2FromCenterAtLength_counterClockWideSide);
    _qvectqpf_boxCornersNoZLI.push_back(corner1FromCenterAtLength_counterClockWideSide);

    QVector<qreal> qvect_xMinMaxCandidates {
        corner1FromCenterAtLength_counterClockWideSide.x(),
        corner2FromCenterAtLength_counterClockWideSide.x(),
        corner3FromCenterAtLength_clockWideSide.x(),
        corner4FromCenterAtLength_clockWideSide.x()
    };
    QVector<qreal> qvect_yMinMaxCandidates {
        corner1FromCenterAtLength_counterClockWideSide.y(),
        corner2FromCenterAtLength_counterClockWideSide.y(),
        corner3FromCenterAtLength_clockWideSide.y(),
        corner4FromCenterAtLength_clockWideSide.y()
    };


    _qvectqpf_boxCornersZLI.fill({.0,.0}, 4);

    _polygonFNoZLI = {_qvectqpf_boxCornersNoZLI};
    _qPainterPath_shapeNoZLI.addPolygon(_polygonFNoZLI);


    //boundingBoxNoZLI:
    qreal yTopNoZLI    = *std::min_element(qvect_yMinMaxCandidates.constBegin(), qvect_yMinMaxCandidates.constEnd());
    qreal yBottomNoZLI = *std::max_element(qvect_yMinMaxCandidates.constBegin(), qvect_yMinMaxCandidates.constEnd());
    qreal xLeftNoZLI   = *std::min_element(qvect_xMinMaxCandidates.constBegin(), qvect_xMinMaxCandidates.constEnd());
    qreal xRightNoZLI  = *std::max_element(qvect_xMinMaxCandidates.constBegin(), qvect_xMinMaxCandidates.constEnd());

//#define def_boxItem2_bboxcomputeMethod1
#ifdef def_boxItem2_bboxcomputeMethod1
    qreal widthBorderNoZLI_div2 = _widthBorderNoZLI*10; /// 2.0;
    _qrectf_boundingBoxNoZLI_noWidthBorder.setTop   (yTopNoZLI    - widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI_noWidthBorder.setBottom(yBottomNoZLI + widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI_noWidthBorder.setLeft  (xLeftNoZLI   - widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI_noWidthBorder.setRight (xRightNoZLI  + widthBorderNoZLI_div2);
#else
    _qrectf_boundingBoxNoZLI_noWidthBorder.setTop   (yTopNoZLI   );
    _qrectf_boundingBoxNoZLI_noWidthBorder.setBottom(yBottomNoZLI);
    _qrectf_boundingBoxNoZLI_noWidthBorder.setLeft  (xLeftNoZLI  );
    _qrectf_boundingBoxNoZLI_noWidthBorder.setRight (xRightNoZLI );
#endif
    setAcceptHoverEvents(true);

    setVisible(true);

    _scaleOfViewSideForCurrentZLI = 1.0;
    adjustGraphicsItemWidth(_scaleOfViewSideForCurrentZLI);

    adjustLocationAndSizeToZLI(1);


}

/*
void CustomGraphicsBoxItem2::setDevicePixelRatioF(qreal devicePixelRatioF) {
    if (devicePixelRatioF < 1.0) { //avoid any futur division by zero
        _devicePixelRatioF = 1.0;
    } else {
        _devicePixelRatioF = devicePixelRatioF;
    }
}*/

QPainterPath CustomGraphicsBoxItem2::shape() const {
    return(_qPainterPath_shapeZLI);
}

QRectF CustomGraphicsBoxItem2::boundingRect() const {
    return(_qrectf_boundingBoxZLI);
}

void CustomGraphicsBoxItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, _bUseAntialiasing);

    //@#LP debug
    /*QBrush brush_debug(Qt::SolidPattern);
    //brush_debug.setColor({0,128,0,128});
    brush_debug.setColor({128,0,0,128});
    painter->setBrush(brush_debug);
    painter->setPen(Qt::PenStyle::NoPen);

    //QRectF qrectf_boundingBoxZLI_shifftedForComp = _qrectf_boundingBoxZLI;
    //qrectf_boundingBoxZLI_shifftedForComp.setTop(qrectf_boundingBoxZLI_shifftedForComp.top()+5000);
    //painter->drawRect(static_cast<QRectF>(qrectf_boundingBoxZLI_shifftedForComp));
    painter->drawRect(static_cast<QRectF>(_qrectf_boundingBoxZLI));

    //painter->drawEllipse(static_cast<QRectF>(_qrectf_boundingBoxZLI));
    */

    //QBrush brush_debug2(Qt::NoBrush);
    //painter->setBrush(brush_debug2);
    painter->setPen(*_PtrUsedPen);

    painter->drawPolygon(_polygonFZLI);

    painter->restore();
}

void CustomGraphicsBoxItem2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

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

    _pForUnselected.setWidthF(_widthBorderZLI);
    _pForUnselected.setCosmetic(_bUseCosmeticPen);

    _pForSelected.setWidthF(_widthBorderZLI);
    _pForSelected.setCosmetic(_bUseCosmeticPen);

    _pForOverflowOnPrevious.setWidthF(_widthBorderZLI);
    _pForOverflowOnPrevious.setCosmetic(_bUseCosmeticPen);

    _pForFinalAfterAdjustement.setWidthF(_widthBorderZLI);
    _pForFinalAfterAdjustement.setCosmetic(_bUseCosmeticPen);

    _pForHighlight.setWidthF(_widthBorderZLI);
    _pForHighlight.setCosmetic(_bUseCosmeticPen);

    _pFor_tooCloseBox_rejected_needGoFurther.setWidthF(_widthBorderZLI);
    _pFor_tooCloseBox_rejected_needGoFurther.setCosmetic(_bUseCosmeticPen);

    _pFor_tooFarBox_rejected_needGoCloser.setWidthF(_widthBorderZLI);
    _pFor_tooFarBox_rejected_needGoCloser.setCosmetic(_bUseCosmeticPen);

}


#include <OpenImageIO/fmath.h> //for floorfrac
using namespace OIIO;

void CustomGraphicsBoxItem2::adjustLocationAndSizeToZLI(int ZLI) {

    if (ZLI == -1) {
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    prepareGeometryChange(); //@#LP best location ?

    /*if (_bUseCosmeticPen) {
        _widthBorderZLI = 1.0;
    } else {
        _widthBorderZLI = _widthBorderNoZLI / _currentZLI;
    }*/
    _widthBorderZLI = _widthBorderNoZLI;
    _scaleOfViewSideForCurrentZLI = 1.0;

    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _currentZLI : " << _currentZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) (not used here _scaleOfViewSideForCurrentZLI : " << _scaleOfViewSideForCurrentZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderNoZLI: " << _widthBorderNoZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderZLI => " << _widthBorderZLI;

#ifdef DEF_LP_debug_trace_file_cgbox
    QString qstrDbg0 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- start -----\n\r";
    QString qstrDbg2 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _currentZLI: " + QString::number(_currentZLI) + "\n\r";
    QString qstrDbg1 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) (not used here _scaleOfViewSideForCurrentZLI :: " + QString::number(_scaleOfViewSideForCurrentZLI) + "\n\r";
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

    _centerPointLocationZLI = _centerPointLocationZLI / static_cast<qreal>(_currentZLI);

    _polygonFZLI.clear();
    int idxqpf = 0;
    //qDebug() << __FUNCTION__ << "_qvectqpf_boxCornersNoZLI.size() =" << _qvectqpf_boxCornersNoZLI.size();
    //qDebug() << __FUNCTION__ << "_qvectqpf_boxCornersZLI.size() =" << _qvectqpf_boxCornersZLI.size();
    for(auto& iterqpf : _qvectqpf_boxCornersNoZLI) {
        //qDebug() << __FUNCTION__ << "looping on _qvectqpf_boxCornersNoZLI";
        _qvectqpf_boxCornersZLI[idxqpf] = iterqpf / static_cast<qreal>(_currentZLI);
        _polygonFZLI << _qvectqpf_boxCornersZLI[idxqpf];
        idxqpf++;
    }
    _qPainterPath_shapeZLI = {};
    _qPainterPath_shapeZLI.addPolygon(_polygonFZLI);

#ifdef def_boxItem2_bboxcomputeMethod1
    _qrectf_boundingBoxZLI.setTop   ((_qrectf_boundingBoxNoZLI_noWidthBorder.top()   / _currentZLI));
    _qrectf_boundingBoxZLI.setBottom((_qrectf_boundingBoxNoZLI_noWidthBorder.bottom()/ _currentZLI));
    _qrectf_boundingBoxZLI.setLeft  ((_qrectf_boundingBoxNoZLI_noWidthBorder.left()  / _currentZLI));
    _qrectf_boundingBoxZLI.setRight ((_qrectf_boundingBoxNoZLI_noWidthBorder.right() / _currentZLI));
#else
    qreal widthBorderZLI = _widthBorderZLI; //* static_cast<qreal>(_currentZLI)) / 2.0;
    /*if (_bUseAntialiasing) {
        widthBorderZLI *= 2.0;
    }*/

    _qrectf_boundingBoxZLI.setTop   ((_qrectf_boundingBoxNoZLI_noWidthBorder.top()   / _currentZLI) - widthBorderZLI);
    _qrectf_boundingBoxZLI.setBottom((_qrectf_boundingBoxNoZLI_noWidthBorder.bottom()/ _currentZLI) + widthBorderZLI);
    _qrectf_boundingBoxZLI.setLeft  ((_qrectf_boundingBoxNoZLI_noWidthBorder.left()  / _currentZLI) - widthBorderZLI);
    _qrectf_boundingBoxZLI.setRight ((_qrectf_boundingBoxNoZLI_noWidthBorder.right() / _currentZLI) + widthBorderZLI);
#endif

    qDebug() << __FUNCTION__ << "_qvectqpf_boxCornersNoZLI = " << _qvectqpf_boxCornersNoZLI;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxNoZLI (top,let) (bottom,right) = "
             << _qrectf_boundingBoxNoZLI_noWidthBorder.topLeft()
             << _qrectf_boundingBoxNoZLI_noWidthBorder.bottomRight();
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qPainterPath_shapeNoZLI = " << _qPainterPath_shapeNoZLI;
    qDebug() << __FUNCTION__ << "_polygonFNoZLI = " << _polygonFNoZLI;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qvectqpf_boxCornersZLI = " << _qvectqpf_boxCornersZLI;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxZLI (top,let) (bottom,right) = "
             << _qrectf_boundingBoxZLI.topLeft()
             << _qrectf_boundingBoxZLI.bottomRight();
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qPainterPath_shapeZLI = " << _qPainterPath_shapeZLI;
    qDebug() << __FUNCTION__ << "_polygonFZLI = " << _polygonFZLI;

    //qDebug() << __FUNCTION__ << "_qrectf_boundingBoxNoZLI   = " << _qrectf_boundingBoxNoZLI;
    //qDebug() << __FUNCTION__ << "_qrectf_boundingBox = " << _qrectf_boundingBox;
}

CustomGraphicsBoxItem2::~CustomGraphicsBoxItem2() {
    //qDebug() << __FUNCTION__;
}

void CustomGraphicsBoxItem2::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    //qDebug() << __FUNCTION__ << "about CustomGraphicsBoxItem2";

    _bHover = true;
    if (!_bSelectable) {
        event->accept();
        return;
    }

    setSelected(_bHover);
    event->accept();

}

void CustomGraphicsBoxItem2::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {

    _bHover = false;

    if (!_bSelectable) {
        event->accept();
        return;
    }

    setSelected(_bHover);
    event->accept();

}

void CustomGraphicsBoxItem2::setSelectable(bool bState) {
    _bSelectable = bState;
    _bHighlight = false;
    setSelected(false);
}

void CustomGraphicsBoxItem2::setSelected(bool bNewState) {
    if (bNewState) {
        _PtrUsedPen = &_pForSelected;        
    } else {
        if (_bHighlight) {
            setState_highlighted(_bHighlight);
            return;
        } else {
            _PtrUsedPen = &_pForUnselected;
        }
    }    
    update();

}

void CustomGraphicsBoxItem2::setState_highlighted(bool bHighlight) { //this is about current selection @#LP rename, to avoid amibigiguity with setSelected (which is for now about mouseHover)
    _bHighlight = bHighlight;

    if (_bHighlight) {
       _PtrUsedPen = &_pForHighlight;

       update();
       return;
    }
    if (_bSelectable) {
        setSelected(_bHover);
    } else {
        setSelected(false);
    }
}

bool CustomGraphicsBoxItem2::getState_highlighted() {
    return(_bHighlight);
}

bool CustomGraphicsBoxItem2::getState_hover() {
    return(_bHover);
}

int CustomGraphicsBoxItem2::getBoxID() {
    return(_boxID);
}
