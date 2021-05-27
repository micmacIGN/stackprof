#include <QGraphicsView>
#include <QDebug>

#include <QMouseEvent>

#include "layersSwitcher.h"

#include "../../logic/model/core/ComputationCore_structures.h" //for eLA_* ##LP move enum eLA_ to dedicated header

#include <QStackedWidget>

LayersSwitcher::LayersSwitcher(QWidget *parent) : QWidget(parent),
    _ieLA_currentLayer(eLA_Invalid),
    _bUpdatingButtonsAvailability(false),
    _bEmitSignalAboutLayerSwitch(false) {

    qDebug() << __FUNCTION__;

    ui = new Ui::LayersButtonsSwitcher;
    ui->setupUi(this);

    for (int i = eLA_PX1; i <= eLA_deltaZ; i++) {
        _qvect_widgetsForTabWidgets.push_back(new QWidget);
    }

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &LayersSwitcher::slot_tabWidget_index_currentChanged);
}


void LayersSwitcher::setButtonsName(const QHash<int, QString>& qhash_int_qstr) {
    _qhash_int_qstrTabTitle = qhash_int_qstr;
}

bool LayersSwitcher::setStateSetOn(int idxLayerOn) {
    qDebug() << __FUNCTION__ << "(LayersSwitcher) idxLayerOn= " << idxLayerOn;

    if ((idxLayerOn < 0)||(idxLayerOn > 2)) {
        return(false);
    }
    qDebug() << __FUNCTION__ << "(LayersSwitcher) ui->tabWidget->setCurrentIndex( " << idxLayerOn << " )";

    _bEmitSignalAboutLayerSwitch = false;

    ui->tabWidget->setCurrentIndex(idxLayerOn);

    _bEmitSignalAboutLayerSwitch = true;

    return(true);
}

void LayersSwitcher::set_enableLayerSwitching(bool bEnable) {
    ui->tabWidget->setEnabled(bEnable);
}

void LayersSwitcher::updateButtonsAvailability(const QVector<bool>& qvectsAvailableLayers) {

    qDebug() << __FUNCTION__ << "(LayersSwitcher)";

    if (qvectsAvailableLayers.size() != 3 ) {
        qDebug() << __FUNCTION__ << "(LayersSwitcher) if (qvectsAvailableLayers.size() != 3 ) {";
        return;
    }
    _qvectsAvailableLayers = qvectsAvailableLayers;
    qDebug() << __FUNCTION__ << "(LayersSwitcher) _qvectsAvailableLayers =" << _qvectsAvailableLayers;

    _bUpdatingButtonsAvailability = true;
    _bEmitSignalAboutLayerSwitch = false;

    ui->tabWidget->setUpdatesEnabled(false);
    ui->tabWidget->clear();

    qDebug() << __FUNCTION__ << "while (ui->tabWidget->count()) { after loop: " << ui->tabWidget->count();

    int countButtonsToShow = _qvectsAvailableLayers.count(true);
    if (countButtonsToShow == 1) { //only one layer, no layer switch possible //DeltaZ alone => tabwidget not visible
        qDebug() << __FUNCTION__ << "(LayersSwitcher) if (countButtonsToShow == 1)";

        ui->tabWidget->setUpdatesEnabled(true);
        return;
    }

    //@LP: setTabVisible is available in Qt 5.15
    //Hence handle differently with Qt 5.13

    //DeltaZ alone => tabwidget not visible
    //
    //PX1, PX2 => tabwidget with 2 tabs
    //PX1, PX2, DeltaZ => tabwidget with 3 tabs


    //add tabs for PX2 and DeltaZ if requiered with according title
    for (int iButtonKey = eLA_PX1; iButtonKey <= eLA_deltaZ; iButtonKey++) {

        qDebug() << __FUNCTION__ << "(LayersSwitcher) check if insert tab required for iButtonKey1=" << iButtonKey;

         if (_qvectsAvailableLayers[iButtonKey]) {

             QString strDefaultTitle = "Layer" + QString::number(iButtonKey);
             QString strTitle = _qhash_int_qstrTabTitle.value(iButtonKey, strDefaultTitle);

             int resultIdx = ui->tabWidget->insertTab(iButtonKey, _qvect_widgetsForTabWidgets[iButtonKey], strTitle);

             qDebug() << __FUNCTION__ << "(LayersSwitcher) strTitle for iButtonKey=" << iButtonKey << " = " << strTitle << " resultIdx : " << resultIdx;

         }

    }

    ui->tabWidget->setUpdatesEnabled(true);

    _bUpdatingButtonsAvailability = false;
    _bEmitSignalAboutLayerSwitch = true;

}


void LayersSwitcher::slot_tabWidget_index_currentChanged(int index) {

    qDebug() << __FUNCTION__ << "(LayersSwitcher)" << " index = " << index;

    qDebug() << __FUNCTION__ << "(LayersSwitcher)" << " _bUpdatingButtonsAvailability = " << _bUpdatingButtonsAvailability;
    qDebug() << __FUNCTION__ << "(LayersSwitcher)" << " _bEmitSignalAboutLayerSwitch = " << _bEmitSignalAboutLayerSwitch;

    if (  (!_bUpdatingButtonsAvailability)
        &&(_bEmitSignalAboutLayerSwitch) ) {

        qDebug() << __FUNCTION__ << "(LayersSwitcher)" << "emit signal_pushButtonClicked( " << index;

        emit signal_pushButtonClicked(index);

    }
}


LayersSwitcher::~LayersSwitcher() {

    ui->tabWidget->clear();

    qDeleteAll(_qvect_widgetsForTabWidgets);

    delete ui;
}
