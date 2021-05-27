#include "WindowImageView.h"
#include "ui_windowimageview.h"

#include "../logic/model/imageScene/customgraphicsscene_usingTPDO.h"

#include <QDebug>

#include "../logic/model/geoUiInfos/GeoUiInfos.hpp"

#include <QWindow>

WindowImageView::WindowImageView(QWidget *parent) : QDialog(parent), ui(new Ui::WindowImageView),

    _ptrCustomGraphicsScene(nullptr),
    _ptrZoomHandler(nullptr),
    _ptrGeoUiInfos(nullptr) {

    _timerId = 0;

    _bFirstShow = true;

    ui->setupUi(this);

    //setWindowFlags(Qt::Window); //add WindowMaximizeButtonHint, WindowMinimizeButtonHint and WindowCloseButtonHint

    //
    //dialog
    /*Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags( flags );*/

    connect(ui->widgetImgViewStatusBar, &WidgetImageViewStatusBar::signal_setGlobalZoomFactorTo1, this, &WindowImageView::signal_setGlobalZoomFactorTo1); //direct forward

    connect(ui->widgetImgViewStatusBar, &WidgetImageViewStatusBar::signal_zoomStepIncDec, this, &WindowImageView::signal_zoomStepIncDec); //direct forward

    qDebug() << __FUNCTION__ << "devicePixelRatio() =" << /*QWindow::*/devicePixelRatio();

    //#define DEF_LP_debug_trace_file_devicePixelRatio
    #ifdef DEF_LP_debug_trace_file_devicePixelRatio

        QString qstrDbg0 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- start -----\n\r";
        QString qstrDbg1 = QString(__FUNCTION__) + "devicePixelRatio() = "  + QString::number(devicePixelRatio())  + "\n\r";
        QString qstrDbg2 = QString(__FUNCTION__) + "devicePixelRatioF() = " + QString::number(devicePixelRatioF()) + "\n\r";
        QString qstrDbg3 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- end -----\n\r";

        QFile qfdebug("/tmp/devicePixelRatio_log.txt");
        if (qfdebug.open(QIODevice::Append)) {
            qfdebug.write(qstrDbg0.toStdString().c_str());
            qfdebug.write(qstrDbg1.toStdString().c_str());
            qfdebug.write(qstrDbg2.toStdString().c_str());
            qfdebug.write(qstrDbg3.toStdString().c_str());
            qfdebug.close();
        }
    #endif

}

void WindowImageView::setZoomHandlerptr(ZoomHandler* ptrZoomHandler) {
    _ptrZoomHandler = ptrZoomHandler;
}

void WindowImageView::setCustomGraphicsScenePtr(CustomGraphicsScene *ptrCustomGraphicsScene) {
    _ptrCustomGraphicsScene = ptrCustomGraphicsScene;
}

CustomGraphicsScene* WindowImageView::getCustomGraphicsScenePtr() {
    return(_ptrCustomGraphicsScene);
}

void WindowImageView::setGeoUiInfosPtr(GeoUiInfos *ptrGeoUiInfos) {
    _ptrGeoUiInfos = ptrGeoUiInfos;
}

void WindowImageView::setVisible_layersSwitcher(bool bVisible) {
    ui->widgetImgViewUpBar->setVisible_layersSwitcher(bVisible);
}

void WindowImageView::setLayersNamesForLayersSwitcher(const QVector<QString>& qvectStrLayersName) {
     ui->widgetImgViewUpBar->setLayersNamesForLayersSwitcher(qvectStrLayersName);
}

void WindowImageView::setAvailableLayersForLayersSwitcher(const QVector<bool>& qvectb_layersToUseForCompute, int idxLayerToDisplayAsBackgroundImage) {
    qDebug() << __FUNCTION__ << "(WindowImageView)";
    ui->widgetImgViewUpBar->setAvailablelayersForLayersSwitcher(qvectb_layersToUseForCompute);
    ui->widgetImgViewUpBar->setStateSetOn(idxLayerToDisplayAsBackgroundImage);
}

void WindowImageView::showEvent(QShowEvent *event) {
    qDebug() << __FUNCTION__ << "(WindowImageView)";
    qDebug() << __FUNCTION__ << "(WindowImageView)" <<  ui->graphicsView->viewport()->size();
    if (_bFirstShow) {
        _bFirstShow = false;
        if (_ptrCustomGraphicsScene) {
            _ptrCustomGraphicsScene->initSceneToFitImageInWindowView(ui->graphicsView->viewport()->size());
        }
    }
}

