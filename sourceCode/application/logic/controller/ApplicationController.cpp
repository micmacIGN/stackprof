#include <QObject>
#include <QDebug>
#include <QMessageBox>

#include <QCoreApplication>

#include "ApplicationController.h"

#include "model/ApplicationModel.h"
#include "ui/ApplicationView.h"

#include "model/mainAppState.hpp"

#include "AppState_enum.hpp"

#include "model/MenuActionStates.hpp"

#include "ui/dialogIOFiles/dialog_savefile.h"

#include "ui/dialogIOFiles/dialog_inputfiles_images.h"

#include "ui/georefinfos/dialog_geoRefInfos.h"

#include "OsPlatform/standardPathLocation.hpp"
#include <QFileDialog>

#include "ui/dialogIOFiles/dialog_replaceImageFile.h"

#include "ui/project/dialog_project_inputfiles.h"
#include "ui/project/dialog_selectroutefromrouteset.h"
#include "ui/stackedProfilBox/dialog_addboxes_automaticdistribution_closeAsPossible.h"
#include "ui/stackedProfilBox/dialog_addboxes_automaticdistribution_distanceBetweenCenter.h"

#include "ui/computationParameters/dialog_computationparameters.h"

#include "ui/dialogProfilsCurves/dialogVLayoutStackProfilEdit.h"
#include "ui/stackedProfilBox/dialog_stackedProfilBoxes.h"

#include "ui/MainWindowMenu.h"
#include "ui/windowImageView/WindowImageView.h"
#include "model/imageScene/customgraphicsscene_usingTPDO.h"
#include "model/smallImageNavScene/smallimagenav_customgraphicsscene.h"

#include "ui/appSettings/dialog_appsettings.h"
#include "ui/appSettings/dialog_graphicsSettings.h"

#include "dialogProfilOrientation/dialog_setprofilorientation.h"

#include "model/core/exportResult.h"

#include "dialog_project_inputfiles_fixingMissingFiles.h"

#include <QGuiApplication>

#include "dialog_micmacstepvalueandspatialresolution.h"

#include "../toolbox/toolbox_math.h"
#include "../toolbox/toolbox_conversion.h"

#include "georefinfos/dialog_geoRefInfos.h"

#include "dialogProfilsCurves/dialog_profilcurvesmeanings.h"

#include "project/dialog_exportresult.h"

#include "ui/smallImageNavigation/windowsmallimagenav.h"

#include "ui/geoCRSSelect/dialog_geoCRSSelect_tableview.h"

#include "ui/traceEdition/dialog_traces_edit.h"

ApplicationController::ApplicationController(ApplicationModel *ptrAppModel, ApplicationView *ptrAppView, QObject *parent):
    QObject(parent),
    _ptrAppModel(ptrAppModel),
    _ptrAppView(ptrAppView) {

    clearVarStates();
    _mainAppState.setState(MainAppState::eMAS_idle);

    connect(&_mainAppState, &MainAppState::signal_actionFromUser,
                      this, &ApplicationController::slot_actionFromUser_byMenuAction);

    _qvectStrLayersName = {"Px1", "Px2", "deltaZ"};

    _bRejectAnyMainMenuAction_dueToLongTaskInProgress = false;

    _bLayerSwitchIsPermitted = false;

}

bool ApplicationController::tryToLoadAppRecall() {
    _appRecall.clearLoaded();
    return(_appRecall.load_setMissingToDefault());
}

bool ApplicationController::tryToLoadGraphicAppSettings() {
    if (!_ptrAppModel) {
        return(false);
    }
    return(_ptrAppModel->tryToLoadGraphicAppSettings());
}

bool ApplicationController::tryToLoadAppSettings() {
    _appSettings.clearLoaded();
    return(_appSettings.load_setMissingToDefault());
}


