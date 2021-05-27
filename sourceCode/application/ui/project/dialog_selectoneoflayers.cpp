#include <QVector>
#include <QDebug>

#include "dialog_selectoneoflayers.h"
#include "ui_dialog_selectoneoflayers.h"

Dialog_selectOneOfLayers::Dialog_selectOneOfLayers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_selectOneOfLayers), _tRadioButtonPtr {nullptr, nullptr, nullptr},
    _selected_iLayer(-1) {
    ui->setupUi(this);

    _tRadioButtonPtr[0] = ui->radioButton_L1;
    _tRadioButtonPtr[1] = ui->radioButton_L2;
    _tRadioButtonPtr[2] = ui->radioButton_L3;

    for (int i = 0; i < 3; i++) {
        _tRadioButtonPtr[i]->setProperty("iLayer", i);
        connect(_tRadioButtonPtr[i], &QRadioButton::clicked, this, &Dialog_selectOneOfLayers::slot_radioButtonClicked);
    }
}

void Dialog_selectOneOfLayers::slot_radioButtonClicked() {

    QObject *ptrSender = sender();

    QVariant qvariant_iLayer = ptrSender->property("iLayer");
    bool bOk = false;
    int iLayer = qvariant_iLayer.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_iLayer.toInt()";
        return;
    }
    _selected_iLayer = iLayer;
}

void Dialog_selectOneOfLayers::setLayersName(const QVector<QString> qVectStr) {
    if (qVectStr.size() != 3) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        _tRadioButtonPtr[i]->setText(qVectStr[i]);
    }
}

void Dialog_selectOneOfLayers::setAvailableLayers(const QVector<bool>& qvectBAvailableLayers) {

    if (qvectBAvailableLayers.size() != 3) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        _tRadioButtonPtr[i]->setVisible(false);
    }
    for (int i = 0; i < 3; i++) {
        _tRadioButtonPtr[i]->setVisible(qvectBAvailableLayers[i]);
    }
    _selected_iLayer = qvectBAvailableLayers.indexOf(true); //@LP if none, will be -1
    if (_selected_iLayer >= 0) {
        _tRadioButtonPtr[_selected_iLayer]->setChecked(true);
    }
}

int Dialog_selectOneOfLayers::getSelected() {
    return(_selected_iLayer);
}

Dialog_selectOneOfLayers::~Dialog_selectOneOfLayers() {
    delete ui;
}
