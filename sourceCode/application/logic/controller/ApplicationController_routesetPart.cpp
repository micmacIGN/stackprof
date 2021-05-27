#include <QObject>
#include <QDebug>
#include <QMessageBox>

#include "ApplicationController.h"

#include "model/ApplicationModel.h"
#include "ui/ApplicationView.h"

#include "model/mainAppState.hpp"

#include "model/MenuActionStates.hpp"

#include "ui/dialogIOFiles/dialog_savefile.h"

#include "dialogIOFiles/dialog_inputfiles_images.h"

#include "dialog_geoRefInfos.h"

#include <QFileDialog>

#include "dialogIOFiles/dialog_replaceImageFile.h"

#include "ui/windowImageView/WindowImageView.h"

#include "ui/geoCRSSelect/dialog_geoCRSSelect_tableview.h"

#include "toolbox/toolbox_pathAndFile.h"

#include <QGuiApplication>

void ApplicationController::slot_routeSet_addNewRoute(QString strRouteName) {
    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);

    if (_ptrAppModel) {
        qDebug() << __FUNCTION__ << "call now _ptrAppModel->addNewRoute(strRouteName)";

        _ptrAppModel->addNewRoute(strRouteName);
    }
}

void ApplicationController::slot_routeSet_removeRoute(int idRoute) {
    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);

    if (_ptrAppModel) {
        qDebug() << __FUNCTION__ << "call now _ptrAppModel->removeRoute(idRoute)";

        _ptrAppModel->removeRoute(idRoute);
    }
}

void ApplicationController::slot_routeSet_routeEditionRenameRoute(int routeId, QString strRouteName) {
    if (_ptrAppModel) {
        qDebug() << __FUNCTION__ << "call now _ptrAppModel->renameRoute(routeId, strRouteName, false)";
        _ptrAppModel->renameRoute(routeId, strRouteName, false);
    }
}

void ApplicationController::slot_routeSet_routeEditionStateChanged(int ieRouteEditionState, int routeId) {

    e_AppModelState eAppModelState = eAModS_idle;

    _mainAppState.setState(MainAppState::eMAS_editingRouteSet_editingRoute);
    eAppModelState =                   eAModS_editingRouteSet_editingRoute;

    if (_ptrAppModel) {
        qDebug() << __FUNCTION__ << "call now _ptrAppModel->setState_editingRoute(selectedRouteTableViewRow)";
        _ptrAppModel->setState_editingRoute(eAppModelState, ieRouteEditionState, routeId);
    }
}

void ApplicationController::slot_routeSet_route_showAlone(int routeId, bool bShowAlone) {
    if (!_ptrAppModel) {
        return;
    }
    _ptrAppModel->routeSet_route_showAlone(routeId, bShowAlone);

}

