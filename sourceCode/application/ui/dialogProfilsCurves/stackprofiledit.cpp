#include <QGraphicsView>
#include <QDebug>

#include <QMouseEvent>

#include "stackprofiledit.h"

#include "../../logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp"

#include "../../logic/toolbox/toolbox_conversion.h"

stackprofiledit::stackprofiledit(QWidget *parent) : QWidget(parent),
    ui(nullptr),
    _eProfilAdjustMode(e_PAM_notSet),
    _stackProfilEdit_textWidgetPartModelPtr(nullptr) {

    qDebug() << __FUNCTION__;

    ui = new Ui::Form;
    ui->setupUi(this);

    //prepare access to QlineEdit by indexes:
    _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[e_LRsA_left ][e_LRsA_left ] = ui->lineEdit_inout_leftSide_leftAdj;
    _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[e_LRsA_left ][e_LRsA_right] = ui->lineEdit_inout_leftSide_rightAdj;
    _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[e_LRsA_right][e_LRsA_left ] = ui->lineEdit_inout_rightSide_leftAdj;
    _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[e_LRsA_right][e_LRsA_right] = ui->lineEdit_inout_rightSide_rightAdj;

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] = .0; //just to have initial init
            _a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] = true;//valid by default, consider the feed from model as consistent
        }
    }

    _doubleValueOfQLineEditTextContent_X0PosValue = .0;//just to have initial init
    _evaluatedConsistencyFlag_X0PosValue = true;//valid by default, consider the feed from model as consistent

    clearUiContent();

    set_switchAdjustMode(e_PAM_leftRight);

}

void stackprofiledit::set_switchAdjustMode(e_ProfilAdjustMode eProfilAdjustMode) {

    if (eProfilAdjustMode == e_PAM_notSet) {
        return;
    }
    if (eProfilAdjustMode == _eProfilAdjustMode) {
        return;
    }

    _eProfilAdjustMode = eProfilAdjustMode;

    if (_eProfilAdjustMode == e_PAM_leftRight) {
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        ui->stackedWidget->setCurrentIndex(1);
    }

    if (ui->cgViewBasic->customGraphicsScenePtr()) {
        ui->cgViewBasic->customGraphicsScenePtr()->setProfilAdjustMode(_eProfilAdjustMode);
    }
}

void stackprofiledit::setSceneForView(QGraphicsScene *sceneForView) {
    qDebug() << __FUNCTION__ << "(stackprofiledit) @sceneForView =" << (void*)sceneForView;

    if (!sceneForView) {
        qDebug() << __FUNCTION__ << "(stackprofiledit) error: sceneForView is nullptr";
        return;
    }
    ui->cgViewBasic->setScene(sceneForView);
    ui->cgViewBasic->setCustomGraphicsScenePtr(dynamic_cast<CustomGraphicsScene_profil *>(sceneForView));
    //setCustomGraphicsScenePtr can appears redundant with setScene. The idea is to avoid dynamic cast on scene()
    //If we need specific call on CustomGraphicsScene, use the dedicated ptr.

    ui->cgViewBasic->setBackgroundBrush(QBrush(QColor(240,240,240), Qt::SolidPattern)); //@LP gray background

}


void stackprofiledit::setTextWidgetsPartModel_andConnectModelViewSignalsSlots(StackProfilEdit_textWidgetPartModel* stackProfilEdit_textWidgetPartModelPtr) {
    _stackProfilEdit_textWidgetPartModelPtr = stackProfilEdit_textWidgetPartModelPtr;

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "(stackprofiledit) error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";
        return;
    }

    connect(ui->pButton_setLeftRight, &QPushButton::clicked,
            this, &stackprofiledit::slot_validateEditedAdjustersSetPositionValues_evaluatingConsistency);

    //@#LP update is in two steps; reduce to one (sending the values with the signal)
    connect(_stackProfilEdit_textWidgetPartModelPtr, &StackProfilEdit_textWidgetPartModel::signal_feed_stackProfilEdit_textWidgets,
            this, &stackprofiledit::slot_feed_stackProfilEdit_textWidgets);

    qDebug() << __FUNCTION__ << "(stackprofiledit)>";

    connect(ui->pButton_setX0, &QPushButton::clicked,
            this, &stackprofiledit::slot_validateEditedAdjustersSetPositionValues_evaluatingConsistency);

    connect(ui->checkBox_aLR_flag, &QCheckBox::toggled, this, &stackprofiledit::slot_checkBox_flag_toggled_fromWidget);
    connect(ui->checkBox_aX0_flag, &QCheckBox::toggled, this, &stackprofiledit::slot_checkBox_flag_toggled_fromWidget);

    connect(_stackProfilEdit_textWidgetPartModelPtr, &StackProfilEdit_textWidgetPartModel::signal_setWarnFlag_forUi,
            this, &stackprofiledit::slot_setWarnFlag);

}

