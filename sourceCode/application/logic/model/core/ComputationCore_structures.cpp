#include <QJsonArray>

#include <QDebug>

#include "ComputationCore_structures.h"

#include "leftRight_sideAccess.hpp"

#include "../../toolbox/toolbox_json.h"
#include "../../toolbox/toolbox_conversion.h"
#include "../../toolbox/toolbox_pathAndFile.h"
#include "../../toolbox/toolbox_math.h"
#include "../../toolbox/toolbox_math_geometry.h"

#include "../../io/InputImageFormatChecker.h"

//#TagToFindAboutReleaseDeliveryOption
//#define debug_showContent_S_ProfilsBoxParameters
#ifndef DONT_USE_QDEBUG__USE_COUT_INSTEAD
#define useQDebug
#endif
#ifdef useQDebug
    #include <QDebug>
    #define debugOut qDebug()
    #define debugEndl ""
#else
    #define debugOut cout
    #define debugEndl endl;
#endif

void S_CoupleOfQPointF_withValidityFlag::clear() {
    _pointWithSmallestX = {.0,.0};
    _pointWithBiggestX  = {.0,.0};
    _bValid = false;
}

void U_CorrelationScoreMapFileUseFlags::clear() {
    _sCSF_AllIndependant_PX1_PX2_DeltaZ = {false, false, false};
}

void U_CorrelationScoreMapFileUseFlags::showContent() {
    qDebug() << __FUNCTION__ << "(U_CorrelationScoreMapFileUseFlags) _b_onPX1PX2 = " << _sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2;
    qDebug() << __FUNCTION__ << "(U_CorrelationScoreMapFileUseFlags) _b_onDeltaZ = " << _sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ;
}

void U_MeanMedian_computedValuesWithValidityFlag::clear() {
    _anonymMajorMinorWithValidityFlag = {.0, .0, false };
}

U_MeanMedian_computedValuesWithValidityFlag::U_MeanMedian_computedValuesWithValidityFlag() {
    qDebug() << __FUNCTION__;
    clear();
}

S_ProfilsBoxParametersForRelativeMoveAlongRoute::S_ProfilsBoxParametersForRelativeMoveAlongRoute() {
    _shiftFromInitialLocation = 0;
    _bPossible= false;
    _profilsBoxParameters = {};
}

S_ComputationParameters::S_ComputationParameters():
    _ePixelExtractionMethod(e_PEM_notSet),
    _eBaseComputationMethod(e_BCM_notSet) {
}

S_CorrelationScoreMapParameters::S_CorrelationScoreMapParameters():
    _bUse(false),
    _thresholdRejection {false, .0f},
    _weighting {false, 2.0f} {
}

void S_CorrelationScoreMapParameters::setDefaultValues() {
    _bUse = {false};
    _thresholdRejection = {false, .0f};
    _weighting = {false, 2.0f};
}


void S_CorrelationScoreMapParameters::show(const QString& strTitle) const {
    qDebug() << __FUNCTION__  << "-------------";
    qDebug() << __FUNCTION__  << "(S_CorrelationScoreMapParameters) " << strTitle;
    qDebug() << __FUNCTION__  << "---";
    qDebug() << __FUNCTION__  << "_bUse = " << _bUse;
    qDebug() << __FUNCTION__  << "---";
    qDebug() << __FUNCTION__  << "thresholdRejection: bUse ; _f_rejectIfBelowValue:";
    qDebug() << __FUNCTION__  << _thresholdRejection._bUse << "; " << _thresholdRejection._f_rejectIfBelowValue;
    qDebug() << __FUNCTION__  << "weighting           bUse ; weightExponent:";
    qDebug() << __FUNCTION__  << _weighting._bUse << "; " << _weighting._f_weightExponent;
    qDebug() << __FUNCTION__  << "-------------";
}


bool S_CorrelationScoreMapParameters::isEqualTo_withTolerance(const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters) {

    bool bSameUseFlag = (_bUse == sCorrelationScoreMapParameters._bUse);
    if (!bSameUseFlag) {
        return(false);
    }

    bool bSameUseFlagThresholdRejection = (_thresholdRejection._bUse == sCorrelationScoreMapParameters._thresholdRejection._bUse);
    bool bSameUseFlagWeighting          = (_weighting._bUse          == sCorrelationScoreMapParameters._weighting._bUse);

    if ((!bSameUseFlagThresholdRejection)||(!bSameUseFlagWeighting)) {
        return(false);
    }

    if (_thresholdRejection._bUse) {
        bool bThresholdRejectionsEvaluatedAsSame  = doubleValuesAreClose(
                    _thresholdRejection._f_rejectIfBelowValue,
                    sCorrelationScoreMapParameters._thresholdRejection._f_rejectIfBelowValue,
                    0.001);
        if (!bThresholdRejectionsEvaluatedAsSame) {
            return(false);
        }
    }

    if (_weighting._bUse) {
        bool bWeightingsEvaluatedAsSame = doubleValuesAreClose(
                    _weighting._f_weightExponent,
                    sCorrelationScoreMapParameters._weighting._f_weightExponent,
                    0.1);
        if (!bWeightingsEvaluatedAsSame) {
            return(false);
        }
    }
    return(true);
}




//@LP be aware: default value for _f_rejectIfBelow = 0.70;

void S_ComputationParameters::clear() {
    _ePixelExtractionMethod = e_PEM_notSet;
    _eBaseComputationMethod = e_BCM_notSet;
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX1].setDefaultValues();
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX2].setDefaultValues();
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_deltaZ].setDefaultValues();
}

void S_ComputationParameters::debugShow() const {

    qDebug() << __FUNCTION__ << "(S_ComputationParameters) _ePixelExtractionMethod = " << _ePixelExtractionMethod;
    qDebug() << __FUNCTION__ << "(S_ComputationParameters) _eBaseComputationMethod = " << _eBaseComputationMethod;
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX1].show("PX1");
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX2].show("PX2");
    _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_deltaZ].show("deltaZ");
}

bool S_ComputationParameters::isEqualTo_withTolerance(const S_ComputationParameters& sComputationParameters, const QVector<bool>& qvectBool_usedLayer) {

    if (qvectBool_usedLayer.count() != 3) {
        return(false);
    }
    if (!qvectBool_usedLayer.count(true)) {
        return(false);
    }

    bool bSame = true;

    bSame &= (_ePixelExtractionMethod == sComputationParameters._ePixelExtractionMethod);
    bSame &= (_eBaseComputationMethod == sComputationParameters._eBaseComputationMethod);

    if (!bSame) {
        return(false);
    }

    for(int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        if (qvectBool_usedLayer[ieLA]) {
            bSame &= _correlationScoreMapParameters_PX1_PX2_DeltaZ[ieLA].
                     isEqualTo_withTolerance(sComputationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[ieLA]);
        }
    }

    return(bSame);
}





bool S_CorrelationScoreMapParameters::toQJson(QJsonObject& qjsonObj) const {

    qjsonObj.insert("bUse", _bUse);

    QJsonObject QJsonObj_thresholdRejection;
    QJsonObj_thresholdRejection.insert("bUse",  _thresholdRejection._bUse);

    qreal thresholdRejection_FixedPrecision = .0;
    qrealToRealNumberFixedPrecision_f(static_cast<double>(_thresholdRejection._f_rejectIfBelowValue), 3, thresholdRejection_FixedPrecision);
    QJsonObj_thresholdRejection.insert("rejectValueIfBelow", thresholdRejection_FixedPrecision);

    QJsonObject QJsonObj_weighting;
    QJsonObj_weighting.insert("bUse", _weighting._bUse);
    QJsonObj_weighting.insert("weightExponent", (static_cast<int>(_weighting._f_weightExponent)));

    qjsonObj.insert("option_thresholdRejection", QJsonObj_thresholdRejection);
    qjsonObj.insert("option_weighting", QJsonObj_weighting);

    return(true);
}

bool S_CorrelationScoreMapParameters::fromQJson(const QJsonObject qjsonObj) {

    qDebug() << __FUNCTION__ << "----------";
    qDebug() << __FUNCTION__ << qjsonObj;
    qDebug() << __FUNCTION__ << "----------";

    QString strKey_bUse {"bUse"};

    //main bUse flag
    if (!qjsonObj.contains(strKey_bUse)) {
        return(false);
    }
    _bUse = qjsonObj.value(strKey_bUse).toBool();

    QString strKey_thresholdRejection {"option_thresholdRejection"};
    if (!qjsonObj.contains(strKey_thresholdRejection)) {
        return(false);
    }
    QJsonObject qjsonObj_thresholdRejection = qjsonObj.value(strKey_thresholdRejection).toObject();

    bool bUseTGot = getBoolValueFromJson(qjsonObj_thresholdRejection, strKey_bUse, _thresholdRejection._bUse);
    double df_rejectIfBelow = .0;
    bool bRejectValueIfBelowGot = getDoubleValueFromJson(qjsonObj_thresholdRejection, "rejectValueIfBelow", df_rejectIfBelow);
    if (  (!bUseTGot)
        ||(!bRejectValueIfBelowGot)) {
        return(false);
    }    

    if (  (df_rejectIfBelow < .0)
        ||(df_rejectIfBelow > 999.999)) {
        return(false);
    }

    bool bIsNotStringNumericZeroValue = false;
    if (!isConformToFormatPrecision_zeroValueIsAccepted(df_rejectIfBelow, 3, 5, bIsNotStringNumericZeroValue)) {
        return(false);
    }

    if (bIsNotStringNumericZeroValue) {
        _thresholdRejection._f_rejectIfBelowValue = static_cast<float>(df_rejectIfBelow);
    } else {
        //to avoid to have -0 if this is the json double value
        _thresholdRejection._f_rejectIfBelowValue = .0;
    }

    QString strKey_weighting {"option_weighting"};
    if (!qjsonObj.contains(strKey_weighting)) {
        return(false);
    }
    QJsonObject qjsonObj_weighting = qjsonObj.value(strKey_weighting).toObject();

    bool bUseWGot = getBoolValueFromJson(qjsonObj_weighting, strKey_bUse, _weighting._bUse);
    int iWeightExponent = 1;
    bool bWeightExponentGot = getIntValueFromJson(qjsonObj_weighting, "weightExponent", iWeightExponent);
    if (  (!bUseWGot)
        ||(!bWeightExponentGot)) {
        return(false);
    }

    if (  (iWeightExponent < 2)
        ||(iWeightExponent > 6))  {
        return(false);
    }
    _weighting._f_weightExponent = static_cast<float>(iWeightExponent);

    return(true);
}


S_StackedProfilWithMeasurements::S_StackedProfilWithMeasurements() {
    _qvectqvect_aboutMeanMedian.clear();
    _qvectqvect_aboutMeanMedian.resize(e_MMIA_count);

}


S_ProfilsBoxParameters::S_ProfilsBoxParameters():
    _sUniqueBoxID {-1, -1},
    _sInfoAboutAutomaticDistribution {e_DBSBuDA_notSet},
    _qpfCenterPoint {.0,.0},
    _unitVectorDirection {.0,.0},
    _oddPixelWidth { 1 }, //
    _oddPixelLength { 3 },
    _idxSegmentOwnerOfCenterPoint {-1},
    _bDevDebug_wasAdjusted {false},
    _bDevDebug_tooCloseBox_rejected_needGoFurther{false},
    _bDevDebug_tooFarBox_rejected_needGoCloser{false} {

}

S_ProfilsBoxParameters::S_ProfilsBoxParameters(
    const S_UniqueBoxID& sUniqueBoxID,
    const S_infoAboutAutomaticDistribution& sInfoAboutAutomaticDistribution,
    const QPointF& qpfCenterPoint,
    const QPointF& unitVectorDirection,
    int oddPixelWidth,
    int oddPixelLength,
    int idxSegmentOwnerOfCenterPoint,
    bool bDevDebug_wasAdjusted,
    bool bDevDebug_tooCloseBox_rejected_needGoFurther,
    bool bDevDebug_tooFarBox_rejected_needGoCloser):

    _sUniqueBoxID { sUniqueBoxID },
    _sInfoAboutAutomaticDistribution { sInfoAboutAutomaticDistribution },
    _qpfCenterPoint { qpfCenterPoint },
    _unitVectorDirection { unitVectorDirection },
    _oddPixelWidth { oddPixelWidth }, //
    _oddPixelLength { oddPixelLength },
    _idxSegmentOwnerOfCenterPoint { idxSegmentOwnerOfCenterPoint },
    _bDevDebug_wasAdjusted { bDevDebug_wasAdjusted },
    _bDevDebug_tooCloseBox_rejected_needGoFurther { bDevDebug_tooCloseBox_rejected_needGoFurther },
    _bDevDebug_tooFarBox_rejected_needGoCloser { bDevDebug_tooFarBox_rejected_needGoCloser } {

}

void S_ProfilsBoxParameters::showContent() const {
//@#LP fix debug way to show or not depending of debug contect
#ifdef debug_showContent_S_ProfilsBoxParameters
    //unsigned int _id;
    debugOut << "(S_ProfilsBoxParameters)::" << __FUNCTION__ << debugEndl;
    debugOut << "  _sUniqueBoxID._setID       = " << _sUniqueBoxID._setID<< debugEndl;
    debugOut << "  _sUniqueBoxID._idxBoxInSet = " << _sUniqueBoxID._idxBoxInSet<< debugEndl;
    debugOut << "  _"<< debugEndl;
#ifndef useQDebug
        debugOut << "  _qpfCenterPoint = "      << std::setprecision(14) << _qpfCenterPoint.x() << " , " <<  _qpfCenterPoint.y() << debugEndl;
        debugOut << "  _unitVectorDirection = " << std::setprecision(14) << _unitVectorDirection.x() << " , " <<   _unitVectorDirection.y() << debugEndl;
#else
    debugOut << "  _qpfCenterPoint = "      << _qpfCenterPoint.x() << " , " <<  _qpfCenterPoint.y() << debugEndl;
    debugOut << "  _unitVectorDirection = " << _unitVectorDirection.x() << " , " <<   _unitVectorDirection.y() << debugEndl;
#endif

    debugOut << "  _"<< debugEndl;
    debugOut << "  _oddPixelWidth = " << _oddPixelWidth << debugEndl;
    debugOut << "  _oddPixelLength = " << _oddPixelLength << debugEndl;
    debugOut << "  _"<< debugEndl;
    debugOut << "  _idxSegmentOwnerOfCenterPoint = " << _idxSegmentOwnerOfCenterPoint << debugEndl;
    debugOut << "  _"<< debugEndl;
    //@#LP dev-debug purpose flags:
    debugOut << "  _bDevDebug_wasAdjusted = " << _bDevDebug_wasAdjusted << debugEndl;
    debugOut << "  _bDevDebug_tooCloseBox_rejected_needGoFurther = " << _bDevDebug_tooCloseBox_rejected_needGoFurther << debugEndl;
    debugOut << "  _bDevDebug_tooFarBox_rejected_needGoCloser = " << _bDevDebug_tooFarBox_rejected_needGoCloser << debugEndl;
#endif
}

