#include <QDebug>
#include <QCloseEvent>

#include "MainWindowMenu.h"
#include "ui_mainwindowMenu.h"

#include "model/MenuActionStates.hpp"
#include "model/MenuStates.hpp"

MainWindowMenu::MainWindowMenu(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindowMenu),
    _menuActionStatesPtr(nullptr),
    _menuStatesPtr(nullptr),
    _mainAppStatePtr(nullptr) {

    ui->setupUi(this);

    //for linux without menubar locked to the screen (like macos)
    //a work-around to have always the menu visible is to maximizz the mainwindow
    //This lead to e big empty window
    //hence, set a dark background to avoid eyes inconfort
    // //this->setWindowFlags(Qt::WindowStaysOnTopHint);
    //ui->centralwidget->setStyleSheet("QWidget { background-color: rgb(127,127,127); }"); //@LP gray background
    //ui->statusbar->setStyleSheet("QWidget { background-color: rgb(127,127,127); }"); //@LP gray background

    //menus:
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_file         , ui->menuFile);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_preferences  , ui->menuPreferences);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_routeSet, ui->menuRouteSet);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_routeInsideProject, ui->menuRoute);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_stackedProfilBoxes, ui->menuStackedProfilBoxes);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_computation       , ui->menuComputation);
    _qhash_ieM_qMenuPtr.insert(MenuStates::eMenu_view       , ui->menuView);

    //menu preferences:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAPreferences_edit_caches , ui->actionPreferences_editCaches);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAPreferences_edit_graphicsStyle , ui->actionPreferences_editGraphicsStyle);

    //menu project:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAProject_new , ui->actionProject_new);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAProject_open, ui->actionProject_open);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAProject_save, ui->actionProject_save);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAProject_saveAs, ui->actionProject_saveas);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAProject_close , ui->actionProject_close);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAApp_exit, ui->actionApp_exit);

    //menu routeSet:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_newSet  , ui->actionRouteSet_new);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_openSet , ui->actionRouteSet_open);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_saveSet , ui->actionRouteSet_save);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_saveSetAs, ui->actionRouteSet_saveas);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_setEPSGCode , ui->actionRouteSet_SetEPSGCode);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteSet_closeSet , ui->actionRouteSet_close);

    //menu route:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteInsideProject_editOrLock, ui->actionRoute_editOrLock);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteInsideProject_rename, ui->actionRoute_rename);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMARouteInsideProject_export, ui->actionRoute_export);

    //menu stacked profil boxes:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAStackProfBoxes_addBoxes_autoDistrib, ui->actionStackProfBoxes_addBoxes_autoDistrib);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAStackProfBoxes_editBoxes, ui->actionStackProfBoxes_editBoxes);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAStackProfBoxes_setProgressDirectionAlongTheFault, ui->actionSet_progress_direction_along_the_fault);

     //menu computation:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAComp_SetParametersAndRun, ui->actionComp_SetParametersAndRun);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAComp_EditStackedProfils , ui->actionComp_EditStackedProfils);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAComp_exportResult, ui->actionComp_exportResult);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAComp_setSpatialResolution , ui->actionComp_setSpatialResolution);

    //menu view:
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_imageView, ui->actionView_imageView);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_imageOverview, ui->actionView_imageOverview);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_projectInputFiles, ui->actionView_projectInputFiles);

    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_georefInputData, ui->actionView_georefInputData);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_tracesList, ui->actionView_tracesList);
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAView_stackedProfilesGraphicsMeaning, ui->actionView_stackedProfilesGraphicsMeaning);

    //about
    _qhash_ieMA_qActionPtr.insert(MenuActionStates::eMAAbout_softwareInfos, ui->actionAbout_softwareInfos);

    //all the triggered signals are connected to the same slot at the end, we use a property to know what action was triggered

    ui->actionPreferences_editCaches->setProperty("eMA_id", MenuActionStates::eMAPreferences_edit_caches);
    ui->actionPreferences_editGraphicsStyle->setProperty("eMA_id", MenuActionStates::eMAPreferences_edit_graphicsStyle);

    ui->actionProject_new   ->setProperty("eMA_id", MenuActionStates::eMAProject_new);
    ui->actionProject_open  ->setProperty("eMA_id", MenuActionStates::eMAProject_open);
    ui->actionProject_save  ->setProperty("eMA_id", MenuActionStates::eMAProject_save);
    ui->actionProject_saveas->setProperty("eMA_id", MenuActionStates::eMAProject_saveAs);
    ui->actionProject_close ->setProperty("eMA_id", MenuActionStates::eMAProject_close);

    ui->actionRouteSet_new   ->setProperty("eMA_id", MenuActionStates::eMARouteSet_newSet);
    ui->actionRouteSet_open  ->setProperty("eMA_id", MenuActionStates::eMARouteSet_openSet);
    ui->actionRouteSet_save  ->setProperty("eMA_id", MenuActionStates::eMARouteSet_saveSet);
    ui->actionRouteSet_saveas->setProperty("eMA_id", MenuActionStates::eMARouteSet_saveSetAs);
    ui->actionRouteSet_SetEPSGCode  ->setProperty("eMA_id", MenuActionStates::eMARouteSet_setEPSGCode);
    ui->actionRouteSet_close ->setProperty("eMA_id", MenuActionStates::eMARouteSet_closeSet);

    ui->actionRoute_editOrLock->setProperty("eMA_id", MenuActionStates::eMARouteInsideProject_editOrLock);
    ui->actionRoute_rename    ->setProperty("eMA_id", MenuActionStates::eMARouteInsideProject_rename);
    ui->actionRoute_export    ->setProperty("eMA_id", MenuActionStates::eMARouteInsideProject_export);

    ui->actionStackProfBoxes_addBoxes_autoDistrib->setProperty("eMA_id", MenuActionStates::eMAStackProfBoxes_addBoxes_autoDistrib);
    ui->actionStackProfBoxes_editBoxes->setProperty("eMA_id", MenuActionStates::eMAStackProfBoxes_editBoxes);
    ui->actionSet_progress_direction_along_the_fault->setProperty("eMA_id", MenuActionStates::eMAStackProfBoxes_setProgressDirectionAlongTheFault);

    ui->actionComp_SetParametersAndRun->setProperty("eMA_id", MenuActionStates::eMAComp_SetParametersAndRun);
    ui->actionComp_EditStackedProfils->setProperty("eMA_id", MenuActionStates::eMAComp_EditStackedProfils);
    ui->actionComp_exportResult->setProperty("eMA_id", MenuActionStates::eMAComp_exportResult);
    ui->actionComp_setSpatialResolution->setProperty("eMA_id",MenuActionStates::eMAComp_setSpatialResolution);

    ui->actionView_imageView        ->setProperty("eMA_id",MenuActionStates::eMAView_imageView);
    ui->actionView_imageOverview    ->setProperty("eMA_id",MenuActionStates::eMAView_imageOverview);
    ui->actionView_projectInputFiles->setProperty("eMA_id",MenuActionStates::eMAView_projectInputFiles);

    ui->actionView_georefInputData  ->setProperty("eMA_id",MenuActionStates::eMAView_georefInputData);
    ui->actionView_tracesList       ->setProperty("eMA_id",MenuActionStates::eMAView_tracesList);
    ui->actionView_stackedProfilesGraphicsMeaning->setProperty("eMA_id",MenuActionStates::eMAView_stackedProfilesGraphicsMeaning);

    ui->actionAbout_softwareInfos->setProperty("eMA_id", MenuActionStates::eMAAbout_softwareInfos);

    ui->actionApp_exit->setProperty("eMA_id", MenuActionStates::eMAApp_exit);
}

