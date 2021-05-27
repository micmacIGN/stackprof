#include <cmath>

#include <iostream>
using namespace std;
#include <math.h>

#include <qmath.h>

#include <QDebug>

#include <QString>

#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include <OpenImageIO/fmath.h> //for floorfrac
using namespace OIIO;

#include "../logic/model/core/S_Segment.h" //@#LP move  S_Segment outside core

#include <QPointF>

#include "toolbox_math.h"

bool doubleValuesAreClose(double v1, double v2, double deltaAround) {
    qDebug() << __FUNCTION__ << "v1, v2, deltaAround = " << v1 << ", " << v2 << ", " << deltaAround;

    double delta_v1v2 = fabs(v1-v2);

    qDebug() << __FUNCTION__ << "delta_v1v2 = " << delta_v1v2;
    qDebug() << __FUNCTION__ << "delta_v1v2 ; deltaAround => " << delta_v1v2 << ", " << deltaAround;
    qDebug() << __FUNCTION__ << "delta_v1v2 < deltaAround ::: " << (delta_v1v2 < deltaAround);

    return(delta_v1v2 < deltaAround);
}

//@#LP improve name, design and API or limit usage
bool valueIsCloseToZero_deltaLowerThan1ExpMinus14(double v) {
    //qDebug() << __FUNCTION__ << "v       = " << QString::number(v, 'f', 14);
    //qDebug() << __FUNCTION__ << "qAbs(v) = " << QString::number(qAbs(v), 'f', 14);
    //qDebug() << __FUNCTION__ << "if (qAbs(v) < 0.00000000000001)";
    if (qAbs(v) < 0.00000000000001) {
        //qDebug() << "value is close to ZERO !";
        return(true);
    }
    return(false);
}

bool bCheckDoubleIsEven(double v) {
    double moduloDdivider = 2.0;
    double moduloResult = std::fmod(v,moduloDdivider);
    qDebug() << __FUNCTION__ << "moduloResult = " << moduloResult;
    return(moduloResult == .0);
}

double findUpper010ValueFrom(double v, double powerOfTen) {
    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v)) {
        return(.0);
    }
    if (v > .0) {
        return(qPow(10,powerOfTen));
    }
    return(.0);
}


double findLower010ValueFrom(double v, double powerOfTen) {
    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v)) {
        return(.0);
    }
    if (v < .0) {
        return(-qPow(10,powerOfTen));
    }
    return(.0);
}

double findUpperStepValue(double v, double step) {

    //qDebug() << __FUNCTION__ << "@ v    = " << v;
    //qDebug() << __FUNCTION__ << "  step = " << step;

    int int_vDivStep= static_cast<int>(v / step);
    //qDebug() << __FUNCTION__ << "  int_vDivStep = " << int_vDivStep;
    double closeStepValue = static_cast<double>(int_vDivStep)*step;

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v - closeStepValue)) {
        //qDebug() << __FUNCTION__ << "  input value is very close to closeValue";
        //qDebug() << __FUNCTION__ << "=> lowerStepValue = " << closeStepValue;
        return(closeStepValue);
    }

    double valueUpper = .0; //0 as invalid value here
    if (v < .0) {
        //qDebug() << __FUNCTION__ << "  v < .0 {";
        valueUpper = closeStepValue;
    } else { // v > .0
        //qDebug() << __FUNCTION__ << "  v > .0 {";
        valueUpper = static_cast<double>(int_vDivStep+1)*step;
    }

    //qDebug() << __FUNCTION__ << " valueUpper = " << valueUpper;
    return(valueUpper);
}


double findLowerStepValue(double v, double step) {

    //qDebug() << __FUNCTION__ << "@ v    = " << v;
    //qDebug() << __FUNCTION__ << "  step = " << step;

    int int_vDivStep= static_cast<int>(v / step);
    //qDebug() << __FUNCTION__ << "  int_vDivStep = " << int_vDivStep;
    double closeStepValue = (static_cast<double>(int_vDivStep))*step;

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v - closeStepValue)) {
        //qDebug() << __FUNCTION__ << "  input value is very close to closeValue";
        //qDebug() << __FUNCTION__ << "=> lowerStepValue = " << closeStepValue;
        return(closeStepValue);
    }

    double valueLower = .0; //0 as invalid value here
    if (v < .0) {
        //qDebug() << __FUNCTION__ << "  v < .0 {";
        valueLower = static_cast<double>(int_vDivStep-1)*step;
    } else { // v > .0
        //qDebug() << __FUNCTION__ << "  v > .0 {";
        valueLower = closeStepValue;
    }

    //qDebug() << __FUNCTION__ << "=> lowerStepValue = " << valueLower;
    return(valueLower);
}