void S_ProfilsBoxParameters::showContent_centerPointAndIdxSegmentOnly() const {
//@#LP fix debug way to show or not depending of debug contect
#ifdef debug_showContent_S_ProfilsBoxParameters
    debugOut << "(S_ProfilsBoxParameters)::" << __FUNCTION__ << debugEndl;
#ifndef useQDebug
    debugOut << "  _qpfCenterPoint = "      << std::setprecision(14) << _qpfCenterPoint.x() << " , " <<  _qpfCenterPoint.y() << debugEndl;
#else
    debugOut << "  _qpfCenterPoint = "      << _qpfCenterPoint.x() << " , " <<  _qpfCenterPoint.y() << debugEndl;
#endif
    debugOut << "  _idxSegmentOwnerOfCenterPoint = " << _idxSegmentOwnerOfCenterPoint << debugEndl;
#endif
}


//we don't save in json file the internal id of boxes. We just save the idx of the box in the qvect
//An at loading, we will build internals id for loaded boxes, with the same principle than when using automatic distribution)
//this permits to make a difference between boxes from the json and the other added boxes later
bool S_ProfilsBoxParameters::toQJsonObject(int idBox, QJsonObject &qJsonObjOut) const {

    bool bReport = true;

    QJsonArray qjsonArray_centerPoint;
    bReport &= qrealPoint_toQJsonArray(_qpfCenterPoint.x(), _qpfCenterPoint.y(), 7, qjsonArray_centerPoint);

    QJsonArray qjsonArray_unitVectorDirection;
    bReport &= qrealPoint_toQJsonArray(_unitVectorDirection.x(), _unitVectorDirection.y(), 7, qjsonArray_unitVectorDirection);

    if (!bReport) {
        return(false);
    }

    QString qstrKey_boxId {"boxId"};
    qJsonObjOut.insert(qstrKey_boxId, idBox);

    QString qstrKey_centerpoint {"centerPoint"};
    qJsonObjOut.insert(qstrKey_centerpoint, qjsonArray_centerPoint);

    //@LP: normalized vector direction
    //@#LP at loading, be sure to normalize this if some boxes were edited by hand without normalization
    QString qstrKey_unitVectorDirection {"unitVectorDirection"};
    qJsonObjOut.insert(qstrKey_unitVectorDirection, qjsonArray_unitVectorDirection);


    QString qstrKey_oddPixelWidth {"oddPixelWidth"};
    qJsonObjOut.insert(qstrKey_oddPixelWidth, _oddPixelWidth);


    QString qstrKey_oddPixelLength {"oddPixelLength"};
    qJsonObjOut.insert(qstrKey_oddPixelLength, _oddPixelLength);

    //@LP possible futur feature (not implemented): this value could '-1' if the box was inserted by hand in the file and defined as unmovable along the trace.
    QString qstrKey_idxSegmentOwnerOfCenterPoint {"idxSegmentOwnerOfCenterPoint"};
    qJsonObjOut.insert(qstrKey_idxSegmentOwnerOfCenterPoint, _idxSegmentOwnerOfCenterPoint);


    //ieDistanceBSBUAD
    //- optionnal json value: will be present typically when using DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution
    //- doest not exist typically when the box is human edited in the json project file
    //We don't recompute this field at loading. Hence we need to save it to able to warn the user if
    //- some boxes automatically distributed from this loaded json file contains boxes with AutomaticDistribution using DistanceBetweenSuccessivesBoxes
    //- the biLineearInterpolation2x2 is set and not compatible with the distanceBetweenSuccessivesBoxes
    //none chek is made on the other boxes located in a different manners
    /*if (_sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid != e_DBSBuDA_notSet) {
        QString qstrKey_ieDistanceBSBUAD {"ieDistanceBSBUAD"};
        qJsonObjOut.insert(qstrKey_ieDistanceBSBUAD, static_cast<int>(_sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid));
    }*/

    return(true);

}

bool S_ProfilsBoxParameters::fromQJsonObject(const QJsonObject &qJsonObj) {

    //checking that:
    //- all (key, value) pairs exist
    //- boxId >= 0
    //- oddPixelLength and oddPixelWidth are odd
    //- oddPixelLength >= 3
    //- oddPixelWidth >= 3
    //- normalize unitVectorDirection if not normalized (usefull in case of human file edition)
    //- idxSegmentOwnerOfCenterPoint exist or not.
    //   . if does not exist => set it to -1, indicating that the box will be unmovable (human edition)
    //   . if exist check that's its value is >= -1

    //boxId
    int boxId = -1;
    bool bBoxIdGot = getIntValueFromJson(qJsonObj, "boxId", boxId);
    if (!bBoxIdGot) {
        return(false);
    }
    if (boxId < 0) {
        return(false);
    }

    //oddPixelLength and oddPixelWidth
    int oddPixelLength = 1;
    bool bOddPixelLengthGot = getIntValueFromJson(qJsonObj, "oddPixelLength", oddPixelLength);

    int oddPixelWidth = 1;
    bool bOddPixelWidthGot = getIntValueFromJson(qJsonObj, "oddPixelWidth", oddPixelWidth);

    if (  (!bOddPixelLengthGot)
        ||(!bOddPixelWidthGot)) {
        return(false);
    }

    bool bLengthIsEven = bCheckDoubleIsEven(oddPixelLength);
    bool bLength_equalOrGreaterThanThree = (oddPixelLength >= 3.0);

    bool bWidthIsEven = bCheckDoubleIsEven(oddPixelWidth);
    bool bEvenWidth_equalOrGreaterThanOne = (oddPixelWidth >= 3.0);

    if (bLengthIsEven || bWidthIsEven) {
        return(false);
    }

    if (  (!bLength_equalOrGreaterThanThree)
        ||(!bEvenWidth_equalOrGreaterThanOne)) {
        return(false);
    }

    //unitVectorDirection:
    QString strKey_unitVectorDirection {"unitVectorDirection"};
    if (!qJsonObj.contains(strKey_unitVectorDirection)) {
        qDebug() << __FUNCTION__ << ": error: key not found:" << strKey_unitVectorDirection;
        return(false);
    }
    QJsonArray qJsonArrayUnitVectorDirectionXY = qJsonObj.value(strKey_unitVectorDirection).toArray();
    if (qJsonArrayUnitVectorDirectionXY.size() != 2) {
        qDebug() << __FUNCTION__ << ": error: qJsonArrayUnitVectorDirectionXY.size() != 2";
        return(false);
    }

    QPointF qpfUnitVectorDirection(qJsonArrayUnitVectorDirectionXY[0].toDouble(),
                                   qJsonArrayUnitVectorDirectionXY[1].toDouble());

#ifdef debug_showContent_S_ProfilsBoxParameters
    debugOut << __FUNCTION__ << "qpfUnitVectorDirection = qJsonArrayUnitVectorDirectionXY[...].toDouble:"
    #ifndef useQDebug
         <<  std::setprecision(14)
    #endif
          << qpfUnitVectorDirection.x() << ", "
          << qpfUnitVectorDirection.y() << debugEndl;
#endif

    qpfUnitVectorDirection = normalizeQPointFUnitVectorIfNeed(qpfUnitVectorDirection);

#ifdef debug_showContent_S_ProfilsBoxParameters
    debugOut << __FUNCTION__ << "qpfUnitVectorDirection after normalizeQPointFUnitVectorIfNeed:"
    #ifndef useQDebug
         <<  std::setprecision(14)
    #endif
          << qpfUnitVectorDirection.x() << ", "
          << qpfUnitVectorDirection.y() << debugEndl;
#endif

    //idxSegmentOwnerOfCenterPoint

    //@#LP :
    //possible usage using -1 as IdxSegmentOwnerOfCenterPointGot:
    //It can permit to handle the case of 'orphan box' feature: box which can be located but not movable
    //(use case of box located by the user, editing a project file by hand for example)
    int idxSegmentOwnerOfCenterPoint = -1;
    QString strKey_idxSegmentOwnerOfCenterPoint {"idxSegmentOwnerOfCenterPoint"};
    if (qJsonObj.contains(strKey_idxSegmentOwnerOfCenterPoint)) {
        bool bIdxSegmentOwnerOfCenterPointGot = getIntValueFromJson(qJsonObj,strKey_idxSegmentOwnerOfCenterPoint, idxSegmentOwnerOfCenterPoint);
        if (!bIdxSegmentOwnerOfCenterPointGot) {
            qDebug() << __FUNCTION__ << ": error: getting " << strKey_idxSegmentOwnerOfCenterPoint;
            return(false);
        }
        //if (idxSegmentOwnerOfCenterPoint < -1) { //@#LP 'orphan box' feature not used for now
        if (idxSegmentOwnerOfCenterPoint < 0) {
            qDebug() << __FUNCTION__ << ": error: invalid idxSegmentOwnerOfCenterPoint";
            return(false);
        }
    }


    //centerPoint
    QString strKey_centerPoint {"centerPoint"};
    if (!qJsonObj.contains(strKey_centerPoint)) {
        qDebug() << __FUNCTION__ << ": error: key not found:" << strKey_centerPoint;
        return(false);
    }
    QJsonArray qJsonArrayCenterPointXY = qJsonObj.value(strKey_centerPoint).toArray();
    if (qJsonArrayCenterPointXY.size() != 2) {
        qDebug() << __FUNCTION__ << ": error: qJsonArrayCenterPointXY.size() != 2";
        return(false);
    }
    QPointF qpfcenterPoint(qJsonArrayCenterPointXY[0].toDouble(),
                           qJsonArrayCenterPointXY[1].toDouble());

#ifdef debug_showContent_S_ProfilsBoxParameters
    debugOut << __FUNCTION__ << "qpfcenterPoint = qJsonArrayCenterPointXY[...].toDouble:"
    #ifndef useQDebug
         <<  std::setprecision(14)
    #endif
          << qpfcenterPoint.x() << ", "
          << qpfcenterPoint.y() << debugEndl;
#endif

    //ieDistanceBSBUAD
    //- optionnal json value: will be present typically when using DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution
    //- doest not exist typically when the box is human edited in the json project file
    e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDistanceBSBUAD =
            e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution::e_DBSBuDA_notSet;
    QString qstrKey_ieDistanceBSBUAD {"ieDistanceBSBUAD"};
    if (qJsonObj.contains(qstrKey_ieDistanceBSBUAD)) {
       int ieDistanceBSBUAD = e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution::e_DBSBuDA_notSet;
       bool bieDistanceBSBUADGot = getIntValueFromJson(qJsonObj, qstrKey_ieDistanceBSBUAD, ieDistanceBSBUAD);
       if (!bieDistanceBSBUADGot) {
           qDebug() << __FUNCTION__ << ": error: getting " << qstrKey_ieDistanceBSBUAD;
           return(false);
       }
       eDistanceBSBUAD = projectJsonInt_to_e_DistanceBetweenSuccessivesBoxesUAD(ieDistanceBSBUAD);
    }

    //set all the fields except _sUniqueBoxID._setID
    _sUniqueBoxID._idxBoxInSet = boxId;
    _sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid = eDistanceBSBUAD;

    //S_qpf_point_and_unitVectorDirection
    _qpfCenterPoint = qpfcenterPoint;
    _unitVectorDirection = qpfUnitVectorDirection;

    _oddPixelWidth  = oddPixelWidth;
    _oddPixelLength = oddPixelLength;

    _idxSegmentOwnerOfCenterPoint = idxSegmentOwnerOfCenterPoint;



    //@#LP dev-debug purpose flags:
    _bDevDebug_wasAdjusted = false;
    _bDevDebug_tooCloseBox_rejected_needGoFurther = false;
    _bDevDebug_tooFarBox_rejected_needGoCloser = false;

    //at outside, check and set this:
    //- if idxSegmentOwnerOfCenterPoint != -1:
    //   . idxSegmentOwnerOfCenterPoint matches with an existing route segment
    //   . set again the centerPoint using centerPoint as base location and idxSegmentOwnerOfCenterPoint
    //- at least one CenterPoint from the boxes list inside the image

    return(true);
}




S_LeftRightSides_linearRegressionModel* S_StackedProfilWithMeasurements::getPtrOn_LRSidesLinearRegressionModel(const S_MeasureIndex& measureIndex) {
                                                                                                             //bool bOnMainMeasure, int secondaryMeasureIdx) {

    if (measureIndex._bOnMainMeasure) {
        qDebug() << __FUNCTION__ << " if (measureIndex._bOnMainMeasure) { => return @ " << (void*)&_LRSide_linRegrModel_mainMeasure;
        return(&_LRSide_linRegrModel_mainMeasure);
    }

    if (  (measureIndex._secondaryMeasureIdx < 0)
        ||(measureIndex._secondaryMeasureIdx >= _LRSide_linRegrModel_secondaryMeasures.count())) {
        qDebug() << __FUNCTION__ << " invalid _secondaryMeasureIdx => return nullptr";
        return(nullptr);
    }

    qDebug() << __FUNCTION__ << " return _secondaryMeasureIdx @ " << (void*)(&(_LRSide_linRegrModel_secondaryMeasures[measureIndex._secondaryMeasureIdx]));

    return(&(_LRSide_linRegrModel_secondaryMeasures[measureIndex._secondaryMeasureIdx]));
}

