#include <QDebug>

#include "CustomGraphicsItemType_v2.h"
#include "CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.h"

#include "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.h"

#include "../../../core/ComputationCore_structures.h"

//@#LP use const &
CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine(
        S_CoupleOfQPointF_withValidityFlag pointCoupleForLeftSide_forComputedLine,
        S_CoupleOfQPointF_withValidityFlag pointCoupleForRightSide_forComputedLine,

        S_XRangeForLinearRegressionComputation xAdjustersPositionsForLeftSide,
        S_XRangeForLinearRegressionComputation xAdjustersPositionsForRightSide,

        double xLimitMinLeftSide,
        double xLimitMaxRightSide,
        double xCenterAxis, //limits used as: xLimitMaxLeftSide andxLimitMinRightSide

        double yMin, double yMax,

        QSizeF qsizef_adjusterPixelGripSize,
        QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),

    _pointCoupleForLeftSide_forComputedLine { pointCoupleForLeftSide_forComputedLine },
    _pointCoupleForRightSide_forComputedLine { pointCoupleForRightSide_forComputedLine },

    _xAdjustersPositionsForLeftSide(xAdjustersPositionsForLeftSide),
    _xAdjustersPositionsForRightSide(xAdjustersPositionsForRightSide),

    _xLimitMinLeftSide(xLimitMinLeftSide),
    _xLimitMaxRightSide(xLimitMaxRightSide),
    _xCenterAxis(xCenterAxis), //limits used as: xLimitMaxLeftSide andxLimitMinRightSide

    _yMin(yMin),
    _yMax(yMax),

    _qsizef_adjusterPixelGripSize(qsizef_adjusterPixelGripSize)

     {

    qDebug() << __FUNCTION__  << "@ " << (void*)this;

    _CGItemXAdjustersWithComputedLineLeftSide = nullptr;
    _CGItemXAdjustersWithComputedLineRightSide = nullptr;

    _qrectfbbox_qcsDim.setLeft(_xLimitMinLeftSide);
    _qrectfbbox_qcsDim.setRight(_xLimitMaxRightSide);
    _qrectfbbox_qcsDim.setTop(_yMin);
    _qrectfbbox_qcsDim.setBottom(_yMax); //@LP note that this bottom is 'wrong' as the pixel height of the grip square is not added and should be.               //+ /*_parametersForPixelResizeInvariance._qsc_gripSquareHeight*/ + _qsizef_adjusterPixelGripSize.height());
                                         //but we are here in math/logic dimension, not in pixel and we do not have the pixel height in math/logic dimension here

    //bbox_pixelDim has to take this in consideration and add the pixel grip size to _qrectfbbox_qcsDim.setBottom

    //just to have a 'valid invalid init value':
    _converterPixToqsc.setWidthRatioFrom(1.0,1.0);
    _converterPixToqsc.setHeightRatioFrom(1.0,1.0);

    createAndSet_CGItemXAdjustersWithComputedLine(); //updateParameters();
}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc) {

    prepareGeometryChange();
    _converterPixToqsc = converterPixToqsc;

    QRectF qrectfbbox_pixelDim_debugMe;
    qrectfbbox_pixelDim_debugMe.setTop   (_converterPixToqsc.qsc_y_to_pixel(_qrectfbbox_qcsDim.top()));
    qrectfbbox_pixelDim_debugMe.setBottom(_converterPixToqsc.qsc_y_to_pixel(_qrectfbbox_qcsDim.bottom()) + _qsizef_adjusterPixelGripSize.height());

    qrectfbbox_pixelDim_debugMe.setLeft (_converterPixToqsc.qsc_x_to_pixel(_qrectfbbox_qcsDim.left()));
    qrectfbbox_pixelDim_debugMe.setRight(_converterPixToqsc.qsc_x_to_pixel(_qrectfbbox_qcsDim.right()));

    _qrectfbbox_pixelDim = qrectfbbox_pixelDim_debugMe;

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine) (used for pixelDim computation) _qrectfbbox_qcsDim                    as tl, br: " << _qrectfbbox_qcsDim.topLeft() << ", " << _qrectfbbox_qcsDim.bottomRight();
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine) (       computed localy       ) qrectfbbox_pixelDim_debugMe           as tl, br: " <<  qrectfbbox_pixelDim_debugMe.topLeft() << ", " << qrectfbbox_pixelDim_debugMe.bottomRight();
    //qDebug() << __FUNCTION__ << "(CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine) (got directly from oustide) _qrectfbbox_pixelDim as tl, br: " << _qrectfbbox_pixelDim.topLeft() << ", " << _qrectfbbox_pixelDim.bottomRight();

    if (_CGItemXAdjustersWithComputedLineLeftSide) {
        _CGItemXAdjustersWithComputedLineLeftSide->adaptToPixelScene(_converterPixToqsc);
    }
    if (_CGItemXAdjustersWithComputedLineRightSide) {
        _CGItemXAdjustersWithComputedLineRightSide->adaptToPixelScene(_converterPixToqsc);
    }

}


