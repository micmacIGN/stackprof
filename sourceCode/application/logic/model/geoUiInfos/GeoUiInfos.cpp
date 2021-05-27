#include <QDebug>

#include "GeoUiInfos.hpp"

#include "../georef/georefModel_imagesSet.hpp"
#include "../core/valueDequantization.hpp"

#include "../core/ComputationCore_structures.h" //for IndexLayer enum

#include "../../toolbox/toolbox_conversion.h"

GeoUiInfos::GeoUiInfos(): QObject(nullptr),
    _eCoordinateDisplayMode{eCDM_WGS84},
    _ptrGeoRefModel_imagesSet(nullptr),
    _ptrVectValueDequantization_PX1_PX2_deltaZ(nullptr),
    _ieLayerAccesForValueDequantization(eLA_Invalid) {

}

void GeoUiInfos::clear() {
    _ptrGeoRefModel_imagesSet = nullptr;
    _ptrVectValueDequantization_PX1_PX2_deltaZ = nullptr;
    _ieLayerAccesForValueDequantization = eLA_Invalid;
}

void GeoUiInfos::setPtrGeoRefModel_imagesSet(GeoRefModel_imagesSet* ptrGeoRefModel_imagesSet) {
    _ptrGeoRefModel_imagesSet = ptrGeoRefModel_imagesSet;
}

void GeoUiInfos::setCoordinateDisplayMode(e_CoordinateDisplayMode eCoordinateDisplayMode) {
    _eCoordinateDisplayMode = eCoordinateDisplayMode;
}

void GeoUiInfos::setPtrVectDequantization_PX1_PX2_deltaZ(vector<ValueDequantization> *ptrVectValueDequantization_PX1_PX2_deltaZ,
                                                         bool bClear_ieLayerAccesForValueDequantization) {
    qDebug() <<__FUNCTION__;
    if (ptrVectValueDequantization_PX1_PX2_deltaZ->size() != 3) {
        _ieLayerAccesForValueDequantization = eLA_Invalid;

        qDebug() <<__FUNCTION__ << "error: if (ptrVectValueDequantization_PX1_PX2_deltaZ->size() != 3) {";
        //@#LP error msg
        return;
    }
    _ptrVectValueDequantization_PX1_PX2_deltaZ = ptrVectValueDequantization_PX1_PX2_deltaZ;
    if (bClear_ieLayerAccesForValueDequantization) {
        _ieLayerAccesForValueDequantization = eLA_Invalid;
    }
    qDebug() <<__FUNCTION__ << "_ptrVectValueDequantization_PX1_PX2_deltaZ set, with _ieLayerAccesForValueDequantization at eLA_Invalid";

}

void GeoUiInfos::setIndexLayerForValueDequantization(size_t ieLayerAcces) {
    _ieLayerAccesForValueDequantization = ieLayerAcces;
}


void GeoUiInfos::slot_sceneMousePosF_with_pixelValue(QPointF sceneMousePosF_noZLI, float fPixelValue) {

    if (!_ptrVectValueDequantization_PX1_PX2_deltaZ) {
        qDebug() <<__FUNCTION__ << "error: _ptrValueDequantization is nullptr";
        return;
    }

    if (_ieLayerAccesForValueDequantization >= eLA_Invalid) {
        qDebug() <<__FUNCTION__ << "error: _ieLayerAccesForValueDequantization >= eLA_Invalid";
        return;
    }

    QString strPixelValue;
    float fPixelValueDequant = (*_ptrVectValueDequantization_PX1_PX2_deltaZ)[_ieLayerAccesForValueDequantization].dequantize(fPixelValue);
    strPixelValue = QString::number(static_cast<double>(fPixelValueDequant));

    QString strCoordinatesLatLon = conv_scenePosF_noZLI_to_coordinatesForDisplay(sceneMousePosF_noZLI);
    if (_eCoordinateDisplayMode == eCDM_WGS84) { 
        emit signal_strLatLon_strPixelValue(strCoordinatesLatLon, strPixelValue);
    } else { //eCDM_pixelImageXY
        emit signal_strPixelImageXY_strPixelValue(strCoordinatesLatLon, strPixelValue);
    }
}

void GeoUiInfos::slot_sceneMousePosF_noPixelValue(QPointF sceneMousePosF_noZLI) {
    QString strPixelValue = "---";
    QString strCoordinates = conv_scenePosF_noZLI_to_coordinatesForDisplay(sceneMousePosF_noZLI);
    if (_eCoordinateDisplayMode == eCDM_WGS84) {
        //emit signal_strLonLat_strPixelValue(strCoordinates, strPixelValue);
        emit signal_strLatLon_strPixelValue(strCoordinates, strPixelValue);
    } else { //eCDM_pixelImageXY
        emit signal_strPixelImageXY_strPixelValue(strCoordinates, strPixelValue);
    }
}

QString GeoUiInfos::conv_scenePosF_noZLI_to_coordinatesForDisplay(QPointF sceneMousePosF_noZLI) {
    QString strCoordinates;
    if (_eCoordinateDisplayMode == eCDM_WGS84) {
        if (!_ptrGeoRefModel_imagesSet) {
            strCoordinates = "error #GeoRefModelImgSet";  //@#LP dedicated string
        } else {
            //_ptrGeoRefModel_imagesSet->projXY_fromQcs_toStrLonLat(sceneMousePosF_noZLI, strCoordinates);
            _ptrGeoRefModel_imagesSet->projXY_fromQcs_toStrLatLon(sceneMousePosF_noZLI, strCoordinates);
        }
    } else { //eCDM_pixelImageXY
        strCoordinates =
            doubleToQStringPrecision_f_amountOfDecimal(sceneMousePosF_noZLI.x(), 2)
            + ", "
            + doubleToQStringPrecision_f_amountOfDecimal(sceneMousePosF_noZLI.y(), 2);
    }
    return(strCoordinates);
}

void GeoUiInfos::slot_sceneMousePosF_outOfImage() {
    QString strPixelValue = "---";
    if (_eCoordinateDisplayMode == eCDM_WGS84) {
        QString strLonLat;
        if (!_ptrGeoRefModel_imagesSet) {
            strLonLat = "error #GeoRefModelImgSet"; //@#LP dedicated string
        } else {
            strLonLat = _ptrGeoRefModel_imagesSet->qstrFor_outOfImage();
        }
        //emit signal_strLonLat_strPixelValue(strLonLat, strPixelValue);
        emit signal_strLatLon_strPixelValue(strLonLat, strPixelValue);
    } else {
        emit signal_strPixelImageXY_strPixelValue("___, ___", strPixelValue);
        //emit signal_strPixelImageXY_strPixelValue("99999.99, 99999.99", strPixelValue);
    }
}