void S_StackedProfilWithMeasurements::clear_dataContent() {
    for (auto& iterV: _qvectqvect_aboutMeanMedian) {
        qDebug() << __FUNCTION__ << " call iterV.clear()";
        iterV.clear();
    }
}

bool S_StackedProfilWithMeasurements::set_LRSidesLinearRegressionModel(
        const S_MeasureIndex& measureIndex,
        const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) {

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr =
            getPtrOn_LRSidesLinearRegressionModel(measureIndex);

    if (!leftRightSides_linearRegressionModelPtr) {
        qDebug() << __FUNCTION__ << "error: leftRightSides_linearRegressionModelPtr is nullptr";
        return(false);
    }

    bool bNeedToBeComputed[2] = { false, false };

    for (int ieLRSA = e_LRsA_left; ieLRSA <= e_LRsA_right; ieLRSA++) {

        if (!leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[ieLRSA]._xRangeForLinearRegressionComputation._bValid) {
            bNeedToBeComputed[ieLRSA] = true;
            qDebug() << __FUNCTION__ << "#1 bNeedToBeComputed[" << ieLRSA << "] set to true";
        }

        if (  static_cast<int>(leftRightSides_linearRegressionModelPtr->
                               _linearRegressionParam_Left_Right[ieLRSA].
                               _xRangeForLinearRegressionComputation._xMin)
            != static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[ieLRSA]._xMin)) {

            bNeedToBeComputed[ieLRSA] = true;

            qDebug() << __FUNCTION__ << "#2 bNeedToBeComputed[" << ieLRSA << "] set to true";
        }

        if (  static_cast<int>(leftRightSides_linearRegressionModelPtr->
                               _linearRegressionParam_Left_Right[ieLRSA].
                               _xRangeForLinearRegressionComputation._xMax)
            != static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[ieLRSA]._xMax)) {

            bNeedToBeComputed[ieLRSA] = true;

            qDebug() << __FUNCTION__ << "#3 bNeedToBeComputed[" << ieLRSA << "] set to true";
        }

        if (bNeedToBeComputed[ieLRSA]) {

            leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[ieLRSA].set_xRange(
                        xRangeForLinearRegressionComputation_left_right_side[ieLRSA]._xMin,
                        xRangeForLinearRegressionComputation_left_right_side[ieLRSA]._xMax);



            leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[ieLRSA]._mathComp_LinearRegressionParameters.clear();
        }
    }

    if (  bNeedToBeComputed[e_LRsA_left]
        ||bNeedToBeComputed[e_LRsA_right]) {
        leftRightSides_linearRegressionModelPtr->clearResults();
        qDebug() << __FUNCTION__ << "#44 bNeedToBeComputed => leftRightSides_linearRegressionModelPtr->clearResults();";
        return(true);
    }

    return(false);
}

bool S_StackedProfilWithMeasurements::set_X0_forLinearRegressionModel(const S_MeasureIndex& measureIndex, const double& X0Pos) {

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr =
            getPtrOn_LRSidesLinearRegressionModel(measureIndex);

    if (!leftRightSides_linearRegressionModelPtr) {
        qDebug() << __FUNCTION__ << "error: leftRightSides_linearRegressionModelPtr is nullptr";
        return(false);
    }

    bool bNeedToBeComputed = false;

    if (!leftRightSides_linearRegressionModelPtr->_x0ForYOffsetComputation._bValid) {
        bNeedToBeComputed = true;
        qDebug() << __FUNCTION__ << "#1 bNeedToBeComputed set to true";
    }

    if (static_cast<int>(leftRightSides_linearRegressionModelPtr->_x0ForYOffsetComputation._x0) != static_cast<int>(X0Pos)) {
        bNeedToBeComputed = true;
        qDebug() << __FUNCTION__ << "#2 bNeedToBeComputed set to true";
    }

    if (!bNeedToBeComputed) {
        qDebug() << __FUNCTION__ << "(#3) if (!bNeedToBeComputed) {";
        return(false);
    }

    leftRightSides_linearRegressionModelPtr->_x0ForYOffsetComputation.set(X0Pos);
    leftRightSides_linearRegressionModelPtr->clearResults();

    leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[e_LRsA_left ]._mathComp_LinearRegressionParameters.clear();
    leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[e_LRsA_right]._mathComp_LinearRegressionParameters.clear();

    qDebug() << __FUNCTION__ << "#4_ ---------------- ";
    leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[e_LRsA_left  ].showContent();
    leftRightSides_linearRegressionModelPtr->_linearRegressionParam_Left_Right[e_LRsA_right ].showContent();
    qDebug() << __FUNCTION__ << "#_4 ---------------- ";

    qDebug() << __FUNCTION__ << "#44 bNeedToBeComputed => leftRightSides_linearRegressionModelPtr->clearResults();";
    return(true);
}

bool S_StackedProfilWithMeasurements::set_warnFlag(const S_MeasureIndex& measureIndex, bool bWarnFlag) {

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr =
            getPtrOn_LRSidesLinearRegressionModel(measureIndex);

    qDebug() << __FUNCTION__ << "leftRightSides_linearRegressionModelPtr @= " << (void*)leftRightSides_linearRegressionModelPtr;

    if (!leftRightSides_linearRegressionModelPtr) {
        qDebug() << __FUNCTION__ << "error: leftRightSides_linearRegressionModelPtr is nullptr";
        return(false);
    }

    leftRightSides_linearRegressionModelPtr->_bWarningFlagByUser = bWarnFlag;
    return(true);
}

bool S_StackedProfilWithMeasurements::get_warnFlag(const S_MeasureIndex& measureIndex, bool& bWarnFlag) {

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr =
            getPtrOn_LRSidesLinearRegressionModel(measureIndex);

    qDebug() << __FUNCTION__ << "leftRightSides_linearRegressionModelPtr @= " << (void*)leftRightSides_linearRegressionModelPtr;

    if (!leftRightSides_linearRegressionModelPtr) {
        qDebug() << __FUNCTION__ << "error: leftRightSides_linearRegressionModelPtr is nullptr";
        return(false);
    }

    bWarnFlag = leftRightSides_linearRegressionModelPtr->_bWarningFlagByUser;
    return(true);
}


S_LocationFromZeroOfFirstInsertedValueInStackedProfil::S_LocationFromZeroOfFirstInsertedValueInStackedProfil():
    _value(.0), _bValid(false) { }

void S_LocationFromZeroOfFirstInsertedValueInStackedProfil::set(double value) {
    _value = value;
    _bValid = true;
}

void S_LocationFromZeroOfFirstInsertedValueInStackedProfil::clear() {
    _value = .0;
    _bValid = false;
}


S_FloatWithValidityFlag::S_FloatWithValidityFlag(): _f(.0), _bValid(false) {

}

void S_FloatWithValidityFlag::clear() {
    _f = {.0 };
    _bValid = { false };
}

void S_FloatWithValidityFlag::set(float f) {
    _f = f;
    _bValid = { true };
}


S_InputFiles::S_InputFiles() {
    _qvectStr_inputFile_PX1_PX2_DeltaZ.resize(3);
    _qvectStr_correlationScore_PX1_PX2_DeltaZ.resize(3);   
}

void S_InputFiles::clear() {
    _qvectStr_inputFile_PX1_PX2_DeltaZ.clear();
    _qvectStr_inputFile_PX1_PX2_DeltaZ.resize(3);

    _qvectStr_correlationScore_PX1_PX2_DeltaZ.clear();
    _qvectStr_correlationScore_PX1_PX2_DeltaZ.resize(3);
}

void S_InputFiles::showContent() const {
    qDebug() << "(S_InputFiles)" << __FUNCTION__ << "_qvectStr_inputFile_PX1_PX2_DeltaZ        = " << _qvectStr_inputFile_PX1_PX2_DeltaZ;
    qDebug() << "(S_InputFiles)" << __FUNCTION__ << "_qvectStr_correlationScore_PX1_PX2_DeltaZ = " << _qvectStr_correlationScore_PX1_PX2_DeltaZ;
}



bool S_InputFiles::fromQJsonObject(const QJsonObject& qjsonObj, e_mainComputationMode eMainComputationMode) {

    if (eMainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        return(false);
    }

    clear();

    QString str_inputDisplacementMaps { "inputDisplacementMaps" };
    QJsonObject QjsonObj_str_inputDisplacementMaps;
    bool bValueGot = getQJsonObjectFromJson(qjsonObj, str_inputDisplacementMaps, QjsonObj_str_inputDisplacementMaps);
    if (!bValueGot) {
        clear();
        return(false);
    }
    QString _tqstrDescName_image[3] {"PX1", "PX2", "ZOther"};
    for (int iela = eLA_PX1; iela < eLA_LayersCount; iela++) {

        QJsonObject QjsonObj_iDM_content;
        bValueGot = getQJsonObjectFromJson(QjsonObj_str_inputDisplacementMaps, _tqstrDescName_image[iela], QjsonObj_iDM_content);
        if (!bValueGot) {
            clear();
            return(false);
        }

        QJsonObject QjsonObj_iDM_content_pathAndFilename;
        QString str_pathAndFilename { "pathAndFilename" };
        bValueGot = getQJsonObjectFromJson(QjsonObj_iDM_content, str_pathAndFilename, QjsonObj_iDM_content_pathAndFilename);
        if (!bValueGot) {
            clear();
            return(false);
        }

        bValueGot = joinQJsonObjectPathAndFilename_toQString(QjsonObj_iDM_content_pathAndFilename, _qvectStr_inputFile_PX1_PX2_DeltaZ[iela]);
        if (!bValueGot) {
            clear();
            return(false);
        }
    }

    QString str_inputCorrelationtScoreMaps { "inputCorrelationScoreMaps" };
    QJsonObject QjsonObj_str_inputCorrelationScoreMaps;
    bValueGot = getQJsonObjectFromJson(qjsonObj, str_inputCorrelationtScoreMaps, QjsonObj_str_inputCorrelationScoreMaps);
    if (!bValueGot) {
        clear();
        return(false);
    }
    //load PX1_PX2 twice, one for PX1, the other for PX2
    QString _tqstrDescName_correlScoreMap[3] {"PX1_PX2", "PX1_PX2", "ZOther"};
    for (int iela = eLA_CorrelScoreForPX1Alone; iela < eLA_LayersCount; iela++) {

        QJsonObject QjsonObj_cSM_content;
        bValueGot = getQJsonObjectFromJson(QjsonObj_str_inputCorrelationScoreMaps, _tqstrDescName_correlScoreMap[iela], QjsonObj_cSM_content);
        if (!bValueGot) {
            clear();
            return(false);
        }

        QJsonObject QjsonObj_cSM_content_pathAndFilename;
        QString str_pathAndFilename { "pathAndFilename" };
        bValueGot = getQJsonObjectFromJson(QjsonObj_cSM_content, str_pathAndFilename, QjsonObj_cSM_content_pathAndFilename);
        if (!bValueGot) {
            clear();
            return(false);
        }

        qDebug() << __FUNCTION__ <<  "_qvectStr_correlationScore_PX1_PX2_DeltaZ[" << QString::number(iela) <<  _qvectStr_correlationScore_PX1_PX2_DeltaZ[iela];

        bValueGot = joinQJsonObjectPathAndFilename_toQString(QjsonObj_cSM_content_pathAndFilename, _qvectStr_correlationScore_PX1_PX2_DeltaZ[iela]);
        if (!bValueGot) {
            clear();
            return(false);
        }
    }

    return(true);
}


S_InputImagesFileAttributes::S_InputImagesFileAttributes() {
    _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ.resize(3);
    _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ.resize(3);
}

void S_InputImagesFileAttributes::showContent() {
    qDebug() << "(S_InputImagesFileAttributes)" << __FUNCTION__ << "_qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ = ";
    for (auto& it: _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ) {
        it.showContent();
    }
    qDebug() << "(S_InputImagesFileAttributes)" << __FUNCTION__ << "_qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ =";
    for (auto& it: _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ) {
        it.showContent();
    }
}

void S_InputImagesFileAttributes::clear() {
    _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ.clear();
    _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ.resize(3);

    _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ.clear();
    _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ.resize(3);
}

bool S_InputImagesFileAttributes::fromQJsonObject(const QJsonObject& qjsonObj, e_mainComputationMode eMainComputationMode) {

    if (eMainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        return(false);
    }

    clear();

    QString str_inputDisplacementMaps { "inputDisplacementMaps" };
    QJsonObject QjsonObj_str_inputDisplacementMaps;
    bool bValueGot = getQJsonObjectFromJson(qjsonObj, str_inputDisplacementMaps, QjsonObj_str_inputDisplacementMaps);
    if (!bValueGot) {
        clear();
        return(false);
    }
    QString _tqstrDescName_image[3] {"PX1", "PX2", "ZOther"};
    for (int iela = eLA_PX1; iela < eLA_LayersCount; iela++) {

        QJsonObject QjsonObj_iDM_content;
        bValueGot = getQJsonObjectFromJson(QjsonObj_str_inputDisplacementMaps, _tqstrDescName_image[iela], QjsonObj_iDM_content);
        if (!bValueGot) {
            clear();
            return(false);
        }

        QJsonObject QjsonObj_iDM_attributes;
        QString str_key_attribute { "attributes" };
        bool bQjsonObjAttrGot = getQJsonObjectFromJson(QjsonObj_iDM_content, str_key_attribute, QjsonObj_iDM_attributes);
        if (bQjsonObjAttrGot) {
            S_ImageFileAttributes imageFileAttributes;
            bool bImgFileAttributesGot = imageFileAttributes.fromQJsonObject(QjsonObj_iDM_attributes);
            if (!bImgFileAttributesGot) {
                clear();
                return(false);
            }
            _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ[iela] = imageFileAttributes;
        }
    }

    QString str_inputCorrelationtScoreMaps { "inputCorrelationScoreMaps" };
    QJsonObject QjsonObj_str_inputCorrelationScoreMaps;
    bValueGot = getQJsonObjectFromJson(qjsonObj, str_inputCorrelationtScoreMaps, QjsonObj_str_inputCorrelationScoreMaps);
    if (!bValueGot) {
        clear();
        return(false);
    }
    //load PX1_PX2 twice, one for PX1, the other for PX2
    QString _tqstrDescName_correlScoreMap[3] {"PX1_PX2", "PX1_PX2", "ZOther"};
    for (int iela = eLA_CorrelScoreForPX1Alone; iela < eLA_LayersCount; iela++) {

        QJsonObject QjsonObj_cSM_content;
        bValueGot = getQJsonObjectFromJson(QjsonObj_str_inputCorrelationScoreMaps, _tqstrDescName_correlScoreMap[iela], QjsonObj_cSM_content);
        if (!bValueGot) {
            clear();
            return(false);
        }

        QJsonObject QjsonObj_cSM_attributes;
        QString str_key_attribute { "attributes" };
        bool bQjsonObjAttrGot = getQJsonObjectFromJson(QjsonObj_cSM_content, str_key_attribute, QjsonObj_cSM_attributes);
        if (bQjsonObjAttrGot) {
            S_ImageFileAttributes imageFileAttributes;
            bool bImgFileAttributesGot = imageFileAttributes.fromQJsonObject(QjsonObj_cSM_attributes);
            if (!bImgFileAttributesGot) {
                clear();
                return(false);
            }
            _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela] = imageFileAttributes;
            if (iela == eLA_CorrelScoreForPX1Alone) {
                _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela+1] = imageFileAttributes;
            }
        }
        if (iela == eLA_CorrelScoreForPX1Alone) {
            iela++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }

    return(true);
}


