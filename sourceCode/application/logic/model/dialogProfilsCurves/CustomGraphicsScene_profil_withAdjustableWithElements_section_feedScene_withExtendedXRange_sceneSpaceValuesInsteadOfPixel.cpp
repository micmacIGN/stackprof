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

bool CustomGraphicsScene_profil::feedScene_withExtendedXRange_sceneSpaceValuesInsteadOfPixel(
    e_ComponentsAccess eCA_component,
    const S_StackedProfilInEdition& stackedProfilInEdition,

    qreal throughAllScene_left_pixelDim, qreal throughAllScene_right_pixelDim, //scene rect smallest left through all layer, scene rect biggest right through all layer,
    qreal throughAllScene_wSceneSpaceAtLeft_pixelDim, qreal throughAllScene_wSceneSpaceAtRight_pixelDim,//scene rect biggest wSceneSpaceAtLeft and wSceneSpaceAtRight through all layer

    qreal xGraphMin, qreal xGraphMax,
    e_MeanMedianIndexAccess meanMedianIndexAccess,
    const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil)" <<  "with eCA_component = " << eCA_component;

    _eCA_component = eCA_component;

    _eProfilAdjustMode = eProfilAdjustMode;

    removeAllAddedItems();

    _eAlignementSceneMode = e_ASM_alignSameWidth;

    _throughAllScene_left_pixelDim  = throughAllScene_left_pixelDim;
    _throughAllScene_right_pixelDim = throughAllScene_right_pixelDim;
    _throughAllScene_wSceneSpaceAtLeft_pixelDim  = throughAllScene_wSceneSpaceAtLeft_pixelDim;
    _throughAllScene_wSceneSpaceAtRight_pixelDim = throughAllScene_wSceneSpaceAtRight_pixelDim;


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

    initPixelSizeElements_andFontSize(11,
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

    //compute graphic area y min and y max from:
    // ymin and ymax of major values
    // ymin and ymax of major values with minor around
    // these values can be adjusted later to be sure the (x0,y) for the two line equation are visible
    // There are computed now as starting values
    QVector<double> qvect_yMin_candidates;
    QVector<double> qvect_yMax_candidates;

    double yMinCurve, yMaxCurve;
    bool bReportFindMinMax_ofMajor =
            findMinMaxOfMajor_MMcomputedValuesWithValidityFlag(
                _computedValuesWithValidityFlag,
                yMinCurve, yMaxCurve);
    if (bReportFindMinMax_ofMajor) {
        qvect_yMin_candidates << yMinCurve;
        qvect_yMax_candidates << yMaxCurve;
    }

    qDebug() << __FUNCTION__ << " after findMinMaxOfMajor_MMcomputedValuesWithValidityFlag:";
    qDebug() << __FUNCTION__ << " qvect_yMin_candidates = " << qvect_yMin_candidates;
    qDebug() << __FUNCTION__ << " qvect_yMax_candidates = " << qvect_yMax_candidates;


    double yMinCurve_withMinorAround, yMaxCurve_withMinorAround;
    bool bReportFindMinMax_ofMajorWithMinorAround =
            findMinMaxOfMajorWithAddSubMinor_MMcomputedValuesWithValidityFlag(
                _computedValuesWithValidityFlag, 1.0,
                yMinCurve_withMinorAround, yMaxCurve_withMinorAround);
    if (bReportFindMinMax_ofMajorWithMinorAround) {
        qvect_yMin_candidates << yMinCurve_withMinorAround;
        qvect_yMax_candidates << yMaxCurve_withMinorAround;
    }
    qDebug() << __FUNCTION__ << " after findMinMaxOfMajorWithAddSubMinor_MMcomputedValuesWithValidityFlag:";
    qDebug() << __FUNCTION__ << " qvect_yMin_candidates = " << qvect_yMin_candidates;
    qDebug() << __FUNCTION__ << " qvect_yMax_candidates = " << qvect_yMax_candidates;

    double yGraphMin = *std::min_element(qvect_yMin_candidates.constBegin(), qvect_yMin_candidates.constEnd());
    double yGraphMax = *std::max_element(qvect_yMax_candidates.constBegin(), qvect_yMax_candidates.constEnd());

    qDebug() << __FUNCTION__ << "search the min max of the 'curve(s)':";
    //qDebug() << __FUNCTION__ << " qvect_yMin_candidates = " << qvect_yMin_candidates;
    //qDebug() << __FUNCTION__ << " qvect_yMax_candidates = " << qvect_yMax_candidates;

    qDebug() << __FUNCTION__ << " yGraphMin = " << yGraphMin;
    qDebug() << __FUNCTION__ << " yGraphMax = " << yGraphMax;

    qDebug() << __FUNCTION__ << " xGraphMin = " << xGraphMin;
    qDebug() << __FUNCTION__ << " xGraphMax = " << xGraphMax;

    //compute two points about each lines equations
    //It will be used to draw the lines
    //The two points located at x0 are used also as candidated for y min and y max of the curve graphic area
    _x0 = //_stackedProfilWithMeasurementsPtr->_LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0;
            leftRightSides_linearRegressionModelPtr->_x0ForYOffsetComputation._x0;

    double leftRightSides_xlimit[2] = {
        xGraphMin,
        xGraphMax
    };

    bool bExtendedRangeDueToFlatYMinMax = false;

    //for totally horizontal profil, extend the graph min and max around the value
    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(yGraphMin - yGraphMax)) {

        qDebug() << __FUNCTION__ << "needs y range extension; yGraphMin = yGraphMax = " << yGraphMin;

        double powerOfTen_ofY = powerOfTen(yGraphMin);
        qDebug() << __FUNCTION__ << "  powerOfTen_ofY: " << powerOfTen_ofY;
        double tenPowered_ofY = qPow(10,powerOfTen_ofY);
        qDebug() << __FUNCTION__ << "  tenPowered_ofY = " << tenPowered_ofY;

        double lowerStepValue = findLowerStepValueFrom(yGraphMin-tenPowered_ofY, tenPowered_ofY, 1.0);
        double upperStepValue = findUpperStepValueFrom(yGraphMin+tenPowered_ofY, tenPowered_ofY, 1.0);

        yGraphMin = lowerStepValue;
        yGraphMax = upperStepValue;

        qDebug() << __FUNCTION__ << "extended from " << yGraphMin << " to " << yGraphMax;

        bExtendedRangeDueToFlatYMinMax = true;

    }

    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left].clear();
    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right].clear();

    bool bReport = compute_pointsCoupleForLineRegDisplay_left_right_sides(
        yGraphMin, yGraphMax,
        leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right,
        _x0, //x0 to compute according y, this point needs to be visible on screen in any case
        leftRightSides_xlimit,
        _pointCoupleForLineRegDisplay_Left_Right);

    if (bReport) {
        qvect_yMin_candidates << _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left]._pointWithBiggestX.y();
        qvect_yMax_candidates << _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left]._pointWithBiggestX.y();

        qvect_yMin_candidates << _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right]._pointWithSmallestX.y();
        qvect_yMax_candidates << _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right]._pointWithSmallestX.y();
    }

    if (!bExtendedRangeDueToFlatYMinMax) {
        //search again the ymin ymax
        yGraphMin = *std::min_element(qvect_yMin_candidates.constBegin(), qvect_yMin_candidates.constEnd());
        yGraphMax = *std::max_element(qvect_yMax_candidates.constBegin(), qvect_yMax_candidates.constEnd());
    }

    qDebug() << __FUNCTION__ << "search again the ymin ymax:";

    qDebug() << __FUNCTION__ << " qvect_yMin = " << qvect_yMin_candidates;
    qDebug() << __FUNCTION__ << " qvect_yMax = " << qvect_yMax_candidates;

    qDebug() << __FUNCTION__ << " yGraphMin = " << yGraphMin;
    qDebug() << __FUNCTION__ << " yGraphMax = " << yGraphMax;

    qDebug() << __FUNCTION__ << " xGraphMin = " << xGraphMin;
    qDebug() << __FUNCTION__ << " xGraphMax = " << xGraphMax;

    //use computed y range whenAlone
    setXYRange(xGraphMin, xGraphMax, yGraphMin, yGraphMax);

    //uint countNbValidPoint = 0;

    qDebug()  << __FUNCTION__ << "_computedValuesWithValidityFlag.size()=" << _computedValuesWithValidityFlag.size();

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

            qDebug()  << __FUNCTION__ << "v._anonymMajorMinorWithValidityFlag._bValuesValid is" << v._anonymMajorMinorWithValidityFlag._bValuesValid;

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

        qDebug() << __FUNCTION__  << "3) before #500";
        //direct forward:
        connect(_CGItemDblSidesXAdjWithCompLines_mainMeasure, &CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine::signal_locationInAdjustement,
                this, &CustomGraphicsScene_profil::slot_locationInAdjustement);
        qDebug() << __FUNCTION__  << "3) after #504";

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