bool ApplicationController::start() {
    if (!_ptrAppModel) {
        return(false);
    }

    bool bLoadReportAbout_EPSGDataListFromProjDB = false;
    bool bModelInitSuccess = _ptrAppModel->init(_appSettings.get_imagePyramid_OIIOCacheSizeMB(), bLoadReportAbout_EPSGDataListFromProjDB);

    if (bModelInitSuccess) {
        if (_ptrAppView) {

            connect(_ptrAppView->ptrMainWindowMenu(), &MainWindowMenu::signal_mainWindow_closeEvent,
                    this, &ApplicationController::slot_mainWindow_closeEvent);

            _ptrAppView->ptrMainWindowMenu()->set_menuActionAboutProjectEdition_editingRouteAsEditing(_eRouteEditionStateOfRouteOfProject);
            _ptrAppView->ptrDialog_stackedProfilBoxes()->setAddingBoxModePossible(_eRouteEditionStateOfRouteOfProject != e_rES_inEdition);

            _ptrAppView->setModelsPtr_forImageView(
                        _ptrAppModel->ptrCustomGraphicsScene(),
                        _ptrAppModel->ptrZoomHandler(),
                        _ptrAppModel->ptrGeoUiInfos());

            _ptrAppView->setModelPtr_forSmallImageNav(_ptrAppModel->ptrSmallImageNav_CustomGraphicsScene());           

            _ptrAppView->setModelPtrForMainMenuActionStates(_mainAppState.ptrMenuActionStates());
            _ptrAppView->setModelPtrForMainMenuStates(_mainAppState.ptrMenuStates());
            _ptrAppView->setModelPtrForMainAppState(&_mainAppState);

            _ptrAppView->setModelPtrForDialog_traces_edit(_ptrAppModel->ptrRouteContainer());            
            _ptrAppView->setModelPtrForDialog_routeFromRouteset(_ptrAppModel->ptrRouteContainer());

            _ptrAppView->setModelPtrForStackedProfilBox_tableView(_ptrAppModel->ptrCoreComputation_inheritQATableModel());
            _ptrAppView->setModelPtrForStackedProfilBox_stackedProfilBoxesEdition(_ptrAppModel->ptrCoreComputation_inheritQATableModel());

            _ptrAppView->setModelPtrForDialog_geoCRSSelect_tableView(_ptrAppModel->ptrGeoCRSSelect_inheritQATableModel());

            _ptrAppView->initConnectionModelViewSignalsSlots(); //doest not includes ptrDialogVLayoutStackProfilEdit

            clearVarStates();
            _mainAppState.setState(MainAppState::eMAS_idle);

            _ptrAppView->setControllerPtr(this);
            _ptrAppView->initConnectionViewControllerSignalsSlots();

            //routeset edition
            connect(_ptrAppModel->ptrGeoRefModel_imagesSet(), &GeoRefModel_imagesSet::signal_strWorldFileData,
                    _ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::slot_setWorldFileDataFromStr);
            connect(_ptrAppModel->ptrGeoRefModel_imagesSet(), &GeoRefModel_imagesSet::signal_strEPSGCode,
                    _ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::slot_setEPSGCodeFromStr);
            connect(_ptrAppModel->ptrGeoRefModel_imagesSet(), &GeoRefModel_imagesSet::signal_imagesFilename,
                    _ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::slot_setImagesFilename);

            connect(_ptrAppModel,&ApplicationModel::signal_sendStr_geoRefImagesSetStatus_aboutRoutesetEdition,
                    _ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::slot_setStrGeoRefImagesSetStatus);

            connect(_ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::signal_setEPSGCode,
                    this,&ApplicationController::slot_setEPSGCode);

            //project edition
            //routeset file selected: send from Dialog_project_inputFiles to model through controller
            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_project_routeSelectionRequestForImport,
                    this, &ApplicationController::slot_projectEditFct_newProject_routeSelectionRequestForImport);

            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_project_addADefaultRouteStateChanged,
                    this, &ApplicationController::slot_projectEditFct_newProject_addADefaultRouteStateChanged);

            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_project_useGeoreferencing_stateChanged,
                    this, &ApplicationController::slot_projectEditFct_newProject_useGeoreferencing_stateChanged);


            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_selected_filename_routeset_forRouteImport_hasChanged,
                    this, &ApplicationController::slot_projectEditFct_newProject_selected_filename_routeset_forRouteImport_hasChanged);



            connect(_ptrAppModel,&ApplicationModel::signal_projectEditionFunction_sendIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport_toView,
                    _ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::slot_setIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport);

            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_pushButtonOK_clicked, this, &ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonOK_clicked);
            connect(_ptrAppView->ptrDialogProjectInputFile(), &Dialog_project_inputFiles::signal_canceled,             this, &ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonCancel_clicked);

            connect(_ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles(), &Dialog_project_inputFiles_fixingMissingFiles::signal_pushButtonOK_clicked, this, &ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonOK_clicked);
            connect(_ptrAppView->ptrDialogProjectInputFile_fixingMissingFiles(), &Dialog_project_inputFiles_fixingMissingFiles::signal_canceled,             this, &ApplicationController::slot_projectEditFct_dialogProjectEdit_pushButtonCancel_clicked);


            connect(_ptrAppView->ptrDialog_addBoxes_automaticDistribution_closeAsPossible(), &Dialog_addBoxes_automaticDistribution_closeAsPossible::signal_createBoxes_closeAsPossible,
                     this, &ApplicationController::slot_projectEditFct_dialog_addBoxesAutomaticDistribution_closeAsPossible_createBoxes_clicked);

            connect(_ptrAppView->ptrDialog_addBoxes_automaticDistribution_distanceBetweenCenter(), &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::signal_createBoxes_withDistanceBetweenCenter,
                     this, &ApplicationController::slot_projectEditFct_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter_createBoxes_clicked);

            connect(_ptrAppModel->ptrCoreComputation_inheritQATableModel(), &ComputationCore_inheritQATableModel::signal_computationParameters,
                    _ptrAppView->ptrDialog_computationparameters(), &Dialog_computationParameters::slot_setComputationParameters);


            connect(_ptrAppModel->ptrCoreComputation_inheritQATableModel(),  &ComputationCore_inheritQATableModel::signal_vectComputedComponents,
                    _ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::slot_setComponentsPossiblyVisible);

            connect(_ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::signal_switchToBox,
                    this, &ApplicationController::slot_stackProfilEdit_switchToBox);

            connect(_ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::signal_componentsToShowChanged,
                    this, &ApplicationController::slot_componentsToShowChanged);

            connect(_ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::signal_syncYRange_stateChanged,
                     this, &ApplicationController::slot_stackProfilEdit_syncYRange_stateChanged);

            connect(_ptrAppModel, &ApplicationModel::signal_setComponentsVisible_fromQvectBoolComponents,
                    _ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::slot_setComponentsVisible_fromQvectBoolComponents);

            connect(_ptrAppView->ptrDialogVLayoutStackProfilEdit(), &DialogVLayoutStackProfilEdit::signal_profilAdjustMode_changed,
                     _ptrAppModel, &ApplicationModel::slot_profilAdjustMode_changed);


            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_userRequest_onCurrentBox_sizeChange,
                    this, &ApplicationController::slot_userRequest_currentBoxSizeChange);

            connect(_ptrAppModel, &ApplicationModel::signal_sendBoxId_and_sizeToView,
                    _ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::slot_setBoxId_and_boxSize);

            connect(_ptrAppModel, &ApplicationModel::signal_messageBoxForUser,
                    this, &ApplicationController::slot_messageBoxForUser);

            connect(_ptrAppModel, &ApplicationModel::signal_sendBoxCenterPossibleShiftsToView,
                    _ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::slot_boxCenterPossibleShiftsToView);


            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation,
                    this, &ApplicationController::slot_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation);


            //the signal provides infos about the box that the user want to add
            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_addingBoxModeActivated,
                    _ptrAppModel, &ApplicationModel::slot_addingBoxModeActivated);


            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_enteringAddingBoxMode,
                    this, &ApplicationController::slot_enteringAddingBoxMode);

            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_addingBoxModeCanceled,
                    this, &ApplicationController::slot_addingBoxModeCanceled);


            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_removeBoxFromVectId,
                    this, &ApplicationController::slot_removeBoxFromVectId);

            connect(_ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::signal_removeAllBoxes,
                    this, &ApplicationController::slot_removeAllBoxes);


            connect(_ptrAppModel, &ApplicationModel::signal_oneOrMoreBoxesExist,
                    _ptrAppView->ptrDialog_stackedProfilBoxes(), &Dialog_stackedProfilBoxes::slot_oneOrMoreBoxesExist);

            connect(_ptrAppModel, &ApplicationModel::signal_tryAddBox,
                    this, &ApplicationController::slot_tryAddBox);

            connect(_ptrAppView->ptrWindowImageView(), &WindowImageView::signal_layersSwitcher_pushButtonClicked,
                    this, &ApplicationController::slot_layersSwitcher_pushButtonClicked);

            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_currentZLIChanged,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_forUserInfo_currentZLIChanged);

            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_enablePossibleActionsOnImageView,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_enablePossibleActionsOnImageView);

            connect(_ptrAppView->ptrDialog_AppSettings(), &Dialog_AppSettings::accepted,
                    this, &ApplicationController::slot_Dialog_AppSettings_accepted);

            connect(_ptrAppView->ptrDialog_AppSettings(), &Dialog_AppSettings::rejected,
                    this, &ApplicationController::slot_Dialog_AppSettings_rejected);

            connect(_ptrAppView->ptrDialog_graphicsAppSettings(), &Dialog_graphicsSettings::signal_graphicsSettings_profil_changed,
                    this, &ApplicationController::slot_graphicsSettings_profil_changed);

            connect(_ptrAppView->ptrDialog_graphicsAppSettings(), &Dialog_graphicsSettings::signal_graphicsSettings_profil_preset_changed,
                    this, &ApplicationController::slot_graphicsSettings_profil_preset_changed);


            /*connect(_ptrAppView->ptrDialog_graphicsAppSettings(), &Dialog_graphicsSettings::signal_graphicsSettings_itemOnImgView_changed,
                    this, &ApplicationController::slot_graphicsSettings_itemOnImgView_changed);*/


            connect(_ptrAppModel, &ApplicationModel::signal_vectBoxIdChanged_fromAppModel,
                     this, &ApplicationController::slot_vectBoxIdChanged_fromAppModel);


            connect(_ptrAppModel, &ApplicationModel::signal_openDialog_taskInProgress_forLongComputation,
                    this, &ApplicationController::slot_openDialog_taskInProgress_forLongComputation); //, Qt::DirectConnection);

            connect(_ptrAppModel, &ApplicationModel::signal_closeDialog_taskInProgress_forLongComputation,
                    this, &ApplicationController::slot_closeDialog_taskInProgress_forLongComputation);

            connect(_ptrAppModel, &ApplicationModel::signal_stepProgressionFromComputationCore, //the box in computation in the computaton core is transmitted through this signal
                     this, &ApplicationController::slot_stepProgressionForProgressBar); //, Qt::DirectConnection);




            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_setEnable_setGlobalZoomFactorTo1,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_setEnable_setGlobalZoomFactorTo1);

            connect(_ptrAppView->ptrWindowImageView(), &WindowImageView::signal_setGlobalZoomFactorTo1,
                    this, &ApplicationController::slot_setGlobalZoomFactorTo1);

            connect(_ptrAppModel, &ApplicationModel::signal_switchToBox_fromClickOnGraphicBoxItem,
                    this, &ApplicationController::slot_switchToBox_fromClickOnGraphicBoxItem);

            connect(_ptrAppView->ptrDialog_exportResult(),&Dialog_exportResult::signal_project_exportResult,
                    this, &ApplicationController::slot_project_exportResult);

            //StackProfilEdit:
            _ptrAppView->setModelsPtr_forStackProfilEdit(_ptrAppModel->ptrCGScene_profils(),
                                                         _ptrAppModel->ptrStackProfilEdit_textWidgetPartModel());
            _ptrAppView->ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots();


            connect(_ptrAppModel, &ApplicationModel::signal_stepProgressionForProgressBar,
                    this, &ApplicationController::slot_stepProgressionForProgressBar/*,Qt::DirectConnection*/);
            //connect(_ptrAppModel, &ApplicationModel::signal_stepProgressionForProgressBar_terminated,
            //        this, &ApplicationController::slot_stepProgressionForProgressBar_terminated);


            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_callSceneToFitImageInWindowView,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_callSceneToFitImageInWindowView);

            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_switchShowGridState,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_switchShowGridState);

            //
            connect(_ptrAppView->ptrWindowSmallImageNav()->getCustomGraphicsScenePtr(), &SmallImageNav_CustomGraphicsScene::signal_setSceneCenterNoZLI,
                    _ptrAppView->ptrWindowImageView()->getCustomGraphicsScenePtr(), &CustomGraphicsScene::slot_setSceneCenterNoZLI_fromOutside);

            connect(_ptrAppView->ptrWindowSmallImageNav()->getCustomGraphicsScenePtr(), &SmallImageNav_CustomGraphicsScene::signal_setGlobalZoomFactorTo1,
                    _ptrAppView->ptrWindowImageView()->getCustomGraphicsScenePtr(), &CustomGraphicsScene::slot_requestFromOutside_setGlobalZoomFactorTo1);

            connect(_ptrAppView->ptrWindowSmallImageNav()->getCustomGraphicsScenePtr(), &SmallImageNav_CustomGraphicsScene::signal_fitImageInCurrentWindowSize,
                    _ptrAppView->ptrWindowImageView()->getCustomGraphicsScenePtr(), &CustomGraphicsScene::slot_requestFromOutside_fitImageInCurrentWindowSize);

            /*connect(_ptrAppView->ptrWindowSmallImageNav()->getCustomGraphicsScenePtr(), &SmallImageNav_CustomGraphicsScene::signal_setZoomStepIncDec,
                    _ptrAppView->ptrWindowImageView()->getCustomGraphicsScenePtr(), &CustomGraphicsScene::slot_requestFromOutside_setZoomStepIncDec);*/

            connect(_ptrAppView->ptrWindowSmallImageNav()->getCustomGraphicsScenePtr(), &SmallImageNav_CustomGraphicsScene::signal_setZoomStepIncDec,
                    this, &ApplicationController::slot_zoomStepIncDec);

            connect(_ptrAppView->ptrWindowSmallImageNav(), &WindowSmallImageNav::signal_zoomRequest_fromSmallImaveNav,
                    _ptrAppView->ptrWindowImageView()->getCustomGraphicsScenePtr(), &CustomGraphicsScene::slot_zoomRequest);


            connect(_ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::signal_request_EPSGCodeSelectionFromlist,
                    this, &ApplicationController::slot_request_EPSGCodeSelectionFromlist);

            connect(_ptrAppView->ptrDialog_geoCRSSelect_tableView(), &Dialog_geoCRSSelect_tableView::signal_EPSGCodeSelectedFromList,
                    this, &ApplicationController::slot_EPSGCodeSelectedFromList);

            //@#LP move this signal in a dedicated model
            connect(_ptrAppModel, &ApplicationModel::signal_sendAssociatedNameToEPSGCode,
                    _ptrAppView->ptrDialogGeoRefInfos(), &Dialog_geoRefInfos::slot_associatedNameToEPSGCode);



            //action from ui button to model:
            connect(_ptrAppView->ptrWindowImageView(), &WindowImageView::signal_zoomStepIncDec,
                    this, &ApplicationController::slot_zoomStepIncDec);

            //enabling/disabling ui button according to zoom limits reached:
            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_enable_zoomStepInc,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_enable_zoomStepInc);

            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_enable_zoomStepDec,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_enable_zoomStepDec);



            connect(_ptrAppModel->ptrCustomGraphicsScene(), &CustomGraphicsScene::signal_setFocusWidgetOnZoomStepIncOrDec,
                    _ptrAppView->ptrWindowImageView(), &WindowImageView::slot_setFocusWidgetOnZoomStepIncOrDec);



            connect(_ptrAppModel, &ApplicationModel::signal_routeCountAfterFeed,
                    _ptrAppView->ptrDialog_traces_edit(), &Dialog_traces_edit::slot_routeCountAfterFeed);

            connect(_ptrAppModel, &ApplicationModel::signal_routeRemovedSuccessfully,
                    _ptrAppView->ptrDialog_traces_edit(), &Dialog_traces_edit::slot_routeRemovedSuccessfully);

            connect(_ptrAppModel, &ApplicationModel::signal_routeAddedSuccessfully,
                    _ptrAppView->ptrDialog_traces_edit(), &Dialog_traces_edit::slot_routeAddedSuccessfully);


            _ptrAppView->showMenu();

            //@LP for dev-debug purpose:
            //  _ptrAppView->show_dialog_stackedProfilBox_tableView();
            //  _ptrAppView->show_dialog_stackedProfilBox_stackedProfilBoxEdition();

        }
    } else {
        if (!bLoadReportAbout_EPSGDataListFromProjDB) {
            QMessageBox qmb_loadProjDBError(QMessageBox::Icon::Critical,
                        "Resources file error",
                        "Error loading PROJ database (proj.db file)",
                        nullptr);
            qmb_loadProjDBError.exec();
            return(false);
        }
    }

    //we want that some item at 1 will be rendered as 1 pixel on HDPI screens also.
    //Hence, we send here to the graphicContainer the devicePixelRatio about image view
    //if not used, pen width at 1 will be rendered as 2 pixel on HDPI screen.
    //
    //doing it only here implyes that the devicePixelRatio is not udpated if the windows is drag and dropped on a different screen
    //with a different devicePixelRatio.
    //It seems (not tested) that the user will need to restart the app from the another screen.

    _ptrAppModel->ptrGraphicsItemContainer()->setDevicePixelRatioF(_ptrAppView->ptrWindowImageView()->devicePixelRatioF());

    _ptrAppView->ptrDialog_AppSettings()->feed(_appSettings.getAllsAppSettings());

    bool bShowDialogSettingsForMandatorySettingsToRunApp = false;

    bool bAppSettingsContainsSomeInvalidValues = _appSettings.containsSomeInvalidValues();
    if (bAppSettingsContainsSomeInvalidValues) {
        bShowDialogSettingsForMandatorySettingsToRunApp = true;
    }
    if (!bAppSettingsContainsSomeInvalidValues) {
        bool bModelInitAppDiskPathCacheStorage_success = _ptrAppModel->init_appDiskPathCacheStorage_andLinkedInit(_appSettings.get_cacheStorageRootPath());
        bShowDialogSettingsForMandatorySettingsToRunApp = !bModelInitAppDiskPathCacheStorage_success;
    }

    if (bShowDialogSettingsForMandatorySettingsToRunApp){
        _ptrAppView->ptrDialog_AppSettings()->show();
    }

    return(bModelInitSuccess);
}

