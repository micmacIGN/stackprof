#include <QDebug>

#include "../logic/model/core/ComputationCore_structures.h"

#include "widget_correlScoreMapParameters.h"
#include "ui_widget_correlScoreMapParameters.h"

#include "../../logic/toolbox/toolbox_math.h"
#include "../../logic/toolbox/toolbox_conversion.h"

Widget_correlScoreMapParameters::Widget_correlScoreMapParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_correlScoreMapParameters),
    _bUiSetLogicFields(true) {
    ui->setupUi(this);

    _bqstrRejectionValueValid = false;

    feedWithEmptyDefault();

    connect(ui->checkBox_use, &QCheckBox::stateChanged, this, &Widget_correlScoreMapParameters::slot_checkBox_use);

    connect(ui->checkBox_useRejectionThreshold, &QCheckBox::stateChanged, this, &Widget_correlScoreMapParameters::slot_checkBox_useRejectionThreshold_stateChanged);
    connect(ui->checkBox_usedWeighting_above1,  &QCheckBox::stateChanged, this, &Widget_correlScoreMapParameters::slot_checkBox_usedWeighting_stateChanged);

    connect(ui->lineEdit_rejectionValue_rejectIfBelow, &QLineEdit::textEdited, this, &Widget_correlScoreMapParameters::slot_lineEdit_rejectionValue_rejectIfBelow_textEdited);

    connect(ui->spinBox_weighting_exponent, QOverload<int>            ::of(&QSpinBox::valueChanged), this, &Widget_correlScoreMapParameters::slot_spinBox_weighting_exponent_valueChanged);
}

void Widget_correlScoreMapParameters::feedWithEmptyDefault() {

    _corrScoreMapParameters_inEdition.setDefaultValues();

    ui->lineEdit_correlScoreMapFileUsed->clear();
    ui->lineEdit_correlScoreMapFileUsed->setEnabled(false);

    bool bUse = _corrScoreMapParameters_inEdition._bUse;

    ui->checkBox_use->setChecked(bUse);

    ui->groupBox_options->setEnabled(bUse);

    //set checkboxes states and options values if requiered
    //------------------
    //thresholdRejection
    setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(_corrScoreMapParameters_inEdition._thresholdRejection._f_rejectIfBelowValue);
    setUiEnableAndChecked_option_rejection(bUse, _corrScoreMapParameters_inEdition._thresholdRejection._bUse);
    //setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(_corrScoreMapParameters_inEdition._thresholdRejection._f_rejectIfBelowValue);

    //-----------------
    // exponent for weighting
    setUiEnableAndChecked_option_weightingAdjustement(bUse, _corrScoreMapParameters_inEdition._weighting._bUse);
    setUiValueFromFloat_spinBox_weighting_exponent(_corrScoreMapParameters_inEdition._weighting._f_weightExponent);

}

void Widget_correlScoreMapParameters::slot_checkBox_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);

    _corrScoreMapParameters_inEdition._bUse = bChecked;

    ui->lineEdit_correlScoreMapFileUsed->setEnabled(_corrScoreMapParameters_inEdition._bUse);

    qDebug() << __FUNCTION__ << "_corrScoreMapParameters_inEdition._bUse = " << _corrScoreMapParameters_inEdition._bUse;

    ui->groupBox_options->setEnabled(_corrScoreMapParameters_inEdition._bUse);

    _bUiSetLogicFields = false;
    if (_corrScoreMapParameters_inEdition._bUse) {
        setUiEnableAndChecked_option_rejection           (true, _corrScoreMapParameters_inEdition._thresholdRejection._bUse);
        setUiEnableAndChecked_option_weightingAdjustement(true, _corrScoreMapParameters_inEdition._weighting._bUse);
    } else {        
        setUiEnableAndChecked_option_rejection           (false, false);
        setUiEnableAndChecked_option_weightingAdjustement(false, false);
    }
    _bUiSetLogicFields = true;

    _corrScoreMapParameters_inEdition.show(__FUNCTION__);
}

bool Widget_correlScoreMapParameters::inputValid(bool& bRejectionValueValid) {
    bRejectionValueValid = _bqstrRejectionValueValid;
    if (ui->checkBox_use->isChecked()) {
        if (ui->checkBox_useRejectionThreshold->isChecked()) {
            return(_bqstrRejectionValueValid);
        }
    }
    return(true);
}

