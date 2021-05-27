#include <QMessageBox>

#include "dialog_stackedProfilBoxes.h"
#include "ui_dialog_stackedProfilBoxes.h"

#include "../logic/model/core/ComputationCore_inheritQATableModel.h"

#include "widget_stackedprofilbox_edit.h"

#include <qevent.h>

Dialog_stackedProfilBoxes::Dialog_stackedProfilBoxes(QWidget *parent):  QDialog(parent),
    ui(new Ui::Dialog_stackedProfilBoxes),
    _computationCore_iqaTableModel_ptr(nullptr),
    _bOneOrMoreBoxExist(false),
    _bAddingBoxMode(false),
    _selected_vectBoxId(-1),
    _bAddingBoxModePossible(true) {

    ui->setupUi(this);

    setPage(e_spb_sWP_withBoxesList);

    //empty state leads to none box to remove
    setEnable_buttonRemoveAllBoxes(false);

    connect(ui->widget_stackedProfBox_edit, &Widget_stackedprofilbox_edit::signal_pushButton_setSize_clicked,
            this, &Dialog_stackedProfilBoxes::slot_pushButton_setSize_clicked);

    connect(ui->widget_stackedProfBox_edit, &Widget_stackedprofilbox_edit::signal_moveCenterBoxUsingShiftFromCurrentLocation,
            this, &Dialog_stackedProfilBoxes::slot_moveCenterBoxUsingShiftFromCurrentLocation);

    connect(ui->pushButton_addBox, &QPushButton::clicked,
            this, &Dialog_stackedProfilBoxes::slot_pushButton_addBox_clicked);

    connect(ui->widget_stackedProfBox_edit, &Widget_stackedprofilbox_edit::signal_editedWidthLengthValidity,
            this, &Dialog_stackedProfilBoxes::slot_editedWidthLengthValidity);

    connect(ui->widget_stackedProfBox_edit, &Widget_stackedprofilbox_edit::signal_addBoxCanceled,
            this, &Dialog_stackedProfilBoxes::slot_addingBoxModeCanceled);

    connect(ui->widget_stackedProfBox_edit, &Widget_stackedprofilbox_edit::signal_removeBoxFromVectId,
            this, &Dialog_stackedProfilBoxes::slot_removeBoxFromVectId);

    connect(ui->pushButton_removeAllBoxes, &QPushButton::clicked,
            this, &Dialog_stackedProfilBoxes::slot_removeAllBoxes_clicked);
}

void Dialog_stackedProfilBoxes::setAddingBoxModePossible(bool bState) {
    _bAddingBoxModePossible = bState;
}


void Dialog_stackedProfilBoxes::backToState_noneBoxSelected() {
    _bAddingBoxMode = false;
    setPage(e_spb_sWP_withBoxesList);
    ui->widget_stackedProfBox_edit->selectedBoxChanged(-1, 3, 1); //@LP: oddPixelWidth, int oddPixelLength will not be used when using boxId as -1
}

void Dialog_stackedProfilBoxes::slot_oneOrMoreBoxesExist(bool bExist) {
    qDebug() << __FUNCTION__ << "received bExist = " << bExist;
    _bOneOrMoreBoxExist = bExist;
    if (!_bAddingBoxMode) {
        setEnable_buttonRemoveAllBoxes(_bOneOrMoreBoxExist);
    }
}

void Dialog_stackedProfilBoxes::setEnable_buttonRemoveAllBoxes(bool bEnable) {
    ui->pushButton_removeAllBoxes->setEnabled(bEnable);
}

void Dialog_stackedProfilBoxes::slot_removeAllBoxes_clicked() {

    QMessageBox qMsgBox;
    qMsgBox.setWindowTitle("Removing all boxes");
    qMsgBox.setText("Do you confirm to remove all boxes ?");
    //qMsgBox.setDetailedText(...);
    qMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qMsgBox.setDefaultButton(QMessageBox::No);

    int iReturnCode = qMsgBox.exec();
    qDebug() << __FUNCTION__ <<  "iReturnCode = " << iReturnCode;
    if (iReturnCode == QMessageBox::No) {
        return;
    }
    //QDialog::Accepted
    emit signal_removeAllBoxes();
}

void Dialog_stackedProfilBoxes::slot_removeBoxFromVectId(int boxId_toRemove) {
    emit signal_removeBoxFromVectId(boxId_toRemove);
}

void Dialog_stackedProfilBoxes::setPage(e_spb_stackedWidgetPage espb_stackedWidgetPage) {
    ui->stackedWidget_aboutBoxesList->setCurrentIndex(espb_stackedWidgetPage);
}