void stackprofiledit::slot_feed_stackProfilEdit_textWidgets() {
    qDebug() << __FUNCTION__ << " 115 (stackprofiledit)";
    feedTextWidgetsFromModel();
}

void stackprofiledit::validateEditedAdjustersSetPositionValues_evaluatingConsistency() {

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "stackprofiledit::feedWidget()" << " error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";
        return;
    }

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {

            if (!_a2x2_QLineEditptr_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side]) {
                qDebug() << __FUNCTION__ << " error: some _2x2_QLineEditptr_LRCoupleOfLRxPosValues at nullptr";
                return;
            }

            signed int signedIntFromStr = 0;
            bool bOk = false;
            QString strValue = _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side]->text();
            signedIntFromStr = strValue.toInt(&bOk);
            if (!bOk) {
                qDebug() << __FUNCTION__ << " major warning: can't convert text to int: [" << strValue << "]";

                //@LP could be partially handled if none numeric for example (empty, '-', '+', etc)
                return;
            }

            qDebug() << __FUNCTION__ << " strValue = [" << strValue << "]";
            qDebug() << __FUNCTION__ << " signedIntFromStr = [" << signedIntFromStr << "]";

            _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side] =
                    static_cast<double>(signedIntFromStr);
        }
    }

    qDebug() << __FUNCTION__ << " _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues content:";
    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            qDebug() << __FUNCTION__ << _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];
        }
    }

    _stackProfilEdit_textWidgetPartModelPtr->validateEditedAdjustersSetPositionValues_evaluatingConsistency(
                _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues,
                _a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues);

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            QLineEdit *qlineEditPtr = _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];
            if (qlineEditPtr) {
                if (!_a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side]) {
                    qlineEditPtr->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
                } else {

                    qlineEditPtr->setStyleSheet("");
                }
            }
        }
    }
}

void stackprofiledit::validateEditedX0CenterSetPositionValue_evaluatingConsistency() {

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "stackprofiledit::feedWidget()" << " error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";
        return;
    }

    qDebug() << __FUNCTION__ << ui->lineEdit_inout_X0->text();

    signed int signedIntFromStr = 0;
    bool bOk = false;
    QString strValue = ui->lineEdit_inout_X0->text();
    signedIntFromStr = strValue.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " major warning: can't convert text to int: [" << strValue << "]";
        //@LP could be partially handled if none numeric for example (empty, '-', '+', etc)
        return;
    }

    qDebug() << __FUNCTION__ << " strValue = [" << strValue << "]";
    qDebug() << __FUNCTION__ << " signedIntFromStr = [" << signedIntFromStr << "]";

    _doubleValueOfQLineEditTextContent_X0PosValue = static_cast<double>(signedIntFromStr);

    _evaluatedConsistencyFlag_X0PosValue = true;//valid by default, consider the feed from model as consistent

    _stackProfilEdit_textWidgetPartModelPtr->validateEditedX0CenterSetPositionValue_evaluatingConsistency(//validateEditedX0CenterSetPositionValue_evaluatingConsistency_andSendForcedAdjustersPositionValuesToFollowConsistency(
                _doubleValueOfQLineEditTextContent_X0PosValue,
                _evaluatedConsistencyFlag_X0PosValue);

    if (!_evaluatedConsistencyFlag_X0PosValue) {
        ui->lineEdit_inout_X0->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
    } else {

        ui->lineEdit_inout_X0->setStyleSheet("");
    }
}


void stackprofiledit::slot_validateEditedAdjustersSetPositionValues_evaluatingConsistency() {

    qDebug() << __FUNCTION__ << "(stackprofiledit)";

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "stackprofiledit::feedWidget()" << " error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";
        return;
    }

    if (_eProfilAdjustMode == e_PAM_notSet) {
        return;
    }

    if (_eProfilAdjustMode == e_PAM_leftRight) {
        validateEditedAdjustersSetPositionValues_evaluatingConsistency();
    } else { // e_PAM_X0Center
        validateEditedX0CenterSetPositionValue_evaluatingConsistency();
    }

}

