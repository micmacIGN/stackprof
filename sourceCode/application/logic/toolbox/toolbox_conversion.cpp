#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <QString>

#include <QDebug>

#include <QRegularExpression>

#include "toolbox_conversion.h"

//amount of decimal is fixed
bool qrealToRealNumberFixedPrecision_f(qreal value, int realNumberPrecision, qreal& qrealValueOut) {

    QString qstr = doubleToQStringPrecision_f_amountOfDecimal(value, realNumberPrecision);
    qDebug()  << __FUNCTION__<< "qstr =" << qstr;

    bool bConversionOk = false;
    qreal qrealValue = qstr.toDouble(&bConversionOk);
    if (bConversionOk) {
        qrealValueOut = qrealValue;
    }
    return(bConversionOk);
}

//amount of decimal used to convert the value (precision)
QString doubleToQStringPrecision_f_amountOfDecimal(double value, int precision) {
    QString qstrv = QString::number(value, 'f', precision); //no exponent
    return(qstrv);
}

//total amount of number used to convert the value (precision)
QString doubleToQStringPrecision_f_totalDigitNumber(double value, int precision) {
    std::stringstream strStreamOut;
    strStreamOut << std::setprecision(precision) << value;
    //std::cout << __FUNCTION__ << " strStreamOut.str().c_str() :     " << strStreamOut.str().c_str() << std::endl;
    return(QString::fromStdString(strStreamOut.str().c_str()));
}

QString doubleToQStringPrecision_justFit(double value, int precision) {
    std::stringstream strStreamOut;
    strStreamOut << std::setprecision(precision) << value;
    //std::cout << __FUNCTION__ << " strStreamOut.str().c_str() :     " << strStreamOut.str().c_str() << std::endl;

    QString qstr_streamOut = QString::fromStdString(strStreamOut.str().c_str());

    qDebug() << __FUNCTION__ << " qstr_streamOut = " << qstr_streamOut;

    return(qstr_streamOut);
}

QString doubleToQStringPrecision_g(double value, int precision) {
    QString qstrv = QString::number(value, 'g', precision);
    return(qstrv);
}

#include <limits>
typedef std::numeric_limits< double > typedef_dblim;

void test_with_numeric_limits(double value) {
    ////std::cout.precision(typedef_dblim::max_digits10);
    //std::cout << __FUNCTION__ << "value: " <<  std::setprecision(typedef_dblim::max_digits10) << value << std::endl;
}

//total amount of number used to convert the value (precision
QString doubleToQStringFixedPrecision(double value, int realNumberPrecision) {
    std::stringstream strStreamOut;
    strStreamOut << std::fixed << std::setprecision(realNumberPrecision) << value;

    //std::cout << __FUNCTION__ << " strStreamOut.str().c_str(): " << strStreamOut.str().c_str() << std::endl;

    return(QString::fromStdString(strStreamOut.str().c_str()));
}

bool qPointFtoNumberFixedPrecision(const QPointF& qpfIn, int realNumberPrecision, QPointF& qpfOut) {

    qreal xCuttedOut = .0;
    qreal yCuttedOut = .0;

    bool bOK = true;

    bOK &= qrealToRealNumberFixedPrecision_f(qpfIn.x(), realNumberPrecision, xCuttedOut);
    bOK &= qrealToRealNumberFixedPrecision_f(qpfIn.y(), realNumberPrecision, yCuttedOut);

    if (bOK) {
        qpfOut = {xCuttedOut, yCuttedOut};
    }
    return(bOK);
}