bool ApplicationController::routeSet_new() {

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected request #0 : _ptrAppModel is nullptr";
        return(false);
    }
    if (!_ptrAppView) {
        qDebug() << __FUNCTION__ << "rejected request #1 : _ptrAppView is nullptr";
        return(false);
    }

    qDebug() << __FUNCTION__ << "_mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();

    if (   (_mainAppState.get_eAppState() != MainAppState::eMAS_idle)
         &&(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet)
         &&(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet_editingRoute)
        )
    {
       qDebug() << __FUNCTION__ << "rejected request #100";
       return(false);
    }

    bool bOkForNewSet = routesetEditFct_questionAboutSaveSaveAs_and_saveRouteSet();
    if (!bOkForNewSet) {
        return(false);
    }

    _ptrAppView->hideWindowsAbout_routeEditionAndProject();
    //clear many things in Appmodel for a new function start
    _ptrAppModel->setState_editingRoute_closeState_clearRouteset();
    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);



    //new trace with or without georeferencing ?

    QMessageBox messageBox(QMessageBox::Question,
                 "New traceset",
                 "Create traceset using georeferencing ?",
                 QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel, nullptr);
    int ieDialExecReturnCode = messageBox.exec();
    if (ieDialExecReturnCode == QMessageBox::Cancel) {
        routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }
    bool bUsGeoRefForTrace = (ieDialExecReturnCode == QMessageBox::Yes);


    _bUseGeoRefForTrace = bUsGeoRefForTrace;
    //Dialog_inputFiles_images dial_inputFilesImages("Open image as background", e_uSFM_oneFile, nullptr);
    ////Dialog_inputFiles_images dial_inputFilesImages("Open image as background", e_uSFM_twoFiles, nullptr);
    ////dial_inputFilesImages.setFilesTitle({"PX1", "PX2"});

    QString strBackgroundImageFilename;

    if (!_bUseGeoRefForTrace) {

        //while (1) {

            QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
            qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;

            strBackgroundImageFilename = QFileDialog::getOpenFileName(nullptr,
                                                               tr("Select image as background"),
                                                               strStartingDir,
                                                               tr("*.tif *.tiff *.TIF *.TIFF"));

            if (strBackgroundImageFilename.isEmpty()) {
                qDebug() << __FUNCTION__ << " canceled by user";
                routesetEditFct_clearState_backTo_DoNothing();
                return(false);
            }
            _appRecall.set_lastUsedDirectoryFromPathFile(strBackgroundImageFilename);

            qDebug() << __FUNCTION__ << __LINE__ << "strBackgroundImageFilename = " << strBackgroundImageFilename;

            strBackgroundImageFilename = getPathAndFilenameFromPotentialSymlink(strBackgroundImageFilename);

            qDebug() << __FUNCTION__ << __LINE__ << "strBackgroundImageFilename = " << strBackgroundImageFilename;

            /*S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus =*/
            _ptrAppModel->routesetEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_withProvidedBackgroundImageFilename(strBackgroundImageFilename);

        //}

        _ptrAppModel->set_useGeoRefForTrace(false); //by default model is useGeoRefForTrace at true

    } else {

        while (1) {

            QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
            qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;

            strBackgroundImageFilename = QFileDialog::getOpenFileName(nullptr,
                                                               tr("Select image as background"),
                                                               strStartingDir,
                                                               tr("*.tif *.tiff *.TIF *.TIFF"));

            if (strBackgroundImageFilename.isEmpty()) {
                qDebug() << __FUNCTION__ << " canceled by user";
                routesetEditFct_clearState_backTo_DoNothing();
                return(false);
            }
            _appRecall.set_lastUsedDirectoryFromPathFile(strBackgroundImageFilename);

            qDebug() << __FUNCTION__ << __LINE__ << "strBackgroundImageFilename = " << strBackgroundImageFilename;

            strBackgroundImageFilename = getPathAndFilenameFromPotentialSymlink(strBackgroundImageFilename);

            qDebug() << __FUNCTION__ << __LINE__ << "strBackgroundImageFilename = " << strBackgroundImageFilename;

            S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus = _ptrAppModel->
                routesetEditionFunction_set_GeoRefModelImagesSet_fromBackgroundImageFilename(strBackgroundImageFilename);

            qDebug() << __FUNCTION__ << "se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases =" << se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases;

            if ( se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases == e_iSS_geoTFWData_rTBC_available) {
                break;
            }

            //here when se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases != e_iSS_geoTFWData_rTBC_available

            S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sbASMsgStatusA_gRISS_aboutRouteSetEdition;

            sbASMsgStatusA_gRISS_aboutRouteSetEdition = _ptrAppModel->routesetEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus();

            QMessageBox messageBox(QMessageBox::Information,
                        sbASMsgStatusA_gRISS_aboutRouteSetEdition._strMsgAboutWorldFileData,
                        sbASMsgStatusA_gRISS_aboutRouteSetEdition._strMsgAboutWorldFileData_errorDetails,
                        QMessageBox::Ok, nullptr);

            switch (se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases) {

                case e_iSS_geoTFWData_rTBC_available://should never happen here; handled in a dedicated code section below
                case e_iSS_geoTFWData_rTBC_notSetValue://should never happen
                    qDebug() << __FUNCTION__ << __LINE__ << ": dev error: case which should never happen";
                    routesetEditFct_clearState_backTo_DoNothing();
                    return(false);
                    //break;

                case e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData://this case is for projectEdition, it should never happen for routeset
                case e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason:
                case e_iSS_geoTFWData_rTBC_notAvailable_noneFound:
                    messageBox.exec();
                    break;
            }
        }

        //feed the dialogGeoRefInfos before exec on it:
        _ptrAppModel->sendStr_worldFileDataToView();
        _ptrAppModel->sendStr_imagesFilename();
        _ptrAppModel->sendEPSGCodeToView();
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
        _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

        int ieDialExecReturnCode = _ptrAppView->exec_dialogGeoRefInfos();
        if (ieDialExecReturnCode == QDialog::Rejected) {
            qDebug() << __FUNCTION__ << " canceled by user";
            routesetEditFct_clearState_backTo_DoNothing();
            return(false);
        }
    }

    //------
    bool bReport = _ptrAppModel->set_micmacStepAndSpatialResolution_forModels(
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {true, false, false}/*e_vVDFS_FirstAlone_routeSetEditing*/,
        true,
        false);

    if (!bReport) {
        QMessageBox messageBox(QMessageBox::Information,
                    "Internal error",
                    "failed to setDequantizationSteps_forModels",
                    QMessageBox::Ok, nullptr);
        messageBox.exec();
        routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }

    if (_bUseGeoRefForTrace) {
        bReport = _ptrAppModel->init_geoProj_onlyOneImageDisplayed(eLA_PX1); //@#LP eLA_PX1 refactor argument not nice as it's eLA_PX1 because e_vVDFS_FirstAlone_routeSetEditing
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "Internal error",
                        "failed in init_geoProj",
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            _ptrAppModel->setState_editingRoute_closeState_clearRouteset();
            routesetEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);

    } else {
        bool bReport = _ptrAppModel->init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(eLA_PX1); //@#LP eLA_PX1 refactor argument not nice as it's eLA_PX1 because e_vVDFS_FirstAlone_routeSetEditing
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoUiInfos error", //@#LP add dev error number
                        "failed in init_geoUiInfos_noGeoRefUsed...()", //@#LP more details ?
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            _ptrAppModel->setState_editingRoute_closeState_clearRouteset();
            routesetEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY);
    }

    _mainAppState.permitsMenuActionAboutGeoinfoData(_bUseGeoRefForTrace);
    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);

    /*_ptrAppView->open_dialog_taskInProgress(Dialog_taskInProgress::e_PageDTIP_messagePleaseWait_noAbortButton,
                                            "processing...", "Building image pyramid", 0, 100);
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));*/

    bool bBackgroundImageLoaded = _ptrAppModel->
            loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(strBackgroundImageFilename, 0/*eLA_PX1*/); // //@LP idxLayer at '0' as 'only one image'

    /*QGuiApplication::restoreOverrideCursor();
    _ptrAppView->close_dialog_taskInProgress();*/

    if (!bBackgroundImageLoaded) {
        S_boolAndStrErrorMessage_aboutLoad sbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sbaStrErrorMessage_aboutLoad();
        QMessageBox messageBox(QMessageBox::Information,
                    "Image load error",
                    //"failed to load image " + strBackgroundImageFilename, //@#LP could add more explaination about the reason
                    sbaStrErrorMessage_aboutLoad._strMessagAboutImage,
                    QMessageBox::Ok, nullptr);
        messageBox.exec();

        routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }

    //just to init the size to any target point alone, relative size to image size,
    //if not called, the defautl size is 1.415 (very small)
    _ptrAppModel->routesetEditionFunction_initState_andFeedGraphicsItemContainer();

    _ptrAppView->ptrWindowImageView()->setVisible_layersSwitcher(false);
    _ptrAppView->showImageViewWindows(true, true);
    _ptrAppView->showTraceEditWindow();

    return(true);
}




