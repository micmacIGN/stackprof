#include <QApplication>

#include <QDebug>

//---
//@LP contains call to Q_DECLARE_METATYPE, must be done before qRegisterMetaType< . . . >() call;
//
#include "model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"
#include "model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"  //contains call to Q_DECLARE_METATYPE, must be done before qRegisterMetaType<S_GraphicsParameters_Profil>();
#include "model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h"
//
//---

#include "logic/model/ApplicationModel.h"
#include "ui/ApplicationView.h"
#include "logic/controller/ApplicationController.h"

#include <QtGlobal>
#include <stdio.h>
#include <stdlib.h>

#include "logic/mathComputation/initGSL.h"


//#TagToFindAboutReleaseDeliveryOption
#define DEF_APP_DELIVERY_RELEASE


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
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

    qRegisterMetaType<S_Color_Thickness>();
    qRegisterMetaType<S_Color_Thickness_Antialiasing>();
    qRegisterMetaType<S_Color_Antialiasing>();
    qRegisterMetaType<S_GraphicsParameters_ProfilCurveAndEnvelopArea>();
    qRegisterMetaType<S_GraphicsParameters_LinearRegression>();

    qRegisterMetaType<S_GraphicsParameters_Profil>();

    qRegisterMetaType<S_Thickness_Antialiasing>();

    qRegisterMetaType<S_GraphicsParameters_ItemOnImageView>();

#ifndef NAN
    #error "compiler has to support NAN"
#endif

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

    //reject application start if size of int is lower than 32 bits (due to image width and height higher than 16 bits)
    qDebug() << __FUNCTION__ << "size of int: " << sizeof(int)*8 << "bits";
    int sizeof_int_in_bits = sizeof(int)*8;
    if (sizeof_int_in_bits < 32) {
        cout << "App start error: size of integer value type is: " << sizeof_int_in_bits << " bits." << endl;
        cout << "Size of integer value type must be >= 32 for this application." << endl;
        return(3232);
    }



    initGSL();

    QApplication app(argc, argv);

    ApplicationModel appModel;
    ApplicationView appView;
    ApplicationController appController(&appModel, &appView);

    appController.tryToLoadAppRecall();

    if (!appController.tryToLoadGraphicAppSettings()) {
        cout << __FUNCTION__ << "load error in appController.tryToLoadgraphicAppSettings()" << endl;
        return(2);
    }

    /*bool bAppSettingsLoadedAndValidFromComputer = */appController.tryToLoadAppSettings();

    bool bStartSuccess = appController.start();

    if (!bStartSuccess) {
        cout << __FUNCTION__ << ": error: appController.start() failed" << endl;
        return(1);
    }

    return app.exec();
}
