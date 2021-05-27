#ifndef MATH_TOOLBOX_H
#define MATH_TOOLBOX_H

#include <QPointF>

#include "../model/core/S_Segment.h" //@#LP move this file ?

bool doubleValuesAreClose(double v1, double v2, double deltaAround);

//bool valueIsCloseToZero(double v);
bool valueIsCloseToZero_deltaLowerThan1ExpMinus14(double v);

bool bCheckDoubleIsEven(double v);

double findUpper010ValueFrom(double v, double powerOfTen);
double findLower010ValueFrom(double v, double powerOfTen);

double findUpperStepValue(double v, double step);
double findLowerStepValue(double v, double step);

double findUpperStepValueFrom(double v, double tenPowered, double stepFraction);
double findLowerStepValueFrom(double v, double tenPowered, double stepFraction);

//QPointF normalizeQPointFUnitVectorIfNeed(const QPointF& qpfUnitVectorDirection);

double powerOfTen(double value);

void show_segment_7decimals(const S_Segment& segment);
void show_QPointF_7decimals(const QString& strPrefixMsg, const QPointF& qpf);

#include "ImathVec.h"
using namespace IMATH_INTERNAL_NAMESPACE;
void show_Vec2_dbl_7decimals(const QString& strPrefixMsg, const Vec2<double>& vec2dbl);

int floatToInt_usingFloorfrac(float fValue);

//double degToRad(double deg);

#endif // MATH_TOOLBOX_H
