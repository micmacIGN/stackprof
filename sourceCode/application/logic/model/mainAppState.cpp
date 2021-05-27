#include "MenuActionStates.hpp"
#include "mainAppState.hpp"

#include <QObject>
#include <QDebug>

MainAppState::MainAppState(QObject *parent): QObject(parent),
    _eAppState(eMAS_idle),
    _bPermitsMenuActionAboutGeoinfoData(true) {
}

MainAppState::e_MainAppState MainAppState::get_eAppState() {
    return(_eAppState);
}

void MainAppState::setEnable_menu_editingRouteInsideProject(bool bState) {
    if (bState) {
        _menuStates.setState_on({MenuStates::eMenu_routeInsideProject});
        _menuActionStates.setState_on_emitForAll(MenuStates::eMenu_stackedProfilBoxes,
            {MenuActionStates::eMAStackProfBoxes_addBoxes_autoDistrib,//<=
             MenuActionStates::eMAStackProfBoxes_editBoxes,
             MenuActionStates::eMAStackProfBoxes_setProgressDirectionAlongTheFault},
             MenuActionStates::eMAStackProfBoxes_lastAction_whichCanBeDisabled);

    } else {
        _menuStates.setState_off({MenuStates::eMenu_routeInsideProject});
        _menuActionStates.setState_off_emitForAll(MenuStates::eMenu_stackedProfilBoxes,
            {MenuActionStates::eMAStackProfBoxes_addBoxes_autoDistrib, //<=
             /*MenuActionStates::eMAStackProfBoxes_editBoxes,
             MenuActionStates::eMAStackProfBoxes_setProgressDirectionAlongTheFault*/},
             MenuActionStates::eMAStackProfBoxes_lastAction_whichCanBeDisabled);
    }
}


//eMAView_tracesList

void MainAppState::permitsMenuActionAboutGeoinfoData(bool bPermits) {
   _bPermitsMenuActionAboutGeoinfoData = bPermits;
}