//signal sent from the model which has just get the vectBoxId and mapBoxId of the hovered graphicBoxItem
//the controller here will send the order the new selected box in the two sides (models and ui)
void ApplicationController::slot_switchToBox_fromClickOnGraphicBoxItem(int vectBoxId, int mapBoxId) {

    qDebug() << __FUNCTION__ << "(ApplicationController) entering";

    _currentBoxId = vectBoxId;

    if (!_ptrAppView->ptrDialogVLayoutStackProfilEdit()->isVisible()) {        
        qDebug() << __FUNCTION__ << "(ApplicationController) cal now _ptrAppModel->selectBox(_currentBoxId = " << _currentBoxId;
        _ptrAppModel->selectBox(_currentBoxId); //this will update the _currentBoxId for the model and selected the highlight the box in the graphicscontainer

        //_ptrAppView->ptrDialog_exportResult()->setCurrentBoxId(_currentBoxId);

    } else {
        qDebug() << __FUNCTION__ << "(ApplicationController) cal now slot_stackProfilEdit_switchToBox(_currentBoxId = " << _currentBoxId;
        slot_stackProfilEdit_switchToBox(_currentBoxId, false);
    }
}

//Dialog_stackedProfilBoxes::signal_addingBoxModeActivated provide infos about the box that the user would like to add

//signal_enteringAddingBoxMode is the symmetry of slot_addingBoxModeCanceled
void ApplicationController::slot_enteringAddingBoxMode() {
    _bAddBoxModeEntered = true;
    _mainAppState.setEnable_menu_editingRouteInsideProject(false);    
}

void ApplicationController::slot_addingBoxModeCanceled() {
    _bAddBoxModeEntered = false;
    _mainAppState.setEnable_menu_editingRouteInsideProject(true);
    _ptrAppModel->slot_addingBoxModeCanceled();
}

void ApplicationController::slot_setGlobalZoomFactorTo1() {
    _ptrAppModel->setGlobalZoomFactorTo1();
}

void ApplicationController::slot_zoomStepIncDec(bool bStepIsIncrease) {
    _ptrAppModel->setZoomStepIncDec(bStepIsIncrease);
}

void ApplicationController::slot_Dialog_AppSettings_accepted() {
    //here if S_AppSettings in ptrDialog_AppSettings was evaluated as valid
    S_AppSettings sAppSettings = _ptrAppView->ptrDialog_AppSettings()->getAppSettings();
    sAppSettings.debugShow();
    bool bSomeSettingsModified = _ptrAppView->ptrDialog_AppSettings()->someSettingsModified();
    if (!bSomeSettingsModified) { //no change
        return;
    }

    _appSettings.setFrom_sAppSettings(sAppSettings);
    bool bSaved = _appSettings.save();
    if (!bSaved) {
        messageBoxForUser(QMessageBox::Warning, "Preferences", "Error saving preferences");
    } else {
        messageBoxForUser("Preferences", "The application will now quit. Restart it to take into account the new preferences");
        while (!app_exit()); //the user has to choose about save or not the modified file (route or project)
        //no way to continue without exit the app now
    }
}

#include "dialogStateSwitchConfirm/messageBoxWithVariableAnswers.h"

void ApplicationController::slot_Dialog_AppSettings_rejected() {

    if (_appSettings.containsSomeInvalidValues()) {
        MessageBoxWithVariableAnswers mesgBoxWVA_EditPrefOrQuit("Preferences",
                                                     "Some preferences parameters must be set to let the application works.",
                                                     "Edit preferences or quit now the application ?",
                                                     MessageBoxWithVariableAnswers::e_MBPA_EditQuit);
        int iReturnCode = mesgBoxWVA_EditPrefOrQuit.exec();
        if (iReturnCode == QMessageBox::AcceptRole) {
            _ptrAppView->ptrDialog_AppSettings()->show();
        } else {
            app_exit();
        }
    }
}


void ApplicationController::messageBoxForUser(QMessageBox::Icon qMessageBoxIcon, QString strTitle, QString strMessage) {
    QMessageBox messageBox(qMessageBoxIcon,
                           strTitle, strMessage,
                           QMessageBox::Ok, nullptr);
    messageBox.exec();
}

void ApplicationController::messageBoxForUser(QString strTitle, QString strMessage) {
    messageBoxForUser(QMessageBox::Information, strTitle, strMessage);
}


void ApplicationController::slot_messageBoxForUser(QString strTitle, QString strMessage) {
   messageBoxForUser(strTitle, strMessage);
}

void ApplicationController::slot_componentsToShowChanged(QVector<bool> qvectBoolComponentsToShow) {

    qDebug() << __FUNCTION__ <<" qvectBoolComponentsToShow = " << qvectBoolComponentsToShow;
   _ptrAppModel->componentsToShowChanged(qvectBoolComponentsToShow);

}

void ApplicationController::slot_removeBoxFromVectId(int vectBoxId_toRemove) {
    bool bBoxContainerNotEmpty = false;
    bool bRemoveReport = _ptrAppModel->removeBoxFromVectId(vectBoxId_toRemove, bBoxContainerNotEmpty);
    if (bRemoveReport) {

        if (!bBoxContainerNotEmpty) { //none box

            //disable profil edition & export; allow computation settings and run
            _mainAppState.setState(MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes);
            _ptrAppView->ptrDialog_stackedProfilBoxes()->slot_setBoxId_and_boxSize(-1,0,0); //back to none selected box

            _ptrAppModel->set_projectContentStateS_as_noneBox();
        }
    }
}

