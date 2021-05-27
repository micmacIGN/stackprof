#include <QObject>
#include <QDebug>

#include "../core/leftRight_sideAccess.hpp"
#include "../core/ComputationCore_structures.h"

#include "StackProfilEdit_textWidgetPartModel.hpp"


StackProfilEdit_textWidgetPartModel::StackProfilEdit_textWidgetPartModel(QObject *parent): QObject(parent),
    _bFeed_stackedProfilWithMeasurements(false),
    _eLA_layer(eLA_Invalid),
    _xGraphMin(_invalidNumericValue), //invalid value
    _xGraphMax(-_invalidNumericValue) //invalid value
{
    qDebug() << __FUNCTION__ << "(StackProfilEdit_textWidgetPartModel) call now clear_onMeasure()";
    clear_onMeasure();
}

void StackProfilEdit_textWidgetPartModel::clear() {
    _bFeed_stackedProfilWithMeasurements = false;
    _eLA_layer = eLA_Invalid;;
    _xGraphMin =  _invalidNumericValue;
    _xGraphMax = -_invalidNumericValue;
    _qstrGraphicTitle = "no title";
    _stackedProfilInEdition.clear();
    qDebug() << __FUNCTION__ << "(StackProfilEdit_textWidgetPartModel) call now clear_onMeasure()";
    clear_onMeasure();
}

void StackProfilEdit_textWidgetPartModel::clear_onMeasure() {
    _leftRightSides_linearRegressionModel = {};
    _stackedProfilInEdition._measureIndex.clear();
    _bWarningFlagByUser = false;
    _bFeed_measure = false;
}

bool StackProfilEdit_textWidgetPartModel::feed(e_LayersAcces eLA_layer,
                                               S_StackedProfilWithMeasurements* stackedProfilWithMeasurementsPtr,
                                               qreal xGraphMin, qreal xGraphMax,
                                               const QString& qstrGraphicTitle) {

    clear();

    if (!stackedProfilWithMeasurementsPtr) {
        qDebug() << __FUNCTION__ << " (StackProfilEdit_textWidgetPartModel) error: _stackedProfilWithMeasurementsPtr is nullptr";
        return(false);
    }

    if (!(stackedProfilWithMeasurementsPtr->_LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._bValid)) {
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: _x0ForYOffsetComputation not set";
        return(false);
    }

    _eLA_layer = eLA_layer;
    _xGraphMin = xGraphMin;
    _xGraphMax = xGraphMax;
    _qstrGraphicTitle = qstrGraphicTitle;
    _stackedProfilInEdition._stackedProfilWithMeasurementsPtr = stackedProfilWithMeasurementsPtr;
    _bFeed_stackedProfilWithMeasurements = true;

    qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) _stackedProfilWithMeasurementsPtr @ " << (void*)_stackedProfilInEdition._stackedProfilWithMeasurementsPtr;

    qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) feed done!";
    return(true);
}


bool StackProfilEdit_textWidgetPartModel::setOnMeasure(const S_MeasureIndex& measureIndex/*bool bOnMainMeasure, int secondaryMeasureIdx*/) {

    if (!_bFeed_stackedProfilWithMeasurements) {
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: _bFeed_stackedProfilWithMeasurements is false";
        return(false);
    }

    if (!_stackedProfilInEdition._stackedProfilWithMeasurementsPtr) {
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: _stackedProfilWithMeasurementsPtr is nullptr";
        return(false);
    }

    S_LeftRightSides_linearRegressionModel* leftRightSides_linearRegressionModelPtr =
            (_stackedProfilInEdition._stackedProfilWithMeasurementsPtr)->getPtrOn_LRSidesLinearRegressionModel(measureIndex/*bOnMainMeasure, secondaryMeasureIdx*/);

    if (!leftRightSides_linearRegressionModelPtr) {
        //set default on mainMeasure if something goes wrong (about secondaryMeasureIdx)
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: getting ptr on secondaryMeasureIdx (idx: " << measureIndex._secondaryMeasureIdx << " )";
        qDebug() << __FUNCTION__ << "(StackProfilEdit_textWidgetPartModel) call now clear_onMeasure()";
        clear_onMeasure();
        return(false);
    }

    _leftRightSides_linearRegressionModel = (*leftRightSides_linearRegressionModelPtr);
    _stackedProfilInEdition._measureIndex = measureIndex;
    _bWarningFlagByUser = leftRightSides_linearRegressionModelPtr->_bWarningFlagByUser;

    _bFeed_measure = true;

    emit signal_feed_stackProfilEdit_textWidgets();

    qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) #ici 100";
    return(true);
}


