#ifndef APPLICATIONMODEL
#define APPLICATIONMODEL

#include "zoomLevelImage/zoomHandler.h"
#include "model/core/routeContainer.h"

#include "model/imageScene/customGraphicsItems/GraphicsItemsContainer.h"

#include <QVector>

#include <QObject>

#include "core/valueDequantization.hpp"

#include "core/ComputationCore_inheritQATableModel.h"

#include <QHash>

class CustomGraphicsScene;

class SmallImageNav_CustomGraphicsScene;

//StackProfilEdit models
class CustomGraphicsScene_profil;
class StackProfilEdit_textWidgetPartModel;

#include "geoUiInfos/GeoUiInfos.hpp"

#include "core/leftRight_sideAccess.hpp"

#include "StackedProfilInEdition.h"

#include "../model/georef/georefModel_imagesSet.hpp"

#include "../model/AppDiskPathCacheStorage/AppDiskPathCacheStorage.hpp"

#include "mainAppState.hpp"

#include "AppFile.hpp"

#include "core/BoxOrienterAndDistributer.h"

#include "georef/geoRefImagesSetStatus.hpp"

#include "Project_contentState.h"

#include "appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"
#include "appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h"

#include "appSettings/graphicsAppSettings/graphicsAppSettings.h"

#include "dialogProfilsCurves/enum_profiladjustmode.h"

#include "core/exportResult.h"

#include "../logic/model/core/geoConvertRoute.h"

#include "../logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.h"

struct S_boolAndStrErrorMessage_aboutLoad {
    bool _bFileDoesnotExistAtLocation;
    QString _strMessageAboutJson;
    QString _strMessagAboutImage;
    QString _strMessagAboutImage_details_filename;
    S_boolAndStrErrorMessage_aboutLoad();
};

struct S_Fields_AddingBox {

    QPointF _qpfPointToUseAsBoxCenterPoint;
    bool _bValid_qpfPointToUseAsBoxCenterPoint;
    int _idxSegmentOwner_ofQpfPointToUseAsBoxCenterPoint;

    S_ForbiddenRouteSectionsFromStartAndEnd _sForbiddenRouteSectionsFromStartAndEnd;

    S_Fields_AddingBox();
    void clear();
};

//#include <QFutureWatcher>

class ApplicationModel : public QObject {

    Q_OBJECT

    public:    

        void clearSomeModels_forNewAppFunctionStart(); //@#LP clearSomeModels_ naming

        ApplicationModel();
        ///bool init(/*const QString& strRootAppDiskPathCacheStorage,*/ int oiio_cacheAttribute_maxMemoryMB);
        bool init(int oiio_cacheAttribute_maxMemoryMB, bool& bLoadReportAbout_EPSGDataListFromProjDB);

        bool init_appDiskPathCacheStorage_andLinkedInit(const QString& strRootAppDiskPathCacheStorage);
        bool tryToLoadGraphicAppSettings();
        S_GraphicsParameters_Profil get_graphicsParametersProfil();
        S_GraphicsParameters_Profil get_graphicsParametersProfil_preset(int presetId);
        S_GraphicsParameters_ItemOnImageView get_graphicsParametersItemOnImageView();

        void clearStringsError();

        void initStateOfProjectContentState();
        bool setDefaultComputationModeAndParameters_ifNotSet();

        void setIndexLayerForValueDequantization(size_t ieLayerAcces);

        e_ProfilOrientation getProgressDirection_settingToDefaultIfNotSet();
        bool setProfilOrientation(e_ProfilOrientation eProfilOrientation, bool& bReverseDoneOnBoxesContent, bool& bRejected_alreadyOnThisOrientation);

        bool setInputFilesAndCorrelationScoreFiles(const S_InputFiles& inputFiles,const QVector<bool>& qvectb_correlScoreMapToUseForComp);

        void getInputFilesAndCorrelationScoreFiles_withoutFlagsAboutCorrelScoreMapToUseForComp(S_InputFiles& inputFiles) const;

        bool init_geoProj_onlyOneImageDisplayed(e_LayersAcces eLayerAccessForDisplaedImage);
        bool tryToChangeCurrentValidEPSGCode(const QString &strEPSGCode, bool& bChangeDone, QString &strErrorMsg);

        bool init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(e_LayersAcces eLayerAccessForDisplayedImage);

