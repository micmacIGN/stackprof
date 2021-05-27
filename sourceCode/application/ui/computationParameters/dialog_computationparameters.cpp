#include <QDebug>
#include <QPushButton>

#include "../../logic/model/core/ComputationCore_structures.h"

#include "dialog_computationparameters.h"
#include "ui_dialog_computationparameters.h"

Dialog_computationParameters::Dialog_computationParameters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_computationParameters),
    _qvectb_layersToCompute {false, false, false} {
    ui->setupUi(this);

    feedWithEmptyDefault();

    ui->radioButton_mean->setProperty  ("str_eBaseComputationMethod", e_BCM_mean);
    ui->radioButton_median->setProperty("str_eBaseComputationMethod", e_BCM_median);

    ui->radioButton_interpolationMethod_none->setProperty       ("str_ePixelExtractionMethod", e_PEM_NearestPixel);
    ui->radioButton_interpolationMethod_biLinear2x2->setProperty("str_ePixelExtractionMethod", e_PEM_BilinearInterpolation2x2);

    connect(ui->radioButton_interpolationMethod_none       , &QRadioButton::toggled, this, &Dialog_computationParameters::slot_radioButton_interpolationMethod_toggled);
    connect(ui->radioButton_interpolationMethod_biLinear2x2, &QRadioButton::toggled, this, &Dialog_computationParameters::slot_radioButton_interpolationMethod_toggled);

    connect(ui->radioButton_mean,   &QRadioButton::toggled, this, &Dialog_computationParameters::slot_radioButton_baseComputationMethod_toggled);
    connect(ui->radioButton_median, &QRadioButton::toggled, this, &Dialog_computationParameters::slot_radioButton_baseComputationMethod_toggled);

    //ui->buttonBox->setVisible(false);
    //connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok), &QPushButton::clicked, this, &Dialog_computationParameters::slot_pushButtonOK_clicked);

    connect(ui->pushButton_runComputation, &QPushButton::clicked,
            this, &Dialog_computationParameters::slot_pushButton_runComputation_clicked);
    connect(ui->pushButton_cancel, &QPushButton::clicked,
            this, &Dialog_computationParameters::slot_pushButton_cancel_clicked);

    connect(ui->widget_correlScoreMap_PX1PX2_Usage, &Widget_correlScoreMapParameters::signal_rejectionValue_validityChanged,
            this, &Dialog_computationParameters::slot_rejectionValue_validityChanged);

    connect(ui->widget_correlScoreMap_deltaZorOther_Usage, &Widget_correlScoreMapParameters::signal_rejectionValue_validityChanged,
            this, &Dialog_computationParameters::slot_rejectionValue_validityChanged);

    _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ.fill(nullptr, eLA_LayersCount);
    _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ[eLA_CorrelScoreForPX1PX2Together] = ui->widget_correlScoreMap_PX1PX2_Usage;
    _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ[eLA_CorrelScoreForPX2Alone] = ui->widget_correlScoreMap_PX1PX2_Usage; //dev prevent error
    _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ[eLA_CorrelScoreForDeltaZAlone] = ui->widget_correlScoreMap_deltaZorOther_Usage;


}


void Dialog_computationParameters::slot_rejectionValue_validityChanged() {
    setEnable_windowPushButtonOK(evaluateIfParametersValid());
}


bool Dialog_computationParameters::evaluateIfParametersValid() {

    bool bRequieredFieldsAreValid = true;
    for (int ieLA = eLA_CorrelScoreForPX1PX2Together; ieLA <= eLA_CorrelScoreForDeltaZAlone; ieLA++) {
        if (_qvectb_layersToCompute[ieLA]) {
            bool bRejectionValueValid { false};
            bRequieredFieldsAreValid &= _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ[ieLA]->inputValid(bRejectionValueValid);
            if (!bRequieredFieldsAreValid) {
                return(false);
            }
        }
        if (ieLA == eLA_CorrelScoreForPX1PX2Together) {
            ieLA = eLA_CorrelScoreForDeltaZAlone;
        }
    }
    return(true);
}



void Dialog_computationParameters::setEnable_windowPushButtonOK(bool bNewState) {
    //(ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(bNewState);
    ui->pushButton_runComputation->setEnabled(bNewState);
}


