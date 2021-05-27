#include <QMessageBox>

#include <QDebug>

#include "widget_stackedprofilbox_edit.h"
#include "ui_widget_stackedprofilbox_edit.h"

using namespace std;
#include <cmath>

Widget_stackedprofilbox_edit::Widget_stackedprofilbox_edit(QWidget *parent) : QWidget(parent), ui(new Ui::Widget_stackedprofilbox_edit),
    _boxId_currentlySelectedBox(-1),
    _oddPixelWidth_currentlySelectedBox(0),
    _oddPixelLength_currentlySelectedBox(0),
    _bAddingBoxMode(false),
    _oddPixelWidth_forAddingBoxMode(0),
    _oddPixelLength_forAddingBoxMode(0) {

    ui->setupUi(this);
    setPage(e_stackedWidgetPage::e_sWP_adjustLocation);

    setEnabled(false);
    ui->groupBox_selectedProfil->setTitle("none box selected");

    ui->pushButton_plus1->setProperty("stepValue", 1);
    ui->pushButton_plus5->setProperty("stepValue", 5);
    ui->pushButton_plus20->setProperty("stepValue", 20);
    ui->pushButton_minus1->setProperty("stepValue", -1);
    ui->pushButton_minus5->setProperty("stepValue", -5);
    ui->pushButton_minus20->setProperty("stepValue", -20);

    _qhash_step_pushButtonptr.insert(1, ui->pushButton_plus1);
    _qhash_step_pushButtonptr.insert(5, ui->pushButton_plus5);
    _qhash_step_pushButtonptr.insert(20, ui->pushButton_plus20);
    _qhash_step_pushButtonptr.insert(-1, ui->pushButton_minus1);
    _qhash_step_pushButtonptr.insert(-5, ui->pushButton_minus5);
    _qhash_step_pushButtonptr.insert(-20, ui->pushButton_minus20);

    for (auto iter = _qhash_step_pushButtonptr.begin(); iter!= _qhash_step_pushButtonptr.end(); ++iter) {
        connect(iter.value(), &QPushButton::clicked, this, &Widget_stackedprofilbox_edit::slot_pushButton_plusMinus); //plus1);
    }

    connect(ui->pushButton_cancelAdd, &QPushButton::clicked, this, &Widget_stackedprofilbox_edit::slot_pushButton_cancelAdd_clicked);

    connect(ui->widget_stackedProfBox_editSize, &Widget_stackedProfilBox_editSize::signal_pushButton_set_clicked,
            this, &Widget_stackedprofilbox_edit::signal_pushButton_setSize_clicked);

    connect(ui->widget_stackedProfBox_editSize,&Widget_stackedProfilBox_editSize::signal_editedWidthLengthValidity,
            this, &Widget_stackedprofilbox_edit::slot_editedWidthLengthValidity);

    connect(ui->pushButton_remove, &QPushButton::clicked, this, &Widget_stackedprofilbox_edit::slot_pushButton_remove_clicked);

}


void Widget_stackedprofilbox_edit::slot_editedWidthLengthValidity(bool bValid, int oddPixelWidth, int oddPixelLength) {
    ui->label_addBox_help_selectBoxCenterPoint->setEnabled(bValid);

    if (!_bAddingBoxMode) {
        _oddPixelWidth_currentlySelectedBox = oddPixelWidth;
        _oddPixelLength_currentlySelectedBox = oddPixelLength;
    } else {
        _oddPixelWidth_forAddingBoxMode = oddPixelWidth;
        _oddPixelLength_forAddingBoxMode = oddPixelLength;
        emit signal_editedWidthLengthValidity(bValid, _oddPixelWidth_forAddingBoxMode, _oddPixelLength_forAddingBoxMode);
    }
}

