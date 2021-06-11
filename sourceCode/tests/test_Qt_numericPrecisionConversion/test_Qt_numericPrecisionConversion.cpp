#include <QDebug>

#include <iostream>
#include <sstream>

#include <iomanip>
#include <cmath>
#include <limits>

using namespace std;

//#TagToFindAboutReleaseDeliveryOption
#define DEF_APP_DELIVERY_RELEASE

#include "../../application/logic/toolbox/toolbox_conversion.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Dbg: %s\n", localMsg.constData());
        //fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "Inf: %s\n", localMsg.constData());
        //fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stdout, "warning: %s\n", localMsg.constData());
        //fprintf(stdout, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "critical: %s\n", localMsg.constData());
        //fprintf(stdout, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stdout, "fatal: %s\n", localMsg.constData());
        //fprintf(stdout, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

void check_conversionWithPrecision(double dv1) {
/*
    cout << "input as default:" << " = "  << dv1 << endl;

    //test_with_numeric_limits(dv1);
    //test_with_boost_lexical_cast(dv1);

    for (int i = 20; i > -1; i--) {
        cout << "input as 'not fixed' with setprecision(" << i << " = "               << std::setprecision(i) << dv1 << endl;
    }
    cout << endl;
    for (int i = 20; i > -1; i--) {
        cout << "input as     'fixed' with setprecision(" << i << " = " << std::fixed << std::setprecision(i) << dv1 << endl;
    }
    cout << endl;
*/
    for (int i = 20; i > -1; i--) {
        qDebug() << "---" << endl;
        qDebug() << "i = " << i << endl;

        //double dvFixedP_out = .0;
        //if (qrealToRealNumberFixedPrecision(dv1, i, dvFixedP_out)) {
        //    cout << "qrealToRealNumberFixedPrecision => dvFixedP_out:          " << std::setprecision(i) << dvFixedP_out << endl;
        //} else {
        //    cout << "qrealToRealNumberFixedPrecision => failed" << endl;
        //}

        /*
        QString strDv_out_f;
        strDv_out_f = doubleToQStringPrecision_f(dv1, i);
            cout << "doubleToQStringPrecision           => strDv_out_f :         " << strDv_out_f.toStdString().c_str() << endl;

        QString strDv_out_g;
        strDv_out_g = doubleToQStringPrecision_f(dv1, i);
            cout << "doubleToQStringPrecision           => strDv_out_g :         " << strDv_out_g.toStdString().c_str() << endl;
        */

        //QString strDv_out2;
        //strDv_out2 = doubleToQStringFixedPrecision(dv1, i);
        //    cout << "doubleToQStringFixedPrecision     => strDv_out2:          " << strDv_out2.toStdString().c_str() << endl;


        double qrealValueOut = .0;
        bool bReport = qrealToRealNumberFixedPrecision_f(dv1, i, qrealValueOut);
        qDebug()   << "qrealToRealNumberFixedPrecision_f  qrealValueOut     = " << qrealValueOut;

        QString      qstr_doubleToQStringPrecision_f_amountOfDecimal  =  doubleToQStringPrecision_f_amountOfDecimal(dv1, i);
        qDebug() << "qstr_doubleToQStringPrecision_f       = " << qstr_doubleToQStringPrecision_f_amountOfDecimal;

        QString      qstr_doubleToQStringPrecision_justFit =           doubleToQStringPrecision_justFit(dv1, i);
        qDebug() << "qstr_doubleToQStringPrecision_justFit = " << qstr_doubleToQStringPrecision_justFit;

        QString      qstr_doubleToQStringPrecision_g =            doubleToQStringPrecision_g(dv1, i);
        qDebug() << "qstr_doubleToQStringPrecision_g       = " << qstr_doubleToQStringPrecision_g;

        QString      qstr_doubleToQStringFixedPrecision    =           doubleToQStringFixedPrecision(dv1, i);
        qDebug() << "qstr_doubleToQStringFixedPrecision    = " << qstr_doubleToQStringFixedPrecision;

    }
}

