#include <QString>
#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include "toolbox_pathAndFile.h"
 
//do not check if it's a file or a directory
QString getPathAndFilenameFromPotentialSymlink(const QString& strFilename) {

        QString qStrPathAndFilename;
        if (strFilename.isEmpty()) {
            return(qStrPathAndFilename);
        }
        QFileInfo fileInfo(strFilename);
        if (fileInfo.isSymLink()) {
            qStrPathAndFilename = fileInfo.symLinkTarget();
        } else {
            qStrPathAndFilename =  fileInfo.absoluteFilePath();
        }
        return(qStrPathAndFilename);
    /*}*/
}

bool fileExists(const QString& strFilename) {
    QFileInfo fileInfo(strFilename);
    if (fileInfo.isDir()) {
        return(false);
    }
    if (!fileInfo.exists(strFilename)) {
        return(false);
    }
    return(fileInfo.isFile());
}

bool dirExists(const QString& strDir) {
    QDir qdirCheckExists(strDir);
    if (qdirCheckExists.exists()) {
        return(true);
    }
    qDebug() << __FUNCTION__ << "strDir does not exist: " << strDir;
    return(false);
}

QString replaceDirSeparatorBySlash(const QString& strIn) {

    QString qstrWithReplacedDirSep = strIn;

    //replace any directory separator not '/' as '/' (windows typically)
    //When loading from json, the reverse operation will be made
    QChar cDirSeparator = QDir::separator();
    if ( cDirSeparator!= '/') {
        qstrWithReplacedDirSep.replace(R"(\)", "/"); //C++11 raw strings literals
    }
    return(qstrWithReplacedDirSep);
}

void splitStrPathAndFile(const QString& strIn, QString& strPath, QString& strFile) {
    QFileInfo qfinfos(strIn);
    strFile = qfinfos.fileName();
    strPath = qfinfos.absolutePath();
}

QString getStrPathFromPathFile(const QString& strPathFile) {
    QFileInfo qfinfos(strPathFile);
    QString strPath = qfinfos.absolutePath(); //##LP no symlink replace ?
    return(strPath);
}

QString replaceSlashByDirSeparator(const QString& strIn) {

    QString qstrWithReplacedSlash = strIn;

    //replace any directory separator not '/' as '/' (windows typically)
    //When loading from json, the reverse operation will be made
    QChar cDirSeparator = QDir::separator();
    if ( cDirSeparator!= '/') {
        qstrWithReplacedSlash.replace("/", R"(\)"); //C++11 raw strings literals
    }
    return(qstrWithReplacedSlash);
}

QString concatStrPathAndFile(const QString& strPath, const QString& strFile) {
    return(strPath + QDir::separator() + strFile);
}


bool checkDirectoryExistAndReadableWritableWithStrErrorReason(const QString& qstrDirectory, QString& strErrorReason) {

    bool bDirectoryNameNotEmpty = false;
    bool bExists = false;
    bool bISRW = false;

    bool bCheckDirectoryExistAndReadableWritable = checkDirectoryExistAndReadableWritable(qstrDirectory, bDirectoryNameNotEmpty, bExists, bISRW);

    if (bCheckDirectoryExistAndReadableWritable) {
        return(true);
    }
    if (!bDirectoryNameNotEmpty) {
        strErrorReason = "no directory selected";
        return(false);
    }
    if (!bExists) {
        strErrorReason = "the directory does not exist";
        return(false);
    }
    if (!bISRW) {
        strErrorReason = "the directory needs read and write permissions for the current user";
        return(false);
    }
    return(false);
}

bool checkDirectoryExistAndReadableWritable(const QString& qstrDirectory, bool& bDirectoryNameNotEmpty, bool& bExists, bool& bISRW) {

    bDirectoryNameNotEmpty = false;
    bExists = false;
    bISRW = false;

    //check string not empty
    if (qstrDirectory.isEmpty()) {
        qDebug() << "qstrDirectory.isEmpty()";
        return(false);
    }
    bDirectoryNameNotEmpty = true;

    //check directory exist
    if (!dirExists(qstrDirectory)) {
        qDebug() << __FUNCTION__ << "qstrDirectory does not exist: " << qstrDirectory;
        return(false);
    }
    bExists = true;

    //check read write permission on directory:

    //@#LP windows: https://stackoverflow.com/questions/35317097/how-to-check-if-a-folder-is-writable-using-qt
    //https://doc.qt.io/archives/qt-4.8/qfile.html#platform-specific-issues
    /*' Platform Specific Issues
        File permissions are handled differently on Linux/Mac OS X and Windows. In a non writable directory on Linux,
        files cannot be created. This is not always the case on Windows, where, for instance, the 'My Documents'
        directory usually is not writable, but it is still possible to create files in it.' */
    //@#LP still the same with Qt >= 5.12 ?

    //@#LP move this is in OS specific class
    QFileInfo directoryFileInfo(qstrDirectory);
    if(   !directoryFileInfo.isDir()
       || !directoryFileInfo.isWritable()
       || !directoryFileInfo.isReadable()) {
        return(false);
    }
    bISRW = true;

    return(true);
}

bool checkDirectoryExistAndReadableWritable(const QString& qstrDirectory) {
    bool bDirectoryNameNotEmpty = false;
    bool bExists = false;
    bool bISRW = false;
    return(checkDirectoryExistAndReadableWritable(qstrDirectory, bDirectoryNameNotEmpty, bExists, bISRW));
}

#include <QDateTime>

bool getDateTimeLastModified_aboutFile(const QString& strFilename, QDateTime &qDateTime_lastModified) {

    QString strFilenameFromPotentialSymLink = getPathAndFilenameFromPotentialSymlink(strFilename);
    if (!fileExists(strFilenameFromPotentialSymLink)) {
        return(false);
    }

    QFileInfo fileInfo(strFilenameFromPotentialSymLink);

    //qDateTime_birthTime = fileInfo.birthTime(); //@#LP 'QFileDevice::FileBirthTime : When the file was created (may not be not supported on UNIX)'
    qDateTime_lastModified = fileInfo.lastModified();

    //qDebug() << __FUNCTION__ << "qDateTime_birthTime   :" << qDateTime_birthTime;
    qDebug() << __FUNCTION__ << "qDateTime_lastModified:" << qDateTime_lastModified;

    return(qDateTime_lastModified.isValid());
}