void Dialog_computationParameters::slot_radioButton_interpolationMethod_toggled(bool bChecked) {

    if (!bChecked) {
        return;
    }

    QObject *ptrSender = sender();

    QVariant qvariant_str_ePixelExtractionMethod = ptrSender->property("str_ePixelExtractionMethod");
    bool bOk = false;
    int i_ePixelExtractionMethod = qvariant_str_ePixelExtractionMethod.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_str_ePixelExtractionMethod.toInt()";
        return;
    }

    if (  (i_ePixelExtractionMethod != e_PEM_NearestPixel)
        &&(i_ePixelExtractionMethod != e_PEM_BilinearInterpolation2x2)) {
        qDebug() << __FUNCTION__ << " error: invalid i_ePixelExtractionMethod ( " << i_ePixelExtractionMethod << " )";
        return;
    }

    _computationParameters_inEdition._ePixelExtractionMethod = static_cast<e_PixelExtractionMethod>(i_ePixelExtractionMethod);
}

void Dialog_computationParameters::slot_radioButton_baseComputationMethod_toggled(bool bChecked) {

    if (!bChecked) {
        return;
    }

    QObject *ptrSender = sender();

    QVariant qvariant_str_eBaseComputationMethod = ptrSender->property("str_eBaseComputationMethod");
    bool bOk = false;
    int i_eBaseComputationMethod = qvariant_str_eBaseComputationMethod.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_str_eBaseComputationMethod.toInt()";
        return;
    }

    if (  (i_eBaseComputationMethod != e_BCM_mean)
        &&(i_eBaseComputationMethod != e_BCM_median)) {
        qDebug() << __FUNCTION__ << " error: invalid i_eComputationMethod ( " << i_eBaseComputationMethod << " )";
        return;
    }

    _computationParameters_inEdition._eBaseComputationMethod = static_cast<e_BaseComputationMethod>(i_eBaseComputationMethod);

}


S_ComputationParameters Dialog_computationParameters::get_computationParameters_edited() {
    return(_computationParameters_inEdition);
}

void Dialog_computationParameters::slot_setComputationParameters(S_ComputationParameters computationParameters,
                                   const U_CorrelationScoreMapFileUseFlags& uCorrelationScoreUsageFlags,
                                   QVector<QString> qvectCorrelationScoreMapFile_PX1_PX2_DeltaZ) {

    qDebug() << __FUNCTION__ << "(Dialog_computationParameters): now call computationParameters.debugShow() on received computationParameters";
    computationParameters.debugShow();
    qDebug() << __FUNCTION__ << "(Dialog_computationParameters): after computationParameters.debugShow() ";

    qDebug() << __FUNCTION__ << "_sCSF_ ... ._b_onPX1PX2 = " << uCorrelationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2;
    qDebug() << __FUNCTION__ << "_sCSF_ ... ._b_onDeltaZ = " << uCorrelationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ;

    feed(computationParameters, uCorrelationScoreUsageFlags, qvectCorrelationScoreMapFile_PX1_PX2_DeltaZ);
}

