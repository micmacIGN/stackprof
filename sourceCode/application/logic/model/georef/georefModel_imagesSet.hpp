#ifndef GEOREFMODEL_IMAGESSET_HPP
#define GEOREFMODEL_IMAGESSET_HPP

#include <QObject>
#include <QPointF>

#include "../../georef/ImageGeoRef.h"

#include "../../georef/Proj6_crs_to_crs.h"

enum e_inputFileStatus_geoEPSG {
    e_iFS_geoEPSG_notSetValue,

    e_iFS_geoEPSG_notFoundInCache,

    e_iFS_geoEPSG_foundInCache,

    e_iFS_geoEPSG_foundInCache_errorWhileLoading,

    e_iFS_geoEPSG_foundInCache_loaded
};

enum e_imagesSetStatus_geoTFWData {

   e_iSS_geoTFWData_notSetValue,

   e_iSS_geoTFWData_notAvailable_noneFound,
   e_iSS_geoTFWData_notAvailable_errorWhileLoading,
   e_iSS_geoTFWData_notAvailable_loadedButNotUsable,

   e_iSS_geoTFWData_notAvailable_unsyncTFWData,

   e_iSS_geoTFWData_available,

   e_iSS_geoTFWData_notAvailable_unknowError,
};

enum e_imagesSetStatus_geoEPSG {

   e_iSS_geoEPSG_notSetValue,

   e_iSS_geoEPSG_notAvailable_noneFound,

   e_iSS_geoEPSG_notAvailable_unsyncEPSGData,

   e_iSS_geoEPSG_available,

   e_iSS_geoEPSG_notAvailable_unknowError

};

//used to give infos to the user in case of unsync for example
struct S_DetailsAboutEPSGCodeFoundFromCacheForOneImage {
    QString _strImageFilename;
    e_imagesSetStatus_geoEPSG _eImagesSetStatus_geoEPSG;
    QString _qstrEPSGCode;
    S_DetailsAboutEPSGCodeFoundFromCacheForOneImage();
    S_DetailsAboutEPSGCodeFoundFromCacheForOneImage(
            const QString& strImageFilename,
            e_imagesSetStatus_geoEPSG eimagesSetStatus_geoEPSG,
            const QString& qstrEPSGCode);
    void clear();
    void show();
};

enum e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet {
    e_DetailsAboutEPSGCFC_fTIS_notSetValue,

    e_DetailsAboutEPSGCFC_fTIS_noneImage,

    e_DetailsAboutEPSGCFC_fTIS_noneFound_allReadError,

    e_DetailsAboutEPSGCFC_fTIS_noneFound_noneReadError,
    e_DetailsAboutEPSGCFC_fTIS_noneFound_someReadError,

    e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundButNoneReadError,
    e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundWithSomeReadError,

    e_DetailsAboutEPSGCFC_fTIS_allFound
};

class GeoRefModel_imagesSet: public QObject {

    Q_OBJECT

public:
    GeoRefModel_imagesSet(QObject *parent =nullptr);

    //from image computer coordinate system to world file geo XY
    bool convertXY_fromQcs_toWF(S_xy qcsXY, S_xy& geoXY) const;

    //from world file geo XY to image computer coordinate system
    bool convertXY_fromWF_toQcs(S_xy geoXY, S_xy& qcsXY) const;

    bool convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
        qreal pixel_distanceFromStartingRoutePointToRoutePoint,
        qreal& WF_distanceFromStartingRoutePointToRoutePoint) const;

    int imageCount() const;
    void showContent() const;

    bool compareWorldFileDataWith(const GeoRefModel_imagesSet& GRMimgSet_compareToMe) const;

    bool setAndLoad_imageWidthHeight_TFWDataIfAvailable(const QVector<QString>& qvectqstrImagesPathAndFilename, bool& bSomeLoadedButNotUsable);
    e_imagesSetStatus_geoTFWData check_TFWDataSet();

    bool setDataFromAnotherGeoRefModelImagesSet(const GeoRefModel_imagesSet& geoRM_imgSet);

    //used typically to compare wordlfile data from a inputFiles set to the world file data got from project json file
    bool getUniqueImageGeoRef(ImageGeoRef& imgeGeoRef) const;

#ifdef DEF_searchinCache
    //init directory of cache storage of associated EPSG codes for images
    void setRootForEPSGCacheStorage(const QString& qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages);