QString removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(const QString& strIn) {

    qDebug()  << __FUNCTION__<< "-------------";
    qDebug()  << __FUNCTION__<< "strIn = " << strIn;

    QString strOut;

    int strIn_size = strIn.size();

    if (!strIn_size) {
        qDebug()  << __FUNCTION__<< "if (!strIn_size) {";
        return(strOut);
    }

    int idxFirstNonZero = -1;
    bool bFirstNonZeroFound = false;
    for(int i = 0; i < strIn_size; i++) {

            if (strIn[i] != '0') {
                bFirstNonZeroFound = true;
                idxFirstNonZero = i;
                break;
            }

    }

    //LastNonZero and lastIndexOfDot
    int idxLastNonZero = -1;
    int lastIndexOfDot = strIn.lastIndexOf('.');
    if (lastIndexOfDot == -1) {
        idxLastNonZero = strIn_size - 1;
    } else {
        for(int i = strIn_size - 1; i >= lastIndexOfDot; i--) {
            if (strIn[i] != '0') {
                idxLastNonZero = i;
                break;
            }
        }
    }

    qDebug()  << __FUNCTION__<< "bFirstNonZeroFound: " << bFirstNonZeroFound << "; idxFirstNonZero = " << idxFirstNonZero;
    qDebug()  << __FUNCTION__<< "lastIndexOfDot : " << lastIndexOfDot << "; idxLastNonZero  = " << idxLastNonZero;

    if (!bFirstNonZeroFound) {
        qDebug()  << __FUNCTION__<< "return(""0"") case first";
        return("0");
    }

    int countDot   = strIn.count('.');   
    int countZero  = strIn.count('0');
    qDebug()  << __FUNCTION__<< "countDot   = " << countDot;
    qDebug()  << __FUNCTION__<< "countZero  = " << countZero;
    if ((countDot + countZero) == strIn_size) {
        qDebug()  << __FUNCTION__<< "(countDot + countZero) == strIn_size) = " << strIn_size;
        return("0");
    }

    if (idxLastNonZero == idxFirstNonZero) {
        qDebug()  << __FUNCTION__<< "return one char (as string) : " << QString(strIn[idxFirstNonZero]);
        return(QString(strIn[idxFirstNonZero]));
    }

    int nbCharForMid = idxLastNonZero - idxFirstNonZero + 1;

    //this case should never happens from goal of of this function:
    if (strIn[idxLastNonZero] == '.') {
        nbCharForMid--;
    }

    strOut = strIn.mid(idxFirstNonZero, nbCharForMid);

    if (strOut[0] == '.') {
        strOut = '0' + strOut;
        qDebug()  << __FUNCTION__ << "add a first zero before . which if the first char of the string";
    }

    qDebug()  << __FUNCTION__<< "return: mid(" << idxFirstNonZero << ", " << nbCharForMid;

    return(strOut);
}


bool string_isDecimalDigitsOnly(const QString &str) {

    QRegularExpression qregexp_decimalDigitsOnly(R"(^(\d+\.)?\d+$)");

    QRegularExpressionMatch match = qregexp_decimalDigitsOnly.match(str);
    return(match.hasMatch());
}


bool string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(const QString& str, int totalCharIsEqualOrUnder) {
    bool bIsNotStringNumericZeroValue = false;
    bool bTotalCharIsEqualOrUnder = false;
    bool bReport = string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(str, totalCharIsEqualOrUnder, bIsNotStringNumericZeroValue, bTotalCharIsEqualOrUnder);
    return(bReport);
}

bool string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(const QString& str, int totalCharIsEqualOrUnder,
                                                                    bool &bIsNotStringNumericZeroValue,
                                                                    bool &bTotalCharIsEqualOrUnder) {

    qDebug()  << __FUNCTION__<< "return: str =" << str;
    qDebug()  << __FUNCTION__<< "totalCharIsEqualOrUnder to check:" << totalCharIsEqualOrUnder;

    bIsNotStringNumericZeroValue = false;
    bTotalCharIsEqualOrUnder = false;

    int textSize = str.size();
    if (!textSize) {
        return(false);
    }
    if (textSize > totalCharIsEqualOrUnder) {
        return(false);
    }
    bTotalCharIsEqualOrUnder = true;

    int countZeroAndDot = 0;
    for (int i = 0; i < textSize; i++) {       
        if ((str[i] == '0')||(str[i] == '.')) {
           countZeroAndDot++;
        }
    }
    if (countZeroAndDot != textSize) {
        bIsNotStringNumericZeroValue = true;
    }

    return(bIsNotStringNumericZeroValue && bTotalCharIsEqualOrUnder);
}

bool stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(const QString& str, int maxBeforeDot, int maxAfterDot) {

    qDebug() << __FUNCTION__ << "str = " << str;
    qDebug() << __FUNCTION__ << "maxBeforeDot = " << maxBeforeDot;
    qDebug() << __FUNCTION__ << "maxAfterDot = " << maxAfterDot;

    int textSize = str.size();
    if (!textSize) {
        return(false);
    }

    if (str.indexOf('.') == -1) {
        return(textSize <= maxBeforeDot);
    }

    QString str_baseRegexp_maxBeforeDotMaxAfterDot = R"(^(\d{1,_MaxBeforeDot_}\.)?\d{1,_maxAfterDot_}$)";

    QString str_regexp_maxBeforeDotMaxAfterDot = str_baseRegexp_maxBeforeDotMaxAfterDot.replace("_MaxBeforeDot_", QString::number(maxBeforeDot));
    str_regexp_maxBeforeDotMaxAfterDot         =     str_regexp_maxBeforeDotMaxAfterDot.replace("_maxAfterDot_", QString::number(maxAfterDot));

    QRegularExpression qregexp_decimalDigitsOnly(str_regexp_maxBeforeDotMaxAfterDot);
    QRegularExpressionMatch match = qregexp_decimalDigitsOnly.match(str);
    return(match.hasMatch());
}

