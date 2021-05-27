//#include <QCoreApplication>

#include <QDebug>

#include <QVector>
#include <QPointF>

#include <math.h>

#include "../../application/logic/toolbox/toolbox_math_geometry.h"
#include "../../application/logic/model/core/S_Segment.h"

#include "ImathVec.h"
using namespace IMATH_INTERNAL_NAMESPACE;

int test_relocation();

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


int main(int argc, char *argv[]) {
    //QCoreApplication a(argc, argv);
    //return a.exec();

    qInstallMessageHandler(myMessageOutput); // Install the handler

    test_relocation();
}


int test_relocation() {

    /*
     + + +
      \|/
     +-+-+
      /|\
     + + +
    */
    //testings in different directions for segment AB
    QVector<QPointF> qvectqpf_point_startBase {
        {0,-1},
        {0,0},
        {0,1},

        {1,-1},
        {1,0},
        {1,1},

        {-1,-1},
        {-1,0},
        {-1,1},

        {cos(     (M_PI/8.0)), sin(     (M_PI/8.0))},
        {cos(M_PI-(M_PI/8.0)), sin(M_PI-(M_PI/8.0))},
        {cos(M_PI+(M_PI/8.0)), sin(M_PI+(M_PI/8.0))},
        {cos(    -(M_PI/8.0)), sin(    -(M_PI/8.0))},
    };


    //and testing at different locations around zero for segment AB
    QVector<QPointF> qvectqpf_shiftForPointA_startBase = qvectqpf_point_startBase;

    QVector<double> qvectqpf_shiftMultiplier_forShiftForPointA { 1.0, 1.50, 3, 10};

    //and testing with different segment AB Length
    QVector<double>  qvectdbl_segmentLength { 1.0,
                                              2.0, 5.14, 13.723, 12345.678 };


    int count_OK_relocatedInsideSegment = 0;
    int count_OK_isAandRelocatedToA = 0;
    int count_OK_isBandRelocatedToB = 0;
    int count_OK_shouldNotBeRelocated = 0;

    int count_FAIL_relocatedInsideSegment = 0;
    int count_FAIL_isAandRelocatedToA = 0;
    int count_FAIL_isBandRelocatedToB = 0;
    int count_FAIL_shouldNotBeRelocated = 0;

    int total_tested = 0;
    int count_shouldBeRelocated = 0;
    int count_shouldNotBeRelocated = 0;

    int count_internalError = 0;

    for (auto pointA: qvectqpf_point_startBase) {
        for (auto shiftForPointA: qvectqpf_shiftForPointA_startBase) {
            for (auto shiftMultiplier: qvectqpf_shiftMultiplier_forShiftForPointA) {
                for (auto segmentLength: qvectdbl_segmentLength) {
                    double A_x = pointA.x() + shiftForPointA.x()*shiftMultiplier;
                    double A_y = pointA.y() + shiftForPointA.y()*shiftMultiplier;

                    for (auto pointB: qvectqpf_point_startBase) {

                        double B_x = A_x + pointB.x()*segmentLength;
                        double B_y = A_y + pointB.x()*segmentLength;

                        S_Segment segmentAB;
                        segmentAB._ptA = {A_x, A_y};
                        segmentAB._ptB = {B_x, B_y};
                        segmentAB._bValid = true;

                        if (segmentAB.length() < 1.0) {
                            qDebug() << __FUNCTION__ <<  "    bypassed segmentAB due to length() < 1.0 :" << segmentAB._ptA << " => " << segmentAB._ptB;
                            continue;
                        }


                        //on A
                        QPointF qpf_toRelocate_0 {A_x, A_y};

                        //on B
                        QPointF qpf_toRelocate_1 {B_x, B_y};

                        //middle of segment AB
                        QPointF qpf_toRelocate_2 {(A_x+B_x)/2.0,
                                                  (A_y+B_y)/2.0};

                        //1/3 from A in segment AB
                        Vec2<double> vec2_AB {B_x - A_x,
                                              B_y - A_y };
                        Vec2<double> vec2_ABnorm = vec2_AB.normalized();

                        QPointF qpf_toRelocate_3 {A_x + 1.0/3.0 * vec2_ABnorm.x,
                                                  A_y + 1.0/3.0 * vec2_ABnorm.y};

                        //projectable on segment AB (close to the middle of segment AB)
                        QPointF qpf_toRelocate_4 {qpf_toRelocate_2.x() + 0.1 * vec2_AB.length(),
                                                  qpf_toRelocate_2.y() + 0.1 * vec2_AB.length()};

                        QPointF qpf_toRelocate_5 {qpf_toRelocate_2.x() + 0.1 * vec2_AB.length(),
                                                  qpf_toRelocate_2.y() - 0.1 * vec2_AB.length()};

                        QPointF qpf_toRelocate_6 {qpf_toRelocate_2.x() - 0.1 * vec2_AB.length(),
                                                  qpf_toRelocate_2.y() + 0.1 * vec2_AB.length()};

                        QPointF qpf_toRelocate_7 {qpf_toRelocate_2.x() - 0.1 * vec2_AB.length(),
                                                  qpf_toRelocate_2.y() - 0.1 * vec2_AB.length()};

                        // not projectable on segment
                        QPointF qpf_toRelocate_8 {A_x + 3.0 * vec2_AB.length() * vec2_ABnorm.x,
                                                  A_y + 3.0 * vec2_AB.length() * vec2_ABnorm.y};

                        QPointF qpf_toRelocate_9 {A_x - 4.0 * vec2_AB.length() * vec2_ABnorm.x,
                                                  A_y - 4.0 * vec2_AB.length() * vec2_ABnorm.y};

                        QVector<QPointF> qvectqpf_toRelocate;
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_0);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_1);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_2);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_3);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_4);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_5);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_6);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_7);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_8);
                        qvectqpf_toRelocate.push_back(qpf_toRelocate_9);

                        int idx_toRelocate = 0;
                        for (auto pointC: qvectqpf_toRelocate) {

                            qDebug() << __FUNCTION__ <<  "idx_toRelocate =" << idx_toRelocate;
                            qDebug() << __FUNCTION__ <<  "segmentAB = " << segmentAB._ptA << " => " << segmentAB._ptB;
                            qDebug() << __FUNCTION__ <<  "pointC = " << pointC;
                            QPointF qpf_relocated_pointC {0, 0};
                            e_ResultDetailsOfRelocatedPointInSegment eRDORPIS = relocatePointInSegment(pointC, segmentAB, qpf_relocated_pointC);

                            S_Segment segment_additionnalTest;

                            if (eRDORPIS == e_RDORPIS_pointABCnotAligned) {
                                count_internalError++;
                            } else {

                                if (  (eRDORPIS ==  e_RDORPIS_relocatedToA)
                                    ||(eRDORPIS ==  e_RDORPIS_relocatedToB)
                                    ||(eRDORPIS ==  e_RDORPIS_relocatedInsideSegment)) {
                                    qDebug() << __FUNCTION__ <<  "qpf_relocated_pointC =" << qpf_relocated_pointC;
                                }

                                switch (idx_toRelocate) {
                                    case 0: //A
                                        switch (eRDORPIS) {
                                            case e_RDORPIS_relocatedToA:
                                            case e_RDORPIS_relocatedInsideSegment:
                                                count_OK_isAandRelocatedToA++;
                                                break;

                                            case e_RDORPIS_notRelocated_AisCloser:
                                            case e_RDORPIS_relocatedToB:
                                            case e_RDORPIS_notRelocated_BisCloser:
                                                count_FAIL_isAandRelocatedToA++;
                                                qDebug() << "failure isA";
                                                break;

                                            default:
                                                break;
                                        }
                                        break;

                                    case 1: //B
                                        switch (eRDORPIS) {
                                            case e_RDORPIS_relocatedToB:
                                            case e_RDORPIS_relocatedInsideSegment:
                                                count_OK_isBandRelocatedToB++;
                                                break;

                                            case e_RDORPIS_relocatedToA:
                                            case e_RDORPIS_notRelocated_AisCloser:
                                            case e_RDORPIS_notRelocated_BisCloser:
                                                count_FAIL_isBandRelocatedToB++;
                                                qDebug() << "failure isB";
                                                break;
                                            default:
                                                break;
                                        }
                                        break;

                                    //inside segment
                                    case 2:
                                    case 3:
                                    case 4:
                                    case 5:
                                    case 6:
                                    case 7:
                                        switch (eRDORPIS) {
                                            case e_RDORPIS_relocatedInsideSegment:
                                                count_OK_relocatedInsideSegment++;
                                                if ( (idx_toRelocate == 2)
                                                    ||(idx_toRelocate == 3)) {

                                                    segment_additionnalTest._ptA = pointC;
                                                    segment_additionnalTest._ptB = qpf_relocated_pointC;
                                                    qDebug() << "distance  pointC to qpf_relocated_pointC for test point C already on the segment: "
                                                             << QString::number(segment_additionnalTest.length(), 'f', 14);
                                                }
                                                break;

                                            case e_RDORPIS_relocatedToA:
                                            case e_RDORPIS_notRelocated_AisCloser:
                                            case e_RDORPIS_relocatedToB:
                                            case e_RDORPIS_notRelocated_BisCloser:
                                                count_FAIL_relocatedInsideSegment++;
                                                qDebug() << "failure inside";
                                                break;
                                            default:
                                                break;
                                        }
                                        break;

                                    //not projectable points on segment:
                                    case 8:
                                    case 9:
                                        switch (eRDORPIS) {
                                            case e_RDORPIS_notRelocated_AisCloser:
                                            case e_RDORPIS_notRelocated_BisCloser:
                                                count_OK_shouldNotBeRelocated++;
                                                break;

                                            case e_RDORPIS_relocatedInsideSegment:
                                            case e_RDORPIS_relocatedToA:
                                            case e_RDORPIS_relocatedToB:
                                                count_FAIL_shouldNotBeRelocated++;
                                                qDebug() << "failure not projectable";
                                                break;

                                            default:
                                                break;
                                        }
                                        break;
                                    }
                                }
                                qDebug() << __FUNCTION__ <<  "---";

                                if (idx_toRelocate < 8) {
                                    count_shouldBeRelocated++;
                                } else {
                                    count_shouldNotBeRelocated++;
                                }
                                total_tested++;
                                idx_toRelocate++;
                        }
                    }
                }
            }
        }
    }

    qDebug() << __FUNCTION__ << "count_internalError                :" << count_internalError;
    qDebug() << __FUNCTION__ ;

    qDebug() << __FUNCTION__ << "  count_OK_isAandRelocatedToA      : " << count_OK_isAandRelocatedToA;
    qDebug() << __FUNCTION__ << "  count_FAIL_isAandRelocatedToA    : " << count_FAIL_isAandRelocatedToA;
    qDebug() << __FUNCTION__;

    qDebug() << __FUNCTION__ << "  count_OK_isBandRelocatedToB      : " << count_OK_isBandRelocatedToB;
    qDebug() << __FUNCTION__ << "  count_FAIL_isBandRelocatedToB    : " << count_FAIL_isBandRelocatedToB;
    qDebug() << __FUNCTION__;

    qDebug() << __FUNCTION__ << "  count_OK_relocatedInsideSegment  : " << count_OK_relocatedInsideSegment;
    qDebug() << __FUNCTION__ << "  count_FAIL_relocatedInsideSegment: " << count_FAIL_relocatedInsideSegment;
    qDebug() << __FUNCTION__;

    qDebug() << __FUNCTION__ << "count OK any relocated             : " << count_OK_isAandRelocatedToA
                                                                         + count_OK_isBandRelocatedToB
                                                                         + count_OK_relocatedInsideSegment;
    qDebug() << __FUNCTION__ << "count FAIL any relocated           : " << count_FAIL_isAandRelocatedToA
                                                                         + count_FAIL_isBandRelocatedToB
                                                                         + count_FAIL_relocatedInsideSegment;
    qDebug() << __FUNCTION__;

    qDebug() << __FUNCTION__ << "count_OK_shouldNotBeRelocated     : " << count_OK_shouldNotBeRelocated;
    qDebug() << __FUNCTION__ << "count_FAIL_shouldNotBeRelocated    : " << count_FAIL_shouldNotBeRelocated;
    qDebug() << __FUNCTION__;

    qDebug() << __FUNCTION__ << "total_tested                      : " << total_tested;
    qDebug() << __FUNCTION__ << "count_shouldBeRelocated           : " << count_shouldBeRelocated;
    qDebug() << __FUNCTION__ << "count_shouldNotBeRelocated        : " << count_shouldNotBeRelocated;

    /* test relocatePointInSegment #1

    {
        QPointF qpf_1 {7.0/4.0, 3.0};
        S_Segment segment;
        segment._ptA = {0,0};
        segment._ptB = {4,7};

        QPointF qpf_relocated {0, 0};
        if (relocatePointInSegment(qpf_1, segment, qpf_relocated)) {
            qDebug() << __FUNCTION__ <<  "qpf_relocated = " << qpf_relocated;
        }
    }

    {
        QPointF qpf_1 {1, 3.5};
        S_Segment segment;
        segment._ptA = {0,0};
        segment._ptB = {7,0};

        QPointF qpf_relocated {0, 0};
        if (relocatePointInSegment(qpf_1, segment, qpf_relocated)) {
            qDebug() << __FUNCTION__ <<  "qpf_relocated = " << qpf_relocated;
        }
    }

    {
        QPointF qpf_1 {1, 3.5};
        S_Segment segment;
        segment._ptA = {7,0};
        segment._ptB = {0,0};

        QPointF qpf_relocated {0, 0};
        if (relocatePointInSegment(qpf_1, segment, qpf_relocated)) {
            qDebug() << __FUNCTION__ <<  "qpf_relocated = " << qpf_relocated;
        }
    }


    {
        QPointF qpf_1 {1, -3.5};
        S_Segment segment;
        segment._ptA = {7,0};
        segment._ptB = {-2,-6};

        QPointF qpf_relocated {0, 0};
        if (relocatePointInSegment(qpf_1, segment, qpf_relocated)) {
            qDebug() << __FUNCTION__ <<  "qpf_relocated = " << qpf_relocated;
        }

    }*/

    return(0);
}