void S_InputFilesWithImageFilesAttributes::clear() {
    _inputFiles.clear();
    _inputImagesFileAttributes.clear();
    _eMainComputationMode = e_mCM_notSet;
}

S_InputFilesWithImageFilesAttributes::S_InputFilesWithImageFilesAttributes() {
    clear();
}

void S_InputFilesWithImageFilesAttributes::set_mainComputationMode(e_mainComputationMode eMainComputationMode) {
    _eMainComputationMode = eMainComputationMode;

}

bool S_InputFilesWithImageFilesAttributes::fromQJsonObject(const QJsonObject& qjsonObj) {

    if (_eMainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        return(false);
    }

    bool bGotInputFile = _inputFiles.fromQJsonObject(qjsonObj, _eMainComputationMode);
    bool bGotImagesFileAttributesWithoutError = _inputImagesFileAttributes.fromQJsonObject(qjsonObj, _eMainComputationMode);
    bool bGot = bGotInputFile && bGotImagesFileAttributesWithoutError;
    if (!bGot) {
        _inputFiles.clear();
        _inputImagesFileAttributes.clear();
    }
    return(bGot);
}

void S_InputFilesWithImageFilesAttributes::showContent() {
    qDebug() << __FUNCTION__ << " @ " <<__LINE__ ; //@#LP debug purpose
    _inputFiles.showContent();
    _inputImagesFileAttributes.showContent();
}


bool S_InputFilesWithImageFilesAttributes::fromInputFiles(const S_InputFiles& inputFiles) {

    _inputFiles = inputFiles;

    for (int iela = eLA_PX1; iela < eLA_LayersCount; iela++) {
        QString strFilename = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela];
        if (!strFilename.isEmpty()) {

            bool bGotImgFileAttributes = _inputImagesFileAttributes._qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ[iela].fromFile(strFilename);
            if (!bGotImgFileAttributes) {
                _inputImagesFileAttributes.clear();
                return(false);
            }
        }
    }

    qDebug() << __FUNCTION__ << " @ " <<__LINE__ ;
    _inputFiles.showContent(); //@#LP debug purpose

    //feed twice for PX1_PX2, for symmetry
    for (int iela = eLA_CorrelScoreForPX1Alone; iela <= eLA_CorrelScoreForDeltaZAlone; iela++) {
        QString strFilename = _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela];

        qDebug() << __FUNCTION__ << "about CorrelScore file: iela = " << iela << " strFilename = " << strFilename;

        if (!strFilename.isEmpty()) {

            qDebug() << __FUNCTION__ << "if (!strFilename.isEmpty()) {";

            bool bGotImgFileAttributes = _inputImagesFileAttributes._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela].fromFile(strFilename);

            qDebug() << __FUNCTION__ << "bGotImgFileAttributes = " << bGotImgFileAttributes;

            if (!bGotImgFileAttributes) {
                qDebug() << __FUNCTION__ << "if (!bGotImgFileAttributes) { ";
                _inputImagesFileAttributes.clear();
                return(false);
            }
        }
        if (iela == eLA_CorrelScoreForPX1Alone) {
            _inputImagesFileAttributes._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela+1] =
            _inputImagesFileAttributes._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela];
            iela++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }
    return(true);
}


bool S_InputFilesWithImageFilesAttributes::toQJsonObject(QJsonObject& qjsonObj) const {

    QString str_pathAndFilename { "pathAndFilename" };

    QString _tqstrDescName_image[3] {"PX1", "PX2", "ZOther"};
    QJsonObject qjsonObj_inputDisplacementMaps;
    for (int iela = eLA_PX1; iela < eLA_LayersCount; iela++) {

        QJsonObject qjsonObj_splittedPathAndFilename;
        splitStrPathAndFilename_toQJsonObject(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela], qjsonObj_splittedPathAndFilename);

        QJsonObject qjsonObj_layer;
        qjsonObj_layer.insert(str_pathAndFilename, qjsonObj_splittedPathAndFilename);

        //add size, type and channels count here
        QString strFilename = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela];
        //insert attributes for used file
        //At loading, if this section is missing for a used file, we do not consider that as a fatal error. The reason is that project file can be made by
        //a human, and for now, the app do not permit to generate this attributes section. Hence, we do not want to force the human to feed this.
        //If a used file is not found and its attributes section is missing, we will make none check.
        //But before any computation core runs call, call this:
        //checkInputFilesAccordingToMainComputationMode_andInitPixelPickers which make a check

        if (!strFilename.isEmpty()) {

            QJsonObject qjonObject_inputFileAttributes;
            _inputImagesFileAttributes._qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ[iela].toQJsonObject(qjonObject_inputFileAttributes);


            QString str_key_attribute { "attributes" };
            qjsonObj_layer.insert(str_key_attribute, qjonObject_inputFileAttributes);
        }

        qjsonObj_inputDisplacementMaps.insert(_tqstrDescName_image[iela], qjsonObj_layer);
    }
    QString str_inputDisplacementMaps { "inputDisplacementMaps" };
    qjsonObj.insert(str_inputDisplacementMaps, qjsonObj_inputDisplacementMaps);


    QString _tqstrDescName_correlScoreMap[3] {"PX1_PX2", "PX1_PX2", "ZOther"};
    QJsonObject qjsonObj_inputCorrelationScoreMaps;
    for (int iela = eLA_CorrelScoreForPX1Alone; iela <= eLA_CorrelScoreForDeltaZAlone; iela++) {

        QJsonObject qjsonObj_splittedPathAndFilename;
        splitStrPathAndFilename_toQJsonObject(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela], qjsonObj_splittedPathAndFilename);

        QJsonObject qjsonObj_corrScoreMaps;
        qjsonObj_corrScoreMaps.insert(str_pathAndFilename, qjsonObj_splittedPathAndFilename);

        //add size, type and channels count here
        QString strFilename = _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela];

        qDebug() << __FUNCTION__<< "_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[" << iela << "] = " << strFilename;

        //insert attributes for used file
        //At loading, if this section is missing for a used file, we do not consider that as a fatal error. The reason is that project file can be made by
        //a human, and for now, the app do not permit to generate this attributes section. Hence, we do not want to force the human to feed this.
        //If a used file is not found and its attributes section is missing, we will make none check.
        //But before any computation core runs call, call this:
        //checkInputFilesAccordingToMainComputationMode_andInitPixelPickers which make a check
        if (!strFilename.isEmpty()) {

            QJsonObject qjonObject_inputFileAttributes;
            _inputImagesFileAttributes._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[iela].toQJsonObject(qjonObject_inputFileAttributes);


            QString str_key_attribute { "attributes" };
            qjsonObj_corrScoreMaps.insert(str_key_attribute, qjonObject_inputFileAttributes);

            qDebug() << __FUNCTION__ << "qjsonObj_corrScoreMaps = " << qjsonObj_corrScoreMaps;

        }
        qjsonObj_inputCorrelationScoreMaps.insert(_tqstrDescName_correlScoreMap[iela], qjsonObj_corrScoreMaps);

        if (iela == eLA_CorrelScoreForPX1Alone) {
            iela++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }
    QString str_inputCorrelationtScoreMaps { "inputCorrelationScoreMaps" };
    qjsonObj.insert(str_inputCorrelationtScoreMaps, qjsonObj_inputCorrelationScoreMaps);

    return(true);
}




S_MeasureIndex::S_MeasureIndex(bool bOnMainMeasure, int secondaryMeasureIdx):
    _bOnMainMeasure(bOnMainMeasure),
    _secondaryMeasureIdx(secondaryMeasureIdx) {
}

S_MeasureIndex::S_MeasureIndex():
    _bOnMainMeasure(false),
    _secondaryMeasureIdx(-1) {
}

void S_MeasureIndex::clear() {
    _bOnMainMeasure = false;
    _secondaryMeasureIdx = -1;
}



S_XRangeForLinearRegressionComputation::S_XRangeForLinearRegressionComputation():
    _bValid(false),
    _xMin(.0),
    _xMax(.0) {
}

void S_XRangeForLinearRegressionComputation::clear() {
    _bValid = { false };
    _xMin = { .0 };
    _xMax = { .0 };
}

void S_XRangeForLinearRegressionComputation::showContent() const {
    qDebug() << __FUNCTION__ << "(S_XRangeForLinearRegressionComputation)";
    qDebug() << __FUNCTION__ << " _bValid : " << _bValid;
    qDebug() << __FUNCTION__ << "  _xMin  : " << _xMin;
    qDebug() << __FUNCTION__ << "  _xMax  : " << _xMax;
}

S_XRangeForLinearRegressionComputation::S_XRangeForLinearRegressionComputation(qreal xMin, qreal xMax):
    _bValid(true),
    _xMin(xMin),
    _xMax(xMax) {
}

void S_XRangeForLinearRegressionComputation::set(qreal xMin, qreal xMax) {
    qDebug() << __FUNCTION__ << " (S_XRangeForLinearRegressionComputation)" << " xMin = " << xMin << " xMax =" << xMax;

    _bValid = true;
    _xMin = xMin;
    _xMax = xMax;
}

bool S_XRangeForLinearRegressionComputation::toQJsonObject(QJsonObject &qJsonObjOut) const {

    qJsonObjOut.insert("bValid", _bValid);
    if (!_bValid) {
        qJsonObjOut.insert("xMin", 0);
        qJsonObjOut.insert("xMax", 0);
    } else {
        qJsonObjOut.insert("xMin", _xMin); //to integer is ok
        qJsonObjOut.insert("xMax", _xMax); //to integer is ok
    }
    return(true);
}

bool S_XRangeForLinearRegressionComputation::fromQJsonObject(const QJsonObject &qJsonObj)  {

    bool bGotReport = true;

    //bool bValidGot = false;
    int xMinGot = 0;
    int xMaxGot = 0;

    //bGotReport &= getBoolValueFromJson(qJsonObj, "bValid", bValidGot);
    bGotReport &= getIntValueFromJson (qJsonObj, "xMin", xMinGot);
    bGotReport &= getIntValueFromJson (qJsonObj, "xMax", xMaxGot);

    if (!bGotReport) {
        return(false);
    }

    _bValid = false; //false by default, waiting check about consistency with the other linear regression model values
    _xMin = xMinGot;
    _xMax = xMaxGot;
    return(true);
}



S_LinearRegressionParameters::S_LinearRegressionParameters() {
    qDebug() << __FUNCTION__ << "(constructor())";
    clear();
}

S_LinearRegressionParameters::S_LinearRegressionParameters(qreal xMin, qreal xMax):
    _xRangeForLinearRegressionComputation(xMin, xMax) {
    qDebug() << __FUNCTION__ << "(constructor(qreal xMin, qreal xMax))";
    _mathComp_LinearRegressionParameters.clear();
}

void S_LinearRegressionParameters::clear() {
    qDebug() << __FUNCTION__ << "(clear())";
    _xRangeForLinearRegressionComputation.clear();
    _mathComp_LinearRegressionParameters.clear();
}

void S_LinearRegressionParameters::set_xRange(qreal xMin, qreal xMax) {
    _xRangeForLinearRegressionComputation.set(xMin, xMax);
}

bool S_LinearRegressionParameters::XRangeForLinearRegressionComputation_toQJsonObject(QJsonObject &qJsonObjOut) const {
    QJsonObject qJsonObJ_XRangeForLinearRegressionComputation;
    bool bReport = _xRangeForLinearRegressionComputation.toQJsonObject(qJsonObJ_XRangeForLinearRegressionComputation);
    if (!bReport) {
        return(false);
    }
    qJsonObjOut.insert("XRange", qJsonObJ_XRangeForLinearRegressionComputation);
    return(true);
}

bool S_LinearRegressionParameters::linearRegrModel_toQJsonObject(QJsonObject &qjsonObj) const {
    return(_mathComp_LinearRegressionParameters.linearRegrModel_toQJsonObject(qjsonObj));
}

bool S_LinearRegressionParameters::linearRegrModel_toASciiStringlist(
        bool bSetLinearRegressionData_asEmpty,
        QStringList& qstrList_sideOfLinearRegressionModel) const {
    return(_mathComp_LinearRegressionParameters.linearRegrModel_toASciiStringlist(
               bSetLinearRegressionData_asEmpty,
               qstrList_sideOfLinearRegressionModel));
}

bool S_LinearRegressionParameters::XRangeForLinearRegressionComputation_fromQJsonObject(const QJsonObject &qJsonObj) {

    QString strKey_XRange { "XRange" };
    QJsonObject qJsonObJ_XRangeForLinearRegressionComputation;
    bool bGot_qJsonObJ_XRangeForLinearRegressionComputation = false;
    bGot_qJsonObJ_XRangeForLinearRegressionComputation = getQJsonObjectFromJson(qJsonObj, strKey_XRange, qJsonObJ_XRangeForLinearRegressionComputation);
    if (!bGot_qJsonObJ_XRangeForLinearRegressionComputation) {
        return(false);
    }
    bool bReport = _xRangeForLinearRegressionComputation.fromQJsonObject(qJsonObJ_XRangeForLinearRegressionComputation);

    return(bReport);
}