void MainWindowMenu::closeEvent(QCloseEvent *event) {
    event->ignore();
    emit signal_mainWindow_closeEvent();
}

MainWindowMenu::~MainWindowMenu() {
    delete ui;
}

void MainWindowMenu::setModelPtrForMainMenuActionStates(MenuActionStates *menuActionStatesPtr) {
    _menuActionStatesPtr = menuActionStatesPtr;
}

void MainWindowMenu::setModelPtrForMainMenuStates(MenuStates *menuStatesPtr) {
    _menuStatesPtr = menuStatesPtr;
}

#include "../logic/model/mainAppState.hpp"

void MainWindowMenu::setModelPtrForMainAppState(MainAppState *mainAppStatePtr) {
    _mainAppStatePtr = mainAppStatePtr;
}


void MainWindowMenu::set_menuActionAboutProjectEdition_editingRouteAsEditing(e_routeEditionState eRouteEditionState) {
    auto pairFound = _qhash_ieMA_qActionPtr.find(MenuActionStates::eMARouteInsideProject_editOrLock);
    if (pairFound !=_qhash_ieMA_qActionPtr.end()) {
        switch(eRouteEditionState) {
            case e_rES_inEdition:
                pairFound.value()->setText("Lock");
                break;
            case e_rES_locked:
                pairFound.value()->setText("Edit");
                break;
            case e_rES_notApplicable:
                pairFound.value()->setText("Edit/Lock"); //@LP this state must be displayed in grayed (disabled) state only
                break;
        }
    }
}


