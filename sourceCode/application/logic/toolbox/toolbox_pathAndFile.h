#ifndef toolbox_pathAndFile_H
#define toolbox_pathAndFile_H

class QString;

QString getPathAndFilenameFromPotentialSymlink(const QString& strFilename);

QString replaceDirSeparatorBySlash(const QString& strIn);
void splitStrPathAndFile(const QString& strIn, QString& strPath, QString& strFile);
QString getStrPathFromPathFile(const QString& strPathFile);

QString replaceSlashByDirSeparator(const QString& strIn);
QString concatStrPathAndFile(const QString& strPath, const QString& strFile);

bool fileExists(const QString& strFilename);
bool dirExists(const QString& strDir);

bool checkDirectoryExistAndReadableWritableWithStrErrorReason(const QString& qstrDirectory, QString& strErrorReason);
bool checkDirectoryExistAndReadableWritable(const QString& qstrDirectory);
bool checkDirectoryExistAndReadableWritable(const QString& qstrDirectory, bool& bDirectoryNameNotEmpty, bool& bExists, bool& bISRW);

#include <QDateTime>
bool getDateTimeLastModified_aboutFile(const QString& strFilename, QDateTime &qDateTime_lastModified);

#endif