void check_conversionWithPrecision_testlist() {

    cout << "The  default precision is " << std::cout.precision() << endl;
    cout << "---" << endl;
    //test_with_numeric_limits    (3.14159265358979);
    /*test_with_boost_lexical_cast(3.14159265358979);
    test_with_boost_lexical_cast(1.01);
    test_with_boost_lexical_cast(1.1);*/
    cout << "---" << endl;

    cout << "dv1 ((in source code: 129.01234567890123))" << endl;
    double dv1 =                   129.01234567890123;//456789;
    check_conversionWithPrecision(dv1);
    /*
    cout << "dv2 ((in source code: 24.987))" << endl;
    double dv2 =                   24.987;
    check_conversionWithPrecision(dv2);

    cout << "dv3 ((in source code: -1.019))" << endl;
    double dv3 =                   -1.019;
    check_conversionWithPrecision(dv3);
    */

    cout << "dv4 ((in source code: 1.1234567))" << endl;
    double dv4 =                   1.1234567;
    check_conversionWithPrecision(dv4);

    cout << "dv5 ((in source code: 4567.891))" << endl;
    double dv5 =                   4567.891;
    check_conversionWithPrecision(dv5);

    cout << "dv6 ((in source code: 0.000001))" << endl;
    double dv6 =                   0.000001;
    check_conversionWithPrecision(dv6);

}

void test_removeUselessZerosInString() {

    QVector<QString> qstrTests = { "",
                                   ".", "..",
                                   "0",
                                   ".0", "0.", ".0.", "0.0",
                                   ".1234", "1234.",

                                   ".12034", "12034.",
                                   ".01234", "12340.",

                                   ".120034", "120034.",
                                   ".001234", "123400.",

                                   "01234",
                                   "12340",
                                   "12034",

                                   "001234",
                                   "123400",
                                   "120034",

                                   "00123400",
                                     "12003400",
                                   "00120034",
                                   "0012003400",

                                   "00.123400",
                                    "0.123400",
                                      "12.003400",
                                     "120.034",
                                     "120.340",
                                     "120.3400"
                                 };

    for(auto str: qstrTests) {
        qDebug() << "str = " << str << " => " << removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(str);
    }

}


bool test_precision_micmacStep_spatialResolution_values();
bool test_precision_spatialResolution_or_micmac(const QVector<double>& qvectDoubleToTest, bool bTestValuesAreValid);

bool test_precision_rejectionThreshold_values();
bool test_precision_rejectionThreshold(const QVector<double>& qvectDoubleToTest, bool bTestValuesAreValid);

void test_behavior_of_floatToDoubleWithReducedPrecision_usingInputString();



int main(int argc, char *argv[]) {

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

    test_behavior_of_floatToDoubleWithReducedPrecision_usingInputString();

    test_removeUselessZerosInString();

    check_conversionWithPrecision_testlist();

    double out = 0;
    floatToDoubleWithReducedPrecision(4567.8910, 4, out);
    qDebug() << __FUNCTION__ << "out = " << out;

    qreal out2 = 0;
    qrealToRealNumberFixedPrecision_f(4567.8910, 4, out2);

    //total amount of number used to convert the value (precision)
    //double value_= -10.0000190;
    double value_= -0.000019123456789;
    int precision_ = 7;

    std::stringstream strStreamOut;
    strStreamOut << std::setprecision(precision_) << value_;
    qDebug() << "stringstream << std::setprecision => fromStdString :" << QString::fromStdString(strStreamOut.str().c_str());

    QString qstrv = QString::number(value_, 'f', precision_); //no exponent
    qDebug() << "QString::number(f, precision) => " << qstrv;


    qDebug() << "test_micmacStep_spatialResolution_values & test_rejectionThreshold_values";

    bool bPass = true;
    bPass &= test_precision_micmacStep_spatialResolution_values();
    bPass &= test_precision_rejectionThreshold_values();

    qDebug() << __FUNCTION__ << "  bPass = " << bPass;

    return(0);

}

