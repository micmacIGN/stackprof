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


int main(int argc, char *argv[]) {

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

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

    return(0);

}


