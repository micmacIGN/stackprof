#include <QDebug>

#include "dialog_exportresult.h"
#include "ui_dialog_exportresult.h"

#include "../../logic/model/core/exportResult.h"

Dialog_exportResult::Dialog_exportResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_exportResult)
{
    ui->setupUi(this);

    _bUiSetLogicFields_forsetAsEmpty = true;

    setExportSettings({});

    connect(ui->radioButton_json ,&QRadioButton::toggled, this, &Dialog_exportResult::slot_radioButton_json_toggled);
    connect(ui->radioButton_ascii,&QRadioButton::toggled, this, &Dialog_exportResult::slot_radioButton_ascii_toggled);

    connect(ui->radioButton_allBoxes     , &QRadioButton::toggled, this, &Dialog_exportResult::slot_radioButton_allBoxes_toggled);
    connect(ui->radioButton_onlyCurrentBox, &QRadioButton::toggled, this, &Dialog_exportResult::slot_radioButton_onlyCurrentBox_toggled);

    connect(ui->checkBox_includeProfilesCurvesData, &QCheckBox::stateChanged, this, &Dialog_exportResult::slot_checkBox_includeProfilesCurvesData_stateChanged);

    connect(ui->pushButton_selectOutputFileAndExport, &QPushButton::clicked, this, &Dialog_exportResult::slot_selectOutputFileAndExport);
    connect(ui->pushButton_close, &QPushButton::clicked, this, &Dialog_exportResult::reject);

    connect(ui->checkBox_setLinRegResultAsEmpty    , &QCheckBox::stateChanged, this,  &Dialog_exportResult::slot_checkBox_setLinRegResultAsEmpty_stateChanged);
    connect(ui->checkBox_setProfileCurveDataAsEmpty, &QCheckBox::stateChanged, this,  &Dialog_exportResult::slot_checkBox_setProfileCurveDataAsEmpty_stateChanged);


}

void Dialog_exportResult::setExportSettings(const S_ExportSettings& sExportSettings) {

    _sExportSettings = sExportSettings;

    bool bAscii = (_sExportSettings._ieExportResult_fileType == e_ERfT_ascii);
    ui->radioButton_ascii->setChecked(bAscii);
    ui->radioButton_json->setChecked(!bAscii);

    bool bAllBoxes = (_sExportSettings._ieExportResult_boxesContent == e_ERbC_all);
    ui->radioButton_allBoxes->setChecked(bAllBoxes);
    ui->radioButton_onlyCurrentBox->setChecked(!bAllBoxes);

    ui->checkBox_includeProfilesCurvesData->setChecked(_sExportSettings._bIncludeProfilesCurvesData);

    setCurrentBoxId(_sExportSettings._idOfCurrentBox);
    //ui->checkBox_setLinRegResultAsEmpty->setChecked(_sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty);
    //ui->checkBox_setProfileCurveDataAsEmpty->setChecked(_sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty);
    //ui->checkBox_setProfileCurveDataAsEmpty->setEnabled(_sExportSettings._bIncludeProfilesCurvesData);

    setSomeFlaggedProfilExist(false, false); //@LP default set as 'none flagged', which can be unsync.
                                             //Hence, be sure to call setSomeFlaggedProfilExist later to have synced state.
}

void Dialog_exportResult::setSomeFlaggedProfilExist(bool bSomeFlaggedProfilExist_anyBoxes,
                                                    bool bSomeFlaggedProfilExistForCurrentBoxId) {
    _bSomeFlaggedProfilExist_anyBoxes = bSomeFlaggedProfilExist_anyBoxes;
    _bSomeFlaggedProfilExistForCurrentBoxId = bSomeFlaggedProfilExistForCurrentBoxId;
    updateEnableWidgetState_aboutSomeFlaggedProfilExistStates();
}

