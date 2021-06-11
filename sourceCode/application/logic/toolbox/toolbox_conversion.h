#ifndef TOOLBOX_CONVERSION_H
#define TOOLBOX_CONVERSION_H

class QString;
#include <QtGlobal>

#include <QPointF>

//QString qrealToRealNumberPrecision(qreal value, int realNumberPrecision);

bool qrealToRealNumberFixedPrecision_f(qreal value, int realNumberPrecision, qreal& qrealValueOut);

//QString doubleToQStringPrecision(double v, int precision);

QString doubleToQStringPrecision_f_amountOfDecimal(double v, int precision);
QString doubleToQStringPrecision_f_totalDigitNumber(double v, int precision);

QString doubleToQStringPrecision_g(double v, int precision);
QString doubleToQStringPrecision_justFit(double v, int precision);

QString doubleToQStringFixedPrecision(double value, int realNumberPrecision);

//QString floatToQStringPrecision_f(float value, int precision);

bool qPointFtoNumberFixedPrecision(const QPointF& qpfIn, int realNumberPrecision, QPointF& qpfOut);

QString removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(const QString& strIn);

bool string_isDecimalDigitsOnly(const QString &str);
bool string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(const QString& str, int totalCharIsEqualOrUnder);
bool string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(const QString& str, int totalCharIsEqualOrUnder,
                                                                    bool &bIsNotStringNumericZeroValue,
                                                                    bool &bTotalCharIsEqualOrUnder);

bool stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(const QString& str, int maxBeforeDot, int maxAfterDot);

bool isConformToFormatPrecision(double dValue, int maximumDecimalForStringConversion, int maxCharCountInString);
bool isConformToFormatPrecision_zeroValueIsAccepted(double dValue, int maximumDecimalForStringConversion, int maxCharCountInString,
                                                    bool& bIsNotStringNumericZeroValue);


bool floatToDoubleWithReducedPrecision(double dValue, int precision, double& dValueOut);

/*
QString doubleToQStringFixedPrecision(double value, int realNumberPrecision);


void test_with_numeric_limits(double value);
void test_with_boost_lexical_cast(double value);
*/

#endif // TOOLBOX_CONVERSION_H
