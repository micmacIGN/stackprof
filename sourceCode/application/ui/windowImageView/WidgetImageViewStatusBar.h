#ifndef widgetImageViewStatusBar_H
#define widgetImageViewStatusBar_H

#include <QGraphicsView>

#include "ui_widgetImageviewStatusBar.h"

#include "../logic/model/geoUiInfos/GeoUiInfos.hpp"

namespace Ui {
    class widgetImageViewStatusBar;
}

class WidgetImageViewStatusBar : public QWidget {

  Q_OBJECT

public:
    WidgetImageViewStatusBar(QWidget *parent = nullptr);

    ~WidgetImageViewStatusBar() override;

    void setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode);

    void setToNoValues();

    void setEnable_setGlobalZoomFactorTo1(bool bState);

    //void setEnableAuthorization_zoomStepIncDec(bool bEnableInc, bool bEnableDec);
    void setEnable_zoomStepIncDec(bool bEnableInc, bool bEnableDec);
    void setEnable_zoomStepInc(bool bEnableInc);
    void setEnable_zoomStepDec(bool bEnableDec);

    void setWidgetFocusOnZoomPushButton(bool bIsZoomStepInc);

signals:  
    void signal_setGlobalZoomFactorTo1();

    void signal_zoomStepIncDec(bool bStepIsIncrease);

public slots:

    //void slot_strLonLat_strPixelValue(QString strLonLat, QString strPixelValue);
    void slot_strLatLon_strPixelValue(QString strLatLon, QString strPixelValue);

    void slot_strPixelImageXY_strPixelValue(QString strPixelImageXY, QString strPixelValue);

    void slot_currentZLIChanged(int ZLI);

    void slot_zoomMinus();
    void slot_zoomPlus();

private:

    //void set_strLonLat(const QString& strLonLat);
    //void set_strPixelImageXY(const QString& strPixelImageXY);
    void set_strCoordinatesValue(const QString& strCoordinatesValue);

    void set_strPixelValue(const QString& strPixelValue);
    void set_zoomLevelImageValue(int ZLI);

    Ui::widgetImageViewStatusBar *ui;

    enum e_AccessZoomStepIncDec_limitReached_array {
        e_AZSIDlRa_plus_increase = 0,
        e_AZSIDlRa_minus_decrease
    };

    bool _tbLimitReached_zoomStepPlusMinus[2];

    GeoUiInfos::e_CoordinateDisplayMode _eCoordinateDisplayMode;
};


#endif // widgetImageViewStatusBar_H
