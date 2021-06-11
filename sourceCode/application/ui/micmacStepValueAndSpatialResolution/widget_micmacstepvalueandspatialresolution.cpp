#include <QDebug>

#include "../../logic/toolbox/toolbox_conversion.h"

#include "widget_micmacstepvalueandspatialresolution.h"
#include "ui_widget_micmacstepvalueandspatialresolution.h"

Widget_MicMacStepValueAndSpatialResolution::Widget_MicMacStepValueAndSpatialResolution(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_MicMacStepValueAndSpatialResolution),
    _eUiFileTitleMode(e_uSFM_noTitle),
    _qlineEditPtr_micMacStep(nullptr),
    _qlineEditPtr_spatialResolution(nullptr),
    _bMicMacStepValid(false),
    _bSpatialResolutionValid(false) {

    ui->setupUi(this);

    _qlineEditPtr_micMacStep  = ui->lineEdit_micmacStepValue;
    _qlineEditPtr_spatialResolution  = ui->lineEdit_spatialResolution;

    connect(_qlineEditPtr_micMacStep, &QLineEdit::textEdited,
            this, &Widget_MicMacStepValueAndSpatialResolution::slot_lineEdit_micMacStep_textEdited);

    connect(_qlineEditPtr_spatialResolution , &QLineEdit::textEdited,
            this, &Widget_MicMacStepValueAndSpatialResolution::slot_lineEdit_spatialResolution_textEdited);

    setToNoValues();
    setFileTitlemode(_eUiFileTitleMode);

}

void Widget_MicMacStepValueAndSpatialResolution::slot_lineEdit_micMacStep_textEdited(const QString &text) {
    qDebug() << __FUNCTION__;

    bool bValidityChanged = updateFieldAndWidget(text, _qlineEditPtr_micMacStep, _qstrMicMacStep, _bMicMacStepValid);
    if (bValidityChanged) {
        signal_lineEdit_aboutMicMacStepOrSpatialResolution_validityChanged(/*bMatch*/);
    }
}

void Widget_MicMacStepValueAndSpatialResolution::slot_lineEdit_spatialResolution_textEdited(const QString &text) {
    qDebug() << __FUNCTION__;

    bool bValidityChanged = updateFieldAndWidget(text, _qlineEditPtr_spatialResolution, _qstrSpatialResolution, _bSpatialResolutionValid);
    if (bValidityChanged) {
        signal_lineEdit_aboutMicMacStepOrSpatialResolution_validityChanged(/*bMatch*/);
    }
}

//return true if the validy has changed
bool Widget_MicMacStepValueAndSpatialResolution::updateFieldAndWidget(const QString &text, QLineEdit *qlineEditPtr, QString &qstr, bool &bValid) {

    qDebug() << __FUNCTION__;

    bool bMatch = string_isDecimalDigitsOnly(text);
    if (bMatch) {
        bMatch = string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(text, 7);
    }
    if (bMatch) {
        bMatch = stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(text, 3, 4);
    }

    bool bValidityBefore = bValid;
    bValid = bMatch;

    if (bMatch) {
       qlineEditPtr->setStyleSheet("");
       qstr = qlineEditPtr->text();
    } else {
       qlineEditPtr->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
       qstr.clear();
    }

    if (bMatch != bValidityBefore) {
        return(true);
    }
    return(false);
}

void Widget_MicMacStepValueAndSpatialResolution::setToNoValues() {

    qDebug() << __FUNCTION__ << "(Widget_MicMacStepValueAndSpatialResolution)"
                ;
    if (!ui) {
        return;
    }

    ui->lineEdit_file->clear();
    ui->lineEdit_fileWST->clear();

    _qstrMicMacStep = "1.0";
    _qstrSpatialResolution = "1.0";

    _qlineEditPtr_micMacStep->setText(_qstrMicMacStep);
    _qlineEditPtr_spatialResolution->setText(_qstrSpatialResolution);

    _bMicMacStepValid = true;
    _bSpatialResolutionValid = true;
}

void Widget_MicMacStepValueAndSpatialResolution::setFileTitlemode(e_UiFileTitleMode eUiFileTitleMode) {
    _eUiFileTitleMode = eUiFileTitleMode;
    e_Page_wFileTitle ePage_wFileTitle = e_Page_wFileTitle::page_fileNoTitle;
    if (_eUiFileTitleMode == e_uSFM_smallTitle) {
        ePage_wFileTitle = e_Page_wFileTitle::page_fileWithSmallTitle;
    }
    setCurrentIndex(ePage_wFileTitle);
}

void Widget_MicMacStepValueAndSpatialResolution::setCurrentIndex(e_Page_wFileTitle ePage_wFileTitle) {
    if (!ui) {
        return;
    }
    ui->stackedWidget->setCurrentIndex(ePage_wFileTitle);
}

void Widget_MicMacStepValueAndSpatialResolution::setFileTitle(const QString& qStrFileTitle) {
    ui->label_fileSmallTitle->setText(qStrFileTitle);
}

void Widget_MicMacStepValueAndSpatialResolution::setFilename(const QString& qStrFilename) {
    switch (_eUiFileTitleMode) {
        case e_uSFM_noTitle   : ui->lineEdit_file->setText(qStrFilename); break;
        case e_uSFM_smallTitle: ui->lineEdit_fileWST->setText(qStrFilename); break;
    }
}

bool Widget_MicMacStepValueAndSpatialResolution::inputValid(bool& bMicMacStepValid, bool& bSpatialResolutionValid) {
    bMicMacStepValid = _bMicMacStepValid;
    bSpatialResolutionValid = _bSpatialResolutionValid;
    return(bMicMacStepValid && bSpatialResolutionValid);
}

void Widget_MicMacStepValueAndSpatialResolution::setFromQStr_micMacStepAndSpatialResolution(
        const QString& qstrMicMacStep,
        const QString& qstrSpatialResolution) {

    ui->lineEdit_micmacStepValue->setText(qstrMicMacStep);
    ui->lineEdit_spatialResolution->setText(qstrSpatialResolution);

    slot_lineEdit_micMacStep_textEdited(qstrMicMacStep);
    slot_lineEdit_spatialResolution_textEdited(qstrSpatialResolution);
}

//QString getStr_stepDequantizationStep();
bool Widget_MicMacStepValueAndSpatialResolution::get_micMacStep(float& fMicMacStep) const {
    bool bOk = false;
    fMicMacStep = _qstrMicMacStep.toFloat(&bOk);

    qDebug() << __FUNCTION__ << "fMicMacStep = " << fMicMacStep;
    return(bOk);
}

bool Widget_MicMacStepValueAndSpatialResolution::get_spatialResolution(float& fSpatialResolution) const {
    bool bOk = false;
    fSpatialResolution = _qstrSpatialResolution.toFloat(&bOk);

    qDebug() << __FUNCTION__ << "fSpatialResolution = " << fSpatialResolution;
    return(bOk);
}

bool Widget_MicMacStepValueAndSpatialResolution::get_micMacStep_asString(QString& qstrMicMacStep) const {
    qstrMicMacStep = _qstrMicMacStep;
    return(true);
}

bool Widget_MicMacStepValueAndSpatialResolution::get_spatialResolution_asString(QString& qstrSpatialResolution) const {
    qstrSpatialResolution = _qstrSpatialResolution;
    return(true);
}

Widget_MicMacStepValueAndSpatialResolution::~Widget_MicMacStepValueAndSpatialResolution() {
    delete ui;
}

