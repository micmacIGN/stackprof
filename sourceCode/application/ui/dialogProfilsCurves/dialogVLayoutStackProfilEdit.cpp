#include "logic/model/dialogProfilsCurves/CustomGraphicsScene_profil.h"

#include "dialogVLayoutStackProfilEdit.h"

#include "ui_dialogVLayoutStackProfilEdit.h"

#include <QDebug>

#include <QResizeEvent>

#include "e_hexaCodeLayerEditProfil.hpp"

#include "../../logic/model/dialogProfilsCurves/enum_profiladjustmode.h"

DialogVLayoutStackProfilEdit::DialogVLayoutStackProfilEdit(QWidget *parent) : QDialog(parent),
    ui(nullptr),
    _eProfilAdjustMode(e_PAM_notSet),
    _bSwitchAdjustMode_actionIsPossible(true),
    _qvectPtrCustomGScene_Perp_Parall_deltaZ { nullptr, nullptr, nullptr},
    _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ {false, false, false},  //by default none layers possibly visible
    _qvectBoolComponentsVisible_Perp_Parall_deltaZ {false, false, false},
    _bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination{false}

{
    //setWindowFlags(Qt::Window); //adding close, maximize and minimize buttons

    ui = new Ui::dialogVLayoutStackProfilEdit;
    ui->setupUi(this);

    QHash<e_hexaCodeComponentEditProfil, QString> qhash_e_hxCCEP_qstr;
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_Perp         , "Perp.");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_Parall       , "Parall.");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_Perp_Parall  , "Perp,Parall");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_deltaZ       , "deltaZ");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_Perp_deltaZ  , "Perp,deltaZ");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_Parall_deltaZ, "Parall,deltaZ");
    qhash_e_hxCCEP_qstr.insert(e_hxCCEP_All          , "All");
    ui->wdgt_componentsSwitcher->setButtonsName(qhash_e_hxCCEP_qstr);

    //@#LP eLA_used as vector index
    _qvectStackprofileditPtr.resize(3);


    _qvectStackprofileditPtr[e_CA_Perp]   = ui->widget1;
    _qvectStackprofileditPtr[e_CA_Parall] = ui->widget2;
    _qvectStackprofileditPtr[e_CA_deltaZ] = ui->widget3;

    connect(ui->pushButton_nextBox    , &QPushButton::clicked, this, &DialogVLayoutStackProfilEdit::slot_pushButton_nextBox_clicked);
    connect(ui->pushButton_previousBox, &QPushButton::clicked, this, &DialogVLayoutStackProfilEdit::slot_pushButton_previousBox_clicked);

    connect(ui->wdgt_componentsSwitcher, &ComponentsSwitcher::signal_pushButtonClicked, this, &DialogVLayoutStackProfilEdit::slot_componentsSwitcherPushButtonClicked);

    connect(ui->checkBox_syncYRange, &QCheckBox::stateChanged, this, &DialogVLayoutStackProfilEdit::slot_checkBox_syncYRange_stateChanged);

    connect(ui->pushButton_switchAdjustMode,  &QPushButton::clicked, this, &DialogVLayoutStackProfilEdit::slot_pushButton_switchAdjustMode_clicked);

    connect(ui->wdgt_currentBoxIdEditToReach, &widgetCurrentBoxIdEditToReach::signal_boxIdToReachFromEditedId,
            this, &DialogVLayoutStackProfilEdit::slot_boxIdToReachFromEditedId);

    setLabelAndButtonsStateCurrentBoxId(-1,0);//_boxId = -1, _boxCount = 0

    set_switchAdjustMode(e_PAM_leftRight);

    ui->wdgt_currentBoxIdEditToReach->setFocus();

}

void DialogVLayoutStackProfilEdit::slot_locationInAdjustement(bool bState) {
    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit): bState = " << bState;
    _bSwitchAdjustMode_actionIsPossible = !bState;
    ui->pushButton_switchAdjustMode->setEnabled(_bSwitchAdjustMode_actionIsPossible);
}

void DialogVLayoutStackProfilEdit::slot_boxIdToReachFromEditedId(int boxId) {
    _boxId = boxId;
    emit signal_switchToBox(_boxId);
    ui->wdgt_currentBoxIdEditToReach->setCurrentAndBoxCount(_boxId, _boxCount);
}

void DialogVLayoutStackProfilEdit::slot_pushButton_switchAdjustMode_clicked() {
    switchAdjustMode();
}

