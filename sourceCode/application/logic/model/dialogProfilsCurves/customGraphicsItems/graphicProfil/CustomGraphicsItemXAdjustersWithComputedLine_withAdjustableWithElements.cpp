#include <QDebug>

#include <QBrush>
#include <QPen>

#include "CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h"
#include "CustomGraphicsItemType_v2.h"

#include "CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.h"

#include "../../../core/ComputationCore_structures.h"

#include "CustomGraphicsLineItem.h"

CustomGraphicsItemXAdjustersWithComputedLine::CustomGraphicsItemXAdjustersWithComputedLine(
        e_LRsideAccess e_LRsA_locationAboutX0CentralAxis,
        S_CoupleOfQPointF_withValidityFlag coupleOfQPointFOnComputedLine,
        S_XRangeForLinearRegressionComputation xAdjustersPositions,

        double xMin, double xMax,
        double yMin, double yMax,

        QSizeF qsizef_adjusterPixelGripSize,
        QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),

    _e_LRsA_locationAboutX0CentralAxis(e_LRsA_locationAboutX0CentralAxis),
    _coupleOfQPointFOnComputedLine(coupleOfQPointFOnComputedLine),

    _xAdjustersPositions(xAdjustersPositions),
    _xMin(xMin), _xMax(xMax),
    _yMin(yMin), _yMax(yMax),

    _qsizef_adjusterPixelGripSize(qsizef_adjusterPixelGripSize),
    _addedLinePtr(nullptr) {

    _qrectfbbox_qcsDim.setTop(yMin);
    _qrectfbbox_qcsDim.setBottom(yMax); // @LP missing the grip height (do not forget to add it for bbox pixel dim)
    _qrectfbbox_qcsDim.setLeft(xMin);
    _qrectfbbox_qcsDim.setRight(xMax);


    qDebug() << __FUNCTION__ << "(constructor CustomGraphicsItemXAdjustersWithComputedLine) _qrectfbbox_qcsDim = tl, br: " << _qrectfbbox_qcsDim.topLeft() << ", " << _qrectfbbox_qcsDim.bottomRight();

    _CGItemVerticalItemAdjusterLeft = nullptr;
    _CGItemVerticalItemAdjusterRight = nullptr;

    qDebug() << __FUNCTION__ << "_yMin = " << _yMin;
    qDebug() << __FUNCTION__ << "_yMax = " << _yMax;


    _CGItemVerticalItemAdjusterLeft = new CustomGraphicsItemVerticalAdjuster(_xAdjustersPositions._xMin, //starting location
                                                                              _xMin, _xMax,
                                                                             _xAdjustersPositions._xMax,//additional x limit

                                                                             e_LRsideAccess::e_LRsA_left,

                                                                             _yMax,

                                                                             true,

                                                                             _qsizef_adjusterPixelGripSize.width(),
                                                                             _qsizef_adjusterPixelGripSize.height());


    _CGItemVerticalItemAdjusterLeft->setParentItem(this);    

    _CGItemVerticalItemAdjusterRight = new CustomGraphicsItemVerticalAdjuster(_xAdjustersPositions._xMax, //starting location
                                                                               _xMin, _xMax,
                                                                              _xAdjustersPositions._xMin, //additional x limit

                                                                               e_LRsideAccess::e_LRsA_right,

                                                                              _yMax,

                                                                              true,

                                                                              _qsizef_adjusterPixelGripSize.width(),
                                                                              _qsizef_adjusterPixelGripSize.height());

    _CGItemVerticalItemAdjusterRight->setParentItem(this);

    _addedLinePtr = new CustomGraphicsLineItem();
    _addedLinePtr->setParentItem(this);

    qDebug() << __FUNCTION__ << "line from ( " <<
            _coupleOfQPointFOnComputedLine._pointWithSmallestX.x() << ", " << _coupleOfQPointFOnComputedLine._pointWithSmallestX.y() << " )";
    qDebug() << __FUNCTION__ << "       to (" <<
            _coupleOfQPointFOnComputedLine._pointWithBiggestX.x()  << ", " << _coupleOfQPointFOnComputedLine._pointWithBiggestX.y();


    //connect the xlimit from one to the another, in the two direction
    connect(_CGItemVerticalItemAdjusterLeft,  &CustomGraphicsItemVerticalAdjuster::signal_setXLimitForMovement,
            _CGItemVerticalItemAdjusterRight, &CustomGraphicsItemVerticalAdjuster::slot_setXLimitForMovement);

    connect(_CGItemVerticalItemAdjusterRight, &CustomGraphicsItemVerticalAdjuster::signal_setXLimitForMovement,
            _CGItemVerticalItemAdjusterLeft,  &CustomGraphicsItemVerticalAdjuster::slot_setXLimitForMovement);

    //to handle the zvalue between the two sides, (a new signal will be sent adding _e_LRsA_locationAboutX0CentralAxis)
    connect(_CGItemVerticalItemAdjusterLeft, &CustomGraphicsItemVerticalAdjuster::signal_selectionStateChanged,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_selectionStateChanged);
    connect(_CGItemVerticalItemAdjusterRight, &CustomGraphicsItemVerticalAdjuster::signal_selectionStateChanged,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_selectionStateChanged);

    //connect the xPosition_changed from the two vertical adjuster
    connect(_CGItemVerticalItemAdjusterLeft,  &CustomGraphicsItemVerticalAdjuster::signal_xPosition_changed,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_xPosition_changed);
    connect(_CGItemVerticalItemAdjusterRight, &CustomGraphicsItemVerticalAdjuster::signal_xPosition_changed,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_xPosition_changed);


    //direct forward
    connect(_CGItemVerticalItemAdjusterLeft,  &CustomGraphicsItemVerticalAdjuster::signal_locationInAdjustement,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_locationInAdjustement);
    connect(_CGItemVerticalItemAdjusterRight, &CustomGraphicsItemVerticalAdjuster::signal_locationInAdjustement,
            this, &CustomGraphicsItemXAdjustersWithComputedLine::slot_locationInAdjustement);

}


