#include <QDebug>

#include "../../application/logic/model/core/route.h"

//#TagToFindAboutReleaseDeliveryOption
#define DEF_APP_DELIVERY_RELEASE

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

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

    //QVector<QPointF> qvectQppf {
    //    {    .0,     .0},
    //    {    1.0,    .0},
    //    {    7.0,   9.0},
    //    { -120.0, 250.0},
    //};

    //check find middle
    {
        Route r1;
        bool bFirstPoint = false;
        r1.tryAddPointAtRouteEnd({ .0, .0}, bFirstPoint);
        r1.tryAddPointAtRouteEnd({1.0, .0}, bFirstPoint);
        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r1.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            {  .0, .0}, 0,
            { 1.0, .0}, 0,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        {
            QPointF qpfMustBeMiddlePoint_forward;
            int idxSegmentOfMustBeMiddlePoint_forward = -1;
            bReport = r1.findPointAtADistanceFromAnotherPoint_forwardDirection(
                {  .0, .0}, 0, 0.5,
                qpfMustBeMiddlePoint_forward, idxSegmentOfMustBeMiddlePoint_forward);

            qDebug() <<" forwardDirection:";
            qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint_forward;
            qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint_forward;
        }

        {
            QPointF qpfMustBeMiddlePoint_backward;
            int idxSegmentOfMustBeMiddlePoint_backward = -1;
            bReport = r1.findPointAtADistanceFromAnotherPoint_backwardDirection(
                { 1.0, .0}, 0, 0.5,
                qpfMustBeMiddlePoint_backward, idxSegmentOfMustBeMiddlePoint_backward);

            qDebug() <<" backwardDirection:";
            qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint_backward;
            qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint_backward;
        }



        {
            QPointF qpfMustFail_forward;
            int idxSegmentOfMustFail_forward = -1;
            bReport = r1.findPointAtADistanceFromAnotherPoint_forwardDirection(
                {  .0, .0}, 0, 1.1,
                qpfMustFail_forward, idxSegmentOfMustFail_forward);

            qDebug() <<" forwardDirection:";
            qDebug() << "  qpfMustFail_forward = " << qpfMustFail_forward;
            qDebug() << "  idxSegmentOfMustFail_forward = " << idxSegmentOfMustFail_forward;
        }

        {
            QPointF qpfMustFail_backward;
            int idxSegmentOfMustFail_backward = -1;
            bReport = r1.findPointAtADistanceFromAnotherPoint_backwardDirection(
                { 1.0, .0}, 0, 1.1,
                qpfMustFail_backward, idxSegmentOfMustFail_backward);

            qDebug() <<" backwardDirection:";
            qDebug() << "  qpfMustFail_backward = " << qpfMustFail_backward;
            qDebug() << "  idxSegmentOfMustFail_backward = " << idxSegmentOfMustFail_backward;
        }

    }

    //check find middle
    {
        Route r2;
        bool bFirstPoint = false;
        r2.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r2.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            {  .0,   .0}, 0,
            {10.0, 10.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        QPointF qpfMustBeMiddlePoint;
        int idxSegmentOfMustBeMiddlePoint = -1;
        bReport = r2.findPointAtADistanceFromAnotherPoint_forwardDirection(
            {  .0, .0}, 0, 10.0,
            qpfMustBeMiddlePoint, idxSegmentOfMustBeMiddlePoint);

        qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint;
        qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint;


        {
            QPointF qpfMustFail_forward;
            int idxSegmentOfMustFail_forward = -1;
            bReport = r2.findPointAtADistanceFromAnotherPoint_forwardDirection(
                {  .0, .0}, 0, 20.1,
                qpfMustFail_forward, idxSegmentOfMustFail_forward);

            qDebug() <<" forwardDirection:";
            qDebug() << "  qpfMustFail_forward = " << qpfMustFail_forward;
            qDebug() << "  idxSegmentOfMustFail_forward = " << idxSegmentOfMustFail_forward;
        }

        {
            QPointF qpfMustFail_backward;
            int idxSegmentOfMustFail_backward = -1;
            bReport = r2.findPointAtADistanceFromAnotherPoint_backwardDirection(
                { 10.0, 10.0}, 1, 20.1,
                qpfMustFail_backward, idxSegmentOfMustFail_backward);

            qDebug() <<" backwardDirection:";
            qDebug() << "  qpfMustFail_backward = " << qpfMustFail_backward;
            qDebug() << "  idxSegmentOfMustFail_backward = " << idxSegmentOfMustFail_backward;
        }

    }

    {
        Route r3;
        bool bFirstPoint = false;
        r3.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r3.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { 5.0,  .0}, 0,
            {10.0, 5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        QPointF qpfMustBeMiddlePoint;
        int idxSegmentOfMustBeMiddlePoint = -1;
        bReport = r3.findPointAtADistanceFromAnotherPoint_backwardDirection(
            {10.0, 10.0}, 1, 10.0,
            qpfMustBeMiddlePoint, idxSegmentOfMustBeMiddlePoint);

        qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint;
        qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint;
    }

    {
        Route r4;
        bool bFirstPoint = false;
        r4.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r4.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r4.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r4.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { 5.0,   .0}, 0,
            {10.0, 10.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

    }

    {
        Route r5;
        bool bFirstPoint = false;
        r5.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r5.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r5.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r5.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            {  .0, 0.0}, 0,
            {10.0, 5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }


    {
        Route r6;
        bool bFirstPoint = false;
        r6.tryAddPointAtRouteEnd({ -5.0, -5.0}, bFirstPoint);
        r6.tryAddPointAtRouteEnd({  0.0,   .0}, bFirstPoint);
        r6.tryAddPointAtRouteEnd({  5.0,  5.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r6.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { -5.0, -5.0}, 0,
            {  5.0,  5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }

    {
        Route r7;
        bool bFirstPoint = false;
        r7.tryAddPointAtRouteEnd({ -5.0, -5.0}, bFirstPoint);
        r7.tryAddPointAtRouteEnd({  0.0,   .0}, bFirstPoint);
        r7.tryAddPointAtRouteEnd({  5.0,  5.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r7.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { -0.0001, -0.0001}, 0,
            {  0.0001,  0.0001}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }

    return(0);
}

