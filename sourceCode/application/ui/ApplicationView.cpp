#include <QDebug>

#include <QtGlobal>

#include "MainWindowMenu.h"
#include "WindowImageView.h"
#include "smallImageNavigation/windowsmallimagenav.h"

#include "../logic/model/imageScene/customgraphicsscene_usingTPDO.h"
#include "../logic/zoomLevelImage/zoomHandler.h"

#include "../logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.h"

#include "dialogVLayoutStackProfilEdit.h"

#include "../logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp"

#include "dialog_traces_edit.h"

#include "../logic/controller/ApplicationController.h"

#include "ApplicationView.h"

#include "dialog_geoRefInfos.h"

#include "dialog_project_inputfiles.h"

#include "dialog_selectroutefromrouteset.h"

#include "dialog_addboxesautomaticdistribution_selectmethod.h"

#include "dialog_addboxes_automaticdistribution_closeAsPossible.h"

#include "dialog_addboxes_automaticdistribution_distanceBetweenCenter.h"

#include "dialog_stackedprofilbox_tableview.h"

#include "dialog_computationparameters.h"

#include "dialog_micmacstepvalueandspatialresolution.h"

#include "dialog_stackedProfilBoxes.h"

#include "dialog_appsettings.h"

#include "dialog_taskinprogress.h"

#include "../logic/controller/AppState_enum.hpp"

#include "dialog_graphicsSettings.h"

#include "dialog_project_inputfiles_fixingMissingFiles.h"

#include "dialog_project_inputfiles_viewContentOnly.h"

#include "traceEdition/dialog_traceparameters.h"

#include "dialogProfilsCurves/dialog_profilcurvesmeanings.h"

#include "project/dialog_exportresult.h"

#include "geoCRSSelect/dialog_geoCRSSelect_tableview.h"

#include "../logic/model/appVersionInfos/appVersionInfos.hpp"

ApplicationView::ApplicationView() {
    //let qt ui widget on raw ptr
    _mainWindowMenu = new MainWindowMenu();

    _windowImageView = new WindowImageView();
    _windowSmallImageNav = new WindowSmallImageNav(/*_windowImageView*/);

    _dialog_traces_edit = new Dialog_traces_edit();
    _dialog_geoRefInfos = new Dialog_geoRefInfos();

    _dialog_project_inputFiles = new Dialog_project_inputFiles();
    _dialog_selectRouteFromRouteset = new Dialog_selectRouteFromRouteset();
    _dialog_micMacStepValueAndSpatialResolution = new Dialog_MicMacStepValueAndSpatialResolution();

    _dialog_addBoxesAutomaticDistribution_selectMethod = new Dialog_addBoxesAutomaticDistribution_selectMethod();
    _dialog_addBoxes_automaticDistribution_closeAsPossible = new Dialog_addBoxes_automaticDistribution_closeAsPossible();
    _dialog_addBoxes_automaticDistribution_distanceBetweenCenter = new Dialog_addBoxes_automaticDistribution_distanceBetweenCenter();

    _dialog_stackedProfilBox_tableView = new Dialog_stackedProfilBox_tableView();

    _dialog_computationparameters = new Dialog_computationParameters();

    _dialogVLayoutStackProfilEdit = new DialogVLayoutStackProfilEdit();

    _dialog_stackedProfilBoxes = new Dialog_stackedProfilBoxes();

    _dialog_appSettings = new Dialog_AppSettings();

    _dialog_graphicsAppSettings = new Dialog_graphicsSettings();

    _dialog_project_inputFiles_fixingMissingFiles = new Dialog_project_inputFiles_fixingMissingFiles();

    _dialog_project_inputfiles_viewContentOnly = new Dialog_project_inputFiles_viewContentOnly();

    _dialog_taskInProgress = nullptr;

    _dialog_profilCurvesMeanings = new Dialog_profilCurvesMeanings();

    _dialog_exportResult = new Dialog_exportResult();

    _dialog_geoCRSSelect_tableView = new Dialog_geoCRSSelect_tableView();

    _dialog_softwareInfos = new DialogAbout();

}

