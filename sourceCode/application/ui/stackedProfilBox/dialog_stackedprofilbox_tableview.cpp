#include <QDebug>

#include "dialog_stackedprofilbox_tableview.h"
#include "ui_dialog_stackedprofilbox_tableview.h"

#include "../logic/model/core/ComputationCore_inheritQATableModel.h"

Dialog_stackedProfilBox_tableView::Dialog_stackedProfilBox_tableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_stackedProfilBox_tableView), _computationCore_iqaTableModel_ptr(nullptr) {
    ui->setupUi(this);
}

Dialog_stackedProfilBox_tableView::~Dialog_stackedProfilBox_tableView() {
    delete ui;
}

void Dialog_stackedProfilBox_tableView::setModelsPtr_stackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr) {
    _computationCore_iqaTableModel_ptr = computationCore_iqaTableModel_ptr;
}

void Dialog_stackedProfilBox_tableView::initConnectionModelViewSignalsSlots() {

    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView)";

    if (!ui) {
        qDebug() << __FUNCTION__ << " error: ui is nullptr";
        return;
    }

    ui->tableView->setModel(_computationCore_iqaTableModel_ptr);

}

