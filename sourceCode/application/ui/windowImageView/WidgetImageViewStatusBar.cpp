#include <QDebug>

#include "WidgetImageViewStatusBar.h"

#include "../logic/model/geoUiInfos/GeoUiInfos.hpp"

WidgetImageViewStatusBar::WidgetImageViewStatusBar(QWidget *parent) : QWidget(parent) {

    qDebug() << __FUNCTION__;

    ui = new Ui::widgetImageViewStatusBar;
    ui->setupUi(this);

    setToNoValues();

    connect(ui->pushButton_setZLITo1, &QPushButton::clicked, this, &WidgetImageViewStatusBar::signal_setGlobalZoomFactorTo1); //direct forward

    _tbLimitReached_zoomStepPlusMinus[e_AZSIDlRa_plus_increase] = false;
    _tbLimitReached_zoomStepPlusMinus[e_AZSIDlRa_minus_decrease] = false;

    connect(ui->pushButton_zoomMinus, &QPushButton::clicked, this, &WidgetImageViewStatusBar::slot_zoomMinus);
    connect(ui->pushButton_zoomPlus , &QPushButton::clicked, this, &WidgetImageViewStatusBar::slot_zoomPlus);

    ui->pushButton_setZLITo1->setAutoDefault (false);

    ui->label_pixelValue->setFocus(); //to avoid to have focus on pushButton_setZLITo1, which would appears focused and very too visible on Mac OS X (blue background)

}

void WidgetImageViewStatusBar::setEnable_setGlobalZoomFactorTo1(bool bState) {
    ui->pushButton_setZLITo1->setEnabled(bState);
}

void WidgetImageViewStatusBar::setWidgetFocusOnZoomPushButton(bool bIsZoomStepInc) {
    /*if (bIsZoomStepInc) {
        ui->pushButton_zoomPlus->setFocus();
    } else {
        ui->pushButton_zoomMinus->setFocus();
    }*/
    //ui->label_pixelValue->setFocus(); //to avoid to have focus on pushButton_setZLITo1, which would appears focused and very too visible on Mac OS X (blue background)
}

#if false
void WidgetImageViewStatusBar::setEnableAuthorization_zoomStepIncDec(bool bEnableInc, bool bEnableDec) {
    if (bEnableInc) { //action authorized
        if (!_tbLimitReached_zoomStepPlusMinus[e_AZSIDlRa_plus_increase]) { //limit not reached, hence enable the button
            ui->pushButton_zoomPlus->setEnabled(bEnableInc);
        }/* else { //limit reached
            //do not enable the button
            //(do nothing)
        }*/
    } else { //forbid action
        ui->pushButton_zoomPlus->setEnabled(bEnableInc); //disable the button
    }

    if (bEnableDec) { //action authorized
        if (!_tbLimitReached_zoomStepPlusMinus[e_AZSIDlRa_minus_decrease]) { //limit not reached, hence enable the button
            ui->pushButton_zoomMinus->setEnabled(bEnableDec);
        }/* else { //limit reached
            //do not enable the button
            //(do nothing)
        }*/
    } else { //forbid action
        ui->pushButton_zoomMinus->setEnabled(bEnableDec); //disable the button
    }
}
#endif


void WidgetImageViewStatusBar::setEnable_zoomStepIncDec(bool bEnableInc, bool bEnableDec) {
   setEnable_zoomStepInc(bEnableInc);
   setEnable_zoomStepDec(bEnableDec);
}

void WidgetImageViewStatusBar::setEnable_zoomStepInc(bool bEnableInc) {
    ui->pushButton_zoomPlus->setEnabled(bEnableInc);
}

void WidgetImageViewStatusBar::setEnable_zoomStepDec(bool bEnableDec) {
    ui->pushButton_zoomMinus->setEnabled(bEnableDec);
}

void WidgetImageViewStatusBar::setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode eCoordinateDisplayMode) {
    if (!ui) {
        return;
    }
    _eCoordinateDisplayMode = eCoordinateDisplayMode;
    QString strCoordinateTypeLegend;
    switch (_eCoordinateDisplayMode) {
        case GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84:
            strCoordinateTypeLegend = "WGS84 (Lat, Lon):";
            break;
        case GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY:
            strCoordinateTypeLegend = "pixel (x, y):";
            break;
    }
    ui->label_coordinatesType->setText(strCoordinateTypeLegend);
}


void WidgetImageViewStatusBar::setToNoValues() {
    if (!ui) {
        return;
    }
    ui->lineEdit_coordinatesValue->setText("---, ---");
    ui->lineEdit_pixelValue->setText("---");
    ui->label_zoomLevel->setText("---");

    setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);
}

/*void WidgetImageViewStatusBar::slot_strLonLat_strPixelValue(QString strLonLat, QString strPixelValue) {
    set_strCoordinatesValue(strLonLat);
    set_strPixelValue(strPixelValue);
}*/

void WidgetImageViewStatusBar::slot_strLatLon_strPixelValue(QString strLatLon, QString strPixelValue) {
    set_strCoordinatesValue(strLatLon);
    set_strPixelValue(strPixelValue);
}

void WidgetImageViewStatusBar::slot_strPixelImageXY_strPixelValue(QString strPixelImageXY, QString strPixelValue) {
    qDebug() << __FUNCTION__;
    set_strCoordinatesValue(strPixelImageXY);
    set_strPixelValue(strPixelValue);
}

void WidgetImageViewStatusBar::slot_currentZLIChanged(int ZLI) {
    set_zoomLevelImageValue(ZLI);
}

void WidgetImageViewStatusBar::set_strCoordinatesValue(const QString& strCoordinatesValue) {
    if (!ui) {
        return;
    }
    ui->lineEdit_coordinatesValue->setText(strCoordinatesValue);
}

/*
set_strCoordinates
void WidgetImageViewStatusBar::slot_strCoordinates_strPixelValue(QString strCoordinates, QString strPixelValue) {
*/

void WidgetImageViewStatusBar::set_strPixelValue(const QString& strPixelValue) {
    if (!ui) {
        return;
    }
    ui->lineEdit_pixelValue->setText(strPixelValue);
}

void WidgetImageViewStatusBar::set_zoomLevelImageValue(int ZLI) {
    if (!ui) {
        return;
    }
    if (!ZLI) {
        ui->label_zoomLevel->setText("---");
    } else {
        //QString strZLI = "ZLI: 1/" + QString::number(ZLI);
        //QString strZLI = "Image Pyramid Level: 1/" + QString::number(ZLI);
        QString strZLI = "PyramLvl: 1/" + QString::number(ZLI);
        //QString strZLI = "1/" + QString::number(ZLI);
        ui->label_zoomLevel->setText(strZLI);
    }
}

void WidgetImageViewStatusBar::slot_zoomMinus() {
    emit signal_zoomStepIncDec(false);
}

void WidgetImageViewStatusBar::slot_zoomPlus() {
    emit signal_zoomStepIncDec(true);
}

WidgetImageViewStatusBar::~WidgetImageViewStatusBar() {
    delete ui;
}