void Dialog_stackedProfilBoxes::slot_editedWidthLengthValidity(bool bValid, int oddPixelWidth, int oddPixelLength) {
    qDebug() << __FUNCTION__ << " bValid = " << bValid;
    emit signal_addingBoxModeActivated(bValid, oddPixelWidth, oddPixelLength);
}

void Dialog_stackedProfilBoxes::closeEvent(QCloseEvent *event) {
    if (_bAddingBoxMode) {
        ui->widget_stackedProfBox_edit->addBoxCanceled();
        slot_addingBoxModeCanceled();
    }
    event->accept();
}

void Dialog_stackedProfilBoxes::reject() {
    if (_bAddingBoxMode) {
        ui->widget_stackedProfBox_edit->addBoxCanceled();
        slot_addingBoxModeCanceled();
    }
    QDialog::reject();
}

void Dialog_stackedProfilBoxes::slot_pushButton_addBox_clicked() {

    if (!_bAddingBoxModePossible) {
        return;
    }

    _bAddingBoxMode = true;
    ui->widget_stackedProfBox_edit->setPage(Widget_stackedprofilbox_edit::e_sWP_addBox);

    ui->tableView_boxesList->setEnabled(false);
    setPage(e_spb_sWP_withoutBoxesList);

    ui->pushButton_addBox->setEnabled(false);
    ui->pushButton_removeAllBoxes->setEnabled(false);

    emit signal_enteringAddingBoxMode();
}

void Dialog_stackedProfilBoxes::set_addingBoxModeCanceled() {

    ui->widget_stackedProfBox_edit->setPage(Widget_stackedprofilbox_edit::e_sWP_adjustLocation);

    _bAddingBoxMode = false;

    ui->tableView_boxesList->setEnabled(true);
    setPage(e_spb_sWP_withBoxesList);

    ui->pushButton_addBox->setEnabled(true);
    ui->pushButton_removeAllBoxes->setEnabled(_bOneOrMoreBoxExist);
}

void Dialog_stackedProfilBoxes::slot_addingBoxModeCanceled() {

    set_addingBoxModeCanceled();
    emit signal_addingBoxModeCanceled();
}


void Dialog_stackedProfilBoxes::slot_moveCenterBoxUsingShiftFromCurrentLocation(int boxId, int shift) {
    emit signal_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(boxId, shift);
}

void Dialog_stackedProfilBoxes::slot_pushButton_setSize_clicked(int oddPixelWidth, int oddPixelLength) {
    qDebug() << __FUNCTION__ << "oddPixelWidth = "  << oddPixelWidth;
    qDebug() << __FUNCTION__ << "oddPixelLength = " << oddPixelLength;
    emit signal_userRequest_onCurrentBox_sizeChange(_selected_vectBoxId, oddPixelWidth, oddPixelLength);
}

void Dialog_stackedProfilBoxes::slot_setBoxId_and_boxSize(int boxId, int oddPixelWidth, int oddPixelLength) {

    if (boxId > -1) {
        ui->tableView_boxesList->selectRow(boxId);
    }
    _selected_vectBoxId = boxId;
    ui->widget_stackedProfBox_edit->selectedBoxChanged(boxId, oddPixelWidth, oddPixelLength); //+1 to be sync with idx displayed on Ui (and provided by the model about table view)

}

void Dialog_stackedProfilBoxes::slot_boxCenterPossibleShiftsToView(QHash<int, bool> qhash_shift_bPossibleShift) {

    ui->widget_stackedProfBox_edit->boxCenterPossibleShiftsToView(qhash_shift_bPossibleShift);

}


void Dialog_stackedProfilBoxes::setModelsPtr_stackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr) {
    _computationCore_iqaTableModel_ptr = computationCore_iqaTableModel_ptr;
}

void Dialog_stackedProfilBoxes::initConnectionModelViewSignalsSlots() {

    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView)";

    if (!ui) {
        qDebug() << __FUNCTION__ << " error: ui is nullptr";
        return;
    }

    if (ui) {
        ui->tableView_boxesList->setModel(_computationCore_iqaTableModel_ptr);
        //just show the vectboxId
        for (int i = e_StackedProfilTableViewColumn::e_SPTVC_vectIdx+1; i <= e_SPTVC_lastColumn; i++) {
            ui->tableView_boxesList->setColumnHidden(i, true);
        }

        connect( ui->tableView_boxesList->selectionModel(), &QItemSelectionModel::selectionChanged,
                        _computationCore_iqaTableModel_ptr, &ComputationCore_inheritQATableModel::selectionChanged);

    }
}

Dialog_stackedProfilBoxes::~Dialog_stackedProfilBoxes() {
    delete ui;
}