void ApplicationView::setAdditionnalStringToTitleOfMainWindowMenu(const QString& strToAdd) {

    if (!_mainWindowMenu) {
        return;
    }

    QString qstrAppTitle = AppVersionInfos::_sctct_softwareName;

    QString qstrFinalTitle = qstrAppTitle;

    if (!strToAdd.isEmpty()) {
        QString strToAdddCutIfNeeds = strToAdd;
        if (strToAdddCutIfNeeds.count() > 70 ) {
            strToAdddCutIfNeeds = "..." + strToAdddCutIfNeeds.right(70);
        }
        qstrFinalTitle += " (" + strToAdddCutIfNeeds + ")";
    }

    _mainWindowMenu->setWindowTitle(qstrFinalTitle);
}



void ApplicationView::setModelsPtr_forImageView(
        CustomGraphicsScene *ptrCustomGraphicsScene,
        ZoomHandler *ptrZoomHandler,
        GeoUiInfos *ptrGeoUiInfos) {
    if (!_windowImageView) {
        return;
    }
    _windowImageView->setZoomHandlerptr(ptrZoomHandler);
    _windowImageView->setCustomGraphicsScenePtr(ptrCustomGraphicsScene);
    _windowImageView->setGeoUiInfosPtr(ptrGeoUiInfos);
}

void ApplicationView::setModelPtr_forSmallImageNav(SmallImageNav_CustomGraphicsScene *ptrSmallImageNav_CustomGraphicsScene) {
   if (_windowSmallImageNav) {
       _windowSmallImageNav->setCustomGraphicsScenePtr(ptrSmallImageNav_CustomGraphicsScene);
   }
}

void  ApplicationView::setModelsPtr_forStackProfilEdit(
        const QVector<CustomGraphicsScene_profil*>& qvectPtrCGScene_profil_Perp_Parall_deltaZ,
        const QVector<StackProfilEdit_textWidgetPartModel*>& qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ) {
    if (!_dialogVLayoutStackProfilEdit) {
        return;
    }
    //dialog with vertical layout:
    _dialogVLayoutStackProfilEdit->setCGScenePtr(qvectPtrCGScene_profil_Perp_Parall_deltaZ);
    _dialogVLayoutStackProfilEdit->setTextWidgetPartModelPtr(qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ);
}

void ApplicationView::setControllerPtr(ApplicationController *appControllerPtr) {
    if (_dialog_traces_edit) {
        _dialog_traces_edit->setControllerPtr(appControllerPtr);
    }
}

void ApplicationView::initConnectionViewControllerSignalsSlots() {
    if (_dialog_traces_edit) {
        _dialog_traces_edit->initConnectionViewControllerSignalsSlots();
    }
}

void ApplicationView::set_menuActionAboutProjectEdition_editingRouteAsEditing(e_routeEditionState eRouteEditionState) {
    if (_mainWindowMenu) {
        _mainWindowMenu->set_menuActionAboutProjectEdition_editingRouteAsEditing(eRouteEditionState);
    }
}


//@#LP none nullptr check error report
void ApplicationView::initConnectionModelViewSignalsSlots() {

    if (_windowImageView) {
        _windowImageView->initConnectionModelViewSignalsSlots();
    }

    if (_windowSmallImageNav) {
        _windowSmallImageNav->initConnectionModelViewSignalsSlots();
    }

    if (_mainWindowMenu) {
        _mainWindowMenu->initConnectionModelViewSignalsSlots();
    }

    if (_dialog_traces_edit) {
        _dialog_traces_edit->initConnectionModelViewSignalsSlots();
    }
    if (_dialog_selectRouteFromRouteset) {
        _dialog_selectRouteFromRouteset->initConnectionModelViewSignalsSlots();
    }

    if (_dialog_stackedProfilBox_tableView) {
        _dialog_stackedProfilBox_tableView->initConnectionModelViewSignalsSlots();
    }
    if (_dialog_stackedProfilBoxes) {
        _dialog_stackedProfilBoxes->initConnectionModelViewSignalsSlots();
    }
    if (_dialog_geoCRSSelect_tableView) {
        _dialog_geoCRSSelect_tableView->initConnectionModelViewSignalsSlots();
    }
}