        bool prepareComputedBoxForStackEdition(int boxId, S_MeasureIndex measureIndex);
        bool prepareComputedBoxForStackEdition(int boxId);

        bool compute_leftRightSidesLinearRegressionsModels_forAllBoxes();


        void addNewRoute(QString strRouteName);
        void removeRoute(int idRoute);
        void renameRoute(int idRoute, const QString& strRouteName, bool bAboutRouteInsideProject);

        void routeSet_route_showAlone(int routeId, bool bShowAlone);


        QString getProjectRouteName();

        void setState_editingRoute_closeState_clearRouteset();
        void setState_editingProject_closeState_clearProject();

        CustomGraphicsScene *ptrCustomGraphicsScene();
        ZoomHandler* ptrZoomHandler();
        SmallImageNav_CustomGraphicsScene *ptrSmallImageNav_CustomGraphicsScene();
        GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet();

        const QVector<CustomGraphicsScene_profil*>& ptrCGScene_profils();
        const QVector<StackProfilEdit_textWidgetPartModel*>& ptrStackProfilEdit_textWidgetPartModel();

        GeoUiInfos* ptrGeoUiInfos();
        RouteContainer *ptrRouteContainer();
        GraphicsItemsContainer* ptrGraphicsItemContainer();

        ComputationCore_inheritQATableModel *ptrCoreComputation_inheritQATableModel();

        ~ApplicationModel();

        //@LP return copy
        AppFile getAppFile_routeset();
        AppFile getAppFile_project();

        //@#LP refactor to use the model instead
        //called only from controller when some files for the project were replaced due to input file location changes
        //In all others cases, this is in the model that the appFiles state are triggered to true
        void setAppFile_project_asModified();


        bool set_micmacStepAndSpatialResolution_forModels(const QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                                                          const QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                                                          const QVector<bool>& qvectb_layersToCompute,
                                                          bool bClear_geoUiInfos_ieLayerAccesForValueDequantization,
                                                          bool bSetAppFileModifiedStateAtTrue);

        bool get_micmacStepAndSpatialResolution(QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                                                QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                                                QVector<bool>& qvectb_layersToCompute) const;


        void setPtrVectDequantization_forGeoUiInfosModel(bool bClear_geoUiInfos_ieLayerAccesForValueDequantization);

        void setState_editingRoute(e_AppModelState eAppModelState, int ieRouteEditionState, int routeId);
        void setState_editingRoute_close();

        void sendStr_worldFileDataToView();
        void sendStr_imagesFilename();
        void sendEPSGCodeToView();
        void sendAssociatedNameToEPSGCodeToView();

        void send_vectLayersInUse_toView();
        void send_vectComputedComponentsInUse_toView();

        void sendComputationParametersToView();


        bool loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                const QString& qstrRootImageDisplay, int idxLayer);

        bool loadAdditionnalLayerImageBuildingImagePyramidIfNeeds_loadAlsoForSmallNav(int idxLayer);

        void setVisible_startAndEndofRoute(bool state);


        void set_projectContentStateS_as_noneBox();

        void selectBox(int vectBoxId);

        void setGlobalZoomFactorTo1();
        void setZoomStepIncDec(bool bStepIsIncrease);

        bool setComputationParameters_evaluatingIfEqualToCurrent(const S_ComputationParameters& computationParameters);
        bool isComputationRequiered_dueToParametersChanges();
        bool isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages();

        //--- routesetEditionFunction ---
        //
