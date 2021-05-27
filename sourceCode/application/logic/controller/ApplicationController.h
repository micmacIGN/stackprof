#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

class ApplicationModel;
class ApplicationView;

#include "QMessageBox"

#include "model/mainAppState.hpp"
#include "model/AppFile.hpp"

#include "model/core/ComputationCore_structures.h" //for S_InputFilesWithImageFilesAttributes

#include "AppState_enum.hpp"

#include "model/appSettings/appSettings.h"
#include "model/appRecall/appRecall.h"
#include "model/appSettings/graphicsAppSettings/graphicsAppSettings.h"

#include "model/core/exportResult.h"

class ApplicationController : public QObject {

    Q_OBJECT

public:

  ApplicationController(ApplicationModel *ptrAppModel, ApplicationView *ptrAppView, QObject *parent = nullptr);

  bool tryToLoadAppRecall();
  bool tryToLoadAppSettings();
  bool tryToLoadGraphicAppSettings();

  bool start();

  void tryToSelectFirstBoxIfNoneSelected();

public slots:

  void slot_vectBoxIdChanged_fromAppModel(int vectBoxId);

  void slot_mainWindow_closeEvent();

  void slot_routeSet_addNewRoute(QString strRouteName);
  void slot_routeSet_removeRoute(int idRoute);
  void slot_routeSet_routeEditionStateChanged(int ieRouteEditionState, int selectedRouteTableViewRow);
  void slot_routeSet_routeEditionRenameRoute(int routeId, QString strRouteName);

  void slot_routeSet_route_showAlone(int routeId, bool bShowAlone);


  void slot_actionFromUser_byMenuAction(int ieMA_id);

  void slot_setEPSGCode(QString strEPSGCode);

  void slot_stackProfilEdit_switchToBox(int boxId, bool bShowWindowLayoutStackProfilEditOnTopAndGiveFocusToIt);
  void slot_stackProfilEdit_syncYRange_stateChanged(bool bState);

  void slot_messageBoxForUser(QString strTitle, QString strMessage);

  void slot_tryAddBox(QPointF scenePosNoZLI, int currentZLI);
  void slot_removeBoxFromVectId(int vectBoxId_toRemove);
  void slot_removeAllBoxes();  

  //void slot_userRequest_boxSizeChange(int vectBoxId, int oddPixelWidth, int oddPixelLength);
  void slot_userRequest_currentBoxSizeChange(int vectBoxId, int oddPixelWidth, int oddPixelLength);

  void slot_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(int vectBoxId, int shift);

  void slot_Dialog_AppSettings_accepted();
  void slot_Dialog_AppSettings_rejected();

  void slot_graphicsSettings_profil_changed(S_GraphicsParameters_Profil sGP_profil, bool bIsJustForPreview);
  void slot_graphicsSettings_profil_preset_changed(int presetId);

  void slot_graphicsSettings_itemOnImgView_changed(S_GraphicsParameters_ItemOnImageView sGP_itemOnImageView, bool bIsJustForPreview);

  //int iFirstIdxBoxForUiProgress = 1;
  //int iLastIdxBoxForUiProgress = _ptrAppModel->ptrCoreComputation_inheritQATableModel()->getBoxCount();
  //emit signal_showUiProcessingInProgress(iFirstIdxBoxForUiProgress, iLastIdxBoxForUiProgress, iFirstIdxBoxForUiProgress);



  void slot_setGlobalZoomFactorTo1();
  void slot_zoomStepIncDec(bool bStepIsIncrease);

  void slot_switchToBox_fromClickOnGraphicBoxItem(int vectBoxId, int mapBoxId);

  void slot_enteringAddingBoxMode();
  void slot_addingBoxModeCanceled();

  void slot_project_exportResult(S_ExportSettings sExportSettings);

  void slot_openDialog_taskInProgress_forLongComputation(QString strTitle, QString strMessage, int min, int max);
  void slot_closeDialog_taskInProgress_forLongComputation();
  void slot_stepProgressionForProgressBar(int value);

private:
  void clearVarStates();