//open existing routeset
bool ApplicationController::routeSet_open() {

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected request #0 : _ptrAppModel is nullptr";
        return(false);
    }
    if (!_ptrAppView) {
        qDebug() << __FUNCTION__ << "rejected request #1 : _ptrAppView is nullptr";
        return(false);
    }

    qDebug() << __FUNCTION__ << "_mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();

     if (  (_mainAppState.get_eAppState() != MainAppState::eMAS_idle)
         &&(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet)
         &&(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet_editingRoute) /* merged cases with editingRoute ni a project (valid merge?)*/
        )
     {
        qDebug() << __FUNCTION__ << "rejected request #100";
        return(false);
     }

     bool bOkForOpenExistingSet = routesetEditFct_questionAboutSaveSaveAs_and_saveRouteSet();
     if (!bOkForOpenExistingSet) {
         return(false);
     }

    //@#LP send the path from logic to preset on last image or StandardPathLocation::strStartingDefaultDir()
    QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strfileName = QFileDialog::getOpenFileName(nullptr, tr("Open an existing routeset"), strStartingDir, "*.jtraces");
    if (strfileName.isEmpty()) {
        //routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }
    _appRecall.set_lastUsedDirectoryFromPathFile(strfileName);

    //clear many things in Appmodel for a new function start
    routesetEditFct_clearState_backTo_DoNothing();

    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);

    bool bRoutesetFileLoaded = _ptrAppModel->routesetEditionFunction_loadRouteset(strfileName);
    if (!bRoutesetFileLoaded) {
        S_boolAndStrErrorMessage_aboutLoad sbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sbaStrErrorMessage_aboutLoad();
        QMessageBox messageBox(QMessageBox::Information,
                               "error loading traceset (.jtraces file)",
                               sbaStrErrorMessage_aboutLoad._strMessageAboutJson,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }

    bool bTryWithInputImageIndicatedInJsonTracesetFile = true;
    bool bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch = false;
    bool bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError = false;

    bool bExists = false;
    QString strBackgroundImageFilename;
    bool bSucces = _ptrAppModel->routesetOpenFunction_getBackgroundImageFilenameGotFromJsonLoad(strBackgroundImageFilename, bExists);
    if (!bSucces) {
        QMessageBox messageBox(QMessageBox::Information,
                               "error loading traceset (.jtraces file)",
                               "internal dev error #891",
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        routesetEditFct_clearState_backTo_DoNothing();
        return(false);
    }

    if (!bExists) {

        bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError = true;

    }

    QString strBackgroundImageFilename_refFromTrace = strBackgroundImageFilename;

    _bUseGeoRefForTrace = _ptrAppModel->get_useGeoRefForTrace();

    _mainAppState.permitsMenuActionAboutGeoinfoData(_bUseGeoRefForTrace);
    _mainAppState.setState(MainAppState::eMAS_editingRouteSet);


    while (1) {

        if (!bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError) { //the file exists

            if (_bUseGeoRefForTrace) {

                //check that geo data from disk about the background image match with the geo data loaded from the json:

                S_e_geoRefImagesSetStatus seGeoRefImagesSetStatus_aboutComparison;
                S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison;
                seGeoRefImagesSetStatus_aboutComparison = _ptrAppModel->routesetOpenFunction_checkThatGeoDataOfInputImageFileMatchesWithGeoDataLoadedFromJson(
                            strBackgroundImageFilename, sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison);

                //check that world file matches

                if (seGeoRefImagesSetStatus_aboutComparison._eiSSgTFW_reducedToBigCases != e_iSS_geoTFWData_rTBC_available) { //don't match

                    QMessageBox messageBox(QMessageBox::Information,
                                sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutWorldFileData,
                                sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutWorldFileData_errorDetails,
                                QMessageBox::Ok, nullptr);

                    messageBox.exec();

                    bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch = true;


                } else {

                    //check that epsg code (.jepsg vs from .jtraces) matches

                    //don't match
                    if (  (seGeoRefImagesSetStatus_aboutComparison._eiSS_geoEPSG != e_iSS_geoEPSG_available)
                        &&(seGeoRefImagesSetStatus_aboutComparison._eiSS_geoEPSG != e_iSS_geoEPSG_notAvailable_noneFound)) {

                        QMessageBox messageBox(QMessageBox::Information,
                                    sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutEPSG,
                                    sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutEPSG_errorDetails,
                                    QMessageBox::Ok, nullptr);

                         messageBox.exec();

                         bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch = true;


                    } else {

                        //here if geo data matches; and epsg code matches or not found from disk (not an error case)

                        if (seGeoRefImagesSetStatus_aboutComparison._eiSS_geoEPSG == e_iSS_geoEPSG_available) {
                            //EPSG Code are equal or
                            //ok to use the new input image as replacement
                            qDebug() << __FUNCTION__ << "EPSG Code are equal";

                        } else { //e_iSS_geoEPSG_notAvailable_noneFound

                            //the moved image could be renamed and hence do not appears in the cache about the epsg code

                            QString strQuestion = "Apply the EPSG Code from .jtraces to the selected image ?";
                            strQuestion += "(" + sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation + ")";

                            QMessageBox messageBox(QMessageBox::Question,
                                        "EPSG Code for selected image not found (.jepsg)",
                                        strQuestion,
                                        QMessageBox::No|QMessageBox::Yes, nullptr);

                            int ieDialExecReturnCode = messageBox.exec();
                            if (ieDialExecReturnCode == QMessageBox::No) {

                                //do nothing

                            } else { //Yes

                                bool bReport = _ptrAppModel->routesetEditionFunction_storeInCacheTheCurrentEPSGCode_forImage(strBackgroundImageFilename);

                                QString strMessage_base =
                                        "Association of the code "
                                        + sboolAndStrMsgStatusAbout_geoRefImageSetStatus_aboutComparison._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation
                                        + " for the image ";
                                QString strMessage;
                                if (bReport) {
                                    strMessage = strMessage_base + "done";
                                } else {
                                    strMessage = strMessage_base + "failed";
                                }
                                QMessageBox messageBox(QMessageBox::Information,
                                            "EPSG Code storage",
                                            strMessage,
                                            QMessageBox::Ok, nullptr);
                                messageBox.exec();
                            }
                        }
                    }
                }
            }
        } else { //the file does not exist

            //do nothing here
        }

        //the file exists and all its geo data match
        if (  (!bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch)
            &&(!bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError)) {

            _ptrAppModel->routesetEditionFunction_replaceBackgroundImage(strBackgroundImageFilename);

            //try to load the image

            /*_ptrAppView->open_dialog_taskInProgress(Dialog_taskInProgress::e_PageDTIP_messagePleaseWait_noAbortButton,
                                                    "processing...", "Building image pyramid", 0, 100);
            QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));*/

            bool bBackgroundImageLoaded = _ptrAppModel->
                    routesetEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav();

            /*QGuiApplication::restoreOverrideCursor();
            _ptrAppView->close_dialog_taskInProgress();*/


            if (!bBackgroundImageLoaded) {
                bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError = true;

                S_boolAndStrErrorMessage_aboutLoad SbaStrErrorMessage_aboutLoad;
                SbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sboolAndStrMsgAbout_aboutLoad();
                if (!SbaStrErrorMessage_aboutLoad._bFileDoesnotExistAtLocation) {
                    QMessageBox messageBox(QMessageBox::Information,
                                           "error loading image",
                                           "internal dev error #892",
                                           QMessageBox::Ok, nullptr);
                    messageBox.exec();
                    routesetEditFct_clearState_backTo_DoNothing();
                    return(false);
                } else {
                    //strReplaceMsgReasonDetails = SbaStrErrorMessage_aboutLoad._strMessagAboutImage_details_filename;
                }
            }
        }

        //the file does not exists
        //or its geo does not match
        if (  (bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch)
            ||(bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError)) {

            QString strTitle = "<anonym title>";
            QString strTextAboutResolve = "<anonym resolve text>";
            QString strBackgroundImageFilename_ToDisplay= "<anonym>";

            if (bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError) {

                strTitle = "Background image used in routeset not found";
                strTextAboutResolve = "If you moved the image, click replace to fix the issue";

                if (bTryWithInputImageIndicatedInJsonTracesetFile) {
                    bTryWithInputImageIndicatedInJsonTracesetFile = false;
                } else {
                    strTitle = "fail loading selected image";
                    strTextAboutResolve = "Click replace to fix the issue";
                }
                strBackgroundImageFilename_ToDisplay = strBackgroundImageFilename;

            } else {

                strTitle = "Geo data from image don't match with traceset geo data";
                strTextAboutResolve = "Click replace to select an image file to use instead";

                if (bTryWithInputImageIndicatedInJsonTracesetFile) {
                    bTryWithInputImageIndicatedInJsonTracesetFile = false;
                } else {
                    strTitle = "Geo data from image don't match with traceset geo data";
                    strTextAboutResolve = "Click replace to select an image file to use instead";
                }
                strBackgroundImageFilename_ToDisplay = strBackgroundImageFilename_refFromTrace;

            }

            Dialog_replaceImageFile dial_replaceImageFile(strTitle,
                                                      strBackgroundImageFilename_ToDisplay,
                                                      strTextAboutResolve,
                                                      _appRecall.get_lastUsedDirectory(),
                                                      nullptr);

            int ieDialExecReturnCode = dial_replaceImageFile.exec();
            if (ieDialExecReturnCode == QDialog::Rejected) {
                qDebug() << __FUNCTION__ << " canceled by user";
                routesetEditFct_clearState_backTo_DoNothing();
                return(false);
            }

            strBackgroundImageFilename = dial_replaceImageFile.getStrSelectedfileName();
            bExists = true;

            _appRecall.set_lastUsedDirectoryFromPathFile(strBackgroundImageFilename);

        } else {

            break;

        }

        bProposeToReplaceInputImageFile_dueTo_geoDataDoestNotMatch = false;
        bProposeToReplaceInputImageFile_dueTo_imageFileExistenceError = false;
    }

    if (_bUseGeoRefForTrace) {
        S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus = _ptrAppModel->
            routesetEditionFunction_set_GeoRefModelImagesSet_fromBackgroundImageFilename(strBackgroundImageFilename);

        qDebug() << __FUNCTION__ << "se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases =" << se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases;

        if (   (se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases != e_iSS_geoTFWData_rTBC_available)
             ||(  (se_geoRefImagesSetStatus._eiSS_geoEPSG != e_iSS_geoEPSG_available)
                &&(se_geoRefImagesSetStatus._eiSS_geoEPSG != e_iSS_geoEPSG_notAvailable_noneFound))) {

               QMessageBox messageBox(QMessageBox::Information,
                                      "error checking geo data about traceset",
                                      "internal dev error #893",
                                      QMessageBox::Ok, nullptr);
               messageBox.exec();
               routesetEditFct_clearState_backTo_DoNothing();
               return(false);
        }
    }

    /*if (bImageBackgroundWasReplaced) {
        _ptrAppModel->setAppFile_routeset_asModified();
    }*/



    //feed the dialogGeoRefInfos for any epsg code adjustement (if the value was an error in previous edition):
    _ptrAppModel->sendStr_worldFileDataToView();
    _ptrAppModel->sendStr_imagesFilename();
    _ptrAppModel->sendEPSGCodeToView();
    _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
    _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

    //---
    _ptrAppModel->setPtrVectDequantization_forGeoUiInfosModel(true);

    _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu(_ptrAppModel->getAppFile_routeset().getFilename());

    if (_bUseGeoRefForTrace) {
        bool bReport = _ptrAppModel->init_geoProj_onlyOneImageDisplayed(eLA_PX1); //@#LP eLA_PX1 refactor argument not nice as it's eLA_PX1 because e_vVDFS_FirstAlone_routeSetEditing
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoProj error",
                        "failed in init_geoProj",
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            routesetEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);
        //---
    } else {
        bool bReport = _ptrAppModel->init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(eLA_PX1); //@#LP eLA_PX1 refactor argument not nice as it's eLA_PX1 because e_vVDFS_FirstAlone_routeSetEditing
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoUiInfos error", //@#LP add dev error number
                        "failed in init_geoUiInfos_noGeoRefUsed...()", //@#LP more details ?
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            routesetEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY);
    }

    _ptrAppModel->routesetEditionFunction_initState_andFeedGraphicsItemContainer();

    _ptrAppView->ptrWindowImageView()->setVisible_layersSwitcher(false);
    _ptrAppView->showImageViewWindows(true, true);
    _ptrAppView->showTraceEditWindow();

    return(true);
}

