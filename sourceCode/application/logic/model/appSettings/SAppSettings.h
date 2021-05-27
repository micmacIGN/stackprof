#ifndef SAPPSETTINGS_H
#define SAPPSETTINGS_H

#include <QString>
#include <QMetaType>

struct S_AppSettings_CacheStorage {
    QString _qstrPath;

    bool _bPathValid;
    //bool _bCacheStorageRootPath_loadedFromExistingQSettings;

    S_AppSettings_CacheStorage();

    bool checkValid(const QString& qstrCacheStorageRootPath, QString& strErrorReason);
//  bool checkValid_and_set(const QString& qstrCacheStorageRootPath, QString& strErrorReason);

    bool isValid();

    void clear();
    void set(const QString& qstrValidPath);
    bool get(QString& qtrValidPath);

    void debugShow();
};
Q_DECLARE_METATYPE(S_AppSettings_CacheStorage);

struct S_AppSettings_ImagePyramid {

    int _OIIOCacheSizeMB;

    S_AppSettings_ImagePyramid();
    bool setWithRangeLimit(int OIIOCacheSizeMB);
    bool isValid();

    //default values
    //this is by image for the tile provider's OIIO cache
    static constexpr int _cst_defaultImagePyramid_OIIOCacheSizeMB = 512;
    static constexpr int _cst_minImagePyramid_OIIOCacheSizeMB = 64;
    static constexpr int _cst_maxImagePyramid_OIIOCacheSizeMB = 1024*64; //64GB as maximum ? @#LP: too big or future proof compatible ?

    void debugShow();

};
Q_DECLARE_METATYPE(S_AppSettings_ImagePyramid);

struct S_AppSettings {
    S_AppSettings_CacheStorage _sas_cacheStorage;
    S_AppSettings_ImagePyramid _sas_imagePyramid;
    void clear();
    void debugShow();
};
Q_DECLARE_METATYPE(S_AppSettings);

#endif // SAPPSETTINGS_H