void DialogVLayoutStackProfilEdit::switchAdjustMode() {

    if (_eProfilAdjustMode == e_PAM_notSet) {
        return;
    }

    if (_eProfilAdjustMode == e_PAM_leftRight) {
        _eProfilAdjustMode = e_PAM_X0Center;
    } else {
        _eProfilAdjustMode = e_PAM_leftRight;
    }

    //QWidget *qWidgetWithFocus_ptr = QApplication::focusWidget();

    set_switchAdjustMode(_eProfilAdjustMode);

    //re-set focus leads to re-set on a widget possibly in a hidden stacked page, which leads to visible artefact about the focud border (MacOSX)
    /*if (qWidgetWithFocus_ptr) {
        qWidgetWithFocus_ptr->setFocus();
    }*/
}

void DialogVLayoutStackProfilEdit::set_switchAdjustMode(e_ProfilAdjustMode eProfilAdjustMode) {
    if (eProfilAdjustMode == e_PAM_notSet) {
        return;
    }
    _eProfilAdjustMode = eProfilAdjustMode;
    for (int iComp = e_CA_Perp; iComp <= e_CA_deltaZ; iComp++) {
        _qvectStackprofileditPtr[iComp]->set_switchAdjustMode(_eProfilAdjustMode);
    }
    //@LP button text show the other mode
    if (_eProfilAdjustMode == e_PAM_leftRight) {
        ui->pushButton_switchAdjustMode->setText("Adj. C");
    } else { //e_PAM_X0Center
        ui->pushButton_switchAdjustMode->setText("L/R");
    }

    emit signal_profilAdjustMode_changed(_eProfilAdjustMode);//inform model
}

#include <QMessageBox>

void DialogVLayoutStackProfilEdit::slot_pushButton_meaning_clicked() {

    QMessageBox messageBox(QMessageBox::Information,
                           "Infos about graphs",
                           "<Work In progress> You will find here infos like units, curves and envelops meanings", QMessageBox::Ok, nullptr);
    messageBox.exec();
}

#include "../../logic/model/imageScene/S_booleanKeyStatus.hpp"  //@#LP move S_booleanKeyStatus in a more generic location

void DialogVLayoutStackProfilEdit::keyPressEvent(QKeyEvent *event) {

    if (event->count() > 1) { return; }
    if (event->isAutoRepeat()) { return; }

    if (!_bSwitchAdjustMode_actionIsPossible) {
        return;
    }

    S_booleanKeyStatus SbkeyStatus;

    //ctrl key pressed requiered for all key shortcut:
    if (!SbkeyStatus._bCtrlKeyAlone) {
        qDebug() << "ctrl key not pressed";
        return;
    }

    //ctrl + shift => switchAdjustMode
    if (SbkeyStatus._bShiftKeyAlone) {
        switchAdjustMode(); //_bSwitchAdjustMode_isWaitingKeyPressRelease = true;
        return;
    }

    if (_boxId == -1) {
        return;
    }
    if (_boxCount < 2) { //@LP: for -1, 0, 1 values
        return;
    }

    int stepDirection = 0;
    bool bGoFirst = false;
    bool bGoLast = false;

    switch (event->key()) {

        //Previous
        case Qt::Key_PageUp:
        //case Qt::Key_1:
        case Qt::Key_S:
            stepDirection = -1;
            break;

        //Next
        case Qt::Key_PageDown:
        //case Qt::Key_2:
        case Qt::Key_D:
            stepDirection = +1;
            break;

        //First
        case Qt::Key_Home:
        //case Qt::Key_4:
        case Qt::Key_G:
            qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit): Key_Home: " << event->key();
            bGoFirst = true;
            break;

        //Last
        case Qt::Key_End:
        //case Qt::Key_5:
        case Qt::Key_H:
            qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit): Key_End: " << event->key();
            bGoLast = true;
            break;
     }

    //@LP ( Key_Greater and Key_Less can be two distincts physical keys on mac keyboards )


    int boxId_beforeChange = _boxId;

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit): boxId_beforeChange: " << boxId_beforeChange;

    if (stepDirection) {
        _boxId += stepDirection;
        if (_boxId < 0) {
            _boxId = 0;
        }
        if (_boxId >= _boxCount) {
            _boxId = _boxCount-1;
        }
    } else {
        if (bGoFirst) {
            _boxId = 0;
        } else {
            if (bGoLast) {
                _boxId = _boxCount-1;
            }
        }
    }

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit): after: _boxId: " << _boxId;

    if (_boxId != boxId_beforeChange) {
        emit signal_switchToBox(_boxId);
    }
}


void DialogVLayoutStackProfilEdit::slot_pushButton_nextBox_clicked() {
    emit signal_switchToBox(_boxId+1);
}

void DialogVLayoutStackProfilEdit::slot_pushButton_previousBox_clicked() {
    emit signal_switchToBox(_boxId-1);
}

void DialogVLayoutStackProfilEdit::slot_checkBox_syncYRange_stateChanged(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    emit signal_syncYRange_stateChanged(bChecked);
}

