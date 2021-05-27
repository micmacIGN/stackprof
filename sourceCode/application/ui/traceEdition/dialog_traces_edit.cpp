#include <QDebug>
#include <QMessageBox>

#include "dialog_traces_edit.h"
#include "ui_dialog_traces_edit.h"

#include "../logic/model/core/routeContainer.h"

#include "../logic/controller/ApplicationController.h"

//@LP design break: direct acces to _routeContainerPtr to get infos

Dialog_traces_edit::Dialog_traces_edit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_traces_edit),
    _routeContainerPtr(nullptr),
    _selectedRouteTableViewRow(-1),
    _eRES_selectedRoute(e_rES_locked),
    _ptrAppController(nullptr)
{
    ui->setupUi(this);

    //setWindowTitle("Trace list");

    /*Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags( flags );*/

    _bSendSignalToUi = true;
    _bShowAlone = false;
    setWidgetsStates_noRoute();
}

void Dialog_traces_edit::setWidgetsStates_noRoute() {

    if (!ui) { return; }

    ui->groupBoxRoute->setTitle("");
    setStateAndWidgetState_routeEditOrLock(e_rES_locked);
    ui->groupBoxRoute->setEnabled(false);

    ui->pushButton_add->setEnabled(true);

    ui->checkBox_showAlone->setEnabled(false);
}

void Dialog_traces_edit::setStateAndWidgetState_routeEditOrLock(e_routeEditionState eRES_newState) {
    if (eRES_newState == e_rES_notApplicable) {
        return;
    }
    if (!ui) { return; }
    _eRES_selectedRoute = eRES_newState;
    QString qstrTextForState_locked_edit[3] {"N/A", "Edit", "Lock"};
    ui->pushButton_editOrLock->setText(qstrTextForState_locked_edit[_eRES_selectedRoute]);

    bool bEnableState = (_eRES_selectedRoute == e_rES_locked);
    ui->tableView->setEnabled(bEnableState);
    ui->pushButton_remove->setEnabled(bEnableState);
    ui->pushButton_add->setEnabled(bEnableState);

    //@LP: on macosx, the focus goes by default to an another button (the 'remove' button)
    //hence force to editOrLock button for better user experience:
    /*if (bEnableState) {
        ui->pushButton_editOrLock->setFocus();
    }*/
    //fixed reordering widget focus order in .ui

}

void Dialog_traces_edit::setWidgetsStates_someRoute() {

    if (!ui) { return; }

    if (_selectedRouteTableViewRow == -1) {
        ui->groupBoxRoute->setTitle("Trace #");
        setStateAndWidgetState_routeEditOrLock(e_rES_locked);
        ui->groupBoxRoute->setEnabled(false);
    } else {
        ui->groupBoxRoute->setTitle("Trace #"+QString::number(_selectedRouteTableViewRow));
        setStateAndWidgetState_routeEditOrLock(e_rES_locked);
        ui->groupBoxRoute->setEnabled(true);
    }

    int routeCount = 0;
    int nbSegmentsInFirstExistingRoute = 0;
    if (!_routeContainerPtr) {
        ui->checkBox_showAlone->setEnabled(false);
    } else {
        routeCount = _routeContainerPtr->routeCount();
        nbSegmentsInFirstExistingRoute = _routeContainerPtr->getRouteRef(0).segmentCount();

        ui->checkBox_showAlone->setEnabled( routeCount > 1 );
    }

    ui->pushButton_add->setEnabled(true);

}

void Dialog_traces_edit::setModelPtrFortableView(RouteContainer *routeContainerPtr) {
    _routeContainerPtr = routeContainerPtr;
}

void Dialog_traces_edit::setControllerPtr(ApplicationController *appControllerPtr) {
    _ptrAppController = appControllerPtr;
}