double findUpperStepValueFrom(double v, double tenPowered, double stepFraction) {

    qDebug() << __FUNCTION__ << "           v   = " << v;
    qDebug() << __FUNCTION__ << "  tenPowered   = " << tenPowered;
    qDebug() << __FUNCTION__ << "  stepFraction = " << stepFraction;

    double tenPoweredDivStepFraction = tenPowered / stepFraction;
    qDebug() << __FUNCTION__ << "  tenPoweredDivStepFraction = " << tenPoweredDivStepFraction;

    qDebug() << __FUNCTION__ << " intVDivTenPoweredDivStepFraction = static_cast<int>(" << v  << " / tenPoweredDivStepFraction";

    double intVDivTenPoweredDivStepFraction = static_cast<int>(v / tenPoweredDivStepFraction);
    qDebug() << __FUNCTION__ << "  intVDivTenPoweredDivStepFraction = " << intVDivTenPoweredDivStepFraction;

    double closeValue = intVDivTenPoweredDivStepFraction * tenPoweredDivStepFraction;
    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v - closeValue)) {
        qDebug() << __FUNCTION__ << "  input value is very close to closeValue";
        return(closeValue);
    }

    double valueUpper = .0; //0 as invalid value here
    if (v < .0) {
        qDebug() << __FUNCTION__ << "  v < .0 {";
        valueUpper = closeValue;
    } else { // v > .0
        qDebug() << __FUNCTION__ << "  v > .0 {";
        valueUpper = (intVDivTenPoweredDivStepFraction+1) * tenPoweredDivStepFraction;
    }
    qDebug() << __FUNCTION__ << " valueUpper = " << valueUpper;

    return(valueUpper);
}

double findLowerStepValueFrom(double v, double tenPowered, double stepFraction) {

    double tenPoweredDivStepFraction = tenPowered / stepFraction;
    //qDebug() << __FUNCTION__ << "  tenPoweredDivStepFraction = " << tenPoweredDivStepFraction;

    double intVDivTenPoweredDivStepFraction = static_cast<int>(v / tenPoweredDivStepFraction);
    //qDebug() << __FUNCTION__ << "  intVDivTenPoweredDivStepFraction = " << intVDivTenPoweredDivStepFraction;

    double closeValue = intVDivTenPoweredDivStepFraction * tenPoweredDivStepFraction;
    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v - closeValue)) {
        //qDebug() << "  input value is very close to closeValue";
        return(closeValue);
    }

    double valueLower = .0; //0 as invalid value here
    if (v < .0) {
        //qDebug() << __FUNCTION__ << "  v < .0 {";
        valueLower = (intVDivTenPoweredDivStepFraction-1) * tenPoweredDivStepFraction;
    } else { // v > .0
        //qDebug() << __FUNCTION__ << "  v > .0 {";
        valueLower = closeValue;
    }
    //qDebug() << __FUNCTION__ << "valueLower = " << valueLower;

    return(valueLower);
}

double powerOfTen(double value) {

    //qDebug() << __FUNCTION__;

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(value)) {
        return(0.); //@LP we return 0. in case of zero numeric input value  0x10°
    }

    //remove sign to be able to use natural logarithm to compute power of ten
    value = qAbs(value);

    qreal log10_ofValue = qLn(value)/qLn(10.0);
    //qDebug() << "  log10_ofValue:" << log10_ofValue;

    int texel = .0;
    floorfrac(static_cast<float>(log10_ofValue), &texel);

    // // qFloor(log10_ofValue);
    // // Incorrect result with qFloor, testing with value 3.0000, giving 2 instead of 3 :
    // // for (double testpower = 0.000001; testpower < 1000000; testpower*=10.0) {
    // //   qDebug() << __FUNCTION__ << "--- powerOfTen( " << testpower << " ) = " << powerOfTen(testpower);
    // // }
    // // Hence, use OIIO::floorfrac instead.
    // //
    // @#LP: add this test as unit test

    double powerOfTen_value = static_cast<double>(texel);
    //qDebug() << "  powerOfTen_value: " << powerOfTen_value;

    //fprintf(stdout, "log10_ofValue = %f , powerOfTen_value = %f\n", log10_ofValue, powerOfTen_value);
    //qDebug() << /* "; log10_ofValue:" << log10_ofValue << "  ; */ " powerOfTen_value: " << powerOfTen_value;

    return(powerOfTen_value);
}

void show_segment_7decimals(const S_Segment& segment) {
    qDebug() << "segment: "
             << QString::number(segment._ptA.x(), 'f', 7) << ", "
             << QString::number(segment._ptA.y(), 'f', 7)
             << " => "
             << QString::number(segment._ptB.x(), 'f', 7) << ", "
             << QString::number(segment._ptB.y(), 'f', 7);
}

void show_QPointF_7decimals(const QString& strPrefixMsg, const QPointF& qpf) {
    qDebug() << strPrefixMsg
             << QString::number(qpf.x(), 'f', 7) << ", "
             << QString::number(qpf.y(), 'f', 7);
}

void show_Vec2_dbl_7decimals(const QString& strPrefixMsg, const Vec2<double>& vec2dbl) {
    qDebug() << strPrefixMsg
             << QString::number(vec2dbl.x, 'f', 7) << ", "
             << QString::number(vec2dbl.y, 'f', 7);
}

int floatToInt_usingFloorfrac(float fValue) {
    int intVTexel = 0;
    floorfrac(fValue, &intVTexel);
    return(intVTexel);
}