//
void DialogVLayoutStackProfilEdit::slot_setComponentsPossiblyVisible(QVector<bool> qvectComponentsComputed) {
    //qDebug() << __FUNCTION__ << QWidget::testAttribute(Qt::WA_DeleteOnClose);
    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";
    setComponentsPossiblyVisibleAndUpdateComponentsSwitcher_fromQvectComponentsComputed(qvectComponentsComputed);
}

void DialogVLayoutStackProfilEdit::setComponentsPossiblyVisibleAndUpdateComponentsSwitcher_fromQvectComponentsComputed(const QVector<bool>& qvectComputedComponents) {
    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    if (_bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination) { //previous combination set

        if (qvectComputedComponents == _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ) { //and the possibly visible is the same as previously

            //do nothing, stay on the same component switcher and profils visible

        } else { //the new possibly visible is different than previously
            //update and set the the according default state
            _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ = qvectComputedComponents;
            ui->wdgt_componentsSwitcher->updateButtonsAvailability_setDefaultState(_qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ, true);
            _bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination = true; //due to setDefaultState just above

            //udpate the visible profils components
            if (!setComponentsVisible_fromQvectBoolComponents(_qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ)) {
                qDebug() << __FUNCTION__ << "if (!setComponentsVisible_fromQvectBoolComponents(...) ";
            }

        }
    } else { //no previously combination set
        //update and set the the according default state
        _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ = qvectComputedComponents;
        ui->wdgt_componentsSwitcher->updateButtonsAvailability_setDefaultState(_qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ, true);
        _bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination = true; //due to setDefaultState just above

        //udpate the visible profils components
        if (!setComponentsVisible_fromQvectBoolComponents(_qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ)) {
            qDebug() << __FUNCTION__ << "if (!setComponentsVisible_fromQvectBoolComponents(...) ";
        }
    }
}



void DialogVLayoutStackProfilEdit::slot_componentsSwitcherPushButtonClicked(int iehxCLEP) {

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    QVector<bool> qvectComponentsToShow(3, false);
    qvectComponentsToShow[e_CA_Perp  ] = iehxCLEP & e_hxCCEP_Perp;
    qvectComponentsToShow[e_CA_Parall] = iehxCLEP & e_hxCCEP_Parall;
    qvectComponentsToShow[e_CA_deltaZ] = iehxCLEP & e_hxCCEP_deltaZ;

    _qvectBoolComponentsVisible_Perp_Parall_deltaZ = qvectComponentsToShow;
    emit signal_componentsToShowChanged(_qvectBoolComponentsVisible_Perp_Parall_deltaZ);
}

void DialogVLayoutStackProfilEdit::slot_setComponentsVisible_fromQvectBoolComponents(QVector<bool> qvectBoolComponents) {

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    qDebug() << __FUNCTION__ << "qvectBoolComponents = " << qvectBoolComponents;
    setComponentsVisible_fromQvectBoolComponents(qvectBoolComponents);
}

bool DialogVLayoutStackProfilEdit::setComponentsVisible_fromQvectBoolComponents(const QVector<bool>& qvectBoolComponents) {

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    qDebug() << __FUNCTION__ << "qvectBoolComponents = " << qvectBoolComponents;

    if (qvectBoolComponents.size() != 3 ) {
        qDebug() << __FUNCTION__ << " if (qvectBoolComponents.size() != 3 )";
        return(false);
    }

    _qvectBoolComponentsVisible_Perp_Parall_deltaZ = qvectBoolComponents;

    ui->checkBox_syncYRange->setEnabled(_qvectBoolComponentsVisible_Perp_Parall_deltaZ.count(true) > 1);

    qDebug() << __FUNCTION__ << "_qvectBoolComponentsVisible_Perp_Parall_deltaZ = " << _qvectBoolComponentsVisible_Perp_Parall_deltaZ ;

    //without hiding all and updating size spaces took by widgets: there is a missing space update.
    //This miss creates an incorrect graphic refresh (some graphic items on Perp alone not completely drawn.
    //The missing area is where Perp custom graphic basic widget would be if not alone.

    //this way works on ubuntu
    //@#LP have to comfirm this works also on MS Windows
    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        _qvectStackprofileditPtr[ieCA]->setVisible(false);
        _qvectStackprofileditPtr[ieCA]->updateGeometry();
        ui->verticalLayout->update();
    }

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        if (_qvectBoolComponentsVisible_Perp_Parall_deltaZ[ieCA]) {

            qDebug() << __FUNCTION__ << "ieCA : " << ieCA << "_qvectBoolComponentsVisible_Perp_Parall_deltaZ[ieLA] = " << _qvectBoolComponentsVisible_Perp_Parall_deltaZ[ieCA];

            _qvectStackprofileditPtr[ieCA]->setVisible(true);
            _qvectStackprofileditPtr[ieCA]->updateGeometry();
            ui->verticalLayout->update();
        }
    }

    return(true);
}



