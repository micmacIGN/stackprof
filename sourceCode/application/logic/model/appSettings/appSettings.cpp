#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDir>

#include <QDebug>

#include "../../toolbox/toolbox_pathAndFile.h"

#include "appSettings.h"

AppSettings::AppSettings() {
   clearLoaded();
}

void AppSettings::clearLoaded() {
    _sAppSettings.clear();

}

bool AppSettings::containsSomeInvalidValues() {

    bool bContainsSomeInvalidValues = false;

    bContainsSomeInvalidValues |= (!_sAppSettings._sas_cacheStorage._bPathValid);
    qDebug() << __FUNCTION__ << "_sAppSettings._sas_cacheStorage._bPathValid = " << _sAppSettings._sas_cacheStorage._bPathValid;
    qDebug() << __FUNCTION__ << " => _bContainsSomeInvalidValues = " << bContainsSomeInvalidValues;

    //@LP should never happens, requiered to go in the correct tabwidget in the ui side
    bContainsSomeInvalidValues |= (!_sAppSettings._sas_imagePyramid.isValid());
    qDebug() << __FUNCTION__ << "_sAppSettings._sas_imagePyramid.isValid() = " << _sAppSettings._sas_imagePyramid.isValid();
    qDebug() << __FUNCTION__ << " => bContainsSomeInvalidValues = " << bContainsSomeInvalidValues;

    return(bContainsSomeInvalidValues);
}

const S_AppSettings& AppSettings::getAllsAppSettings() const {
    return(_sAppSettings);
}

void AppSettings::setFrom_sAppSettings(const S_AppSettings& sAppSettings) {
    _sAppSettings = sAppSettings;
    _sAppSettings.debugShow();
}

QString AppSettings::get_cacheStorageRootPath() {
    QString qstrCacheStorageRootPath;
    _sAppSettings._sas_cacheStorage.get(qstrCacheStorageRootPath);
    return(qstrCacheStorageRootPath);
}

QString AppSettings::get_cacheStorageRootPath(bool& bValid) {
    QString qstrCacheStorageRootPath;
    bValid = _sAppSettings._sas_cacheStorage.get(qstrCacheStorageRootPath);
    return(qstrCacheStorageRootPath);
}

bool AppSettings::set_cacheStorageRootPath(const QString& qstrCacheStoragePath) {
    _sAppSettings._sas_cacheStorage.clear();
    bool bCheckedAndSet = checkDirectoryExistAndReadableWritable(qstrCacheStoragePath);
    if (!bCheckedAndSet) {
        return(false);
    }
    return(save());
}

int AppSettings::get_imagePyramid_OIIOCacheSizeMB() {
    return(_sAppSettings._sas_imagePyramid._OIIOCacheSizeMB);
}

bool AppSettings::set_imagePyramid_OIIOCacheSizeMB(int OIIOCacheSizeMB) {
    return(_sAppSettings._sas_imagePyramid.setWithRangeLimit(OIIOCacheSizeMB));
}


bool AppSettings::load_setMissingToDefault() {

    _sAppSettings.clear();

    QSettings settings(_strCompanyName, _strAppName);

    settings.beginGroup(_strSectionName);

    //ImagePyramid_OIIOCacheSizeMB
    qDebug() << __FUNCTION__ << "_sAppSettings._sas_imagePyramid._cst_defaultImagePyramid_OIIOCacheSizeMB = " << _sAppSettings._sas_imagePyramid._cst_defaultImagePyramid_OIIOCacheSizeMB;

    QVariant qvariantDefault_imagePyramid_OIIOCacheSizeMB = _sAppSettings._sas_imagePyramid._cst_defaultImagePyramid_OIIOCacheSizeMB;
    int OIIOCacheSizeMB = settings.value(_strKey_imagePyramid_OIIOCacheSizeMB, qvariantDefault_imagePyramid_OIIOCacheSizeMB).toInt();

    qDebug() << __FUNCTION__ << "OIIOCacheSizeMB = " << OIIOCacheSizeMB;

    _sAppSettings._sas_imagePyramid.setWithRangeLimit(OIIOCacheSizeMB);

    //strCacheStoragePath
    QString qstrDefaultInvalid_qstrCacheStoragePath = "_n_o_t_Loaded_";//@LP a string which should not never used as a cache storage path root
    QVariant qvariantDefaultInvalid_qstrCacheStoragePath = qstrDefaultInvalid_qstrCacheStoragePath;
    QString qstrCacheStorageRootPath = settings.value(
        _strKey_cacheStorage_rootPath, qvariantDefaultInvalid_qstrCacheStoragePath).toString();
    if (!qstrCacheStorageRootPath.compare(qstrDefaultInvalid_qstrCacheStoragePath)) {
        return(false);
    }

    qstrCacheStorageRootPath = replaceSlashByDirSeparator(qstrCacheStorageRootPath); //restore back slack dir separator on windows OS

    bool bChecked = checkDirectoryExistAndReadableWritable(qstrCacheStorageRootPath);
    if (bChecked) {
        _sAppSettings._sas_cacheStorage.set(qstrCacheStorageRootPath);
    }
    _sAppSettings.debugShow();
    /*qDebug() << __FUNCTION__ << "_imagePyramid_OIIOCacheSizeMB = " << _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB;
    qDebug() << __FUNCTION__ << "_qstrCacheStorageRootPath = " << _sAppSettings._sas_cacheStorage._qstrPath;
    qDebug() << __FUNCTION__ << "_bCacheStoragePathValid   = " << _sAppSettings._sas_cacheStorage._bPathValid;*/

    return(bChecked);
}


bool AppSettings::save() {

    QSettings settings(_strCompanyName, _strAppName);

    settings.beginGroup(_strSectionName);

    qDebug() << __FUNCTION__ << "_imagePyramid_OIIOCacheSizeMB = " << _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB;
    qDebug() << __FUNCTION__ << "_qstrCacheStoragePath = " << _sAppSettings._sas_cacheStorage._qstrPath;
    qDebug() << __FUNCTION__ << "(_bCacheStoragePathValid = " << _sAppSettings._sas_cacheStorage._bPathValid;

    //ImagePyramid_OIIOCacheSizeMB
    settings.setValue(_strKey_imagePyramid_OIIOCacheSizeMB, _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB); //qvariantRead_imagePyramid_OIIOCacheSizeMB);

    //strCacheStoragePath
    bool bCacheStorage_isValid = _sAppSettings._sas_cacheStorage.isValid();
    if (bCacheStorage_isValid) {
        QString qstrPathWithSlash = replaceDirSeparatorBySlash(_sAppSettings._sas_cacheStorage._qstrPath); //store slash for dir separator on any platforms (ie windows included)
        settings.setValue(_strKey_cacheStorage_rootPath, qstrPathWithSlash);
    } else {
        qDebug() << __FUNCTION__ << "_bCacheStoragePathValid is false" << _sAppSettings._sas_cacheStorage._bPathValid;
        qDebug() << __FUNCTION__ << "does not alter strKey_cacheStorage_rootPath (" << _strKey_cacheStorage_rootPath <<")";
    }
    settings.endGroup();

    settings.sync();
    return(bCacheStorage_isValid);
}