//@#LP move connections in controller
void Dialog_traces_edit::initConnectionViewControllerSignalsSlots() {
    connect(this, &Dialog_traces_edit::signal_addNewRoute, _ptrAppController, &ApplicationController::slot_routeSet_addNewRoute);
    connect(this, &Dialog_traces_edit::signal_removeRoute, _ptrAppController, &ApplicationController::slot_routeSet_removeRoute);
    connect(this, &Dialog_traces_edit::signal_routeEditionStateChanged, _ptrAppController, &ApplicationController::slot_routeSet_routeEditionStateChanged);
    connect(this, &Dialog_traces_edit::signal_renameRoute, _ptrAppController, &ApplicationController::slot_routeSet_routeEditionRenameRoute);
    connect(this, &Dialog_traces_edit::signal_route_showAlone, _ptrAppController, &ApplicationController::slot_routeSet_route_showAlone);
}


void Dialog_traces_edit::initConnectionModelViewSignalsSlots() {

    qDebug() << __FUNCTION__ << "(Dialog_traces_edit)";

    if (!ui) { return; }

     qDebug() << __FUNCTION__ << "_routeContainerPtr =" << (void*)_routeContainerPtr;

    ui->tableView->setModel(_routeContainerPtr);

    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int widthFirstColumn  = 43;//32;
    int widthSecondColumn = 86;
    ui->tableView->setColumnWidth(0,widthFirstColumn);
    ui->tableView->setColumnWidth(1,widthSecondColumn);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect( ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                                        this, &Dialog_traces_edit::slot_selectionChanged);

    connect( ui->pushButton_add   , &QPushButton::clicked, this, &Dialog_traces_edit::slot_addTrace);
    connect( ui->pushButton_remove, &QPushButton::clicked, this, &Dialog_traces_edit::slot_removeTrace);

    connect(_routeContainerPtr, &RouteContainer::signal_errorFromModel, this, &Dialog_traces_edit::slot_errorFromModel);

    connect(this, &Dialog_traces_edit::signal_selectionChangedInTableView, _routeContainerPtr, &RouteContainer::slot_selectionChangedInTableView);

    connect( ui->pushButton_editOrLock, &QPushButton::clicked, this, &Dialog_traces_edit::slot_editOrLockClicked);

    connect(ui->pushButton_rename, &QPushButton::clicked, this, &Dialog_traces_edit::slot_rename);

    connect(ui->checkBox_showAlone, &QCheckBox::stateChanged, this, &Dialog_traces_edit::slot_checkBox_showAlone_stateChanged);

}

/*
void Dialog_traces_edit::slot_routeCountAfterFeed(int routeCount) {
    qDebug() << __FUNCTION__;

    if (!routeCount) {
        _selectedRouteTableViewRow = -1;
        setWidgetsStates_noRoute();
    } else {
        _selectedRouteTableViewRow = -1;
        setWidgetsStates_someRoute();
    }

    //ui->tableView->clearSelection();
}
*/

#include "dialog_traceparameters.h"

void Dialog_traces_edit::slot_addTrace() {
    qDebug() << __FUNCTION__;

    if (!_routeContainerPtr) {
        return;
    }

    Dialog_traceParameters dial_traceParameters(_routeContainerPtr->routeCount(), this);
    int ieDialogCodeReport = dial_traceParameters.exec();

    if (ieDialogCodeReport == QDialog::Accepted) {
        emit signal_addNewRoute(dial_traceParameters.getEditedName());
    } else { //  QDialog::Rejected
        //do nothing
    }
}

void Dialog_traces_edit::slot_errorFromModel(QString strError) {
    //@#LP silent error here. to do
}

void Dialog_traces_edit::slot_routeAddedSuccessfully(int idRouteAdded) {

    ui->tableView->selectRow(idRouteAdded);
    _selectedRouteTableViewRow = idRouteAdded;
    setWidgetsStates_someRoute();
}

void Dialog_traces_edit::slot_routeRemovedSuccessfully(int routeCountAfterRemove, int newSelectedRoute) {

    //model now manage the route selection after a remove
   _selectedRouteTableViewRow = newSelectedRoute;

    if (!routeCountAfterRemove) {
        //_selectedRouteTableViewRow = -1;
        ui->tableView->clearSelection();
        setWidgetsStates_noRoute();
        return;
    }

    setWidgetsStates_someRoute();

    ui->tableView->selectRow(newSelectedRoute);

}