void ApplicationController::slot_removeAllBoxes() {
    if (_ptrAppModel) {
        bool bRemoveReport = _ptrAppModel->removeAllBoxes();
        if (bRemoveReport) {
            _currentBoxId = -1;
            _mainAppState.setState(MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes);
            _ptrAppView->ptrDialog_stackedProfilBoxes()->slot_setBoxId_and_boxSize(-1,0,0); //back to none selected box

            _ptrAppModel->set_projectContentStateS_as_noneBox();
        }
    }
}

void ApplicationController::slot_tryAddBox(QPointF scenePosNoZLI, int currentZLI) {

    if (_eRouteEditionStateOfRouteOfProject == e_rES_inEdition) { //silent rejection, should never happen if dialog and menuAction about adding box is correctly disabled
        return;
    }

    bool bCriticalError_pleaseExitApp = false;

    bool bAdded = _ptrAppModel->tryAddBox(scenePosNoZLI, currentZLI, bCriticalError_pleaseExitApp);
    if (!bAdded) {
        if (bCriticalError_pleaseExitApp) {
            messageBoxForUser(QMessageBox::Critical, "Fatal error adding box",
                                                     "Boxes data are unsync. Quit the application Without saving the project");
        } else {
            //messageBoxForUser("Adding boxes failed", "Error"); //@LP silent error case. Key press on an invalid red trace section leads a return value at false
        }
        return;
    }

    _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);//computation needs to be made before any new profil stack edition or export
    if (_bAddBoxModeEntered) {
        _mainAppState.setEnable_menu_editingRouteInsideProject(false);
    }
}


//this slot exists because table view is directly connected to the tablemodel which is an instance inside the AppModel
//Hence the ApplicationController which store a _currentBoxId for some ui handling has to be aware of vectBoxId change
//the use case is just to preselect the stacked profil edition to the selected box in the table view
//and for the export_result.
//The others ui parts are updated from anothers signals/slots
void ApplicationController::slot_vectBoxIdChanged_fromAppModel(int vectBoxId) {
    qDebug() << __FUNCTION__ << "(ApplicationController) " << "vectBoxId = " << vectBoxId;
    _currentBoxId = vectBoxId;
    qDebug() << __FUNCTION__ << "(ApplicationController) " << "set _currentBoxId to: " << _currentBoxId;

    //_ptrAppView->ptrDialog_exportResult()->setCurrentBoxId(_currentBoxId);
}


void ApplicationController::slot_stackProfilEdit_syncYRange_stateChanged(bool bState) {

    if (_currentBoxId == -1) {
        return;
    }

    _bStackProfilEdit_syncYRange = bState;
    _ptrAppModel->set_stackProfilEdit_syncYRange(_bStackProfilEdit_syncYRange);

    bool bReport = _ptrAppModel->prepareComputedBoxForStackEdition(_currentBoxId);

    if (!bReport) {
        qDebug() << __FUNCTION__ << "prepareComputedBoxForStackEdition said false";
        //@#LP error msg for user. Shoudl never happens and the developer has to look what's wrong if it happens
        messageBoxForUser("internal dev error", "prepareComputedBoxForStackEdition failed");
        return;
    }

    //@refactoring:
    //previously: called here:
    // - _ptrAppView->setModelsPtr_forStackProfilEdit(and setModelsPtr_forStackProfilEdit called updateOnDemand_onQvectStackprofiledit equivalent)
    // - _ptrAppView->ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots()
    //
    //_ptrAppView->setModelsPtr_forStackProfilEdit(_ptrAppModel->ptrCGScene_profils(),
    //                                             _ptrAppModel->ptrStackProfilEdit_textWidgetPartModel());
    //_ptrAppView->ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots();
    //

    //now: just call _ptrAppView->ptrDialogVLayoutStackProfilEdit()->updateOnDemand_onQvectStackprofiledit()
    //and setModelsPtr_forStackProfilEdit and ptrDialogVLayoutStackProfilEdit()->initConnectionModelViewSignalsSlots() called one time at init

    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->updateOnDemand_onQvectStackprofiledit();

    _ptrAppView->show_dialogVLayoutStackProfilEdit();

    _ptrAppModel->onStackedProfilScenes_alignComponentsForCurrentAlignMode();

}

//The main principles are:
//
// - the ui views receive update by signals from the model, and for these signals, the connections is made by each view
// In some cases, this main principale is bypassed, view and model signal/slot connected without the controller as a main in the midde.
// This is typically the case when using the zoom or moving in the image; the view needs to update according to the model directly. More,
// custom graphicsItem are considered as 'model' added to the scene (model for the graphical view).
//
// - the ui views request which are strongly associated with menu action use the controller as a main in the midde.
// the controller maintains states about the main current operation (eMainAppState)

bool ApplicationController::applicationPreferences_edit_caches() {

    /*QMessageBox messageBox(QMessageBox::Information,
                           __FUNCTION__, "entering",
                           QMessageBox::Ok, nullptr);
    messageBox.exec();*/

    _ptrAppView->ptrDialog_AppSettings()->feed(_appSettings.getAllsAppSettings());
    _ptrAppView->show_dialogAppSettings();
    return(true);
}

bool ApplicationController::applicationPreferences_edit_graphicsStyle() {
    _ptrAppView->ptrDialog_graphicsAppSettings()->feed_profil(_ptrAppModel->get_graphicsParametersProfil());
    //_ptrAppView->ptrDialog_graphicsAppSettings()->feed_itemOnImageView(_ptrAppModel->get_graphicsParametersItemOnImageView());
    _ptrAppView->show_dialogGraphicsAppSettings();
    return(true);
}

void ApplicationController::slot_actionFromUser_byMenuAction(int ieMA_id) {

    qDebug() << __FUNCTION__ << "ieMA_id = " << ieMA_id;

    //workaround specific to mac os which reenable menu action when the focus on the application was lost and got again
    //hence to avoid that the user can try to make a action during a long progress task bar,
    //this boolean was added and set to true when showing the task in progress dialog
    //and set to false when at the end of the task in progress (hiding the taks in progress dialog)
    if (_bRejectAnyMainMenuAction_dueToLongTaskInProgress) {
        return;
    }

    switch (ieMA_id) {

    case MenuActionStates::eMAPreferences_edit_caches:
        applicationPreferences_edit_caches();
        break;

    case MenuActionStates::eMAPreferences_edit_graphicsStyle:
        applicationPreferences_edit_graphicsStyle();
        break;

    //Routeset
    case MenuActionStates::eMARouteSet_newSet:
        routeSet_new();
        break;
    case MenuActionStates::eMARouteSet_openSet:
        routeSet_open();
        break;
    case MenuActionStates::eMARouteSet_saveSet:
        routeSet_save();
        break;
    case MenuActionStates::eMARouteSet_saveSetAs:
        routeSet_saveAs();
        break;
    case MenuActionStates::eMARouteSet_setEPSGCode:
        show_georefInputData_EPSGCodeIsEditableIfContextAccepts();
        break;

    case MenuActionStates::eMARouteSet_closeSet:
        routeSet_close();
        break;

    //Project
    case MenuActionStates::eMAProject_new:
        project_new();
        break;

    case MenuActionStates::eMAProject_open:
        project_open();
        break;

    case MenuActionStates::eMAProject_save:
        project_save();
        break;

    case MenuActionStates::eMAProject_saveAs:
        project_saveAs();
        break;

    case MenuActionStates::eMAProject_close:
        project_close();
        break;

    case MenuActionStates::eMARouteInsideProject_editOrLock:
        projectEditFct_aboutRoute_switchRouteEditionstate();
        break;

    case MenuActionStates::eMARouteInsideProject_rename:
        projectEditFct_aboutRoute_rename();
        break;

    case MenuActionStates::eMARouteInsideProject_export:
        projectEditFct_aboutRoute_export();
        break;

    case MenuActionStates::eMAStackProfBoxes_addBoxes_autoDistrib:
        projectEditFct_addBoxesUsingAutomaticDistribution();
        break;

    case MenuActionStates::eMAComp_SetParametersAndRun:
        projectEditFct_setComputationParametersAndRun();
        break;

    case MenuActionStates::eMAComp_EditStackedProfils:
        projectEditFct_menuAction_editStackedProfils();
        break;

    case MenuActionStates::eMAStackProfBoxes_editBoxes:
        projectEditFct_menuAction_editBoxes();
        break;

    case MenuActionStates::eMAStackProfBoxes_setProgressDirectionAlongTheFault:
        projectEditFct_menuAction_setProgressDirectionAlongTheFault();
        break;

    case MenuActionStates::eMAComp_exportResult:
        project_menuAction_exportResult();
        break;

    case MenuActionStates::eMAComp_setSpatialResolution:
        show_spatialResolutions_editable();
        break;

    case MenuActionStates::eMAView_imageView:
        showImageView(true, false);
        break;

    case MenuActionStates::eMAView_imageOverview:
        showImageView(false, true);
        break;

    case MenuActionStates::eMAView_projectInputFiles:
        show_projectInputFiles_notEditable();
        break;

    case MenuActionStates::eMAView_georefInputData:
        show_georefInputData_EPSGCodeIsEditableIfContextAccepts(); // editable if about trace, not about project
        break;

    case MenuActionStates::eMAView_tracesList:
        show_tracesList(); // editable if about trace, not about project
        break;

    case MenuActionStates::eMAView_stackedProfilesGraphicsMeaning:
        show_stackedProfilesGraphicsMeaning();
        break;

    case MenuActionStates::eMAApp_exit:
        app_exit();
        break;

    case MenuActionStates::eMAAbout_softwareInfos:
        app_show_softwareInfos();
        break;

    default:
        break;
    }
}


