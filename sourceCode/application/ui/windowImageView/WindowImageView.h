#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

#include "../logic/model/geoUiInfos/GeoUiInfos.hpp"

class ZoomHandler;
class CustomGraphicsScene;
class GeoUiInfos;

namespace Ui {
class WindowImageView;
}

class WindowImageView : public QDialog
{
    Q_OBJECT

public:
    explicit WindowImageView(QWidget *parent = nullptr);

    void setZoomHandlerptr(ZoomHandler* ptrZoomHandler);
    void setCustomGraphicsScenePtr(CustomGraphicsScene *ptrCustomGraphicsScene);
    void setGeoUiInfosPtr(GeoUiInfos *ptrGeoUiInfos);

    CustomGraphicsScene* getCustomGraphicsScenePtr();

    void setVisible_layersSwitcher(bool bVisible);
    void setLayersNamesForLayersSwitcher(const QVector<QString>& qvectStrLayersName);
    void setAvailableLayersForLayersSwitcher(const QVector<bool>& qvectb_layersToUseForCompute, int idxLayerToDisplayAsBackgroundImage);

    void setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode);

    void hide_resetbFirstShow();

    void initConnectionModelViewSignalsSlots();

    ~WindowImageView();

    void resizeEvent(QResizeEvent* pEvent);

    void timerEvent(QTimerEvent *te);

    void showEvent(QShowEvent *event);

signals:

    void signal_zoomChanged();

    void signal_endOfResize();

    void signal_layersSwitcher_pushButtonClicked(int ieLA); //to the controller

    void signal_setGlobalZoomFactorTo1();
    void signal_zoomStepIncDec(bool bStepIsIncrease);

public slots:

    //void slot_debug_valueChanged(int v);

    //void slotTest_ZoomSliderValueChanged(int value);

    void slot_layersSwitcher_pushButtonClicked(int ieLA); //from the up Bar

    void slot_forUserInfo_currentZLIChanged(int ZLI);

    //void slot_enableLayerSwitching(bool bEnable);
    void slot_enablePossibleActionsOnImageView(bool bEnable);

    void slot_setEnable_setGlobalZoomFactorTo1(bool bEnable);

    void slot_callSceneToFitImageInWindowView();

    void slot_enable_zoomStepIncDec(bool bEnableInc, bool bEnableDec);
    void slot_enable_zoomStepInc(bool bEnableInc);
    void slot_enable_zoomStepDec(bool bEnableDec);

    void slot_setFocusWidgetOnZoomStepIncOrDec(bool bsetOnStepInc);

    void slot_switchShowGridState();

private:
    Ui::WindowImageView *ui;

    CustomGraphicsScene *_ptrCustomGraphicsScene;
    ZoomHandler *_ptrZoomHandler;
    GeoUiInfos *_ptrGeoUiInfos;

    int _timerId;

    bool _bFirstShow;

};

#endif // MAINWINDOW_H