public:
        bool routesetEditionFunction_writeRouteset(); //use the current file
        bool routesetEditionFunction_writeRouteset(const QString& strNeWFileToSaveRoutesetInto); //will set on strNeWFileToSaveRoutesetInto and save on it

        //
        bool routesetEditionFunction_loadRouteset(const QString& strFileToLoadRouteset);//will call routesetEditionFunction_loadRoutesetw with _ptrGeoRefModel_imagesSet;
        //call also by projectEditionFunction__loadRouteset when importing routeset (ptrGeoRefModel_imagesSet set to _forImport...)
        bool routesetEditionFunction_loadRouteset(const QString& strFileToLoadRouteset, GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet);

        S_e_geoRefImagesSetStatus routesetEditionFunction_set_GeoRefModelImagesSet_fromBackgroundImageFilename(const QString& qstrImageFilename);
        S_e_geoRefImagesSetStatus routesetEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(const QString& strEPSGCode);
        void routesetEditionFunction_sendStr_geoRefImagesSetStatus();            
        bool routesetEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav();

        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus routesetEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus();
        S_boolAndStrErrorMessage_aboutLoad get_sbaStrErrorMessage_aboutLoad();
        S_boolAndStrErrorMessage_aboutLoad get_sboolAndStrMsgAbout_aboutLoad();

        S_e_geoRefImagesSetStatus routesetEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_withProvidedBackgroundImageFilename(const QString& qstrImageFilename) ;

        bool routesetOpenFunction_checkThatInputImageFileExists(const QString& strInputImageFileToCheckExistence);
        bool routesetOpenFunction_getBackgroundImageFilenameGotFromJsonLoad(QString& strBackgroundImageFilename, bool& bExists);
        bool routesetOpenFunction_checkThatInputImageFileExists(const QString& strFilename, bool& bExist);

        S_e_geoRefImagesSetStatus routesetOpenFunction_checkThatGeoDataOfInputImageFileMatchesWithGeoDataLoadedFromJson(
                const QString& strInputImageFileForComparisonWithGeoDataLoadedFromJson,
                S_boolAndStrMsgStatusAbout_geoRefImageSetStatus &sbASMsgStatusA_gRISS_aboutRouteSetEdition);

public:
        //when replacing the missing (moved file) image background of a already loaded routeset      
        S_e_geoRefImagesSetStatus routesetEditionFunction_compareCurrentWorldFileData_withDataFromFile(
                const QString& strImageBackgroundReplacement,
                S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sboolAndStrMsgStatusAbout_geoRefImageSetStatus) const;

        S_e_geoRefImagesSetStatus routesetEditionFunction_compareCurrentEPSGCode_withEPSGCodeFromFile(
                const QString& strImageBackgroundReplacement,
                S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sboolAndStrMsgStatusAbout_geoRefImageSetStatus) const;

public:
        bool routesetEditionFunction_storeInCacheTheCurrentEPSGCode_forImage(const QString& strImage);
        bool routesetEditionFunction_replaceBackgroundImage(const QString& strImageBackgroundReplacement);
        void routesetEditionFunction_initState_andFeedGraphicsItemContainer();
        QString routesetEditionFunction_strDetailsAboutFail_routesetWrite() const;
        bool    routesetEditionFunction_routesetContentIsCompatibleWithWriteOperation(QString &strDetailsAboutNotCompatible) const;

public slots:
        void slot_routesetEditionFunction_routeCountAdded(int routeCount);
        void slot_routesetEditionFunction_routeRemovedSuccessfully(int routeCountAfterRemove, int idRouteRemoved) ;
        void slot_routesetEditionFunction_routeAddedSuccessfully(int idRouteAdded);

signals:
        void signal_routeCountAfterFeed(int routeCount);
        void signal_routeRemovedSuccessfully(int routeCountAfterRemove, int newSelectedRoute);
        void signal_routeAddedSuccessfully(int idRouteAdded);

private:
        S_e_geoRefImagesSetStatus routesetEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus();
        S_e_geoRefImagesSetStatus routesetEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus();

        //-------------------------------

        //
        //--- projectEditionFunction ---
        //