bool isConformToFormatPrecision(double dValue, int maximumDecimalForStringConversion, int maxCharCountInString) {
    //convert using maximum decimal with more than maximumDecimalForStringConversion
    QString strOf_dValue = doubleToQStringPrecision_f_amountOfDecimal(dValue, maximumDecimalForStringConversion+4);
    qDebug() << __FUNCTION__ << "#1 strOf_dValue = " << strOf_dValue;

    //remove useless zero
    strOf_dValue = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(strOf_dValue);
    qDebug() << __FUNCTION__ << "#2 strOf_dValue = " << strOf_dValue;

    bool bCompliant = stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(strOf_dValue,
                                                                                maxCharCountInString,
                                                                                maximumDecimalForStringConversion);
    qDebug() << __FUNCTION__ << "bCompliant: " << bCompliant;
    if (!bCompliant) {
        return(false);
    }

    return(string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(strOf_dValue, maxCharCountInString));
}

bool isConformToFormatPrecision_zeroValueIsAccepted(double dValue, int maximumDecimalForStringConversion, int maxCharCountInString,
                                                    bool& bIsNotStringNumericZeroValue) {
    //convert using maximum decimal with more than maximumDecimalForStringConversion
    QString strOf_dValue = doubleToQStringPrecision_f_amountOfDecimal(dValue, maximumDecimalForStringConversion+4);
    qDebug() << __FUNCTION__ << "#1 strOf_dValue = " << strOf_dValue;

    //remove useless zero
    strOf_dValue = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(strOf_dValue);
    qDebug() << __FUNCTION__ << "#2 strOf_dValue = " << strOf_dValue;

    bool bCompliant = stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(strOf_dValue,
                                                                                maxCharCountInString,
                                                                                maximumDecimalForStringConversion);
    qDebug() << __FUNCTION__ << "bCompliant: " << bCompliant;
    if (!bCompliant) {        
        return(false);
    }

    /*bool*/ bIsNotStringNumericZeroValue = false;
    bool bTotalCharIsEqualOrUnder = false;
    /*bool bReport = */string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(strOf_dValue, maxCharCountInString,
                                                                                      bIsNotStringNumericZeroValue,
                                                                                      bTotalCharIsEqualOrUnder);
    if (!bTotalCharIsEqualOrUnder) {
        return(false);
    }
    //here if bTotalCharIsEqualOrUnder is true

    //bIsNotStringNumericZeroValue true of false, it's ok
    return(true);
}



bool floatToDoubleWithReducedPrecision(double dValue, int precision, double& dValueOut) {

    qDebug() << __FUNCTION__ << "#0 strOf_dValue = " << dValue;
    qDebug() << __FUNCTION__ <<  "#0 precision = " << precision;

    qDebug() << __FUNCTION__ << "dev-test: " << QString::number(dValue, 'f', precision); //no exponent

    //convert using maximum decimal as maximumDecimalForStringConversion
    //QString strOf_dValue = doubleToQStringPrecision_justFit(dValue, precision);
    QString strOf_dValue = doubleToQStringPrecision_f_amountOfDecimal(dValue, precision);

    qDebug() << __FUNCTION__ <<  "#1 strOf_dValue = " << strOf_dValue;

    //remove useless zero
    strOf_dValue = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(strOf_dValue);

    qDebug() << __FUNCTION__ <<  "#2 strOf_dValue = " << strOf_dValue;

    int indexOfDot = strOf_dValue.lastIndexOf('.');
    int decimalPrecision = 0;
    if (indexOfDot != -1) {
        decimalPrecision  = strOf_dValue.size() - indexOfDot -1;
    }
    qDebug() << __FUNCTION__ <<  "#3 decimalPrecision = " << decimalPrecision;
    bool bfixedPrecision = qrealToRealNumberFixedPrecision_f(dValue, decimalPrecision, dValueOut);

    qDebug() << __FUNCTION__ <<  "#4 dValueOut = " << dValueOut;

    return(bfixedPrecision);
}