void S_LinearRegressionParameters::showContent() const {
    qDebug() << __FUNCTION__ << "(S_LinearRegressionParameters)";
    _xRangeForLinearRegressionComputation.showContent();
    _mathComp_LinearRegressionParameters.showContent();
}


S_BoxAndStackedProfilWithMeasurements::S_BoxAndStackedProfilWithMeasurements():
    _bNeedsToBeRecomputedFromImages(true),
    _profilsBoxParameters {} {

    qDebug() << "constructor S_BoxAndStackedProfilWithMeasurements()";
    _locationFromZeroOfFirstInsertedValueInStackedProfil.clear();
    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements.resize(3);
}

S_BoxAndStackedProfilWithMeasurements::S_BoxAndStackedProfilWithMeasurements(
        bool bNeedsToBeRecomputedFromImages,
        const S_ProfilsBoxParameters& profilsBoxParameters):
    _bNeedsToBeRecomputedFromImages(bNeedsToBeRecomputedFromImages),
    _profilsBoxParameters(profilsBoxParameters) {

    qDebug() << "constructor S_BoxAndStackedProfilWithMeasurements(2 params)";
    _locationFromZeroOfFirstInsertedValueInStackedProfil.set( - (_profilsBoxParameters._oddPixelLength / 2));
    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements.resize(3);
}

S_BoxAndStackedProfilWithMeasurements::S_BoxAndStackedProfilWithMeasurements(const S_BoxAndStackedProfilWithMeasurements& copyMe) {

    qDebug() << "S_BoxAndStackedProfilWithMeasurements(const S_BoxAndStackedProfilWithMeasurements& copyMe)";

    _bNeedsToBeRecomputedFromImages = copyMe._bNeedsToBeRecomputedFromImages;
    _profilsBoxParameters = copyMe._profilsBoxParameters;

    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements = copyMe._qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements;
    _locationFromZeroOfFirstInsertedValueInStackedProfil = copyMe._locationFromZeroOfFirstInsertedValueInStackedProfil;
}

S_BoxAndStackedProfilWithMeasurements& S_BoxAndStackedProfilWithMeasurements::operator=(const S_BoxAndStackedProfilWithMeasurements& sBASPWM) {
    if (this == &sBASPWM) {
        return (*this);
    }

    _bNeedsToBeRecomputedFromImages = sBASPWM._bNeedsToBeRecomputedFromImages;
    _profilsBoxParameters = sBASPWM._profilsBoxParameters;

    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements = sBASPWM._qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements;
    _locationFromZeroOfFirstInsertedValueInStackedProfil = sBASPWM._locationFromZeroOfFirstInsertedValueInStackedProfil;

    return (*this);
}


void S_BoxAndStackedProfilWithMeasurements::showContent_noMeasurements() const {

    qDebug() << __FUNCTION__;
    _profilsBoxParameters.showContent();
    qDebug() << __FUNCTION__ << "_bNeedsToBeRecomputedFromImages = "<< _bNeedsToBeRecomputedFromImages;
    qDebug() << __FUNCTION__ << "_locationFromZeroOfFirstInsertedValueInStackedProfil._value = " << _locationFromZeroOfFirstInsertedValueInStackedProfil._value;
    qDebug() << __FUNCTION__ << "_locationFromZeroOfFirstInsertedValueInStackedProfil._bValid = " << _locationFromZeroOfFirstInsertedValueInStackedProfil._bValid;
    qDebug() << __FUNCTION__ << "_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements.size() = " << _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements.size();

}

void S_BoxAndStackedProfilWithMeasurements::showContent_mainMeasurement_linearRgressionModel() const {

    qDebug() << __FUNCTION__ ;
    showContent_noMeasurements();
    qDebug() << __FUNCTION__ << "_locationFromZeroOfFirstInsertedValueInStackedProfil._bValid = " << _locationFromZeroOfFirstInsertedValueInStackedProfil._bValid;
    qDebug() << __FUNCTION__ << "_locationFromZeroOfFirstInsertedValueInStackedProfil._value = " << _locationFromZeroOfFirstInsertedValueInStackedProfil._value;

    for (int ieCA = e_CA_Perp; ieCA < e_CA_deltaZ; ieCA++) {
        qDebug() << __FUNCTION__ << "for ieCA = " << ieCA << "mainMeasure._x0ForYOffsetComputation = "
                 << _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                        _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0;

        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure._linearRegressionParam_Left_Right[e_LRsA_left].showContent();

        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure._linearRegressionParam_Left_Right[e_LRsA_right].showContent();
    }
}



void S_BoxAndStackedProfilWithMeasurements::clearMeasurementsDataContent() {

    qDebug() << "S_BoxAndStackedProfilWithMeasurements::clearMeasurementsDataContent()";

    _bNeedsToBeRecomputedFromImages = true;
    for (auto& iter: _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements) {
        qDebug() << __FUNCTION__ << " call iter.clear_dataContent() (loop#0:perp/ lopp#1:parall/ loop#2:deltaz)";
        iter.clear_dataContent();
    }
}

bool S_BoxAndStackedProfilWithMeasurements::profilWithMeasurements_withoutLinearRegresionModelResult_toQJsonObject(
        const QVector<bool>& qvectb_computedComponentsFlags_Perp_Parall_deltaZ,
        int idBox,
        QJsonObject &qJsonObjOut) const {

    if (qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ <<  "error: qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size() != 3  (is:"
                 << qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size();
        return(false);
    }
    int countTrue = qvectb_computedComponentsFlags_Perp_Parall_deltaZ.count(true);
    if (!countTrue) {
        qDebug() << __FUNCTION__ <<  "error: countTrue = 0";
        return(false);
    }

    QString qstrKey_boxId {"boxId"};
    qJsonObjOut.insert(qstrKey_boxId, idBox);

    QString tqstrComponentJSonDescName[3] {"Perp", "Parall", "ZOther"};

    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

        if (qvectb_computedComponentsFlags_Perp_Parall_deltaZ[ieCA]) {

            QJsonObject qJsonObj_layer;

            QJsonObject qJsonObjOut_XRangesForLinearRegressionComputation;
            bool bReport_LRMainMeasure = false;
            bReport_LRMainMeasure =
                _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                    XRangesForLinRegr_withX0_toQJsonObject(qJsonObjOut_XRangesForLinearRegressionComputation);
            qJsonObj_layer.insert("XRangesForLinRegr_withX0", qJsonObjOut_XRangesForLinearRegressionComputation);

            QJsonObject qJsonObjOut_warnFlagByUser;
                _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                    warnFlagByUser_toQJsonObject(qJsonObjOut_warnFlagByUser);
            qJsonObj_layer.insert("Bookmark", qJsonObjOut_warnFlagByUser);

            qJsonObjOut.insert(tqstrComponentJSonDescName[ieCA], qJsonObj_layer);
        }
    }

    return(true);
}



bool S_BoxAndStackedProfilWithMeasurements::profilWithMeasurements_withLinearRegresionModelResult_toQJsonObject(
        const QVector<bool>& qvectb_computedComponentsFlags_Perp_Parall_deltaZ,
        int idBox,
        const QVector<bool>& qvectbSetLinearRegressionData_asEmpty,
        QJsonObject &qJsonObjOut) const {

    if (qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ <<  "error: qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size() != 3  (is:"
                 << qvectb_computedComponentsFlags_Perp_Parall_deltaZ.size();
        return(false);
    }
    int countTrue = qvectb_computedComponentsFlags_Perp_Parall_deltaZ.count(true);
    if (!countTrue) {
        qDebug() << __FUNCTION__ <<  "error: countTrue = 0";
        return(false);
    }    
    if (qvectbSetLinearRegressionData_asEmpty.size() != 3) {
        qDebug() << __FUNCTION__ << "error: qvectbSetLinearRegressionData_asEmpty.size() != 3";
        return(false);
    }

    QString qstrKey_boxId {"boxId"};
    qJsonObjOut.insert(qstrKey_boxId, idBox);

    QString tqstrComponentJSonDescName[3] {"Perp", "Parall", "ZOther"};

    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

        if (qvectb_computedComponentsFlags_Perp_Parall_deltaZ[ieCA]) {

            QJsonObject qJsonObj_layer;

            QJsonObject qJsonObjOut_XRangesForLinearRegressionComputation;
            bool bReport_LRMainMeasure_XRangeWithX0 = false;
            bReport_LRMainMeasure_XRangeWithX0 =
                _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                    XRangesForLinRegr_withX0_toQJsonObject(qJsonObjOut_XRangesForLinearRegressionComputation);

            //accept to export XRanges with X0 even when bSetLinearRegressionData_asEmpty is true
            qJsonObj_layer.insert("XRangesForLinRegr_withX0", qJsonObjOut_XRangesForLinearRegressionComputation);

            if (!qvectbSetLinearRegressionData_asEmpty[ieCA]) {
                QJsonObject qJsonObjOut_mainMeasure_linearRegressionModel;
                bool bReport_LRMainMeasure_linearRegrModel = false;
                bReport_LRMainMeasure_linearRegrModel =
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                        linearRegrModel_toQJsonObject(qJsonObjOut_mainMeasure_linearRegressionModel);

                QJsonObject qJsonObjOut_LRmainMeasure_modelValuesResults;
                bool bReport_LRMainMeasure_offsetAndSigmaSum = false;
                bReport_LRMainMeasure_offsetAndSigmaSum =
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                        modelValuesResults_toQJsonObject(qJsonObjOut_LRmainMeasure_modelValuesResults);

                qJsonObj_layer.insert("LinearRegressionModel", qJsonObjOut_mainMeasure_linearRegressionModel);
                qJsonObj_layer.insert("modelResult", qJsonObjOut_LRmainMeasure_modelValuesResults); //yOffsetAX0 and sigmabsum
            }

            QJsonObject qJsonObjOut_warnFlagByUser;
            _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure.
                warnFlagByUser_toQJsonObject(qJsonObjOut_warnFlagByUser);
            qJsonObj_layer.insert("Bookmark", qJsonObjOut_warnFlagByUser);

            qJsonObjOut.insert(tqstrComponentJSonDescName[ieCA], qJsonObj_layer);
        }
    }

    return(true);
}

bool getVectStr_aboutProfilCurvesMeanings(e_mainComputationMode eMainComputationMode,
                                          const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                                          const vector<e_ComputationMethod>& vect_eComputationMethod_Perp_Parall_DeltaZ,
                                          QVector<QString>& qvectStr_majorCurveMeaning,
                                          QVector<QString>& qvectStr_minorEnveloppeAreaMeaning) {

    if (eMainComputationMode == e_mCM_notSet) {
        return(false);
    }
    //e_mCM_Typical_PX1PX2Together_DeltazAlone

    if (  (qvectb_componentsToUse_Perp_Parall_DeltaZ.size()  != 3)
        ||(vect_eComputationMethod_Perp_Parall_DeltaZ.size() != 3)) {
        return(false);
    }

    QHash<e_ComputationMethod, QString> qhash_majorCurveMeaning {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "mean"}, //
        {e_CM_median,         "median"}, //
        {e_CM_weightedMean,   "weighted mean"}, //
        {e_CM_weightedMedian, "weighted median"}//
    };

    QHash<e_ComputationMethod, QString> qhash_minorEnveloppeAreaMeaning {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "+ and - absolute deviation around mean"},
        {e_CM_median,         "+ and - absolute deviation around median"},
        {e_CM_weightedMean,   "+ and - absolute deviation around weighted mean"},
        {e_CM_weightedMedian, "+ and - absolute deviation around weighted median"}
    };

    qvectStr_majorCurveMeaning.fill("<...>",3);
    qvectStr_minorEnveloppeAreaMeaning.fill("<...>",3);

    if (qvectb_componentsToUse_Perp_Parall_DeltaZ[e_CA_Perp]) {

        if (vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp] == e_CM_notSet) {
            return(false);
        }

        qvectStr_majorCurveMeaning[e_CA_Perp  ] = qhash_majorCurveMeaning.value(vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp]);
        qvectStr_majorCurveMeaning[e_CA_Parall] = qvectStr_majorCurveMeaning[e_CA_Perp];

        qvectStr_minorEnveloppeAreaMeaning[e_CA_Perp  ] = qhash_minorEnveloppeAreaMeaning.value(vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp]);
        qvectStr_minorEnveloppeAreaMeaning[e_CA_Parall] = qvectStr_minorEnveloppeAreaMeaning[e_CA_Perp];

    }

    if (qvectb_componentsToUse_Perp_Parall_DeltaZ[e_CA_deltaZ]) {

        if (vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_deltaZ] == e_CM_notSet) {
            return(false);
        }

        qvectStr_majorCurveMeaning        [e_CA_deltaZ] = qhash_majorCurveMeaning.value(vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_deltaZ]);
        qvectStr_minorEnveloppeAreaMeaning[e_CA_deltaZ] = qhash_minorEnveloppeAreaMeaning.value(vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_deltaZ]);

    }

    return(true);
}