void ApplicationController::project_menuAction_exportResult(/*e_exportResultFileType eExportResultFileType*/) {

    qDebug() << __FUNCTION__ ;

    /*QMessageBox messageBox(QMessageBox::Information,
                           __FUNCTION__, "entering",
                           QMessageBox::Ok, nullptr);
    messageBox.exec();*/

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected: _ptrAppModel is nullptr";
        return;
    }

    _ptrAppModel->getConstRef_projectContentState().showDebug_states();

    e_Project_stackedProfilComputedState eP_stackedProfilComputedState =
            _ptrAppModel->getConstRef_projectContentState().getState_stackedProfilComputed();

    //should never happen if the menu action about export are correctly disabled after any changes which produce a not 'all computed' state
    if (eP_stackedProfilComputedState != e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_allComputed) {
        qDebug() << __FUNCTION__ << "can not export: eP_stackedProfilComputedState is: " << static_cast<int>(eP_stackedProfilComputedState);
        //msg box, computation needs to be run before a possible result export
        return;
    }

    e_Project_stackedProfilEditedState eP_stackedProfilEditedState =
            _ptrAppModel->getConstRef_projectContentState().getState_stackedProfilEdited();
    //should never happen, dev check test
    if (eP_stackedProfilEditedState == e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable) {
        //msg box, computation needs to be run before a possible result export
        qDebug() << __FUNCTION__ << "can not export: eP_stackedProfilEditedState is: " << static_cast<int>(eP_stackedProfilEditedState);
        return;
    }

    bool bSomeFlaggedProfilExist = _ptrAppModel->ptrCoreComputation_inheritQATableModel()->someFlaggedProfilExist();
    bool bSomeFlaggedProfilExistForCurrentBox = _ptrAppModel->ptrCoreComputation_inheritQATableModel()->someFlaggedProfilExistForBoxId(_currentBoxId);

    _ptrAppView->ptrDialog_exportResult()->setCurrentBoxId(_currentBoxId);
    _ptrAppView->ptrDialog_exportResult()->setSomeFlaggedProfilExist(bSomeFlaggedProfilExist, bSomeFlaggedProfilExistForCurrentBox);

    _ptrAppView->show_dialogExportResult();

}

void ApplicationController::slot_project_exportResult(S_ExportSettings sExportSettings) {

    //propose the file selection for the export:
    QString strDotExtension;
    switch (sExportSettings._ieExportResult_fileType) {
        case e_ERfT_json:
            strDotExtension = ".json";
            break;
        case e_ERfT_ascii:
            strDotExtension = ".txt";
            break;
    }
    QString strFilter = "*" + strDotExtension;

    QString strStartingDir = _appRecall.get_lastUsedDirectory();//StandardPathLocation::strStartingDefaultDir();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;
    QString strSaveFileName = QFileDialog::getSaveFileName(nullptr, tr("Export result to"), strStartingDir, strFilter);
    if (strSaveFileName.isEmpty()) {
        return;
    }
    if (!strSaveFileName.endsWith(strDotExtension)) {
        strSaveFileName+= strDotExtension;
    }

    bool bExportReport = false;

    bExportReport = _ptrAppModel->project_exportResult(strSaveFileName, sExportSettings);
    if (!bExportReport) {
        messageBoxForUser(QMessageBox::Warning, "Export", "Export error in file "+ strSaveFileName);
    } else {
        messageBoxForUser("Export", "Export successful in file "+ strSaveFileName);
                                  //"Success writing result into file "+ strSaveFileName);
    }
    _appRecall.set_lastUsedDirectoryFromPathFile(strSaveFileName);

}


void ApplicationController::projectEditFct_menuAction_setProgressDirectionAlongTheFault() {

    if (!_ptrAppModel) {
        //@#LP no err msg
        return;
    }
    e_ProfilOrientation eProfilOrientation = _ptrAppModel->getProgressDirection_settingToDefaultIfNotSet();

    Dialog_setProfilOrientation dial_setProfilOrientation;

    qDebug() << __FUNCTION__ << "eProfilOrientation = " << eProfilOrientation;

    _ptrAppModel->setVisible_startAndEndofRoute(true);

    dial_setProfilOrientation.setProfilOrientation(eProfilOrientation);

    int ieDialExecReturnCode = dial_setProfilOrientation.exec();
    qDebug() << "ieDialExecReturnCode = " << ieDialExecReturnCode;

    _ptrAppModel->setVisible_startAndEndofRoute(false);

    if (ieDialExecReturnCode == QDialog::Accepted) {        
        eProfilOrientation = dial_setProfilOrientation.getProfilOrientation();
        qDebug() <<  __FUNCTION__ << "test_dialog_setProfilOrientation.getProfilOrientation() " << eProfilOrientation;

        bool bReverseDoneOnBoxesContent = false;
        bool bRejected_alreadyOnThisOrientation = false;
        bool bNoFatalError = _ptrAppModel->setProfilOrientation(eProfilOrientation, bReverseDoneOnBoxesContent, bRejected_alreadyOnThisOrientation);

        if (!bNoFatalError) {
            messageBoxForUser(QMessageBox::Critical,
                              "Fatal error applying profil orientation",
                              "Boxes content are unsync. Don't export results and quit the application Without saving the project");
            return;
        }
        if (!bRejected_alreadyOnThisOrientation) {
            if (!bReverseDoneOnBoxesContent) {
                messageBoxForUser(QMessageBox::Warning,
                                  "Failed to apply profil orientation",
                                  "Orientation not applyed due to an internal error");
                return;
            }
        }
        if (_ptrAppView->ptrDialogVLayoutStackProfilEdit()->isVisible()) {
            slot_stackProfilEdit_switchToBox(_currentBoxId, false);
        }

    } else { // Dialog::Rejected
        qDebug() <<  __FUNCTION__ << " setProfilOrientation using dialog canceled";
    }

}

void ApplicationController::projectEditFct_menuAction_editBoxes() {

    qDebug() << __FUNCTION__ ;

    //avoid box edition window and profil curve displayed at the same time
    //moving or adding boxes lead to re-indexed boxes (and profils)
    _ptrAppView->hide_dialogVLayoutStackProfilEdit();
    _ptrAppView->ptrDialogVLayoutStackProfilEdit()->clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults();

    _ptrAppView->show_dialog_stackedProfilBox_stackedProfilBoxEdition();

}

void ApplicationController::slot_setEPSGCode(QString strEPSGCode) {

    qDebug() << __FUNCTION__ << "strEPSGCode :" << strEPSGCode;

    if (!_ptrAppModel) {
        qDebug() << __FUNCTION__ << "rejected request #0 : _ptrAppModel is nullptr";
        return;
    }    

    if (!_ptrAppModel->ptrGeoCRSSelect_inheritQATableModel()->checkStrEPSGCodeisKnown(strEPSGCode)) {
        messageBoxForUser(QMessageBox::Warning,
                          "Error about EPSG code",
                          "EPSG code "
                          + strEPSGCode
                          + " is unknown by PROJ database");
        return;
    }

    if (  (_mainAppState.get_eAppState() == MainAppState::eMAS_editingRouteSet)
        /*||(_mainAppState.get_eAppState() == MainAppState::eMAS_editingRouteSet_editingRoute)*/) {
        _ptrAppModel->routesetEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(strEPSGCode);
        _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

        qDebug() << __FUNCTION__ << "now call _ptrAppModel->sendAssociatedNameToEPSGCodeToView();";
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();

    } else {

        if (_mainAppState.get_eAppState() == MainAppState::eMAS_editingProject_newProject_settingIO) {
            _ptrAppModel->projectEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(strEPSGCode);
            _ptrAppModel->projectEditionFunction_sendStr_geoRefImagesSetStatus();

            qDebug() << __FUNCTION__ << "now call _ptrAppModel->sendAssociatedNameToEPSGCodeToView();";
            _ptrAppModel->sendAssociatedNameToEPSGCodeToView();

        } else {

            if (_mainAppState.get_eAppState() == MainAppState::eMAS_editingRouteSet_modifyingEPSGCode) {

                QString strErrorMsg;
                bool bChangeDone = false;
                bool bFatalErrorReport = _ptrAppModel->tryToChangeCurrentValidEPSGCode(strEPSGCode, bChangeDone, strErrorMsg);
                if (!bFatalErrorReport) {
                   messageBoxForUser(QMessageBox::Critical, "Fatal error about EPSG code", "No way to continue this session properly. Please quit the application now.");
                   return;
                }

                if (!bChangeDone) {

                    //feed the dialogGeoRefInfos before exec on it:
                    _ptrAppModel->sendStr_worldFileDataToView();
                    _ptrAppModel->sendStr_imagesFilename();
                    _ptrAppModel->sendEPSGCodeToView();

                    qDebug() << __FUNCTION__ << "now call _ptrAppModel->sendAssociatedNameToEPSGCodeToView();";
                    _ptrAppModel->sendAssociatedNameToEPSGCodeToView();

                    _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

                    QString strMsg = "Use of EPSG code " + strEPSGCode + " was rejected";
                    messageBoxForUser("Error about EPSG code", strMsg);
                    return;
                }

                _ptrAppView->ptrDialogGeoRefInfos()->setEditMode(e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet);

                _ptrAppModel->routesetEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(strEPSGCode);
                _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

                //feed the dialogGeoRefInfos before exec on it:
                _ptrAppModel->sendStr_worldFileDataToView();
                _ptrAppModel->sendStr_imagesFilename();
                _ptrAppModel->sendEPSGCodeToView();

                qDebug() << __FUNCTION__ << "now call _ptrAppModel->sendAssociatedNameToEPSGCodeToView();";
                _ptrAppModel->sendAssociatedNameToEPSGCodeToView();

                _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();

                return;

            } else {
                qDebug() << __FUNCTION__ << "rejected request #1 : _mainAppState.get_eAppState() = " << _mainAppState.get_eAppState();
            }
        }
    }
}