  void sync_UseGeoRefForTrace_ofControllerAndModel_with_UseGeoRefForProject();

  void messageBoxForUser(QString strTitle, QString strMessage); //default QMessageBox::Icon::Information
  void messageBoxForUser(QMessageBox::Icon qMessageBoxIcon , QString strTitle, QString strMessage);

  bool applicationPreferences_edit_caches();
  bool applicationPreferences_edit_graphicsStyle();

  bool routeSet_new();
  bool routeSet_open();
  bool routeSet_save();
  bool routeSet_saveAs();
  bool routeSet_close();

  bool project_new();
  bool project_open();
  bool project_save();
  bool project_saveAs();
  bool project_close();


  bool routesetEditFct_questionAboutSaveSaveAs_and_saveRouteSet();
  bool routesetEditFct_writeRouteset_withMessageBoxIfFail(const QString& strRouteSetFilenameToSaveInto = "");
  bool routesetEditFct_writeRouteset_handleNoChangeOrChangeNotCompatibleWithWriteOperation(bool bWarnAboutNoChange, bool& bHasChanges);
  void routesetEditFct_clearState_backTo_DoNothing();


  bool projectEditFct_aboutRoute_switchRouteEditionstate();
  bool projectEditFct_aboutRoute_rename();
  bool projectEditFct_aboutRoute_export();


  bool projectEditFct_questionAboutSaveSaveAs_and_saveProject();
  bool projectEditFct_writeProject_withMessageBoxIfFail(const QString& strProjectFilenameToSaveInto = "");
  bool projectEditFct_writeproject_handleNoChangeOrChangeNotCompatibleWithWriteOperation(bool bWarnAboutNoChange, bool& bHasChanges);

  void projectEditFct_clearState_backTo_DoNothing();

  bool projectEditFct_addBoxesUsingAutomaticDistribution();
  //bool projectEditFct_addBoxesUsingAutomaticDistribution_closeAsPossible();


  bool projectEditFct_setComputationParametersAndRun();
  bool projectEditFct_menuAction_editStackedProfils();

  void project_menuAction_exportResult();

  bool handleDialogQuestionsAboutSaving_forAppFile(
          const AppFile& appFileToConsider,
          const QString& strDialogTitle,
          const QString& strDialogTextAboutNoSavedFile,
          const QString& strDialogQuestion,
          const QString& strDotExtentionForFile,
          bool &bWriteTheFile,
          bool &bSaveAs,
          QString &strRouteSetFilenameForSaveAs);

  void showImageView(bool bShowImageView, bool bShowImageOverview);
  void show_projectInputFiles_notEditable();
  void show_spatialResolutions_editable();
  void show_georefInputData_EPSGCodeIsEditableIfContextAccepts(); // editable if about trace, not about project
  void show_tracesList();
  void show_stackedProfilesGraphicsMeaning();

  bool app_exit();
  void app_show_softwareInfos();

  void proposeToAssociateTheEPSGCodeForImageWithout(const QString& strEPSGCodeForMessageBox);

public slots:
  void slot_componentsToShowChanged(QVector<bool> qvectBoolComponentsToShow);

  void slot_layersSwitcher_pushButtonClicked(int ieLA);

public slots:
  void slot_projectEditFct_newProject_routeSelectionRequestForImport(QString strRoutesetFilenameToUseForImport);
  void slot_projectEditFct_newProject_addADefaultRouteStateChanged(bool bAddADefaultRoute);

  void slot_projectEditFct_newProject_useGeoreferencing_stateChanged(bool bUseGeoreferencingForProject);

  void slot_projectEditFct_newProject_selected_filename_routeset_forRouteImport_hasChanged();

  void slot_projectEditFct_dialogProjectEdit_pushButtonOK_clicked();
  void slot_projectEditFct_dialogProjectEdit_pushButtonCancel_clicked();

