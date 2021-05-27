#include <QString>
#include <QDir>
#include <QVariantMap>

#include <QDebug>

#include "../toolbox/toolbox_pathAndFile.h"
#include "iojsoninfos.h"

#include "ZoomOutLevelImageBuilder.h"

#include "ZLIStorageContentManager.h"

qlonglong ZLIStorageContentManager::get_nextAvailableUniqueIDForDirName() {
    return(_qll_currentUsedMaximalUniqueID+1);
}

bool ZLIStorageContentManager::add_pair_strPathAndFilename_uniqueIdForZLIRelativePath(const QString&strPathAndFilename, qlonglong uniqueIdForZLIRelativePath) {
    _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.insert(strPathAndFilename, uniqueIdForZLIRelativePath);
    return (true);
}

void ZLIStorageContentManager::increase_currentUsedMaximalUniqueID() {
    _qll_currentUsedMaximalUniqueID++;
}

ZLIStorageContentManager::ZLIStorageContentManager(): _qll_currentUsedMaximalUniqueID(-1) {

}

bool ZLIStorageContentManager::setRootPath(const QString& qstrRootPath) {
  if (!checkDirectoryExistAndReadableWritable(qstrRootPath)) {
      return(false);
  }
  _strRootPath = qstrRootPath;
  return(true);
}

//return false in case of fatal error
bool ZLIStorageContentManager::load_ZLIStorageContentFile(bool& bFileDoesNotExist) {

    qDebug() << __FUNCTION__ << "_strRootPath is: " << _strRootPath;
    qDebug() << __FUNCTION__ << "_strZLIStorageContentFilename is: " << _strZLIStorageContentFilename;

    bFileDoesNotExist = true;

    _qll_currentUsedMaximalUniqueID = -1;

    if (!checkDirectoryExistAndReadableWritable(_strRootPath)) {
        qDebug() << __FUNCTION__ << "if (!checkDirectoryExistAndReadableWritable(_strRootPath)) { with _strRootPath = " << _strRootPath;
        return(false);
    }

    QString qtrZLIStorageContentPathAndFilename = concatStrPathAndFile(_strRootPath, _strZLIStorageContentFilename);
    qDebug() << __FUNCTION__ << "qtrZLIStorageContentPathAndFilename <- " << qtrZLIStorageContentPathAndFilename;

    if (!fileExists(qtrZLIStorageContentPathAndFilename)) {
        qDebug() << __FUNCTION__ << "if (!fileExists(qtrZLIStorageContentPathAndFilename)) { ";
        bFileDoesNotExist = true;
        qDebug() << __FUNCTION__ << "bFileDoesNotExist <- true, and return(true)";
        return(true);
    }

    bFileDoesNotExist = false;

    IOJsonInfos IOJsoFileContent;
    bool bReport = IOJsoFileContent.loadFromFile(qtrZLIStorageContentPathAndFilename);
    if (!bReport) {        
        qDebug() << __FUNCTION__ << "IOJsoFileContent.loadFromFile(qtrZLIStorageContentPathAndFilename) failed";
        return(false);
    }

    _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath = IOJsoFileContent.toVariantMap();

    //now find the _currentUsedMaximalUniqueID
    auto iter = _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.constBegin();
    while (iter != _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.constEnd()) {
        qDebug() << __FUNCTION__ << iter.value() << iter.key();

        bool bOK = false;
        qlonglong qllID = iter.value().toLongLong(&bOK);
        if (!bOK) {            
          qDebug() << __FUNCTION__ << "iter.value().toLongLong(&bOK) failed";
          return(false);
        }
        if (qllID > _qll_currentUsedMaximalUniqueID) {
            _qll_currentUsedMaximalUniqueID = qllID;
            qDebug() << __FUNCTION__
                     << "if (qllID > _qll_currentUsedMaximalUniqueID) { => _qll_currentUsedMaximalUniqueID <- "
                     << _qll_currentUsedMaximalUniqueID;
        }
        ++iter;
    }

    return(true);
}


//return false in case of fatal error
bool ZLIStorageContentManager::checkExistsInDBandGetId(const QString& qstrPathAndFilename, qlonglong &qll_uniqueID, bool& bExist) {

    QVariant qvariantDefaultForNotFound("///notFound///");
    QVariant qvariantFound = _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.value(qstrPathAndFilename, qvariantDefaultForNotFound);

    if (qvariantFound == qvariantDefaultForNotFound) {
        qDebug() << __FUNCTION__ << "if (qvariantFound == qvariantDefaultForNotFound) {";
        qDebug() << __FUNCTION__ << "bExist <- false, return(true)";

        bExist = false;
        return(true);
    }

    qDebug() << __FUNCTION__ << "qvariantFound != qvariantDefaultForNotFound";

    bool bOK = false;
    qll_uniqueID = qvariantFound.toLongLong(&bOK);
    if (!bOK) {
        qDebug() << __FUNCTION__ << "qvariantFound.toLongLong(&bOK) failed ";
        qDebug() << __FUNCTION__ << "bExist <- false, return(false)";

        bExist = false;
        return(false);
    }

    qDebug() << __FUNCTION__ << "bExist <- true, return(true)";

    bExist = true;
    return(true);
}