bool S_BoxAndStackedProfilWithMeasurements::computedComponentsToHeader_toQJsonObject_aboutProfilCurves(
                                                                       const vector<e_ComputationMethod>& vector_eComputationMethod,
                                                                       const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                                                                       QJsonObject& qjsonObj_computedComponentsToHeader) const {
    if (qvectb_componentsToUse_Perp_Parall_DeltaZ.size()!= 3) {
        return(false);
    }

    int componentToUseCount = qvectb_componentsToUse_Perp_Parall_DeltaZ.count(true);
    if (!componentToUseCount) {
        return(false);
    }

    if (vector_eComputationMethod.size() !=3) {
        return(false);
    }

    //titles for column
    //ComputationMethod
    QHash<e_ComputationMethod, QString> qhash_CM_majorMethodNameForJsonOutput {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "mean"},
        {e_CM_median,         "median"},
        {e_CM_weightedMean,   "weightedMean"},
        {e_CM_weightedMedian, "weightedMedian"}
    };

    QHash<e_ComputationMethod, QString> qhash_CM_minorMethodNameForJsonOutput {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "absDevAroundMean"},
        {e_CM_median,         "absDevAroundMedian"},
        {e_CM_weightedMean,   "absDevAroundWeightedMean"},
        {e_CM_weightedMedian, "absDevAroundWeightedMedian"}
    };

    //-------------------------------------------

    QJsonArray qjsonArray_arrayTitle_computationMethodForComputedComponents;
    //insert N/A for column x in any case:
    qjsonArray_arrayTitle_computationMethodForComputedComponents.insert(0, "N/A"); // ( N/A as: Not Applicable )

    //insert computed method for components:
    int idxTitleOut = 1;
    for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
        if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {
            e_ComputationMethod eComputationMethod = vector_eComputationMethod[iComponent];
            if (eComputationMethod == e_CM_notSet) {
                return(false);
            }
            qjsonArray_arrayTitle_computationMethodForComputedComponents.insert(idxTitleOut, qhash_CM_majorMethodNameForJsonOutput.value(eComputationMethod));
            idxTitleOut++;
            qjsonArray_arrayTitle_computationMethodForComputedComponents.insert(idxTitleOut, qhash_CM_minorMethodNameForJsonOutput.value(eComputationMethod));
            idxTitleOut++;
        }
    }

    //x and components (Perp, Parall, DeltaZ)
    QJsonArray qjsonArray_arrayTitle_tuple_X_Perp_Parall_DeltaZ;

    //insert x in any case:
    qjsonArray_arrayTitle_tuple_X_Perp_Parall_DeltaZ.insert(0, "x");

    //insert computed components:
    QVector<QString> qvectQString_componentTitle {"Perp", "Parall", "deltaZ"};
    idxTitleOut = 1;
    for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
        if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {
            qjsonArray_arrayTitle_tuple_X_Perp_Parall_DeltaZ.insert(idxTitleOut, qvectQString_componentTitle[iComponent]);
            idxTitleOut++;
            //Perp major, Perp minor, Parall major, Parall minor, deltaZ major, deltaZ minor,
            qjsonArray_arrayTitle_tuple_X_Perp_Parall_DeltaZ.insert(idxTitleOut, qvectQString_componentTitle[iComponent]);
            idxTitleOut++;
        }
    }

    //Examples of possibles columns titles:
    //
    // method used for each components by column
    // x and components by column
    //
    //examples:
    //
    // N/A    mean  absDevAroundMean   mean     meanabsDevAroundMean
    //  x     Perp       Perp         Parall          Parall
    //
    // N/A  weightedMean   absDevAroundWeightedMean
    //  x     deltaZ                deltaZ
    //
    // N/A  weightedMedian    absDevAroundWeightedMedian     median  absDevAroundMedian
    //  x     Perp                   Perp                    deltaZ        deltaZ
    //
    //@LP: this is not possible to have mean and median mixed here
    //(or any combination with some mean and median (including weighted...)

    qjsonObj_computedComponentsToHeader.insert("methodMajorMinor_by_component", qjsonArray_arrayTitle_computationMethodForComputedComponents);
    qjsonObj_computedComponentsToHeader.insert("title_by_component", qjsonArray_arrayTitle_tuple_X_Perp_Parall_DeltaZ);

    return(true);
}


bool S_BoxAndStackedProfilWithMeasurements::computedComponentsToHeader_toAscii_aboutProfilCurves(
    const vector<e_ComputationMethod>& vector_eComputationMethod,
    const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
        QStringList& qstrList_computedComponentsToHeader, QStringList& qstrList_title_tuple_X_Perp_Parall_DeltaZ) const {


    if (qvectb_componentsToUse_Perp_Parall_DeltaZ.size()!= 3) {
        return(false);
    }

    int componentToUseCount = qvectb_componentsToUse_Perp_Parall_DeltaZ.count(true);
    if (!componentToUseCount) {
        return(false);
    }

    if (vector_eComputationMethod.size() !=3) {
        return(false);
    }

    //titles for column
    //ComputationMethod
    QHash<e_ComputationMethod, QString> qhash_CM_majorMethodNameForJsonOutput {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "mean"},
        {e_CM_median,         "median"},
        {e_CM_weightedMean,   "weightedMean"},
        {e_CM_weightedMedian, "weightedMedian"}
    };

    QHash<e_ComputationMethod, QString> qhash_CM_minorMethodNameForJsonOutput {
        {e_CM_notSet,         "ERROR"},
        {e_CM_mean,           "absDevAroundMean"},
        {e_CM_median,         "absDevAroundMedian"},
        {e_CM_weightedMean,   "absDevAroundWeightedMean"},
        {e_CM_weightedMedian, "absDevAroundWeightedMedian"}
    };

    //-------------------------------------------

    //QJsonArray qjsonArray_arrayTitle_computationMethodForComputedComponents;
    //insert N/A for column x in any case:
    qstrList_computedComponentsToHeader.push_back("methodMajorMinor_by_component");
    qstrList_computedComponentsToHeader.push_back("N/A"); // ( N/A as: Not Applicable )

    //insert computed method for components:
    for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
        if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {
            e_ComputationMethod eComputationMethod = vector_eComputationMethod[iComponent];
            if (eComputationMethod == e_CM_notSet) {
                return(false);
            }
            qstrList_computedComponentsToHeader.push_back(qhash_CM_majorMethodNameForJsonOutput.value(eComputationMethod));
            qstrList_computedComponentsToHeader.push_back(qhash_CM_minorMethodNameForJsonOutput.value(eComputationMethod));
        }
    }

    //x and components (Perp, Parall, DeltaZ)

    //insert x in any case:
    qstrList_title_tuple_X_Perp_Parall_DeltaZ.push_back("title_by_component");
    qstrList_title_tuple_X_Perp_Parall_DeltaZ.push_back("x");

    //insert computed components:
    QVector<QString> qvectQString_componentTitle {"Perp", "Parall", "deltaZ"};
    for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
        if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {
            qstrList_title_tuple_X_Perp_Parall_DeltaZ.push_back(qvectQString_componentTitle[iComponent]);
            //Perp major, Perp minor, Parall major, Parall minor, deltaZ major, deltaZ minor,
            qstrList_title_tuple_X_Perp_Parall_DeltaZ.push_back(qvectQString_componentTitle[iComponent]);
        }
    }

    //Examples of possibles columns titles:
    //
    // method used for each components by column
    // x and components by column
    //
    //examples:
    //
    // N/A    mean  absDevAroundMean   mean     meanabsDevAroundMean
    //  x     Perp       Perp         Parall          Parall
    //
    // N/A  weightedMean   absDevAroundWeightedMean
    //  x     deltaZ                deltaZ
    //
    // N/A  weightedMedian    absDevAroundWeightedMedian     median  absDevAroundMedian
    //  x     Perp                   Perp                    deltaZ        deltaZ
    //
    //@LP: this is not possible to have mean and median mixed here
    //(or any combination with some mean and median (including weighted...)

    return(true);
}




bool S_BoxAndStackedProfilWithMeasurements::profilCurves_toQJsonObject(int vectBoxId,
                                                                       const vector<e_ComputationMethod>& vector_eComputationMethod,
                                                                       const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                                                                       const QVector<bool>& qvectbSetProfilesCurvesData_asEmpty,
                                                                       QJsonObject& qjsonObj_profilCurves) const {

    if (qvectb_componentsToUse_Perp_Parall_DeltaZ.size() != 3) {
        return(false);
    }

    int componentToUseCount = qvectb_componentsToUse_Perp_Parall_DeltaZ.count(true);
    if (!componentToUseCount) {
        return(false);
    }

    if (vector_eComputationMethod.size() != 3) {
        return(false);
    }

    if  (qvectbSetProfilesCurvesData_asEmpty.size() != 3) {
        return(false);
    }

    if (!_locationFromZeroOfFirstInsertedValueInStackedProfil._bValid) {
        return(false);
    }

    QJsonArray qjsonArray_ArrayOfTuple_X_Perp_Parall_DeltaZ;

    for (int xi = 0; xi < _profilsBoxParameters._oddPixelLength; xi++) {

        QJsonArray qjsonArray_tuple_X_Perp_Parall_DeltaZ;

        qjsonArray_tuple_X_Perp_Parall_DeltaZ.insert(0, _locationFromZeroOfFirstInsertedValueInStackedProfil._value + xi);

        int iColumnInsert = 1;//first column for insert is just after x column
        for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {

            if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {

                if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].
                    _qvectqvect_aboutMeanMedian[ vector_eComputationMethod[iComponent] ].size()
                     < _profilsBoxParameters._oddPixelLength ) {
                    return(false);
                }
                S_AnonymMajorMinorWithValidityFlag sAnonymMajorMinorWithValidityFlag =
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].
                    _qvectqvect_aboutMeanMedian[ vector_eComputationMethod[iComponent] ][xi].
                    _anonymMajorMinorWithValidityFlag;

                bool bInsertJsonNullValue = false;

                if (qvectbSetProfilesCurvesData_asEmpty[iComponent]) {
                    bInsertJsonNullValue = true;
                }
                if (!sAnonymMajorMinorWithValidityFlag._bValuesValid) {
                    bInsertJsonNullValue = true;
                }

                if (bInsertJsonNullValue) {
                    qjsonArray_tuple_X_Perp_Parall_DeltaZ.insert(iColumnInsert, QJsonValue(QJsonValue::Null));
                    qjsonArray_tuple_X_Perp_Parall_DeltaZ.insert(iColumnInsert+1, QJsonValue(QJsonValue::Null));
                } else {
                     qjsonArray_tuple_X_Perp_Parall_DeltaZ.insert(iColumnInsert, sAnonymMajorMinorWithValidityFlag._major_centralValue);
                     qjsonArray_tuple_X_Perp_Parall_DeltaZ.insert(iColumnInsert+1, sAnonymMajorMinorWithValidityFlag._minor_aroundCentralValue);
                }
                iColumnInsert+=2;
            }

        }

        qjsonArray_ArrayOfTuple_X_Perp_Parall_DeltaZ.insert(xi, qjsonArray_tuple_X_Perp_Parall_DeltaZ);
    }

    qjsonObj_profilCurves.insert("values",  qjsonArray_ArrayOfTuple_X_Perp_Parall_DeltaZ);
    qjsonObj_profilCurves.insert("boxId",  vectBoxId);

    return(true);
}

bool S_BoxAndStackedProfilWithMeasurements::profilCurves_toAscii(int vectBoxId,
                          const vector<e_ComputationMethod>& vector_eComputationMethod,
                          const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                          bool bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                          QVector<QStringList>& qvectQstrList_profilCurveContent) const {

    if (qvectb_componentsToUse_Perp_Parall_DeltaZ.size()!= 3) {
        return(false);
    }

    int componentToUseCount = qvectb_componentsToUse_Perp_Parall_DeltaZ.count(true);
    if (!componentToUseCount) {
        return(false);
    }

    if (vector_eComputationMethod.size() !=3) {
        return(false);
    }

    if (!_locationFromZeroOfFirstInsertedValueInStackedProfil._bValid) {
        return(false);
    }

    for (int xi = 0; xi < _profilsBoxParameters._oddPixelLength; xi++) {

        QStringList qstrList_allValuesForASpecificX;
        qstrList_allValuesForASpecificX.push_back("");//empty first column due to title about the headers
        qstrList_allValuesForASpecificX.push_back(QString::number(_locationFromZeroOfFirstInsertedValueInStackedProfil._value + xi));

        for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {

            if (qvectb_componentsToUse_Perp_Parall_DeltaZ[iComponent]) {

                if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].
                    _qvectqvect_aboutMeanMedian[ vector_eComputationMethod[iComponent] ].size()
                     < _profilsBoxParameters._oddPixelLength ) {
                    return(false);
                }
                S_AnonymMajorMinorWithValidityFlag sAnonymMajorMinorWithValidityFlag =
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].
                    _qvectqvect_aboutMeanMedian[ vector_eComputationMethod[iComponent] ][xi].
                    _anonymMajorMinorWithValidityFlag;

                bool bFeedWithEmptyString = false;

                if (!sAnonymMajorMinorWithValidityFlag._bValuesValid) {
                   bFeedWithEmptyString = true;
                } else {
                    if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent]._LRSide_linRegrModel_mainMeasure._bWarningFlagByUser) {
                        if (bIfWarnFlagByUser_setProfilesCurvesData_asEmpty) {
                            bFeedWithEmptyString = true;
                        }
                    }
                }
                if (bFeedWithEmptyString) {
                    qstrList_allValuesForASpecificX.push_back("");
                    qstrList_allValuesForASpecificX.push_back("");
                } else {
                    qstrList_allValuesForASpecificX.push_back(doubleToQStringPrecision_g(sAnonymMajorMinorWithValidityFlag._major_centralValue, 14));
                    qstrList_allValuesForASpecificX.push_back(doubleToQStringPrecision_g(sAnonymMajorMinorWithValidityFlag._minor_aroundCentralValue,14));
                }
            }
        }
        qvectQstrList_profilCurveContent.push_back(qstrList_allValuesForASpecificX);

    }
    return(true);
}



bool S_BoxAndStackedProfilWithMeasurements::setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel(
        const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ) {

    qDebug() << __FUNCTION__<< "qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ =" << qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ;
    qDebug() << __FUNCTION__<< "qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size() =" << qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size();

    if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements.size() != 3) {
        //(dev error)
        qDebug() << __FUNCTION__<< "error:  _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements.size() != 3";
        return(false);
    }


    if (qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size() != 3) {
        //(dev error)
        qDebug() << __FUNCTION__<< "error: qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size() != 3";
        return(false);
    }

    double min = _locationFromZeroOfFirstInsertedValueInStackedProfil._value;
    double max = -min;
    double center = .0;

    qDebug() << __FUNCTION__<< "min = " << min;
    qDebug() << __FUNCTION__<< "max = " << max;
    qDebug() << __FUNCTION__<< "center = " << center;

    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

        qDebug() << __FUNCTION__<< "ieCA = " << ieCA;
        qDebug() << __FUNCTION__<< "qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ[ieCA] = " << qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ[ieCA];

        if (qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ[ieCA]) {

            qDebug() << __FUNCTION__<< "call now  set_xRange";

            //@LP_following_devdebug
            _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                _LRSide_linRegrModel_mainMeasure.
                _linearRegressionParam_Left_Right[e_LRsA_left].
                set_xRange(min, center);

            qDebug() << __FUNCTION__ << "min, center ="  << min << ", " << center;

            _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                _LRSide_linRegrModel_mainMeasure.
                _linearRegressionParam_Left_Right[e_LRsA_right].
                set_xRange(center, max);

            qDebug() << __FUNCTION__ << "center, max ="  << center << ", " << max;

           _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
               _LRSide_linRegrModel_mainMeasure.
               _x0ForYOffsetComputation.set(center);
       }
    }
    return(true);
}



