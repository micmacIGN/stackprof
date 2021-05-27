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

#include "OsPlatform/standardPathLocation.hpp"
#include <QFileDialog>

#include "dialogIOFiles/dialog_replaceImageFile.h"

#include "dialog_selectroutefromrouteset.h"

#include "dialog_project_inputfiles.h"

#include "../../logic/model/core/InputFilesCheckerForComputation.h"

#include "dialog_computationparameters.h"

#include "dialogVLayoutStackProfilEdit.h"

#include "dialog_micmacstepvalueandspatialresolution.h"

#include "../toolbox/toolbox_conversion.h"

#include "../toolbox/toolbox_pathAndFile.h"

#include "ui/MainWindowMenu.h"

#include "dialog_addboxesautomaticdistribution_selectmethod.h"

#include "../../logic/model/core/enum_automaticDistributionMethod.h"

#include "dialog_selectoneoflayers.h"

#include "WindowImageView.h"

#include "dialog_stackedProfilBoxes.h"

#include "dialog_project_inputfiles_fixingMissingFiles.h"

#include "ui/dialogStateSwitchConfirm/messageBoxWithVariableAnswers.h"

#include "ui/project/dialog_project_inputfiles_viewContentOnly.h"
#include "ui/project/dialog_exportresult.h"

#include "ui/project/dialog_exportresult.h"

#include "ui/geoCRSSelect/dialog_geoCRSSelect_tableview.h"

#include "logic/model/core/geoConvertRoute.h"

#include <QGuiApplication>

bool ApplicationController::project_new() {

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected request #0 : _ptrAppModel is nullptr";
        return(false);
    }
    if (!_ptrAppView) {
        qDebug() << __FUNCTION__ << "rejected request #1 : _ptrAppView is nullptr";
        return(false);
    }

    qDebug() << __FUNCTION__ << "_mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();

    bool bNewProjectIsPossible = true;
    bNewProjectIsPossible &= (_mainAppState.get_eAppState() == MainAppState::eMAS_idle);
    bNewProjectIsPossible &= (_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet)
                            &(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet_editingRoute);
    if (!bNewProjectIsPossible) {
        qDebug() << __FUNCTION__ << "rejected request #100";
        return(false);
    }

    bool bOkForNewProject = projectEditFct_questionAboutSaveSaveAs_and_saveProject();
    if (!bOkForNewProject) {
        return(false);
    }

    _ptrAppView->hideWindowsAbout_routeEditionAndProject();
    //clear many things in Appmodel for a new function start
    _ptrAppModel->setState_editingProject_closeState_clearProject();
    _mainAppState.setState(MainAppState::eMAS_editingProject_newProject_settingIO);

    _ptrAppView->ptrDialogProjectInputFile()->setStrStartingPathForFileSelection(_appRecall.get_lastUsedDirectory());


    /*int ieDialExecReturnCode = */_ptrAppView->show_dialog_projectInputFiles(true);

    return(true);
}

#include "../model/core/ComputationCore_structures.h" //just for e_LayersAcces //@#LP move e_LayersAcces in a dedicated header file

//we reach this slot when the user click cancel on the dialog project about input files
void ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonCancel_clicked() {

    switch (_mainAppState.get_eAppState()) {
        case MainAppState::eMAS_editingProject_newProject_settingIO:
            _appRecall.set_lastUsedDirectory(_ptrAppView->ptrDialogProjectInputFile()->getStrStartingPathForFileSelection());
            projectEditFct_aboutNewProject_dialogProjectEditPushButtonCancelClicked();
            break;

        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
            _appRecall.set_lastUsedDirectory(_ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->getStrStartingPathForFileSelection());
            projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonCancelClicked();
            break;
        default:
            break;
    }
}

//we reach this slot when the user click OK on the dialog project about input files
void ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonOK_clicked() {


    switch (_mainAppState.get_eAppState()) {
        case MainAppState::eMAS_editingProject_newProject_settingIO:
            _appRecall.set_lastUsedDirectory(_ptrAppView->ptrDialogProjectInputFile()->getStrStartingPathForFileSelection());
            projectEditFct_aboutNewProject_dialogProjectEditPushButtonOKClicked();
            break;

        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
            _appRecall.set_lastUsedDirectory(_ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->getStrStartingPathForFileSelection());
            projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonOKClicked();
            break;
        default:
            break;
    }
}

void ApplicationController::projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonCancelClicked() {
    qDebug() << __FUNCTION__;
    _ptrAppView->hide_dialog_projectEdition();
    clearVarStates();
    _mainAppState.setState(MainAppState::eMAS_idle);
}

void ApplicationController::projectEditFct_aboutNewProject_dialogProjectEditPushButtonCancelClicked() {
    qDebug() << __FUNCTION__;
    _ptrAppView->hide_dialog_projectEdition();
    clearVarStates();
    _mainAppState.setState(MainAppState::eMAS_idle);
}