void ApplicationView::setModelPtrForMainMenuActionStates(MenuActionStates *menuActionStatesPtr) {
    if (_mainWindowMenu) {
        _mainWindowMenu->setModelPtrForMainMenuActionStates(menuActionStatesPtr);
    }
}

void ApplicationView::setModelPtrForMainMenuStates(MenuStates *menuStatesPtr) {
    if (_mainWindowMenu) {
        _mainWindowMenu->setModelPtrForMainMenuStates(menuStatesPtr);
    }
}

void ApplicationView::setModelPtrForMainAppState(MainAppState *mainAppStatePtr) {
    if (_mainWindowMenu) {
        _mainWindowMenu->setModelPtrForMainAppState(mainAppStatePtr);
    }
}


//@#LP could merge these two setModelPtrForDialog methods (#1)
void ApplicationView::setModelPtrForDialog_traces_edit(RouteContainer *routeContainerPtr) {
    if (_dialog_traces_edit) {
        _dialog_traces_edit->setModelPtrFortableView(routeContainerPtr);
    }   
}
//@#LP could merge these two setModelPtrForDialog methods (#2)
void ApplicationView::setModelPtrForDialog_routeFromRouteset(RouteContainer *routeContainerPtr) {
    if (_dialog_selectRouteFromRouteset) {
        _dialog_selectRouteFromRouteset->setModelPtrFortableView(routeContainerPtr);
    }
}

void ApplicationView::setModelPtrForDialog_geoCRSSelect_tableView(geoCRSSelect_inheritQATableModel *geoCRSSelect_inheritQATableModel_ptr) {
    if (_dialog_geoCRSSelect_tableView) {
        _dialog_geoCRSSelect_tableView->setModelsPtr_geoCRSSelect_tableView(geoCRSSelect_inheritQATableModel_ptr);
    }
}


#include <QMenuBar>

void ApplicationView::showMenu() { //@#LP be more accurate about what will be shown

    if (!_mainWindowMenu) {
        return;
    }
    _mainWindowMenu->show();

//
#ifdef Q_OS_OSX
    //MAC OS needs that the menu as no parent. But the menu as the mainwindow as parent (multiplatform implementation)
    //to avoid to have to extract the generated code by qt about this menu and set no parent to it
    //the workaround solution is to let mac show the empty mainwindow but minimize it after the first show (first show needs to be done to be able to populate the menu in the NativeMenuBar
    //
    //_mainWindowMenu->resize(300, 20);
    //_mainWindowMenu->setWindowState(Qt::WindowMinimized);   //_mainWindowMenu->showMinimized();

    //The other way can be to use AA_DontUseNativeMenuBar

#endif

}