void MainAppState::setState(e_MainAppState eAppState) {

    _eAppState = eAppState;

    switch(_eAppState) {

    case e_MainAppState::eMAS_idle:
        _menuStates.setState_on({MenuStates::eMenu_file,
                                 MenuStates::eMenu_routeSet});
        //all others menu off
        _menuStates.setState_off({MenuStates::eMenu_routeInsideProject,
                                  MenuStates::eMenu_stackedProfilBoxes,
                                  MenuStates::eMenu_computation});

        //about project: new & open, all other actions off
        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_file,
            {MenuActionStates::eMAProject_new,
             MenuActionStates::eMAProject_open},
             MenuActionStates::eMAProject_lastAction_whichCanBeDisabled);

        //about routeSet: new & open, all other actions off
        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_routeSet,
            {MenuActionStates::eMARouteSet_newSet,
             MenuActionStates::eMARouteSet_openSet},
             MenuActionStates::eMARouteSet_lastAction_whichCanBeDisabled);

        _menuStates.setState_off({MenuStates::eMenu_view});
        break;

    case e_MainAppState::eMAS_editingRouteSet:
        //about routeSet: all possible
        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_routeSet,
            {MenuActionStates::eMARouteSet_newSet,
             MenuActionStates::eMARouteSet_openSet,
             MenuActionStates::eMARouteSet_saveSet,
             MenuActionStates::eMARouteSet_saveSetAs,
             MenuActionStates::eMARouteSet_setEPSGCode,
             MenuActionStates::eMARouteSet_closeSet},
             MenuActionStates::eMARouteSet_lastAction_whichCanBeDisabled);
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMARouteSet_setEPSGCode});
        }
         //all others menu off
        _menuStates.setState_off({MenuStates::eMenu_routeInsideProject,
                                  MenuStates::eMenu_stackedProfilBoxes,
                                  MenuStates::eMenu_computation});        
        _menuActionStates.setState_off_emitForAll(
            MenuStates::eMenu_file,
            {MenuActionStates::eMAProject_new,
             MenuActionStates::eMAProject_open,
             MenuActionStates::eMAProject_save,
             MenuActionStates::eMAProject_saveAs,
             MenuActionStates::eMAProject_close},
             MenuActionStates::eMAProject_lastAction_whichCanBeDisabled);

        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
             MenuStates::eMenu_view,
             {MenuActionStates::eMAView_imageView,
              MenuActionStates::eMAView_imageOverview,
              //MenuActionStates::eMAView_projectInputFiles,
              MenuActionStates::eMAView_georefInputData,
              MenuActionStates::eMAView_tracesList},
              MenuActionStates::eMAView_lastAction_whichCanBeDisabled);
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMAView_georefInputData});
        }
        break;


    case e_MainAppState::eMAS_editingRouteSet_editingRoute:
        //no change
        break;

    //@#LP seems never used for now
    case eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes:
        _menuActionStates.setState_on({
          //MenuActionStates::eMAProject_new,
          //MenuActionStates::eMAProject_open,
          MenuActionStates::eMAProject_save, //the user will received a warning message rejecting the save operation   //@#LP needs to be tested
          MenuActionStates::eMAProject_saveAs, //the user will received a warning message rejecting the save operation //@#LP needs to be tested
          MenuActionStates::eMAProject_close});
        _menuStates.setState_on({MenuStates::eMenu_file/*,
                                 MenuStates::eMenu_routeInsideProject*/}); //@#LP editing trace in project not available for now
        _menuStates.setState_off({MenuStates::eMenu_routeSet,
                                  MenuStates::eMenu_stackedProfilBoxes,
                                  MenuStates::eMenu_computation,
                                  MenuStates::eMenu_stackedProfils});

        _menuActionStates.setState_off({MenuActionStates::eMAView_stackedProfilesGraphicsMeaning});
        break;


    case e_MainAppState::eMAS_editingProject_editingRouteInsideProject:
        _menuStates.setState_off({//MenuStates::eMenu_route, //already off
                                  MenuStates::eMenu_stackedProfilBoxes,
                                  MenuStates::eMenu_computation});
        _menuActionStates.setState_off({MenuActionStates::eMAView_stackedProfilesGraphicsMeaning});
        break;


    case e_MainAppState::eMAS_editingProject_idle:
        _menuActionStates.setState_on({
          //MenuActionStates::eMAProject_new,
          //MenuActionStates::eMAProject_open,
          MenuActionStates::eMAProject_save,
          MenuActionStates::eMAProject_saveAs,
          MenuActionStates::eMAProject_close});
        _menuStates.setState_on({MenuStates::eMenu_file,
                                 MenuStates::eMenu_routeInsideProject,
                                 MenuStates::eMenu_stackedProfilBoxes,
                                 MenuStates::eMenu_computation});
        _menuStates.setState_off({MenuStates::eMenu_routeSet});

        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
                    MenuStates::eMenu_view,
                    {MenuActionStates::eMAView_imageView,
                     MenuActionStates::eMAView_imageOverview,
                     //MenuActionStates::eMAView_projectInputFiles,
                     //MenuActionStates::eMAView_georefInputData
                     },
                     MenuActionStates::eMAView_lastAction_whichCanBeDisabled);

        _menuActionStates.setState_off({MenuActionStates::eMAView_stackedProfilesGraphicsMeaning});//@#LP TBC
        break;    

    case e_MainAppState::eMAS_editingProject_idle_noneStackedProfilsBoxes:
        /*_menuActionStates.setState_on({
          //MenuActionStates::eMAProject_new,
          //MenuActionStates::eMAProject_open,
          MenuActionStates::eMAProject_save,
          MenuActionStates::eMAProject_saveAs,
          MenuActionStates::eMAProject_close});*/

        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_file,
            {MenuActionStates::eMAProject_save,
             MenuActionStates::eMAProject_saveAs,
             MenuActionStates::eMAProject_close},
             MenuActionStates::eMAProject_lastAction_whichCanBeDisabled);

        _menuStates.setState_on({MenuStates::eMenu_file,
                                 MenuStates::eMenu_routeInsideProject,
                                 MenuStates::eMenu_stackedProfilBoxes});

        _menuStates.setState_off({MenuStates::eMenu_routeSet,
                                  MenuStates::eMenu_computation,
                                  MenuStates::eMenu_stackedProfils});

        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
                     MenuStates::eMenu_view,
                    {MenuActionStates::eMAView_imageView,
                     MenuActionStates::eMAView_imageOverview,
                     MenuActionStates::eMAView_projectInputFiles,
                     MenuActionStates::eMAView_georefInputData,
                     },
                     MenuActionStates::eMAView_lastAction_whichCanBeDisabled);
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMAView_georefInputData});
        }
        break;

    //case e_MainAppState::eMAS_editingProject_editingStackedProfilBoxes:
    case e_MainAppState::eMAS_editingProject_editStackedProfilIsPossible:
        _menuActionStates.setState_on({
          //MenuActionStates::eMAProject_new,
          //MenuActionStates::eMAProject_open,
          MenuActionStates::eMAProject_save,
          MenuActionStates::eMAProject_saveAs,
          MenuActionStates::eMAProject_close});
        _menuStates.setState_on({MenuStates::eMenu_file,
                                 //MenuStates::eMenu_route, //@#LP editing trace in project not available for now
                                 MenuStates::eMenu_stackedProfilBoxes,
                                 MenuStates::eMenu_computation,
                                 MenuStates::eMenu_stackedProfils});
        _menuStates.setState_off({MenuStates::eMenu_routeSet});

        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_stackedProfils,
            {MenuActionStates::eMAComp_SetParametersAndRun,
             MenuActionStates::eMAComp_EditStackedProfils,
             //MenuActionStates::eMAComp_exportCondensedResultsToAscii,
             //MenuActionStates::eMAComp_exportCondensedResultsToJson,
             //MenuActionStates::eMAComp_exportCompleteResultsToAscii,
             //MenuActionStates::eMAComp_exportCompleteResultsToJson,
             MenuActionStates::eMAComp_exportResult,
             MenuActionStates::eMAComp_setSpatialResolution},
             MenuActionStates::eMAComp_lastAction_whichCanBeDisabled);

        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
                    MenuStates::eMenu_view,
                    {MenuActionStates::eMAView_imageView,
                     MenuActionStates::eMAView_imageOverview,
                     MenuActionStates::eMAView_projectInputFiles,
                     MenuActionStates::eMAView_georefInputData,
                     MenuActionStates::eMAView_stackedProfilesGraphicsMeaning,
                     },
                     MenuActionStates::eMAView_lastAction_whichCanBeDisabled);
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMAView_georefInputData});
        }
        break;

    case e_MainAppState::eMAS_editingProject_settingComputation:
        qDebug() << __FUNCTION__ << "eMAS_editingProject_settingComputation";
        _menuStates.setState_off({MenuStates::eMenu_routeSet});

        _menuStates.setState_on({MenuStates::eMenu_file,
                                 MenuStates::eMenu_routeInsideProject,
                                 MenuStates::eMenu_stackedProfilBoxes,
                                 MenuStates::eMenu_computation});

        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_file,
            {//MenuActionStates::eMAProject_new,
            //MenuActionStates::eMAProject_open,
            MenuActionStates::eMAProject_save,
            MenuActionStates::eMAProject_saveAs,
            MenuActionStates::eMAProject_close},
            MenuActionStates::eMAProject_lastAction_whichCanBeDisabled);

        _menuActionStates.setState_on_emitForAll(
            MenuStates::eMenu_computation,
            {MenuActionStates::eMAComp_SetParametersAndRun,
             MenuActionStates::eMAComp_setSpatialResolution},
            MenuActionStates::eMAComp_lastAction_whichCanBeDisabled);

        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
                    MenuStates::eMenu_view,
                    {MenuActionStates::eMAView_imageView,
                     MenuActionStates::eMAView_imageOverview,
                     MenuActionStates::eMAView_projectInputFiles,
                     MenuActionStates::eMAView_georefInputData,
                     },
                     MenuActionStates::eMAView_lastAction_whichCanBeDisabled);
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMAView_georefInputData});
        }
        break;

    //case e_MainAppState::eMAS_editingProject_newProject_settingIO:
    //case e_MainAppState::eMAS_editingProject_openingExistingProject_fixingIO:
    //    break;

    case e_MainAppState::eMAS_editingProject_editingStackedProfil:
        _menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on_emitForAll(
                    MenuStates::eMenu_view,
                    {MenuActionStates::eMAView_imageView,
                     MenuActionStates::eMAView_imageOverview,
                     MenuActionStates::eMAView_projectInputFiles,
                     MenuActionStates::eMAView_georefInputData,
                     MenuActionStates::eMAView_stackedProfilesGraphicsMeaning,
                     },
                     MenuActionStates::eMAView_lastAction_whichCanBeDisabled);
        _menuActionStates.setState_off({MenuActionStates::eMAView_stackedProfilesGraphicsMeaning});
        if (!_bPermitsMenuActionAboutGeoinfoData) {
            _menuActionStates.setState_off({MenuActionStates::eMAView_georefInputData});
        }
        break;

    case e_MainAppState::eMAS_editingProject_AppIscomputing:
        break;

    case e_MainAppState::eMAS_editingPreferences:
        /*_menuStates.setState_on({MenuStates::eMenu_view});
        _menuActionStates.setState_on*//*_emitForAll*//*(
                   //MenuStates::eMenu_view,
                   {MenuActionStates::eMAView_imageView,
                    MenuActionStates::eMAView_imageOverview,
                    //MenuActionStates::eMAView_projectInputFiles,
                    //MenuActionStates::eMAView_spatialResolution,
                    MenuActionStates::eMAView_georefInputData,
                    }*//*,
                    MenuActionStates::eMAView_lastAction_whichCanBeDisabled*//*);*/
        break;
    }
}


MenuStates* MainAppState::ptrMenuStates() {
    return(&_menuStates);
}

void MainAppState::slot_menuActionTriggered() {

    qDebug() << __FUNCTION__;

    QObject *ptrSender = sender();
    QVariant qvariant_MAid = ptrSender->property("eMA_id");
    bool bOk = false;
    int ieMA_id = qvariant_MAid.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error with qvariant_MAid.toInt()";
        return;
    }
    qDebug() << __FUNCTION__ << "(MainAppState) eMA_id = " << ieMA_id;

    if (  (ieMA_id <= MenuActionStates::eMA_none)
        ||(ieMA_id >= MenuActionStates::eMAApp_count)) {
        qDebug() << __FUNCTION__ << " error: id for MenuActionStates not handled";
        return;
    }

    emit signal_actionFromUser(ieMA_id);
}

MenuActionStates* MainAppState::ptrMenuActionStates() {
    return(&_menuActionStates);
}