//we reach this slot when the user click OK on the dialog project about input files
void ApplicationController::projectEditFct_aboutNewProject_dialogProjectEditPushButtonOKClicked() {

    S_InputFiles inputFiles = _ptrAppView->ptrDialogProjectInputFile()->get_inputFiles_imageAndCorrelationScoreMap();

    //qDebug() << __FUNCTION__ << "qvectDequantizationStep_PX1_PX2_deltaZ = " << qvectDequantizationStep_PX1_PX2_deltaZ;

    qDebug() << __FUNCTION__ << "sInputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ = " << inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << "sInputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ = " << inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ;

    QVector<bool> qvectb_layersForComputation(3, false);
    QVector<bool> qvectb_correlScoreMapToUseForComp(3, false);

    int nbLayersForComputation = 0;
    int nbcorrelScoreMapForComp = 0;

    QString strMsgErrorDetails;

    bool bCheckReport = _ptrAppModel->//projectEditionFunction_setInputFilesForComputation_and_checkMatchWithRequierement(
        projectEditionFunction_checkInputFilesForComputationMatchWithRequierement(
        inputFiles,
        e_mCM_Typical_PX1PX2Together_DeltazAlone,//set to sole mode for now: e_mCM_Typical_PX1PX2Together_DeltazAlone
        qvectb_layersForComputation, nbLayersForComputation,
        qvectb_correlScoreMapToUseForComp, nbcorrelScoreMapForComp,
        strMsgErrorDetails);

    qDebug() <<  __FUNCTION__ << "qvectb_correlScoreMapToUseForComp = " << qvectb_correlScoreMapToUseForComp;

    if (!bCheckReport) {
        QMessageBox messageBox(QMessageBox::Information,
                               "issue with input files",
                               strMsgErrorDetails,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        return;
    }


    S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sbASMsgStatusA_gRISS_aboutProjectEdition;

    if (!_bUseGeoRefForProject) {

        _ptrAppModel->projectEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_fromInputImageFilenames(inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ);

    } else {

        //here if enough filename are selected/entered in the lineEdit with valid dequantization value (regex valid string at this step)
        //route is selected for importation or create later is checkes instead
        //entered filename can be non existing, we will test this now before more check about input files

        //check that the tfw are sync for all input images
        S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus = _ptrAppModel->
            projectEditionFunction_set_GeoRefModelImagesSet_fromInputImageFilenames(inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ);

        //S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sbASMsgStatusA_gRISS_aboutProjectEdition;
        sbASMsgStatusA_gRISS_aboutProjectEdition = _ptrAppModel->projectEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus();

        bool bTFW_available = (se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases == e_iSS_geoTFWData_rTBC_available);
        if (!bTFW_available) {
            QMessageBox messageBox(QMessageBox::Information,
                        sbASMsgStatusA_gRISS_aboutProjectEdition._strMsgAboutWorldFileData,
                        sbASMsgStatusA_gRISS_aboutProjectEdition._strMsgAboutWorldFileData_errorDetails,
                        QMessageBox::Ok, nullptr);

            switch (se_geoRefImagesSetStatus._eiSSgTFW_reducedToBigCases) {
                case e_iSS_geoTFWData_rTBC_available://should never happen here; handled in a dedicated code section below
                case e_iSS_geoTFWData_rTBC_notSetValue://should never happen
                    qDebug() << __FUNCTION__ << __LINE__ << ": dev error: case which should never happen";
                    projectEditFct_clearState_backTo_DoNothing();
                    return;

                case e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData:
                case e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason:
                case e_iSS_geoTFWData_rTBC_notAvailable_noneFound:
                    break;
            }
            messageBox.exec();
            return;
        }

        if (  (se_geoRefImagesSetStatus._eiSS_geoEPSG != e_iSS_geoEPSG_available)
            &&(se_geoRefImagesSetStatus._eiSS_geoEPSG != e_iSS_geoEPSG_notAvailable_noneFound)) {

            QMessageBox messageBox(QMessageBox::Information,
                        sbASMsgStatusA_gRISS_aboutProjectEdition._strMsgAboutEPSG,
                        sbASMsgStatusA_gRISS_aboutProjectEdition._strMsgAboutEPSG_errorDetails,
                        QMessageBox::Ok, nullptr);

            switch (se_geoRefImagesSetStatus._eiSS_geoEPSG) {
                case e_iSS_geoEPSG_notSetValue:
                case e_iSS_geoEPSG_available:
                case e_iSS_geoEPSG_notAvailable_noneFound:
                    qDebug() << __FUNCTION__ << __LINE__ << ": dev error: case which should never happen";
                    projectEditFct_clearState_backTo_DoNothing();
                    return;

                case e_iSS_geoEPSG_notAvailable_unsyncEPSGData:
                case e_iSS_geoEPSG_notAvailable_unknowError:
                    break;
            }
            messageBox.exec();
            return;
        }
    }

    sync_UseGeoRefForTrace_ofControllerAndModel_with_UseGeoRefForProject();

    _ptrAppView->hide_dialog_projectEdition();

    if (_bUseGeoRefForProject) {
        //feed the dialogGeoRefInfos before exec on it:
        _ptrAppModel->sendStr_worldFileDataToView();
        _ptrAppModel->sendStr_imagesFilename();
        _ptrAppModel->sendEPSGCodeToView();
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
        _ptrAppModel->projectEditionFunction_sendStr_geoRefImagesSetStatus();
    }

    QVector<float> qvectf_micmacStep_PX1_PX2_deltaZ        {1.0, 1.0, 1.0};
    QVector<float> qvectf_spatialResolution_PX1_PX2_deltaZ {1.0, 1.0, 1.0};
    //@LP use string directly to feed the dialog instead of converting float to QString
    QVector<QString> qvectQstr_micmacStep_PX1_PX2_deltaZ        {"1", "1", "1"};
    QVector<QString> qvectQstr_spatialResolution_PX1_PX2_deltaZ {"1", "1", "1"};

    enum e_stepsInLoop_AboutNewProject {
        e_sINLANP_dialogGeoRefInfos,
        e_sINLANP_micMacStepValueAndSpatialResolution,
        e_sINLANP_selectLayerToDisplayAsBackgroundImage,
        e_sINLANP_loopExit
    };

    e_stepsInLoop_AboutNewProject eSINLANP_CurrentStep = e_sINLANP_dialogGeoRefInfos;
    if (!_bUseGeoRefForProject) {
        eSINLANP_CurrentStep = e_sINLANP_micMacStepValueAndSpatialResolution;
    }


    int idxLayerToDisplayAsBackgroundImage = -1;

    while (1) { //need to confirm micmacstepvalue and spatial resolution to quit the loop which is linked to the geoinfos

        if (_bUseGeoRefForProject) {

            if (eSINLANP_CurrentStep == e_sINLANP_dialogGeoRefInfos) {

                int ieDialExecReturnCode = _ptrAppView->exec_dialogGeoRefInfos();
                if (ieDialExecReturnCode == QDialog::Rejected) {
                    qDebug() << __FUNCTION__ << " canceled by user";

                    //back to the project edition dialog:
                    /*int ieDialExecReturnCode = */_ptrAppView->show_dialog_projectInputFiles(false);//stay on previous feed field
                    return;
                }

                //epsg code available but it's possible that some image do not have the code in the cache
                //propose to associate for futur usage:

                proposeToAssociateTheEPSGCodeForImageWithout(sbASMsgStatusA_gRISS_aboutProjectEdition._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation);

                //###LP route container is cleared in case of error: difference needs to be done when the route does not intersect with the image:
                //###replace the content in the model side, or reload from here.
                //###idx selected route for import needs to be cleared or reset after reload

                bool bGeoRouteConversion = _ptrAppModel->projectEditionFunction_geoConvertSelectedRouteFromRouteset_andKeepAlone();

                if (!bGeoRouteConversion) {

                    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaSMsgStatusA_GCAIR_aboutProjectEdition =
                            _ptrAppModel->get_sboolAndStrMsg_aboutGeoConvertAndImportRoute();

                    QMessageBox messageBox(QMessageBox::Information,
                                sbaSMsgStatusA_GCAIR_aboutProjectEdition._strMessage,
                                "Error: " + sbaSMsgStatusA_GCAIR_aboutProjectEdition._strMessage_errorDetails,
                                QMessageBox::Ok, nullptr);
                    messageBox.exec();

                    //back to the project edition dialog:
                    /*int ieDialExecReturnCode = */_ptrAppView->show_dialog_projectInputFiles(false);//stay on previous feed field
                    return;
                }
                eSINLANP_CurrentStep = e_sINLANP_micMacStepValueAndSpatialResolution;
            }
        } else {

            bool bGeoRouteConversion = _ptrAppModel->projectEditionFunction_geoConvertSelectedRouteFromRouteset_andKeepAlone();

            if (!bGeoRouteConversion) {

                S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaSMsgStatusA_GCAIR_aboutProjectEdition =
                        _ptrAppModel->get_sboolAndStrMsg_aboutGeoConvertAndImportRoute();

                QMessageBox messageBox(QMessageBox::Information,
                            sbaSMsgStatusA_GCAIR_aboutProjectEdition._strMessage,
                            "Error: " + sbaSMsgStatusA_GCAIR_aboutProjectEdition._strMessage_errorDetails,
                            QMessageBox::Ok, nullptr);
                messageBox.exec();

                //back to the project edition dialog:
                /*int ieDialExecReturnCode = */_ptrAppView->show_dialog_projectInputFiles(false);//stay on previous feed field
                return;
            }
        }

        if (eSINLANP_CurrentStep == e_sINLANP_micMacStepValueAndSpatialResolution) {

            _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersFilenameAndSetVisibility(
                        inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ,
                        qvectb_layersForComputation);

            _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersmicMacStepValueAndSpatialResolution(
                qvectQstr_micmacStep_PX1_PX2_deltaZ,
                qvectQstr_spatialResolution_PX1_PX2_deltaZ,
                qvectb_layersForComputation);

            int ieDialExecReturnCode = _ptrAppView->exec_dialogMicmacStepAndSpatialResolution();
            if (ieDialExecReturnCode == QDialog::Rejected) {
                qDebug() << __FUNCTION__ << " canceled by user";

                if (_bUseGeoRefForProject) {
                    //back to geo dialog:
                    eSINLANP_CurrentStep = e_sINLANP_dialogGeoRefInfos;
                    continue;
                } else {
                    //back to the project edition dialog:
                    /*int ieDialExecReturnCode = */_ptrAppView->show_dialog_projectInputFiles(false);//stay on previous feed field
                    return;
                }
            }

            //stay in the loop until any potential errors linked with geo infos checked not present (micMacStepValueAndSpatialResolution)
            bool bValid = _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
                                                                                          qvectf_micmacStep_PX1_PX2_deltaZ,
                                                                                          qvectf_spatialResolution_PX1_PX2_deltaZ);

            //should never happen:
            if (!bValid) {
                QMessageBox messageBox(QMessageBox::Information,
                    "Dev error #300",
                    "get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(...) said invalid",
                    QMessageBox::Ok, nullptr);
                messageBox.exec();

                if (_bUseGeoRefForProject) {
                    //back to geo dialog:
                    eSINLANP_CurrentStep = e_sINLANP_dialogGeoRefInfos;
                    continue;
                } else {
                    return;
                }
            }
            eSINLANP_CurrentStep = e_sINLANP_selectLayerToDisplayAsBackgroundImage;
        }

        if (eSINLANP_CurrentStep == e_sINLANP_selectLayerToDisplayAsBackgroundImage) {

            if (!nbLayersForComputation) {
                //@#LP no ui msg error
                qDebug() << __FUNCTION__ << "Dev error: nbLayersToCompute is zero but projectEditionFunction_loadBackgroundImage... would be reached";
                projectEditFct_clearState_backTo_DoNothing();
                return;
            }

            idxLayerToDisplayAsBackgroundImage = -1;
            bool bCanceledByTheUser = false;
            bool bOneLayerAvailableSelected = selectLayerToDisplayAsBackgroundImage(qvectb_layersForComputation, idxLayerToDisplayAsBackgroundImage, bCanceledByTheUser);
            if (!bOneLayerAvailableSelected) {
                if (bCanceledByTheUser) {
                    eSINLANP_CurrentStep = e_sINLANP_micMacStepValueAndSpatialResolution;
                    continue;
                } else {
                    projectEditFct_clearState_backTo_DoNothing();
                    return;
                }
            }
            eSINLANP_CurrentStep = e_sINLANP_loopExit;
        }

        if (eSINLANP_CurrentStep == e_sINLANP_loopExit) {
            break;
        }
    }



    qDebug() << __FUNCTION__ << "qvectb_layersToCompute = " << qvectb_layersForComputation;
    //qDebug() << __FUNCTION__ << "qvectb_layersToDisplay = " << qvectb_layersToDisplay;
    qDebug() << __FUNCTION__ << "sInputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ = " << inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ;

    //--------
    bool bReport = _ptrAppModel->set_micmacStepAndSpatialResolution_forModels(qvectf_micmacStep_PX1_PX2_deltaZ,
                                                                              qvectf_spatialResolution_PX1_PX2_deltaZ,
                                                                              qvectb_layersForComputation, true,
                                                                              false);
    if (!bReport) {
        QMessageBox messageBox(QMessageBox::Information,
                    "Internal error", //@#LP add dev error number
                    "failed in set_micmacStepAndSpatialResolution_forModel",
                    QMessageBox::Ok, nullptr);
        messageBox.exec();
        projectEditFct_clearState_backTo_DoNothing();

        return;
    }


    e_LayersAcces eLA = static_cast<e_LayersAcces>(idxLayerToDisplayAsBackgroundImage);


    if (_bUseGeoRefForProject) {
        bool bReport = _ptrAppModel->init_geoProj_onlyOneImageDisplayed(eLA);
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoProj error", //@#LP add dev error number
                        "failed in init_geoProj", //@#LP more details ?
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            projectEditFct_clearState_backTo_DoNothing();
            return;
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);

    } else {
        bool bReport = _ptrAppModel->init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(eLA);
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoUiInfos error", //@#LP add dev error number
                        "failed in init_geoUiInfos_noGeoRefUsed...()", //@#LP more details ?
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            projectEditFct_clearState_backTo_DoNothing();
            return;
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY);
    }

    _mainAppState.permitsMenuActionAboutGeoinfoData(_bUseGeoRefForProject);

    /*_ptrAppView->open_dialog_taskInProgress(Dialog_taskInProgress::e_PageDTIP_messagePleaseWait_noAbortButton,
                                            "processing...", "Building image pyramid", 0, 100);
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));*/

    bool bBackgroundImagesLoaded = _ptrAppModel->
            projectEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ,
                qvectb_layersForComputation, idxLayerToDisplayAsBackgroundImage);

    /*QGuiApplication::restoreOverrideCursor();
    _ptrAppView->close_dialog_taskInProgress();*/

    if (!bBackgroundImagesLoaded) {

        S_boolAndStrErrorMessage_aboutLoad SbaStrErrorMessage_aboutLoad;
        SbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sboolAndStrMsgAbout_aboutLoad();

           QMessageBox messageBox(QMessageBox::Information,
                       "Image load error",
                       SbaStrErrorMessage_aboutLoad._strMessagAboutImage,
                       QMessageBox::Ok, nullptr);
           messageBox.exec();
           projectEditFct_clearState_backTo_DoNothing();

           return;
    }

    //, inputFiles of appmodel and computation core are stored independant
    //at this step, we do not have set the inputfiles for the computation core

    _ptrAppModel->setDefaultComputationModeAndParameters_ifNotSet();

    /*bReport = */_ptrAppModel->setInputFilesAndCorrelationScoreFiles(inputFiles, qvectb_correlScoreMapToUseForComp);

    //adding missing set about _eRouteEditionStateOfRouteOfProject
    _eRouteEditionStateOfRouteOfProject = e_rES_locked;
    _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);
    _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible( _eRouteEditionStateOfRouteOfProject != e_rES_inEdition);

    _ptrAppView->hide_dialog_projectEdition();
    _ptrAppModel->projectEditionFunction_initState_andFeedGraphicsItemContainer(true);

    _bLayerSwitchIsPermitted = false;
    _ptrAppView->ptrWindowImageView()->setLayersNamesForLayersSwitcher(_qvectStrLayersName);
    _ptrAppView->ptrWindowImageView()->setAvailableLayersForLayersSwitcher(qvectb_layersForComputation, idxLayerToDisplayAsBackgroundImage);
    _bLayerSwitchIsPermitted = true;

    _ptrAppView->showImageViewWindows(true, true);

    _mainAppState.setState(MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes);

    _ptrAppView->ptrDialog_project_inputFiles_viewContentOnly()->setEditedProject(
                inputFiles,
                qvectb_layersForComputation,
                _ptrAppModel->getProjectRouteName());

}