void WindowImageView::setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setCoordinateDisplayMode(eCoordinateDisplayMode);
}

//avoiding using hide() as a method name for easiest track usage of _bFirstShow
void WindowImageView::hide_resetbFirstShow() {
    hide();
    _bFirstShow = true;
}

void WindowImageView::initConnectionModelViewSignalsSlots() {

    if (!_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "(WindowImageView)" << " _ptrCustomGraphicsScene is nullptr";
        return;
    }
    if (!_ptrZoomHandler) {
        qDebug() << __FUNCTION__ << "(WindowImageView)" << " _ptrZoomHandler is nullptr";
        return;
    }
    if (!_ptrGeoUiInfos) {
        qDebug() << __FUNCTION__ << "(WindowImageView)" << " _ptrGeoUiInfos is nullptr";
        return;
    }

    ui->graphicsView->setScene(_ptrCustomGraphicsScene);


    connect(       ui->graphicsView, &CustomGraphicsView::signal_endOfPan_onImageArea,
            _ptrCustomGraphicsScene, &CustomGraphicsScene::slot_updateTiles);

    connect(       ui->graphicsView, &CustomGraphicsView::signal_endOfPan_usingScrollBar,
            _ptrCustomGraphicsScene, &CustomGraphicsScene::slot_updateTiles);


    connect(       ui->graphicsView, &CustomGraphicsView::signal_zoomRequest,
            _ptrCustomGraphicsScene, &CustomGraphicsScene::slot_zoomRequest);

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_adjustView,
                   ui->graphicsView, &CustomGraphicsView::slot_adjustView);


    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_setCenterOn,
                   ui->graphicsView, &CustomGraphicsView::slot_setCenterOn);


    /*ui->verticalSliderTestZoom->setMinimum(0);
    qDebug() << __FUNCTION__ << "_zoomHandler.getIdxMax() =" << _ptrZoomHandler->getIdxMax();
    ui->verticalSliderTestZoom->setMaximum(_ptrZoomHandler->getIdxMax());
    connect(ui->verticalSliderTestZoom, &QSlider::valueChanged, this, &WindowImageView::slotTest_ZoomSliderValueChanged);*/

    connect(            this, &WindowImageView::signal_endOfResize,
            _ptrCustomGraphicsScene, &CustomGraphicsScene::slot_updateTiles);
    connect(            this, &WindowImageView::signal_endOfResize,
            ui->graphicsView, &CustomGraphicsView::slot_endOfResize);

    connect(ui->graphicsView, &CustomGraphicsView::signal_visibleAreaChanged,
            _ptrCustomGraphicsScene, &CustomGraphicsScene::slot_visibleAreaChanged);

    //connect(_ptrGeoUiInfos, &GeoUiInfos::signal_strLonLat_strPixelValue,
    connect(_ptrGeoUiInfos, &GeoUiInfos::signal_strLatLon_strPixelValue,
            ui->widgetImgViewStatusBar, &WidgetImageViewStatusBar::slot_strLatLon_strPixelValue);
    connect(_ptrGeoUiInfos, &GeoUiInfos::signal_strPixelImageXY_strPixelValue,
            ui->widgetImgViewStatusBar, &WidgetImageViewStatusBar::slot_strPixelImageXY_strPixelValue);

    connect(ui->widgetImgViewUpBar, &WidgetImageViewUpBar::signal_layersSwitcher_pushButtonClicked,
            this, &WindowImageView::slot_layersSwitcher_pushButtonClicked);

    connect(ui->widgetImgViewUpBar, &WidgetImageViewUpBar::signal_fitImageInView,
            this, &WindowImageView::slot_callSceneToFitImageInWindowView);

    connect(ui->graphicsView, &CustomGraphicsView::signal_showPixelGridIsPossible,
            ui->widgetImgViewUpBar, &WidgetImageViewUpBar::slot_showPixelGridIsPossible);

    connect(ui->widgetImgViewUpBar, &WidgetImageViewUpBar::signal_showPixelGrid,
            ui->graphicsView, &CustomGraphicsView::slot_showPixelGrid);
}