bool ApplicationController::handleDialogQuestionsAboutSaving_forAppFile(
        const AppFile& appFileToConsider,
        const QString& strDialogTitle,
        const QString& strDialogTextAboutNoSavedFile,
        const QString& strDialogQuestion,
        const QString& strDotExtentionForFile,
        bool &bWriteTheFile,
        bool &bSaveAs,
        QString &strRouteSetFilenameForSaveAs) {

    bWriteTheFile = false;
    bSaveAs = false;
    strRouteSetFilenameForSaveAs.clear();

    bool bOkForNewFunction = false;

    if (appFileToConsider.modified()) { //(in ram)
       qDebug() << __FUNCTION__ << "appFile_routeset.modified()";

       if (appFileToConsider.filenameSet()) { //wrote previously on disk
           qDebug() << __FUNCTION__ << "appFile_routeset.inUse()";

           //here if the file does not contain the last modification on disk

           Dialog_saveFile dialogSaveFile(strDialogTitle,
                                           strDialogTextAboutNoSavedFile,
                                           strDialogQuestion,
                                           strDotExtentionForFile,
                                           false);
           int ieDialExecReturnCode = dialogSaveFile.exec();
           qDebug() << __FUNCTION__ << "ieDialExecReturnCode = " << ieDialExecReturnCode;

           if (ieDialExecReturnCode == Dialog_saveFile::e_DialogSaveFile_cancel) {
               return(bOkForNewFunction); //new function will be canceled
           }
           if (ieDialExecReturnCode == Dialog_saveFile::e_DialogSaveFile_save) {
                bWriteTheFile = true;
           } else { // don't save the file before the new operation
               bOkForNewFunction = true;
           }

       } else { //no file associated, never wrote on disk and does not contains the last modification

           qDebug() << __FUNCTION__ << "never wrote on disk and does not contains the last modification";

           //=> 'cancel / do not save ' save as' dialog
           //before ok to close and edit a new routeset
           Dialog_saveFile dialogSaveFile(strDialogTitle,
                                           strDialogTextAboutNoSavedFile,
                                           strDialogQuestion,
                                           strDotExtentionForFile,
                                           true);
           QString strStartingDir = _appRecall.get_lastUsedDirectory();
           dialogSaveFile.setStartingDir(strStartingDir);

           int ieDialExecReturnCode = dialogSaveFile.exec();
           qDebug() << __FUNCTION__ << "ieDialExecReturnCode = " << ieDialExecReturnCode;

           if (ieDialExecReturnCode == Dialog_saveFile::e_DialogSaveFile_cancel) {
               return(bOkForNewFunction); //new function will be canceled
           }

           //user want to '"'save as' the current file before the new operation
           if (ieDialExecReturnCode == Dialog_saveFile::e_DialogSaveFile_saveAs) {
               bWriteTheFile = true;
               bSaveAs = true;
               strRouteSetFilenameForSaveAs = dialogSaveFile.getStrSelectedfileName();
            } else {
               //the user doesn't want to save the file before the new operation
               bOkForNewFunction = true;
            }
        }
    } else { //no new modification in ram

        bOkForNewFunction = true;

    }
    return(bOkForNewFunction);
}

void ApplicationController::slot_mainWindow_closeEvent() {
    app_exit();
}

//return false, if the user canceled the save or not modified file
bool ApplicationController::app_exit() {

    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_ptrAppModel->getAppFile_project().filenameSet()    = " << _ptrAppModel->getAppFile_project().filenameSet/*inUse*/();
    qDebug() << __FUNCTION__ << "_ptrAppModel->getAppFile_project().modified() = " << _ptrAppModel->getAppFile_project().modified();

    qDebug() << __FUNCTION__ << "_ptrAppModel->getAppFile_routeset().filenameSet()    = " << _ptrAppModel->getAppFile_routeset().filenameSet/*inUse*/();
    qDebug() << __FUNCTION__ << "_ptrAppModel->getAppFile_routeset().modified() = " << _ptrAppModel->getAppFile_routeset().modified();

    _appRecall.save();

    if (_ptrAppModel->getAppFile_project().modified()) {
        if (!project_close()) {
            return(false);
        }
    }
    if (_ptrAppModel->getAppFile_routeset().modified()) {
        if (!routeSet_close()) {
            return(false);
        }
    }

    QCoreApplication::quit();
    return(true);
}



void ApplicationController::slot_layersSwitcher_pushButtonClicked(int ieLA) {

    //in Ui side, updating the QTabWidget content about layers or state is very prone to emit signal about currentIndex changed
    //There are two booleans in the widget about this QTabWidget to avoid these unwanted signal to be emitted when the QTabWidget content
    //changed (to remove or add layers), and when setting the current Index when open a new or existing project using on of the layers.
    //_bLayerSwithIsPermitted is added in logic side as a second safety, rejecting to manage the signal when the application is
    //in a state about opening a new or existing project
    if (!_bLayerSwitchIsPermitted) {
        return;
    }

    qDebug() << __FUNCTION__ << "(ApplicationController)";

    if ((ieLA < eLA_PX1)||(ieLA > eLA_deltaZ)) {
        return;
    }

    if (_ptrAppModel->ptrCustomGraphicsScene()->tileProviderForIndexLayerIsLoaded(ieLA)) {
        _ptrAppModel->ptrCustomGraphicsScene()->switchToLayer(ieLA);
        _ptrAppModel->ptrSmallImageNav_CustomGraphicsScene()->switchToLayer(ieLA);
        _ptrAppModel->setIndexLayerForValueDequantization(ieLA);
        return;
    }



    bool bImageLoaded = _ptrAppModel->loadAdditionnalLayerImageBuildingImagePyramidIfNeeds_loadAlsoForSmallNav(ieLA);


    if (!bImageLoaded) {
        S_boolAndStrErrorMessage_aboutLoad SbaStrErrorMessage_aboutLoad;
        SbaStrErrorMessage_aboutLoad = _ptrAppModel->get_sboolAndStrMsgAbout_aboutLoad();

        QMessageBox messageBox(QMessageBox::Information,
                   "Image load error",
                   SbaStrErrorMessage_aboutLoad._strMessagAboutImage,
                   QMessageBox::Ok, nullptr);
        messageBox.exec();
        projectEditFct_clearState_backTo_DoNothing();
        _mainAppState.setState(MainAppState::eMAS_editingProject_newProject_settingIO);
        return;
    }

    _ptrAppModel->ptrCustomGraphicsScene()->switchToLayer(ieLA);
    _ptrAppModel->ptrSmallImageNav_CustomGraphicsScene()->switchToLayer(ieLA);
    _ptrAppModel->setIndexLayerForValueDequantization(ieLA);

}


void ApplicationController::slot_graphicsSettings_profil_changed(S_GraphicsParameters_Profil sGP_profil, bool bIsJustForPreview) {
    _ptrAppModel->set_graphicsSettings_profil(sGP_profil);
    if (!bIsJustForPreview) {
        _ptrAppModel->save_graphicsSettings();
    }
    if (_ptrAppView->ptrDialogVLayoutStackProfilEdit()->isVisible()) {
        slot_stackProfilEdit_switchToBox(_currentBoxId, false);
    }
}

void ApplicationController::slot_graphicsSettings_profil_preset_changed(int presetId) {
    S_GraphicsParameters_Profil SgppPreset = _ptrAppModel->get_graphicsParametersProfil_preset(presetId);
    _ptrAppView->ptrDialog_graphicsAppSettings()->feed_profilFromPreset_cancelable(SgppPreset);
    slot_graphicsSettings_profil_changed(SgppPreset, true);
}

void ApplicationController::slot_graphicsSettings_itemOnImgView_changed(S_GraphicsParameters_ItemOnImageView sGP_itemOnImageView, bool bIsJustForPreview) {
    _ptrAppModel->set_graphicsSettings_itemOnImageView(sGP_itemOnImageView);
    if (!bIsJustForPreview) {
        _ptrAppModel->save_graphicsSettings();
    }
}

void ApplicationController::showImageView(bool bShowImageView, bool bShowImageOverview) {

    int for_debug_breakpoint = 0;

    switch (_mainAppState.get_eAppState()) {

        case MainAppState::eMAS_idle:

        //cases which should never happen, as it's about open modal window
        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
        case MainAppState::eMAS_editingPreferences:
        case MainAppState::eMAS_editingProject_newProject_settingIO:
        case MainAppState::eMAS_editingProject_AppIscomputing:
            return;

        //possible MainAppStates
        case MainAppState::eMAS_editingRouteSet:
        case MainAppState::eMAS_editingRouteSet_editingRoute:

        case MainAppState::eMAS_editingProject_settingComputation: //state when setting computation is allowed //@#LP rename eMAS_editingProject_settingComputation
        //case MainAppState::eMAS_editingProject_editingStackedProfilBoxes:
        case MainAppState::eMAS_editingProject_idle:
        case MainAppState::eMAS_editingProject_editingRouteInsideProject:
        case MainAppState::eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes:
        case MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes:
        case MainAppState::eMAS_editingProject_editStackedProfilIsPossible:
        case MainAppState::eMAS_editingProject_editingStackedProfil:
            break;
    }
    _ptrAppView->showImageViewWindows(bShowImageView, bShowImageOverview);
}