//return false if selection was user rejected or failed (error)
//bLayerSelectionCanceledByTheUser tell is this is a user rejection
bool ApplicationController::selectLayerToDisplayAsBackgroundImage(const QVector<bool>& qvectb_layersForComputation,
                                                                  int &idxLayerToDisplayAsBackgroundImage,
                                                                  bool& bCanceledByTheUser) {

    bCanceledByTheUser = false;

    int countLayersForComputation = qvectb_layersForComputation.count(true);

    qDebug() << __FUNCTION__ << "countLayersForComputation = " << countLayersForComputation;

    idxLayerToDisplayAsBackgroundImage = -1;

    //only one layer (should be deltaZ) = no choice to the user to choose which one if want as background image; and no layer switch possible
    if (countLayersForComputation == 1) {
        //set on first available for now:
        idxLayerToDisplayAsBackgroundImage = qvectb_layersForComputation.indexOf(true);
        if (idxLayerToDisplayAsBackgroundImage == -1) {
            qDebug() << __FUNCTION__ << "Dev error: should never happens since nbLayersToCompute is not zero";
            //projectEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        return(true);
    }

    //user can choose wich layers he want as background image for project; before any layer switch
    Dialog_selectOneOfLayers dialSelectOneLayer;
    dialSelectOneLayer.setLayersName(_qvectStrLayersName);
    dialSelectOneLayer.setAvailableLayers(qvectb_layersForComputation);

    int iReport = dialSelectOneLayer.exec();
    if (iReport == QDialog::Rejected) {
        qDebug() << "QDialog::Rejected";
        bCanceledByTheUser = true;
        return(false);
    }
    // (iReport == QDialog::Accepted) {
    qDebug() << "QDialog::Accepted : selected = ";
    idxLayerToDisplayAsBackgroundImage = dialSelectOneLayer.getSelected();
    return(true);
}



//when the selected .jtraces is different than the previous we need to invalid the possiblie selected routeId
//and this leads to no selection in trace selection ui
//in the case of the same file is selected but content has changed since the last load,
//the ui will invalidate the selected routeId in ui side if it can fit in the traces count, leading to a mandatory user trace selection
void ApplicationController::slot_projectEditFct_newProject_selected_filename_routeset_forRouteImport_hasChanged() {
    //_ptrAppView->ptrDialogSelectRouteFromRouteset()->set_selectedRouteId(-1);
    _ptrAppModel->projectEditionFunction_setAsInvalid_selectedRouteFromRouteset();
}

//this slot is called when the user press the "select route " from the project_edition dialog
//the route selection dialog execution is launched by the controller, not internally by the project_edition dialog
//to avoid to needs to clear the model from the ui if needs
//@#LP refactorize design about this ?
void ApplicationController::slot_projectEditFct_newProject_routeSelectionRequestForImport(QString strRoutesetFilename) {

    if (!_ptrAppModel) {
        return;
    }

    int previous_selectedRouteId_forRouteImport = _ptrAppModel->projectEditionFunction_get_selectedRouteId_forRouteImport();

    qDebug() << __FUNCTION__ << "previous_selectedRouteId_forRouteImport = " << previous_selectedRouteId_forRouteImport;


    qDebug() << __FUNCTION__ << "call now _ptrAppModel->ptrRouteContainer()->slot_addNewRoute(strRouteTitle)";

    _ptrAppModel->setState_editingProject_closeState_clearProject();
    _mainAppState.setState(MainAppState::eMAS_editingProject_newProject_settingIO);

    _ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);

    bool bRoutesetFileLoaded = _ptrAppModel->projectEditionFunction_setAndLoad_routesetFileForRouteImport(strRoutesetFilename);

    if (!bRoutesetFileLoaded) {

        S_boolAndStrErrorMessage_aboutLoad sbaStrErrorMessage_aboutLoad =  _ptrAppModel->get_sbaStrErrorMessage_aboutLoad();

        qDebug() << __FUNCTION__ << "sbaStrErrorMessage_aboutLoad._strMessageAboutJson = " << sbaStrErrorMessage_aboutLoad._strMessageAboutJson;

        QMessageBox messageBox(QMessageBox::Information,
                               "error loading traceset (.jtraces file)",
                               sbaStrErrorMessage_aboutLoad._strMessageAboutJson,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        _ptrAppModel->setState_editingProject_closeState_clearProject();
        _mainAppState.setState(MainAppState::eMAS_editingProject_newProject_settingIO);

    } else {

        if (_ptrAppModel->get_useGeoRefForTrace() != _bUseGeoRefForProject) {

            QString strMsg;
            if (_bUseGeoRefForProject) {
                strMsg = "Selected traceset is not georeferenced. It can not be used to create a georeferenced project";
            } else {
                strMsg = "Selected traceset is georeferenced. It can not be used to create a not georeferenced project";
            }

            QMessageBox messageBox(QMessageBox::Information,
                                   "traceset (.jtraces file)",
                                    strMsg,
                                   QMessageBox::Ok, nullptr);
            messageBox.exec();
            _ptrAppModel->setState_editingProject_closeState_clearProject();
            _mainAppState.setState(MainAppState::eMAS_editingProject_newProject_settingIO);

        } else {

            qDebug() << __FUNCTION__ << "previous_selectedRouteId_forRouteImport = " << previous_selectedRouteId_forRouteImport;

            //_ptrAppModel->projectEditionFunction_sendStr_routesetFilenameForRouteImport_toView();
            int ieDialExecReturnCode = _ptrAppView->exec_dialog_project_selectRouteFromRouteset(strRoutesetFilename,
                                                                                                previous_selectedRouteId_forRouteImport);

            //get the routeId_selectedForImport from the ui,
            //get index even if selection is canceled (routeId must be -1 this case)
            int routeId_selectedForImport = _ptrAppView->ptrDialogSelectRouteFromRouteset()->getSelectedRouteId();

            //canceled selection can lead to 'stay on previous selection'
            if (ieDialExecReturnCode  == QDialog::Rejected) {
                /*if (routeId_selectedForImport == -1) {
                    qDebug() << __FUNCTION__ << " user canceled the route selection";
                    //stay on the previous selection (can be none)
                    return;
                }*/
                routeId_selectedForImport = -1;
            }

            //send to the model which will send infos to the ui to update its widgets
            _ptrAppModel->projectEditionFunction_selectedRouteFromRouteset(routeId_selectedForImport);
        }
    }
}

void ApplicationController::slot_projectEditFct_newProject_addADefaultRouteStateChanged(bool bAddADefaultRoute) {

    if (!_ptrAppModel) {
        return;
    }

    if (bAddADefaultRoute) {
        _ptrAppModel->projectEditionFunction_setStateAddADefaultRoute(bAddADefaultRoute);
        return;
    }

    //here if bAddADefaultRoute is false

    //we need to check and clear if requiered the selected route and routeset filename
    //if the georef is unsync with the current project use georef state

    bool bContainsRoute = false;
    bool bRouteSetUseGeoRef = false;
    _ptrAppModel->projectEditionFunction_forRouteImport_routeSet_states_bContainsRoute_bUseGeoRef(bContainsRoute, bRouteSetUseGeoRef);

    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bContainsRoute:" << bContainsRoute;
    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bRouteSetUseGeoRef:" << bRouteSetUseGeoRef;

    //--- current mode is "import a trace"
    //no route selected, no it's not sure that the traceset is georef or not
    //the futur push button request to selection trace will detect if the georef is not sync with the user choice about use georef
    if (!bContainsRoute) {
        qDebug() << __FUNCTION__ << "if (!bContainsRoute) {";
        //_bUseGeoRefForProject = bUseGeoreferencingForProject;
        //_ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
        return;
    }

    //contains a route. Hence the user as selected one previously the use or not georef request
    //and the route geosref state is unsync
    if (_bUseGeoRefForProject != bRouteSetUseGeoRef) {

        qDebug() << __FUNCTION__ << "if (_bUseGeoRefForProject != bRouteSetUseGeoRef) {";

        //the route georef usage is unsync. hence we clear the selected route and routeset filename

        /*QString strMsg;
        if (!bRouteSetUseGeoRef) {
            strMsg = "Selected traceset is not georeferenced. Click OK to switch to 'use georeferencing' and select another traceSet";
        } else {
            strMsg = "Selected traceset is georeferenced. Click OK to switch to 'don't use georeferencing' and select another traceSet";
        }

        QMessageBox messageBox(QMessageBox::Question,
                  "Traceset and georeferencing",
                  strMsg,
                  QMessageBox::Ok|QMessageBox::Cancel, nullptr);
        int ieDialExecReturnCode = messageBox.exec();
        bool bSwitchGeoRefStateConfirmed = (ieDialExecReturnCode == QMessageBox::Ok);
        if (bSwitchGeoRefStateConfirmed) {
        */

        _ptrAppView->ptrDialogProjectInputFile()->clearTraceSetAndTraceSelection_forRouteImport();
        _ptrAppModel->projectEditionFunction_forRouteImport_clearTraceSetAndTraceSelection_forRouteImport();

        //_bUseGeoRefForProject = bUseGeoreferencingForProject;
        //_ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
        /*
        } else {

            //replace the previous state about checkbox in ui
            _ptrAppView->ptrDialogProjectInputFile()->forceState_bUseGeoref(_bUseGeoRefForProject);

        }
        */
    } else {

        qDebug() << __FUNCTION__ << "_bUseGeoRefForProject == bRouteSetUseGeoRef";

        //_bUseGeoRefForProject = bUseGeoreferencingForProject;
        //_ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);

    }


/*
    bool bContainsRoute = false;
    bool bRouteSetUseGeoRef = false;
    _ptrAppModel->projectEditionFunction_forRouteImport_routeSet_states_bContainsRoute_bUseGeoRef(bContainsRoute, bRouteSetUseGeoRef);

    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bContainsRoute:" << bContainsRoute;
    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bRouteSetUseGeoRef:" << bRouteSetUseGeoRef;

    if (!bContainsRoute) {
        _ptrAppModel->projectEditionFunction_setStateAddADefaultRoute(bActivated);
        return;
    }

    if (bRouteSetUseGeoRef != _bUseGeoRefForProject) {

        QString strMsg;
        if (!bRouteSetUseGeoRef) {
            strMsg = "Selected traceset is not georeferenced. Click OK to switch to 'use georeferencing' and select another traceSet";
        } else {
            strMsg = "Selected traceset is georeferenced. Click OK to switch to 'don't use georeferencing' and select another traceSet";
        }

        QMessageBox messageBox(QMessageBox::Question,
                  "Traceset and georeferencing",
                  strMsg,
                  QMessageBox::Ok|QMessageBox::Cancel, nullptr);
        int ieDialExecReturnCode = messageBox.exec();
        bool bSwitchGeoRefStateConfirmed = (ieDialExecReturnCode == QMessageBox::Ok);
        if (bSwitchGeoRefStateConfirmed) {

            //_ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
            _ptrAppView->ptrDialogProjectInputFile()->forceState_bUseGeoref(_bUseGeoRefForProject);
            _ptrAppView->ptrDialogProjectInputFile()->clearTraceSetAndTraceSelection_forRouteImport();
            _ptrAppModel->projectEditionFunction_forRouteImport_clearTraceSetAndTraceSelection_forRouteImport();

        } else {

            //replace the previous state about checkbox in ui
            _ptrAppView->ptrDialogProjectInputFile()->forceState_bUseGeoref(_bUseGeoRefForProject);

        }

    } else {

       //do nothing

    }
*/
}

void ApplicationController::slot_projectEditFct_newProject_useGeoreferencing_stateChanged(bool bUseGeoreferencingForProject) {

    if (!_ptrAppModel) {
        return;
    }

    bool bContainsRoute = false;
    bool bRouteSetUseGeoRef = false;
    _ptrAppModel->projectEditionFunction_forRouteImport_routeSet_states_bContainsRoute_bUseGeoRef(bContainsRoute, bRouteSetUseGeoRef);

    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bContainsRoute:" << bContainsRoute;
    qDebug() << __FUNCTION__ << "forRouteImport_routeSet, bRouteSetUseGeoRef:" << bRouteSetUseGeoRef;

    //--- current mode is "add a default route"
    //but as we permits to have in the same time a possible selected traceset file for route import
    //we have to take that in consideration to avoid to permit to switch on this traceset if it's not in the useGeoRef state that the projet checkbox use GeoRef
    if (_ptrAppModel->projectEditionFunction_getStateAddADefaultRoute()) {
        qDebug() << __FUNCTION__ << "if (_ptrAppModel->projectEditionFunction_getStateAddADefaultRoute()) {";
        _bUseGeoRefForProject = bUseGeoreferencingForProject;
        _ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
        return;
    }

    //--- current mode is "import a trace"
    //no route selected, no it's not sure that the traceset is georef or not
    //the futur push button request to selection trace will detect if the georef is not sync with the user choice about use georef
    if (!bContainsRoute) {
        qDebug() << __FUNCTION__ << "if (!bContainsRoute) {";
        _bUseGeoRefForProject = bUseGeoreferencingForProject;
        _ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
        return;
    }

    //contains a route. Hence the user as selected one previously the use or not georef request
    //and the route geosref state is unsync
    if (bUseGeoreferencingForProject != bRouteSetUseGeoRef) {

        qDebug() << __FUNCTION__ << "if (bState != bRouteSetUseGeoRef) {";

        //the route georef usage is unsync. hence we clear the selected route and routeset filename

        /*QString strMsg;
        if (!bRouteSetUseGeoRef) {
            strMsg = "Selected traceset is not georeferenced. Click OK to switch to 'use georeferencing' and select another traceSet";
        } else {
            strMsg = "Selected traceset is georeferenced. Click OK to switch to 'don't use georeferencing' and select another traceSet";
        }

        QMessageBox messageBox(QMessageBox::Question,
                  "Traceset and georeferencing",
                  strMsg,
                  QMessageBox::Ok|QMessageBox::Cancel, nullptr);
        int ieDialExecReturnCode = messageBox.exec();
        bool bSwitchGeoRefStateConfirmed = (ieDialExecReturnCode == QMessageBox::Ok);
        if (bSwitchGeoRefStateConfirmed) {
        */

        _ptrAppView->ptrDialogProjectInputFile()->clearTraceSetAndTraceSelection_forRouteImport();
        _ptrAppModel->projectEditionFunction_forRouteImport_clearTraceSetAndTraceSelection_forRouteImport();

        _bUseGeoRefForProject = bUseGeoreferencingForProject;
        _ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);
        /*
        } else {

            //replace the previous state about checkbox in ui
            _ptrAppView->ptrDialogProjectInputFile()->forceState_bUseGeoref(_bUseGeoRefForProject);

        }
        */
    } else {

        qDebug() << __FUNCTION__ << "bState === bRouteSetUseGeoRef";

        _bUseGeoRefForProject = bUseGeoreferencingForProject;
        _ptrAppModel->set_useGeoRefForProject(_bUseGeoRefForProject);

    }
}