void stackprofiledit::updateOnDemand() {
    ui->cgViewBasic->updateOnDemand();
}

//@#LP the ui took the action to feed itself taking data from the model //@#LP object design break
void stackprofiledit::feedTextWidgetsFromModel() {

   const QString strNoTitle ="no title";

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "stackprofiledit::feedWidget()" << " error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";

        clearUi_with_invalid_value();

        ui->label_title->setText(strNoTitle);
        return;
    }

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            QLineEdit *qlineEditPtr = _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];
            if (qlineEditPtr) {
                qlineEditPtr->setStyleSheet("");
                qlineEditPtr->setText(QString::number(static_cast<int>
                                                      (_stackProfilEdit_textWidgetPartModelPtr->get_xAdjusterPos(
                                                           static_cast<e_LRsideAccess>(i_eLRsA_coupleRelativeToX0_side),
                                                           static_cast<e_LRsideAccess>(i_eLRsA_insideCouple_side)))));
            }
        }
    }

    ui->lineEdit_inout_X0->setText(QString::number(_stackProfilEdit_textWidgetPartModelPtr->get_X0()));

    if (!_stackProfilEdit_textWidgetPartModelPtr->linearModelIsAvailable()) {

      clearUi_offsetAtX0_sigmaSum();

    } else {

        ui->lineEdit_out_offsetAtX0->setText(doubleToQStringPrecision_f_totalDigitNumber(_stackProfilEdit_textWidgetPartModelPtr->get_offsetAtX0(),7));
        ui->lineEdit_out_sigmaSum  ->setText(doubleToQStringPrecision_f_totalDigitNumber(_stackProfilEdit_textWidgetPartModelPtr->get_sigmaSum(),7));

        ui->lineEdit_out_offsetAtX0->setEnabled(true);
        ui->lineEdit_out_sigmaSum  ->setEnabled(true);
    }

    QString strTitleAndUnits = _stackProfilEdit_textWidgetPartModelPtr->get_strTitle();
    if (strTitleAndUnits.isEmpty()) {
        strTitleAndUnits = strNoTitle;
    }

    ui->label_title->setText(strTitleAndUnits);

    bool bWarnFlag = false;
    bool bWarnFlagValid = _stackProfilEdit_textWidgetPartModelPtr->get_warningFlagByUser(bWarnFlag);

    setEnabled_aLR_aX0_flag(bWarnFlagValid);

    qDebug() << __FUNCTION__ << "(stackprofiledit)";

    setWarnFlag_optionnalForceWidgetCheckedState_noUpdateModel(bWarnFlag, true);

}

#include "../../logic/model/imageScene/S_booleanKeyStatus.hpp"

void stackprofiledit::slot_checkBox_flag_toggled_fromWidget(bool checked) {

    qDebug() << __FUNCTION__ << "(stackprofiledit)";

    qDebug() << __FUNCTION__ << "(stackprofiledit) _bUpdateModel_aboutChekBoxFlag is " << _bUpdateModel_aboutChekBoxFlag;

    if (!_bUpdateModel_aboutChekBoxFlag) {
        qDebug() << __FUNCTION__ << "(stackprofiledit)  if (!_bUpdateModel_aboutChekBoxFlag) {";
        return;
    }

    if (!_stackProfilEdit_textWidgetPartModelPtr) {
        qDebug() << __FUNCTION__ << "stackprofiledit::slot_checkBox_flag_toggled()" << " error: _stackProfilEdit_textWidgetPartModelPtr is nullptr";
        return;
    }

    S_booleanKeyStatus SbkeyStatus;

    bool bShiftKeyPressed = SbkeyStatus._bShiftKeyAlone;
    _stackProfilEdit_textWidgetPartModelPtr->set_warningFlagByUser_UiModelFromUi(checked, bShiftKeyPressed);

}

void stackprofiledit::slot_setWarnFlag(uint ieComponent, bool bWarnFlag) {
    qDebug() << __FUNCTION__ << "(stackprofiledit) bWarnFlag = " << bWarnFlag;
    setWarnFlag_optionnalForceWidgetCheckedState_noUpdateModel(bWarnFlag, false);
}