bool S_BoxAndStackedProfilWithMeasurements::profilWithMeasurements_withoutLinearRegresionModelResult_fromQJsonObject(
        const QJsonObject &qJsonObj, const QVector<bool>& qvectb_componentToCompute_Perp_Parall_deltaZ, int boxId,
        QString& strMsgErrorDetails) {

    strMsgErrorDetails.clear();

    if (qvectb_componentToCompute_Perp_Parall_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ <<  "error: qvectb_componentToCompute_Perp_Parall_deltaZ.size() != 3  (is:" << qvectb_componentToCompute_Perp_Parall_deltaZ.size();
        strMsgErrorDetails = "dev error #801";
        return(false);
    }
    int countTrue = qvectb_componentToCompute_Perp_Parall_deltaZ.count(true);
    if (!countTrue) {
        qDebug() << __FUNCTION__ <<  "error: countTrue = 0";
        strMsgErrorDetails = "dev error #802";
        return(false);
    }


    QString qstrKey_boxId {"boxId"};
    int boxIdGot = -1;
    bool bBoxIdGot = getIntValueFromJson(qJsonObj, qstrKey_boxId, boxIdGot);

    if (! bBoxIdGot) {
        strMsgErrorDetails = "failed getting " + qstrKey_boxId;
        return(false);
    }

    //qDebug() << __FUNCTION__ <<  "_*_ qJsonObj = " << qJsonObj;
    //qDebug() << __FUNCTION__ <<  "_*_ bBoxIdGot = " << bBoxIdGot;

    if ( boxIdGot != boxId) {
        strMsgErrorDetails = "StackedProfil: boxId mismatch. Got " + QString::number(bBoxIdGot) + ", should be:" + QString::number(boxId);
        return(false);
    }

    QString tqstrComponentJSonDescName[3] {"Perp", "Parall", "ZOther"};


    QJsonObject qJsonObj_Perp_Parall_deltaZ[3];
    QVector<bool> qvectbQjsonObj_Perp_Parall_deltaZ {false, false, false};

    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        qvectbQjsonObj_Perp_Parall_deltaZ[ieCA] = getQJsonObjectFromJson(qJsonObj, tqstrComponentJSonDescName[ieCA], qJsonObj_Perp_Parall_deltaZ[ieCA]);
    }
    qDebug() << __FUNCTION__ << "__________";
    //qDebug() << __FUNCTION__ <<  "_*_ qvectbQjsonObj_Perp_Parall_deltaZ = " << qvectbQjsonObj_Perp_Parall_deltaZ;
    //qDebug() << __FUNCTION__ <<  "_*_qvectb_componentToCompute_Perp_Parall_deltaZ = " << qvectb_componentToCompute_Perp_Parall_deltaZ;

    if (!qvectbQjsonObj_Perp_Parall_deltaZ.count(true)) {
        strMsgErrorDetails = "StackedProfil: none component values found for boxId #" + QString::number(boxIdGot);
        return(false);
    }

    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

        //qDebug() << __FUNCTION__ <<  "_*_ qJsonObj_Perp_Parall_deltaZ[ " <<  ieCA << " ] = " << qJsonObj_Perp_Parall_deltaZ[ieCA];

        //component which should not be present:
        if (  (!qvectb_componentToCompute_Perp_Parall_deltaZ[ieCA])
            &&(qvectbQjsonObj_Perp_Parall_deltaZ[ieCA])) { //but found
            strMsgErrorDetails = "StackedProfil: layer " + tqstrComponentJSonDescName[ieCA] + " should not exist. BoxId: #" + QString::number(boxId);
            return(false);
         }

        //layers which have to be present:
        if (  (qvectb_componentToCompute_Perp_Parall_deltaZ[ieCA])
            &&(!qvectbQjsonObj_Perp_Parall_deltaZ[ieCA])) { //but not found
            strMsgErrorDetails = "StackedProfil: layer " + tqstrComponentJSonDescName[ieCA] + " have to exist. BoxId: #" + QString::number(boxId);
            return(false);
        }

        if (!qvectb_componentToCompute_Perp_Parall_deltaZ[ieCA]) {
            continue;
        }

        QString strKey_XRangesForLinRegr_withX0 { "XRangesForLinRegr_withX0" };

        qDebug() << __FUNCTION__ <<  "_*_ will try to get " << strKey_XRangesForLinRegr_withX0 << " for component #" << ieCA;

        QJsonObject qJsonObjComponent_XRangesForLinRegr_withX0;
        bool bqJsonObjComponentGot = getQJsonObjectFromJson(qJsonObj_Perp_Parall_deltaZ[ieCA], strKey_XRangesForLinRegr_withX0, qJsonObjComponent_XRangesForLinRegr_withX0);
        if (!bqJsonObjComponentGot) {
            strMsgErrorDetails = "key not found: " + strKey_XRangesForLinRegr_withX0 + " for " + tqstrComponentJSonDescName[ieCA] + ". BoxId: #" + QString::number(boxId);
            return(false);
        }


        //qDebug() << __FUNCTION__ <<  "_*_ qJsonObjComponent_XRangesForLinRegr_withX0 = " << qJsonObjComponent_XRangesForLinRegr_withX0;

        //XRangesForLinRegr_withX0_fromQJsonObject perform some consistency check between loaded values, but none with the box parameters
        bool bXRangesForLinRegr_withX0_got = _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
            _LRSide_linRegrModel_mainMeasure.XRangesForLinRegr_withX0_fromQJsonObject(qJsonObjComponent_XRangesForLinRegr_withX0);
        if (!bXRangesForLinRegr_withX0_got) {
            strMsgErrorDetails = "failed getting " + strKey_XRangesForLinRegr_withX0 + " for " + tqstrComponentJSonDescName[ieCA] + ". BoxId: #" + QString::number(boxId);
            return(false);
        }


        QString strKey_bookmark { "Bookmark" };
        QJsonObject qJsonObjComponent_bookmark;
        bool bBookmark_got = getQJsonObjectFromJson(qJsonObj_Perp_Parall_deltaZ[ieCA], strKey_bookmark, qJsonObjComponent_bookmark);
        if (!bBookmark_got) {
            //do nothing; already set by default at false
        } else {
            QString strMsgErrorDetails_warnFlagByUser;
            bool bWarnFlagByUser_got = _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                _LRSide_linRegrModel_mainMeasure.warnFlagByUser_fromQJsonObject(qJsonObjComponent_bookmark, strMsgErrorDetails_warnFlagByUser);
            if (!bWarnFlagByUser_got) {
                strMsgErrorDetails =
                        strMsgErrorDetails_warnFlagByUser
                        + "("
                        + strKey_bookmark
                        + " for " + tqstrComponentJSonDescName[ieCA] + ". BoxId: #" + QString::number(boxId)
                        + ")";
                return(false);
            }
        }

        QVector<double> qvectdbl_valuesToCheck;
        QVector<QString> qvectqstr_stringValuesToCheckForErrorReport;

        //check now XRanges consistency with Box parameters
        if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0
             < -(_profilsBoxParameters._oddPixelLength/2)) {
                strMsgErrorDetails = "too small x0 for mainMeasure about +" + tqstrComponentJSonDescName[ieCA] + " of BoxId: #" + QString::number(boxId);
                return(false);
        }
        if (_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0
             > (_profilsBoxParameters._oddPixelLength/2)) {
                strMsgErrorDetails = "too big x0 for mainMeasure about +" + tqstrComponentJSonDescName[ieCA] + " of BoxId: #" + QString::number(boxId);
                return(false);
        }

        qvectdbl_valuesToCheck.push_back(_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0);
        qvectqstr_stringValuesToCheckForErrorReport.push_back("x0");

        QString qstrSideForErrorReport_Left_Right[2] {"left side", "right side"};
        for (int ieLRS = e_LRsA_left; ieLRS <= e_LRsA_right; ieLRS++) {

                qvectdbl_valuesToCheck.push_back(_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                                 _LRSide_linRegrModel_mainMeasure.
                                                 _linearRegressionParam_Left_Right[ieLRS].
                                                 _xRangeForLinearRegressionComputation._xMin);
                qvectqstr_stringValuesToCheckForErrorReport.push_back(qstrSideForErrorReport_Left_Right[ieLRS]+" xMin");

                qvectdbl_valuesToCheck.push_back(_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                                 _LRSide_linRegrModel_mainMeasure.
                                                 _linearRegressionParam_Left_Right[ieLRS].
                                                 _xRangeForLinearRegressionComputation._xMax);
                qvectqstr_stringValuesToCheckForErrorReport.push_back(qstrSideForErrorReport_Left_Right[ieLRS]+" xMax");
            }


        double rangeXMax =  _profilsBoxParameters._oddPixelLength/2;
        double rangeXMin =  -rangeXMax;

        int idxVCheck = 0;
        for(auto vToCheck: qvectdbl_valuesToCheck) {
            if (vToCheck < rangeXMin) {
                strMsgErrorDetails = qvectqstr_stringValuesToCheckForErrorReport[idxVCheck]+ " value: " + QString::number(vToCheck, 'f', 0)
                        + " smaller than rangeXMin: " + QString::number(rangeXMin, 'f', 0);
                return(false);
            }
            if (vToCheck > rangeXMax) {
                strMsgErrorDetails = qvectqstr_stringValuesToCheckForErrorReport[idxVCheck] + " value: " + QString::number(vToCheck, 'f', 0)
                        + " bigger than rangeXMax: " + QString::number(vToCheck, 'f', 0);
                return(false);
            }
        }

        //set the _x0ForYOffsetComputation as valid
        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._bValid = true;

        //set the left right for main measure as valid
        for (int ieLRS = e_LRsA_left; ieLRS <= e_LRsA_right; ieLRS++) {
            qvectdbl_valuesToCheck.push_back(_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                             _LRSide_linRegrModel_mainMeasure.
                                             _linearRegressionParam_Left_Right[ieLRS].
                                             _xRangeForLinearRegressionComputation._bValid = true);
        }

    }

    return(true);
}

S_X0ForYOffsetComputation::S_X0ForYOffsetComputation(): _bValid(false), _x0(.0) {
    qDebug() << __FUNCTION__ << "(constructor)";
}

void S_X0ForYOffsetComputation::clear() {
    _bValid = false;
    _x0 = .0;
}

void S_X0ForYOffsetComputation::set(double x0) {
    _bValid = true;
    _x0 = x0;
}

void S_X0ForYOffsetComputation::showContent() const {
    qDebug() << __FUNCTION__ << "(S_X0ForYOffsetComputation) _x0 = " << _x0;
    qDebug() << __FUNCTION__ << "(S_X0ForYOffsetComputation) _bValid = " << _bValid;
}

S_LeftRightSides_linearRegressionModel::S_LeftRightSides_linearRegressionModel() {
    //@LP values set at zero

    //_linearRegressionParam_Left_Right[e_LRsA_left].clear();
    //_linearRegressionParam_Left_Right[e_LRsA_right].clear();
    //_x0ForYOffsetComputation.clear();
    //_modelValuesResults.clear();

    _bWarningFlagByUser = false;
    qDebug() << __FUNCTION__;

    showContent();

    qDebug() << __FUNCTION__ << "*** after";

}

void S_LeftRightSides_linearRegressionModel::clearResults() {
    _modelValuesResults.clear();
}


void S_LeftRightSides_linearRegressionModel::showContent() {

    qDebug() << __FUNCTION__ << "_linearRegressionParam_Left_Right[e_LRsA_left] :";
    _linearRegressionParam_Left_Right[e_LRsA_left].showContent();

    qDebug() << __FUNCTION__ << "_linearRegressionParam_Left_Right[e_LRsA_right] :";
    _linearRegressionParam_Left_Right[e_LRsA_right].showContent();

    qDebug() << __FUNCTION__ << "_bWarningFlagByUser = " <<  _bWarningFlagByUser;

    //_x0ForYOffsetComputation.showContent()
    _x0ForYOffsetComputation.showContent();

    //_modelValuesResults
    _modelValuesResults.showContent();

}

void S_LeftRightSides_linearRegressionModel::clearResultsAndReverseSign() {

    S_LinearRegressionParameters newLeftSide;
    newLeftSide.set_xRange( - ( _linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._xMax),
                            - ( _linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._xMin));
    newLeftSide._xRangeForLinearRegressionComputation._bValid =
                                _linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._bValid;

    S_LinearRegressionParameters newRightSide;
    newRightSide.set_xRange( - ( _linearRegressionParam_Left_Right[e_LRsA_left]._xRangeForLinearRegressionComputation._xMax),
                             - ( _linearRegressionParam_Left_Right[e_LRsA_left]._xRangeForLinearRegressionComputation._xMin));
    newRightSide._xRangeForLinearRegressionComputation._bValid =
                                 _linearRegressionParam_Left_Right[e_LRsA_left]._xRangeForLinearRegressionComputation._bValid;

    // clear linear regression GLSL math results
    _linearRegressionParam_Left_Right[e_LRsA_left ].clear();
    _linearRegressionParam_Left_Right[e_LRsA_right].clear();
    //clear linear regression results
    clearResults();

    //set the limits
    _linearRegressionParam_Left_Right[e_LRsA_left ] = newLeftSide;
    _linearRegressionParam_Left_Right[e_LRsA_right] = newRightSide;

    //set x0
    _x0ForYOffsetComputation._x0 = - (_x0ForYOffsetComputation._x0);
    //@LP _x0ForYOffsetComputation._bvalid stay with its current value

}