bool ApplicationController::project_save() {
    qDebug() << __FUNCTION__;

    AppFile appFile_project = _ptrAppModel->getAppFile_project();

    bool bHasChanges = false;
    bool bCanBeSaved = projectEditFct_writeproject_handleNoChangeOrChangeNotCompatibleWithWriteOperation(true, bHasChanges);
    if (!bCanBeSaved) {
        return(false);
    }
    if (!appFile_project.filenameSet()) {
        return(project_saveAs());
    }
    if (bHasChanges) {
        return(projectEditFct_writeProject_withMessageBoxIfFail());
    }
    return(true);
}

bool ApplicationController::project_saveAs() {

    bool bHasChanges = false;
    bool bCanBeSaved = projectEditFct_writeproject_handleNoChangeOrChangeNotCompatibleWithWriteOperation(false, bHasChanges);
    if (!bCanBeSaved) {
        return(false);
    }
    //change or not, permits to "save as"

    QString strDotExtension = ".jstackprof";
    QString strFilter = "*" + strDotExtension;


    QString strStartingDir = _appRecall.get_lastUsedDirectory(); //StandardPathLocation::strStartingDefaultDir();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strSelectedfileName = QFileDialog::getSaveFileName(nullptr, tr("Save As"), strStartingDir, strFilter);
    if (strSelectedfileName.isEmpty()) {
        return(false);
    }
    if (!strSelectedfileName.endsWith(strDotExtension)) {
        strSelectedfileName+= strDotExtension;
    }
    bool bWriteReport = projectEditFct_writeProject_withMessageBoxIfFail(strSelectedfileName);
    _appRecall.set_lastUsedDirectoryFromPathFile(strSelectedfileName);
    return(bWriteReport);
}

bool ApplicationController::project_close() {

    //@#LP if project has no change => just close and process the close
    //@#LP if project has changes not compatible with write operation => message and return
    //@#LP Hence projectEditFct_writeproject_handleNoChangeOrChangeNotCompatibleWithWriteOperation need a report code(s) change

    qDebug() << __FUNCTION__;
    bool bOkForCloseSet = projectEditFct_questionAboutSaveSaveAs_and_saveProject();
    if (!bOkForCloseSet) {
        return(false);
    }

    _ptrAppView->ptrDialogProjectInputFile()->setStateAsEmptyProject();
    _ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->setStateAsEmptyProject();

    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->clearUiContent_all();

    _ptrAppView->ptrDialogGeoRefInfos()->clearUiContent_all_backToDefaultState();

    clearVarStates();

    slot_addingBoxModeCanceled();

    _ptrAppView->ptrDialog_stackedProfilBoxes()->set_addingBoxModeCanceled();

    _ptrAppView->ptrDialog_project_inputFiles_viewContentOnly()->setStateAsEmptyProject();

    projectEditFct_clearState_backTo_DoNothing();

    _ptrAppView->ptrDialog_stackedProfilBoxes()->backToState_noneBoxSelected();

    return(true);
}

void ApplicationController::clearVarStates() {

    _currentBoxId = -1;
    _bStackProfilEdit_syncYRange = false;

    _eRouteEditionStateOfRouteOfProject = e_rES_notApplicable;
    if (_ptrAppView) {
        if (_ptrAppView->ptrMainWindowMenu()) {
            _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);
        }
        if (_ptrAppView->ptrDialog_stackedProfilBoxes()) {
            _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(_eRouteEditionStateOfRouteOfProject != e_rES_inEdition);
        }
        if (_ptrAppView->ptrDialog_exportResult()) {
            _ptrAppView->ptrDialog_exportResult()->setCurrentBoxId(_currentBoxId);
            _ptrAppView->ptrDialog_exportResult()->setSomeFlaggedProfilExist(false, false);
        }
    }

    _qvectb_layersToCompute_justCheckingStringEmptiness.fill(false,3);
    _qvectb_correlationMapUsage_justCheckingStringEmptiness.fill(false,3);
    _inputFileWithAttributes_fromJsonFile.clear();

    _bAddBoxModeEntered = false;

    _bUseGeoRefForTrace = true;
    _bUseGeoRefForProject = true;

    _bLayerSwitchIsPermitted = false;

}



void ApplicationController::projectEditFct_clearState_backTo_DoNothing() { //@##LP check no missing usage

    _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu();
    _ptrAppView->hideWindowsAbout_routeEditionAndProject();
    _ptrAppView->ptrDialog_geoCRSSelect_tableView()->resetToDefault();

    //clear many things in Appmodel for a new function
    _ptrAppModel->setState_editingProject_closeState_clearProject();
    clearVarStates();
    _mainAppState.setState(MainAppState::eMAS_idle);
    _ptrAppModel->clearStringsError();
}

bool ApplicationController::projectEditFct_questionAboutSaveSaveAs_and_saveProject() {

    qDebug() << __FUNCTION__;

    AppFile appFile_project = _ptrAppModel->getAppFile_project();

    bool bWriteTheFile = false;
    bool bSaveAs = false;
    QString strProjectFilenameForSaveAs;

    bool bOkForNewFunction = false;

    bOkForNewFunction = handleDialogQuestionsAboutSaving_forAppFile(
                appFile_project,
                "Save Project?",
                "Some modifications are not saved yet", "Do you want to save it?",
                ".jstackprof",
                bWriteTheFile,
                bSaveAs,
                strProjectFilenameForSaveAs);

    qDebug() << __FUNCTION__ << "bWriteTheFile = " << bWriteTheFile;

    if (bWriteTheFile) {
        bool bWriteReport = false;
        if (bSaveAs) {
            bWriteReport = projectEditFct_writeProject_withMessageBoxIfFail(strProjectFilenameForSaveAs);
        } else {
            bWriteReport = projectEditFct_writeProject_withMessageBoxIfFail();
        }
        bOkForNewFunction = bWriteReport;
    }

    qDebug() << __FUNCTION__ << "final bOkForNewFunction = " << bOkForNewFunction;

    return(bOkForNewFunction);
}


bool ApplicationController::projectEditFct_writeProject_withMessageBoxIfFail(const QString& strProjectFilenameToSaveInto) {

    bool bWriteReport = false;

    if (strProjectFilenameToSaveInto.isEmpty()) {
        bWriteReport = _ptrAppModel->projectEditionFunction_writeProject();
    } else {
        bWriteReport = _ptrAppModel->projectEditionFunction_writeProject(strProjectFilenameToSaveInto);
    }

    QString strMsg;
    AppFile appFile_project = _ptrAppModel->getAppFile_project();
    if (!bWriteReport) {
        strMsg = "Can not save file: " + _ptrAppModel->projectEditionFunction_strDetailsAboutFail_projectWrite();// appFile_routeset.getFilename(); //+ _ptrAppModel->get_routesetFilename();
    } else {
        strMsg = "Saved !"; //// + appFile_routeset.getFilename() + " )"; //_ptrAppModel->get_routesetFilename();
    }
    QMessageBox messageBox(QMessageBox::Information,
                "Project file",
                strMsg,
                QMessageBox::Ok, nullptr);
    messageBox.setText(strMsg);
    messageBox.exec();

    if (bWriteReport) {
        _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu(_ptrAppModel->getAppFile_project().getFilename());
    }
    return(bWriteReport);
}

//return true if the file can be saved
//return false if the project content is not compatible with save operation
//bHasChanges is set by the method
bool ApplicationController::projectEditFct_writeproject_handleNoChangeOrChangeNotCompatibleWithWriteOperation(bool bWarnAboutNoChange, bool& bHasChanges) {

    AppFile appFile_project = _ptrAppModel->getAppFile_project();
    bHasChanges = appFile_project.modified();

    QString strDetailsAboutNotCompatibleWithSaving;
    if (!_ptrAppModel->projectEditionFunction_projectContentIsCompatibleWithWriteOperation(strDetailsAboutNotCompatibleWithSaving)) {
        QMessageBox messageBox(
            QMessageBox::Information,
            "project edition current state doesn't not permit save operations",
            strDetailsAboutNotCompatibleWithSaving,
            QMessageBox::Ok, nullptr);
        messageBox.exec();
        return(false);
    }

    return(true);
}

//edition -> lock ; lock -> edition
//return true if the switch was done
bool ApplicationController::projectEditFct_switch_eRouteEditionStateOfRouteOfProject() {

    if (_eRouteEditionStateOfRouteOfProject == e_rES_notApplicable) {
        qDebug() << __FUNCTION__ << "_eRouteEditionStateOfRouteOfProject is e_rES_notApplicable ( " << _eRouteEditionStateOfRouteOfProject << " )";
        return(false);
    }
    if (_eRouteEditionStateOfRouteOfProject == e_rES_inEdition) {
        _eRouteEditionStateOfRouteOfProject = e_rES_locked;        
        qDebug() << __FUNCTION__ << "_eRouteEditionStateOfRouteOfProject is now e_rES_locked << ( " << _eRouteEditionStateOfRouteOfProject << " )";
        return(true);
    }
    _eRouteEditionStateOfRouteOfProject = e_rES_inEdition;
    qDebug() << __FUNCTION__ << "_eRouteEditionStateOfRouteOfProject is now e_rES_inEdition << ( " << _eRouteEditionStateOfRouteOfProject << " )";
    return(true);
}

bool ApplicationController::projectEditFct_aboutRoute_switchRouteEditionstate() {

    if (!_ptrAppModel) { return(false); }
    if (!_ptrAppView) { return(false); }

    if (_eRouteEditionStateOfRouteOfProject == e_rES_notApplicable) {
        return(false);
    }

    if (_eRouteEditionStateOfRouteOfProject == e_rES_inEdition) {
        //trace lock request
        //check that the trace is valid for project life (more than one point)
        QString strErrorMsg;
        bool bReport = _ptrAppModel->projectEditFct_aboutRoute_checkRouteIsValid(strErrorMsg);

        if (!bReport) {
            messageBoxForUser("Trace lock rejected", strErrorMsg);
            return(false);
        }

        //update location of first and last point of the route
        _ptrAppModel->updateLocationOfSpecificItemsOfInterest_firstAndLastPointOfRoute();

        qDebug() << __FUNCTION__ << "call now _ptrAppModel->setState_editingRoute(selectedRouteTableViewRow)";
        _ptrAppModel->setState_editingRoute(e_AppModelState::eAModS_editingProject_editingRouteInsideProject,
                                            e_routeEditionState::e_rES_locked, 0);
        _ptrAppModel->set_projectContentStateS_as_noneBox();


        //enter in 'locked trace' mode
        projectEditFct_switch_eRouteEditionStateOfRouteOfProject();

        _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);        
        _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(_eRouteEditionStateOfRouteOfProject != e_rES_inEdition);//_ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(true);


        _mainAppState.setState(MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes);

        return(true);
    }


    //trace edition request

    //warn the user if some boxes exists along the current route
    //moving point lead to remove all existing boxes and edited linear regression model about profils if any

    e_Project_routeContentState eAMrouteWantedState = e_Project_routeContentState::e_P_routeCS_alteringRoute;
    S_StatusForFuturStateSwitch_withMessage sStatusForFuturStateSwitch_withMessage;
    sStatusForFuturStateSwitch_withMessage = (_ptrAppModel->getConstRef_projectContentState()).getStatusForFuturStateSwitch_withMessage(eAMrouteWantedState);

    bool bEnterInRouteEdition = false;

    if (sStatusForFuturStateSwitch_withMessage._eStatusForFuturStateSwitch == e_SFFSS_acceptable_noMsgForUser) {        
        bEnterInRouteEdition = true;
    } else {

        if (sStatusForFuturStateSwitch_withMessage._eStatusForFuturStateSwitch != e_SFFSS_acceptable_butWarning_QuestionOKCancel) {
            return(false);
        }

        MessageBoxWithVariableAnswers mesgBoxWVA_EditPrefOrQuit("Trace edition",
                                                     sStatusForFuturStateSwitch_withMessage._qstrMessage,
                                                     sStatusForFuturStateSwitch_withMessage._qstrQuestion,
                                                     MessageBoxWithVariableAnswers::e_MBPA_OKCancel);
        int iReturnCode = mesgBoxWVA_EditPrefOrQuit.exec();
        qDebug() << __FUNCTION__ << "mesgBoxWVA_EditPrefOrQuit.exec() iReturnCode = " << iReturnCode;
        if (iReturnCode == QMessageBox::Ok) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Trace edition");
            msgBox.setText("Confirmation required");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setInformativeText("Please confirm again to enter in trace edition");
            msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ieDialExecReturnCode = msgBox.exec();
            if (ieDialExecReturnCode == QMessageBox::Cancel) {
                qDebug() << __FUNCTION__ << " canceled by user";
                return(false);
            }

            slot_removeAllBoxes();
            bEnterInRouteEdition = true;
        }
    }

    if (bEnterInRouteEdition) {

        //enter in 'trace edition' mode
        projectEditFct_switch_eRouteEditionStateOfRouteOfProject();

        _ptrAppModel->slot_addingBoxModeCanceled(); //cancel adding mode in model if previously set

         //cancel adding mode in ui side if previously set
        _ptrAppView->ptrDialog_stackedProfilBoxes()->backToState_noneBoxSelected();//_ptrAppView->ptrDialog_stackedProfilBoxes()->slot_setBoxId_and_boxSize(-1,0,0); //back to none selected box

        //avoid adding box button of Dialog_stackedProfilBoxes to do something if developer forget to hide the dialog        
        _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(_eRouteEditionStateOfRouteOfProject != e_rES_inEdition);//_ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(false); //we consider setVisible(false) below as too weak to prevent adding action

        //hide the dialog Dialog_stackedProfilBoxes
        _ptrAppView->ptrDialog_stackedProfilBoxes()->setVisible(false);

        //set the trace point as highlighted
        qDebug() << __FUNCTION__ << "call now _ptrAppModel->setState_editingRoute(selectedRouteTableViewRow)";
        _ptrAppModel->setState_editingRoute(e_AppModelState::eAModS_editingProject_editingRouteInsideProject,
                                            e_routeEditionState::e_rES_inEdition, 0);


        _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);
        _mainAppState.setState(MainAppState::eMAS_editingProject_editingRouteInsideProject);
        return(true);
    }

    return(false);
}