void Dialog_traces_edit::slot_removeTrace() {  
    qDebug() << __FUNCTION__;

    QMessageBox qMsgBox;
    qMsgBox.setWindowTitle("Removing trace");
    qMsgBox.setText("Do you confirm to remove trace #"
                    + QString::number(_selectedRouteTableViewRow)
                    + " named: "+ _routeContainerPtr->getRouteRef(_selectedRouteTableViewRow).getName()
                    + " ?");

    qMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qMsgBox.setDefaultButton(QMessageBox::No);

    int iReturnCode = qMsgBox.exec();
    qDebug() << __FUNCTION__ <<  "iReturnCode = " << iReturnCode;
    if (iReturnCode == QMessageBox::No) {
        return;
    }
    //QDialog::Accepted
    if (_selectedRouteTableViewRow != -1) {
        emit signal_removeRoute(_selectedRouteTableViewRow);
    }
}

void Dialog_traces_edit::slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    if (!selected.indexes().count()) {
        _selectedRouteTableViewRow = -1;
        setWidgetsStates_noRoute();
    } else {
        _selectedRouteTableViewRow = selected.indexes().first().row();
        setWidgetsStates_someRoute();
    }

    qDebug() << __FUNCTION__ << "(Dialog_traces_edit) _selectedRouteTableViewRow = " << _selectedRouteTableViewRow;
    emit signal_selectionChangedInTableView(_selectedRouteTableViewRow);

}


void Dialog_traces_edit::slot_editOrLockClicked() {

    e_routeEditionState eRES_newState = (_eRES_selectedRoute == e_rES_locked)?e_rES_inEdition:e_rES_locked;
    setStateAndWidgetState_routeEditOrLock(eRES_newState);       
    emit signal_routeEditionStateChanged(_eRES_selectedRoute, _selectedRouteTableViewRow);
}

void Dialog_traces_edit::slot_rename() {
    qDebug() << __FUNCTION__;

    if (!_routeContainerPtr) {
        return;
    }

    Dialog_traceParameters dial_traceParameters(_selectedRouteTableViewRow, this);
    dial_traceParameters.setRouteName(_routeContainerPtr->getRouteRef(_selectedRouteTableViewRow).getName());

    int ieDialogCodeReport = dial_traceParameters.exec();

    if (ieDialogCodeReport == QDialog::Accepted) {
        emit signal_renameRoute(_selectedRouteTableViewRow, dial_traceParameters.getEditedName());
    } else { //  QDialog::Rejected
        //do nothing
    }
}

void Dialog_traces_edit::slot_checkBox_showAlone_stateChanged(int iCheckState) {

    bool bChecked = (iCheckState == Qt::Checked);
    _bShowAlone = bChecked;

    qDebug() << __FUNCTION__;
    if (!_routeContainerPtr) {
        return;
    }

    /*if (_selectedRouteTableViewRow == -1) {
        return;
    }*/

    //permit to send -1 as _selectedRouteTableViewRow
    //to be able to show alone or not when the first selection in the tableview is made
    //(checkbox showAlone can be set by user before any selection in the tableview)

    //int routeCount = _routeContainerPtr->routeCount();
    emit signal_route_showAlone(_selectedRouteTableViewRow, _bShowAlone);
}

void Dialog_traces_edit::slot_routeCountAfterFeed(int routeCount) {

    _bSendSignalToUi = false;

    ui->checkBox_showAlone->setChecked(false);
    _bShowAlone = false;

    _bSendSignalToUi = true;

    qDebug() << __FUNCTION__;

    if (!routeCount) {
        _selectedRouteTableViewRow = -1;
        setWidgetsStates_noRoute();
    } else {
        _selectedRouteTableViewRow = -1;
        setWidgetsStates_someRoute();
    }
    //ui->tableView->clearSelection();
}


Dialog_traces_edit::~Dialog_traces_edit() {
    delete ui;
}
