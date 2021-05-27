#include <QDebug>
#include <QString>

#include "dialog_addboxes_automaticdistribution_distanceBetweenCenter.h"
#include "ui_dialog_addboxes_automaticdistribution_distanceBetweenCenter.h"

Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::Dialog_addBoxes_automaticDistribution_distanceBetweenCenter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_addBoxes_automaticDistribution_distanceBetweenCenter),
    _pixelDistanceBetweenCenter(0),//0 as invalid
    _bPixelDistanceBetweenCenterValid(false),
    _bWidthLengthValidForComputation(false),

    _eSPFAD_selected(e_SPFAD_firstPoint)
{
    ui->setupUi(this);

    ui->widget_stackedProfBox_editSize->setPage(Widget_stackedProfilBox_editSize::page_withoutSetButton);
    ui->widget_stackedProfBox_editSize->setEnabled(true);


    setWidgetStates_startingPoint(_eSPFAD_selected);

    updateEnableState_of_setPushButton(false);

    ui->widget_stackedProfBox_editSize->setFocus();//@LP trying to fix default focus set on distance (third line edit field)

    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::reject);
    connect(ui->pushButton_createBoxes, &QPushButton::clicked, this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_pushButtonCreateBoxes_clicked);

    connect(ui->widget_stackedProfBox_editSize,&Widget_stackedProfilBox_editSize::signal_editedWidthLengthValidity,
            this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_editedWidthLengthValidity);

    connect(ui->lineEdit_distanceBetweenCenters,&QLineEdit::textEdited,
            this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_distanceBetweenCenters_edited);

    connect(ui->radioButton_firstPoint, &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_setStartingPoint_firstPoint);
    connect(ui->radioButton_lastPoint,  &QRadioButton::clicked, this, &Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_setStartingPoint_lastPoint);

}


void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_distanceBetweenCenters_edited(const QString &qstrText) {

    qDebug() << __FUNCTION__ << "qstrText = " << qstrText;

     bool bFieldEmpty = qstrText.isEmpty();
     if (bFieldEmpty) {
        _pixelDistanceBetweenCenter = 0;
        _bPixelDistanceBetweenCenterValid = false;
     } else {
        bool bOk = false;
        _pixelDistanceBetweenCenter = qstrText.toInt(&bOk);
        if (!bOk) {
            _pixelDistanceBetweenCenter = 0;
            _bPixelDistanceBetweenCenterValid = false;
        } else {
            if (_pixelDistanceBetweenCenter < 1) {
                _pixelDistanceBetweenCenter = 0;
                _bPixelDistanceBetweenCenterValid = false;
            } else {
                _bPixelDistanceBetweenCenterValid = true;
            }
        }
     }
     distanceBetweenCenters_edited();
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::distanceBetweenCenters_edited() {
    if (_bPixelDistanceBetweenCenterValid) {

        ui->lineEdit_distanceBetweenCenters->setStyleSheet("");
   } else {
       ui->lineEdit_distanceBetweenCenters->setStyleSheet("QLineEdit { color : red; }");
   }
   updateEnableState_of_setPushButton(_bWidthLengthValidForComputation && _bPixelDistanceBetweenCenterValid);
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::setWidgetStates_startingPoint(e_StartingPointForAutomaticDistribution eSPFAD_value) {
    _eSPFAD_selected = eSPFAD_value;
    bool bFirstPoint = (_eSPFAD_selected == e_SPFAD_firstPoint);
    ui->radioButton_firstPoint->setChecked(bFirstPoint);
    ui->radioButton_lastPoint->setChecked(!bFirstPoint);
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_setStartingPoint_firstPoint() {
    _eSPFAD_selected = e_SPFAD_firstPoint;
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_setStartingPoint_lastPoint() {
    _eSPFAD_selected = e_SPFAD_lastPoint;
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_editedWidthLengthValidity(bool bValid) {
    _bWidthLengthValidForComputation = bValid;
    updateEnableState_of_setPushButton(_bWidthLengthValidForComputation && _bPixelDistanceBetweenCenterValid);
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::updateEnableState_of_setPushButton(bool bEnable) {
    ui->pushButton_createBoxes->setEnabled(bEnable);
}

void Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::slot_pushButtonCreateBoxes_clicked() {
    int editedBoxWidth = -1;
    int editedBoxlength = -1;

    ui->widget_stackedProfBox_editSize->get_widthLength(editedBoxWidth, editedBoxlength);

    accept();
    emit signal_createBoxes_withDistanceBetweenCenter(editedBoxWidth, editedBoxlength, _eSPFAD_selected, _pixelDistanceBetweenCenter);
}

Dialog_addBoxes_automaticDistribution_distanceBetweenCenter::~Dialog_addBoxes_automaticDistribution_distanceBetweenCenter() {
    delete ui;
}