bool ApplicationController::projectEditFct_aboutRoute_rename() {

    qDebug() << __FUNCTION__;

    int routeId = 0;
    QString qstrCurrentRouteName = _ptrAppModel->getProjectRouteName();
    QString qstrNewRouteName;
    int ieDialogCodeReport =_ptrAppView->exec_dialog_renameTrace(qstrCurrentRouteName, qstrNewRouteName);

    if (ieDialogCodeReport == QDialog::Accepted) {
        _ptrAppModel->renameRoute(routeId, qstrNewRouteName, true);
        _ptrAppView->ptrDialog_project_inputFiles_viewContentOnly()->setEditedProject_routeName(qstrNewRouteName);
    } else { //  QDialog::Rejected
        //do nothing
    }
    return(true);
}

bool ApplicationController::projectEditFct_aboutRoute_export() {
    qDebug() << __FUNCTION__;

    QString strErrorMsg;
    bool bReport = _ptrAppModel->projectEditFct_aboutRoute_checkRouteIsValid(strErrorMsg);
    if (!bReport) {
        messageBoxForUser(QMessageBox::Warning, "Trace export not possible", strErrorMsg);
        return(false);
    }

    QString strDotExtension = ".jtraces";
    QString strFilter = "*" + strDotExtension;

    QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strfileName = QFileDialog::getSaveFileName(nullptr, tr("Export trace as"), strStartingDir, strFilter);
    if (strfileName.isEmpty()) {
        return(false);
    }
    if (!strfileName.endsWith(strDotExtension)) {
        strfileName+= strDotExtension;
    }

    bool bWriteReport = routesetEditFct_writeRouteset_withMessageBoxIfFail(strfileName);
    return(bWriteReport);
}


bool ApplicationController::projectEditFct_addBoxesUsingAutomaticDistribution() {
    int iReport = _ptrAppView->exec_dialog_addBoxesAutomaticDistribution_selectMethod();
    if (iReport == QDialog::Rejected) {
        return(false);
    }

    bool bAddBoxes = false;

    //@#LP no nullptr check
    _ptrAppView->hide_dialogVLayoutStackProfilEdit();
    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults();

    _ptrAppModel->setVisible_startAndEndofRoute(true);

    switch (_ptrAppView->ptrDialog_addBoxesAutomaticDistribution_selectMethod()->get_selectedMethod()) {
        case e_ADM_closeAsPossible:
            bAddBoxes = _ptrAppView->exec_dialog_addBoxesAutomaticDistribution_closeAsPossible();
            break;

        case e_ADM_distanceBetweenCenter:
            bAddBoxes = _ptrAppView->exec_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter();
            break;

        default:
            break;
    }

    _ptrAppModel->setVisible_startAndEndofRoute(false);

    return(bAddBoxes);
}

void ApplicationController::slot_projectEditFct_dialog_addBoxesAutomaticDistribution_closeAsPossible_createBoxes_clicked(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int ieDistanceBSBuAD_selected) {

    //--- waitCursor
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //---

    QString strErrorReason;
    bool bCriticalError_pleaseExitApp = false;

    bool bReport= _ptrAppModel->addBoxesAutomaticDistribution_closeAsPossible(boxWidth, boxlength, ieStartingPointPFAD_selected, ieDistanceBSBuAD_selected,
                                                                              strErrorReason, bCriticalError_pleaseExitApp);
    //--- restoreOverrideCursor
    QGuiApplication::restoreOverrideCursor();
    //---

    if (!bReport) {

        if (bCriticalError_pleaseExitApp) {
            messageBoxForUser(QMessageBox::Critical, "Fatal error adding boxes",
                                                     "Boxes data are unsync. Quit the application Without saving the project");
        } else {
            messageBoxForUser("Adding boxes failed", strErrorReason);
        }
        return;
    }

    //allow computation settings and run
    _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);

    tryToSelectFirstBoxIfNoneSelected();

}


void ApplicationController::slot_projectEditFct_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter_createBoxes_clicked(int boxWidth, int boxlength,
                                                                                                                               int ieStartingPointPFAD_selected, int pixelDistanceBetweenCenter) {
    //--- waitCursor
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //---

    QString strErrorReason;
    bool bCriticalError_pleaseExitApp = false;

    bool bReport= _ptrAppModel->addBoxesAutomaticDistribution_distanceBetweenCenter(boxWidth, boxlength,
                                                                                    ieStartingPointPFAD_selected, pixelDistanceBetweenCenter,
                                                                                    strErrorReason, bCriticalError_pleaseExitApp);
    //--- restoreOverrideCursor
    QGuiApplication::restoreOverrideCursor();
    //---

    if (!bReport) {
        if (bCriticalError_pleaseExitApp) {
            messageBoxForUser(QMessageBox::Critical, "Fatal error adding boxes",
                                                     "Boxes data are unsync. Quit the application Without saving the project");
        } else {
            messageBoxForUser("Adding boxes failed", strErrorReason);
        }
        return;
    }

    //allow computation settings and run
    _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);

    tryToSelectFirstBoxIfNoneSelected();

}


void ApplicationController::tryToSelectFirstBoxIfNoneSelected() {

    int boxCount =  _ptrAppModel->ptrCoreComputation_inheritQATableModel()->getBoxCount();

    if (!boxCount) {
        _currentBoxId = -1;
        return;
    }

    //prevent invalid case in case of a buggy/missing _currentBoxId re-set (prepareComputedBoxForStackEdition below contains also a check for rejection)
    if (_currentBoxId >= boxCount) {
        _currentBoxId = 0;
    }

    //set on first box if box not set
    if (_currentBoxId == -1) {
        _currentBoxId = 0;
    }

    if (!_ptrAppView->ptrDialogVLayoutStackProfilEdit()->isVisible()) {
        qDebug() << __FUNCTION__ << "(ApplicationController) cal now _ptrAppModel->selectBox(_currentBoxId = " << _currentBoxId;
        _ptrAppModel->selectBox(_currentBoxId); //this will update the _currentBoxId for the model and selected the highlight the box in the graphicscontainer

    } else {
        qDebug() << __FUNCTION__ << "(ApplicationController) cal now slot_stackProfilEdit_switchToBox(_currentBoxId = " << _currentBoxId;
        slot_stackProfilEdit_switchToBox(_currentBoxId, true);
    }
}




bool ApplicationController::projectEditFct_setComputationParametersAndRun() {

    _ptrAppModel->setDefaultComputationModeAndParameters_ifNotSet();

    qDebug() << __FUNCTION__ ;
    _ptrAppModel->sendComputationParametersToView();

    int ieDialExecReturnCode = _ptrAppView->exec_dialog_computationParameters();
    if (ieDialExecReturnCode == QDialog::Rejected) {
        qDebug() << __FUNCTION__ << " canceled by user";
        return(false);
    }

    S_ComputationParameters computationParameters_edited = _ptrAppView->ptrDialog_computationparameters()->get_computationParameters_edited();

    qDebug() << __FUNCTION__  << "computationParameters_edited = ";
    computationParameters_edited.debugShow();

    _ptrAppModel->setComputationParameters_evaluatingIfEqualToCurrent(computationParameters_edited);

    bool bComputationIsRequiered_dueToParametersChanges = _ptrAppModel->isComputationRequiered_dueToParametersChanges();
    bool bComputationIsRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages = _ptrAppModel->isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages();

    if (  (!bComputationIsRequiered_dueToParametersChanges)
        &&(!bComputationIsRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages)) {
        qDebug() << __FUNCTION__ << "None computation parameters has changed. And none box needs to be recomputed.";
        messageBoxForUser("Computation", "None computation parameters has changed. And none box needs to be recomputed.");
        return(true);
    }

    _ptrAppView->hide_dialogVLayoutStackProfilEdit();
    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults();

    bool bReport = _ptrAppModel->runComputation(/*computationParameters_edited*/);
    qDebug() << __FUNCTION__ << "_ptrAppModel->setComputationParametersAndRun said " <<  bReport;
    if (!bReport) {
        /*QGuiApplication::restoreOverrideCursor();
        _ptrAppView->close_dialog_taskInProgress();*/

        messageBoxForUser(QMessageBox::Warning, "Stacked profiles", "Computation failed");

        return(false);
    }

    //--- WaitCursor
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bReport = _ptrAppModel->compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
    if (!bReport) {

        //-- restoreOverrideCursor
        QGuiApplication::restoreOverrideCursor();

        messageBoxForUser(QMessageBox::Warning, "Linear regression models", "Computation Failed");
        return(false);
    }

    //-- restoreOverrideCursor
    QGuiApplication::restoreOverrideCursor();

    _ptrAppModel->send_vectComputedComponentsInUse_toView(); //ui destination is DialogVLayoutStackProfilEdit

    _mainAppState.setState(MainAppState::eMAS_editingProject_editStackedProfilIsPossible);

    return(true);
}

bool ApplicationController::projectEditFct_menuAction_editStackedProfils() {

    qDebug() << __FUNCTION__ ;

    int boxCount =  _ptrAppModel->ptrCoreComputation_inheritQATableModel()->getBoxCount();

    //no box
    if (!boxCount) {
        qDebug() << __FUNCTION__ << " boxCount is 0";
        return(false);
    }

    qDebug() << __FUNCTION__ << "(ApplicationController) _currentBoxId before check:" << _currentBoxId;

    //prevent invalid case in case of a buggy/missing _currentBoxId re-set (prepareComputedBoxForStackEdition below contains also a check for rejection)
    if (_currentBoxId >= boxCount) {
        _currentBoxId = 0;
    }

    //set on first box if box not set
    if (_currentBoxId == -1) {
        _currentBoxId = 0;
    }

    qDebug() << __FUNCTION__ << "(ApplicationController) _currentBoxId after check:" << _currentBoxId;

    S_MeasureIndex measureIndex {true};

    //doing:
    //- clear_allQvectCccsYMinYMax_PX1_PX2_deltaZ
    //- feedscene
    //- getYranges
    //- adjustYRange_onProfilScene_PX1_PX2_deltaZ if needs (this method extend the Y range and call adjustYRange_and_createTextLandmarkGraphicsItems)
    bool bReport = _ptrAppModel->prepareComputedBoxForStackEdition(_currentBoxId, measureIndex);
    if (!bReport) {        
        qDebug() << __FUNCTION__ << "prepareComputedBoxForStackEdition said false";
        //@#LP error msg for user. Shoudl never happens and the developer has to look what's wrong if it happens
        messageBoxForUser("internal dev error", "prepareComputedBoxForStackEdition failed");
        return(false);
    }

    //avoid box edition window and profil curve displayed at the same time
    //moving or adding boxes lead to re-indexed boxes (and profils)
    _ptrAppView->hide_dialog_editBoxes();

    //@refactoring:
    //previously: called here:
    // - _ptrAppView->setModelsPtr_forStackProfilEdit(and setModelsPtr_forStackProfilEdit called updateOnDemand_onQvectStackprofiledit equivalent)
    // - _ptrAppView->ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots()
    //_ptrAppView->setModelsPtr_forStackProfilEdit(_ptrAppModel->ptrCGScene_profils(),
    //                                             _ptrAppModel->ptrStackProfilEdit_textWidgetPartModel());
    //_ptrAppView->ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots();

    //now: just call _ptrAppView->ptrDialogVLayoutStackProfilEdit()->updateOnDemand_onQvectStackprofiledit()
    //and setModelsPtr_forStackProfilEdit and ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots() called one time at init

    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->updateOnDemand_onQvectStackprofiledit();

    //show will call updateItems
    _ptrAppView->show_dialogVLayoutStackProfilEdit(_currentBoxId, boxCount, true);

    _ptrAppModel->onStackedProfilScenes_alignComponentsForCurrentAlignMode();

    _ptrAppModel->selectBox(_currentBoxId);

    return(true);
}