bool ApplicationController::routeSet_save() {
    qDebug() << __FUNCTION__;

    AppFile appFile_routeset = _ptrAppModel->getAppFile_routeset();

    bool bHasChanges = false;
    bool bCanBeSaved = routesetEditFct_writeRouteset_handleNoChangeOrChangeNotCompatibleWithWriteOperation(true, bHasChanges);
    if (!bCanBeSaved) {
        return(false);
    }
    if (!appFile_routeset.filenameSet/*inUse*/()) {
        return(routeSet_saveAs());
    }
    if (bHasChanges) {
        return(routesetEditFct_writeRouteset_withMessageBoxIfFail());
    }
    return(true);
}

bool ApplicationController::routeSet_saveAs() {

    bool bHasChanges = false;
    bool bCanBeSaved = routesetEditFct_writeRouteset_handleNoChangeOrChangeNotCompatibleWithWriteOperation(false, bHasChanges);
    if (!bCanBeSaved) {
        return(false);
    }
    //change or not, permits to "save as"

    QString strDotExtension = ".jtraces";
    QString strFilter = "*" + strDotExtension;

    //@#LP send the path from logic to preset on last image or StandardPathLocation::strStartingDefaultDir()
    QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strSelectedfileName = QFileDialog::getSaveFileName(nullptr, tr("Save As"), strStartingDir, strFilter);
    if (strSelectedfileName.isEmpty()) {
        return(false);
    }
    if (!strSelectedfileName.endsWith(strDotExtension)) {
        strSelectedfileName+= strDotExtension;
    }

    bool bWriteReport = routesetEditFct_writeRouteset_withMessageBoxIfFail(strSelectedfileName);
    _appRecall.set_lastUsedDirectoryFromPathFile(strSelectedfileName);
    return(bWriteReport);
}