bool ZLIStorageContentManager::ZLIStorageContentManager::save_ZLIStorageContentFile() {
    if (!checkDirectoryExistAndReadableWritable(_strRootPath)) {
        return(false);
    }
    QString qtrZLIStorageContentPathAndFilename = concatStrPathAndFile(_strRootPath, _strZLIStorageContentFilename);

    IOJsonInfos IOJsoFileContent;
    IOJsoFileContent.fromVariantMap(_qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath);
    bool bReport = IOJsoFileContent.saveToFile(qtrZLIStorageContentPathAndFilename);
    return(bReport);
}

bool ZLIStorageContentManager::findZLIStorageAbsoluteDirectoryForPathAndFilename(
    const QString& qstrPathAndFilename, QString& qstrAbsolutePathOfZLIforFile) {
    QVariant qvariantDefaultForNotFound("///notFound///");
    QVariant qvariantFound = _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.
            value(qstrPathAndFilename, qvariantDefaultForNotFound);
    if (qvariantFound == qvariantDefaultForNotFound) {
        qDebug() << __FUNCTION__ << "if (qvariantFound == qvariantDefaultForNotFound) {";
        qDebug() << __FUNCTION__ << "return(false)";
        return(false);
    }

    qstrAbsolutePathOfZLIforFile = concatStrPathAndFile(_strRootPath, qvariantFound.toString());

    qDebug() << __FUNCTION__ << "qvariantFound != qvariantDefaultForNotFound";
    qDebug() << __FUNCTION__ << "qstrAbsolutePathOfZLIforFile is set to: " << qstrAbsolutePathOfZLIforFile;
    qDebug() << __FUNCTION__ << "return(true)";

    return(true);
}

bool ZLIStorageContentManager::removeRelativePathOfZLIforFile_forAccordingDisappearedDirectories(bool& bSomeRemoved) {

    bSomeRemoved = false;

    QVariantMap qVariantMap_cleaned_strPathAndFilename_accordingVariantStrRelativePathOfZLIforFile;

    auto iter = _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.constBegin();
    while (iter != _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath.constEnd()) {

        qDebug() << __FUNCTION__ << iter.value() << iter.key();

        ZoomOutLevelImageBuilder zLIImageBuilder(iter.key(), _strRootPath);

        QString strAbsolutePathofZLIStorageForFile;
        bool bReport = zLIImageBuilder.absoluteDirNameForRootImageAndID(iter.key(), iter.value().toLongLong(), strAbsolutePathofZLIStorageForFile);
        if (!bReport) {            
            qDebug() << __FUNCTION__ << " zLIImageBuilder.absoluteDirNameForRootImageAndID said false";
            qDebug() << __FUNCTION__ << " this happen when the root image does not exist (removed or moved by the user)";

            //let in place in the json list, and do not check sub dir.
            //cleaning the json list is about disappeared sub-dir, not about disapparead root images
            //the user will have to remove the sub dir himself

            qDebug() << __FUNCTION__ << " keep in json list";
            qVariantMap_cleaned_strPathAndFilename_accordingVariantStrRelativePathOfZLIforFile.insert(iter.key(), iter.value());
            qDebug() << __FUNCTION__ << "keep :" << iter.key().toStdString().c_str() << " ( id: " << iter.value().toLongLong() << " )";

            ++iter;
            continue; //return(false);
        }

        qDebug() << __FUNCTION__ << " strAbsolutePathofZLIStorageForFile is:" <<  strAbsolutePathofZLIStorageForFile;

        bool bDirectoryNameNotEmpty = false;
        bool bExists = false;
        bool bISRW = false;
        checkDirectoryExistAndReadableWritable(strAbsolutePathofZLIStorageForFile, bDirectoryNameNotEmpty, bExists, bISRW);
        qDebug() << __FUNCTION__ << "checkDirectoryExistAndReadableWritable give:";
        qDebug() << __FUNCTION__ << "  bDirectoryNameNotEmpty: " << bDirectoryNameNotEmpty;
        qDebug() << __FUNCTION__ << "  bExists: " << bExists;
        qDebug() << __FUNCTION__ << "  bISRW: " << bISRW;

        if (bExists) {
            qDebug() << __FUNCTION__ << " if (bExists) {";
            qVariantMap_cleaned_strPathAndFilename_accordingVariantStrRelativePathOfZLIforFile.insert(iter.key(), iter.value());
            qDebug() << __FUNCTION__ << "keep :" << iter.key().toStdString().c_str() << " ( id: " << iter.value().toLongLong() << " )";
        } else {
            bSomeRemoved = true;
            qDebug() << __FUNCTION__ << " bSomeRemoved <- true;";
        }

        ++iter;
    }

    _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath =
        qVariantMap_cleaned_strPathAndFilename_accordingVariantStrRelativePathOfZLIforFile;

    return(true);
}