//used to validate a keyboard input value
bool StackProfilEdit_textWidgetPartModel::xPos_isAPossibleValue(
        double xPos,
        e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis,
        e_LRsideAccess eLRSA_adjuster) {

    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " (StackProfilEdit_textWidgetPartModel) error: _bFeed_measure is false";
        return(false);
    }

    bool bIsInsideTheAccordingRange = checkPosition_isInsideTheAccordingRange(xPos, eLRsA_locationRelativeToX0CentralAxis);
    if (!bIsInsideTheAccordingRange) {
        qDebug() << __FUNCTION__ << " (StackProfilEdit_textWidgetPartModel) if (!bIsInsideTheAccordingRange) {";
        return(false);
    }
    return(checkRelativePositionInsideAdjustersCouple(xPos,
                                                      eLRSA_adjuster,
                                                      _leftRightSides_linearRegressionModel.
                                                      _linearRegressionParam_Left_Right[eLRsA_locationRelativeToX0CentralAxis].
                                                      _xRangeForLinearRegressionComputation));
}


bool StackProfilEdit_textWidgetPartModel::checkPosition_isInsideTheAccordingRange(
        double xPos, e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis) {

    //left side adjusters couple
    if (eLRsA_locationRelativeToX0CentralAxis == e_LRsA_left) {
        //min
        if (xPos < _xGraphMin) {
            return(false);
        }

        //max is X0
        if (!_leftRightSides_linearRegressionModel._x0ForYOffsetComputation._bValid)   { return(false); }
        if (xPos > _leftRightSides_linearRegressionModel._x0ForYOffsetComputation._x0) { return(false); }
    }

    //right side adjusters couple
    if (eLRsA_locationRelativeToX0CentralAxis == e_LRsA_right) {

        //min is X0
        if (!_leftRightSides_linearRegressionModel._x0ForYOffsetComputation._bValid)   { return(false); }
        if (xPos < _leftRightSides_linearRegressionModel._x0ForYOffsetComputation._x0) { return(false); }

        //max
        if (xPos > _xGraphMax) {
            return(false);
        }
    }
    return(true);
}

bool StackProfilEdit_textWidgetPartModel::checkRelativePositionInsideAdjustersCouple(
        double xPos, e_LRsideAccess eLRSA_adjuster, const S_XRangeForLinearRegressionComputation& xRangeForLinearRegressionComputation) {
    //
    //common cases of the two sides adjusters couple: about one adjuster relative to the other in the same side:
    //
    //left adjuster can't to close or beyound right adjuster
    if (eLRSA_adjuster == e_LRsA_left) {

        if (!xRangeForLinearRegressionComputation._bValid) {
            qDebug() << __FUNCTION__ << " #left inv";
            return(false);
        }
        if (xPos >= xRangeForLinearRegressionComputation._xMax) {
            qDebug() << __FUNCTION__ << " #left >= max";
            return(false);
        }
    } else { //right adjuster can't to close or beyound left adjuster
        if (!xRangeForLinearRegressionComputation._bValid) {
            qDebug() << __FUNCTION__ << " #right inv";
            return(false);
        }
        if (xPos <= xRangeForLinearRegressionComputation._xMin) {
            qDebug() << __FUNCTION__ << " #left <= min";
            return(false);
        }
    }
    return(true);
}

QString StackProfilEdit_textWidgetPartModel::get_strTitle() {
    return(_qstrGraphicTitle);
}