void ApplicationController::show_tracesList() { // editable if about trace, not about project

    switch (_mainAppState.get_eAppState()) {

        case MainAppState::eMAS_idle:

        //cases which should never happen, as it's about open modal window
        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
        case MainAppState::eMAS_editingPreferences:
        case MainAppState::eMAS_editingProject_newProject_settingIO:
        case MainAppState::eMAS_editingProject_AppIscomputing:
        case MainAppState::eMAS_editingProject_settingComputation: //state when setting computation is allowed //@#LP rename eMAS_editingProject_settingComputation
            return;

        //cases about project. Hence no way to show the trace list which is about traceSet edition
        //@LP we avoid to only rely on 'disabled menu action'. Hence we check here these cases:
        //case MainAppState::eMAS_editingProject_editingStackedProfilBoxes:
        case MainAppState::eMAS_editingProject_idle:
        case MainAppState::eMAS_editingProject_editingRouteInsideProject:
        case MainAppState::eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes:
        case MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes:
        case MainAppState::eMAS_editingProject_editStackedProfilIsPossible:
        case MainAppState::eMAS_editingProject_editingStackedProfil:
            return;

        //possible MainAppStates
        case MainAppState::eMAS_editingRouteSet:
        case MainAppState::eMAS_editingRouteSet_editingRoute:
            break;
    }
    _ptrAppView->showTraceEditWindow();
}

void ApplicationController::show_stackedProfilesGraphicsMeaning() {

    _ptrAppView->ptrDialog_profilCurvesMeanings()->setUnits({"pixel", "meter"});

    QVector<bool> qvectBool_componentToCompute;
    QVector<QString> qvectStr_majorCurveMeaning;
    QVector<QString> qvectStr_minorEnveloppeAreaMeaning;
    bool bReport = _ptrAppModel->getQvectStrAndBoolFlagForComputedComponents_profilGraphicMeanings(
                qvectBool_componentToCompute,
                qvectStr_majorCurveMeaning,
                qvectStr_minorEnveloppeAreaMeaning);

    if (!bReport) {
        _ptrAppView->ptrDialog_profilCurvesMeanings()->setToNoValues();
        return;
    }
    _ptrAppView->ptrDialog_profilCurvesMeanings()->setMeanings(qvectBool_componentToCompute, qvectStr_majorCurveMeaning, qvectStr_minorEnveloppeAreaMeaning);
    //{false, false, false}, {"", "", ""},{"", "", ""});
    _ptrAppView->showStackedProfilesGraphicsMeaning();

}

void ApplicationController::show_projectInputFiles_notEditable() {

    _ptrAppView->show_dialog_projectInputFiles_viewContentOnly();

}

#include "../toolbox/toolbox_conversion.h"

void ApplicationController::show_spatialResolutions_editable() {

    bool bNeedToCloseDialogProfilsCurves = false;

    MainAppState::e_MainAppState currentMainAppState = _mainAppState.get_eAppState();

    switch (currentMainAppState) {

        case MainAppState::eMAS_idle:

        //cases which should never happen, as it's about open modal window
        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
        case MainAppState::eMAS_editingPreferences:
        case MainAppState::eMAS_editingProject_newProject_settingIO:
        case MainAppState::eMAS_editingProject_AppIscomputing:
        case MainAppState::eMAS_editingProject_editingRouteInsideProject:
        case MainAppState::eMAS_editingRouteSet_modifyingEPSGCode:
            return;

        //cases about traces set
        //@LP we avoid to only rely on 'disabled menu action'. Hence we check here these cases:
        case MainAppState::eMAS_editingRouteSet:
        case MainAppState::eMAS_editingRouteSet_editingRoute:
            return;

        //editing spatialResolution with computed boxes will lead to recomputation if any changes about spatialResolutions
        case MainAppState::eMAS_editingProject_editingStackedProfil:
        case MainAppState::eMAS_editingProject_editStackedProfilIsPossible:
            bNeedToCloseDialogProfilsCurves = true;
            break;

        //cases about project. Hence no way to show the trace list which is about traceSet edition
        //@LP we avoid to only rely on 'disabled menu action'. Hence we check here these cases:
        //case MainAppState::eMAS_editingProject_editingStackedProfilBoxes:
        case MainAppState::eMAS_editingProject_idle:
        //case MainAppState::eMAS_editingProject_editingRouteInsideProject:
        case MainAppState::eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes:
        case MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes:
        case MainAppState::eMAS_editingProject_settingComputation: //state when setting computation is allowed //@#LP rename eMAS_editingProject_settingComputation
            break;
    }

    bool bStackProfilEditWasVisisible = false;

    if (bNeedToCloseDialogProfilsCurves) {

        //change about spatial resolution leads to mandatory box recomputation
        //save the visible state of the dialogVLayoutStackProfilEdit to be restored if no change was made about spatial resolution
        //otherwise, recomputation will need to be run before profils edition
        bStackProfilEditWasVisisible = _ptrAppView->ptrDialogVLayoutStackProfilEdit()->isVisible();
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->hide_dialogVLayoutStackProfilEdit();
        }
    }

    QVector<float> qvectf_micmacStep_PX1_PX2_deltaZ_fromModelBeforeUpdate        {1.0, 1.0, 1.0};
    QVector<float> qvectf_spatialResolution_PX1_PX2_deltaZ_fromModelBeforeUpdate {1.0, 1.0, 1.0};
    QVector<bool> qvectb_layersInUseForComputation_fromModel {false,false, false};
    bool bGot = _ptrAppModel->get_micmacStepAndSpatialResolution(qvectf_micmacStep_PX1_PX2_deltaZ_fromModelBeforeUpdate,
                                                                 qvectf_spatialResolution_PX1_PX2_deltaZ_fromModelBeforeUpdate,
                                                                 qvectb_layersInUseForComputation_fromModel);
    //should never happen:
    if (!bGot) {
        QMessageBox messageBox(QMessageBox::Information,
            "Dev error #301",
            "get_micmacStepAndSpatialResolution(...) said false",
            QMessageBox::Ok, nullptr);
        messageBox.exec();

        //restore
        _mainAppState.setState(currentMainAppState);
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->show_dialogVLayoutStackProfilEdit();
        }
        return;
    }

    QVector<QString> qvectQstr_micmacStep_PX1_PX2_deltaZ        {"1", "1", "1"};
    QVector<QString> qvectQstr_spatialResolution_PX1_PX2_deltaZ {"1", "1", "1"};
    for (int iLayer = eLA_PX1; iLayer <= eLA_deltaZ; iLayer++) {
        if (qvectb_layersInUseForComputation_fromModel[iLayer]) {
            qvectQstr_micmacStep_PX1_PX2_deltaZ[iLayer] =                          doubleToQStringPrecision_justFit(static_cast<double>(qvectf_micmacStep_PX1_PX2_deltaZ_fromModelBeforeUpdate[iLayer]), 7);
            qvectQstr_micmacStep_PX1_PX2_deltaZ[iLayer] = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(qvectQstr_micmacStep_PX1_PX2_deltaZ[iLayer]);

            qvectQstr_spatialResolution_PX1_PX2_deltaZ[iLayer] =                   doubleToQStringPrecision_justFit(static_cast<double>(qvectf_spatialResolution_PX1_PX2_deltaZ_fromModelBeforeUpdate[iLayer]), 7);
            qvectQstr_spatialResolution_PX1_PX2_deltaZ[iLayer] = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(qvectQstr_spatialResolution_PX1_PX2_deltaZ[iLayer]);
        }
    }

    qDebug() << __FUNCTION__ << "qvectQstr_micmacStep_PX1_PX2_deltaZ = " << qvectQstr_micmacStep_PX1_PX2_deltaZ;
    qDebug() << __FUNCTION__ << "qvectQstr_spatialResolution_PX1_PX2_deltaZ = " << qvectQstr_spatialResolution_PX1_PX2_deltaZ;

    S_InputFiles inputFiles;
    _ptrAppModel->getInputFilesAndCorrelationScoreFiles_withoutFlagsAboutCorrelScoreMapToUseForComp(inputFiles);

    _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersFilenameAndSetVisibility(
                      inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ,
                      qvectb_layersInUseForComputation_fromModel);
    _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->initLayersmicMacStepValueAndSpatialResolution(
              qvectQstr_micmacStep_PX1_PX2_deltaZ,
              qvectQstr_spatialResolution_PX1_PX2_deltaZ,//setting default value with {str_xScale, str_YScale, "1.0"}, is not a good idea
              qvectb_layersInUseForComputation_fromModel);

    int ieDialExecReturnCode = _ptrAppView->exec_dialogMicmacStepAndSpatialResolution();
    if (ieDialExecReturnCode == QDialog::Rejected) {
        qDebug() << __FUNCTION__ << " canceled by user";

        //restore
        _mainAppState.setState(currentMainAppState);
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->show_dialogVLayoutStackProfilEdit();
        }
        return;
    }


    //to avoid any tolerance comparing numeric value, we compare string (with any useless zero removed)
    QVector<QString> qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ        = qvectQstr_micmacStep_PX1_PX2_deltaZ;
    QVector<QString> qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ = qvectQstr_spatialResolution_PX1_PX2_deltaZ;

    /*bool bValid = */_ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ_asString(
                                                                                  qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ,
                                                                                  qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ);
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (qvectb_layersInUseForComputation_fromModel[iela]) {
            qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ[iela]        = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ[iela]);
            qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ[iela] = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ[iela]);
        }
    }

    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (qvectb_layersInUseForComputation_fromModel[iela]) {
            qDebug() << __FUNCTION__ << "             qvectQstr_micmacStep_PX1_PX2_deltaZ[ " << iela << " ]= " <<              qvectQstr_micmacStep_PX1_PX2_deltaZ[iela];
            qDebug() << __FUNCTION__ << "qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ[ " << iela << " ]= " << qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ[iela];
            qDebug() << __FUNCTION__;
            qDebug() << __FUNCTION__ << "             qvectQstr_spatialResolution_PX1_PX2_deltaZ[ " << iela << " ]= " << qvectQstr_spatialResolution_PX1_PX2_deltaZ[iela];
            qDebug() << __FUNCTION__ << "qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ[ " << iela << " ]= " << qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ[iela];
        }
    }

    QVector<bool> qvect_bSame_PX1_PX2_deltaZ = {false, false, false};
    bool bAllSame = true;
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (qvectb_layersInUseForComputation_fromModel[iela]) {
            qvect_bSame_PX1_PX2_deltaZ[iela]  = (       qvectQstr_micmacStep_PX1_PX2_deltaZ[iela] ==        qvectQstr_afterEdition_micmacStep_PX1_PX2_deltaZ[iela]);
            qvect_bSame_PX1_PX2_deltaZ[iela] &= (qvectQstr_spatialResolution_PX1_PX2_deltaZ[iela] == qvectQstr_afterEdition_spatialResolution_PX1_PX2_deltaZ[iela]);

            bAllSame &= qvect_bSame_PX1_PX2_deltaZ[iela];
        }
    }

    //no change, show the previously visible VLayoutStackProfilEdit
    if (bAllSame) {
       //restore
        _mainAppState.setState(currentMainAppState);
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->show_dialogVLayoutStackProfilEdit();
        }
        return;
    }


    //double thresoldValueAboutDiff_for_micmacStep = 0.0009; //0.051 and 0.050 seen as different values; not 0.0500 and 0.0509)
    //double thresoldValueAboutDiff_for_spatialResolution = 0.0009; //0.002 and 0.001 seen as different values, not 0.0019 and 0.002)


    QVector<float> qvectf_micmacStep_PX1_PX2_deltaZ_newValues        {1.0, 1.0, 1.0};
    QVector<float> qvectf_spatialResolution_PX1_PX2_deltaZ_newValues {1.0, 1.0, 1.0};

    bool bValid = _ptrAppView->ptrDialog_micMacStepValueAndSpatialResolution()->get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
                                                                                  qvectf_micmacStep_PX1_PX2_deltaZ_newValues,
                                                                                  qvectf_spatialResolution_PX1_PX2_deltaZ_newValues);

    //should never happen:
    if (!bValid) {
        QMessageBox messageBox(QMessageBox::Information,
            "Dev error #300",
            "get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(...) said invalid",
            QMessageBox::Ok, nullptr);
        messageBox.exec();

        //restore
        _mainAppState.setState(currentMainAppState);
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->show_dialogVLayoutStackProfilEdit();
        }
        return;
    }

    bool bReport = _ptrAppModel->set_micmacStepAndSpatialResolution_forModels(qvectf_micmacStep_PX1_PX2_deltaZ_newValues,
                                                                              qvectf_spatialResolution_PX1_PX2_deltaZ_newValues,
                                                                              qvectb_layersInUseForComputation_fromModel, false,
                                                                              true);//set file as modified
    if (!bReport) {
        QMessageBox messageBox(QMessageBox::Information,
                    "Internal error", //@#LP no dev error number
                    "failed in set_micmacStepAndSpatialResolution_forModel",
                    QMessageBox::Ok, nullptr);
        messageBox.exec();

        //restore
        _mainAppState.setState(currentMainAppState);
        if (bStackProfilEditWasVisisible) {
            _ptrAppView->show_dialogVLayoutStackProfilEdit();
        }
        return;
    }

    //set recomputation as mandatory

    _mainAppState.setState(MainAppState::eMAS_editingProject_settingComputation);//computation needs to be made before any new profil stack edition or export
}