void CustomGraphicsItemXAdjustersWithComputedLine::slot_selectionStateChanged(bool bNewState) {
    emit signal_selectionStateChanged(_e_LRsA_locationAboutX0CentralAxis, bNewState);
}

void CustomGraphicsItemXAdjustersWithComputedLine::slot_xPosition_changed(double xpos, int i_eLRsA) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemXAdjustersWithComputedLine)" << "xpos =" << xpos << "i_eLRsA =" << i_eLRsA;

    if (i_eLRsA == e_LRsA_left) {
        qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "if (i_eLRsA == e_LRsA_left) {";
        if (static_cast<int>(_xAdjustersPositions._xMin) != static_cast<int>(xpos)) {

            qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "yes, changed (min)";
            qDebug() << __FUNCTION__ << "(CGIXAWCL)"
                     << "emit signal_xPosition_changed("
                     << _e_LRsA_locationAboutX0CentralAxis << ", "
                     << xpos << ", "
                     << i_eLRsA;

            emit signal_xPosition_changed(_e_LRsA_locationAboutX0CentralAxis, xpos, i_eLRsA);
        } else {
            qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "no changed (min)";
        }
    } else { //e_LRsA_right
        qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "i_eLRsA is e_LRsA_right";
        if (static_cast<int>(_xAdjustersPositions._xMax) != static_cast<int>(xpos)) {
            qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "yes, changed (max)";
            qDebug() << __FUNCTION__ << "(CGIXAWCL)"
                     << "emit signal_xPosition_changed("
                     << _e_LRsA_locationAboutX0CentralAxis << ", "
                     << xpos << ", "
                     << i_eLRsA;

            emit signal_xPosition_changed(_e_LRsA_locationAboutX0CentralAxis, xpos, i_eLRsA);
        } else {
            qDebug() << __FUNCTION__ << "(CGIXAWCL)" << "no changed (max)";
        }
    }
}


QRectF CustomGraphicsItemXAdjustersWithComputedLine::boundingRect() const {
    //qDebug() <<__FUNCTION__ << "CustomGraphicsItemXAdjustersWithComputedLine";

    return(_qrectfbbox_pixelDim);
}

#include <QPainter>

void CustomGraphicsItemXAdjustersWithComputedLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // qDebug() <<__FUNCTION__ << "CustomGraphicsItemXAdjustersWithComputedLine";

    //@#LP debug

    /*
    QBrush qbrush_debug;
    qbrush_debug.setStyle(Qt::SolidPattern);
    qbrush_debug.setColor({128,255,0,128});
    painter->setBrush(qbrush_debug);
    painter->drawRect(_qrectfbbox_pixelDim);
    */

}

int CustomGraphicsItemXAdjustersWithComputedLine::type() const {
    return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsItemXAdjustersWithComputedLine);
}

void CustomGraphicsItemXAdjustersWithComputedLine::setGraphicsParameters(
    const S_Color_Thickness& sCT_xAdjuster,
    const S_Color_Thickness_Antialiasing& sCTA_computedLine) {

    _CGItemVerticalItemAdjusterLeft->setGraphicsParameters(sCT_xAdjuster);
    _CGItemVerticalItemAdjusterRight->setGraphicsParameters(sCT_xAdjuster);

    _addedLinePtr->setGraphicsParameters(sCTA_computedLine);
}

CustomGraphicsItemXAdjustersWithComputedLine::~CustomGraphicsItemXAdjustersWithComputedLine() {
    qDebug() << __FUNCTION__;
}