bool Widget_correlScoreMapParameters::get_correlScoreMapParameters(S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters) {
    _corrScoreMapParameters_inEdition.show(__FUNCTION__);

    if (_corrScoreMapParameters_inEdition._thresholdRejection._bUse) {
        if (_bqstrRejectionValueValid) {
            float fThresholdRejection = 0;
            bool bOk = false;
            fThresholdRejection = _qstrRejectionValue.toFloat(&bOk);
            qDebug() << __FUNCTION__ << "bOk = " << bOk;
            qDebug() << __FUNCTION__ << "fThresholdRejection = " << fThresholdRejection;
            if (!bOk) {
                return(false);
            }
            _corrScoreMapParameters_inEdition._thresholdRejection._f_rejectIfBelowValue = fThresholdRejection;
        } else {
            return(false);
        }
    }
    sCorrelationScoreMapParameters = _corrScoreMapParameters_inEdition;
    return(true);
}

void Widget_correlScoreMapParameters::slot_lineEdit_rejectionValue_rejectIfBelow_textEdited(const QString& text) {
    bool bValidityChanged = updateFieldAndWidget(text, _qstrRejectionValue, _bqstrRejectionValueValid);
    if (bValidityChanged) {
        emit signal_rejectionValue_validityChanged();
    }
}

bool Widget_correlScoreMapParameters::updateFieldAndWidget(const QString& text, QString& qstr, bool &bValid) {

    bool bMatch = string_isDecimalDigitsOnly(text);
    qDebug() << __FUNCTION__ << "string_isDecimalDigitsOnly(" << text << ") => bMatch = " << bMatch;

    if (bMatch) {

        bool bIsNotStringNumericZeroValue = false;
        bool bTotalCharIsEqualOrUnder = false;
        /*bool*/ bMatch = string_isNotStringNumericZeroValue_and_totalCharIsEqualOrUnder(
                    text, 5,
                    bIsNotStringNumericZeroValue, bTotalCharIsEqualOrUnder);

        qDebug() << __FUNCTION__ << "bIsNotStringNumericZeroValue = " << bIsNotStringNumericZeroValue;
        qDebug() << __FUNCTION__ << "bTotalCharIsEqualOrUnder     = " << bTotalCharIsEqualOrUnder;
        qDebug() << __FUNCTION__ << "bMatch = " << bMatch;

        if (!bMatch) {
            qDebug() << __FUNCTION__ << "if (!bMatch) {";

            if (bTotalCharIsEqualOrUnder && (!bIsNotStringNumericZeroValue)) {                
                qDebug() << __FUNCTION__ << "if (bTotalCharIsEqualOrUnder && (!bIsNotStringNumericZeroValue)) {";
                bMatch = true;

                qDebug() << __FUNCTION__ << "  => bMatch set to " << bMatch;
            }
        }
    }

    qDebug() << __FUNCTION__ << "#1 bMatch = " << bMatch;

    if (bMatch) {

        bMatch = stringNumericValue_isComplianWithMaxBeforeDottMaxAfterDot(text, 3, 3);

        qDebug() << __FUNCTION__ << "if (bMatch) { => #2 bMatch set to " << bMatch;
    }

    bool bValidityBefore = bValid;
    bValid = bMatch;

    if (bMatch) {
       ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet("");
       qstr = ui->lineEdit_rejectionValue_rejectIfBelow->text();
    } else {
       ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
       qstr.clear();
    }

    if (bMatch != bValidityBefore) {
        return(true);
    }
    return(false);
}

void Widget_correlScoreMapParameters::feed(bool bAvailableForEdit,
                                           const QString& strCorrelScoreMapFileUsed,
                                           const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters) {

    qDebug() << __FUNCTION__ << "(Widget_correlScoreMapParameters) set _corrScoreMapParameters_inEdition from method param";

    _corrScoreMapParameters_inEdition = sCorrelationScoreMapParameters;

    _corrScoreMapParameters_inEdition.show(__FUNCTION__);

    bool bUse = _corrScoreMapParameters_inEdition._bUse;

    if (!bAvailableForEdit) {
        feedWithEmptyDefault();
        return;
    }

    //@LP the logic side has to keep sync _corrScoreMapParameters_inEdition._bUse and filePath string emptiness
    ui->lineEdit_correlScoreMapFileUsed->setText(strCorrelScoreMapFileUsed);
    ui->lineEdit_correlScoreMapFileUsed->setEnabled(bUse);

    ui->checkBox_use->setChecked(bUse);

    ui->groupBox_options->setEnabled(bUse);

    //check options if set
    //------------------
    //thresholdRejection
    setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(_corrScoreMapParameters_inEdition._thresholdRejection._f_rejectIfBelowValue);
    setUiEnableAndChecked_option_rejection(bUse, _corrScoreMapParameters_inEdition._thresholdRejection._bUse);
    //setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(_corrScoreMapParameters_inEdition._thresholdRejection._f_rejectIfBelowValue);

    //-----------------
    // exponent for weighting
    setUiEnableAndChecked_option_weightingAdjustement(bUse, _corrScoreMapParameters_inEdition._weighting._bUse);
    setUiValueFromFloat_spinBox_weighting_exponent(_corrScoreMapParameters_inEdition._weighting._f_weightExponent);
}

