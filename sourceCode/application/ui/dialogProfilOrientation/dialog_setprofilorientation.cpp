#include <QDebug>

#include "dialog_setprofilorientation.h"
#include "ui_dialog_setprofilorientation.h"

#include "../logic/model/core/ComputationCore_structures.h"

Dialog_setProfilOrientation::Dialog_setProfilOrientation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_setProfilOrientation),
    _eProfilOrientation(e_PO_notSet)
{
    ui->setupUi(this);

    setRadioButtonsCheckedStatesFromProfilOrientation(_eProfilOrientation);

    connect(ui->radioButton_firstToLast, &QRadioButton::clicked, this, &Dialog_setProfilOrientation::slot_radioButton_firstToLast_clicked);
    connect(ui->radioButton_lastToFirst, &QRadioButton::clicked, this, &Dialog_setProfilOrientation::slot_radioButton_lastToFirst_clicked);

}

void Dialog_setProfilOrientation::setProfilOrientation(e_ProfilOrientation eProfilOrientation) {
    _eProfilOrientation = eProfilOrientation;
    setRadioButtonsCheckedStatesFromProfilOrientation(_eProfilOrientation);
}

void Dialog_setProfilOrientation::slot_radioButton_firstToLast_clicked() {
    _eProfilOrientation = e_PO_progressDirectionIsFromFirstToLastPointOfRoute;
}

void Dialog_setProfilOrientation::slot_radioButton_lastToFirst_clicked() {
  _eProfilOrientation = e_PO_progressDirectionIsFromLastToFirstPointOfRoute;
}

e_ProfilOrientation Dialog_setProfilOrientation::getProfilOrientation() {
    return(_eProfilOrientation);
}

void Dialog_setProfilOrientation::setRadioButtonsCheckedStatesFromProfilOrientation(e_ProfilOrientation eProfilOrientation) {

    qDebug() << __FUNCTION__ << "eProfilOrientation = " << eProfilOrientation;

    ui->radioButton_firstToLast->setChecked(false);
    ui->radioButton_lastToFirst->setChecked(false);

    switch (eProfilOrientation) {

     case e_PO_progressDirectionIsFromFirstToLastPointOfRoute:
        ui->radioButton_firstToLast->setChecked(true);
        break;

    case e_PO_progressDirectionIsFromLastToFirstPointOfRoute:
        ui->radioButton_lastToFirst->setChecked(true);
        break;

    case e_PO_notSet:
        break;
    }
}

Dialog_setProfilOrientation::~Dialog_setProfilOrientation() {
    delete ui;
}
