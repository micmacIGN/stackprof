#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsItem>
#include <QDebug>

#include <QKeyEvent>

#include <QList>
#include <QGraphicsView>

#include <QGraphicsRectItem>

#include "CustomGraphicsScene_profil.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h"

#include <QFontMetrics>

#include <QTransform>

#include <QWidget>

#include "landmarks.h"

#include <QRandomGenerator> //for dev test only

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.h"
#include "customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.h"

#include "ConverterPixelToQsc.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.h"

#include <algorithm> //for min_element() and max_element()

#include "../../mathComputation/linearRegression.h" //to compute locations of y for x0 left side and x0 right side

#include "../core/leftRight_sideAccess.hpp"

#include "../core/ComputationCore_structures.h"

#include "../../toolbox/toolbox_math.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.h"

#include "customGraphicsItems/graphicProfil/CustomLineNoEvent.h"

#include <qmath.h>

bool CustomGraphicsScene_profil::feedScene_withExtendedYRange(
        e_ComponentsAccess eCA_component,
        const S_StackedProfilInEdition& stackedProfilInEdition,
        qreal throughAllScene_yMin, qreal throughAllScene_yMax,
        qreal xGraphMin, qreal xGraphMax,
        e_MeanMedianIndexAccess meanMedianIndexAccess,
        const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil)" <<  "with eCA_component = " << eCA_component;

    _eCA_component = eCA_component;

    _eProfilAdjustMode = eProfilAdjustMode;

    removeAllAddedItems();

    _eAlignementSceneMode = e_ASM_alignSameRangeWidthAndHeight;

    _computedValuesWithValidityFlag.clear();

    if (meanMedianIndexAccess <= e_MMIA_invalid) {
        qDebug() << __FUNCTION__ << "error: meanMedianIndexAccess is e_MMIA_invalid";
        return(false);
    }
    if (meanMedianIndexAccess >= e_MMIA_count) {
        qDebug() << __FUNCTION__ << "error: meanMedianIndexAccess is e_MMIA_invalid";
        return(false);
    }

    if (!stackedProfilInEdition._stackedProfilWithMeasurementsPtr) {
        qDebug() << __FUNCTION__ << "error: _stackedProfilWithMeasurementsPtr is nullptr";
        return(false);
    }

    _stackedProfilWithMeasurementsPtr = stackedProfilInEdition._stackedProfilWithMeasurementsPtr;

    S_LeftRightSides_linearRegressionModel* leftRightSides_linearRegressionModelPtr =
            _stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(stackedProfilInEdition._measureIndex);

    if (!leftRightSides_linearRegressionModelPtr) {
        qDebug() << __FUNCTION__  << " error: getting ptr on secondaryMeasureIdx (idx: " << stackedProfilInEdition._measureIndex._secondaryMeasureIdx << " )";
        return(false);
    }

    initPixelSizeElements_andFontSize(11, //12, //8
                                      2,3,
                                      3,5,
                                      7,3,7,3);

    showLinesForBigLandmarkThroughArea(true,false);

    _xRangeForLinearRegressionComputation[e_LRsA_left  ] = leftRightSides_linearRegressionModelPtr->
            _linearRegressionParam_Left_Right[e_LRsA_left].
            _xRangeForLinearRegressionComputation;

    _xRangeForLinearRegressionComputation[e_LRsA_right ] = leftRightSides_linearRegressionModelPtr->
            _linearRegressionParam_Left_Right[e_LRsA_right].
            _xRangeForLinearRegressionComputation;

    qDebug() << __FUNCTION__ << "_xRangeForLinearRegressionComputation[left] = "
             << "_bValid = " << _xRangeForLinearRegressionComputation[e_LRsA_left  ]._bValid
             << "_xMin   = " << _xRangeForLinearRegressionComputation[e_LRsA_left  ]._xMin
             << "_xMax   = " << _xRangeForLinearRegressionComputation[e_LRsA_left  ]._xMax;

    qDebug() << __FUNCTION__ << "_xRangeForLinearRegressionComputation[right] = "
             << "_bValid = " << _xRangeForLinearRegressionComputation[e_LRsA_right  ]._bValid
             << "_xMin   = " << _xRangeForLinearRegressionComputation[e_LRsA_right  ]._xMin
             << "_xMax   = " << _xRangeForLinearRegressionComputation[e_LRsA_right  ]._xMax;

    //copy the curve for futur usage
    _computedValuesWithValidityFlag = _stackedProfilWithMeasurementsPtr->_qvectqvect_aboutMeanMedian[meanMedianIndexAccess];

    /*qDebug() << "v._anonymMajorMinorWithValidityFlag._major_centralValue:";
    for (auto &v: _computedValuesWithValidityFlag) {
        if (v._anonymMajorMinorWithValidityFlag._bValuesValid) {
            qDebug() << "";
            qDebug() << " before: " << v._anonymMajorMinorWithValidityFlag._major_centralValue;

            v._anonymMajorMinorWithValidityFlag._major_centralValue       = cccsY_to_qcsY(v._anonymMajorMinorWithValidityFlag._major_centralValue);
            v._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue = cccsY_to_qcsY(v._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue);

            qDebug() << " after: " << v._anonymMajorMinorWithValidityFlag._major_centralValue;

        }
    }*/

    //extended y range give warranty that any two lines from equation are visible centered on X0

    //compute two points about each lines equations
    //It will be used to draw the lines
    //The two points located at x0 are used also as candidated for y min and y max of the curve graphic area
    _x0 = //_stackedProfilWithMeasurementsPtr->_LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0;
            leftRightSides_linearRegressionModelPtr->_x0ForYOffsetComputation._x0;

    double leftRightSides_xlimit[2] = {
        xGraphMin,
        xGraphMax
    };

    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left].clear();
    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right].clear();

    bool bReport = compute_pointsCoupleForLineRegDisplay_left_right_sides(
        throughAllScene_yMin, throughAllScene_yMax,
        leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right,
        _x0, //x0 to compute according y, this point needs to be visible on screen in any case
        leftRightSides_xlimit,
        _pointCoupleForLineRegDisplay_Left_Right);
    //@#LP no bReport check

    qDebug() << __FUNCTION__ << "debug check ymin ymax:";

    //use computed y range whenAlone
    setXYRange(xGraphMin, xGraphMax, throughAllScene_yMin, throughAllScene_yMax);

    //uint countNbValidPoint = 0;

    qDebug()  << __FUNCTION__ << "_computedValuesWithValidityFlag.size() = " << _computedValuesWithValidityFlag.size();

    //'convert' the curve to match with qsc
    for (auto &v: _computedValuesWithValidityFlag) {
        if (v._anonymMajorMinorWithValidityFlag._bValuesValid) {

            //countNbValidPoint++;

            //qDebug()  << __FUNCTION__ << " _major_centralValue       before: " << v._anonymMajorMinorWithValidityFlag._major_centralValue;
            //qDebug()  << __FUNCTION__ << " _minor_aroundCentralValue before: " << v._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;

            v._anonymMajorMinorWithValidityFlag._major_centralValue = cccsY_to_qscY(
            v._anonymMajorMinorWithValidityFlag._major_centralValue);// + (-yGraphMin));

            //qDebug()  << __FUNCTION__ << " _major_centralValue       after: " << v._anonymMajorMinorWithValidityFlag._major_centralValue;
            //qDebug()  << __FUNCTION__ << " _minor_aroundCentralValue after: " << v._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;
        } else {
            qDebug()  << __FUNCTION__ << "v._anonymMajorMinorWithValidityFlag._bValuesValid is false!";
        }
    }

    //qDebug()  << __FUNCTION__ << "_computedValuesWithValidityFlag.size() = " << _computedValuesWithValidityFlag.size();
    //qDebug()  << __FUNCTION__ << "                     countNbValidPoint = " << countNbValidPoint;

    /*qDebug()  << __FUNCTION__ << "_computedValuesWithValidityFlag after :";
    for (const auto &v: _computedValuesWithValidityFlag) {
        if (v._anonymMajorMinorWithValidityFlag._bValuesValid) {
            qDebug()  << __FUNCTION__ << "v._major_centralValue = "       << v._anonymMajorMinorWithValidityFlag._major_centralValue;
            qDebug()  << __FUNCTION__ << "v._minor_aroundCentralValue = " << v._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;
        }
    }*/

    if (bReport) {

        for (int ieLRSA = e_LRsA_left; ieLRSA <= e_LRsA_right; ieLRSA++) {

            double y_convBig = _pointCoupleForLineRegDisplay_Left_Right[ieLRSA]._pointWithBiggestX.y();
                _pointCoupleForLineRegDisplay_Left_Right[ieLRSA]._pointWithBiggestX.setY(cccsY_to_qscY(y_convBig));

            double y_convSmall = _pointCoupleForLineRegDisplay_Left_Right[ieLRSA]._pointWithSmallestX.y();
                _pointCoupleForLineRegDisplay_Left_Right[ieLRSA]._pointWithSmallestX.setY(cccsY_to_qscY(y_convSmall));
        }
    }

    computeLandmarks_and_createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem();

    if (!_customGraphicsItemEnvelop) {

        qDebug() << __FUNCTION__ << "before new: _customGraphicsItemEnvelop = @" << (void*)_customGraphicsItemEnvelop;

        qDebug() << __FUNCTION__ << "_cccs_yMin =" << _cccs_yMin;
        qDebug() << __FUNCTION__ << "_cccs_yMax =" << _cccs_yMax;
        qDebug() << __FUNCTION__ << "cccsY_to_qscY(_cccs_yMin) =" << cccsY_to_qscY(_cccs_yMin);
        qDebug() << __FUNCTION__ << "cccsY_to_qscY(_cccs_yMax) =" << cccsY_to_qscY(_cccs_yMax);

        _customGraphicsItemEnvelop = new CustomGraphicsItemEnvelop();    //consider that an alloc error exception will exist if alloc error
        _customGraphicsItemEnvelop->setCurve(_computedValuesWithValidityFlag,
                                            _cccs_xMin,
                                            _cccs_xMax,
                                            cccsY_to_qscY(_cccs_yMax),
                                            cccsY_to_qscY(_cccs_yMin),
                                            1.0);
        _customGraphicsItemEnvelop->setGraphicsParameters(sGraphicParameters._sGP_PCurveAndEnvelop);
        addItem(_customGraphicsItemEnvelop);
    }
    _customGraphicsItemEnvelop->setZValue(-1.0); //curve under graduation

    if (!_customGraphicsItemCurve) {

        qDebug() << __FUNCTION__ << "before new: _customGraphicsItemCurve = @" << (void*)_customGraphicsItemCurve;

        _customGraphicsItemCurve = new CustomGraphicsItemCurve();    //consider that an alloc error exception will exist if alloc error
        _customGraphicsItemCurve->setCurve(_computedValuesWithValidityFlag,
                                            _cccs_xMin, _cccs_xMax,
                                            cccsY_to_qscY(_cccs_yMax),
                                            cccsY_to_qscY(_cccs_yMin), 1.0);
        _customGraphicsItemCurve->setGraphicsParameters(sGraphicParameters._sGP_PCurveAndEnvelop);
        addItem(_customGraphicsItemCurve);
    }

    _customGraphicsItemCurve->setZValue(1.0); //curve over graduation
    //_customGraphicsItemCurve->setZValue(-1.0); //curve under graduation

    if (!_CGItemDblSidesXAdjWithCompLines_mainMeasure) {

        qDebug() << __FUNCTION__  << " if (_CGItemDblSidesXAdjWithCompLines_mainMeasure == nullptr) {";

        _CGItemDblSidesXAdjWithCompLines_mainMeasure = new CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine(
            _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left ],
            _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right],

            _xRangeForLinearRegressionComputation[e_LRsA_left],
            _xRangeForLinearRegressionComputation[e_LRsA_right],

           _cccs_xMin,
           _cccs_xMax,
                 _x0,

           cccsY_to_qscY(_cccs_yMax),
           cccsY_to_qscY(_cccs_yMin),
           {_pixel_gripSquareSideWidth, _pixel_gripSquareSideHeight});


        _CGItemDblSidesXAdjWithCompLines_mainMeasure->setGraphicsParameters(sGraphicParameters._sGP_LinearRegression);

        addItem(_CGItemDblSidesXAdjWithCompLines_mainMeasure);

        _CGItemDblSidesXAdjWithCompLines_mainMeasure->setZValue(3.0); //bar and line over curve (and graduation)

        connect(_CGItemDblSidesXAdjWithCompLines_mainMeasure, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_xPosition_changed,
                this, &CustomGraphicsScene_profil::slot_xAdjusterPosition_changed);

        connect(_CGItemDblSidesXAdjWithCompLines_mainMeasure, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_selectionStateChanged,
                this, &CustomGraphicsScene_profil::slot_selectionStateChanged_aboutXAdjusterOfDoubleSidesXAdjusters);

        qDebug() << __FUNCTION__  << "3) before #318";
        //direct forward:
        connect(_CGItemDblSidesXAdjWithCompLines_mainMeasure, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_locationInAdjustement,
                this, &CustomGraphicsScene_profil::slot_locationInAdjustement);
        qDebug() << __FUNCTION__  << "3) after #322";
    }

    if (!_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure) {

        qDebug() << __FUNCTION__  << " if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure == nullptr) {";

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure = new CustomGraphicsItemVerticalAdjusterWithCentereredGrip(_x0,
                                                                                                                         _cccs_xMin+1.0, _cccs_xMax-1.0,
                                                                                                                         cccsY_to_qscY(_cccs_yMin),
                                                                                                                         false, //false: grip not visible
                                                                                                                         false, //false: not mouse movable
                                                                                                                         _pixel_gripSquareSideWidth,
                                                                                                                         _pixel_gripSquareSideHeight);

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setGraphicsParameters(sGraphicParameters._sGP_LinearRegression._sCT_xCentralAxis);

        addItem(_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure);

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setZValue(2.0); //bar and line over graduation, curve, and x adjusters

        connect(_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::signal_xPosition_changed,
                this, &CustomGraphicsScene_profil::slot_centerX0AdjusterPosition_changed);

        //@LP note dev: was Not here (missing) ?
        qDebug() << __FUNCTION__  << "3) before #551";
        //direct forward:
        connect(_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::signal_locationInAdjustement,
                this, &CustomGraphicsScene_profil::signal_locationInAdjustement);
        qDebug() << __FUNCTION__  << "3) after #555";

    }

    setProfilAdjustMode(eProfilAdjustMode);

    return(true);
}
