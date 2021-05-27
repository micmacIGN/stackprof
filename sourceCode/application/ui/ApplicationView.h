#ifndef APPLICATIONVIEW
#define APPLICATIONVIEW

#include <QVector>

#include "../logic/controller/AppState_enum.hpp"

class MainWindowMenu;
class WindowImageView;
class WindowSmallImageNav;

class CustomGraphicsScene;
class ZoomHandler;
class GeoUiInfos;

class SmallImageNav_CustomGraphicsScene;

class DialogVLayoutStackProfilEdit;
class Dialog_traces_edit;
class Dialog_geoRefInfos;

class CustomGraphicsScene_profil;
class StackProfilEdit_textWidgetPartModel;

class MenuActionStates;
class MenuStates;

class RouteContainer;

class ApplicationController;
class MainAppState;
class Dialog_project_inputFiles;

class Dialog_selectRouteFromRouteset;

class Dialog_addBoxes_automaticDistribution_closeAsPossible;

class Dialog_addBoxes_automaticDistribution_distanceBetweenCenter;

class Dialog_stackedProfilBox_tableView;

class ComputationCore_inheritQATableModel;

class Dialog_computationParameters;

class Dialog_MicMacStepValueAndSpatialResolution;

class Dialog_stackedProfilBoxes;

class Dialog_AppSettings;

class Dialog_addBoxesAutomaticDistribution_selectMethod;

class Dialog_graphicsSettings;

class Dialog_project_inputFiles_fixingMissingFiles;

class Dialog_project_inputFiles_viewContentOnly;

class Dialog_profilCurvesMeanings;

class Dialog_exportResult;

class Dialog_geoCRSSelect_tableView;

class geoCRSSelect_inheritQATableModel;

class DialogAbout;

#include <QProgressDialog>

#include "taskInProgress/dialog_taskinprogress.h"

class ApplicationView {

    public:
        ApplicationView();

        void setModelsPtr_forImageView(
                CustomGraphicsScene *ptrCustomGraphicsScene,
                ZoomHandler *ptrZoomHandler,
                GeoUiInfos *ptrGeoUiInfos);
        void initConnectionModelViewSignalsSlots();


        void hideWindowsAbout_routeEditionAndProject();

        void showMenu();
        void setAdditionnalStringToTitleOfMainWindowMenu(const QString& strToAdd ="");


        void showImageViewWindows(bool bShowImageView, bool bShowSmallImageNav);

        void showTraceEditWindow();

        void setModelPtr_forSmallImageNav(SmallImageNav_CustomGraphicsScene *ptrSmallImageNav_CustomGraphicsScene);


        void setModelsPtr_forStackProfilEdit(
                const QVector<CustomGraphicsScene_profil*>& qvectPtrCGScene_profil_Perp_Parall_deltaZ,
                const QVector<StackProfilEdit_textWidgetPartModel*>& qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ);

        ~ApplicationView();

        void setModelPtrForMainMenuActionStates(MenuActionStates *menuActionStatesPtr);
        void setModelPtrForMainMenuStates(MenuStates *menuStatesPtr);
        void setModelPtrForMainAppState(MainAppState *mainAppStatePtr);
        void setModelPtrForDialog_traces_edit(RouteContainer *routeContainerPtr);

        void setModelPtrForDialog_geoCRSSelect_tableView(geoCRSSelect_inheritQATableModel *geoCRSSelect_inheritQATableModel_ptr);

        //about menu user action, we use the controller as a man in the middle
        void setControllerPtr(ApplicationController *appControllerPtr);
        void initConnectionViewControllerSignalsSlots();