void WindowImageView::slot_layersSwitcher_pushButtonClicked(int ieLA) {
    //@LP let a chance here to the WindowImageView to set some internal flags if need to handle the transition
    emit signal_layersSwitcher_pushButtonClicked(ieLA);
}

void WindowImageView::slot_forUserInfo_currentZLIChanged(int ZLI) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->slot_currentZLIChanged(ZLI);
}

void WindowImageView::slot_enablePossibleActionsOnImageView(bool bEnable) {
//void WindowImageView::slot_enableLayerSwitching(bool bEnable) {
    if (!ui) { return; }
    ui->widgetImgViewUpBar->set_enableLayerSwitching(bEnable);
    ui->graphicsView->set_enablePanOnImage(bEnable);
    ui->widgetImgViewUpBar->set_enablePossibleActionsOnImage(bEnable);

}

void WindowImageView::slot_setEnable_setGlobalZoomFactorTo1(bool bEnable) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setEnable_setGlobalZoomFactorTo1(bEnable);
    ui->graphicsView->setFocus();//reset focus on graphicview to avoid to have focus on pushbutton '1:1' or any other widgets
}


void WindowImageView::slot_enable_zoomStepIncDec(bool bEnableInc, bool bEnableDec) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setEnable_zoomStepIncDec(bEnableInc, bEnableDec);
    ui->graphicsView->setFocus();//reset focus on graphicview to avoid to have focus on pushbutton '1:1' or any other widgets
}

void WindowImageView::slot_enable_zoomStepInc(bool bEnableInc) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setEnable_zoomStepInc(bEnableInc);
    ui->graphicsView->setFocus();//reset focus on graphicview to avoid to have focus on pushbutton '1:1' or any other widgets
}

void WindowImageView::slot_enable_zoomStepDec(bool bEnableDec) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setEnable_zoomStepDec(bEnableDec);
    ui->graphicsView->setFocus();//reset focus on graphicview to avoid to have focus on pushbutton '1:1' or any other widgets
}

void WindowImageView::slot_setFocusWidgetOnZoomStepIncOrDec(bool bsetOnStepInc) {
    if (!ui) { return; }
    ui->widgetImgViewStatusBar->setWidgetFocusOnZoomPushButton(bsetOnStepInc);
    ui->graphicsView->setFocus();//reset focus on graphicview to avoid to have focus on pushbutton '1:1' or any other widgets
}

WindowImageView::~WindowImageView() {
    delete ui;
}

/*void WindowImageView::slotTest_ZoomSliderValueChanged(int value) {
    if (_ptrZoomHandler) {
        if (_ptrZoomHandler->setToZoomFromIdx(value)) {
            qDebug() << __FUNCTION__ << ":invalidate=>";
            emit signal_zoomChanged();
        }
    }
}*/

/*
void WindowImageView::slot_debug_valueChanged(int v) {
    qDebug() << __FUNCTION__ << ": " << v;
}
*/

//way to update scene and view updating tiles at end of resizing the window:
//https://stackoverflow.com/questions/50200972/how-to-resize-qgraphicsview-properly
void WindowImageView::resizeEvent(QResizeEvent *){
    qDebug() << __FUNCTION__ << "(WindowImageView)";
    if (_timerId){
        killTimer(_timerId);
        _timerId = 0;
    }
    _timerId = startTimer(750); //delay beetween ends of resize and the action to do
}

void WindowImageView::timerEvent(QTimerEvent *te) {
    qDebug() << __FUNCTION__<< "(WindowImageView)";

    killTimer(te->timerId());
    _timerId = 0;

    emit signal_endOfResize();
}

void WindowImageView::slot_callSceneToFitImageInWindowView() {
    qDebug() << __FUNCTION__ << "(WindowImageView)";
    if (!ui) { return; }
    qDebug() << __FUNCTION__ << "(WindowImageView)" <<  ui->graphicsView->viewport()->size();
    if (_ptrCustomGraphicsScene) {
        _ptrCustomGraphicsScene->initSceneToFitImageInWindowView(ui->graphicsView->viewport()->size());
    }
}

void WindowImageView::slot_switchShowGridState() {
    if (!ui) { return; }
    ui->widgetImgViewUpBar->switchShowGridState();
}

