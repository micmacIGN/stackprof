#ifndef MAINWINDOWMENU_H
#define MAINWINDOWMENU_H

#include <QMainWindow>

#include "model/MenuActionStates.hpp"

#include "controller/AppState_enum.hpp"

#include "logic/model/geoUiInfos/GeoUiInfos.hpp"

namespace Ui {
    class MainWindowMenu;
}

class MenuActionStates;
class MenuStates;

class MainAppState;


class MainWindowMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowMenu(QWidget *parent = nullptr);
    ~MainWindowMenu();

    void setModelPtrForMainMenuActionStates(MenuActionStates *menuActionStatesPtr);

    void setModelPtrForMainMenuStates(MenuStates *menuStatesPtr);

    void setModelPtrForMainAppState(MainAppState *mainAppStatePtr);

    void initConnectionModelViewSignalsSlots();

    void set_menuActionAboutProjectEdition_editingRouteAsEditing(e_routeEditionState eRouteEditionState);

    void closeEvent(QCloseEvent *event);

    //void setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode);

signals:
    void signal_mainWindow_closeEvent();

public slots:
    void slot_setEnabledMenuAction(unsigned short int ieMenuAction, bool bEnabled);
    void slot_setEnabledMenu(unsigned short int ieMenu, bool bEnabled);

private:
    Ui::MainWindowMenu *ui;

    MenuActionStates *_menuActionStatesPtr;
    MenuStates *_menuStatesPtr;
    MainAppState * _mainAppStatePtr;

    QHash<unsigned short int, QAction*> _qhash_ieMA_qActionPtr;
    QHash<unsigned short int, QMenu*> _qhash_ieM_qMenuPtr;

};


#endif // MAINWINDOWMENU_H