void Widget_correlScoreMapParameters::slot_checkBox_useRejectionThreshold_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    qDebug() << __FUNCTION__ <<" bChecked = " << bChecked;

    if (_bUiSetLogicFields) {
        qDebug() << __FUNCTION__ <<"  if (_bUiSetFieldOf_corrScoreMapParameters_inEdition) {";
        _corrScoreMapParameters_inEdition._thresholdRejection._bUse = bChecked;
    }
    setUiEnable_option_rejection();
}

void Widget_correlScoreMapParameters::slot_checkBox_usedWeighting_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    qDebug() << __FUNCTION__ <<" bChecked = " << bChecked;

    if (_bUiSetLogicFields) {
        qDebug() << __FUNCTION__ <<"  if (_bUiSetFieldOf_corrScoreMapParameters_inEdition) {";
        _corrScoreMapParameters_inEdition._weighting._bUse = bChecked;
    }
    setUiEnable_option_weightingAdjustement();
}

void Widget_correlScoreMapParameters::setUiEnableAndChecked_option_rejection(bool bEnable, bool bChecked) {

    qDebug() << __FUNCTION__ << "bEnable = " << bEnable << ", bChecked = " << bChecked;

    ui->checkBox_useRejectionThreshold->setChecked(bChecked);
    ui->checkBox_useRejectionThreshold->setEnabled(bEnable);

    ui->label_rejectionValue_rejectIfBelow->setEnabled(bChecked);
    ui->lineEdit_rejectionValue_rejectIfBelow->setEnabled(bChecked);

    update_StyleSheet_lineEdit_rejectionValue_rejectIfBelow(bChecked);
}

void Widget_correlScoreMapParameters::setUiEnableAndChecked_option_weightingAdjustement(bool bEnable, bool bChecked) {

    qDebug() << __FUNCTION__ << "bEnable = " << bEnable << ", bChecked = " << bChecked;

    ui->checkBox_usedWeighting_above1->setChecked(bChecked);
    ui->checkBox_usedWeighting_above1->setEnabled(bEnable);

    ui->label_exponentForWeight->setEnabled(bChecked);
    ui->spinBox_weighting_exponent->setEnabled(bChecked);
}

void Widget_correlScoreMapParameters::setUiEnable_option_rejection() {

    bool bChecked = _corrScoreMapParameters_inEdition._thresholdRejection._bUse;

    ui->label_rejectionValue_rejectIfBelow->setEnabled(bChecked);
    ui->lineEdit_rejectionValue_rejectIfBelow->setEnabled(bChecked);

    update_StyleSheet_lineEdit_rejectionValue_rejectIfBelow(bChecked);
}

void Widget_correlScoreMapParameters::update_StyleSheet_lineEdit_rejectionValue_rejectIfBelow(bool bChecked) {
    //current rejectionValue string valid
    if (_bqstrRejectionValueValid) {
         if (!bChecked) { //unchecked option
            ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet(""); //grayed
         } else { //checked option

             ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet("");
         }
    } else { //current rejectionValue string invalid
        if (!bChecked) { //unchecked option
           ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet(""); //grayed
        } else { //checked option
           ui->lineEdit_rejectionValue_rejectIfBelow->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
        }
    }
}

void Widget_correlScoreMapParameters::setUiEnable_option_weightingAdjustement() {

    bool bChecked = _corrScoreMapParameters_inEdition._weighting._bUse;

    ui->label_exponentForWeight->setEnabled(bChecked);
    ui->spinBox_weighting_exponent->setEnabled(bChecked);
}

void Widget_correlScoreMapParameters::setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(float fValue) {

    //feed the rejectionValue in the widget
    _qstrRejectionValue = "0";
    int decimalPrecision = 3;
    qreal qreal_convertedFixedPrecision = .0;

    _qstrRejectionValue = doubleToQStringPrecision_justFit(static_cast<double>(fValue), 4);
    _qstrRejectionValue = removeUselessZerosInNumericValueString_withoutSign_and_oneDecimalDotMaximum(_qstrRejectionValue);

    _bqstrRejectionValueValid = true;

    ui->lineEdit_rejectionValue_rejectIfBelow->setText(_qstrRejectionValue);
}

void Widget_correlScoreMapParameters::setUiValueFromFloat_spinBox_weighting_exponent(float fValue) {
    //in any case ,feed the value (the value wil be 1 if the option is not used (grayed))
    int intWeightExponent = static_cast<int>(fValue);
    ui->spinBox_weighting_exponent->setValue(intWeightExponent);
}

void Widget_correlScoreMapParameters::slot_spinBox_weighting_exponent_valueChanged(int value) {
    _corrScoreMapParameters_inEdition._weighting._f_weightExponent = static_cast<float>(value);
}

Widget_correlScoreMapParameters::~Widget_correlScoreMapParameters() {
    delete ui;
}