//floatToDoubleWithReducedPrecision handles correctly string numeric content to numeric value when < 1000
//numeric values > 999.999 with decimal tends to add extra 0.0001 which fails the remove useless zero in decimals operation
//example with string "1234.56" => 1234.56 => "1234.5601"
void test_behavior_of_floatToDoubleWithReducedPrecision_usingInputString() {

    bool bOk = false;
    QStringList qStrListV= { "1.2345" ,
                             "1.23450",
                             "12.345" ,
                             "12.3450",
                             "123.45" ,
                             "123.450",
                             "123.456",
                             "123.4560",
                             "1234.56" ,
                             "1234.560",
                             "12345.6" ,
                             "12345.60",
                             "12345.600",

                             "1000",
                             "1000.0",
                             "1000.01",
                             "1000.001",
                             "1000.0001",

                             "1000.056",
                             "1000.56",
                             "1200.56",

                             "0.0001",
                             "999.99",


                             "9.9001",
                             "999.999",

                             "99.990",
                             "99.990",

                             "999.009",

                             "999",
                             "999.0",
                             "999.00",

                             "999.56",

                           };


    for(auto qStrV: qStrListV) {

        qDebug() << "-_-_-_\n";
        qDebug() << "qDebug qStrV: " << qStrV;


        float fv = qStrV.toFloat(&bOk);

        qDebug() << "qDebug fv #1: " << fv;

        double double_micmacStep_forOutJson = 1.0;
        floatToDoubleWithReducedPrecision(static_cast<double>(fv), 4, double_micmacStep_forOutJson);

        //cout     << "cout fv: " << fv;
        qDebug() << "qDebug fv #2: " << fv;

    }
}


bool test_precision_micmacStep_spatialResolution_values() {

    QVector<double> qvectDoubleToTest_mustPass = {

        1,
        1.,
        1.0,
        1.00,
        1.000,
        1.0000,

        0.1,
        0.01,
        0.001,
        0.0001,

        0.10,
        0.010,
        0.0010,

        0.1000,
        0.0100,
        0.0010,
        0.00010,

        0.10000,
        0.01000,
        0.00100,
        0.000100,

        0.15,
        0.015,
        0.0015,

        0.150,
        0.1500,
        0.0150,
        0.0015,

        0.01500,
        0.00150,

        0.12,
        0.123,
        0.1234,

        0.10000,
        0.12000,
        0.12300,
        0.12340,

        1.0009,

        1.1,
        1.12,
        1.123,
        1.1234,

        12.1,
        12.12,
        12.123,
        12.1234,

        1.10,
        1.120,
        1.1230,
        1.12340,

        12.100,
        12.1200,
        12.12300,
        12.123400,

        12,
        12.,
        12.0,
        12.00,
        12.000,
        12.0000,
        12.00000,

        12.0015,

        123.,
        123.0,
        123.00,
        123.000,
        123.0000,
        123.00000,

        123.,
        123.0,
        123.01,
        123.001,

        123.1,
        123.12,
        123.123,

        123.0,
        123.00,
        123.010,
        123.0010,

        123.10,
        123.120,
        123.1230,

    };

    QVector<double> qvectDoubleToTest_mustFail = {

        0,
        0.,
        0.0,
        0.00,
        0.000,

        -0,
        -.0,
        -0.0,

        123.0001,
        123.00015,
        123.1234,
        123.1234000,

        123.12345,
        123.123456,
        12.12345,
        1.12345,
        0.12345,
        0.123456,
    };


    bool bAllTestPass = true;
    bAllTestPass &= test_precision_spatialResolution_or_micmac(qvectDoubleToTest_mustPass, true);
    bAllTestPass &= test_precision_spatialResolution_or_micmac(qvectDoubleToTest_mustFail, false);

    qDebug() << __FUNCTION__ << "  bAllTestPass = " << bAllTestPass;

    return(bAllTestPass);

}