void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::setGraphicsParameters(const S_GraphicsParameters_LinearRegression& sGP_LinearRegression) {

    _CGItemXAdjustersWithComputedLineLeftSide ->setGraphicsParameters(sGP_LinearRegression._sCT_xAdjuster, sGP_LinearRegression._sCTA_computedLine);
    _CGItemXAdjustersWithComputedLineRightSide->setGraphicsParameters(sGP_LinearRegression._sCT_xAdjuster, sGP_LinearRegression._sCTA_computedLine);

}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::setAndUpdateParameters_pointCoupleForLeftRightSide(
        const S_CoupleOfQPointF_withValidityFlag& pointCoupleForLeftSide,
        const S_CoupleOfQPointF_withValidityFlag& pointCoupleForRightSide) {

    qDebug() << __FUNCTION__ << "pointCoupleForLeftSide:"
                             << "_bValid: " << pointCoupleForLeftSide._bValid
                             << "pointWithBiggestX : " << pointCoupleForLeftSide._pointWithBiggestX.x()  << pointCoupleForLeftSide._pointWithBiggestX.y()
                             << "pointWithSmallestX: " << pointCoupleForLeftSide._pointWithSmallestX.x() << pointCoupleForLeftSide._pointWithSmallestX.y();

    qDebug() << __FUNCTION__ << "pointCoupleForRightSide:"
                             << "_bValid: " << pointCoupleForRightSide._bValid
                             << "pointWithBiggestX : " << pointCoupleForRightSide._pointWithBiggestX.x()  << pointCoupleForRightSide._pointWithBiggestX.y()
                             << "pointWithSmallestX: " << pointCoupleForRightSide._pointWithSmallestX.x() << pointCoupleForRightSide._pointWithSmallestX.y();

    //@#LP use array[2] with indexes left/right access
    set_pointCoupleForLeftSide(pointCoupleForLeftSide);
    set_pointCoupleForRightSide(pointCoupleForRightSide);
}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::setAndUpdateParameters_xAdjustersPositionsForLeftRightSide(const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation[2]) {
    _xAdjustersPositionsForLeftSide  = xRangeForLinearRegressionComputation[e_LRsA_left];
    _xAdjustersPositionsForRightSide = xRangeForLinearRegressionComputation[e_LRsA_right];

    if (_CGItemXAdjustersWithComputedLineLeftSide) {
        _CGItemXAdjustersWithComputedLineLeftSide->setAndUpdateParameters_xAdjustersPositions(_xAdjustersPositionsForLeftSide);
    }
    if (_CGItemXAdjustersWithComputedLineRightSide) {
        _CGItemXAdjustersWithComputedLineRightSide->setAndUpdateParameters_xAdjustersPositions(_xAdjustersPositionsForRightSide);
    }
}


//@#LP refactor with container and left right index access
//@#LP check if the other structure without validity flag is better instead of S_CoupleOfQPointF_withValidityFlag
void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::set_pointCoupleForLeftSide(const S_CoupleOfQPointF_withValidityFlag& pointCoupleForLeftSide_forComputedLine) {
    _pointCoupleForLeftSide_forComputedLine = pointCoupleForLeftSide_forComputedLine;
    if (_CGItemXAdjustersWithComputedLineLeftSide == nullptr) {
        //@#LP msg
        return;
    }
    _CGItemXAdjustersWithComputedLineLeftSide->setAndUpdateParameters_coupleOfQPointFOnComputedLine(_pointCoupleForLeftSide_forComputedLine);
}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::set_pointCoupleForRightSide(const S_CoupleOfQPointF_withValidityFlag& pointCoupleForRightSide_forComputedLine) {
    _pointCoupleForRightSide_forComputedLine = pointCoupleForRightSide_forComputedLine;
    if (_CGItemXAdjustersWithComputedLineRightSide == nullptr) {
        //@#LP msg
        return;
    }

    _CGItemXAdjustersWithComputedLineRightSide->setAndUpdateParameters_coupleOfQPointFOnComputedLine(_pointCoupleForRightSide_forComputedLine);
}


