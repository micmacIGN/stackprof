#include <QString>
#include <QDir>
#include <QDebug>

#include "../../toolbox/toolbox_pathAndFile.h"

#include "AppDiskPathCacheStorage.hpp"
/*
//https://www.qtcentre.org/threads/5124-How-to-create-a-symlink-to-a-directory
#ifdef Q_OS_UNIX
    QFile::link(sourceDir.absolutePath(), destDir.absolutePath());
#endif

#ifdef Q_OS_WIN
    QFile::link(sourceDir.absolutePath(), destDir.absolutePath().append(".lnk"));
#endif
*/

AppDiskPathCacheStorage::AppDiskPathCacheStorage(): _bRootDirValid(false),  _bSubDirsValid(false) {

}

//qstrRootDir has to exist
bool AppDiskPathCacheStorage::setRootDir(const QString& qstrRootDir) {
    _qstrRootDir.clear();
    _bRootDirValid = false;

    _qstrSubDir_EPSGCodesAssociatedToImages.clear();
    _bSubDirsValid = false;

    QString qStrPathAndFilenameRootDir = getPathAndFilenameFromPotentialSymlink(qstrRootDir);
    QDir qdirRoot(qStrPathAndFilenameRootDir);
    if (!qdirRoot.exists()) {
        qDebug() << __FUNCTION__ << " : AppDiskCacheStorage : error: directoy has to exist ( " << qStrPathAndFilenameRootDir << " )";
        return(false);
    }
    _qstrRootDir = qStrPathAndFilenameRootDir;
    _bRootDirValid = true;
    return(true);
}

bool AppDiskPathCacheStorage::makeSubDirectories() {

    _qstrSubDir_EPSGCodesAssociatedToImages.clear();
    _bSubDirsValid = false;

    if (!_bRootDirValid) {
        qDebug() << __FUNCTION__ << " : AppDiskCacheStorage : error: can not make sub directories: RootDir is invalid";
        return(false);
    }
    //set and create the _EPSG_cache directory if it does not exists
    /*_qstrSubDir_EPSGCodesAssociatedToImages = _qstrRootDir + QDir::separator() + "_EPSG_cache";
    QDir qdirSubDir_EPSGCodes(_qstrSubDir_EPSGCodesAssociatedToImages);
    if (!qdirSubDir_EPSGCodes.exists()) {
        if (!qdirSubDir_EPSGCodes.mkdir(_qstrSubDir_EPSGCodesAssociatedToImages)) {
            qDebug() << __FUNCTION__ << ": error: can not mkdir " << _qstrSubDir_EPSGCodesAssociatedToImages;
            _qstrSubDir_EPSGCodesAssociatedToImages.clear();
            return(false);
        }
    }*/

    _bSubDirsValid = true;
    return(true);
}
    
QString AppDiskPathCacheStorage::getRootDir() {
    return(_qstrRootDir);
}

QString AppDiskPathCacheStorage::getSubDir_EPSGCodes() {
    return(_qstrSubDir_EPSGCodesAssociatedToImages);
}