void Dialog_exportResult::updateEnableWidgetState_aboutSomeFlaggedProfilExistStates() {
    bool bEnable_groupBox_flaggedProfil = false;

    if (_sExportSettings._ieExportResult_boxesContent == e_ERbC_all) {
        bEnable_groupBox_flaggedProfil = _bSomeFlaggedProfilExist_anyBoxes;
    } else {
        bEnable_groupBox_flaggedProfil = _bSomeFlaggedProfilExistForCurrentBoxId;
    }

    ui->groupBox_flaggedProfil->setEnabled(bEnable_groupBox_flaggedProfil);

    if (!bEnable_groupBox_flaggedProfil) {
        _bUiSetLogicFields_forsetAsEmpty = false;
        ui->checkBox_setLinRegResultAsEmpty->setChecked(false);
        ui->checkBox_setProfileCurveDataAsEmpty->setChecked(false);
        _bUiSetLogicFields_forsetAsEmpty = true;
    } else {

        ui->checkBox_setLinRegResultAsEmpty->setChecked(_sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty);

        ui->checkBox_setProfileCurveDataAsEmpty->setEnabled(_sExportSettings._bIncludeProfilesCurvesData);
        if (!_sExportSettings._bIncludeProfilesCurvesData) {
            ui->checkBox_setProfileCurveDataAsEmpty->setChecked(false);
        } else {
            ui->checkBox_setProfileCurveDataAsEmpty->setChecked(_sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty);
        }
    }
}

void Dialog_exportResult::setCurrentBoxId(int currentBoxId) {

    qDebug() << __FUNCTION__ << "currentBoxId =" << currentBoxId;

    _sExportSettings._idOfCurrentBox = currentBoxId;

    QString str_onlyCurrentlySelectedBox = "Only currently selected box";
    bool bIdOfCurrentBoxIsValid = (currentBoxId != -1);
    if (!bIdOfCurrentBoxIsValid) {        
        ui->radioButton_allBoxes->setChecked(true);
        ui->radioButton_onlyCurrentBox->setChecked(false);

        ui->radioButton_onlyCurrentBox->setText(str_onlyCurrentlySelectedBox);
        ui->radioButton_onlyCurrentBox->setEnabled(false);
        return;
    }

    ui->radioButton_onlyCurrentBox->setText(str_onlyCurrentlySelectedBox + " (#"+ QString::number(_sExportSettings._idOfCurrentBox+1) + ")");
    ui->radioButton_onlyCurrentBox->setEnabled(true);
}

void Dialog_exportResult::slot_radioButton_json_toggled(bool bChecked) {
    if (!bChecked) { return; }
    _sExportSettings._ieExportResult_fileType = e_ERfT_json;
}

void Dialog_exportResult::slot_radioButton_ascii_toggled(bool bChecked) {
    if (!bChecked) { return; }
    _sExportSettings._ieExportResult_fileType = e_ERfT_ascii;
}

void Dialog_exportResult::slot_radioButton_allBoxes_toggled(bool bChecked) {
    if (!bChecked) { return; }
    _sExportSettings._ieExportResult_boxesContent = e_ERbC_all;

    _bUiSetLogicFields_forsetAsEmpty = false;
    updateEnableWidgetState_aboutSomeFlaggedProfilExistStates();
    _bUiSetLogicFields_forsetAsEmpty = true;
}

void Dialog_exportResult::slot_radioButton_onlyCurrentBox_toggled(bool bChecked) {
    if (!bChecked) {return; }
    _sExportSettings._ieExportResult_boxesContent = e_ERbC_onlyCurrent;

    _bUiSetLogicFields_forsetAsEmpty = false;
    updateEnableWidgetState_aboutSomeFlaggedProfilExistStates();
    _bUiSetLogicFields_forsetAsEmpty = true;
}

void Dialog_exportResult::slot_checkBox_includeProfilesCurvesData_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sExportSettings._bIncludeProfilesCurvesData = bChecked;

    _bUiSetLogicFields_forsetAsEmpty = false;
    updateEnableWidgetState_aboutSomeFlaggedProfilExistStates();
    _bUiSetLogicFields_forsetAsEmpty = true;
}

void Dialog_exportResult::slot_checkBox_setLinRegResultAsEmpty_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    if (_bUiSetLogicFields_forsetAsEmpty) {
        _sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty = bChecked;
    }
}

void Dialog_exportResult::slot_checkBox_setProfileCurveDataAsEmpty_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    if (_bUiSetLogicFields_forsetAsEmpty) {
        _sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty = bChecked;
    }
}

void Dialog_exportResult::slot_selectOutputFileAndExport() {
   emit signal_project_exportResult(_sExportSettings);
}

Dialog_exportResult::~Dialog_exportResult() {
    delete ui;
}