        MainWindowMenu *ptrMainWindowMenu()   { return(_mainWindowMenu);  }
        WindowImageView *ptrWindowImageView() { return(_windowImageView); }
        WindowSmallImageNav *ptrWindowSmallImageNav() { return(_windowSmallImageNav); }
        DialogVLayoutStackProfilEdit *ptrDialogVLayoutStackProfilEdit() { return(_dialogVLayoutStackProfilEdit); }
        Dialog_geoRefInfos *ptrDialogGeoRefInfos() { return(_dialog_geoRefInfos); }
        Dialog_project_inputFiles *ptrDialogProjectInputFile() { return(_dialog_project_inputFiles); }
        Dialog_selectRouteFromRouteset *ptrDialogSelectRouteFromRouteset() { return(_dialog_selectRouteFromRouteset); }
        Dialog_addBoxes_automaticDistribution_closeAsPossible *ptrDialog_addBoxes_automaticDistribution_closeAsPossible() { return(_dialog_addBoxes_automaticDistribution_closeAsPossible); }
        Dialog_addBoxes_automaticDistribution_distanceBetweenCenter *ptrDialog_addBoxes_automaticDistribution_distanceBetweenCenter() { return(_dialog_addBoxes_automaticDistribution_distanceBetweenCenter); }
        Dialog_computationParameters *ptrDialog_computationparameters() { return(_dialog_computationparameters); }
        Dialog_MicMacStepValueAndSpatialResolution *ptrDialog_micMacStepValueAndSpatialResolution() { return(_dialog_micMacStepValueAndSpatialResolution); }
        Dialog_stackedProfilBoxes *ptrDialog_stackedProfilBoxes() { return(_dialog_stackedProfilBoxes); }
        Dialog_AppSettings *ptrDialog_AppSettings() { return(_dialog_appSettings); }
        Dialog_addBoxesAutomaticDistribution_selectMethod *ptrDialog_addBoxesAutomaticDistribution_selectMethod() { return(_dialog_addBoxesAutomaticDistribution_selectMethod); }
        Dialog_graphicsSettings *ptrDialog_graphicsAppSettings() { return(_dialog_graphicsAppSettings); }
        Dialog_project_inputFiles_fixingMissingFiles *ptrDialogProjectInputFile_fixingMissingFiles() { return(_dialog_project_inputFiles_fixingMissingFiles); }
        Dialog_project_inputFiles_viewContentOnly *ptrDialog_project_inputFiles_viewContentOnly() { return(_dialog_project_inputfiles_viewContentOnly); }
        Dialog_profilCurvesMeanings *ptrDialog_profilCurvesMeanings() { return(_dialog_profilCurvesMeanings); }
        Dialog_exportResult *ptrDialog_exportResult() { return(_dialog_exportResult);}

        Dialog_geoCRSSelect_tableView *ptrDialog_geoCRSSelect_tableView() { return(_dialog_geoCRSSelect_tableView);}

        //QProgressDialog *ptrQProgressDialog() { return(_qProgressDialog); }
        Dialog_taskInProgress *ptrDialog_tDialog_project_inputFiles_askInProgress() { return(_dialog_taskInProgress); }

        Dialog_traces_edit *ptrDialog_traces_edit() { return(_dialog_traces_edit);}


        void set_menuActionAboutProjectEdition_editingRouteAsEditing(e_routeEditionState eRouteEditionState);

        int exec_dialogGeoRefInfos();

        //int exec_dialog_projectInputFiles(bool bClearFields);
        void show_dialog_projectInputFiles(bool bClearFields);
        void show_dialog_projectInputFiles_fixingMissingFiles();

        void hide_dialog_projectEdition();
        void hide_dialog_editBoxes();

        int exec_dialog_project_selectRouteFromRouteset(const QString strRoutesetFilename, int preSelectedRouteId_forRouteImport);

        void setModelPtrForDialog_routeFromRouteset(RouteContainer *routeContainerPtr);

