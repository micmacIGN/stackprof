#include "dialog_addboxesautomaticdistribution_selectmethod.h"
#include "ui_dialog_addboxesautomaticdistribution_selectmethod.h"

Dialog_addBoxesAutomaticDistribution_selectMethod::Dialog_addBoxesAutomaticDistribution_selectMethod(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_addBoxesAutomaticDistribution_selectMethod),
    _eAutomaticDistributionMethod(e_ADSM_noSet)
{
    ui->setupUi(this);

    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &Dialog_addBoxesAutomaticDistribution_selectMethod::reject);
    connect(ui->pushButton_closeAsPossible, &QPushButton::clicked, this, &Dialog_addBoxesAutomaticDistribution_selectMethod::slot_closeAsPossible_clicked);
    connect(ui->pushButton_selectDistanceBetweenBoxesCenter, &QPushButton::clicked, this, &Dialog_addBoxesAutomaticDistribution_selectMethod::slot_selectDistanceBetweenBoxesCenter_clicked);

}

e_AutomaticDistributionMethod Dialog_addBoxesAutomaticDistribution_selectMethod::get_selectedMethod() {
    return(_eAutomaticDistributionMethod);
}

void Dialog_addBoxesAutomaticDistribution_selectMethod::slot_closeAsPossible_clicked() {
    _eAutomaticDistributionMethod = e_ADM_closeAsPossible;
    accept();
}


void Dialog_addBoxesAutomaticDistribution_selectMethod::slot_selectDistanceBetweenBoxesCenter_clicked() {
    _eAutomaticDistributionMethod = e_ADM_distanceBetweenCenter;
    accept();
}

Dialog_addBoxesAutomaticDistribution_selectMethod::~Dialog_addBoxesAutomaticDistribution_selectMethod() {
    delete ui;
}
