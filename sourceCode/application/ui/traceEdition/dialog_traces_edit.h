#ifndef DIALOG_TRACES_EDIT_H
#define DIALOG_TRACES_EDIT_H

#include <QDialog>
#include <QItemSelection>

namespace Ui {
class Dialog_traces_edit;
}

class RouteContainer;
class ApplicationController;

#include "controller/AppState_enum.hpp"

class Dialog_traces_edit : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_traces_edit(QWidget *parent = nullptr);
    ~Dialog_traces_edit();

    void setModelPtrFortableView(RouteContainer *routeContainerPtr);
    void initConnectionModelViewSignalsSlots();


    void setControllerPtr(ApplicationController *appControllerPtr);
    void initConnectionViewControllerSignalsSlots();

public slots:

    //ui to  ui
    void slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slot_addTrace();
    void slot_removeTrace();

    void slot_editOrLockClicked();
    void slot_rename();

    void slot_checkBox_showAlone_stateChanged(int iCheckState);

    //from model:
    void slot_errorFromModel(QString strError);
    void slot_routeAddedSuccessfully(int idRouteAdded);
    void slot_routeRemovedSuccessfully(int routeCountAfterRemove, int idRouteRemoved);

    void slot_routeCountAfterFeed(int routeCount);

signals:
    //to controller, which will forward to model:
    void signal_addNewRoute(QString strRouteName);
    void signal_removeRoute(int selectedRouteTableViewRow);
    void signal_routeEditionStateChanged(int ieRouteEditionState, int routeId);
    void signal_renameRoute(int routeId, QString strRouteName);

    void signal_route_showAlone(int routeId, bool bShowAlone);

    void signal_selectionChangedInTableView(int selectedRouteTableViewRow);

private:
    void setWidgetsStates_noRoute();
    void setWidgetsStates_someRoute();
    void setStateAndWidgetState_routeEditOrLock(e_routeEditionState eRES_newState);

    Ui::Dialog_traces_edit *ui;

    RouteContainer *_routeContainerPtr;
    int _selectedRouteTableViewRow;
    e_routeEditionState _eRES_selectedRoute;

    ApplicationController *_ptrAppController;

    bool _bShowAlone;

    bool _bSendSignalToUi;
};

#endif // DIALOG_TRACES_EDIT_H