bool test_precision_spatialResolution_or_micmac(const QVector<double>& qvectDoubleToTest, bool bTestValuesAreValid) {

    bool bAllValidPass = true;
    bool bAllInvalidPass = true;

    for (auto dValue : qvectDoubleToTest) {

        qDebug() << __FUNCTION__ << "---------------";

        //reject condition #2
        if (!isConformToFormatPrecision(dValue, 4, 7)) {

            if (bTestValuesAreValid) {
                bAllValidPass &= false;
                qDebug() << __FUNCTION__ << "#2 failed on value " << dValue << ": should be valid";
            } else {
               //do nothing
            }

        } else {
            if (bTestValuesAreValid) {
                //do nothing
            } else {
                bAllInvalidPass &= false;
                qDebug() << __FUNCTION__ << "#2 failed on value " << dValue << ": should be invalid";
            }
        }
    }

    if (bTestValuesAreValid) {
        return(bAllValidPass);
    }
    return(bAllInvalidPass);
}



bool test_precision_rejectionThreshold_values() {

    QVector<double> qvectDoubleToTest_mustPass = {

        0,
        0.,
        0.0,
        0.00,
        0.000,

        -0,
        -.0,
        -0.0,
        -0.00000,

        0.1,
        0.01,
        0.001,

        0.10,
        0.010,
        0.0010,

        0.100,
        0.0100,
        0.00100,

        1,
        1.,
        1.0,
        1.00,
        1.000,
        1.0000,
        1.009,

        1.1,
        1.12,
        1.123,

        12,
        12.,
        12.0,
        12.00,
        12.000,
        12.0000,
        12.00000,

        12.1,
        12.10,
        12.100,
        12.100000,
        12.12,
        12.120,
        12.1200,
        12.12000,
        12.120000,

        123.,
        123.0,
        123.00,
        123.000,
        123.0000,
        123.00000,

        123.1,
        123.10,
        123.100,

    };

    QVector<double> qvectDoubleToTest_mustFail = {

        1.1234,
        1.12345,
        1.123456,

        12.123,
        12.1230,
        12.1234,
        12.12345,
        12.123456,

        12.1230,
        12.12300,

        12.12340,
        12.123450,
        12.1234560,

        123.01,
        123.12,
        123.123,
        123.1234,
        123.12345,
        123.123456,

        123.010,
        123.0100,

        123.123000,
        123.123450,
        123.1234560,

        1.12345,
        0.12345,
        0.123456,
    };


    bool bAllTestPass = true;
    bAllTestPass &= test_precision_rejectionThreshold(qvectDoubleToTest_mustPass, true);
    bAllTestPass &= test_precision_rejectionThreshold(qvectDoubleToTest_mustFail, false);

    qDebug() << __FUNCTION__ << "  bAllTestPass = " << bAllTestPass;

    return(bAllTestPass);

}



bool test_precision_rejectionThreshold(const QVector<double>& qvectDoubleToTest, bool bTestValuesAreValid) {

    bool bAllValidPass = true;
    bool bAllInvalidPass = true;

    for (auto dValue : qvectDoubleToTest) {

        qDebug() << __FUNCTION__ << "---------------";

        //reject condition #2
        bool bIsNotStringNumericZeroValue = false;
        if (!isConformToFormatPrecision_zeroValueIsAccepted(dValue, 3, 5, bIsNotStringNumericZeroValue)) {
            if (bTestValuesAreValid) {
                bAllValidPass &= false;
                qDebug() << __FUNCTION__ << "#2 failed on value " << dValue << ": should be valid";
            } else {
               //do nothing
            }

        } else {
            if (bTestValuesAreValid) {
                //do nothing
            } else {
                bAllInvalidPass &= false;
                qDebug() << __FUNCTION__ << "#2 failed on value " << dValue << ": should be valid";
            }
        }
    }

    if (bTestValuesAreValid) {
        return(bAllValidPass);
    }
    return(bAllInvalidPass);
}