        void setModelPtrForStackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr);
        void setModelPtrForStackedProfilBox_stackedProfilBoxesEdition(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr);

        void show_dialog_stackedProfilBox_tableView();
        void show_dialog_stackedProfilBox_stackedProfilBoxEdition();

        int exec_dialog_computationParameters();

        void show_dialogVLayoutStackProfilEdit(int boxId, int boxCount, bool bShowWindowOnTopAndGiveFocusToIt); //@#LP boxId passed through here
        void show_dialogVLayoutStackProfilEdit();
        void hide_dialogVLayoutStackProfilEdit();

        int exec_dialogMicmacStepAndSpatialResolution();

        int exec_dialog_addBoxesAutomaticDistribution_selectMethod();
        int exec_dialog_addBoxesAutomaticDistribution_closeAsPossible();
        int exec_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter();

        void show_dialogAppSettings();

        void show_dialogGraphicsAppSettings();

        /*void setProgressDialog_label_min_max(const QString& strLabel, int min, int max);
        void setProgressDialog_show();
        void setProgressDialog_stepValue(int value);
        void setProgressDialog_hide();*/

        int open_dialog_taskInProgress(Dialog_taskInProgress::e_Page_dialogTaskInProgress eP_dialogTaskInProgress,
                                        const QString& strBoxTitle, const QString& strProgressMessage,
                                        int progressValueMin, int progressValueMax);
        void setStepProgression_dialog_taskInProgress(int value);
        void close_dialog_taskInProgress();

        void show_dialog_projectInputFiles_viewContentOnly();
        void showStackedProfilesGraphicsMeaning();

        int exec_dialog_renameTrace(const QString& qstrCurrentRouteName, QString& qstrNewRouteName);

        int show_dialogExportResult();

        void show_dialog_geoCRSSelect_tableView();
        int exec_dialog_geoCRSSelectFromlist();

        void show_dialog_softwareInfos();

        //void allocInit_dialog_taskInProgress();

    private:
        MainWindowMenu *_mainWindowMenu;
        WindowImageView *_windowImageView;
        WindowSmallImageNav *_windowSmallImageNav;

        Dialog_traces_edit *_dialog_traces_edit;

        Dialog_geoRefInfos *_dialog_geoRefInfos;

        Dialog_project_inputFiles* _dialog_project_inputFiles;
        Dialog_selectRouteFromRouteset *_dialog_selectRouteFromRouteset;

        Dialog_MicMacStepValueAndSpatialResolution *_dialog_micMacStepValueAndSpatialResolution;

        Dialog_addBoxesAutomaticDistribution_selectMethod *_dialog_addBoxesAutomaticDistribution_selectMethod;

        Dialog_addBoxes_automaticDistribution_closeAsPossible *_dialog_addBoxes_automaticDistribution_closeAsPossible;

        Dialog_addBoxes_automaticDistribution_distanceBetweenCenter *_dialog_addBoxes_automaticDistribution_distanceBetweenCenter;

        Dialog_stackedProfilBox_tableView *_dialog_stackedProfilBox_tableView;

        Dialog_computationParameters *_dialog_computationparameters;

        DialogVLayoutStackProfilEdit *_dialogVLayoutStackProfilEdit;

        Dialog_stackedProfilBoxes *_dialog_stackedProfilBoxes;

        Dialog_AppSettings *_dialog_appSettings;

        Dialog_graphicsSettings *_dialog_graphicsAppSettings;

        Dialog_project_inputFiles_fixingMissingFiles *_dialog_project_inputFiles_fixingMissingFiles;

        Dialog_project_inputFiles_viewContentOnly *_dialog_project_inputfiles_viewContentOnly;

        Dialog_profilCurvesMeanings *_dialog_profilCurvesMeanings;

        //QProgressDialog *_qProgressDialog; //way #1

        Dialog_taskInProgress *_dialog_taskInProgress; //way #2

        Dialog_exportResult *_dialog_exportResult;

        Dialog_geoCRSSelect_tableView *_dialog_geoCRSSelect_tableView;

        DialogAbout *_dialog_softwareInfos;
};

#include "ui/appAbout/dialogabout.h"

#endif