bool ApplicationController::routeSet_close() {

    qDebug() << __FUNCTION__;
    bool bOkForCloseSet = routesetEditFct_questionAboutSaveSaveAs_and_saveRouteSet();
    if (!bOkForCloseSet) {
        return(false);
    }

    routesetEditFct_clearState_backTo_DoNothing();

    return(true);
}

void ApplicationController::routesetEditFct_clearState_backTo_DoNothing() {

    _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu();
    _ptrAppView->hideWindowsAbout_routeEditionAndProject();
    _ptrAppView->ptrDialogGeoRefInfos()->clearUiContent_all_backToDefaultState();
    _ptrAppView->ptrDialog_geoCRSSelect_tableView()->resetToDefault();

    //clear many things in Appmodel for a new function
    _ptrAppModel->setState_editingRoute_closeState_clearRouteset();
    clearVarStates();
    _mainAppState.setState(MainAppState::eMAS_idle);
}

bool ApplicationController::routesetEditFct_questionAboutSaveSaveAs_and_saveRouteSet() {

    qDebug() << __FUNCTION__;

    AppFile appFile_routeset = _ptrAppModel->getAppFile_routeset();

    bool bWriteTheFile = false;
    bool bSaveAs = false;
    QString strRouteSetFilenameForSaveAs;

    bool bOkForNewFunction = false;

    bOkForNewFunction = handleDialogQuestionsAboutSaving_forAppFile(
                appFile_routeset,
                "Save Traceset?",
                "Some modifications are not saved yet", "Do you want to save it?",
                ".jtraces",
                bWriteTheFile,
                bSaveAs,
                strRouteSetFilenameForSaveAs);

    qDebug() << __FUNCTION__ << "bWriteTheFile = " << bWriteTheFile;

    if (bWriteTheFile) {
        bool bWriteReport = false;
        if (bSaveAs) {
            bWriteReport = routesetEditFct_writeRouteset_withMessageBoxIfFail(strRouteSetFilenameForSaveAs);
        } else {
            bWriteReport = routesetEditFct_writeRouteset_withMessageBoxIfFail();
        }
        bOkForNewFunction = bWriteReport;
    }

    qDebug() << __FUNCTION__ << "final bOkForNewFunction = " << bOkForNewFunction;

    return(bOkForNewFunction);
}