void stackprofiledit::setWarnFlag_optionnalForceWidgetCheckedState_noUpdateModel(bool bWarnFlag, bool bForceWidgetCheckedState) {
    qDebug() << __FUNCTION__ << "(stackprofiledit) bWarnFlag = " << bWarnFlag;
    qDebug() << __FUNCTION__ << "(stackprofiledit) bForceWidgetCheckedState = " << bForceWidgetCheckedState;

    _bUpdateModel_aboutChekBoxFlag = false;

    if (bForceWidgetCheckedState) {
        qDebug() << __FUNCTION__ << "(stackprofiledit) if (bForceWidgetCheckedState) { ui->checkBox_flag->setChecked(bWarnFlag = " << bWarnFlag;
        setCheckedWithColor_aLR_aX0_flag(bWarnFlag);

    } else { //- When a checkbox is checked by the user, the ui state is set by Qt.
             //- When we set checkstate by code for a new box, we can force.
             //=> When we need to set/unset a checked box to be sync with this "just set checkbox" about the same box
             //   we have to use setChecked

        qDebug() << __FUNCTION__ << "(stackprofiledit) if (bForceWidgetCheckedState) { ui->checkBox_flag->setChecked(bWarnFlag = " << bWarnFlag;
        setCheckedWithColor_aLR_aX0_flag(bWarnFlag);
    }

    _bUpdateModel_aboutChekBoxFlag = true;
}

void stackprofiledit::setCheckedWithColor_aLR_aX0_flag(bool bState) {
    if (!ui) { return; }
    setCheckedWithColor_checkBox(ui->checkBox_aLR_flag, bState);
    setCheckedWithColor_checkBox(ui->checkBox_aX0_flag, bState);
}

void stackprofiledit::setCheckedWithColor_checkBox(QCheckBox *qCheckBoxPtr, bool bState) {
    if (!qCheckBoxPtr) {
        return;
    }
    qCheckBoxPtr->setChecked(bState);
    if (bState) {
        //qCheckBoxPtr->setStyleSheet("QCheckBox { color: rgb(255, 0, 0); }"); //red in the image view is about the current selected box
                                                                               //and magenta can lead to 'not enough visible' box in image view

        qCheckBoxPtr->setStyleSheet("QCheckBox { color: rgb(255, 0, 255); }"); //magenta

    } else {
        qCheckBoxPtr->setStyleSheet("");
    }
}

void stackprofiledit::setEnabled_aLR_aX0_flag(bool bState) {
    ui->checkBox_aX0_flag->setEnabled(bState);
    ui->checkBox_aLR_flag->setEnabled(bState);
}

void stackprofiledit::clearUiContent() {

    //ui about text widget:
    clearUi_offsetAtX0_sigmaSum();
    clearUi_with_invalid_value();

    const QString strNoTitle ="no title";
    ui->label_title->setText(strNoTitle);

    //ui about graphic view:
    ui->cgViewBasic->clear();

}

void stackprofiledit::clearUi_offsetAtX0_sigmaSum() {

    ui->lineEdit_out_offsetAtX0->clear();
    ui->lineEdit_out_offsetAtX0->setEnabled(false);

    ui->lineEdit_out_sigmaSum->clear();
    ui->lineEdit_out_sigmaSum->setEnabled(false);
}

void stackprofiledit::clearUi_with_invalid_value() {

    const double invalid_value = 999999.999;

    for (int i_eLRsA_coupleRelativeToX0_side = e_LRsA_left; i_eLRsA_coupleRelativeToX0_side <= e_LRsA_right; i_eLRsA_coupleRelativeToX0_side++) {
        for (int i_eLRsA_insideCouple_side = e_LRsA_left; i_eLRsA_insideCouple_side <= e_LRsA_right; i_eLRsA_insideCouple_side++) {
            QLineEdit *qlineEditPtr = _a2x2_QLineEditptr_LRCoupleOfLRxPosValues[i_eLRsA_coupleRelativeToX0_side][i_eLRsA_insideCouple_side];
            if (qlineEditPtr) {

               qlineEditPtr->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
               qlineEditPtr->setText(QString::number(static_cast<int>(invalid_value)));
            }
        }
    }

    ui->lineEdit_out_offsetAtX0->setText(QString::number(invalid_value));
    ui->lineEdit_out_sigmaSum->setText(QString::number(invalid_value));

    ui->lineEdit_inout_X0->setText(QString::number(invalid_value));

    _bUpdateModel_aboutChekBoxFlag = false; //add here to avoid to send this cleared state to the model

    setCheckedWithColor_aLR_aX0_flag(false);

    _bUpdateModel_aboutChekBoxFlag = true; //re-enabled for user

    setEnabled_aLR_aX0_flag(false);

}

stackprofiledit::~stackprofiledit() {
    delete ui;
}