void ApplicationView::showImageViewWindows(bool bShowImageView, bool bShowSmallImageNav) {
    if (bShowImageView) {
        if (_windowImageView) {
            _windowImageView->show();
            //_windowImageView->setWindowState(Qt::WindowState::WindowActive); // Bring window to foreground

            //https://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
            _windowImageView->setWindowState( (_windowImageView->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            _windowImageView->raise();  // for MacOS
            _windowImageView->activateWindow(); // for Windows
        }
    }
    if (bShowSmallImageNav) {
        if (_windowSmallImageNav) {
            _windowSmallImageNav->resizeToMatchWithSmallImageNav();
            _windowSmallImageNav->show();
            //_windowSmallImageNav->setWindowState(Qt::WindowState::WindowActive); // Bring window to foreground

            //https://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
            _windowSmallImageNav->setWindowState( (_windowSmallImageNav->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            _windowSmallImageNav->raise();  // for MacOS
            _windowSmallImageNav->activateWindow(); // for Windows
        }
    }
}

void ApplicationView::showTraceEditWindow() {
    if (_dialog_traces_edit) {
        _dialog_traces_edit->show();

        //https://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
        _dialog_traces_edit->setWindowState( (_dialog_traces_edit->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        _dialog_traces_edit->raise();  // for MacOS
        _dialog_traces_edit->activateWindow(); // for Windows
    }
}


//(non modal window)
void ApplicationView::hideWindowsAbout_routeEditionAndProject() {
    if (_windowImageView) {
        _windowImageView->hide_resetbFirstShow();
    }
    if (_windowSmallImageNav) {
        _windowSmallImageNav->hide();
    }
    if (_dialogVLayoutStackProfilEdit) {
        _dialogVLayoutStackProfilEdit->hide();
    }
    if (_dialog_traces_edit) {
        _dialog_traces_edit->hide();
    }
    if (_dialog_stackedProfilBoxes) {
        _dialog_stackedProfilBoxes->hide();
    }
    if (_dialog_project_inputfiles_viewContentOnly) {
        _dialog_project_inputfiles_viewContentOnly->hide();
    }
}

ApplicationView::~ApplicationView() {
    //@#LP let Qt ui widget on raw ptr
    delete _mainWindowMenu;
    delete _windowImageView;
    ////delete _windowSmallImageNav; //_windowSmallImageNav as _windowImageView as parent
    delete _dialogVLayoutStackProfilEdit;
    delete _dialog_traces_edit;
    delete _dialog_geoRefInfos;
    delete _dialog_project_inputFiles;
    delete _dialog_selectRouteFromRouteset;

    delete _dialog_addBoxesAutomaticDistribution_selectMethod;
    delete _dialog_addBoxes_automaticDistribution_closeAsPossible;
    delete _dialog_addBoxes_automaticDistribution_distanceBetweenCenter;

    delete _dialog_stackedProfilBoxes;

    delete _dialog_project_inputfiles_viewContentOnly;

    delete _dialog_profilCurvesMeanings;

    //@LP  Dialog_taskInProgress parent is _mainWindowMenu => do not delete _dialog_taskInProgress here
    /*if (_dialog_taskInProgress) { delete(_dialog_taskInProgress); }*/

    delete _dialog_exportResult;

    delete _dialog_geoCRSSelect_tableView;
}

int ApplicationView::exec_dialogGeoRefInfos() {
    if (!_dialog_geoRefInfos) {
        qDebug() << __FUNCTION__ << " error: _dial_geoRefInfos is nullptr";
        return(QDialog::Rejected);
    }
    return(_dialog_geoRefInfos->exec());
}

void ApplicationView::show_dialog_projectInputFiles(bool bClearFields) {
    if (!_dialog_project_inputFiles) {
        qDebug() << __FUNCTION__ << " error: _dial_project_inputFiles is nullptr";
        return;
    }
    if (bClearFields) {
        _dialog_project_inputFiles->setStateAsEmptyProject(); //_dialog_project_inputFiles->setToNoValues();
    }

    _dialog_project_inputFiles->show();
}

void ApplicationView::show_dialog_projectInputFiles_fixingMissingFiles() {
    if (!_dialog_project_inputFiles_fixingMissingFiles) {
        qDebug() << __FUNCTION__ << " error: _dial_project_inputFiles is nullptr";
        return;
    }

    _dialog_project_inputFiles_fixingMissingFiles->show();
}

void ApplicationView::hide_dialog_projectEdition() {

    if (_dialog_project_inputFiles) {
        if (_dialog_project_inputFiles->isVisible()) {
            _dialog_project_inputFiles->hide();
        }
    }

    if (_dialog_project_inputFiles_fixingMissingFiles) {
        if (_dialog_project_inputFiles_fixingMissingFiles->isVisible()) {
            _dialog_project_inputFiles_fixingMissingFiles->hide();
        }
    }
}

void ApplicationView::hide_dialog_editBoxes() {
    if (!_dialog_stackedProfilBoxes) {
        qDebug() << __FUNCTION__ << " error: _dialog_stackedProfilBoxes is nullptr";
        return;
    }
    _dialog_stackedProfilBoxes->hide();
}

int ApplicationView::exec_dialog_project_selectRouteFromRouteset(const QString strRoutesetFilename, int preSelectedRouteId_forRouteImport) {
    if (!_dialog_selectRouteFromRouteset) {
        qDebug() << __FUNCTION__ << " error: _dial_selectRouteFromRouteset is nullptr";
        return(QDialog::Rejected);
    }
    _dialog_selectRouteFromRouteset->set_routesetFileSelected(strRoutesetFilename);
    _dialog_selectRouteFromRouteset->set_selectedRouteId(preSelectedRouteId_forRouteImport);
    return(_dialog_selectRouteFromRouteset->exec());
}

void ApplicationView::setModelPtrForStackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr) {
    if (_dialog_stackedProfilBox_tableView) {
        _dialog_stackedProfilBox_tableView->setModelsPtr_stackedProfilBox_tableView(computationCore_iqaTableModel_ptr);
    }
}

void ApplicationView::setModelPtrForStackedProfilBox_stackedProfilBoxesEdition(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr) {
    if (_dialog_stackedProfilBoxes) {
        _dialog_stackedProfilBoxes->setModelsPtr_stackedProfilBox_tableView(computationCore_iqaTableModel_ptr);
    }
}

int ApplicationView::exec_dialog_renameTrace(const QString& qstrCurrentRouteName, QString& qstrNewRouteName) {

    Dialog_traceParameters dial_traceParameters(0, nullptr);
    dial_traceParameters.setRouteName(qstrCurrentRouteName);

    int ieDialogCodeReport = dial_traceParameters.exec();
    if (ieDialogCodeReport == QDialog::Accepted) {
        qstrNewRouteName = dial_traceParameters.getEditedName();
    }
    return(ieDialogCodeReport);
}

int ApplicationView::show_dialogExportResult() {
    if (!_dialog_exportResult) {
        qDebug() << __FUNCTION__ << " error: _dialog_exportResult is nullptr";
        return(QDialog::Rejected);
    }
    return(_dialog_exportResult->exec());
}

void ApplicationView::show_dialog_stackedProfilBox_tableView() {
    if (!_dialog_stackedProfilBox_tableView) {
        qDebug() << __FUNCTION__ << " error: _dialog_stackedProfilBox_tableView is nullptr";
        return;
    }
    _dialog_stackedProfilBox_tableView->show();
}

void ApplicationView::show_dialog_stackedProfilBox_stackedProfilBoxEdition() {
    if (!_dialog_stackedProfilBoxes) {
        qDebug() << __FUNCTION__ << " error: _dialog_stackedProfilBoxes is nullptr";
        return;
    }
    _dialog_stackedProfilBoxes->show();    

    //https://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
    _dialog_stackedProfilBoxes->setWindowState( (_dialog_stackedProfilBoxes->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    _dialog_stackedProfilBoxes->raise();  // for MacOS
    _dialog_stackedProfilBoxes->activateWindow(); // for Windows

}

int ApplicationView::exec_dialog_addBoxesAutomaticDistribution_closeAsPossible() {
    qDebug() << __FUNCTION__ ;
    if (!_dialog_addBoxes_automaticDistribution_closeAsPossible) {
        qDebug() << __FUNCTION__ << " error: dialog_addBoxesAutomaticDistribution is nullptr";
        return(QDialog::Rejected);
    }

    return(_dialog_addBoxes_automaticDistribution_closeAsPossible->exec());
}

int ApplicationView::exec_dialog_addBoxesAutomaticDistribution_distanceBetweenCenter() {
    qDebug() << __FUNCTION__ ;
    if (!_dialog_addBoxes_automaticDistribution_distanceBetweenCenter) {
        qDebug() << __FUNCTION__ << " error: _dialog_addBoxes_automaticDistribution_distanceBetweenCenter is nullptr";
        return(QDialog::Rejected);
    }
    return(_dialog_addBoxes_automaticDistribution_distanceBetweenCenter->exec());
}

int ApplicationView::exec_dialog_computationParameters() {
    qDebug() << __FUNCTION__ ;

    if (!_dialog_computationparameters) {
        qDebug() << __FUNCTION__ << " error: _dialog_computationparameters is nullptr";
        return(QDialog::Rejected);
    }
    return(_dialog_computationparameters->exec());
}


void ApplicationView::show_dialogVLayoutStackProfilEdit(int boxId, int boxCount, bool bShowWindowOnTopAndGiveFocusToIt) { //@#LP boxId passed through here

    qDebug() << "ApplicationView::show_dialogVLayoutStackProfilEdit(int boxId, int boxCount, bool bShowWindowOnTopAndGiveFocusToIt)";

     if (!_dialogVLayoutStackProfilEdit) {
          qDebug() << __FUNCTION__ << " error: _dialogVLayoutStackProfilEdit is nullptr";
          return;
    }
    _dialogVLayoutStackProfilEdit->feedTextWidgets(); //@#LP best place ?
    _dialogVLayoutStackProfilEdit->show();
    _dialogVLayoutStackProfilEdit->setLabelAndButtonsStateCurrentBoxId(boxId, boxCount);

    if (bShowWindowOnTopAndGiveFocusToIt) {
        //https://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
        _dialogVLayoutStackProfilEdit->setWindowState( (_dialogVLayoutStackProfilEdit->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        _dialogVLayoutStackProfilEdit->raise();  // for MacOS
        _dialogVLayoutStackProfilEdit->activateWindow(); // for Windows
    }

}

void ApplicationView::show_dialogVLayoutStackProfilEdit() {

    qDebug() << "ApplicationView::show_dialogVLayoutStackProfilEdit()";

    if (!_dialogVLayoutStackProfilEdit) {
          qDebug() << __FUNCTION__ << " error: _dialogVLayoutStackProfilEdit is nullptr";
          return;
    }
    _dialogVLayoutStackProfilEdit->feedTextWidgets(); //@#LP best place ?
    _dialogVLayoutStackProfilEdit->show();

}

//@#LP refactor hide and show merging with a boolean state:
void ApplicationView::hide_dialogVLayoutStackProfilEdit() {
   if (!_dialogVLayoutStackProfilEdit) {
         qDebug() << __FUNCTION__ << " error: _dialogVLayoutStackProfilEdit is nullptr";
         return;
   }
   _dialogVLayoutStackProfilEdit->hide();
}

void ApplicationView::show_dialogAppSettings() {
    if (!_dialog_appSettings) {
        qDebug() << __FUNCTION__ << " error: _dialogVLayoutStackProfilEdit is nullptr";
        return;
    }
    _dialog_appSettings->show();
}

void ApplicationView::show_dialogGraphicsAppSettings() {
    if (!_dialog_graphicsAppSettings) {
        qDebug() << __FUNCTION__ << " error: _dialog_graphicsAppSettings is nullptr";
        return;
    }
    _dialog_graphicsAppSettings->show();
}

int ApplicationView::exec_dialogMicmacStepAndSpatialResolution() {
    if (!_dialog_micMacStepValueAndSpatialResolution) {
         qDebug() << __FUNCTION__ << " error: _dialog_micMacStepValueAndSpatialResolution is nullptr";
         return(QDialog::Rejected);
   }
   return(_dialog_micMacStepValueAndSpatialResolution->exec());
}

int ApplicationView::exec_dialog_addBoxesAutomaticDistribution_selectMethod() {
    if (!_dialog_addBoxesAutomaticDistribution_selectMethod) {
         qDebug() << __FUNCTION__ << " error: _dialog_addBoxesAutomaticDistribution_selectMethod is nullptr";
         return(QDialog::Rejected);
   }
   return(_dialog_addBoxesAutomaticDistribution_selectMethod->exec());
}

void ApplicationView::show_dialog_projectInputFiles_viewContentOnly() {
    if (!_dialog_project_inputfiles_viewContentOnly) {
        qDebug() << __FUNCTION__ << " error: _dialog_addBoxesAutomaticDistribution_selectMethod is nullptr";
        return;
    }
    _dialog_project_inputfiles_viewContentOnly->show();
}

void ApplicationView::showStackedProfilesGraphicsMeaning() {
    if (!_dialog_profilCurvesMeanings) {
        qDebug() << __FUNCTION__ << " error: _dialog_profilCurvesMeanings is nullptr";
        return;
    }
    _dialog_profilCurvesMeanings->exec();
}

#include "ui/taskInProgress/dialog_taskinprogress.h"

#include <QApplication>

int ApplicationView::open_dialog_taskInProgress(Dialog_taskInProgress::e_Page_dialogTaskInProgress eP_dialogTaskInProgress,
                                                const QString& strBoxTitle, const QString& strProgressMessage,
                                                int progressValueMin, int progressValueMax) {
    if (_dialog_taskInProgress) {
        _dialog_taskInProgress->close();
        delete _dialog_taskInProgress;
        _dialog_taskInProgress = nullptr;
    }

    _dialog_taskInProgress = new Dialog_taskInProgress(_mainWindowMenu);

    _dialog_taskInProgress->setInitialState(eP_dialogTaskInProgress,
                                            strBoxTitle, strProgressMessage,
                                            progressValueMin, progressValueMax);

    _dialog_taskInProgress->open();  

    return(0);
}



void ApplicationView::setStepProgression_dialog_taskInProgress(int value) {
    qDebug() << __FUNCTION__ << "value: " << value;
    if (!_dialog_taskInProgress) {
        qDebug() << __FUNCTION__ << " error: _dialog_taskInProgress is nullptr";
        return;
    }
    _dialog_taskInProgress->setProgressState(value);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void ApplicationView::close_dialog_taskInProgress() {
    qDebug() << __FUNCTION__;
    if (!_dialog_taskInProgress) {
        qDebug() << __FUNCTION__ << " error: _dialog_taskInProgress is nullptr";
        return;
    }
    _dialog_taskInProgress->closeAndClear();

    delete _dialog_taskInProgress;
    _dialog_taskInProgress = nullptr;

}

/*
void ApplicationView::show_dialog_geoCRSSelect_tableView() {
    if (!_dialog_geoCRSSelect_tableView) {
        qDebug() << __FUNCTION__ << " error: _dialog_geoCRSSelect_tableView is nullptr";
        return;
    }
    _dialog_geoCRSSelect_tableView->show();
}
*/

int ApplicationView::exec_dialog_geoCRSSelectFromlist() {
    if (!_dialog_geoCRSSelect_tableView) {
        qDebug() << __FUNCTION__ << " error: _dialog_geoCRSSelect_tableView is nullptr";
        return(QDialog::Rejected);
    }
    return(_dialog_geoCRSSelect_tableView->exec());
}

void ApplicationView::show_dialog_softwareInfos() {
    if (!_dialog_softwareInfos) {
        qDebug() << __FUNCTION__ << " error: _dialog_softwareInfos is nullptr";
        return;
    }
    _dialog_softwareInfos->show();
}

