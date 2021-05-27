#include <QDebug>
#include <QPushButton>

#include "dialog_micmacstepvalueandspatialresolution.h"
#include "ui_dialog_micmacstepvalueandspatialresolution.h"

#include "../logic/model/core/ComputationCore_structures.h" //@#LP for layer access (move it on a separated header)

Dialog_MicMacStepValueAndSpatialResolution::Dialog_MicMacStepValueAndSpatialResolution(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_MicMacStepValueAndSpatialResolution),
    _widgetPtr_PX1_PX2_deltaZ {nullptr, nullptr, nullptr},
    _qvectb_layersToCompute {false, false, false}
{
    ui->setupUi(this);

    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(false);

    ui->widget_PX1->setFileTitlemode(e_uSFM_smallTitle);
    ui->widget_PX1->setFileTitle("PX1");

    ui->widget_PX2->setFileTitlemode(e_uSFM_smallTitle);
    ui->widget_PX2->setFileTitle("PX2");

    ui->widget_deltaZorOther->setFileTitlemode(e_uSFM_noTitle);

    _widgetPtr_PX1_PX2_deltaZ[eLA_PX1] = ui->widget_PX1;
    _widgetPtr_PX1_PX2_deltaZ[eLA_PX2] = ui->widget_PX2;
    _widgetPtr_PX1_PX2_deltaZ[eLA_deltaZ] = ui->widget_deltaZorOther;

    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        connect(_widgetPtr_PX1_PX2_deltaZ[ieLA], &Widget_MicMacStepValueAndSpatialResolution::signal_lineEdit_aboutMicMacStepOrSpatialResolution_validityChanged,
                this, &Dialog_MicMacStepValueAndSpatialResolution::slot_aboutLineEdit_validityChanged);
    }
}

bool Dialog_MicMacStepValueAndSpatialResolution::initLayersmicMacStepValueAndSpatialResolution(
        const QVector<QString>& qvectStr_micMacStepValue_PX1_PX2_DeltaZ,
        const QVector<QString>& qvectStr_spatialResolution_PX1_PX2_DeltaZ,
        const QVector<bool>& qvectb_layersToCompute) {

    qDebug() << __FUNCTION__;

    if (qvectb_layersToCompute.size()!=3) {
        return(false);
    }
    int countTrue = qvectb_layersToCompute.count(true);
    if (!countTrue) {
        return(false);
    }

    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        qDebug() << __FUNCTION__ << "qvectb_layersToCompute[ " << ieLA << " ] =" << qvectb_layersToCompute[ieLA];
        if (qvectb_layersToCompute[ieLA]) {
            qDebug() << __FUNCTION__ << "qvectStr_micMacStepValue_PX1_PX2_DeltaZ[ieLA] =" << qvectStr_micMacStepValue_PX1_PX2_DeltaZ[ieLA];
            qDebug() << __FUNCTION__ << "qvectStr_spatialResolution_PX1_PX2_DeltaZ[ieLA] =" << qvectStr_spatialResolution_PX1_PX2_DeltaZ[ieLA];

            _widgetPtr_PX1_PX2_deltaZ[ieLA]->setFromQStr_micMacStepAndSpatialResolution(
                        qvectStr_micMacStepValue_PX1_PX2_DeltaZ[ieLA],
                        qvectStr_spatialResolution_PX1_PX2_DeltaZ[ieLA]);
        } else {
            _widgetPtr_PX1_PX2_deltaZ[ieLA]->setToNoValues();
        }
    }
    setEnable_windowPushButtonOK(evaluateIfParametersValid());
    return(true);
}


