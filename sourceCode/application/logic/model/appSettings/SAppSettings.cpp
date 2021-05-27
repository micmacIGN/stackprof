#include <QDebug>

#include <QString>

#include "../../toolbox/toolbox_pathAndFile.h"

#include "SAppSettings.h"

S_AppSettings_CacheStorage::S_AppSettings_CacheStorage() {

    _bPathValid = false;
}

void S_AppSettings_CacheStorage::set(const QString& qstrValidPath) {
    _qstrPath = qstrValidPath;
    _bPathValid = true;
}

bool S_AppSettings_CacheStorage::get(QString& qstrValidPath) {
    if (_bPathValid) {
        qstrValidPath = _qstrPath;
    }
    return(_bPathValid);
}

bool S_AppSettings_CacheStorage::isValid() {
    return(_bPathValid);
}

void S_AppSettings_CacheStorage::clear() {
    _qstrPath.clear();

    _bPathValid = false;
}

void S_AppSettings_CacheStorage::debugShow() {
    qDebug() << __FUNCTION__ << "(S_AppSettings_CacheStorage)" << "_qstrPath = " << _qstrPath;
    //qDebug() << __FUNCTION__ << "(S_AppSettings_CacheStorage)" << "_bCacheStorageRootPath_loadedFromExistingQSettings = " << _bCacheStorageRootPath_loadedFromExistingQSettings;
    qDebug() << __FUNCTION__ << "(S_AppSettings_CacheStorage)" << "_bPathValid = " << _bPathValid;
}












bool S_AppSettings_CacheStorage::checkValid(const QString& qstrCacheStorageRootPath, QString& strErrorReason) {
    bool bChecked = checkDirectoryExistAndReadableWritableWithStrErrorReason(qstrCacheStorageRootPath, strErrorReason);
    return(bChecked);
}


S_AppSettings_ImagePyramid::S_AppSettings_ImagePyramid() {
    _OIIOCacheSizeMB = _cst_defaultImagePyramid_OIIOCacheSizeMB;
}

bool S_AppSettings_ImagePyramid::setWithRangeLimit(int OIIOCacheSizeMB) {
    _OIIOCacheSizeMB = OIIOCacheSizeMB;
    //@LP none check about available used computed RAM
    if (_OIIOCacheSizeMB < _cst_minImagePyramid_OIIOCacheSizeMB) {
        _OIIOCacheSizeMB = _cst_minImagePyramid_OIIOCacheSizeMB;
        return(true);
    }
    if (_OIIOCacheSizeMB > _cst_maxImagePyramid_OIIOCacheSizeMB) {
        _OIIOCacheSizeMB = _cst_maxImagePyramid_OIIOCacheSizeMB;
        return(true);
    }
    return(false);
}

bool S_AppSettings_ImagePyramid::isValid() {
    qDebug() << __FUNCTION__ << "_OIIOCacheSizeMB = " << _OIIOCacheSizeMB;

    if (_OIIOCacheSizeMB < _cst_minImagePyramid_OIIOCacheSizeMB) {
        return(false);
    }
    if (_OIIOCacheSizeMB > _cst_maxImagePyramid_OIIOCacheSizeMB) {
        return(false);
    }
    return(!(_OIIOCacheSizeMB%64));
}

void S_AppSettings_ImagePyramid::debugShow() {
    qDebug() << __FUNCTION__ << "(S_AppSettings_ImagePyramid)" << "_OIIOCacheSizeMB = " << _OIIOCacheSizeMB;
}

void S_AppSettings::clear() {
    _sas_cacheStorage.clear();
    _sas_imagePyramid = {};
}

void S_AppSettings::debugShow() {
    qDebug() << __FUNCTION__ << "(S_AppSettings)";
    _sas_cacheStorage.debugShow();
    _sas_imagePyramid.debugShow();
}

