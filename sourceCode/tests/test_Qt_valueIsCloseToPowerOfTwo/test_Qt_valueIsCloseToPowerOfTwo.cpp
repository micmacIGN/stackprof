#include <QDebug>

#include "../../application/logic/zoomLevelImage/zoomHandler.h"


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


class ZoomHandler_test : public ZoomHandler {

    public:
    bool test_valueIsCloseToPowerOfTwo(float v, float deltaAround, float& adjustedV) {
        return(valueIsCloseToPowerOfTwo(v, deltaAround, adjustedV));
   }

};

int main(int argc, char *argv[]) {

    qInstallMessageHandler(myMessageOutput); // Install the handler

    ZoomHandler_test ZoomHdlrTest;

    for (float globalZoomFactor = 1.0f - (1.0f/1024.0f);
         globalZoomFactor <= 1024.f;
         globalZoomFactor += (1.0f/1024.0f)) {

        //valueIsPowerOfTwo
        qDebug() << "----- globalZoomFactor: " << globalZoomFactor << " with deltaAround: " <<  0.0075*globalZoomFactor;
        float adjustedV = .0;
        bool bIsPowerOfTwo = ZoomHdlrTest.test_valueIsCloseToPowerOfTwo(globalZoomFactor, 0.0075f*globalZoomFactor, adjustedV);
        qDebug() << "      bIsPowerOfTwo: " << bIsPowerOfTwo << "=> adjustedV = " << adjustedV;
    }

    return(0);

}
