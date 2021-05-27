#include <QDebug>

#include <QString>

#include "../../toolbox/toolbox_pathAndFile.h"

#include "SAppRecall.h"

S_AppRecall_lastUsedDirectory::S_AppRecall_lastUsedDirectory() {
    _bPathValid = false;
    _bLoadedFromExistingQSettings = false;
}

void S_AppRecall_lastUsedDirectory::set(const QString& qstrValidPath) {
    _qstrPath = qstrValidPath;
    _bPathValid = true;
    qDebug() << __FUNCTION__ << "(S_AppRecall_lastUsedDirectory)" << "_qstrPath set to: " << _qstrPath;
}

bool S_AppRecall_lastUsedDirectory::get(QString& qstrValidPath) const {
    if (_bPathValid) {
        qstrValidPath = _qstrPath;
    }
    return(_bPathValid);
}

bool S_AppRecall_lastUsedDirectory::isValid() {
    return(_bPathValid);
}

void S_AppRecall_lastUsedDirectory::clear() {
    _qstrPath.clear();
    _bLoadedFromExistingQSettings = false;
    _bPathValid = false;
}

void S_AppRecall_lastUsedDirectory::debugShow() {
    qDebug() << __FUNCTION__ << "(S_AppRecall_lastUsedDirectory)" << "_qstrPath = " << _qstrPath;
    qDebug() << __FUNCTION__ << "(S_AppRecall_lastUsedDirectory)" << "_bLoadedFromExistingQSettings = " << _bLoadedFromExistingQSettings;
    qDebug() << __FUNCTION__ << "(S_AppRecall_lastUsedDirectory)" << "_bPathValid = " << _bPathValid;
}

bool S_AppRecall_lastUsedDirectory::checkValid(const QString& qstrCacheStorageRootPath, QString& strErrorReason) {
    bool bChecked = checkDirectoryExistAndReadableWritableWithStrErrorReason(qstrCacheStorageRootPath, strErrorReason);
    return(bChecked);
}


void S_AppRecall::clear() {
    _sas_lastUsedDirectory.clear();
}

void S_AppRecall::debugShow() {
    qDebug() << __FUNCTION__ << "(S_AppRecall)";
    _sas_lastUsedDirectory.debugShow();
}