void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::createAndSet_CGItemXAdjustersWithComputedLine() { //updateParameters() {

    qDebug() << __FUNCTION__ ;

    if (_CGItemXAdjustersWithComputedLineLeftSide == nullptr) {

        qDebug() << __FUNCTION__  <<"    if (_CGItemXAdjustersWithComputedLineLeftSide == nullptr) {";

        qDebug() << __FUNCTION__ << "_xAdjustersPositionsForLeftSide._xMin = " << _xAdjustersPositionsForLeftSide._xMin;
        qDebug() << __FUNCTION__ << "_xAdjustersPositionsForLeftSide._xMax = " << _xAdjustersPositionsForLeftSide._xMax;

           _CGItemXAdjustersWithComputedLineLeftSide = new CustomGraphicsItemXAdjustersWithComputedLine(
                        e_LRsA_left, //this is about the adjusters couple location about the central x0
                       _pointCoupleForLeftSide_forComputedLine,
                       _xAdjustersPositionsForLeftSide,

                       _xLimitMinLeftSide,_xCenterAxis,

                       _yMin,_yMax,

                       _qsizef_adjusterPixelGripSize
                       );
           _CGItemXAdjustersWithComputedLineLeftSide->setParentItem(this);
    }

    if (_CGItemXAdjustersWithComputedLineRightSide == nullptr) {

        qDebug() << __FUNCTION__  << "    if (_CGItemXAdjustersWithComputedLineRightSide == nullptr) {";

        qDebug() << __FUNCTION__ << "_xAdjustersPositionsForRightSide._xMin = " << _xAdjustersPositionsForRightSide._xMin;
        qDebug() << __FUNCTION__ << "_xAdjustersPositionsForRightSide._xMax = " << _xAdjustersPositionsForRightSide._xMax;


           _CGItemXAdjustersWithComputedLineRightSide = new CustomGraphicsItemXAdjustersWithComputedLine(
                       e_LRsA_right, //this is about the adjusters couple location about the central x0

                       _pointCoupleForRightSide_forComputedLine,
                       _xAdjustersPositionsForRightSide,

                       _xCenterAxis,_xLimitMaxRightSide,

                       _yMin,_yMax,

                       _qsizef_adjusterPixelGripSize
                       );
           _CGItemXAdjustersWithComputedLineRightSide->setParentItem(this);

    }

    if (  (_CGItemXAdjustersWithComputedLineLeftSide)
        &&(_CGItemXAdjustersWithComputedLineRightSide)) {

        //about the zvalue use case:
        connect(_CGItemXAdjustersWithComputedLineLeftSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_selectionStateChanged,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged);
        connect(_CGItemXAdjustersWithComputedLineRightSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_selectionStateChanged,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged);

        //about xpos change of vertical x adjusters:
        //'direct re-send'
        connect(_CGItemXAdjustersWithComputedLineLeftSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_xPosition_changed,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_xPosition_changed/*slot_xPosition_changed*/);
        connect(_CGItemXAdjustersWithComputedLineRightSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_xPosition_changed,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_xPosition_changed/*slot_xPosition_changed*/);

         //'direct re-send'
        connect(_CGItemXAdjustersWithComputedLineLeftSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_locationInAdjustement,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_locationInAdjustement/*slot_xPosition_changed*/);
        connect(_CGItemXAdjustersWithComputedLineRightSide, &CustomGraphicsItemXAdjustersWithComputedLine::signal_locationInAdjustement,
                this, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_locationInAdjustement/*slot_xPosition_changed*/);

    }
}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_locationInAdjustement(bool bState){ //just redirect
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine) bState = " << bState;
    emit signal_locationInAdjustement(bState);
}


/*void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_xPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA) {
    qDebug() << "CGID_BL_SXWCL::slot_xPosition_changed";
}*/


//The right line on left side can be located at the same x than the left line on the right side.
//Hence, a z value needs to be applyed to be able to see one over the other.
//As the lines are joined by side, to handle the case described above, the zvalue switch needs to be done here (CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine)
void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState) {

    qDebug() << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged";

    if (i_eLRsA_locationRelativeToX0CentralAxis == e_LRsA_right) {
        if (bNewState) {
            qDebug() << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged" << " ___ e_LRsA_right true";
            _CGItemXAdjustersWithComputedLineRightSide->setZValue(5.0);
        } else {
            qDebug() << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged" << " ___ e_LRsA_right false";
            _CGItemXAdjustersWithComputedLineRightSide->setZValue(4.0);
        }
    } else { //e_LRsA_left
        if (bNewState) {
            qDebug() << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged" << " ___ e_LRsA_left true";
            _CGItemXAdjustersWithComputedLineLeftSide->setZValue(5.0);
        } else {
            qDebug() << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::slot_selectionStateChanged" << " ___ e_LRsA_left false";
            _CGItemXAdjustersWithComputedLineLeftSide->setZValue(4.0);
        }
    }

    //special use case when one of the vertical adjuster is in the same location than the X0centerProfil
    emit signal_selectionStateChanged(i_eLRsA_locationRelativeToX0CentralAxis, bNewState);

}



QRectF CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::boundingRect() const {
    return(_qrectfbbox_pixelDim);
}

#include <QPainter>

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //qDebug() << __FUNCTION__ << "CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines";

    //@#LP debug:
    /*QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,0,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectfbbox_pixelDim);*/
}


int CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::type() const {
    return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines);
}

CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::~CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine() {
    qDebug() << __FUNCTION__  << "@ " << (void*)this;
}

void CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::setState_movableAdjustersWithAccordGripsSquareVisibility(bool bState) {

    if (_CGItemXAdjustersWithComputedLineLeftSide) {
        _CGItemXAdjustersWithComputedLineLeftSide->setState_movableAdjustersWithAccordGripsSquareVisibility(bState);

    }
    if (_CGItemXAdjustersWithComputedLineRightSide) {
        _CGItemXAdjustersWithComputedLineRightSide->setState_movableAdjustersWithAccordGripsSquareVisibility(bState);

    }
}