void CustomGraphicsItemXAdjustersWithComputedLine::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc/*, const QRectF& qrectfBBox_pixelDim*/) {

    prepareGeometryChange();
    _converterPixToqsc = converterPixToqsc;

    _qrectfbbox_pixelDim.setTop   (_converterPixToqsc.qsc_y_to_pixel(_qrectfbbox_qcsDim.top()));
    _qrectfbbox_pixelDim.setBottom(_converterPixToqsc.qsc_y_to_pixel(_qrectfbbox_qcsDim.bottom()) + _qsizef_adjusterPixelGripSize.height());

    _qrectfbbox_pixelDim.setLeft (_converterPixToqsc.qsc_x_to_pixel(_qrectfbbox_qcsDim.left()));
    _qrectfbbox_pixelDim.setRight(_converterPixToqsc.qsc_x_to_pixel(_qrectfbbox_qcsDim.right()));

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemXAdjustersWithComputedLine) _qrectfbbox_qcsDim   as tl, br: " << _qrectfbbox_qcsDim.topLeft() << ", " << _qrectfbbox_qcsDim.bottomRight();
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemXAdjustersWithComputedLine) _qrectfbbox_pixelDim as tl, br: " << _qrectfbbox_pixelDim.topLeft() << ", " << _qrectfbbox_pixelDim.bottomRight();

    if (_CGItemVerticalItemAdjusterLeft) {
        _CGItemVerticalItemAdjusterLeft->adaptToPixelScene(_converterPixToqsc);
    }
    if (_CGItemVerticalItemAdjusterRight) {
        _CGItemVerticalItemAdjusterRight->adaptToPixelScene(_converterPixToqsc);
    }

}


void CustomGraphicsItemXAdjustersWithComputedLine::setAndUpdateParameters_coupleOfQPointFOnComputedLine(
        const S_CoupleOfQPointF_withValidityFlag& coupleOfQPointFOnComputedLine) {
    _coupleOfQPointFOnComputedLine = coupleOfQPointFOnComputedLine;
    if (!_addedLinePtr) {
        //@#LP
        return;
    }

    prepareGeometryChange();

    //@##LP validity flag is not tested (from S_CoupleOfQPointF_withValidityFlag) and should be handled
    //for now, _addedLinePtr is hence set from 0,0 to 0,0 (location is (x0, y top of graph curve rect area). it is invisible (without specific code), even with large width.

    _addedLinePtr->setLine(_converterPixToqsc.qsc_x_to_pixel(_coupleOfQPointFOnComputedLine._pointWithSmallestX.x()),
                           _converterPixToqsc.qsc_y_to_pixel(_coupleOfQPointFOnComputedLine._pointWithSmallestX.y()),
                           _converterPixToqsc.qsc_x_to_pixel(_coupleOfQPointFOnComputedLine._pointWithBiggestX.x()),
                           _converterPixToqsc.qsc_y_to_pixel(_coupleOfQPointFOnComputedLine._pointWithBiggestX.y()));

   qDebug() << __FUNCTION__ << "---";
   qDebug() << __FUNCTION__ << "pixel loc pointWithSmallest x =>" << _converterPixToqsc.qsc_x_to_pixel(_coupleOfQPointFOnComputedLine._pointWithSmallestX.x());
   qDebug() << __FUNCTION__ << "pixel loc pointWithSmallest y =>" << _converterPixToqsc.qsc_y_to_pixel(_coupleOfQPointFOnComputedLine._pointWithSmallestX.y());
   qDebug() << __FUNCTION__ << "-";
   qDebug() << __FUNCTION__ << "pixel loc pointWithBiggest  x =>" << _converterPixToqsc.qsc_x_to_pixel(_coupleOfQPointFOnComputedLine._pointWithBiggestX.x());
   qDebug() << __FUNCTION__ << "pixel loc pointWithBiggest  y =>" << _converterPixToqsc.qsc_y_to_pixel(_coupleOfQPointFOnComputedLine._pointWithBiggestX.y());
   qDebug() << __FUNCTION__ << "---";

}

void CustomGraphicsItemXAdjustersWithComputedLine::setAndUpdateParameters_xAdjustersPositions(S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation) {

    _xAdjustersPositions = xRangeForLinearRegressionComputation;

    qDebug() << "(CustomGraphicsItemXAdjustersWithComputedLine) _xAdjustersPositions._xMin = " << _xAdjustersPositions._xMin;
    qDebug() << "(CustomGraphicsItemXAdjustersWithComputedLine) _xAdjustersPositions._xMax = " << _xAdjustersPositions._xMax;

    if (_CGItemVerticalItemAdjusterLeft) {
        _CGItemVerticalItemAdjusterLeft->setXPos(_xAdjustersPositions._xMin);
    }
    if (_CGItemVerticalItemAdjusterRight) {
        _CGItemVerticalItemAdjusterRight->setXPos(_xAdjustersPositions._xMax);
    }
}


void CustomGraphicsItemXAdjustersWithComputedLine::setState_movableAdjustersWithAccordGripsSquareVisibility(bool bState) {

    if (_CGItemVerticalItemAdjusterLeft) {
        _CGItemVerticalItemAdjusterLeft->setState_movableWithAccordGripsSquareVisibility(bState);
    }
    if (_CGItemVerticalItemAdjusterRight) {
        _CGItemVerticalItemAdjusterRight->setState_movableWithAccordGripsSquareVisibility(bState);
    }
}


void CustomGraphicsItemXAdjustersWithComputedLine::slot_locationInAdjustement(bool bState) { //just redirect
    qDebug() << "(CustomGraphicsItemXAdjustersWithComputedLine) bState =" << bState;
    emit signal_locationInAdjustement(bState);
}

