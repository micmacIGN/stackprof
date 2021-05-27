#include <QFileDialog>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>

#include "../../logic/model/core/ComputationCore_structures.h" //just for e_LayersAcces //@#LP move e_LayersAcces in a dedicated header file

#include "../../logic/OsPlatform/standardPathLocation.hpp"

#include "dialog_project_inputfiles_viewContentOnly.h"
#include "ui_dialog_project_inputfiles_viewContentOnly.h"

Dialog_project_inputFiles_viewContentOnly::Dialog_project_inputFiles_viewContentOnly(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog_project_inputFiles_viewContentOnly) {

    ui->setupUi(this);

    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = ui->lineEdit_PX1PX2_correlScoreMap;
    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone   ] = ui->lineEdit_deltaZorOther_correlScoreMap;
    //should never be used:
    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX2Alone] = nullptr;

    ui->widget_PX1PX2_inputImageFiles->setSelectFilemode(e_uSFM_twoFiles);
    ui->widget_PX1PX2_inputImageFiles->setFilesTitle({"Px1", "Px2"});

    ui->widget_deltaZorOther_inputImageFiles->setSelectFilemode(e_uSFM_oneFile);

    _qvectb_layersToCompute.fill(false, 3);
    _qvectb_correlScoreCheckBoxStateWhenGroupEnabled_PX1PX2Together_DeltaZAlone.fill(false, 3);
    setWidgets_enabledStatus_for_correlScoreMap_PX1PX2(false);
    setWidgets_enabledStatus_for_correlScoreMap_deltaZOrOther(false);

    setStateAsEmptyProject();

}

void Dialog_project_inputFiles_viewContentOnly::setStateAsEmptyProject() {

    setToNoValues();

    //enable group according to layers to compute
    ui->groupBox_PX1PX2->setEnabled(false);
    ui->groupBox_DeltaZorOther->setEnabled(false);

    ui->lineEdit_PX1PX2_correlScoreMap->setEnabled(false);
    ui->lineEdit_deltaZorOther_correlScoreMap->setEnabled(false);

}


void Dialog_project_inputFiles_viewContentOnly::setEditedProject_routeName(const QString& strRouteName) {
    //feed route infos
    ui->lineEdit_routeName->setText(strRouteName);
    ui->lineEdit_routeName->setEnabled(true);
}


void Dialog_project_inputFiles_viewContentOnly::setEditedProject(const S_InputFiles& inputFiles,
                                                                               const QVector<bool>& qvectb_layersToCompute,
                                                                               const QString& strRouteName) {

    setToNoValues();

    _inputFiles = inputFiles;

    qDebug() << __FUNCTION__ << "_inputFiles = ";
    _inputFiles.showContent();

    _qvectb_layersToCompute = qvectb_layersToCompute;

    //adjusting any other project parameters than fixing missing files is forbidden
    //
    //enable group according to layers to compute
    ui->groupBox_PX1PX2->setEnabled(_qvectb_layersToCompute[eLA_PX1]);
    ui->groupBox_DeltaZorOther->setEnabled(_qvectb_layersToCompute[eLA_deltaZ]);

    //set input files:
    if (_qvectb_layersToCompute[eLA_PX1]) {
        ui->widget_PX1PX2_inputImageFiles->set_file1_file2(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1],
                                                           _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2]);
    }
    if (_qvectb_layersToCompute[eLA_deltaZ]) {
        ui->widget_deltaZorOther_inputImageFiles->set_fileAlone(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_deltaZ]);
    }

    //set text correlation score maps and enable according widgets if used:
    bool bPX1PX2_correlScoreMapUsed = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].isEmpty();
    ui->lineEdit_PX1PX2_correlScoreMap->setText(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]);
    ui->lineEdit_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);
    ui->label_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);
    ui->label_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);

    bool bDeltaZ_correlScoreMapUsed = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].isEmpty();
    ui->lineEdit_deltaZorOther_correlScoreMap->setText(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);
    ui->lineEdit_deltaZorOther_correlScoreMap->setEnabled(bDeltaZ_correlScoreMapUsed);
    ui->label_deltaZorOther_correlScoreMap->setEnabled(bDeltaZ_correlScoreMapUsed);

    setEditedProject_routeName(strRouteName);

}

void Dialog_project_inputFiles_viewContentOnly::setToNoValues() {

    //widgets part:
    ui->widget_PX1PX2_inputImageFiles->setToNoValues();
    ui->widget_deltaZorOther_inputImageFiles->setToNoValues();

    ui->lineEdit_PX1PX2_correlScoreMap->clear();
    ui->lineEdit_deltaZorOther_correlScoreMap->clear();

    setWidgets_enabledStatus_for_correlScoreMap_PX1PX2(false);
    setWidgets_enabledStatus_for_correlScoreMap_deltaZOrOther(false);

    ui->lineEdit_routeName->clear();
    ui->lineEdit_routeName->setEnabled(false);

   _inputFiles.clear();
}

void Dialog_project_inputFiles_viewContentOnly::setWidgets_enabledStatus_for_correlScoreMap_PX1PX2(bool bEnabled) {
    ui->lineEdit_PX1PX2_correlScoreMap->setEnabled(bEnabled);
    ui->label_PX1PX2_correlScoreMap->setEnabled(bEnabled);
}

void Dialog_project_inputFiles_viewContentOnly::setWidgets_enabledStatus_for_correlScoreMap_deltaZOrOther(bool bEnabled) {
    ui->label_deltaZorOther_correlScoreMap->setEnabled(bEnabled);
    ui->lineEdit_deltaZorOther_correlScoreMap->setEnabled(bEnabled);
}

Dialog_project_inputFiles_viewContentOnly::~Dialog_project_inputFiles_viewContentOnly() {
    delete ui;
}
