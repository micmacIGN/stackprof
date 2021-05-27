#include "dialog_profilcurvesmeanings.h"
#include "ui_dialog_profilcurvesmeanings.h"

Dialog_profilCurvesMeanings::Dialog_profilCurvesMeanings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_profilCurvesMeanings)
{
    ui->setupUi(this);

    connect(ui->pButton_closeWindow, &QPushButton::clicked, this, &Dialog_profilCurvesMeanings::accept);
}

void Dialog_profilCurvesMeanings::setToNoValues() {
    setToNoValues_units();
    setToNoValues_meaning();
}

void Dialog_profilCurvesMeanings::setToNoValues_units() {

    ui->label_unitValue_x->setEnabled(false);
    ui->label_unitValue_x->clear();

    ui->label_unitValue_y->setEnabled(false);
    ui->label_unitValue_y->clear();

    ui->label_titleUnits->setEnabled(false);
    ui->label_titleX->setEnabled(false);
    ui->label_titleY->setEnabled(false);
}


void Dialog_profilCurvesMeanings::setToNoValues_meaning() {

    ui->label_meaning_curve_perpParal->setText("N/A");
    ui->label_meaning_enveloppeArea_perpParal->setText("N/A");

    ui->label_meaning_curve_deltaZOther->setText("N/A");
    ui->label_meaning_enveloppeArea_deltaZOther->setText("N/A");

    ui->label_titlePerpParal->setEnabled(false);
    ui->label_titleDeltZOther->setEnabled(false);

    ui->label_meaning_curve_perpParal->setEnabled(false);
    ui->label_meaning_enveloppeArea_perpParal->setEnabled(false);

    ui->label_meaning_curve_deltaZOther->setEnabled(false);
    ui->label_meaning_enveloppeArea_deltaZOther->setEnabled(false);
}


void Dialog_profilCurvesMeanings::setUnits(const QVector<QString>& qvectstr_xyunits) {

    setToNoValues_units();
    if (qvectstr_xyunits.size() != 2) {
        return;
    }

    ui->label_unitValue_x->setEnabled(true);
    ui->label_unitValue_x->setText(qvectstr_xyunits[0]); //@#LP use x/y enum/define/... instead of 0/1

    ui->label_unitValue_y->setEnabled(true);
    ui->label_unitValue_y->setText(qvectstr_xyunits[1]); //@#LP use x/y enum/define/... instead of 0/1

    ui->label_titleUnits->setEnabled(true);
    ui->label_titleX->setEnabled(true);
    ui->label_titleY->setEnabled(true);
}

#include "../../logic/model/core/ComputationCore_structures.h"
#include <vector>
using namespace std;

void Dialog_profilCurvesMeanings::setMeanings(const QVector<bool>& qvectBool_componentToCompute,
                                              const QVector<QString>& qvectstr_curveMeaning,
                                              const QVector<QString>& qvectstr_enveloppeAreaMeaning) {

    setToNoValues_meaning();

    if (  (qvectBool_componentToCompute.size() != 3)
        ||(qvectstr_curveMeaning.size() != 3)
        ||(qvectstr_enveloppeAreaMeaning.size() != 3)) {
        setToNoValues();
        return;
    }

    if (qvectBool_componentToCompute[e_CA_Perp]) {
        ui->label_meaning_curve_perpParal        ->setText(qvectstr_curveMeaning[e_CA_Perp]);
        ui->label_meaning_enveloppeArea_perpParal->setText(qvectstr_enveloppeAreaMeaning[e_CA_Perp]);

        ui->label_meaning_curve_perpParal        ->setEnabled(true);
        ui->label_meaning_enveloppeArea_perpParal->setEnabled(true);

        ui->label_titlePerpParal->setEnabled(true);
    }

    if (qvectBool_componentToCompute[e_CA_deltaZ]) {

        ui->label_meaning_curve_deltaZOther        ->setText(qvectstr_curveMeaning[e_CA_deltaZ]);
        ui->label_meaning_enveloppeArea_deltaZOther->setText(qvectstr_enveloppeAreaMeaning[e_CA_deltaZ]);

        ui->label_meaning_curve_deltaZOther        ->setEnabled(true);
        ui->label_meaning_enveloppeArea_deltaZOther->setEnabled(true);

        ui->label_titleDeltZOther->setEnabled(true);
    }

}

Dialog_profilCurvesMeanings::~Dialog_profilCurvesMeanings() {
    delete ui;
}