bool Dialog_MicMacStepValueAndSpatialResolution::initLayersFilenameAndSetVisibility(
            const QVector<QString>& qvectStr_inputFile_PX1_PX2_DeltaZ,
            const QVector<bool>& qvectb_layersToCompute) {

    _qvectb_layersToCompute.fill(false, 3);

    if (qvectb_layersToCompute.size()!=3) {
        return(false);
    }
    int countTrue = qvectb_layersToCompute.count(true);
    if (!countTrue) {
        return(false);
    }

    //considers only the PX1_PX2_Together main mode
    if (qvectb_layersToCompute[eLA_PX1] != qvectb_layersToCompute[eLA_PX2]) {
        return(false);
    }

    _qvectb_layersToCompute = qvectb_layersToCompute;

    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        _widgetPtr_PX1_PX2_deltaZ[ieLA]->setToNoValues();
        if (_qvectb_layersToCompute[ieLA]) {            
            _widgetPtr_PX1_PX2_deltaZ[ieLA]->setFilename(qvectStr_inputFile_PX1_PX2_DeltaZ[ieLA]);
        }
    }

    //considers only the PX1_PX2_Together, deltaZ alone, main mode
    ui->groupBox_PX1PX2->setVisible(_qvectb_layersToCompute[eLA_PX1]);
    ui->groupBox_DeltaZorOther->setVisible(_qvectb_layersToCompute[eLA_deltaZ]);

    return(true);
}

bool Dialog_MicMacStepValueAndSpatialResolution::get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
        QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
        QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ) const {

    qDebug() << __FUNCTION__;

    qvect_micmacStep_PX1_PX2_deltaZ.fill(1.0, 3);
    qvect_spatialResolution_PX1_PX2_deltaZ.fill(1.0, 3);

    bool bGotAll = true;
    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        if (_qvectb_layersToCompute[ieLA]) {
            bGotAll &= _widgetPtr_PX1_PX2_deltaZ[ieLA]->get_micMacStep(qvect_micmacStep_PX1_PX2_deltaZ[ieLA]);
            bGotAll &= _widgetPtr_PX1_PX2_deltaZ[ieLA]->get_spatialResolution(qvect_spatialResolution_PX1_PX2_deltaZ[ieLA]);
            if (!bGotAll) {
                //@#LP error msg
                return(false);
            }
        }
    }
    return(true);
}

bool Dialog_MicMacStepValueAndSpatialResolution::get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ_asString(
        QVector<QString>& qvectStr_micmacStep_PX1_PX2_deltaZ,
        QVector<QString>& qvectStr_spatialResolution_PX1_PX2_deltaZ) const {

    qvectStr_micmacStep_PX1_PX2_deltaZ.fill("1", 3);
    qvectStr_spatialResolution_PX1_PX2_deltaZ.fill("1", 3);


    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        if (_qvectb_layersToCompute[ieLA]) {
            _widgetPtr_PX1_PX2_deltaZ[ieLA]->get_micMacStep_asString(qvectStr_micmacStep_PX1_PX2_deltaZ[ieLA]);
            _widgetPtr_PX1_PX2_deltaZ[ieLA]->get_spatialResolution_asString(qvectStr_spatialResolution_PX1_PX2_deltaZ[ieLA]);

        }
    }
    return(true);
}


void Dialog_MicMacStepValueAndSpatialResolution::slot_aboutLineEdit_validityChanged() {
    setEnable_windowPushButtonOK(evaluateIfParametersValid());
}

void Dialog_MicMacStepValueAndSpatialResolution::setEnable_windowPushButtonOK(bool bNewState) {
    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(bNewState);
}

bool Dialog_MicMacStepValueAndSpatialResolution::evaluateIfParametersValid() {

    bool bRequieredFieldsAreValid = true;
    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        if (_qvectb_layersToCompute[ieLA]) {
            bool bMicmacStepValid { false };
            bool bSpatialResolutionValid { false};
            bRequieredFieldsAreValid &= _widgetPtr_PX1_PX2_deltaZ[ieLA]->inputValid(bMicmacStepValid, bSpatialResolutionValid);
            if (!bRequieredFieldsAreValid) {
                return(false);
            }
        }
    }
    return(true);
}


Dialog_MicMacStepValueAndSpatialResolution::~Dialog_MicMacStepValueAndSpatialResolution() {
    delete ui;
}

