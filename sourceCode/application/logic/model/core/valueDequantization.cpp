#include <QDebug>

#include "valueDequantization.hpp"

#include "../../toolbox/toolbox_json.h"

#include "../../toolbox/toolbox_conversion.h"

ValueDequantization::ValueDequantization() {
     clear();
}

void ValueDequantization::clear() {
    _SFwvf_micmacStep.clear();
    _SFwvf_spatialResolution.clear();

    _SFwvf_dequant.clear();
}

void ValueDequantization::set_micmacStep(float fMicmacStep) {
    qDebug() << __FUNCTION__ << "fMicmacStep = " << fMicmacStep;
    _SFwvf_micmacStep.set(fMicmacStep);
    updateDequantValidityAndValue();
}

void ValueDequantization::set_spatialResolution(float fSpatialResolution) {
    qDebug() << __FUNCTION__ << "fSpatialResolution = " << fSpatialResolution;
    _SFwvf_spatialResolution.set(fSpatialResolution);
    updateDequantValidityAndValue();
}

void ValueDequantization::updateDequantValidityAndValue() {
    _SFwvf_dequant._bValid = (_SFwvf_micmacStep._bValid && _SFwvf_spatialResolution._bValid);
    if (!_SFwvf_dequant._bValid) {
        _SFwvf_dequant.clear();
    } else {
        _SFwvf_dequant.set( _SFwvf_micmacStep._f * _SFwvf_spatialResolution._f);
    }
}

bool ValueDequantization::isDequantValid() const {
    return (_SFwvf_dequant._bValid);
}

bool ValueDequantization::isMicmacStepValid() const {
    return(_SFwvf_micmacStep._bValid);
}

bool ValueDequantization::isSpatialResolutionValid() const {
    return(_SFwvf_spatialResolution._bValid);
}

//@LP no check about _bfDequantStepValid at each dequantize call to avoid repetitive test
//Hence, the caller needs to call isDequantStepValid() one time before any call to dequantize()
//to be sure dequantized value returned by dequantize() is valid
float ValueDequantization::dequantize(float fValue) {
    return(fValue*_SFwvf_dequant._f);
}

float ValueDequantization::get_micmacStep() const {
    return(_SFwvf_micmacStep._f);
}

float ValueDequantization::get_spatialResolution() const {
    return(_SFwvf_spatialResolution._f);
}

float ValueDequantization::get_dequant() const {
    return(_SFwvf_dequant._f);
}


bool ValueDequantization::toQJsonObject(QJsonObject& qjsonObj) const {

    if (!isDequantValid()) {
        return(false);
    }

    //micmacStep
    /*qDebug() << __FUNCTION__ << "------------------";
    QString strOf_dValue = doubleToQStringPrecision_f(static_cast<double>(_SFwvf_micmacStep._f), 4);
    qDebug() << __FUNCTION__ << "#A: strOf_dValue = " << strOf_dValue;
    strOf_dValue = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(strOf_dValue);
    qDebug() << __FUNCTION__ << "#B: strOf_dValue = " << strOf_dValue;
    if (strOf_dValue == "0") {
        return(false);
    }
    int indexOfDot = strOf_dValue.indexOf('.');
    int decimalPrecision = 0;
    if (indexOfDot != -1) {
        decimalPrecision  = strOf_dValue.size() - indexOfDot -1;
    }
    qDebug() << __FUNCTION__ << "#CA: indexOfDot = " << indexOfDot;
    qDebug() << __FUNCTION__ << "#CA: decimalPrecision = " << decimalPrecision;
    double final = 1.0;
    bool bfixedPrecision = floatToDoubleWithReducedPrecision(static_cast<double>(_SFwvf_micmacStep._f), decimalPrecision, final);
    qDebug() << __FUNCTION__ << "#CA: final = " << final;
    qDebug() << __FUNCTION__ << "------------------";
    */

    double double_micmacStep_forOutJson = 1.0;
    bool bfixedPrecision_micmacStep = floatToDoubleWithReducedPrecision(static_cast<double>(_SFwvf_micmacStep._f), 4, double_micmacStep_forOutJson);
    if ( double_micmacStep_forOutJson < 0.0001) {
        return(false);
    }

    double double_spatialResolution_forOutJson = 1.0;
    bool bfixedPrecision_spatialResolution = floatToDoubleWithReducedPrecision(static_cast<double>(_SFwvf_spatialResolution._f), 4, double_spatialResolution_forOutJson);
    if ( double_micmacStep_forOutJson < 0.0001) {
        return(false);
    }

    if ((!bfixedPrecision_micmacStep)||(!bfixedPrecision_spatialResolution)) {
        return(false);
    }

    qjsonObj.insert("micmacStep", double_micmacStep_forOutJson);
    qjsonObj.insert("spatialResolution", double_spatialResolution_forOutJson);

//#define DEF_ValueDequantization_outjson_way1
#ifdef DEF_ValueDequantization_outjson_way1
    qjsonObj.insert("micmacStep", static_cast<double>(_SFwvf_micmacStep._f));
    qjsonObj.insert("spatialResolution", static_cast<double>(_SFwvf_spatialResolution._f));
#endif
    return(true);
}






bool ValueDequantization::fromQJsonObject(const QJsonObject& qjsonObj) {

    qDebug() << __FUNCTION__ << "qjsonObj =" << qjsonObj;

    double micmacStepFromJson = .0;
    double spatialResolutionFromJson = .0;

    bool bGot = true;
    bGot &= getDoubleValueFromJson(qjsonObj, "micmacStep", micmacStepFromJson);
    bGot &= getDoubleValueFromJson(qjsonObj, "spatialResolution", spatialResolutionFromJson);

    if (!bGot) {
        clear();
        qDebug() << __FUNCTION__ << " error: qjsonObj does not contains all keys";
        return(false);
    }

    if (  (micmacStepFromJson < 0.0001)
        ||(micmacStepFromJson > 999.999)) {
        return(false);
    }

    if (  (spatialResolutionFromJson < 0.0001)
        ||(spatialResolutionFromJson > 999.999)) {
        return(false);
    }

    if (!isConformToFormatPrecision(micmacStepFromJson, 4, 7)) {
        return(false);
    }

    if (!isConformToFormatPrecision(spatialResolutionFromJson, 4, 7)) {
        return(false);
    }

    set_micmacStep(static_cast<float>(micmacStepFromJson));
    set_spatialResolution(static_cast<float>(spatialResolutionFromJson));

    return(true);
}