void ApplicationController::slot_userRequest_currentBoxSizeChange(int vectBoxId, int oddPixelWidth, int oddPixelLength) {

    qDebug() << __FUNCTION__ << "(ApplicationController) received vectBoxId:" << vectBoxId;

    /*if (vectBoxId != _currentBoxId) {
        return;
    }*/
    if (!_ptrAppModel) {
        return;
    }
    bool bDone = _ptrAppModel->setBoxSizeOfCurrentBoxCheckingThatBoxFitInRoute(vectBoxId, oddPixelWidth, oddPixelLength);
    if (bDone) {
        _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);//computation needs to be made before any new profil stack edition or export
    }
}


void ApplicationController::slot_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(int vectBoxId, int shift) {

    qDebug() << __FUNCTION__ << "(ApplicationController) received vectBoxId:" << vectBoxId;

    /*if (vectBoxId != _currentBoxId) {
        return;
    }*/
    if (!_ptrAppModel) {
        return;
    }
    bool bDone = _ptrAppModel->moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(vectBoxId, shift);
    if (bDone) {
        _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);//computation needs to be made before any new profil stack edition or export
    }
}



void ApplicationController::slot_stackProfilEdit_switchToBox(int boxId, bool bShowWindowLayoutStackProfilEditOnTopAndGiveFocusToIt) {

    qDebug() << __FUNCTION__ << "(ApplicationController) boxId received:" << boxId;

    int boxCount =  _ptrAppModel->ptrCoreComputation_inheritQATableModel()->getBoxCount();
    if (!boxCount) {
        qDebug() << __FUNCTION__ << " boxCount is 0";
        return;//(false);
    }

    if (boxId < 0) {
        return;//(false);
    }
    if (boxId >= boxCount) {
        return;//(false);
    }

    _currentBoxId = boxId;  
    qDebug() << __FUNCTION__ << "(ApplicationController) set_currentBoxId to:" << _currentBoxId;

    S_MeasureIndex measureIndex {true};
    bool bReport = _ptrAppModel->prepareComputedBoxForStackEdition(_currentBoxId, measureIndex);

    if (!bReport) {
        qDebug() << __FUNCTION__ << "prepareComputedBoxForStackEdition said false";
        //@#LP error msg for user. Shoudl never happens and the developer has to look what's wrong if it happens
        messageBoxForUser("internal dev error", "prepareComputedBoxForStackEdition failed");
        return;
    }

    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->updateOnDemand_onQvectStackprofiledit(); //@LP add the missing space around curve area when switching box

    _ptrAppView->show_dialogVLayoutStackProfilEdit(_currentBoxId, boxCount, bShowWindowLayoutStackProfilEditOnTopAndGiveFocusToIt);

    _ptrAppModel->onStackedProfilScenes_alignComponentsForCurrentAlignMode();

    _ptrAppModel->selectBox(_currentBoxId); //this will update the _currentBoxId for the model and selected the highlight the box in the graphicscontainer
                                            //and selected box in edit box dialog due to updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect call

}

bool ApplicationController::project_open() {

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected request #0 : _ptrAppModel is nullptr";
        return(false);
    }
    if (!_ptrAppView) {
        qDebug() << __FUNCTION__ << "rejected request #1 : _ptrAppView is nullptr";
        return(false);
    }

    qDebug() << __FUNCTION__ << "_mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();

    bool bOpenProjectIsPossible = false;
    bOpenProjectIsPossible |= (_mainAppState.get_eAppState() == MainAppState::eMAS_idle);
    bOpenProjectIsPossible |= (_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet)
                             &(_mainAppState.get_eAppState() != MainAppState::eMAS_editingRouteSet_editingRoute);
    //the others MainAppState::eMAS_editingRouteSet is about modal windows, no way to reach here when these windows are opened
    if (!bOpenProjectIsPossible ) {
        qDebug() << __FUNCTION__ << "rejected request #100";
        return(false);
    }

    bool bOkForOpenExistingSet = projectEditFct_questionAboutSaveSaveAs_and_saveProject();
    if (!bOkForOpenExistingSet) {
        return(false);
    }

    QString strStartingDir = /*StandardPathLocation::strStartingDefaultDir();*/_appRecall.get_lastUsedDirectory();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strfileName = QFileDialog::getOpenFileName(nullptr, tr("Open an existing project"), strStartingDir, "*.jstackprof");
    if (strfileName.isEmpty()) {
        return(false);
    }
    _appRecall.set_lastUsedDirectoryFromPathFile(strfileName);

    //clear many things in Appmodel for a new function start
    projectEditFct_clearState_backTo_DoNothing();

    //load data from the project, no check about real existence of input files at this step
    //just check that if a non empty file matches with some stacked profiles data if present in json.
    //and that  empty file matches with none stacked profiles data present in json

    _qvectb_layersToCompute_justCheckingStringEmptiness.fill(false,3);
    _qvectb_correlationMapUsage_justCheckingStringEmptiness.fill(false,3);

    _inputFileWithAttributes_fromJsonFile.clear();
    e_mainComputationMode eMainComputationMode { e_mCM_notSet };

    //projectEditionFunction_loadProject:
    //- if input files json attributes (size, height, etc) were present in json, there are loaded and checked completely present
    //  but none check made about sync with files or any thing like that.
    //- a sync check is made between emptiness of correlation score map file and correlation score map parameters (threshold or weighting at true)
    bool bProjectFileLoaded = _ptrAppModel->projectEditionFunction_loadProject(strfileName,
                                                                               _inputFileWithAttributes_fromJsonFile,
                                                                               eMainComputationMode,
                                                                               _qvectb_layersToCompute_justCheckingStringEmptiness,
                                                                               _qvectb_correlationMapUsage_justCheckingStringEmptiness);
    //
    qDebug() << __LINE__ << __FUNCTION__ << "_qvectb_layersToCompute_justCheckingStringEmptiness = " << _qvectb_layersToCompute_justCheckingStringEmptiness;
    qDebug() << __LINE__ << __FUNCTION__ << "_qvectb_correlationMapUsage_justCheckingStringEmptiness = " << _qvectb_correlationMapUsage_justCheckingStringEmptiness;


    if (!bProjectFileLoaded) {
        S_boolAndStrErrorMessage_aboutLoad sbaStrErrorMessage_aboutLoad =  _ptrAppModel->get_sbaStrErrorMessage_aboutLoad();
        QMessageBox messageBox(QMessageBox::Information,
                               "error loading project (.jstackprof file)",
                               sbaStrErrorMessage_aboutLoad._strMessageAboutJson,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        projectEditFct_clearState_backTo_DoNothing();

        return(false);
    }

    _bUseGeoRefForProject = _ptrAppModel->get_useGeoRefForProject();

    sync_UseGeoRefForTrace_ofControllerAndModel_with_UseGeoRefForProject();

    QVector<bool> qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ(3,false);
    QVector<bool> qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone(3,false);

    //the fact that PX1 and PX2 need to be empty together or not empty together was checked in projectEditionFunction_loadProject (error case handled)
    //now check that not empty files exist
    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {

        if (!_qvectb_layersToCompute_justCheckingStringEmptiness[ieLA]) {
            //qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ[ieLA] = false;
            //qvectb_missingFiles_correlationScore_PX1_PX2_DeltaZ[ieLA] = false;
        } else {
            qDebug() << __LINE__ << __FUNCTION__ << "checking !fileExists for inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[" << ieLA << "]: "
                     << _inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA];

            qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ[ieLA] = !fileExists(_inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA]);
        }
    }

    for (int ieLAc = eLA_CorrelScoreForPX1PX2Together; ieLAc <= eLA_CorrelScoreForDeltaZAlone; ieLAc++) {
        //if (_inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela].isEmpty()) {
        if (!_qvectb_correlationMapUsage_justCheckingStringEmptiness[ieLAc]) {
            //qvectb_missingFiles_correlationScore_PX1_PX2_DeltaZ[ieLA] = false;
            //@#LP weird case when json indicated correl score map but empty according input file not handled
        } else {
            qDebug() << __LINE__ << __FUNCTION__ << "checking !fileExists for inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[" << ieLAc << "]: "
                     << _inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[ieLAc];
            qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc] =
                    !fileExists(_inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[ieLAc]);
        }
        if (ieLAc == eLA_CorrelScoreForPX1PX2Together) {
            qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc+1] = qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc];
            ieLAc++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }

    qDebug() << __LINE__ << __FUNCTION__ << "qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ = " << qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ;
    qDebug() << __LINE__ << __FUNCTION__ << "qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone = " << qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone;

    int countMissing = qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ.count(true)
                     + qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone.count(true);

    qDebug() << __LINE__ << __FUNCTION__ << "countMissing = " << countMissing;

    _inputFileWithAttributes_fromJsonFile.showContent();

    if (countMissing) {
        QMessageBox msgBox;
        msgBox.setText("One or more input files used in project are not found.");
        msgBox.setInformativeText("If you moved them, click OK to fix the issue.");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ieDialExecReturnCode = msgBox.exec();
        if (ieDialExecReturnCode == QMessageBox::Cancel) {
            qDebug() << __FUNCTION__ << " canceled by user";

            projectEditFct_clearState_backTo_DoNothing();

            return(false);
        }

        _mainAppState.setState(MainAppState::eMAS_editingProject_openingExistingProject_fixingIO);

        _ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->setEditedProjectState_missingFiles_lockedRoute(
            _inputFileWithAttributes_fromJsonFile._inputFiles,
            _qvectb_layersToCompute_justCheckingStringEmptiness,
            qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ,
            qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone,
            _ptrAppModel->getProjectRouteName());

        _ptrAppView->show_dialog_projectInputFiles_fixingMissingFiles();
        return(true); //@LP true don't have strong meaning here, but false does not match with the context

    }

    return(checkThatAllInputFilesDataAreSyncWithJson_and_open(_inputFileWithAttributes_fromJsonFile._inputFiles, false));
}



void ApplicationController::projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonOKClicked() {

    S_InputFiles inputFiles = _ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->get_inputFiles_imageAndCorrelationScoreMap();

    bool bReport = checkThatAllInputFilesDataAreSyncWithJson_and_open(inputFiles, true);
    if (bReport) {
        _ptrAppModel->setAppFile_project_asModified();
    }
}


