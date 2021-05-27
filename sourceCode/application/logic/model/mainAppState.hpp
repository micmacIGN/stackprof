#ifndef MainAppState_H
#define MainAppState_H

#include "MenuActionStates.hpp"
#include "MenuStates.hpp"

class MainAppState : public QObject {

    Q_OBJECT

public:

  enum e_MainAppState {
    eMAS_idle,

    eMAS_editingRouteSet,
    eMAS_editingRouteSet_editingRoute,
    eMAS_editingRouteSet_modifyingEPSGCode,

    eMAS_editingProject_idle,
    eMAS_editingProject_idle_noneRouteOrInvalidRouteToAddBoxes,
    eMAS_editingProject_idle_noneStackedProfilsBoxes,
    eMAS_editingProject_newProject_settingIO,
    eMAS_editingProject_openingExistingProject_fixingIO,

    eMAS_editingProject_editingRouteInsideProject,
    eMAS_editingProject_settingComputation,


    eMAS_editingProject_editStackedProfilIsPossible,
    eMAS_editingProject_editingStackedProfil,

    eMAS_editingProject_AppIscomputing,

    eMAS_editingPreferences,
  };


  MainAppState(QObject *parent = nullptr);

  e_MainAppState get_eAppState();

  void setState(e_MainAppState eAppState);

  void permitsMenuActionAboutGeoinfoData(bool bPermits);

  void setEnable_menu_editingRouteInsideProject(bool bState);

  MenuActionStates* ptrMenuActionStates();
  MenuStates* ptrMenuStates();

  bool isAboutRouteSet() {
    if (  (_eAppState == eMAS_editingRouteSet)
        ||(_eAppState == eMAS_editingRouteSet_editingRoute)) {
        return(true);
    }
    return(false);
  }

  bool isAboutProject() {
      if (isAboutRouteSet()) {
          return(false);
      }
      return(_eAppState != eMAS_idle);
  }

public slots:
  void slot_menuActionTriggered();

signals:
  void signal_actionFromUser(int ieMA_id);

private:
  e_MainAppState _eAppState;
  bool _bPermitsMenuActionAboutGeoinfoData;

  MenuActionStates _menuActionStates;
  MenuStates _menuStates;

};

enum e_AppModelState {

    eAModS_idle = MainAppState::eMAS_idle,

    //eAModS_editingRouteSet = MainAppState::eMAS_editingRouteSet,
    eAModS_editingRouteSet_editingRoute = MainAppState::
      eMAS_editingRouteSet_editingRoute,

    eAModS_editingProject_idle      = MainAppState::
      eMAS_editingProject_idle,

    //eAModS_editingProject_newProject_settingIO = MainAppState::
    //  eMAS_editingProject_newProject_settingIO,

    //eAModS_editingProject_openingExistingProjet_fixingIO = MainAppState::
    //  eMAS_editingProject_openingExistingProject_fixingIO,

    eAModS_editingProject_settingComputation        = MainAppState::
      eMAS_editingProject_settingComputation,

    /*eAModS_editingProject_editingStackedProfilBoxes = MainAppState::
      eMAS_editingProject_editingStackedProfilBoxes,*/

    eAModS_editingProjecteditStackedProfilIsPossible = MainAppState::
      eMAS_editingProject_editStackedProfilIsPossible,

    eAModS_editingProject_editingStackedProfil       = MainAppState::
      eMAS_editingProject_editingStackedProfil,

    eAModS_editingProject_AppIscomputing = MainAppState::
      eMAS_editingProject_AppIscomputing,

    eAModS_editingProject_editingRouteInsideProject = MainAppState::
      eMAS_editingProject_editingRouteInsideProject,

    eAModS_editingPreferences = MainAppState::
      eMAS_editingPreferences
};

#endif
