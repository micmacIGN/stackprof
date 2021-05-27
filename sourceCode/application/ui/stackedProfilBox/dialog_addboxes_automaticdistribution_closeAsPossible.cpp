#include "dialog_addboxes_automaticdistribution_closeAsPossible.h"
#include "ui_dialog_addboxes_automaticdistribution_closeAsPossible.h"

Dialog_addBoxes_automaticDistribution_closeAsPossible::Dialog_addBoxes_automaticDistribution_closeAsPossible(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_addBoxes_automaticDistribution_closeAsPossible),
    _eDBSBuAD_selected(e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel),
    _eSPFAD_selected(e_SPFAD_firstPoint)
{
    ui->setupUi(this);

    ui->widget_stackedProfBox_editSize->setPage(Widget_stackedProfilBox_editSize::page_withoutSetButton);
    ui->widget_stackedProfBox_editSize->setEnabled(true);

    ui->pushButton_createBoxes->setEnabled(false);

    setWidgetStates_distanceBetweenSuccessivesBoxes(_eDBSBuAD_selected);
    setWidgetStates_startingPoint(_eSPFAD_selected);

    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::reject);
    connect(ui->pushButton_createBoxes, &QPushButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_pushButtonCreateBoxes_clicked);

    connect(ui->widget_stackedProfBox_editSize,&Widget_stackedProfilBox_editSize::signal_editedWidthLengthValidity,
            this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_editedWidthLengthValidity);

    connect(ui->radioButton_distanceBetweenSuccessivesBoxes_square2_interpolationMethod_none,
            &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setDistanceBetweenSuccessivesBoxes_square2);
    connect(ui->radioButton_distanceBetweenSuccessivesBoxes_square8_interpolationMethod_biLinear2x2,
            &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setDistanceBetweenSuccessivesBoxes_square8);

    connect(ui->radioButton_firstPoint, &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setStartingPoint_firstPoint);
    connect(ui->radioButton_lastPoint,  &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setStartingPoint_lastPoint);
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::setWidgetStates_startingPoint(e_StartingPointForAutomaticDistribution eSPFAD_value) {
    _eSPFAD_selected = eSPFAD_value;
    bool bFirstPoint = (_eSPFAD_selected == e_SPFAD_firstPoint);
    ui->radioButton_firstPoint->setChecked(bFirstPoint);
    ui->radioButton_lastPoint->setChecked(!bFirstPoint);
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setStartingPoint_firstPoint() {
    _eSPFAD_selected = e_SPFAD_firstPoint;
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setStartingPoint_lastPoint() {
    _eSPFAD_selected = e_SPFAD_lastPoint;
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setDistanceBetweenSuccessivesBoxes_square2() {
    _eDBSBuAD_selected = e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel;
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_setDistanceBetweenSuccessivesBoxes_square8() {
    _eDBSBuAD_selected = e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel;
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::setWidgetStates_distanceBetweenSuccessivesBoxes(e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution e_DBSBuAD_value) {
    _eDBSBuAD_selected = e_DBSBuAD_value;
    bool bSquare8_down = (_eDBSBuAD_selected == e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel);
    ui->radioButton_distanceBetweenSuccessivesBoxes_square8_interpolationMethod_biLinear2x2->setChecked(bSquare8_down);
    ui->radioButton_distanceBetweenSuccessivesBoxes_square2_interpolationMethod_none->setChecked(!bSquare8_down);
}

void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_editedWidthLengthValidity(bool bValid) {
    ui->pushButton_createBoxes->setEnabled(bValid);
}


void Dialog_addBoxes_automaticDistribution_closeAsPossible::slot_pushButtonCreateBoxes_clicked() {
    int editedBoxWidth = -1;
    int editedBoxlength = -1;

    ui->widget_stackedProfBox_editSize->get_widthLength(editedBoxWidth, editedBoxlength);

    accept();
    emit signal_createBoxes_closeAsPossible(editedBoxWidth, editedBoxlength, _eSPFAD_selected, _eDBSBuAD_selected);
}

Dialog_addBoxes_automaticDistribution_closeAsPossible::~Dialog_addBoxes_automaticDistribution_closeAsPossible() {
    delete ui;
}