double StackProfilEdit_textWidgetPartModel::get_xAdjusterPos(
        e_LRsideAccess eLRsA_locationRelativeToX0CentralAxis,
        e_LRsideAccess eLRSA_adjuster) {


    qDebug() << __FUNCTION__ << "eLRsA_locationRelativeToX0CentralAxis : " << eLRsA_locationRelativeToX0CentralAxis << ", eLRSA_adjuster=" << eLRSA_adjuster;

    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(_invalidNumericValue); //invalid value
    }

    if (!_leftRightSides_linearRegressionModel.
         _linearRegressionParam_Left_Right[eLRsA_locationRelativeToX0CentralAxis].
         _xRangeForLinearRegressionComputation._bValid) {
        qDebug() << __FUNCTION__ << " error: _xRangeForLinearRegressionComputation._bValid is false";
        return(_invalidNumericValue); //invalid value
    }

    qDebug() << __FUNCTION__ << " will return a value from xRangeForLinearRegressionComputation";

    if (eLRSA_adjuster == e_LRsA_left) {      
        return(_leftRightSides_linearRegressionModel.
               _linearRegressionParam_Left_Right[eLRsA_locationRelativeToX0CentralAxis].
               _xRangeForLinearRegressionComputation._xMin);
    } //else //e_LRsA_right

    return(_leftRightSides_linearRegressionModel.
           _linearRegressionParam_Left_Right[eLRsA_locationRelativeToX0CentralAxis].
           _xRangeForLinearRegressionComputation._xMax);
}

bool StackProfilEdit_textWidgetPartModel::linearModelIsAvailable() {
    return(_leftRightSides_linearRegressionModel._modelValuesResults._bComputed);
}

double StackProfilEdit_textWidgetPartModel::get_offsetAtX0() {
    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(_invalidNumericValue); //invalid value
    }

    if (!_leftRightSides_linearRegressionModel._modelValuesResults._bComputed) {
        qDebug() << __FUNCTION__ << " error: linearRegressionModel ValuesResults not computed";
        return(_invalidNumericValue); //invalid value
    }

    qDebug() << __FUNCTION__ << " will return a value from _modelValuesResults";

    return(_leftRightSides_linearRegressionModel._modelValuesResults._yOffsetAtX0);
}

double StackProfilEdit_textWidgetPartModel::get_X0() {

    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(_invalidNumericValue); //invalid value
    }

    if (!_leftRightSides_linearRegressionModel._x0ForYOffsetComputation._bValid) {
        qDebug() << __FUNCTION__ << " error: _x0ForYOffsetComputation._bValid is false";
        return(_invalidNumericValue); //invalid value
    }

    qDebug() << __FUNCTION__ << " will return a value from _x0ForYOffsetComputation";

    return(_leftRightSides_linearRegressionModel._x0ForYOffsetComputation._x0);
}


double StackProfilEdit_textWidgetPartModel::get_sigmaSum() {
    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(_invalidNumericValue); //invalid value
    }

    if (!_leftRightSides_linearRegressionModel._modelValuesResults._bComputed) {
        qDebug() << __FUNCTION__ << " error: linearRegressionModel ValuesResults not computed";
        return(_invalidNumericValue); //invalid value
    }

    qDebug() << __FUNCTION__ << " will return a value from _modelValuesResults";

    return(_leftRightSides_linearRegressionModel._modelValuesResults._sigmabSum);
}

bool StackProfilEdit_textWidgetPartModel::validateEditedX0CenterSetPositionValue_evaluatingConsistency/*_andSendForcedAdjustersPositionValuesToFollowConsistency*/(
    const double doubleValueOfQLineEditTextContent_X0PosValue,
    bool& evaluatedConsistencyFlag_X0PosValue) {

    evaluatedConsistencyFlag_X0PosValue = false;

    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << "error: _bFeed is false";
        return(false);
    }

    //X0 position must let a way to left adjusters to use two points (one at left side of X0; and X0)
    if (doubleValueOfQLineEditTextContent_X0PosValue < _xGraphMin + 1.0 ) {
        return(false);
    }

    //X0 position must let a way toright adjusters to use two points (X0; and one at right side of X0)
    if (doubleValueOfQLineEditTextContent_X0PosValue > _xGraphMax - 1.0) {
        return(false);
    }

    evaluatedConsistencyFlag_X0PosValue = true;

    emit signal_X0SetPositionValue_fromTextEdition/*_and_adjustersSetPositionValues_changed*/(_eLA_layer, doubleValueOfQLineEditTextContent_X0PosValue);

    return(true);

    //force the positions of adjusters is its locations are not consistent with the new wanted X0 position
}

