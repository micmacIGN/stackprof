#ifndef GEOUIINFOS_HPP
#define GEOUIINFOS_HPP

#include <QObject>
#include <QPointF>

#include <vector>
using namespace std;

class GeoRefModel_imagesSet;
class ValueDequantization;

class GeoUiInfos: public QObject {

    Q_OBJECT

public:

    enum e_CoordinateDisplayMode {
        eCDM_WGS84,
        eCDM_pixelImageXY
    };

    GeoUiInfos();
    void setPtrGeoRefModel_imagesSet(GeoRefModel_imagesSet* ptrGeoRefModel_imagesSet);

    void setPtrVectDequantization_PX1_PX2_deltaZ(vector<ValueDequantization> *ptrVectValueDequantization_PX1_PX2_deltaZ, bool bClear_ieLayerAccesForValueDequantization);

    void setIndexLayerForValueDequantization(size_t ieLayerAcces);

    void clear();

    void setCoordinateDisplayMode(e_CoordinateDisplayMode eCoordinateDisplayMode);

public slots:

    void slot_sceneMousePosF_with_pixelValue(QPointF sceneMousePosF_noZLI, float fPixelValue);
    void slot_sceneMousePosF_noPixelValue(QPointF sceneMousePosF_noZLI);
    void slot_sceneMousePosF_outOfImage();

signals:
    //void signal_strLonLat_strPixelValue(QString strLonLat, QString strPixelValue);
    void signal_strLatLon_strPixelValue(QString strLatLon, QString strPixelValue);
    void signal_strPixelImageXY_strPixelValue(QString strPixelImageXY, QString strPixelValue);

private:
    QString conv_scenePosF_noZLI_to_coordinatesForDisplay(QPointF sceneMousePosF_noZLI);

private:
    e_CoordinateDisplayMode _eCoordinateDisplayMode;

    GeoRefModel_imagesSet *_ptrGeoRefModel_imagesSet;

    vector<ValueDequantization> *_ptrVectValueDequantization_PX1_PX2_deltaZ;
    size_t _ieLayerAccesForValueDequantization;
};

#endif // GEOUIINFOS_HPP
