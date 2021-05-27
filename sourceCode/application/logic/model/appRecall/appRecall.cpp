#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDir>

#include <QDebug>

#include "../../toolbox/toolbox_pathAndFile.h"
#include "OsPlatform/standardPathLocation.hpp"

#include "SAppRecall.h"

#include "appRecall.h"

AppRecall::AppRecall(QObject *parent): QObject(parent) {
   clearLoaded();
}

void AppRecall::clearLoaded() {
    _sAppRecall.clear();
}

bool AppRecall::containsSomeInvalidValues() {
    bool bContainsSomeInvalidValues = false;

    bContainsSomeInvalidValues |= (!_sAppRecall._sas_lastUsedDirectory.isValid());
    qDebug() << __FUNCTION__ << "_sAppRecall._sas_lastUsedDirectory.isValid() = " << _sAppRecall._sas_lastUsedDirectory.isValid();
    qDebug() << __FUNCTION__ << " => _bContainsSomeInvalidValues = " << bContainsSomeInvalidValues;

    return(bContainsSomeInvalidValues);
}

const S_AppRecall& AppRecall::getAllsAppRecall() const {
    return(_sAppRecall);
}

void AppRecall::setFrom_sAppRecall(const S_AppRecall& sAppRecall) {
    _sAppRecall = sAppRecall;
    _sAppRecall.debugShow();
}





QString AppRecall::get_lastUsedDirectory() const {
    QString strLastUsedDirectory;
    /*bool bValidPath =*/ _sAppRecall._sas_lastUsedDirectory.get(strLastUsedDirectory);
    return(strLastUsedDirectory);
}

void AppRecall::slot_newLastUsedDirectory(const QString& qstrLastUsedDirectory) {
    set_lastUsedDirectory(qstrLastUsedDirectory);
}

//does not set if dir is empty or does not exist
bool AppRecall::set_lastUsedDirectory(const QString& qstrLastUsedDirectory) {
    bool bDirectoryNameNotEmpty = false;
    bool bExists = false;
    bool bISRW = false;
    checkDirectoryExistAndReadableWritable(qstrLastUsedDirectory, bDirectoryNameNotEmpty, bExists, bISRW);
    if ((!bDirectoryNameNotEmpty) ||(!bExists)) { //does not care Read/Write permission here, if read/write failed later, it will have a specific error message about the file
        return(false);
    }
    _sAppRecall._sas_lastUsedDirectory.set(qstrLastUsedDirectory);
    return(true);
    //return(save());
}

bool AppRecall::set_lastUsedDirectoryFromPathFile(const QString& qstrPathAndFileName) {
    QString qStrPathAndFilenameFollowedSimlink = getPathAndFilenameFromPotentialSymlink(qstrPathAndFileName);
    QString strPath, strFilename;
    splitStrPathAndFile(qStrPathAndFilenameFollowedSimlink, strPath, strFilename);
    return(set_lastUsedDirectory(strPath));
}







bool AppRecall::load_setMissingToDefault(/*bool &bCacheStorageRootPath_loaded, bool &bCacheStorageRootPath_valid*/) {


    _sAppRecall.clear();

    QSettings settings(_strCompanyName, _strRecallAppName);

    //strLastUsedDirectory
    QString qstrDefaultInvalid_qstrLastUsedDirectory = "_n_o_t_Loaded_";//@LP a string which should not never used as a cache storage path root
    QVariant qvariantDefaultInvalid_qstrLastUsedDirectory = qstrDefaultInvalid_qstrLastUsedDirectory;
    QString qstrLastUsedDirectory = settings.value(_strKey_lastUsedDirectory, qvariantDefaultInvalid_qstrLastUsedDirectory).toString();

    qDebug() << __FUNCTION__ <<  "qstrLastUsedDirectory got from qsettings: " << qstrLastUsedDirectory;

    //value loaded from qsetting (but could not exist (deleted or removed by the user))
    if (qstrLastUsedDirectory.compare(qstrDefaultInvalid_qstrLastUsedDirectory)) {
        qstrLastUsedDirectory = replaceSlashByDirSeparator(qstrLastUsedDirectory); //restore back slack dir separator on windows OS
        qDebug() << __FUNCTION__ <<  "qstrLastUsedDirectory <= " << qstrLastUsedDirectory;
    } else {
        //set default if no loaded directory
        qstrLastUsedDirectory = StandardPathLocation::strStartingDefaultDir();
        qDebug() << __FUNCTION__ <<  "qstrLastUsedDirectory <= (strStartingDefaultDir)" << qstrLastUsedDirectory;
    }

    bool bDirectoryNameNotEmpty = false;
    bool bExists = false;
    bool bISRW = false;
    checkDirectoryExistAndReadableWritable(qstrLastUsedDirectory, bDirectoryNameNotEmpty, bExists, bISRW);

    qDebug() << __FUNCTION__ <<  "bDirectoryNameNotEmpty = " << bDirectoryNameNotEmpty;
    qDebug() << __FUNCTION__ <<  "bExists = " << bExists;
    qDebug() << __FUNCTION__ <<  "bISRW = " << bISRW;

    //set default if the loaded directory is invalid
    if ((!bDirectoryNameNotEmpty) ||(!bExists)) { //does not care Read/Write permission here, if read /write failed later, it will have a specific error message about the file
        qstrLastUsedDirectory = StandardPathLocation::strStartingDefaultDir();
    }

    _sAppRecall._sas_lastUsedDirectory.set(qstrLastUsedDirectory);
    _sAppRecall._sas_lastUsedDirectory.debugShow();
    /*qDebug() << __FUNCTION__ << "_imagePyramid_OIIOCacheSizeMB = " << _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB;
    qDebug() << __FUNCTION__ << "_qstrCacheStorageRootPath = " << _sAppSettings._sas_cacheStorage._qstrPath;
    qDebug() << __FUNCTION__ << "_bCacheStoragePathValid   = " << _sAppSettings._sas_cacheStorage._bPathValid;*/

    return(true);
}


bool AppRecall::save() {

    QSettings settings(_strCompanyName, _strRecallAppName);

    qDebug() << __FUNCTION__ << "_sAppRecall._sas_lastUsedDirectory._qstrPath = " << _sAppRecall._sas_lastUsedDirectory._qstrPath;

    //lastUsedDirectory
    if (_sAppRecall._sas_lastUsedDirectory.isValid()) {
        QString qstrPathWithSlash = replaceDirSeparatorBySlash(_sAppRecall._sas_lastUsedDirectory._qstrPath); //store slash for dir separator on any platforms (ie windows included)
        settings.setValue(_strKey_lastUsedDirectory, qstrPathWithSlash);
    } else {
        qDebug() << __FUNCTION__ << "_sAppRecall._sas_lastUsedDirectory.isValid: " << _sAppRecall._sas_lastUsedDirectory.isValid();
        return(false);
    }

    settings.sync();
    return(true);
}

