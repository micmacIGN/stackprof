#include <QApplication>

#include "../../application/logic/mathComputation/initGSL.h"

#include "../../application/logic/model/ApplicationModel.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);


int test_exportResult(const QString& qstrInputProjectFile, const QString& qstrExtraStringForOutputResultFilename);

int main(int argc, char *argv[]) {

    //qInstallMessageHandler(myMessageOutput); // Install the handler

    initGSL();

    QApplication app(argc, argv);

    QString strPathOfInputProjectFile = "/home/laurent/github_lp-rep_topush/stackprof-bin/inputDataForTests/jstackprof/test_exportResult/input_project/";
    QString strInputProjectFile_georef = strPathOfInputProjectFile + "georef_test_12z_8boxes_forexport.jstackprof";
    QString strInputProjectFile_noGeoref = strPathOfInputProjectFile + "noGeoref_test_12z_8boxes_forexport.jstackprof";

    test_exportResult(strInputProjectFile_georef, "georef");
    test_exportResult(strInputProjectFile_noGeoref, "noGeoref");

    return(0);
}

int test_exportResult(const QString& qstrInputProjectFile, const QString& qstrExtraStringForOutputResultFilename) {

    ApplicationModel appModel;

    bool bLoadReportAbout_EPSGDataListFromProjDB = false;
    bool bInitReport = appModel.init(512, bLoadReportAbout_EPSGDataListFromProjDB);

    qDebug() << __FUNCTION__ << " appModel.init(...): bInitReport = " << bInitReport;
    qDebug() << __FUNCTION__ << " appModel.init(...): bLoadReportAbout_EPSGDataListFromProjDB = " << bLoadReportAbout_EPSGDataListFromProjDB;

    if (!bInitReport) {
        qDebug() << __FUNCTION__ << "appModel.init(...) failed (bInitReport)";
        return(1);
    }
    if (!bLoadReportAbout_EPSGDataListFromProjDB) {
        qDebug() << __FUNCTION__ << "appModel.init(...) failed (bLoadReportAbout_EPSGDataListFromProjDB)";
        return(1);
    }

    bool bModelInitAppDiskPathCacheStorage_success = appModel.init_appDiskPathCacheStorage_andLinkedInit("/tmp");
    if (!bModelInitAppDiskPathCacheStorage_success) {
        qDebug() << __FUNCTION__ << "appModel.init_appDiskPathCacheStorage_andLinkedInit(...) failed (bModelInitAppDiskPathCacheStorage_success)";
        return(1);
    }


    S_InputFilesWithImageFilesAttributes sInputFilesWithAttributes_fromJsonFile;
    e_mainComputationMode eMainComputationMode = e_mCM_notSet;
    QVector<bool> qvectb_layersToCompute_justCheckingStringEmptiness;
    QVector<bool> qvectb_correlationMapUsage_justCheckingStringEmptiness;

    bool bLoadProjectReport = appModel.projectEditionFunction_loadProject(qstrInputProjectFile,
                                                sInputFilesWithAttributes_fromJsonFile,
                                                eMainComputationMode,
                                                qvectb_layersToCompute_justCheckingStringEmptiness,
                                                qvectb_correlationMapUsage_justCheckingStringEmptiness);
    if (!bLoadProjectReport) {
        qDebug() << __FUNCTION__ << " appModel.projectEditionFunction_loadProject(...) failed (bLoadProjectReport)";
        return(1);
    }


    bool bUseGeoRefForProject = appModel.get_useGeoRefForProject();
    //appModel.set_useGeoRefForTrace(bUseGeoRefForProject);

    if (bUseGeoRefForProject) {
        bool bReport = appModel.init_geoProj_onlyOneImageDisplayed(eLA_PX1);
        if (!bReport) {
            qDebug() << __FUNCTION__ << " init_geoProj_onlyOneImageDisplayed(...) failed";
            return(1);
        }
    } else {
       bool bReport = appModel.init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(eLA_PX1);
       if (!bReport) {
           qDebug() << __FUNCTION__ << " init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(...) failed";
           return(1);
       }
    }

    appModel.projectEditionFunction_initState_andFeedGraphicsItemContainer(false);

    QVector<bool> qvectb_correlScoreMapToUseForComp;
    qvectb_correlScoreMapToUseForComp.fill(false,3);

    bool bSetInputFilesAndCorrelationScoreFiles = appModel.setInputFilesAndCorrelationScoreFiles(
                sInputFilesWithAttributes_fromJsonFile._inputFiles,
                qvectb_correlScoreMapToUseForComp);

    if (!bSetInputFilesAndCorrelationScoreFiles) {
        qDebug() << __FUNCTION__ << " setInputFilesAndCorrelationScoreFiles(...) failed (bSetInputFilesAndCorrelationScoreFiles)";
        return(false);
    }

    QVector<float> qvect_micmacStep_PX1_PX2_deltaZ;
    QVector<float> qvect_spatialResolution_PX1_PX2_deltaZ;
    QVector<bool> qvectb_layersToCompute;

    bool bGet_micmacStepAndSpatialResolutiong = appModel.get_micmacStepAndSpatialResolution(
                qvect_micmacStep_PX1_PX2_deltaZ,
                qvect_spatialResolution_PX1_PX2_deltaZ,
                qvectb_layersToCompute);

    qDebug() << __FUNCTION__ << " qvect_micmacStep_PX1_PX2_deltaZ:" << qvect_micmacStep_PX1_PX2_deltaZ;
    qDebug() << __FUNCTION__ << " qvect_spatialResolution_PX1_PX2_deltaZ:" << qvect_spatialResolution_PX1_PX2_deltaZ;
    qDebug() << __FUNCTION__ << " qvectb_layersToCompute:" << qvectb_layersToCompute;

    qDebug() << __FUNCTION__ << " AppModel.get_micmacStepAndSpatialResolution() bGet_micmacStepAndSpatialResolutiong = " << bGet_micmacStepAndSpatialResolutiong;

    if (!bGet_micmacStepAndSpatialResolutiong) {
        qDebug() << __FUNCTION__ << " AppModel.get_micmacStepAndSpatialResolution() failed (bGet_micmacStepAndSpatialResolutiong)";
        return(1);
    }

    bool bSomeWasNotSet = appModel.setDefaultComputationModeAndParameters_ifNotSet();
    qDebug() << __FUNCTION__ << " AppModel.setDefaultComputationModeAndParameters_ifNotSet() said bSomeWasNotSet = " << bSomeWasNotSet;

    bool bRunComptationReport = appModel.runComputation();
    if (!bRunComptationReport) {
        qDebug() << __FUNCTION__ << " AppModel.runComputation() failed (bRunComptationReport)";
        return(1);
    }

    bool bCompute_leftRightSidesLinearRegressionsModels_forAllBoxes =
            appModel.compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
    if (!bCompute_leftRightSidesLinearRegressionsModels_forAllBoxes) {
        qDebug() << __FUNCTION__ << " AppModel.compute_leftRightSidesLinearRegressionsModels_forAllBoxes() failed (bCompute_leftRightSidesLinearRegressionsModels_forAllBoxes)";
        return(1);
    }

    S_ExportSettings sExportSettings;

    for (sExportSettings._ieExportResult_fileType = e_ERfT_ascii;
         sExportSettings._ieExportResult_fileType <= e_ERfT_json;
         sExportSettings._ieExportResult_fileType++) {

        sExportSettings._ieExportResult_boxesContent = e_ERbC_all;//e_ERbC_onlyCurrent
        sExportSettings._idOfCurrentBox = -1;

        for (unsigned char icCode = 0; icCode <= 8; icCode++) {

            sExportSettings._bIncludeProfilesCurvesData = icCode&0x01;
            sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty = icCode&0x02;
            sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty = icCode&0x04;

            QString qstrFilenameExportResult_base = "/tmp/resultExportTest_";
            QString qstrFileType[2] { "ascii", "json"}; //@LP considers e_ERfT_ascii is 0 and e_ERfT_json is 1

            QString qstrIncludeProfilesCurvesData_base = "IncProfCurveData";
            QString qstr_ifWarnFlagByUser_setLinearRegressionData_asEmpty_base = "LinRegDataEmpty";
            QString qstr_bIfWarnFlagByUser_setProfilesCurvesData_asEmpty_base = "ProfCurveDataEmpty";

            QString strFilenameExportResult =
                    qstrFilenameExportResult_base
                    + "_" + qstrExtraStringForOutputResultFilename
                    + "_" + qstrFileType[sExportSettings._ieExportResult_fileType]
                    + "_" + qstrIncludeProfilesCurvesData_base + (sExportSettings._bIncludeProfilesCurvesData?"YES":"NO")
                    + "_" + qstr_ifWarnFlagByUser_setLinearRegressionData_asEmpty_base + (sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty?"YES":"NO")
                    + "_" + qstr_bIfWarnFlagByUser_setProfilesCurvesData_asEmpty_base  + (sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty?"YES":"NO")
                    + ".txt";

            appModel.project_exportResult(strFilenameExportResult, sExportSettings);
        }

    }

    return(0);
}



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