  //void slot_projectEditFct_dialog_addBoxesAutomaticDistribution_createBoxes_clicked(int boxWidth, int boxlength, int ieDBSBuAD_selected);
  void slot_projectEditFct_dialog_addBoxesAutomaticDistribution_closeAsPossible_createBoxes_clicked(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int ieDistanceBSBuAD_selected);
  void slot_projectEditFct_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter_createBoxes_clicked(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int pixelDistanceBetweenCenter);

  void slot_request_EPSGCodeSelectionFromlist();
  void slot_EPSGCodeSelectedFromList(QString strEPSGCode);

private:
  void projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonCancelClicked();
  void projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonOKClicked();
  bool checkThatAllInputFilesDataAreSyncWithJson_and_open(const S_InputFiles& inputFiles, bool bIsAboutFilesReplacement);

  //@#LP waiting stabilization:
  //projectEditFct_aboutOpenProjetFixingIssue_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(...)
  //  just call projectEditFct_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(...)
  bool projectEditFct_aboutOpenProjetFixingIssue_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
      const S_InputFiles& inputFiles,
      const S_InputImagesFileAttributes& inputFileWithAttributesFromJsonFile,
      const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness, //@#LP rename ?
      const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
      QString& strMsgError);

  bool projectEditFct_checkInputFilesAttributesMatchWithAttributesGotFromJsonProjectFile(
          const S_InputFiles& inputFiles,
          const S_InputImagesFileAttributes& inputFileWithAttributesFromJsonFile,
          const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
          const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
          QString& strMsgError);

  void projectEditFct_aboutNewProject_dialogProjectEditPushButtonCancelClicked();
  void projectEditFct_aboutNewProject_dialogProjectEditPushButtonOKClicked();

  //call by project_open() or by projectEditFct_aboutOpenProjetFixingIssue_dialogProjectEditPushButtonOKClicked()
  bool projectEditFct_open_inputFilesExist(const S_InputFiles& inputFiles,
                                           const QVector<bool>& qvectb_layersToUseForCompute,
                                           const QVector<bool>& qvectb_correlMapFilenameAvailable,
                                           bool bIsAboutFilesReplacement);

  void projectEditFct_menuAction_editBoxes();
  void projectEditFct_menuAction_setProgressDirectionAlongTheFault();

  bool selectLayerToDisplayAsBackgroundImage(const QVector<bool>& qvectb_layersForComputation,
                                             int &idxLayerToDisplayAsBackgroundImage,
                                             bool& bCanceledByTheUser);

  bool projectEditFct_switch_eRouteEditionStateOfRouteOfProject();

  bool computeFlagsForMissingInputFiles(
      const S_InputFiles& inputFile,
      const QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
      const QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness,
      QVector<bool>& qvectb_missingFiles_inputFile_PX1_PX2_DeltaZ,
      QVector<bool>& qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone);


private:

  ApplicationModel* _ptrAppModel;
  ApplicationView* _ptrAppView;

  MainAppState _mainAppState;

  AppSettings _appSettings;
  AppRecall _appRecall;


  //about opening a project
  QVector<bool> _qvectb_layersToCompute_justCheckingStringEmptiness;
  QVector<bool> _qvectb_correlationMapUsage_justCheckingStringEmptiness;
  S_InputFilesWithImageFilesAttributes _inputFileWithAttributes_fromJsonFile;


  int _currentBoxId;
  bool _bStackProfilEdit_syncYRange;

  e_routeEditionState _eRouteEditionStateOfRouteOfProject;
  //additionnal flags about incompatible feature at the same time:
  //We considers that using hide/show about dialog/or menuAction is a 'too weak shield'
  bool _bAddBoxModeEntered;


  QVector<QString> _qvectStrLayersName;

  bool _bUseGeoRefForTrace;
  bool _bUseGeoRefForProject;

  bool _bRejectAnyMainMenuAction_dueToLongTaskInProgress;

  bool _bLayerSwitchIsPermitted;
};


#endif