bool ApplicationController::checkThatAllInputFilesDataAreSyncWithJson_and_open(const S_InputFiles& inputFiles, bool bIsAboutFilesReplacement) {

    qDebug() << __FUNCTION__ << "_inputFileWithAttributes_fromJsonFile " ;
    _inputFileWithAttributes_fromJsonFile.showContent();

    qDebug() << __FUNCTION__ << "inputFiles = ";
    inputFiles.showContent();

    QString strMgsError;
    bool bCheckReport = projectEditFct_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
                inputFiles,
                _inputFileWithAttributes_fromJsonFile._inputImagesFileAttributes,
                _qvectb_layersToCompute_justCheckingStringEmptiness,
                _qvectb_correlationMapUsage_justCheckingStringEmptiness,
                strMgsError);

    if (!bCheckReport) {
        QString qstrContextMsg;
        if (bIsAboutFilesReplacement) {
            qstrContextMsg = " replacement";
        }
        QMessageBox messageBox(QMessageBox::Information,
                               "issue with input files" + qstrContextMsg,
                               strMgsError,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        return(false);
    }

    QVector<bool> qvectb_layersForComputation(3, false);
    int nbLayersqvectb_layersForComputation = 0;

    QVector<bool> qvectb_correlScoreMapToUseForComp(3, false);
    int nbcorrelScoreMapToUseForComp = 0;

    QString strMsgErrorDetails;

    bCheckReport = _ptrAppModel->//projectEditionFunction_setInputFilesForComputation_and_checkMatchWithRequierement(
        projectEditionFunction_checkInputFilesForComputationMatchWithRequierement(
        inputFiles,
        e_mCM_Typical_PX1PX2Together_DeltazAlone,//set to sole mode for now: e_mCM_Typical_PX1PX2Together_DeltazAlone
        qvectb_layersForComputation, nbLayersqvectb_layersForComputation,
        qvectb_correlScoreMapToUseForComp, nbcorrelScoreMapToUseForComp,
        strMsgErrorDetails);

    if (!bCheckReport) {
        QMessageBox messageBox(QMessageBox::Information,
                               "issue with input files",
                               strMsgErrorDetails,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        return(false);
    }

    //Here the replacement files, if required, were checked to have similar attributes than attributes set in project
    //(set by the app, can be not present if the file was made by hand)
    //@LP no warranty is made about images content (none checksum usage
    //@LP the app accept files with different names than the filename indicated in the json project (?LP: good or bad idea ?)
    //-----------------------------------------------

    if (_bUseGeoRefForProject) {
        //check now that input files have similar TFW
        ImageGeoRef uniqueImageGeoRefGot;
        QString strUniqueEPSGCodeGot;
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem;

        S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem =
                _ptrAppModel->projectEditionFunction_openingFile_compareWorldFileDataAndEPSGCode_fromInputFiles_betweenThem(
                    inputFiles, qvectb_layersForComputation,
                    uniqueImageGeoRefGot, strUniqueEPSGCodeGot,
                    sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem);

        qDebug() << __FUNCTION__ << __LINE__ << ": se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSSgTFW_reducedToBigCases:" <<
                    se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSSgTFW_reducedToBigCases;

        if (se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSSgTFW_reducedToBigCases != e_iSS_geoTFWData_rTBC_available) {

            QMessageBox messageBox(QMessageBox::Information,
                        sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem._strMsgAboutWorldFileData,
                        sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem._strMsgAboutWorldFileData_errorDetails,
                        QMessageBox::Ok, nullptr);

            switch (se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSSgTFW_reducedToBigCases) {
                case e_iSS_geoTFWData_rTBC_available://should never happen here; handled in a dedicated code section below
                    qDebug() << __FUNCTION__ << __LINE__ << ": dev error: case which should never happen";
                    routesetEditFct_clearState_backTo_DoNothing();
                    return(false);

                case e_iSS_geoTFWData_rTBC_notSetValue://should never happen
                case e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData:
                case e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason:
                case e_iSS_geoTFWData_rTBC_notAvailable_noneFound:
                    messageBox.exec();
                    break;
            }
            return(false);
        }

        //set the availability flag for the world file data part
        //(for now this is only important for Ui sync button ok enabled state (when just viewing the geo infos)
        _ptrAppModel->projectEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus();

        //check that input files have similar EPSG Code or at least one found in cache

        //unsync or error
        if (  (se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSS_geoEPSG != e_iSS_geoEPSG_available)
            &&(se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSS_geoEPSG != e_iSS_geoEPSG_notAvailable_noneFound)) {

            QMessageBox messageBox(QMessageBox::Information,
                        sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem._strMsgAboutEPSG,
                        sbaStrMsg_gRIS_projectEdition_cmpWFDataFromInputFilesBetweenThem._strMsgAboutEPSG_errorDetails,
                        QMessageBox::Ok, nullptr);

            switch (se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSS_geoEPSG) {
                case e_iSS_geoEPSG_notSetValue:
                case e_iSS_geoEPSG_available: //specific use case handled below
                case e_iSS_geoEPSG_notAvailable_noneFound: //specific use case handled below
                    qDebug() << __FUNCTION__ << __LINE__ << ": dev error: case which should never happen";
                    projectEditFct_clearState_backTo_DoNothing();
                    return(false);
                case e_iSS_geoEPSG_notAvailable_unsyncEPSGData:
                case e_iSS_geoEPSG_notAvailable_unknowError:
                    break;
            }
            messageBox.exec();
            return(false);
        }

        //we now compare TFW from the json geoRefInfo with the one got from the input files.
        //we compare also the EPSG code from the json with the one got from the input files if available
        bool bCompareEPSGCode = (se_geoRefImagesSetStatus_cmpGeoDataFromInputFilesBetweenThem._eiSS_geoEPSG == e_iSS_geoEPSG_available);

        //check that unique world file and unique EPSG code from inputFiles are similar with the one loaded from project file
        QString strErrorMsg;
        bool bSame = _ptrAppModel->projectEditionFunction_compareWorldFileDataAndEPSGCode_toCurrentImageGeoRefInfos(
                        bCompareEPSGCode, uniqueImageGeoRefGot, strUniqueEPSGCodeGot,
                        strErrorMsg);
        if (!bSame) {
            QMessageBox messageBox(QMessageBox::Information,
                       "Geo data issue",//"World File data issue",
                        strErrorMsg,//"world file data of input files does not match with project",
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            return(false);
        }


        //replaced file can lead to a missing epsg code for this file
        //not replaced file can also lead to a missing epsg code for this file

        QString strEPSGCodeFromCurrent = _ptrAppModel->ptrGeoRefModel_imagesSet()->get_qstrEPSGCode();

        inputFiles.showContent();

        //set files in georef, and try to get EPSG code for them
        bool bReplacedInGeoRefModelImagesSet = _ptrAppModel->projectEditionFunction_inGeoRefModelImagesSet_populateFilenamesAndSyncGeoInfosFromAlreadyInPlace(
                    inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ, qvectb_layersForComputation, bIsAboutFilesReplacement);

        //should never happen:
        if (!bReplacedInGeoRefModelImagesSet) {
            QMessageBox messageBox(QMessageBox::Information,
                        "Internal error",
                        "replaceInGeoRefModelImagesSet_missingFilenamesByEquivalent failed",
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            projectEditFct_clearState_backTo_DoNothing();
            return(false);
        }

        //error from projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus here should never happen:
        S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus_afterUpdateOfGeoRefModelImagesSet =
                _ptrAppModel->projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus();

        qDebug() << __FUNCTION__ << "#2095 se_geoRefImagesSetStatus_afterUpdateOfGeoRefModelImagesSet._eiSSgTFW_allStatusCase = " << se_geoRefImagesSetStatus_afterUpdateOfGeoRefModelImagesSet._eiSSgTFW_allStatusCase;

        if (  (se_geoRefImagesSetStatus_afterUpdateOfGeoRefModelImagesSet._eiSS_geoEPSG == e_iSS_geoEPSG_notAvailable_unsyncEPSGData)
            ||(se_geoRefImagesSetStatus_afterUpdateOfGeoRefModelImagesSet._eiSS_geoEPSG == e_iSS_geoEPSG_notAvailable_unknowError) ) {

            QMessageBox messageBox(QMessageBox::Information,
                       "EPSG Code Internal error",
                        "internal dev error #1234",
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            return(false);
        }
        //re-set the original EPSG code (can be lost using projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus if none EPSG code found for files)
        _ptrAppModel->ptrGeoRefModel_imagesSet()->forceEPSGCode(strEPSGCodeFromCurrent);

        proposeToAssociateTheEPSGCodeForImageWithout(strEPSGCodeFromCurrent);
    }

    _ptrAppView->hide_dialog_projectEdition();

    sync_UseGeoRefForTrace_ofControllerAndModel_with_UseGeoRefForProject();

    bool bOpenReport = projectEditFct_open_inputFilesExist(inputFiles,
                                                           qvectb_layersForComputation,
                                                           qvectb_correlScoreMapToUseForComp,
                                                           bIsAboutFilesReplacement);
    return(bOpenReport);
}


void ApplicationController::sync_UseGeoRefForTrace_ofControllerAndModel_with_UseGeoRefForProject() {
    //set _bUseGeoRefForTrace to false for controller
    _bUseGeoRefForTrace = _bUseGeoRefForProject;
    //and model to have the correct flag for route export
    _ptrAppModel->set_useGeoRefForTrace(_bUseGeoRefForProject);
}

/*
//valid for routeset and project:
void ApplicationModel::projectEditionFunction_sendStr_geoRefImagesSetStatus() {

    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    _sbaStrMsg_gRIS_projectEdition.showContent();

    emit signal_sendStr_geoRefImagesSetStatus_aboutRoutesetEdition(
        _sbaStrMsg_gRIS_projectEdition._bWorldFileData_available,
        _sbaStrMsg_gRIS_projectEdition._bEPSG_available,

        _sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData,
        _sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails,

        _sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG,
        _sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails);
}
*/

bool ApplicationController::projectEditFct_open_inputFilesExist(const S_InputFiles& inputFiles,
                                                                const QVector<bool>& qvectb_layersToUseForCompute,
                                                                const QVector<bool>& qvectb_correlMapFilenameAvailable,
                                                                bool bIsAboutFilesReplacement) {

    if (_bUseGeoRefForProject) {
        //feed the dialogGeoRefInfos before exec on it:
        _ptrAppModel->sendStr_worldFileDataToView();
        _ptrAppModel->sendStr_imagesFilename();
        _ptrAppModel->sendEPSGCodeToView();
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
        _ptrAppModel->projectEditionFunction_sendStr_geoRefImagesSetStatus();
    }

    _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersFilenameAndSetVisibility(
            inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ, qvectb_layersToUseForCompute);

    double xScale_fromGeoRefModel = .0;
    bool bGot_xScale = _ptrAppModel->ptrGeoRefModel_imagesSet()->get_xScale(xScale_fromGeoRefModel);

    double yScale_fromGeoRefModel = .0;
    bool bGot_yScale = _ptrAppModel->ptrGeoRefModel_imagesSet()->get_yScale(yScale_fromGeoRefModel);

    //should never happen:
    if (!bGot_xScale || !bGot_yScale) {
        QMessageBox messageBox(QMessageBox::Information,
                "Dev error #200",
                "failed to get xScale or yScale from GeoRefModel_imagesSet",
                QMessageBox::Ok, nullptr);
        messageBox.exec();

        //back to the project edition dialog:
        _ptrAppView->show_dialog_projectInputFiles(false);//stay on previous feed field
        return(false);
    }

    QString str_xScale = doubleToQStringPrecision_f_amountOfDecimal(qAbs(xScale_fromGeoRefModel), 3);
    QString str_YScale = doubleToQStringPrecision_f_amountOfDecimal(qAbs(yScale_fromGeoRefModel), 3);

    qDebug() << "xScale_fromGeoRefModel = " << xScale_fromGeoRefModel;
    qDebug() << "yScale_fromGeoRefModel = " << yScale_fromGeoRefModel;

    qDebug() << "str_xScale = " << str_xScale;
    qDebug() << "str_YScale = " << str_YScale;

    _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersmicMacStepValueAndSpatialResolution(
        {"1.0", "1.0", "1.0"},
        {str_xScale, str_YScale, "1.0"},
        qvectb_layersToUseForCompute);

    int idxLayerToDisplayAsBackgroundImage = -1;
    bool bCanceledByTheUser = false;
    bool bOneLayerAvailableSelected = selectLayerToDisplayAsBackgroundImage(qvectb_layersToUseForCompute, idxLayerToDisplayAsBackgroundImage, bCanceledByTheUser);

    if (!bOneLayerAvailableSelected) {
        if (bCanceledByTheUser) {
            if (bIsAboutFilesReplacement) {
                _mainAppState.setState(MainAppState::eMAS_editingProject_openingExistingProject_fixingIO);

                QVector<bool> qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ(3,false);
                QVector<bool> qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone(3,false);

                bool bReport = computeFlagsForMissingInputFiles(
                    inputFiles,
                    _qvectb_layersToCompute_justCheckingStringEmptiness,
                    _qvectb_correlationMapUsage_justCheckingStringEmptiness,
                    qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ,
                    qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone);

                if (!bReport) {
                    S_boolAndStrErrorMessage_aboutLoad sbaStrErrorMessage_aboutLoad =  _ptrAppModel->get_sbaStrErrorMessage_aboutLoad();
                    QMessageBox messageBox(QMessageBox::Information,
                                           "error loading project",
                                           "internal dev error #1235",
                                           QMessageBox::Ok, nullptr);
                    messageBox.exec();
                    projectEditFct_clearState_backTo_DoNothing();

                    return(false);
                }

                _ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles()->setEditedProjectState_missingFiles_lockedRoute(
                    inputFiles,
                    _qvectb_layersToCompute_justCheckingStringEmptiness,
                    qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ,
                    qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone,
                    _ptrAppModel->getProjectRouteName());

                _ptrAppView->show_dialog_projectInputFiles_fixingMissingFiles();
                return(false); //@LP true don't have strong meaning here, but false does not match with the context
            }
        }
        projectEditFct_clearState_backTo_DoNothing();
        return(false);
    }

    //
    _ptrAppModel->setPtrVectDequantization_forGeoUiInfosModel(true);

    _ptrAppView->setAdditionnalStringToTitleOfMainWindowMenu(_ptrAppModel->getAppFile_project().getFilename());

    e_LayersAcces eLA = static_cast<e_LayersAcces>(idxLayerToDisplayAsBackgroundImage);

    if (_bUseGeoRefForProject) {
        bool bReport = _ptrAppModel->init_geoProj_onlyOneImageDisplayed(eLA);
        if (!bReport) {
            QMessageBox messageBox(QMessageBox::Information,
                        "geoProj error", //@#LP add dev error number ?
                        "failed in init_geoProj", //@#LP add more details ?
                        QMessageBox::Ok, nullptr);
            messageBox.exec();
            projectEditFct_clearState_backTo_DoNothing();
            return(false);
        }
        _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);

    } else {
       bool bReport = _ptrAppModel->init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(eLA);
       if (!bReport) {
           QMessageBox messageBox(QMessageBox::Information,
                       "geoUiInfos error", //@#LP add dev error number ?
                       "failed in init_geoUiInfos_noGeoRefUsed...()", //@#LP add more details ?
                       QMessageBox::Ok, nullptr);
           messageBox.exec();
           projectEditFct_clearState_backTo_DoNothing();
           return(false);
       }
       _ptrAppView->ptrWindowImageView()->setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY);
    }

    _mainAppState.permitsMenuActionAboutGeoinfoData(_bUseGeoRefForProject);


    bool bBackgroundImagesLoaded = _ptrAppModel->
            projectEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ,
                qvectb_layersToUseForCompute, idxLayerToDisplayAsBackgroundImage);

    if (!bBackgroundImagesLoaded) {

        S_boolAndStrErrorMessage_aboutLoad SbaStrErrorMessage_aboutLoad;
        SbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sboolAndStrMsgAbout_aboutLoad();

       QMessageBox messageBox(QMessageBox::Information,
                   "Image load error",
                   SbaStrErrorMessage_aboutLoad._strMessagAboutImage,
                   QMessageBox::Ok, nullptr);
       messageBox.exec();

       projectEditFct_clearState_backTo_DoNothing();
       return(false);
    }

    //set the inputfiles for the computation core

    _ptrAppModel->setInputFilesAndCorrelationScoreFiles(inputFiles, qvectb_correlMapFilenameAvailable);

    _eRouteEditionStateOfRouteOfProject = e_rES_locked;
    _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);
    _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(_eRouteEditionStateOfRouteOfProject != e_rES_inEdition);

    _ptrAppView->hide_dialog_projectEdition();
    _ptrAppModel->projectEditionFunction_initState_andFeedGraphicsItemContainer(false); //false for 'loaded project' (= 'not a new project')

    _bLayerSwitchIsPermitted = false;
    _ptrAppView->ptrWindowImageView()->setLayersNamesForLayersSwitcher(_qvectStrLayersName);
    _ptrAppView->ptrWindowImageView()->setAvailableLayersForLayersSwitcher(qvectb_layersToUseForCompute, idxLayerToDisplayAsBackgroundImage);
    _bLayerSwitchIsPermitted = true;

    _ptrAppView->showImageViewWindows(true, true);

    if (_ptrAppModel->getConstRef_projectContentState().getState_boxes() == e_Project_boxesContentState::e_P_boxesCS_noneBox) {
        _mainAppState.setState(MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes);
    } else {
        e_Project_stackedProfilEditedState eP_stackedProfilEditedState =
                _ptrAppModel->getConstRef_projectContentState().getState_stackedProfilEdited();

        if (  (eP_stackedProfilEditedState == e_Project_stackedProfilEditedState::
                                              //e_P_stackedProfilEditedS_noneEdited_allSetAsDefaultValues
                                              e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues)
            ||(eP_stackedProfilEditedState == e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession)) {



            //allow computation settings and run
            _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);

        } else { //Project_contentState::e_P_stackedProfilEditedS_notApplicable
            _mainAppState.setState(MainAppState::eMAS_editingProject_idle);
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();

    _ptrAppView->ptrDialog_project_inputFiles_viewContentOnly()->setEditedProject(
                inputFiles,
                qvectb_layersToUseForCompute,
                _ptrAppModel->getProjectRouteName());

    return(true);
}


bool ApplicationController::projectEditFct_aboutOpenProjetFixingIssue_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
        const S_InputFiles& inputFiles,
        const S_InputImagesFileAttributes& inputFileWithAttributesFromJsonFile,
        const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
        const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
        QString& strMsgError) {

    return(projectEditFct_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
        inputFiles,
        inputFileWithAttributesFromJsonFile,
        qvectb_layersToCompute_justCheckingStringEmptiness,
        qvectb_correlationMapUsage_justCheckingStringEmptiness,
        strMsgError));
}

