#include <QDebug>

#include "../../application/logic/model/georef/georefModel_imagesSet.hpp"
#include "../../application/logic/model/georef/geoRefImagesSetStatus.hpp"

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

void test_sync_TFW_EPSG();
void test_sync_TFW_subFunction(const QVector<QString> qvectImageFilename);

int main(int argc, char *argv[]) {

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

    test_sync_TFW_EPSG();
    
    return(0);
}

void test_sync_TFW_EPSG() {

    QString strAbsolutePathOfInputImages = 
    "/home/laurent/inputDataForTests/testData_WordFile_EPSG_syncTest/worldfile";

    {
       QVector<QString> qVector_testImages {
       strAbsolutePathOfInputImages + "/allsync_and_usable/" + "s16_15x15_deltaZ.tif",
       strAbsolutePathOfInputImages + "/allsync_and_usable/" + "s16_15x15_px1.tiff",
       strAbsolutePathOfInputImages + "/allsync_and_usable/" + "s16_15x15_px2.tiff"};

       test_sync_TFW_subFunction(qVector_testImages);
    }


    {
       QVector<QString> qVector_testImages {
       strAbsolutePathOfInputImages + "/allsync_but_unusable_xyscalenotAbsEqu/" + "s16_15x15_deltaZ.tif",
       strAbsolutePathOfInputImages + "/allsync_but_unusable_xyscalenotAbsEqu/" + "s16_15x15_px1.tiff",
       strAbsolutePathOfInputImages + "/allsync_but_unusable_xyscalenotAbsEqu/" + "s16_15x15_px2.tiff"};

       test_sync_TFW_subFunction(qVector_testImages);
    }


    {
       QVector<QString> qVector_testImages {
       strAbsolutePathOfInputImages + "/unsync_one_unusable_rotationTermNotZero/" + "s16_15x15_deltaZ.tif",
       strAbsolutePathOfInputImages + "/unsync_one_unusable_rotationTermNotZero/" + "s16_15x15_px1.tiff",
       strAbsolutePathOfInputImages + "/unsync_one_unusable_rotationTermNotZero/" + "s16_15x15_px2.tiff"};

       test_sync_TFW_subFunction(qVector_testImages);
    }


    {
       QVector<QString> qVector_testImages {
       strAbsolutePathOfInputImages + "/allsync_but_unusable_rotationTermNotZero/" + "s16_15x15_deltaZ.tif",
       strAbsolutePathOfInputImages + "/allsync_but_unusable_rotationTermNotZero/" + "s16_15x15_px1.tiff",
       strAbsolutePathOfInputImages + "/allsync_but_unusable_rotationTermNotZero/" + "s16_15x15_px2.tiff"};

       test_sync_TFW_subFunction(qVector_testImages);
    }


    {
       QVector<QString> qVector_testImages {
       strAbsolutePathOfInputImages + "/unsync_one_unusable_xyscalenotAbsEqu/" + "s16_15x15_deltaZ.tif",
       strAbsolutePathOfInputImages + "/unsync_one_unusable_xyscalenotAbsEqu/" + "s16_15x15_px1.tiff",
       strAbsolutePathOfInputImages + "/unsync_one_unusable_xyscalenotAbsEqu/" + "s16_15x15_px2.tiff"};

       test_sync_TFW_subFunction(qVector_testImages);
    }

}



void test_sync_TFW_subFunction(const QVector<QString> qvectImageFilename) {

    GeoRefModel_imagesSet geoRefModelImagesSet;

    bool bSomeLoadedButNotUsable = false;
    bool bSomeLoaded = geoRefModelImagesSet.setAndLoad_imageWidthHeight_TFWDataIfAvailable(qvectImageFilename, bSomeLoadedButNotUsable);

    S_e_geoRefImagesSetStatus segeoRefImagesSetStatus = {};
    S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sbaStrMsg_gRIS_projectEdition;

    if (  (bSomeLoadedButNotUsable)
        ||(!bSomeLoaded)) {
        if (bSomeLoadedButNotUsable) {
            segeoRefImagesSetStatus._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_loadedButNotUsable;
        } else {
            if (!bSomeLoaded) {
                segeoRefImagesSetStatus._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_noneFound;
            }
        }
        projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImagesSetStatus,
                                                                      sbaStrMsg_gRIS_projectEdition);
    } else {
         segeoRefImagesSetStatus._eiSSgTFW_allStatusCase = geoRefModelImagesSet.check_TFWDataSet();
         projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImagesSetStatus,
                                                                  sbaStrMsg_gRIS_projectEdition);
    }

    qDebug() << __FUNCTION__ << "* -------------------------------------------------------------";
    qDebug() << __FUNCTION__ << "eiSSgTFW_allStatusCase = " <<  segeoRefImagesSetStatus._eiSSgTFW_allStatusCase;
    qDebug() << __FUNCTION__ << "msg = " <<
                sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData << " ; " << sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails;
    qDebug() << __FUNCTION__ << "------------------------------------------------------------- *";
}
