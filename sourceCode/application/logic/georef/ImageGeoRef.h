#ifndef IMAGEGEOREF_H
#define IMAGEGEOREF_H

#include <QString>

#include "WorldFile_parameters.h"

#include "geo_basic_struct.h"

enum e_inputFileStatus_geoTFWData {
    e_iFS_geoTFWData_notSetValue,

    e_iFS_geoTFWData_noneFound,

    e_iFS_geoTFWData_oneFound,
    e_iFS_geoTFWData_moreThanOneFound,

    e_iFS_geoTFWData_errorWhileLoading,

    e_iFS_geoTFWData_loaded,

    e_iFS_geoTFWData_loaded_butNotUsable,

};


#include <QJsonObject>
#include <QSizeF>

class ImageGeoRef {

    public:
        ImageGeoRef();
        ImageGeoRef(QString qstrImagePathAndFilename);

        void clear();

        void showContent() const;

        bool getImageSizeF(QSizeF& sizef) const;

        bool checkSame_geoTFWDataconst(const ImageGeoRef& imageGeoRefToCompare) const;

        bool setAndLoad_imageWidthHeight_TFWDataIfAvailable(bool& bLoaded_butNotUsable);

        // used when the user has specified a world file not automatically load. This new world file is loaded imageGeoRefWithTFWDataToTake
        //void setTFWDataFromAnotherImageGeoRef(const ImageGeoRef& imageGeoRefWithTFWDataToTake);

        bool geoImageXYComputable() const;

        QString getImagePathAndFilename() const;

        //void setCodeEPSG(QString qstrCodeEPSG);

        bool convertXY_fromQcs_toWF(S_xy qcsXY, S_xy& geoXY) const;
        bool convertXY_fromWF_toQcs(S_xy geoXY, S_xy& qcsXY) const;

        e_inputFileStatus_geoTFWData get_inputFileStatus_geoTFWData() const;

        bool toQJsonObject(QJsonObject& qjsonObj) const ;
        bool fromQJsonObject(const QJsonObject& qjsonObj);

        QVector<QString> getqvectstr_worldFileParameters();

        //bool loadSpecificWorldFile(const QString& qstrWorldFile);

        bool setImagePathFilename(const QString& strImagePathAndFilename);

        bool get_xScale(double& xScale) const;
        bool get_yScale(double& yScale) const;

        bool convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
            qreal pixel_distanceFromStartingRoutePointToRoutePoint,
            qreal& WF_distanceFromStartingRoutePointToRoutePoint) const;

        void setDefaultValuesAsNoGeorefUsed();

private:
        bool loadImageWidthHeightFromImageFile();

        // return false if none tfw file found
        // return true  if one or more tfw file found.
        // bLoaded indicates if only one tfw file was found and loaded successfully
        // TFWparametersCanBeUsed indicates if the loaded tfw file can be used with the current implementation
        //bool findAndLoadTFWFileForImage(bool& bLoaded, bool& TFWparametersCanBeUsed);

        bool findWorldFilesForImage(bool& bOnlyOneFound);
        bool loadWorldFileFoundForImage();

        bool computeImageCornersInGeoXY();

    private:
        QString _qstrImagePathAndFilename;

        double _imageWidth;
        double _imageHeight;
        bool _bImageSizeGot;

        e_inputFileStatus_geoTFWData _inputFileStatus_geoTFWData;
        WorldFile_parameters _worldFileParameters;
        QVector<QString> _qvectqstrWFCandidate;
        QString _qstrWordlFile_pathAndFilenameUsed;

        S_xy _upperLeftImageCorner;
        bool _bUpperLeftImageCornerComputed;
};

#endif // IMAGEGEOREF_H