public:
        bool projectEditionFunction_writeProject(); //use the current file
        bool projectEditionFunction_writeProject(const QString& strNeWFileToSaveProjectInto); //will set on strNeWFileToSaveProjectInto and save on it
        bool projectEditionFunction_setAndLoad_routesetFileForRouteImport(const QString& strRoutesetFilename);
        void projectEditionFunction_selectedRouteFromRouteset(int selectedIdRoute);
        void projectEditionFunction_setStateAddADefaultRoute(bool bState);
        bool projectEditionFunction_getStateAddADefaultRoute() const;

        int  projectEditionFunction_get_selectedRouteId_forRouteImport();
        void projectEditionFunction_setAsInvalid_selectedRouteFromRouteset();

        //----------------
        S_e_geoRefImagesSetStatus projectEditionFunction_set_GeoRefModelImagesSet_fromInputImageFilenames(const QVector<QString>& qvectStr_inputFiles);
        S_e_geoRefImagesSetStatus projectEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus();
        S_e_geoRefImagesSetStatus projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus();

        S_e_geoRefImagesSetStatus projectEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_fromInputImageFilenames(const QVector<QString>& qvectStr_inputFiles);



        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus projectEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus();
        void projectEditionFunction_sendStr_geoRefImagesSetStatus();
        S_e_geoRefImagesSetStatus projectEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(const QString& strEPSGCode);

        bool projectEditionFunction_storeInCacheTheCurrentEPSGCode_forImageWhichDoNotHave();


        bool projectEditionFunction_checkInputFilesForComputationMatchWithRequierement(
                const S_InputFiles& inputFiles, e_mainComputationMode eMainComputationMode,
                QVector<bool> &qvectb_LayersForComputationFlag, int& nbLayersForComputation,
                QVector<bool> &qvectb_correlScoreMapToUseForCompFlag, int& correlScoreMapToUseForComp,
                QString& strMsgErrorDetails) const;

        S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute get_sboolAndStrMsg_aboutGeoConvertAndImportRoute();

        bool projectEditionFunction_geoConvertSelectedRouteFromRouteset_andKeepAlone();

        bool projectEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                const QVector<QString>& qvectStr_inputFile_PX1_PX2_DeltaZ,
                const QVector<bool>& qvectb_layersToDisplay,
                int idxLayerToDisplayAsBackgroundImage);

        void projectEditionFunction_initState_andFeedGraphicsItemContainer(bool bIsANewProject);

        QString projectEditionFunction_strDetailsAboutFail_projectWrite() const;

        bool projectEditionFunction_projectContentIsCompatibleWithWriteOperation(QString &strDetailsAboutNotCompatible) const;

        bool projectEditionFunction_loadProject(const QString& strFileToLoadProject,
                                                //S_InputFiles &inputFiles,
                                                S_InputFilesWithImageFilesAttributes &sInputFilesWithAttributes_fromJsonFile,
                                                e_mainComputationMode& eMainComputationMode,
                                                QVector<bool>& qvectb_layersToCompute_justCheckingStringEmptiness,
                                                QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness);

        S_e_geoRefImagesSetStatus projectEditionFunction_openingFile_compareWorldFileDataAndEPSGCode_fromInputFiles_betweenThem(
            const S_InputFiles& inputFiles,
            const QVector<bool>& qvectb_layersToCompute,
            ImageGeoRef& strUniqueImageGeoRefGot,
            QString& strUniqueEPSGCodeGot,
            S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition) const;

        bool projectEditionFunction_compareWorldFileDataFromInputFiles_toCurrentWorldFileData(
            const ImageGeoRef &uniqueImageGotFromInputFiles, QString &strErrorMsg) const;

        bool projectEditionFunction_compareWorldFileDataAndEPSGCode_toCurrentImageGeoRefInfos(
            bool bCompareEPSGCodeAlso, const ImageGeoRef& uniqueImageGeoRef, const QString& strUniqueEPSGCodeGot, QString& strErrorMsg) const;

        bool projectEditionFunction_inGeoRefModelImagesSet_populateFilenamesAndSyncGeoInfosFromAlreadyInPlace(
                const QVector<QString>& qvectStrInputFiles, const QVector<bool>& vectb_layersForComputation, bool bIsAboutFilesReplacement);

        bool project_exportResult(const QString& strOutFileName, const S_ExportSettings &sExportSettings);

        bool addBoxesAutomaticDistribution_closeAsPossible(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int ieDistanceBSBuAD_selected,
                                                           QString &strErrorReason, bool& bCriticalError_pleaseExitApp);

        bool addBoxesAutomaticDistribution_distanceBetweenCenter(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int pixelDistanceBetweenCenter,
                                                                 QString &strErrorReason, bool& bCriticalError_pleaseExitApp);

        bool runComputation();

        bool componentsToShowChanged(const QVector<bool> &qvectBoolComponentsToShow);

        void set_stackProfilEdit_syncYRange(bool bSyncYRangeState);

        bool onStackedProfilScenes_alignComponentsForCurrentAlignMode();

        bool removeBoxFromVectId(int vectBoxId_toRemove, bool &bBoxContainerNotEmpty);
        bool removeAllBoxes();

        bool setBoxSizeOfCurrentBoxCheckingThatBoxFitInRoute(int vectBoxId, int oddPixelWidth, int oddPixelLength);
        bool moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(int vectBoxId, int shift);//received boxId has to be equal to _currentBoxIdInVectOrderAlongRoute (method rejects if not)

        bool projectEditFct_aboutRoute_checkRouteIsValid(QString& strError);
        void updateLocationOfSpecificItemsOfInterest_firstAndLastPointOfRoute();

        bool getQvectStrAndBoolFlagForComputedComponents_profilGraphicMeanings(
            QVector<bool>& qvectBool_componentToCompute,
            QVector<QString>& qvectStr_majorCurveMeaning,
            QVector<QString>& qvectStr_minorEnveloppeAreaMeaning);

