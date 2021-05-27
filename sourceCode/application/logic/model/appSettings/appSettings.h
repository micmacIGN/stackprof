#ifndef AppSettings_h
#define AppSettings_h

#include <QString>

#include "SAppSettings.h"

class AppSettings {

public:

    AppSettings();

    void clearLoaded();
    bool containsSomeInvalidValues();

    bool load_setMissingToDefault(/*bool &bCacheStorageRootPath_loaded, bool &bCacheStorageRootPath_valid*/);

    bool save();

    void setFrom_sAppSettings(const S_AppSettings& sAppSettings);

    QString get_cacheStorageRootPath();
    QString get_cacheStorageRootPath(bool& bValid);
    //bool get_cacheStorageRootPath(QString& qstrCacheStoragePath);
    bool set_cacheStorageRootPath(const QString& qstrCacheStoragePath);

    int get_imagePyramid_OIIOCacheSizeMB();
    //bool get_imagePyramid_OIIOCacheSizeMB(int& OIIOCacheSizeMB);
    bool set_imagePyramid_OIIOCacheSizeMB(int OIIOCacheSizeMB);

    const S_AppSettings& getAllsAppSettings() const;

    //bool checkValid_cacheStorageRootPath(const QString& qstrCacheStoragePath);

    //void sendSAppSettings();

private:
    S_AppSettings _sAppSettings;

private:

    //_bCacheStorageRootPath_loadedbool _bCacheStorageRootPath_loaded;

    //for storage:
    //
    static constexpr const char* _strCompanyName = "ipgp_fr";
    static constexpr const char* _strAppName     = "stackp_settings";
    static constexpr const char* _strSectionName = "AppCaches";

    //
    static constexpr const char* _strKey_imagePyramid_OIIOCacheSizeMB = "imgPyramid_cacheSizeMB";
    static constexpr const char* _strKey_cacheStorage_rootPath = "cacheStorage_rootPath";

};

#endif //AppSettings_h

