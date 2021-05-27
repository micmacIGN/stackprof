#include <QDebug>
#include <QPushButton>

#include "dialog_geoRefInfos.h"
#include "ui_dialog_georefinfos.h"

#include "widgetGeoRefInfos.h"


Dialog_geoRefInfos::Dialog_geoRefInfos(/*const QString& strDialogTitle,*/ QWidget *parent) :
    QDialog(parent),

    ui(new Ui::Dialog_georefinfos) {

    if (!ui) {
        return;
    }

    ui->setupUi(this);

    //setWindowTitle(strDialogTitle);

    connect(ui->wdgtGeoRefInfos, &WidgetGeoRefInfos::signal_setEPSGCode,
                           this, &Dialog_geoRefInfos::slot_setEPSGCodeReceivedFromWidgetGeoRefInfos);

    connect(ui->wdgtGeoRefInfos, &WidgetGeoRefInfos::signal_EPSGCodelineEditChanged,
                           this, &Dialog_geoRefInfos::slot_disablePushButtonOk); //to enable or disable the OK button of any surrounding QDialog

    connect(ui->wdgtGeoRefInfos, &WidgetGeoRefInfos::signal_request_EPSGCodeSelectionFromlist,
                           this, &Dialog_geoRefInfos::slot_request_EPSGCodeSelectionFromlist);

    _bEnableState_pushButtonOK = false;
    setEditMode(e_gRIUIEditMode_newTraceSetOrProject);

}

//default state is EPSG Code editable
void Dialog_geoRefInfos::
clearUiContent_all_backToDefaultState() {

    _bEnableState_pushButtonOK = false;
    setEditMode(e_gRIUIEditMode_newTraceSetOrProject);

    ui->wdgtGeoRefInfos->clearUiContent_all();
}


//prevent any dev error. This is a shield in case of missing disabled Set button in case of activated readonly.
void Dialog_geoRefInfos::slot_setEPSGCodeReceivedFromWidgetGeoRefInfos(QString strEPSGCode) {
   if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_viewOnlyOnOpenedValidProject) {
        return;
   }

   emit signal_setEPSGCode(strEPSGCode);
}

//@LP bCancelButtonAvailable is used at false:
//- when editing the EPSG code on a opened trace (with a valid EPSG code already set)
//- when viewing the georef datas on a opened project
//It use a specific flag to be able to have the button available for new trace and new projet editing cases
void Dialog_geoRefInfos::setEditMode(e_geoRefInfos_uiEditMode eGeoRefInfos_uiEditMode) {

    _eGeoRefInfos_uiEditMode = eGeoRefInfos_uiEditMode;

    bool bViewValuesOnly = (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_viewOnlyOnOpenedValidProject);

    ui->wdgtGeoRefInfos->clearUiContent_all();
    ui->wdgtGeoRefInfos->setModeToViewValuesOnly(bViewValuesOnly);

    if (bViewValuesOnly) {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setVisible(false);
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
        return;
    }

    if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_newTraceSetOrProject) {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setVisible(true);
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setEnabled(true);

        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(_bEnableState_pushButtonOK);
        return;
    }

    //e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setEnabled(true);

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
    return;
}

void Dialog_geoRefInfos::slot_disablePushButtonOk() {
    if (!ui) {
        return;
    }
    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(false);

    if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet) {
        (ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel))->setEnabled(true);
    }
}

#include <QCloseEvent>

void Dialog_geoRefInfos::closeEvent(QCloseEvent *event) {
    if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet) {
        if (_bEnableState_pushButtonOK) {
            QDialog::accept();
        } else {
            QDialog::reject();
        }
        event->accept();
        return;
    }
    event->accept();
}

void Dialog_geoRefInfos::reject() {
    if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet) {
        if (_bEnableState_pushButtonOK) {
            QDialog::accept();
        } else {
            QDialog::reject();
        }
        return;
    }
    QDialog::reject();
}

/*Dialog_geoRefInfos::Dialog_geoRefInfos(QWidget *parent): Dialog_geoRefInfos("", parent) {

}*/

void Dialog_geoRefInfos::slot_setWorldFileDataFromStr(QVector<QString> qvectstr_worldFileParameters) {

    if (!ui) {
        return;
    }
    ui->wdgtGeoRefInfos->setWorldFileDataFromStr(qvectstr_worldFileParameters);
}

void Dialog_geoRefInfos::slot_setEPSGCodeFromStr(QString strEPSGCode) {
    if (!ui) {
        return;
    }
    ui->wdgtGeoRefInfos->setEPSGCodeFromStr(strEPSGCode);
}


void Dialog_geoRefInfos::slot_setImagesFilename(QVector<QString> qvectstr_ImageFilenames_threeMax) {
    if (!ui) {
        return;
    }
    ui->wdgtGeoRefInfos->setImagesFilename(qvectstr_ImageFilenames_threeMax);
}


void Dialog_geoRefInfos::slot_setStrGeoRefImagesSetStatus(
    bool bWorldFileData_available, bool bEPSG_available,
    QString strMsgAboutTFW,  QString strMsgAboutTFW_errorDetails,
    QString strMsgAboutEPSG, QString strMsgAboutEPSG_errorDetails) {

    if (!ui) {
        return;
    }

    ui->wdgtGeoRefInfos->setStrGeoRefImagesSetStatus(strMsgAboutTFW,  strMsgAboutTFW_errorDetails,
                                                     strMsgAboutEPSG, strMsgAboutEPSG_errorDetails);

    ui->wdgtGeoRefInfos->setWidgetTextAndStyleAboutStatus(bWorldFileData_available, bEPSG_available);

    qDebug() << __FUNCTION__ << "bWorldFileData_available = " << bWorldFileData_available;
    qDebug() << __FUNCTION__ << "bEPSG_available = " << bEPSG_available;

    _bEnableState_pushButtonOK = bWorldFileData_available && bEPSG_available;

    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(_bEnableState_pushButtonOK);

    if (_eGeoRefInfos_uiEditMode == e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet) {
        (ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel))->setEnabled(!_bEnableState_pushButtonOK);
    }

}

Dialog_geoRefInfos::~Dialog_geoRefInfos() {
    delete ui;
}

void Dialog_geoRefInfos::slot_request_EPSGCodeSelectionFromlist() {
    qDebug() << __FUNCTION__ << "(Dialog_geoRefInfos)";
    emit signal_request_EPSGCodeSelectionFromlist();
}

void Dialog_geoRefInfos::slot_associatedNameToEPSGCode(QString strAssociatedNameToEPSGCode) {
    qDebug() << __FUNCTION__ << "(Dialog_geoRefInfos)";
    ui->wdgtGeoRefInfos->set_associatedNameToEPSGCode(strAssociatedNameToEPSGCode);
}

//we want that the user validates the EPSG code that he selected from the list
void Dialog_geoRefInfos::setEPSGCodeAsEditedText_UserValidationRequiered(const QString& strEPSGCode) {
    qDebug() << __FUNCTION__ << "(Dialog_geoRefInfos)";
    ui->wdgtGeoRefInfos->setEPSGCodeAsEditedText_UserValidationRequiered(strEPSGCode);
}

