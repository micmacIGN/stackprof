#include <QGraphicsView>
#include <QDebug>

#include <QMouseEvent>

#include "componentsSwitcher.h"

#include "e_hexaCodeLayerEditProfil.hpp"

ComponentsSwitcher::ComponentsSwitcher(QWidget *parent) : QWidget(parent),
    _qpushButtonPtr_ofPreviousState(nullptr) {

    qDebug() << __FUNCTION__;

    ui = new Ui::ComponentsButtonsSwitcher;
    ui->setupUi(this);

    ui->pushButton_1  ->setProperty("iehxCLEP", e_hxCCEP_Perp);
    ui->pushButton_2  ->setProperty("iehxCLEP", e_hxCCEP_Parall);
    ui->pushButton_12 ->setProperty("iehxCLEP", e_hxCCEP_Perp_Parall);
    ui->pushButton_3  ->setProperty("iehxCLEP", e_hxCCEP_deltaZ);
    ui->pushButton_13 ->setProperty("iehxCLEP", e_hxCCEP_Perp_deltaZ);
    ui->pushButton_23 ->setProperty("iehxCLEP", e_hxCCEP_Parall_deltaZ);
    ui->pushButton_123->setProperty("iehxCLEP", e_hxCCEP_All);


    //@LP let widget names as 1,2,3
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_Perp         , ui->pushButton_1);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_Parall       , ui->pushButton_2);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_Perp_Parall  , ui->pushButton_12);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_deltaZ       , ui->pushButton_3);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_Perp_deltaZ  , ui->pushButton_13);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_Parall_deltaZ, ui->pushButton_23);
    _qhash_int_PushButtonsPtr.insert(e_hxCCEP_All          , ui->pushButton_123);

    for (int iButtonKey = e_hxCCEP_Perp; iButtonKey <= e_hxCCEP_All; iButtonKey++) {
        auto iterpushButtonPtr = _qhash_int_PushButtonsPtr.find(iButtonKey);
        if (iterpushButtonPtr != _qhash_int_PushButtonsPtr.end()) {
            connect(iterpushButtonPtr.value(), &QPushButton::clicked, this, &ComponentsSwitcher::slot_pushButtonClicked);
        }
    }
}

void ComponentsSwitcher::setButtonsName(const QHash<e_hexaCodeComponentEditProfil, QString> qhash_e_hxCCEP_qstr) {
    for (auto iterKeyName = qhash_e_hxCCEP_qstr.cbegin(), end = qhash_e_hxCCEP_qstr.cend(); iterKeyName != end; ++iterKeyName) {
        auto iterFound_pushButtonPtr = _qhash_int_PushButtonsPtr.find(iterKeyName.key());
        if (iterFound_pushButtonPtr != _qhash_int_PushButtonsPtr.end()) {
            iterFound_pushButtonPtr.value()->setText(iterKeyName.value());
        }
    }
}

void ComponentsSwitcher::updateButtonsAvailability_setDefaultState(const QVector<bool>& qvectbLayersComputed, bool bSetToDefaultState) {

    qDebug() << __FUNCTION__ << " (ComponentsSwitcher)";

    if (qvectbLayersComputed.size() != 3 ) {
        return;
    }
    _qvectbLayersComputed = qvectbLayersComputed;
    qDebug() << __FUNCTION__ << " (ComponentsSwitcher) _qvectbLayersComputed =" << _qvectbLayersComputed;

    //hide all
    for (int iButtonKey = e_hxCCEP_Perp; iButtonKey <= e_hxCCEP_All; iButtonKey++) {
        setVisibleButtonForHashKey(iButtonKey, false);
    }

    int countButtonsToShow = _qvectbLayersComputed.count(true);
    if (countButtonsToShow == 1) { //only one layer, no layer switch possible
        return;
    }

    int hexaCodeLayers =
              qvectbLayersComputed[0]
            |(qvectbLayersComputed[1]<<1)
            |(qvectbLayersComputed[2]<<2);
    qDebug() << __FUNCTION__ << " (ComponentsSwitcher)  hexaCodeLayers= " << hexaCodeLayers;

    for (int iButtonKey = e_hxCCEP_Perp; iButtonKey <= e_hxCCEP_All; iButtonKey++) {
        qDebug() << __FUNCTION__ << " (ComponentsSwitcher)  iButtonKey " << iButtonKey;
        qDebug() << __FUNCTION__ << " (ComponentsSwitcher)  iButtonKey & hexaCodeLayers: " << (iButtonKey & hexaCodeLayers);

        if ((iButtonKey & hexaCodeLayers) == iButtonKey) {
            setVisibleButtonForHashKey(iButtonKey, true);
            qDebug() << "  (ComponentsSwitcher)  => show button :" << iButtonKey;
        }
    }

    if (bSetToDefaultState) {
        switch(hexaCodeLayers) {
            case e_hxCCEP_Perp_Parall:
                qDebug() << __FUNCTION__ << "  (ComponentsSwitcher)  case e_hxCCEP_Perp_Parall";
                switch_pushButton_forNewState(ui->pushButton_12);
                break;
            case e_hxCCEP_All:
                qDebug() << __FUNCTION__ << "  (ComponentsSwitcher)  case e_hxCCEP_All";
                switch_pushButton_forNewState(ui->pushButton_123);
                break;
            default:
                break;
        }
    }
}

void ComponentsSwitcher::setVisibleButtonForHashKey(int key, bool bVisible) {
    auto iterpushButtonPtr = _qhash_int_PushButtonsPtr.find(key);
    if (iterpushButtonPtr != _qhash_int_PushButtonsPtr.end()) {
        iterpushButtonPtr.value()->setVisible(bVisible);
    }
}


void ComponentsSwitcher::slot_pushButtonClicked() {

    QObject *ptrSender = sender();

    qDebug() << __FUNCTION__ << "sender()->objectName(); = " << sender()->objectName();

    QVariant qvariant_associated_iehxCLEP = ptrSender->property("iehxCLEP");
    bool bOk = false;
    int int_associated_iehxCLEP  = qvariant_associated_iehxCLEP.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associated_iehxCLEP.toInt()";
        return;
    }

    if (  (int_associated_iehxCLEP < e_hxCCEP_Perp)
        ||(int_associated_iehxCLEP > e_hxCCEP_All)) {
        qDebug() << __FUNCTION__ << " error: invalid int_associated_iehxCLEP ( " << int_associated_iehxCLEP << " )";
        return;
    }

    switch_pushButton_forNewState(dynamic_cast<QPushButton*>(ptrSender));

    emit signal_pushButtonClicked(int_associated_iehxCLEP);
}

void ComponentsSwitcher::switch_pushButton_forNewState(QPushButton* pushButtonPtr_ofNewState) {
    //enable the pushbutton about the state we leave:
    if (_qpushButtonPtr_ofPreviousState) {
        _qpushButtonPtr_ofPreviousState->setEnabled(true);
    }
    //disable the pushbutton about the new state
    pushButtonPtr_ofNewState->setEnabled(false);
    _qpushButtonPtr_ofPreviousState = pushButtonPtr_ofNewState;
}

void ComponentsSwitcher::clearState() {
    if (_qpushButtonPtr_ofPreviousState) {
        _qpushButtonPtr_ofPreviousState->setEnabled(true);
    }
    _qpushButtonPtr_ofPreviousState = nullptr;
    _qvectbLayersComputed.clear();
}

ComponentsSwitcher::~ComponentsSwitcher() {
    delete ui;
}