void Widget_stackedprofilbox_edit::setPage(e_stackedWidgetPage eStackedWidgetPage) {
    ui->stackedWidget->setCurrentIndex(eStackedWidgetPage);

    if (eStackedWidgetPage == e_sWP_addBox) {
        _bAddingBoxMode = true;
        ui->groupBox_selectedProfil->setTitle("Add boxes");
        setEnabled(true);
        ui->widget_stackedProfBox_editSize->setPage(Widget_stackedProfilBox_editSize::page_withoutSetButton);
        ui->widget_stackedProfBox_editSize->setEnabled(true);

        if (_boxId_currentlySelectedBox == -1) {
            ui->label_addBox_help_selectBoxCenterPoint->setEnabled(false);
        } else { //adding box width and height inherit from the currently selected box

            qDebug() << __FUNCTION__ << "call now slot_editedWidthLengthValidity with:"
                     << "_oddPixelWidth_currentlySelectedBox  = " << _oddPixelWidth_currentlySelectedBox
                     << "_oddPixelLength_currentlySelectedBox = " << _oddPixelLength_currentlySelectedBox;

            slot_editedWidthLengthValidity(
                        ui->widget_stackedProfBox_editSize->getWidthLengthValidForComputation(),
                        _oddPixelWidth_currentlySelectedBox,
                        _oddPixelLength_currentlySelectedBox);
        }
    } else { //e_sWP_adjustLocation
        _bAddingBoxMode = false;

        slot_editedWidthLengthValidity(
                    ui->widget_stackedProfBox_editSize->getWidthLengthValidForComputation(),
                    _oddPixelWidth_currentlySelectedBox,
                    _oddPixelLength_currentlySelectedBox);

        ui->widget_stackedProfBox_editSize->setPage(Widget_stackedProfilBox_editSize::page_withSetButton);
        if (_boxId_currentlySelectedBox == -1) {
            ui->widget_stackedProfBox_editSize->setEnabled(false);
            setEnabled(false);
        }
    }
}

void Widget_stackedprofilbox_edit::slot_pushButton_cancelAdd_clicked(bool checked) {
    addBoxCanceled();
}

void Widget_stackedprofilbox_edit::addBoxCanceled() {
    setPage(e_sWP_adjustLocation);
    //back to previously in edition selected box:
    selectedBoxChanged(_boxId_currentlySelectedBox, _oddPixelWidth_currentlySelectedBox, _oddPixelLength_currentlySelectedBox);
    emit signal_addBoxCanceled();
}


void Widget_stackedprofilbox_edit::slot_pushButton_remove_clicked(bool checked) {

    QMessageBox qMsgBox;
    qMsgBox.setWindowTitle("Removing box");
    qMsgBox.setText("Do you confirm to remove box #" + QString::number(_boxId_currentlySelectedBox+1) + " ?");
    //qMsgBox.setInformativeText(strQuestion);
    //qMsgBox.setDetailedText(...);
    qMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qMsgBox.setDefaultButton(QMessageBox::No);

    int iReturnCode = qMsgBox.exec();

    qDebug() << __FUNCTION__ <<  "iReturnCode = " << iReturnCode;
    if (iReturnCode == QMessageBox::No) {
        return;
    }
    //QDialog::Accepted

    int _boxId_toRemove = _boxId_currentlySelectedBox;

    //@LP set to no box, waiting new selection from logic
    selectedBoxChanged(-1, _oddPixelWidth_currentlySelectedBox, _oddPixelLength_currentlySelectedBox);

    emit signal_removeBoxFromVectId(_boxId_toRemove);

}

void Widget_stackedprofilbox_edit::slot_pushButton_plusMinus(bool checked) {
    QObject *ptrSender = sender();
    QVariant qvariant_stepValue = ptrSender->property("stepValue");
    bool bOk = false;
    int shift = qvariant_stepValue.toInt(&bOk);
    if (!bOk) {
        return;
    }
    qDebug() << __FUNCTION__ << "shift = " << shift;
    emit signal_moveCenterBoxUsingShiftFromCurrentLocation(_boxId_currentlySelectedBox, shift);
}

Widget_stackedprofilbox_edit::~Widget_stackedprofilbox_edit() {
    delete ui;
}

void Widget_stackedprofilbox_edit::/*slot_*/selectedBoxChanged(int boxId, int oddPixelWidth, int oddPixelLength) {
    _boxId_currentlySelectedBox = boxId;
    _oddPixelWidth_currentlySelectedBox = oddPixelWidth;
    _oddPixelLength_currentlySelectedBox = oddPixelLength;

    if (_boxId_currentlySelectedBox == -1) {
        setEnabled(false);
        ui->groupBox_selectedProfil->setTitle("none box selected");
        ui->widget_stackedProfBox_editSize->setNoValues();
        return;
    }

    ui->groupBox_selectedProfil->setTitle("Box #"+ QString::number(_boxId_currentlySelectedBox+1));
    setEnabled(true);
    ui->widget_stackedProfBox_editSize->set_widthLength(oddPixelWidth, oddPixelLength);
    ui->widget_stackedProfBox_editSize->setEnabled(true);
}

void Widget_stackedprofilbox_edit::boxCenterPossibleShiftsToView(const QHash<int, bool>& qhash_shift_bPossibleShift) {    
    for (auto iter = qhash_shift_bPossibleShift.begin();
         iter != qhash_shift_bPossibleShift.end();
         ++iter) {
        auto iterPushButtonForShift = _qhash_step_pushButtonptr.find(iter.key());
        if (iterPushButtonForShift != _qhash_step_pushButtonptr.end()) {
            iterPushButtonForShift.value()->setEnabled(iter.value());
        }
    }
}