void Dialog_computationParameters::feed(
    const S_ComputationParameters& computationParameters,
    const U_CorrelationScoreMapFileUseFlags& uCorrelationScoreUsageFlags,
    const QVector<QString>& qvectStr_CorrelationScoreMapFileUsed_PX1_PX2_DeltaZ) {

    _computationParameters_received = computationParameters;
    _computationParameters_inEdition = _computationParameters_received;

    //dev case error:
    if (  (_computationParameters_received._ePixelExtractionMethod != e_PEM_NearestPixel)
        &&(_computationParameters_received._ePixelExtractionMethod != e_PEM_BilinearInterpolation2x2)) {
        ui->groupBox_interpolationMethod->setEnabled(false);
        ui->radioButton_interpolationMethod_none->setChecked(false);
        ui->radioButton_interpolationMethod_biLinear2x2->setChecked(false);

    } else {        
         ui->groupBox_interpolationMethod->setEnabled(true);
         bool bNearestPixel = (_computationParameters_received._ePixelExtractionMethod == e_PEM_NearestPixel);
         ui->radioButton_interpolationMethod_none->setChecked(bNearestPixel);
         ui->radioButton_interpolationMethod_biLinear2x2->setChecked(!bNearestPixel);
    }

    //dev case error:
    if (  (_computationParameters_received._eBaseComputationMethod != e_BCM_mean)
        &&(_computationParameters_received._eBaseComputationMethod != e_BCM_median)) {
        ui->groupBox_stackingProfilMethod->setEnabled(false);
        ui->radioButton_mean->setChecked(false);
        ui->radioButton_median->setChecked(false);

    } else {
         ui->groupBox_stackingProfilMethod->setEnabled(true);
         bool bMean = (_computationParameters_received._eBaseComputationMethod == e_BCM_mean);
         ui->radioButton_mean->setChecked(bMean);
         ui->radioButton_median->setChecked(!bMean);
    }

    bool bCorrScoreMapAvailable_forPX1PX2 = uCorrelationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2;
    ui->groupBox_PX1PX2->setVisible(bCorrScoreMapAvailable_forPX1PX2);
    ui->widget_correlScoreMap_PX1PX2_Usage->feed(bCorrScoreMapAvailable_forPX1PX2,
                                                 qvectStr_CorrelationScoreMapFileUsed_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together],
                                                 _computationParameters_received._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]);

    bool bCorrScoreMapAvailable_forDeltaZorOther = uCorrelationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ;
    ui->groupBox_deltaZorOther->setVisible(bCorrScoreMapAvailable_forDeltaZorOther);
    ui->widget_correlScoreMap_deltaZorOther_Usage->feed(bCorrScoreMapAvailable_forDeltaZorOther,
                                                 qvectStr_CorrelationScoreMapFileUsed_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone],
                                                 _computationParameters_received._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);

    qDebug() << __FUNCTION__ << " bCorrScoreMapAvailable_forPX1PX2        = " << bCorrScoreMapAvailable_forPX1PX2;
    qDebug() << __FUNCTION__ << " bCorrScoreMapAvailable_forDeltaZorOther = " << bCorrScoreMapAvailable_forDeltaZorOther;

    _qvectb_layersToCompute[eLA_PX1] = bCorrScoreMapAvailable_forPX1PX2;
    _qvectb_layersToCompute[eLA_PX2] = bCorrScoreMapAvailable_forPX1PX2;
    _qvectb_layersToCompute[eLA_deltaZ] = bCorrScoreMapAvailable_forDeltaZorOther;

}


void Dialog_computationParameters::feedWithEmptyDefault() {

    _computationParameters_received.clear();
    _computationParameters_inEdition = _computationParameters_received;

    ui->groupBox_interpolationMethod->setEnabled(false);
    ui->radioButton_interpolationMethod_none->setChecked(false);
    ui->radioButton_interpolationMethod_biLinear2x2->setChecked(false);

    //ui->label_warningMessageAboutAutomaticDistribution->clear();

    ui->groupBox_stackingProfilMethod->setEnabled(false);
    ui->radioButton_mean->setChecked(false);
    ui->radioButton_median->setChecked(false);

    ui->widget_correlScoreMap_PX1PX2_Usage       ->feed(false,"",_computationParameters_received._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]);
    ui->widget_correlScoreMap_deltaZorOther_Usage->feed(false,"",_computationParameters_received._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);

}

#include <QMessageBox>
//void Dialog_computationParameters::slot_pushButtonOK_clicked() {
void Dialog_computationParameters::slot_pushButton_runComputation_clicked() {

    bool bGotFor_PX1PX2 = ui->widget_correlScoreMap_PX1PX2_Usage->get_correlScoreMapParameters(
        _computationParameters_inEdition._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]);

    bool bGotFor_DeltaZAlone =ui->widget_correlScoreMap_deltaZorOther_Usage->get_correlScoreMapParameters(
       _computationParameters_inEdition._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);

    if ((!bGotFor_PX1PX2) || (!bGotFor_DeltaZAlone)) {
        QMessageBox msgBoxError(QMessageBox::Information,
                                "Computation parameters",
                                "fail to get parameters about correlation score map usage");
        msgBoxError.exec();
        reject();
    }

    qDebug() << __FUNCTION__ << "(Dialog_computationParameters)";
    _computationParameters_inEdition.debugShow();

    accept();
}


void Dialog_computationParameters::slot_pushButton_cancel_clicked() {
    reject();
}

Dialog_computationParameters::~Dialog_computationParameters() {
    delete ui;
}
