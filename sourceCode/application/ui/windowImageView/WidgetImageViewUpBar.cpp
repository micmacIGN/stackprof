#include <QDebug>

#include "WidgetImageViewUpBar.h"

WidgetImageViewUpBar::WidgetImageViewUpBar(QWidget *parent) : QWidget(parent) {

    qDebug() << __FUNCTION__;

    ui = new Ui::widgetImageViewUpBar;
    ui->setupUi(this);

    _bEnabled_checkBox_showPixelGrid = false;

    connect(ui->wdgtLayersSwitcher, &LayersSwitcher::signal_pushButtonClicked,
            this, &WidgetImageViewUpBar::slot_layersSwitcher_pushButtonClicked);

    connect(ui->pushButton_fitImageInView, &QCheckBox::clicked, this, &WidgetImageViewUpBar::slot_pushButton_fitImageInView_clicked);
    connect(ui->checkBox_showPixelGrid, &QCheckBox::stateChanged, this, &WidgetImageViewUpBar::slot_checkBox_showPixelGrid_stateChanged);

}

void WidgetImageViewUpBar::slot_layersSwitcher_pushButtonClicked(int ieLA) {
    emit signal_layersSwitcher_pushButtonClicked(ieLA);
}

void WidgetImageViewUpBar::set_enableLayerSwitching(bool bEnable) {
    if (!ui) { return; }
    ui->wdgtLayersSwitcher->set_enableLayerSwitching(bEnable);
}

void WidgetImageViewUpBar::set_enablePossibleActionsOnImage(bool bEnable) {
    if (!ui) { return; }
    ui->pushButton_fitImageInView->setEnabled(bEnable);
    if (bEnable) {
        if (_bEnabled_checkBox_showPixelGrid) {
            ui->checkBox_showPixelGrid->setEnabled(true);
        }
    }
    ui->checkBox_showPixelGrid->setEnabled(bEnable);
}

void WidgetImageViewUpBar::switchShowGridState() {
    if (!ui) { return; }
    if (!_bEnabled_checkBox_showPixelGrid) {
        qDebug() << __FUNCTION__ << "if (!_bEnabled_checkBox_showPixelGrid) {";
        return;
    }
    bool bCheckedState = ui->checkBox_showPixelGrid->isChecked();
    bCheckedState = !bCheckedState;
    ui->checkBox_showPixelGrid->setChecked(bCheckedState); //QCheckBox::stateChanged will call slot_checkBox_showPixelGrid_stateChanged

    //qDebug() << __FUNCTION__ << "emit signal_showPixelGrid(" << bCheckedState << ")";
    //emit signal_showPixelGrid(bCheckedState);
}


void WidgetImageViewUpBar::slot_showPixelGridIsPossible(bool bIsPossible) {
    set_enableShowPixelGrid(bIsPossible);
}

//this method alters _bEnabled_checkBox_showPixelGrid
//Showing or not the grid does not make sens when the pixel size is too small.
//Hence, when the pixel size is too small, the grid is hidden and the checkBox_showPixelGrid is disabled
void WidgetImageViewUpBar::set_enableShowPixelGrid(bool bEnable) {
    if (!ui) { return; }
    _bEnabled_checkBox_showPixelGrid = bEnable;
    ui->checkBox_showPixelGrid->setEnabled(_bEnabled_checkBox_showPixelGrid);
}

void WidgetImageViewUpBar::setVisible_layersSwitcher(bool bVisible) {
    if (!ui) { return; }
    ui->wdgtLayersSwitcher->setVisible(bVisible);
}

void WidgetImageViewUpBar::setLayersNamesForLayersSwitcher(const QVector<QString>& qvectStrLayersName) {
    if (!ui) { return; }
    QHash<int, QString> qhash_int_qstr_buttonsName;
    for (int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
        qhash_int_qstr_buttonsName.insert(ieLA, qvectStrLayersName[ieLA]);
    }
    ui->wdgtLayersSwitcher->setButtonsName(qhash_int_qstr_buttonsName);
}

void WidgetImageViewUpBar::setAvailablelayersForLayersSwitcher(const QVector<bool>& qvectb_layersToUseForCompute) {
    qDebug() << __FUNCTION__ << "(WidgetImageViewUpBar)";
    if (!ui) { return; }
    if (qvectb_layersToUseForCompute.count(true) < 2) {
        setVisible_layersSwitcher(false);
    } else {
        setVisible_layersSwitcher(true);
        ui->wdgtLayersSwitcher->updateButtonsAvailability(qvectb_layersToUseForCompute);
    }
}

void WidgetImageViewUpBar::setStateSetOn(int idxLayerOn) {
    if (!ui) { return; }
    ui->wdgtLayersSwitcher->setStateSetOn(idxLayerOn);
}

WidgetImageViewUpBar::~WidgetImageViewUpBar() {
    delete ui;
}

void WidgetImageViewUpBar::slot_pushButton_fitImageInView_clicked() {
    emit signal_fitImageInView();
}

void WidgetImageViewUpBar::slot_checkBox_showPixelGrid_stateChanged(int iCheckState) {
    qDebug() << __FUNCTION__ ;
    bool bChecked = (iCheckState == Qt::Checked);
    qDebug() << __FUNCTION__ << "emit signal_showPixelGrid(" << bChecked << ")";
    emit signal_showPixelGrid(bChecked);
}