void ApplicationController::show_georefInputData_EPSGCodeIsEditableIfContextAccepts() { // editable if about trace, not about project

    //add a dev shield
    bool bViewValuesOnly = true;

    MainAppState::e_MainAppState currentMainAppState = _mainAppState.get_eAppState();

    switch (currentMainAppState) {
        case MainAppState::eMAS_idle:
        //cases which should never happen, as it's about open modal window
        case MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
        case MainAppState::eMAS_editingProject_newProject_settingIO:
        case MainAppState::eMAS_editingProject_AppIscomputing:
        case MainAppState::eMAS_editingPreferences: //this case reject also DialogGeoRefInfos, considered a minor use case
        case MainAppState::eMAS_editingRouteSet_modifyingEPSGCode: //if already in this state: return (should never happen)
        return;

        //cases about traces set
        //@LP we avoid to only rely on 'disabled menu action'. Hence we check here these cases:
        case MainAppState::eMAS_editingRouteSet_editingRoute://this case reject also DialogGeoRefInfos, considered a minor use case
        case MainAppState::eMAS_editingRouteSet:
            bViewValuesOnly = false;
            break;

        //@LP we avoid to only rely on 'disabled menu action'. Hence we check here these cases:
        case MainAppState::eMAS_editingProject_editingStackedProfil:
        //case MainAppState::eMAS_editingProject_editingStackedProfilBoxes:
        case MainAppState::eMAS_editingProject_idle:
        case MainAppState::eMAS_editingProject_editingRouteInsideProject:
        case MainAppState::eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes:
        case MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes:
        case MainAppState::eMAS_editingProject_editStackedProfilIsPossible:
        case MainAppState::eMAS_editingProject_settingComputation: //state when setting computation is allowed //@#LP rename eMAS_editingProject_settingComputation
            break;
    }

    _mainAppState.setState(MainAppState::eMAS_editingRouteSet_modifyingEPSGCode);

    if (bViewValuesOnly) {
        _ptrAppView->ptrDialogGeoRefInfos()->setEditMode(e_gRIUIEditMode_viewOnlyOnOpenedValidProject);
        //feed the dialogGeoRefInfos
        _ptrAppModel->sendStr_worldFileDataToView();
        _ptrAppModel->sendStr_imagesFilename();
        _ptrAppModel->sendEPSGCodeToView();
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
        _ptrAppModel->projectEditionFunction_sendStr_geoRefImagesSetStatus();

    } else {
        _ptrAppView->ptrDialogGeoRefInfos()->setEditMode(e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet);
        //feed the dialogGeoRefInfos
        _ptrAppModel->sendStr_worldFileDataToView();
        _ptrAppModel->sendStr_imagesFilename();
        _ptrAppModel->sendEPSGCodeToView();
        _ptrAppModel->sendAssociatedNameToEPSGCodeToView();
        _ptrAppModel->routesetEditionFunction_sendStr_geoRefImagesSetStatus();
    }

    int ieDialExecReturnCode = _ptrAppView->exec_dialogGeoRefInfos();
    if (ieDialExecReturnCode == QDialog::Rejected) {
        qDebug() << __FUNCTION__ << " canceled by user";
    }

    //restore
    _mainAppState.setState(currentMainAppState);
    //back to read write asap, for easiest new trace/new projet cases
    if (bViewValuesOnly) {
        _ptrAppView->ptrDialogGeoRefInfos()->setEditMode(e_gRIUIEditMode_newTraceSetOrProject);
    }
}

void ApplicationController::slot_request_EPSGCodeSelectionFromlist() {
    qDebug() << __FUNCTION__ << "(ApplicationController)";
    _ptrAppView->exec_dialog_geoCRSSelectFromlist();
}

//we want that the user validates the EPSG code that he selected from the list
void ApplicationController::slot_EPSGCodeSelectedFromList(QString strEPSGCode) {
    qDebug() << __FUNCTION__ << "(ApplicationController)";
   _ptrAppView->ptrDialogGeoRefInfos()->setEPSGCodeAsEditedText_UserValidationRequiered(strEPSGCode);
}

#include <QApplication>
void ApplicationController::slot_openDialog_taskInProgress_forLongComputation(QString strTitle, QString strMessage, int min, int max) {

    _bRejectAnyMainMenuAction_dueToLongTaskInProgress = true;

    _ptrAppView->open_dialog_taskInProgress(Dialog_taskInProgress::e_PageDTIP_messagePleaseWait_noAbortButton,
                                            //"processing...", "Building image pyramid", 0, 100);
                                            strTitle,strMessage,min,max);
    QApplication::processEvents(/*QEventLoop::ExcludeUserInputEvents*/);

    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

}

void ApplicationController::slot_closeDialog_taskInProgress_forLongComputation() {

    QGuiApplication::restoreOverrideCursor();

    _ptrAppView->close_dialog_taskInProgress();

    _bRejectAnyMainMenuAction_dueToLongTaskInProgress = false;

}

void ApplicationController::slot_stepProgressionForProgressBar(int value) {
    _ptrAppView->setStepProgression_dialog_taskInProgress(value);
}

void ApplicationController::app_show_softwareInfos() {
    qDebug() << __FUNCTION__ << "(ApplicationController)";
    _ptrAppView->show_dialog_softwareInfos();
}

