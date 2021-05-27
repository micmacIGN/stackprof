#include <QDebug>
#include <QPushButton>
#include <QItemSelection>

#include "dialog_selectroutefromrouteset.h"
#include "ui_dialog_selectroutefromrouteset.h"

#include "../logic/model/core/routeContainer.h"

Dialog_selectRouteFromRouteset::Dialog_selectRouteFromRouteset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_selectRouteFromRouteset),
    _routeContainerPtr(nullptr),
    _selectedRouteTableViewRow(-1)
{
    ui->setupUi(this);

    setWindowTitle("Select the trace to import to project");
    setWidgetsStates_noFileUsed_noRoute();
}

void Dialog_selectRouteFromRouteset::setWidgetsStates_noFileUsed_noRoute() {

    if (!ui) { return; }

    ui->lineEdit_routesetFile->clear();

    ui->lineEdit_routesetFile->setEnabled(false);

    ui->tableView->setEnabled(false);

    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(false);
    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel))->setFocus();

}

void Dialog_selectRouteFromRouteset::setModelPtrFortableView(RouteContainer *routeContainerPtr) {
    _routeContainerPtr = routeContainerPtr;
}

Dialog_selectRouteFromRouteset::~Dialog_selectRouteFromRouteset() {
    delete ui;
}

void Dialog_selectRouteFromRouteset::set_routesetFileSelected(const QString& strRoutesetFilename) {

    _selectedRouteTableViewRow = -1;

    ui->lineEdit_routesetFile->setText(strRoutesetFilename);
    bool bIsEmty = strRoutesetFilename.isEmpty();
    ui->lineEdit_routesetFile->setEnabled(!bIsEmty);
    ui->tableView->setEnabled(!bIsEmty);
    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel))->setFocus();
}

void Dialog_selectRouteFromRouteset::set_selectedRouteId(int preSelectedRouteId_forRouteImport) {

    if (!_routeContainerPtr) {
        return;
    }

    qDebug() << __FUNCTION__ << "(Dialog_selectRouteFromRouteset) preSelectedRouteId_forRouteImport = " << preSelectedRouteId_forRouteImport;
    qDebug() << __FUNCTION__ << "(Dialog_selectRouteFromRouteset) _routeContainerPtr->routeCount() = " << _routeContainerPtr->routeCount();

    if (  (preSelectedRouteId_forRouteImport >= 0)
        &&(preSelectedRouteId_forRouteImport < _routeContainerPtr->routeCount())) {

        _selectedRouteTableViewRow = preSelectedRouteId_forRouteImport;
        ui->tableView->selectRow(_selectedRouteTableViewRow);

        (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(true);

    } else {
        _selectedRouteTableViewRow = -1;
        qDebug() << __FUNCTION__ << "(Dialog_selectRouteFromRouteset) ui->tableView->model()->rowCount() = " << ui->tableView->model()->rowCount();

        ui->tableView->clearSelection();//tableview_unselectAll();
        //ui->tableView->selectionModel()->clearSelection();

        (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(false);
    }

}

/*
void Dialog_selectRouteFromRouteset::tableview_unselectAll() {

    if (!_routeContainerPtr) {
        return;
    }

    QModelIndex topLeft;
    QModelIndex bottomRight;

    _routeContainerPtr->getModelIndex_TL_BR(topLeft, bottomRight);


    QItemSelection qiSelection;
    qiSelection.select(topLeft, bottomRight);

    ui->tableView->selectionModel()->select(qiSelection, QItemSelectionModel::Clear);
}
*/

int Dialog_selectRouteFromRouteset::getSelectedRouteId() {
    return(_selectedRouteTableViewRow);
}

void Dialog_selectRouteFromRouteset::initConnectionModelViewSignalsSlots() {

    qDebug() << __FUNCTION__ << "(Dialog_selectRouteFromRouteset)";

    if (!ui) { return; }

     qDebug() << __FUNCTION__ << "_routeContainerPtr =" << (void*)_routeContainerPtr;

    ui->tableView->setModel(_routeContainerPtr);

    int widthFirstColumn  = 43;//32;
    int widthSecondColumn = 86;
    ui->tableView->setColumnWidth(0,widthFirstColumn);
    ui->tableView->setColumnWidth(1,widthSecondColumn);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect( ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                                        this, &Dialog_selectRouteFromRouteset::slot_selectionChanged);
}

void Dialog_selectRouteFromRouteset::slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    qDebug() << __FUNCTION__ << "(Dialog_geoCRSSelect_tableView)" << "selected.indexes() = " << selected.indexes();

    if (!selected.indexes().count()) {
        _selectedRouteTableViewRow = -1;
    } else {
        _selectedRouteTableViewRow = selected.indexes().first().row();
   }

    if (_selectedRouteTableViewRow != -1) {
        (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(true);
    }
}
