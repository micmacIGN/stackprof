#ifndef MenuActionStates_H
#define MenuActionStates_H

#include <QObject>
#include <QSet>

#include "MenuStates.hpp"

//enable or disable MenuAction on request


class MenuActionStates : public QObject {

    Q_OBJECT

    public:
        MenuActionStates(QObject *parent = nullptr);

        enum e_menuAction {

            eMA_none,

            eMAProject_new   = MenuStates::eMenu_file,
            eMAProject_open  ,
            eMAProject_save  ,
            eMAProject_saveAs,
            eMAProject_close ,
            eMAProject_lastAction_whichCanBeDisabled = eMAProject_close,

            eMAPreferences_edit_caches = MenuStates::eMenu_preferences,
            eMAPreferences_edit_graphicsStyle,
            eMAPreferences_lastAction_whichCanBeDisabled = eMAPreferences_edit_graphicsStyle,

            eMARouteSet_newSet  = MenuStates::eMenu_routeSet,
            eMARouteSet_openSet  ,
            eMARouteSet_saveSet  ,
            eMARouteSet_saveSetAs,
            eMARouteSet_closeSet ,
            eMARouteSet_setEPSGCode,
            eMARouteSet_lastAction_whichCanBeDisabled = eMARouteSet_setEPSGCode,

            eMARouteInsideProject_editOrLock = MenuStates::eMenu_routeInsideProject,
            eMARouteInsideProject_rename,
            eMARouteInsideProject_export,
            eMARouteInsideProject_lastAction_whichCanBeDisabled = eMARouteInsideProject_export,

            eMAStackProfBoxes_addBoxes_autoDistrib = MenuStates::eMenu_stackedProfilBoxes,
            eMAStackProfBoxes_editBoxes,
            eMAStackProfBoxes_setProgressDirectionAlongTheFault,
            eMAStackProfBoxes_lastAction_whichCanBeDisabled = eMAStackProfBoxes_setProgressDirectionAlongTheFault,

            eMAComp_SetParametersAndRun = MenuStates::eMenu_computation,
            eMAComp_EditStackedProfils,            
            eMAComp_exportResult,

            eMAComp_setSpatialResolution,
            eMAComp_lastAction_whichCanBeDisabled = eMAComp_setSpatialResolution,

            eMAView_imageView = MenuStates::eMenu_view,
            eMAView_imageOverview,
            eMAView_georefInputData,
            eMAView_tracesList,
            eMAView_projectInputFiles,
            eMAView_stackedProfilesGraphicsMeaning,
            eMAView_lastAction_whichCanBeDisabled = eMAView_stackedProfilesGraphicsMeaning,

            eMAAbout_softwareInfos,

            eMAApp_exit,

            eMAApp_count
        };

  //emit only for on
  void setState_on(std::initializer_list<e_menuAction> initList_menuAction);
  //emit only for off
  void setState_off(std::initializer_list<e_menuAction> initList_menuAction);


  void setState_on_emitForAll(MenuStates::e_Menu eMenu,
                                std::initializer_list<e_menuAction> initList_menuAction,
                                e_menuAction eMenuActionLastAction);

  void setState_off_emitForAll(MenuStates::e_Menu eMenu,
                               std::initializer_list<e_menuAction> initList_menuAction,
                               e_menuAction eMenuActionLastAction);
public slots:

signals:
  void signal_setMenuActionEnabled(int ieMenuAction, bool enabled);

};

#endif