bool StackProfilEdit_textWidgetPartModel::validateEditedAdjustersSetPositionValues_evaluatingConsistency(
        const double a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[2][2],
                bool a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[2][2]) {

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] = false;
        }
    }

    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << "error: _bFeed is false";
        return(false);
    }


    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {

            bool bIsInsideTheAccordingRange = checkPosition_isInsideTheAccordingRange(
                        a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side],
                        static_cast<e_LRsideAccess>(i_eLRsA_coupleRelativeToX0_side));

            a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] = bIsInsideTheAccordingRange;

            qDebug() << __FUNCTION__ << "about range: " <<  a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];

        }
    }

    S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2];

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {

        xRangeForLinearRegressionComputation_left_right_side[i_eLRsA_coupleRelativeToX0_side].set(
            a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][e_LRsA_left],
            a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][e_LRsA_right]);

        qDebug() << __FUNCTION__ << "about relative pos:  xRangeForLinearRegressionComputation_left_right_side[ " << i_eLRsA_coupleRelativeToX0_side << " ]"
                 <<  xRangeForLinearRegressionComputation_left_right_side[i_eLRsA_coupleRelativeToX0_side]._xMin << " , "
                 <<  xRangeForLinearRegressionComputation_left_right_side[i_eLRsA_coupleRelativeToX0_side]._xMax;

        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            bool bCheckRelativePositionInsideAdjustersCouple = checkRelativePositionInsideAdjustersCouple(
                a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side],
                 static_cast<e_LRsideAccess>(i_eLRsA_insideCouple_side), xRangeForLinearRegressionComputation_left_right_side[i_eLRsA_coupleRelativeToX0_side]);

            a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] &= bCheckRelativePositionInsideAdjustersCouple;

            qDebug() << __FUNCTION__ << "about relative pos: " <<  a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];

        }
    }

    //bool bSomeAtFalse = false;
    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            if  (!a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side]) {

                return(false);
            }
        }
    }

    emit signal_adjustersSetPositionValues_changed_fromTextEdition(_eLA_layer, xRangeForLinearRegressionComputation_left_right_side);

    return(true);
}

bool StackProfilEdit_textWidgetPartModel::get_warningFlagByUser(bool& bWarningFlagByUser) {
    bWarningFlagByUser = false;
    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(false); //invalid value
    }

    bWarningFlagByUser = _bWarningFlagByUser;
    return(true);
}

bool StackProfilEdit_textWidgetPartModel::set_warningFlagByUser_UiModelFromUi(bool bWarningFlagByUser, bool bAlternativeModeActivated) {
    if (!_bFeed_measure) {
        qDebug() << __FUNCTION__ << " error: _bFeed_measure is false";
        return(false); //invalid value
    }

    qDebug() << __FUNCTION__ << " bAlternativeModeActivated:" << bAlternativeModeActivated;

    //_bWarnFlag = bWarnFlag;
    emit signal_setWarnFlag_fromTextEdition(_eLA_layer, bWarningFlagByUser, bAlternativeModeActivated);

    return(true);
}

bool StackProfilEdit_textWidgetPartModel::set_warningFlagByUser(/*const S_MeasureIndex& measureIndex,*/bool bWarningFlagByUser) {

    if (!_bFeed_stackedProfilWithMeasurements) {
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: _bFeed_stackedProfilWithMeasurements is false";
        return(false);
    }

    if (!_stackedProfilInEdition._stackedProfilWithMeasurementsPtr) {
        qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) error: _stackedProfilWithMeasurementsPtr is nullptr";
        return(false);
    }

    if (!_bFeed_measure) {
        return(false);
    }

    //_bFeed_measure = true;
    _bWarningFlagByUser = bWarningFlagByUser;
    qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) _bWarningFlagByUser <- " << _bWarningFlagByUser;

    emit signal_setWarnFlag_forUi(_eLA_layer, _bWarningFlagByUser);

    qDebug() << __FUNCTION__  << " (StackProfilEdit_textWidgetPartModel) #ici 425";
    return(true);
}