private:

        bool project_exportResult_json(const QString& strOutFileName, const S_ExportSettings &sExportSettings);
        bool project_exportResult_ascii(const QString& strOutFileName, const S_ExportSettings &sExportSettings);

        bool onStackedProfilScenes_alignComponentsOnXRange();
        bool onStackedProfilScenes_alignComponentsOnXAndYRange();

        bool projectEditionFunction_loadRouteset(const QString& strFileToLoadRouteset);
        bool projectEditionFunction_loadDefaultRouteset();

        bool setVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
                const QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                const QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                const QVector<bool>& qvectb_layersToCompute);

        bool getVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
                QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                QVector<bool>& qvectb_layersToCompute) const;


        bool vectDequantization_toQJsonObject(QJsonObject& qjsonObj) const;

        bool testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(
            const QVector<int>& qvectShiftToTest,
            QHash<int, bool>& qhash_shift_bPossibleShift,
            QHash<int, S_ProfilsBoxParametersForRelativeMoveAlongRoute>& qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute);

signals:
        void signal_sendStr_routesetFilenameForRouteImport_aboutProjectEdition(QString strRoutesetFilenameForRouteImport);
        void signal_projectEditionFunction_sendIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport_toView(
                int routeId, QString strRoutesetFilename,
                QString strRouteName,
                bool bValid);

        void signal_setComponentsVisible_fromQvectBoolComponents(QVector<bool> qvectBoolComponentsToShow);

        void signal_sendBoxId_and_sizeToView(int boxId, int oddPixelWidth, int oddPixelLength);

        void signal_sendBoxCenterPossibleShiftsToView(QHash<int, bool> qhash_shift_bPossibleShift);

        void signal_tryAddBox(QPointF scenePosNoZLI, int currentZLI);

        void signal_oneOrMoreBoxesExist(bool bExist); //to inform the ui to enable/disable button about remove all boxes

        void signal_openDialog_taskInProgress_forLongComputation(QString strTitle, QString strMessage, int min, int max);
        void signal_closeDialog_taskInProgress_forLongComputation();

        void signal_stepProgressionFromComputationCore(int v); //the box in computation in the computaton core is transmitted through this signal

        void signal_vectBoxIdChanged_fromAppModel(int indexBoxInVectOrderAlongRoute); //used to inform the controller, which store the current vectBoxId to sync some ui view not updaded directly from qtablemodel

        void signal_switchToBox_fromClickOnGraphicBoxItem(int indexBoxInVectOrderAlongRoute, int mapBoxId_ofHoveredAndNotSelected);

public slots:

        void slot_stepProgressionFromComputationCore(int v); //the box in computation in the computaton core is transmitted through this signal


        void slot_selectionBoxChanged(int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion); //indexes of the same box in the different containers
        void slot_profilsBoxChanged(int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion);//indexes of the same box in the different containers


        void slot_addingBoxModeActivated(bool bBoxWidthValid, int oddPixelWidth, int oddPixelLength);
        void slot_addingBoxModeCanceled();
        void slot_displayPointAlongRouteAsPossibleCenterBox(QPointF qpfNoZLI, int currentZLI);


        void set_graphicsSettings_profil(S_GraphicsParameters_Profil sGP_profil);
        void save_graphicsSettings();

        void slot_profilAdjustMode_changed(int ieProfilAdjustMode); //signal emited from ui

        void set_graphicsSettings_itemOnImageView(S_GraphicsParameters_ItemOnImageView sGP_itemOnImageView);

        void slot_trySelectCurrentHoveredBox();


private:
        //-------------------------------
        void send_currentBoxIdInVectOrderAlongRoute_and_boxSize_toView(int boxId);
        void sendBoxCenterPossibleShiftsToView(const QVector<int> &qvectShift, const QVector<bool>& qvectbPossibleShift);
        void updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