DialogVLayoutStackProfilEdit::~DialogVLayoutStackProfilEdit() {
    delete ui;
}

void DialogVLayoutStackProfilEdit::resizeEvent(QResizeEvent *event) {

    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    QDialog::resizeEvent(event);
}

void DialogVLayoutStackProfilEdit::setCGScenePtr(
        const QVector<CustomGraphicsScene_profil*> &qvectPtrSmallImageNav_CustomGraphicsScene) {
    _qvectPtrCustomGScene_Perp_Parall_deltaZ = qvectPtrSmallImageNav_CustomGraphicsScene;
}

void DialogVLayoutStackProfilEdit::setTextWidgetPartModelPtr(
        const QVector<StackProfilEdit_textWidgetPartModel*> &qvectPtrStackProfilEdit_textWidgetPartModel) {
    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ = qvectPtrStackProfilEdit_textWidgetPartModel;
}

void DialogVLayoutStackProfilEdit::setLabelAndButtonsStateCurrentBoxId(int boxId, int boxCount) {
    _boxId = boxId;
    _boxCount = boxCount;

    ui->pushButton_previousBox->setEnabled( _boxId > 0);
    ui->pushButton_nextBox->setEnabled(_boxId < _boxCount-1);

    ui->wdgt_currentBoxIdEditToReach->setCurrentAndBoxCount(boxId, boxCount);
}

bool DialogVLayoutStackProfilEdit::initConnectionModelViewSignalsSlots() {

    //graphic area part:
    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    if (!ui) {
        qDebug() << __FUNCTION__ << " error: ui is nullptr";
        return(false);
    }

    if (_qvectPtrCustomGScene_Perp_Parall_deltaZ.size() != 3 ) {
        qDebug() << __FUNCTION__ << " error: if (_qvectPtrCustomGScene_PX1_PX2_deltaZ.size() != 3 ) {";
        return(false);
    }


    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        qDebug() << __FUNCTION__ << "_qvectStackprofileditPtr[ieCA   ]@ = " << (void*)_qvectStackprofileditPtr[ieCA];
        qDebug() << __FUNCTION__ << "_qvectPtrCustomGScene_Perp_Parall_deltaZ[ieCA]@ = " << (void*)_qvectPtrCustomGScene_Perp_Parall_deltaZ[ieCA];
        qDebug() << __FUNCTION__ << "_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA]@ = " << (void*)_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA];
    }

    //@#LP made at each call, should not be requiered
    //@#LP no nullptr check
    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        _qvectStackprofileditPtr[ieCA   ]->setSceneForView(_qvectPtrCustomGScene_Perp_Parall_deltaZ[ieCA]);

        //text widget parts:
        _qvectStackprofileditPtr[ieCA   ]->setTextWidgetsPartModel_andConnectModelViewSignalsSlots(
                    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA]);

        connect(_qvectPtrCustomGScene_Perp_Parall_deltaZ[ieCA], &CustomGraphicsScene_profil::signal_locationInAdjustement,
                this, &DialogVLayoutStackProfilEdit::slot_locationInAdjustement);

    }

    return(true);
}


void DialogVLayoutStackProfilEdit::updateOnDemand_onQvectStackprofiledit() {
    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        _qvectStackprofileditPtr[ieCA   ]->updateOnDemand();//@LP force a view refresh
    }
}

void DialogVLayoutStackProfilEdit::clearUiContent_all() {

    clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults();

    setLabelAndButtonsStateCurrentBoxId(-1,0);

    ui->wdgt_componentsSwitcher->clearState();

    _qstrUnits.clear();

    _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ.fill(false,3); //by default none layer possibly visible

    _qvectBoolComponentsVisible_Perp_Parall_deltaZ.fill(false,3); //by default none layer visible
    _bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination = false;

}

void DialogVLayoutStackProfilEdit::clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults() {
    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        _qvectStackprofileditPtr[ieCA   ]->clearUiContent();
    }
}



void DialogVLayoutStackProfilEdit::feedTextWidgets() {
    qDebug() << __FUNCTION__ << "(DialogVLayoutStackProfilEdit)";

    if (!ui) {
        qDebug() << __FUNCTION__ << " error: ui is nullptr";
        return;
    }

    _qvectStackprofileditPtr[eLA_PX1]->feedTextWidgetsFromModel();
    _qvectStackprofileditPtr[eLA_PX2]->feedTextWidgetsFromModel();   
    _qvectStackprofileditPtr[eLA_deltaZ]->feedTextWidgetsFromModel();
}