#endif

    e_imagesSetStatus_geoEPSG searchInCache_associatedEPSGCode();

    QString get_qstrEPSGCode() const;

    e_imagesSetStatus_geoEPSG forceEPSGCode(const QString& qstrEPSGCode);

    //bool someImagesHaveNoneAssociatedEPSGCode(bool& bSomehave_notAvailable_noneFound); //there is already a better existing method

    //if not found in cache (or rewrite in cache (for example, after a user mistake entering the EPSG Code), store to the cache:
    e_imagesSetStatus_geoEPSG setEPSGCode_and_storeInCacheForAllImagesSet(const QString& qstrEPSGCode);

    //in public to be able to store eaysily an EPSG Code for a image whithout EPSG Code (image background replacement of a .jroute with a renamed image file)
    bool storeInCache_EPSGCode_forImage(const QString& strImageFilename);
    bool storeInCache_EPSGCode_forImagesWhichDoNotHave();

    bool createProjContext(const QString& strPathProjDB);
    bool createCrsToCrs_to_WGS84as4326();
    bool createCrsToCrs_to_specifiedTargetEPSGCode(const QString& qstr_targetEPSGCode);

    //from world file geo XY to targeted crs (lon, lat)
    bool projXY_fromWF_toLonLat(const S_xy& geoXY, S_lonlat& sLonlat) const;

    bool projXY_fromLonLat_toWF(const S_lonlat& lonlat, S_xy& geoXY) const;

    //from world file geo XY to targeted crs (lon, lat)
    bool projXY_fromWF_toWF(const S_xy& geoXYsrc, S_xy& geoXYdst) const;

    //from image computer coordinate system src to targeted crs (lon, lat)
    bool projXY_fromQcs_toLonLat(const QPointF& sceneMousePosF_noZLI, S_lonlat& sLonlat) const;


    //from image computer coordinate system src to targeted crs (lon, lat)
    //shoul be used only to get the lon lat string about the mouse location in the image
    //bool projXY_fromQcs_toStrLonLat(const QPointF& sceneMousePosF_noZLI, QString& strLonlat) const;
    bool projXY_fromQcs_toStrLatLon(const QPointF& sceneMousePosF_noZLI, QString& strLatLon) const;

    QString proj_qcsXYPos_outOfImage();
    QString qstrFor_outOfImage() const ;

    bool toQJsonObject(QJsonObject& qjsonObj) const;
    bool fromQJsonObject(const QJsonObject& qjsonSrcValue);
    void clear();

    void sendStrWorldFileDataToView();
    void sendImagesFilenameToView();
    void sendEPSGCodeToView();

    QVector<QString> get_qvectOfStrImagePathAndFilenameSet();

    e_imagesSetStatus_geoEPSG setToOneImage_and_getEPSGCodeFromCacheIfAvailable(const QString& strOneImagePathAndFilename, QString& strFoundEPSGCode);

    bool replace_soleImageFilename(const QString& strImagePathAndFilename);
    bool replace_imagesFilename(const QVector<QString>& qvectStrImageFilename);

    bool alignContent_vectImageGeoRefSize_andGeoInfosOnFirst(int size);

    void get_detailsAboutEPSGCodeFoundFromCacheForTheImageSet(
            QVector<S_DetailsAboutEPSGCodeFoundFromCacheForOneImage>& qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage,
            e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet& eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet) const;

    bool proj_qcsXYPos_toLonLat(const QPointF& sceneMousePosF_noZLI, S_lonlat& lonlat);

    bool getImageSizeF(QSizeF& sizef) const; //all images have the same w h, uses  _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection to get size

    bool get_xScale(double &xScale) const;
    bool get_yScale(double &yScale) const;

    bool setDefaultValuesAsNoGeorefUsed(const QString& strImageFilename);
    bool setDefaultValuesAsNoGeorefUsed_fromInputImageFilenames(const QVector<QString>& qvectqstrImagesPathAndFilename);
    bool get_bUseGeoRef() const;
    void set_bUseGeoRef(bool bUseGeoRef);

public slots:

signals:

    void signal_strWorldFileData(QVector<QString> qvecStr_worldFileData);
    void signal_strEPSGCode(QString strEPSGCode);
    void signal_imagesFilename(QVector<QString> qvecStr_imagesFilename_threeMax);

private:

    bool checkOneAvailable() const;

private:    
    e_imagesSetStatus_geoTFWData convert_iFSGeoTFWData_to_imagesSetStatusGeoTFWData(e_inputFileStatus_geoTFWData einputFileStatus_geoTFWData) const;

    //@LP: synced containers indexes
    QVector<ImageGeoRef> _qvectImageGeoRef;
    //QVector<e_inputFileStatus_geoEPSG> _qvect_eInputFileStatus_geoEPSG;

    e_imagesSetStatus_geoTFWData _imagesSetStatus_geoTFWData;

#ifdef DEF_searchinCache
    QString _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages;
#endif

    //for the set:
    QString _qstrEPSGCode;
    e_imagesSetStatus_geoEPSG _imagesSetStatus_geoEPSG;
    bool _bUseGeoRef;

    //details per image about found in cache or not:
    //used to give infos to the user in case of unsync for example
    QVector<S_DetailsAboutEPSGCodeFoundFromCacheForOneImage> _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage;
    e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;

    int _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection;

    Proj6_crs_to_crs _proj6crsTocrs;
};



#endif // GEOREFMODEL_IMAGESSET_HPP