bool ApplicationController::projectEditFct_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
        const S_InputFiles& inputFiles,
        const S_InputImagesFileAttributes& inputFileWithAttributesFromJsonFile,
        const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
        const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
        QString& strMsgError) {

    if (qvectb_layersToCompute_justCheckingStringEmptiness.size() != 3 ) {
        strMsgError = "#dev error 110";
        return(false);
    }

    if (!qvectb_layersToCompute_justCheckingStringEmptiness.count(true)) {
        strMsgError = "#dev error 111";
        return(false);
    }

    S_InputImagesFileAttributes inputFileWithAttributesFromFile;

    QString tqstrLayerJSonDescName[3] {"PX1", "PX2", "ZOther"};
    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        if (qvectb_layersToCompute_justCheckingStringEmptiness[ieLA]) {
            if (!inputFileWithAttributesFromJsonFile._qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ[ieLA]._bAttributesFeed) {
                continue;
            }
            S_ImageFileAttributes sImgFileAttributes;
            bool bGot = sImgFileAttributes.fromFile(inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA]);
            if (!bGot) {
                strMsgError = "fail to get image and file attributes from " + tqstrLayerJSonDescName[ieLA];
                return(false);
            }
            bool bSame = sImgFileAttributes.compareTo(inputFileWithAttributesFromJsonFile._qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ[ieLA]);
            if (!bSame) {
                strMsgError = "some image attributes from " + tqstrLayerJSonDescName[ieLA] + " don't match with opened project";
                return(false);
            }
        }
    }

    //@#LP will need maincomputationmode usage to know how to check differently for mode different than 'PX1PX2 together'
    QString _tqstrDescName_correlScoreMap[3] {"(PX1,PX2)", "(PX1,PX2)", "DeltaZ or Other"};
    for (int ieLA = eLA_CorrelScoreForPX1PX2Together; ieLA <= eLA_CorrelScoreForDeltaZAlone; ieLA++) {
        if (qvectb_correlationMapUsage_justCheckingStringEmptiness[ieLA]) {
            if (!inputFileWithAttributesFromJsonFile._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[ieLA]._bAttributesFeed) {
                continue;
            }
            S_ImageFileAttributes sImgFileAttributes;
            bool bGot = sImgFileAttributes.fromFile(inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[ieLA]);
            if (!bGot) {
                strMsgError = "fail to get image and file attributes from correlation score map " + _tqstrDescName_correlScoreMap[ieLA];
                return(false);
            }
            bool bSame = sImgFileAttributes.compareTo(inputFileWithAttributesFromJsonFile._qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ[ieLA]);
            if (!bSame) {
                strMsgError = "some image attributes from correlation score map " + _tqstrDescName_correlScoreMap[ieLA] + " don't match with opened project";
                return(false);
            }
        }
        if (ieLA == eLA_CorrelScoreForPX1PX2Together) {
            ieLA++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }

    return(true);
}


void ApplicationController::proposeToAssociateTheEPSGCodeForImageWithout(const QString& strEPSGCodeForMessageBox) {

    QVector<S_DetailsAboutEPSGCodeFoundFromCacheForOneImage> qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage;
    e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;
    _ptrAppModel->ptrGeoRefModel_imagesSet()->get_detailsAboutEPSGCodeFoundFromCacheForTheImageSet(
      qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage, eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet);

    if(  (eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet == e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundButNoneReadError)
       ||(eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet == e_DetailsAboutEPSGCFC_fTIS_noneFound_noneReadError)) {
        //the moved images can be because we moved the project from one computer to an another and hence do not appears in the cache about the epsg code
        //Then we propose to set the EPSG code for these images

        QString strQuestion =
                "Do you want to associate the EPSG Code from the projet ("
                + strEPSGCodeForMessageBox + ") to input image(s) ?";

        QMessageBox messageBox(QMessageBox::Question,
                    "Some selected image(s) don't have EPSG Code stored in .jepsg",
                    strQuestion,
                    QMessageBox::No|QMessageBox::Yes, nullptr);

        int ieDialExecReturnCode = messageBox.exec();
        if (ieDialExecReturnCode == QMessageBox::No) {
            //do nothing

        } else { //Yes
            bool bReport = _ptrAppModel->projectEditionFunction_storeInCacheTheCurrentEPSGCode_forImageWhichDoNotHave();

            QString strMessage = "Association of the code " + strEPSGCodeForMessageBox + " for image(s) ";
            if (bReport) {
                strMessage +="done";
            } else {
                strMessage +="failed";
            }
            QMessageBox messageBox(QMessageBox::Information,
                    "EPSG Code storage",
                    strMessage,
                    QMessageBox::Ok, nullptr);
            messageBox.exec();
        }
    }
}

bool ApplicationController::computeFlagsForMissingInputFiles(
            const S_InputFiles& inputFiles,
            const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
            const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
            QVector<bool>& qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ,
            QVector<bool>& qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone) {

    if ( (  qvectb_layersToCompute_justCheckingStringEmptiness.size()
          + qvectb_correlationMapUsage_justCheckingStringEmptiness.size()
          + qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ.size()
          + qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone.size()) != 4*3) {
        return(false);
    }


   //the fact that PX1 and PX2 need to be empty together or not empty together was checked in projectEditionFunction_loadProject (error case handled)
   //now check that not empty files exist
   for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
       //if (_inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA].isEmpty()) {
       if (!qvectb_layersToCompute_justCheckingStringEmptiness[ieLA]) {
           //qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ[ieLA] = false;
           //qvectb_missingFiles_correlationScore_PX1_PX2_DeltaZ[ieLA] = false;
       } else {
           qDebug() << __LINE__ << __FUNCTION__ << "checking !fileExists for inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[" << ieLA << "]: "
                    << inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA];

           qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ[ieLA] = !fileExists(inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA]);
       }
   }

   for (int ieLAc = eLA_CorrelScoreForPX1PX2Together; ieLAc <= eLA_CorrelScoreForDeltaZAlone; ieLAc++) {
       //if (_inputFileWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela].isEmpty()) {
       if (!qvectb_correlationMapUsage_justCheckingStringEmptiness[ieLAc]) {
           //qvectb_missingFiles_correlationScore_PX1_PX2_DeltaZ[ieLA] = false;
           //@#LP weird case when json indicated correl score map but empty according input file not handled
       } else {
           qDebug() << __LINE__ << __FUNCTION__ << "checking !fileExists for inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[" << ieLAc << "]: "
                    << inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[ieLAc];
           qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc] =
                   !fileExists(inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[ieLAc]);
       }
       if (ieLAc == eLA_CorrelScoreForPX1PX2Together) {
           qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc+1] = qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[ieLAc];
           ieLAc++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
       }
   }
   return(true);
}