public slots:

      void slot_xAdjustersSetPositionValues_changed_fromTextEdition(
              uint ieComponent,
              const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) ;

      void slot_xAdjusterPosition_changed_fromGraphics(
              uint ieComponent,
              uint ieLRsA_locationRelativeToX0CentralAxis,
              double xPos,
              int ieLRSA_adjuster);

      void slot_centerX0SetPositionValue_changed_fromTextEdition(uint ieComponent, double centerX0Pos);

      void slot_centerX0Position_changed_fromGraphics(uint ieComponent,
                                                       //bool bOnMainMeasure,
                                                       //int secondaryMeasureIdx,
                                                       double centerX0Pos);

      void slot_setWarnFlag_fromTextEdition(uint ieComponent, bool bWarnFlag, bool bAlternativeModeActivated);



      //connected from the image scene:
      void slot_addPointToSelectedRoute(QPointF scenePosNoZLI); //at the end of the route
      void slot_removePointOfRoute(int idPoint, int idRoute);      
      void slot_tryInsertPointToCurrentSelectedRoute(QPointF scenePosNoZLI, int currentZLI); //insert a point into a existing segment

      bool tryAddBox(QPointF scenePosNoZLI, int currentZLI, bool& bCriticalErrorOccured);

      void slot_pointOfRoute_movedOrRemoved();

      void slot_routesetEditionFunction_selectionChanged(int newlySelectedRouteId);

signals:
      //connected to the slot
      void signal_resetGraphicsRouteItemFromRoute(int routeId); //@#LP check why this is the Appmodel which send this

      void signal_sendStr_geoRefImagesSetStatus_aboutRoutesetEdition(
            bool bWorldFileData_available, bool bEPSG_available,
            QString strMsgAboutWorldFileData,  QString strMsgAboutWorldFileData_errorDetails,
            QString strMsgAboutEPSG, QString strMsgAboutEPSG_errorDetails);

      void signal_messageBoxForUser(QString strTitle, QString strMessage);

      void signal_stepProgressionForProgressBar(int value);

      void signal_sendAssociatedNameToEPSGCode(QString strAssociatedNameToEPSGCode);

private:
      bool onCurrentBox_set_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(
              uint ieComponent,
              const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]);

      bool onCurrentBox_set_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
              uint ieComponent,
              const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]);

      bool onCurrentBox_set_X0_and_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(
            uint ieComponent,
            double centerX0Pos);

      bool onCurrentBox_set_X0_and_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
              uint ieComponent,
              double centerX0Pos);

      bool onCurrentBox_set_warnFlag_andUpdateModelsForUi(uint ieComponent, bool bWarnFlag, bool bAlternativeModeActivated);

      bool constraintRangeDueToX0Change_forLinearRegressionComputation(
              double double_X0,
              S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]);


      void setAppFileModifiedState(bool bmodifiedState);

      bool buildImagePyramidifNeedsForImageDisplay(const QString& qstrRootImageDisplay, QString& qstrZLIStoragePath);