bool ApplicationController::routesetEditFct_writeRouteset_withMessageBoxIfFail(const QString& strRouteSetFilenameToSaveInto) {

    bool bWriteReport = false;

    if (strRouteSetFilenameToSaveInto.isEmpty()) {
        bWriteReport = _ptrAppModel->routesetEditionFunction_writeRouteset();
    } else {
        bWriteReport = _ptrAppModel->routesetEditionFunction_writeRouteset(strRouteSetFilenameToSaveInto);
    }

    QString strMsg;
    AppFile appFile_routeset = _ptrAppModel->getAppFile_routeset();
    if (!bWriteReport) {
        strMsg = "Can not save file: " + _ptrAppModel->routesetEditionFunction_strDetailsAboutFail_routesetWrite();// appFile_routeset.getFilename(); //+ _ptrAppModel->get_routesetFilename();
    } else {
        strMsg = "Saved !"; // + appFile_routeset.getFilename() + " )"; //_ptrAppModel->get_routesetFilename();
    }
    QMessageBox messageBox(QMessageBox::Information,
                "Traceset file",
                strMsg,
                QMessageBox::Ok, nullptr);
    messageBox.setText(strMsg);
    messageBox.exec();

    if (bWriteReport) {
        _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu(_ptrAppModel->getAppFile_routeset().getFilename());
    }
    return(bWriteReport);
}



//return true if the file can be saved
//return false if the routeset content is not compatible with save operation
//bHasChanges is set by the method
bool ApplicationController::routesetEditFct_writeRouteset_handleNoChangeOrChangeNotCompatibleWithWriteOperation(bool bWarnAboutNoChange, bool& bHasChanges) {

    AppFile appFile_routeset = _ptrAppModel->getAppFile_routeset();
    bHasChanges = appFile_routeset.modified();

    QString strDetailsAboutNotCompatibleWithSaving;
    if (!_ptrAppModel->routesetEditionFunction_routesetContentIsCompatibleWithWriteOperation(strDetailsAboutNotCompatibleWithSaving)) {
        QMessageBox messageBox(
            QMessageBox::Information,
            "Traceset edition current state doesn't not permit save operations",
            strDetailsAboutNotCompatibleWithSaving,
            QMessageBox::Ok, nullptr);
        messageBox.exec();
        return(false);
    }

    return(true);
}