void MainWindowMenu::initConnectionModelViewSignalsSlots() {

    QHash<unsigned short int, QAction*>::iterator iter;
    for (iter = _qhash_ieMA_qActionPtr.begin();
         iter!= _qhash_ieMA_qActionPtr.end();
         ++iter) {

        //action from ui menu action to model:

        connect(iter.value(), &QAction::triggered, _mainAppStatePtr, &MainAppState::slot_menuActionTriggered);
    }

    //to enable or disable the ui menu action, depending of the context in the model side
    connect( _menuActionStatesPtr, &MenuActionStates::signal_setMenuActionEnabled, this, &MainWindowMenu::slot_setEnabledMenuAction);

    //to enable or disable the ui menu action, depending of the context in the model side
    connect( _menuStatesPtr, &MenuStates::signal_setEnabled, this, &MainWindowMenu::slot_setEnabledMenu);

}

void MainWindowMenu::slot_setEnabledMenuAction(unsigned short int ieMenuAction, bool bEnabled) {
    qDebug() << __FUNCTION__ << "received ieMenuAction = " << ieMenuAction << "with bEnabled = " << bEnabled;
    QHash<unsigned short int, QAction*>::iterator pairFound = _qhash_ieMA_qActionPtr.find(ieMenuAction);

    if (pairFound != _qhash_ieMA_qActionPtr.end()) {
        qDebug() << __FUNCTION__ << "pairFound.key() : " << pairFound.key() << "=> " << bEnabled;
        qDebug() << __FUNCTION__ << "pairFound.value() : " << pairFound.value();
        pairFound.value()->setEnabled(bEnabled);
    } else {
        qDebug() << __FUNCTION__ << "pair not Found";
    }
}


void MainWindowMenu::slot_setEnabledMenu(unsigned short int ieMenu, bool bEnabled) {
    qDebug() << __FUNCTION__;

    QHash<unsigned short int, QMenu*>::iterator pairFound = _qhash_ieM_qMenuPtr.find(ieMenu);
    if (pairFound != _qhash_ieM_qMenuPtr.end()) {

        qDebug() << __FUNCTION__ << pairFound.key() << "=> " << bEnabled;

        pairFound.value()->setEnabled(bEnabled);
    }
}

/*void MainWindowMenu::setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode) {

    bool bEnable_actions = (eCoordinateDisplayMode == GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);

    if (ui->menuView->isEnabled()) {
        ui->actionView_georefInputData->setEnabled(bEnable_actions);
    }
    if (ui->menuRouteSet->isEnabled()) {
        ui->actionRouteSet_SetEPSGCode->setEnabled(bEnable_actions);
    }
}*/