bool S_LeftRightSides_linearRegressionModel::XRangesForLinRegr_withX0_toQJsonObject(QJsonObject& qjsonObjOut) const {

    //@#LP test if (_bvalid) ?

    bool bReport = true;

    QJsonObject qjsonObj_leftSide;
    bReport &= _linearRegressionParam_Left_Right[e_LRsA_left].XRangeForLinearRegressionComputation_toQJsonObject(qjsonObj_leftSide);

    QJsonObject qjsonObj_rightSide;
    bReport &= _linearRegressionParam_Left_Right[e_LRsA_right].XRangeForLinearRegressionComputation_toQJsonObject(qjsonObj_rightSide);

    if (!bReport) {
        qDebug() << __FUNCTION__ <<  "error: S_LinearRegressionParameters.XRangeForLinearRegressionComputation_toQJsonObject left and/or right failed";
        return(false);
    }

    qjsonObjOut.insert("XRange_left" , qjsonObj_leftSide);
    qjsonObjOut.insert("XRange_right", qjsonObj_rightSide);

    QJsonObject qjsonObj_x0ForYOffsetComputation;
    qjsonObj_x0ForYOffsetComputation.insert("bValid", _x0ForYOffsetComputation._bValid);

    if (!_x0ForYOffsetComputation._bValid) {
        qjsonObj_x0ForYOffsetComputation.insert("x0", "invalid");
    } else {
        qjsonObj_x0ForYOffsetComputation.insert("x0", _x0ForYOffsetComputation._x0);
    }
    qjsonObjOut.insert("x0ForYOffsetComputation", qjsonObj_x0ForYOffsetComputation);

    return(true);
}

void S_LeftRightSides_linearRegressionModel::warnFlagByUser_toQJsonObject(QJsonObject& qjsonObjOut) const {
    qjsonObjOut.insert("bWarningFlagByUser", _bWarningFlagByUser);
}

bool S_LeftRightSides_linearRegressionModel::warnFlagByUser_fromQJsonObject(const QJsonObject &qJsonObj, QString& strMsgErrorDetails) {
    QString strkey_bWarningFlagByUser {"bWarningFlagByUser"};

    //permit to load a project without presence of _bWarningFlagByUser (hence, set to false by default)
    if (!qJsonObj.contains(strkey_bWarningFlagByUser)) {
        _bWarningFlagByUser = false;
        return(true);
    }

    bool bGot_bWarningFlagByUser = false;
    bool bWarnFlagByUser_value = false;
    bGot_bWarningFlagByUser = getBoolValueFromJson(qJsonObj, strkey_bWarningFlagByUser, bWarnFlagByUser_value);
    if (!bGot_bWarningFlagByUser) {
        qDebug() << __FUNCTION__ <<  "error: S_LeftRightSides_linearRegressionModel getting key strkey_bWarnFlagByUser failed";
        strMsgErrorDetails =  "failed getting " + strkey_bWarningFlagByUser;
        return(false);
    }
    _bWarningFlagByUser = bWarnFlagByUser_value;
    return(true);
}


bool S_LeftRightSides_linearRegressionModel::linearRegrModel_toQJsonObject(QJsonObject& qjsonObjOut) const {

    bool bReport = true;

    QJsonObject qjsonObj_leftSide;
    bReport &= _linearRegressionParam_Left_Right[e_LRsA_left].linearRegrModel_toQJsonObject(qjsonObj_leftSide);

    QJsonObject qjsonObj_rightSide;
    bReport &= _linearRegressionParam_Left_Right[e_LRsA_right].linearRegrModel_toQJsonObject(qjsonObj_rightSide);

    if (!bReport) {
        qDebug() << __FUNCTION__ <<  "error: S_LinearRegressionParameters.linearRegrModel_toQJsonObject left and/or right failed";
        return(false);
    }

    qjsonObjOut.insert("left" , qjsonObj_leftSide);
    qjsonObjOut.insert("right", qjsonObj_rightSide);

    return(true);
}

bool S_LeftRightSides_linearRegressionModel::modelValuesResults_toQJsonObject(QJsonObject& qjsonObjOut) const {
    qjsonObjOut.insert("bComputed"  , _modelValuesResults._bComputed);
    qjsonObjOut.insert("yOffsetAtX0", _modelValuesResults._yOffsetAtX0);
    qjsonObjOut.insert("sigmabSum"  , _modelValuesResults._sigmabSum);
    return(true);
}



bool S_LeftRightSides_linearRegressionModel::XRangesForLinRegr_withX0_fromQJsonObject(const QJsonObject& qjsonObj) {

    QString strkey_xrange_left  {"XRange_left"};
    QString strkey_xrange_right {"XRange_right"};

    QJsonObject qjsonObj_xrange_Left_Right[2];


    bool bGot_qjsonObj_xrange = true;    
    bGot_qjsonObj_xrange &= getQJsonObjectFromJson(qjsonObj, strkey_xrange_left , qjsonObj_xrange_Left_Right[e_LRsA_left]);
    bGot_qjsonObj_xrange &= getQJsonObjectFromJson(qjsonObj, strkey_xrange_right, qjsonObj_xrange_Left_Right[e_LRsA_right]);
    if (!bGot_qjsonObj_xrange) {
        qDebug() << __FUNCTION__ <<  "error: S_LeftRightSides_linearRegressionModel getting key left and/or right failed";
        return(false);
    }

    bGot_qjsonObj_xrange &= _linearRegressionParam_Left_Right[e_LRsA_left].XRangeForLinearRegressionComputation_fromQJsonObject(
                                   qjsonObj_xrange_Left_Right[e_LRsA_left]);

    bGot_qjsonObj_xrange &= _linearRegressionParam_Left_Right[e_LRsA_right].XRangeForLinearRegressionComputation_fromQJsonObject(
                                   qjsonObj_xrange_Left_Right[e_LRsA_right]);

    if (!bGot_qjsonObj_xrange) {
        qDebug() << __FUNCTION__ <<  "error: S_LeftRightSides_linearRegressionModel getting qjsonObj_xrange_Left_Right value(s) left and/or right failed";
        return(false);
    }

    bool bGotQjsonObjX0 = false;
    QString strkey_x0ForYOffsetComputation  {"x0ForYOffsetComputation"};
    QJsonObject qjsonObjX0;
    bGotQjsonObjX0 = getQJsonObjectFromJson(qjsonObj, strkey_x0ForYOffsetComputation, qjsonObjX0);
    if (!bGotQjsonObjX0) {
        qDebug() << __FUNCTION__ <<  "error: S_LeftRightSides_linearRegressionModel getting key" << strkey_x0ForYOffsetComputation;
        return(false);
    }

    bool bGotReport = true;

    int x0_got = 0;


    bGotReport &= getIntValueFromJson(qjsonObjX0, "x0", x0_got);
    if (!bGotReport) {
        qDebug() << __FUNCTION__ <<  "error: S_LeftRightSides_linearRegressionModel getting qjsonObjX0 value(s)";
        return(false);
    }

    _x0ForYOffsetComputation._bValid = false; //false by default, waiting check

    _x0ForYOffsetComputation._x0 = x0_got;

    //on each side: check min < max

    //check values consistency:
    int countSidesWithbValidSetAtTrue = 0;
    for (int ieLRS = e_LRsA_left ; ieLRS <= e_LRsA_right; ieLRS++) {

            countSidesWithbValidSetAtTrue++;
            if (_linearRegressionParam_Left_Right[ieLRS]._xRangeForLinearRegressionComputation._xMin >=
                _linearRegressionParam_Left_Right[ieLRS]._xRangeForLinearRegressionComputation._xMax) {
                //@#LP +str error msg
                return(false);
            }

    }

    //check left.xmax <= right.xmin
    if (countSidesWithbValidSetAtTrue == 2) {
        if (_linearRegressionParam_Left_Right[e_LRsA_left ]._xRangeForLinearRegressionComputation._xMax >
            _linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._xMin) {
            //@#LP +str error msg
            return(false);
        }
    }

    //check: left.max =< x0
    if (_linearRegressionParam_Left_Right[e_LRsA_left ]._xRangeForLinearRegressionComputation._xMax > _x0ForYOffsetComputation._x0) {
        //@#LP +str error msg
        return(false);
    }
    //check: right min >= x0
    if (_linearRegressionParam_Left_Right[e_LRsA_right ]._xRangeForLinearRegressionComputation._xMin < _x0ForYOffsetComputation._x0) {
        //@#LP +str error msg
        return(false);
    }

    return(true);
}




S_LRSide_modelValuesResults::S_LRSide_modelValuesResults():
    _bComputed(false),
    _yOffsetAtX0(.0),
    _sigmabSum(.0) {
    qDebug() << __FUNCTION__ << "(constructor)";
}

void S_LRSide_modelValuesResults::clear() {
    _bComputed = false;
    _yOffsetAtX0 = .0;
    _sigmabSum = .0;
}

void S_LRSide_modelValuesResults::set(qreal yOffsetAtX0, qreal sigmabSum) {
    _bComputed = true;
    _yOffsetAtX0 = yOffsetAtX0;
    _sigmabSum = sigmabSum;
}

void S_LRSide_modelValuesResults::showContent() const {
    qDebug() << __FUNCTION__ << "(S_LRSide_modelValuesResults) _bComputed   = " << _bComputed;
    qDebug() << __FUNCTION__ << "(S_LRSide_modelValuesResults) _yOffsetAtX0 = " << _yOffsetAtX0;
    qDebug() << __FUNCTION__ << "(S_LRSide_modelValuesResults) _sigmabSum   = " << _sigmabSum;
}



QString ePixelExtractionMethod_toJsonProjectString(e_PixelExtractionMethod ePixelExtractionMethod) {
    switch(ePixelExtractionMethod) {
        case e_PEM_notSet      : return("notSet");
        case e_PEM_NearestPixel: return("nearestPixel");
        case e_PEM_BilinearInterpolation2x2: return("biLinearInterpolation2x2");
    }
    return("invalid");
}

QString eBaseComputationMethod_toJsonProjectString(e_BaseComputationMethod eBaseComputationMethod) {
    switch(eBaseComputationMethod) {
        case e_BCM_notSet: return("notSet");
        case e_BCM_mean  : return("mean");
        case e_BCM_median: return("median");
    }
    return("invalid");
}

QString eProfilOrientation_toJsonProjectString(e_ProfilOrientation eProfilOrientation) {
    switch(eProfilOrientation) {
        case e_PO_notSet: return("notSet");
        case e_PO_progressDirectionIsFromFirstToLastPointOfRoute: return("progressDirectionAlongTraceIsFromFirstToLastPoint");
        case e_PO_progressDirectionIsFromLastToFirstPointOfRoute: return("progressDirectionAlongTraceIsFromLastToFirstPoint");
    }
    return("invalid");
}

e_PixelExtractionMethod projectJsonString_to_ePixelExtractionMethod(const QString& strPixelExtractionMethod) {
    QHash<QString, e_PixelExtractionMethod> qhash_projectJsonString_ePixelExtractionMethod;
    qhash_projectJsonString_ePixelExtractionMethod.insert("nearestPixel", e_PEM_NearestPixel);
    qhash_projectJsonString_ePixelExtractionMethod.insert("biLinearInterpolation2x2", e_PEM_BilinearInterpolation2x2);
    qhash_projectJsonString_ePixelExtractionMethod.insert("notSet", e_PEM_notSet);

    e_PixelExtractionMethod ePixelExtractionMethod = e_PEM_notSet;
    const auto iterFound = qhash_projectJsonString_ePixelExtractionMethod.find(strPixelExtractionMethod);
    if (iterFound != qhash_projectJsonString_ePixelExtractionMethod.cend()) {
        ePixelExtractionMethod = iterFound.value();
    }
    return(ePixelExtractionMethod);
}

e_BaseComputationMethod projectJsonString_to_eBaseComputationMethod(const QString& strBaseComputationMethod) {
    QHash<QString, e_BaseComputationMethod> qhash_projectJsonString_eBaseComputationMethod;
    qhash_projectJsonString_eBaseComputationMethod.insert("mean", e_BCM_mean);
    qhash_projectJsonString_eBaseComputationMethod.insert("median", e_BCM_median);
    qhash_projectJsonString_eBaseComputationMethod.insert("notSet", e_BCM_notSet);

    e_BaseComputationMethod eBaseComputationMethod = e_BCM_notSet;
    const auto iterFound = qhash_projectJsonString_eBaseComputationMethod.find(strBaseComputationMethod);
    if (iterFound != qhash_projectJsonString_eBaseComputationMethod.cend()) {
        eBaseComputationMethod = iterFound.value();
    }
    return(eBaseComputationMethod);
}

e_ProfilOrientation projectJsonString_to_eProfilOrientation(const QString& strProfilOrientation) {
    QHash<QString, e_ProfilOrientation> qhash_projectJsonString_eProfilOrientation;
    qhash_projectJsonString_eProfilOrientation.insert("progressDirectionAlongTraceIsFromFirstToLastPoint", e_PO_progressDirectionIsFromFirstToLastPointOfRoute);
    qhash_projectJsonString_eProfilOrientation.insert("progressDirectionAlongTraceIsFromLastToFirstPoint", e_PO_progressDirectionIsFromLastToFirstPointOfRoute);
    qhash_projectJsonString_eProfilOrientation.insert("notSet", e_PO_notSet);

    e_ProfilOrientation eProfilOrientation = e_PO_notSet;
    const auto iterFound = qhash_projectJsonString_eProfilOrientation.find(strProfilOrientation);
    if (iterFound != qhash_projectJsonString_eProfilOrientation.cend()) {
        eProfilOrientation = iterFound.value();
    }
    return(eProfilOrientation);
}

//we use int to disencourage the human to try play with it in the project json file.
//The value is produced and altered when adjusting automatically distributed boxes with DistanceBetweenSuccessivesBoxes usingAutomaticDistribution.
//Editing this value by hand should never be done.
e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution projectJsonInt_to_e_DistanceBetweenSuccessivesBoxesUAD(const int ieDistanceBSBAD) {

    if (  (ieDistanceBSBAD < e_DBSBuDA_notSet)
        ||(ieDistanceBSBAD > e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel)) {
        return(e_DBSBuDA_notSet);
    }
    return(static_cast<e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution>(ieDistanceBSBAD));
}