private:
        e_AppModelState _eAppModelState;

        //can contains more than one route, but the computation core will use only one
        RouteContainer _routeContainer;

        //about the image view
        CustomGraphicsScene *_ptrCustomGraphicsScene;

        ZoomHandler _zoomHandler;
        GraphicsItemsContainer _graphicsItemContainer;

        SmallImageNav_CustomGraphicsScene *_ptrSmallImageNav_CustomGraphicsScene;

        //the computation core:
        ComputationCore_inheritQATableModel _computationCore_iqaTableModel;

        vector<ValueDequantization> _vectDequantization_PX1_PX2_deltaZ;


        GeoRefModel_imagesSet *_ptrGeoRefModel_imagesSet;

        GeoRefModel_imagesSet _forRouteImport_geoRefModelImgSetAboutRouteset;

        AppDiskPathCacheStorage _appDiskPathCacheStorage;

        GeoUiInfos _geoUiInfos;

        QString _qstrRootForZLIStorage;
        //QString _qstrRootForEPSGCacheStorage;
        QSize _qsizeMax_smallImageNav; //@#LP constness


        //ui graphic stacked profil visualization and edition:
        QVector<CustomGraphicsScene_profil*> _qvectPtrCGScene_profil_Perp_Parall_deltaZ;

        QVector<StackProfilEdit_textWidgetPartModel*> _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ;
        //containers for a particular measure on a particular box with all need to visualize and edit:
        QVector<S_StackedProfilInEdition> _qvectStackedProfilInEdition_Perp_Parall_deltaZ;

        QVector<QString> _qvectqstr_graphicTitle_Perp_Parall_deltaZ;

        //@#LP maybe put in a model, stand by for now:
        QVector<QString> _qvectqstr_graphicUnits;

        //
        //--- editing project ---
        //
        QString _strDetailsAboutFail_ProjectWrite;
        QString _strRoutesetFilenameForRouteImport;
        int _selectedRouteId_forRouteImport;

        S_e_geoRefImagesSetStatus _segeoRefImageSetStatus_projectEdition;
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus _sbaStrMsg_gRIS_projectEdition;

        QString _strDetailsAboutFail_projectWrite;

        S_InputFiles _inputFiles;

        S_intBoolAndStrMsgStatus_aboutAutomaticDistribution _sibaStrMsgStatus_aboutAutomaticDistribution;
        //----------------

        //error message when loading an existing routeset
        S_boolAndStrErrorMessage_aboutLoad _sbaStrErrorMessage_aboutLoad;

        bool _bStateAddADefaultRoute;

        S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute _sbaStrMsgStatus_aboutGeoConvertAndImportRoute;

        //editing a routeset:
        AppFile _appFile_routeset;

        //editing a project
        AppFile _appFile_project;

        //
        //--- routeset ---
        //
        //--- route (also) ---
        //editing a route (alone or in a loaded routeset)
        int _selectedRouteId;
        //--------------------

        //status and error message when editing a new routeset:
        S_e_geoRefImagesSetStatus _segeoRefImageSetStatus_routeSetEdition;
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus _sbaStrMsg_gRIS_routeSetEdition;

        QString _strDetailsAboutFail_routesetWrite;
        //----------------

        BoxOrienterAndDistributer _boxOrienterAndDistributer;


        QVector<bool> _qvectBoolComputedComponents_Perp_Parall_deltaZ;

        QVector<bool> _qvectBoolComponentsToShow_Perp_Parall_deltaZ;


        int _currentBoxIdInVectOrderAlongRoute;

        S_MeasureIndex _currentMeasureIndex;
        bool _bStackProfilEdit_syncYRange;

        Project_contentState _projectContentState;

        bool _bAddingBoxMode;
        int _oddPixelWidth_addingBoxMode;
        int _oddPixelLength_addingBoxMode;
        S_Fields_AddingBox _fields_AddingBox;

        //used only to compute the minimal element side size for graphics container
        QSizeF _qszInputImageWidthHeight;

public:
        const Project_contentState& getConstRef_projectContentState() const;

private:
        //about moving the center point of an existing box, along the route
        QHash<int, S_ProfilsBoxParametersForRelativeMoveAlongRoute> _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute;
        bool _bFeedValid_qhash_shift;


private:
        int _oiio_cacheAttribute_maxMemoryMB;
        S_GraphicsParameters_Profil _sGgraphicsParameters_profil;
        S_GraphicsParameters_ItemOnImageView _sGgraphicsParameters_itemOnImageView;

        GraphicsAppSettings _graphicAppSettings;

        e_ProfilAdjustMode _eProfilAdjustMode;

private:
        geoCRSSelect_inheritQATableModel _geoCRSSelect_inheritQATableModel;
public:
        bool loadEPSGDataListFromProjDB();
        geoCRSSelect_inheritQATableModel *ptrGeoCRSSelect_inheritQATableModel();


private:
        bool _bUseGeoRefForTrace;
        bool _bUseGeoRefForProject;

public:
        void set_useGeoRefForTrace(bool bUseGeoRefForTrace);
        bool get_useGeoRefForTrace();

        void set_useGeoRefForProject(bool bUseGeoRefForProject);
        bool get_useGeoRefForProject();

        void projectEditionFunction_forRouteImport_routeSet_states_bContainsRoute_bUseGeoRef(bool& bContainsRoute, bool& bRouteSetUseGeoRef) const;
        void projectEditionFunction_forRouteImport_clearTraceSetAndTraceSelection_forRouteImport();

private:
        //QFutureWatcher<bool> _watcherBool;
public slots:
        //void slot_zLIImageBuil_ended();

};

#endif
