#ifndef SAPPRECALL_H
#define SAPPRECALL_H

#include <QString>
#include <QMetaType>

struct S_AppRecall_lastUsedDirectory {
    QString _qstrPath;

    bool _bPathValid;
    bool _bLoadedFromExistingQSettings;

    S_AppRecall_lastUsedDirectory();

    bool checkValid(const QString& qstrCacheStorageRootPath, QString& strErrorReason);
//  bool checkValid_and_set(const QString& qstrCacheStorageRootPath, QString& strErrorReason);

    bool isValid();

    void clear();
    void set(const QString& qstrValidPath);
    bool get(QString& qtrValidPath) const;

    void debugShow();
};
Q_DECLARE_METATYPE(S_AppRecall_lastUsedDirectory);

struct S_AppRecall {
    S_AppRecall_lastUsedDirectory _sas_lastUsedDirectory;
    void clear();
    void debugShow();
};
Q_DECLARE_METATYPE(S_AppRecall);

#endif // SAPPRECALL_H
