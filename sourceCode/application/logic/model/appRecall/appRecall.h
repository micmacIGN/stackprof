#ifndef AppRecall_h
#define AppRecall_h

#include <QObject>
#include <QString>

#include "SAppRecall.h"

class AppRecall : public QObject {

    Q_OBJECT

public:

    AppRecall(QObject *parent = nullptr);

    void clearLoaded();
    bool containsSomeInvalidValues();

    bool load_setMissingToDefault(/*bool &bCacheStorageRootPath_loaded, bool &bCacheStorageRootPath_valid*/);

    bool save();

    void setFrom_sAppRecall(const S_AppRecall& sAppRecall);

    //bool get_lastUsedDirectory(QString& qstrLastUsedDirectory);
    QString get_lastUsedDirectory() const;

    bool set_lastUsedDirectory(const QString& qstrLastUsedDirectory);
    bool set_lastUsedDirectoryFromPathFile(const QString& qstrPathAndFileName);

    const S_AppRecall& getAllsAppRecall() const;

    //bool checkValid_cacheStorageRootPath(const QString& qstrCacheStoragePath);

    //void sendSAppSettings();

//signals:
    //void signal_sendSAppSettings(S_AppSettings sAppSettings);
public slots:
    void slot_newLastUsedDirectory(const QString& qstrLastUsedDirectory);

private:
    S_AppRecall _sAppRecall;

private:

    //for storage:
    //
    static constexpr const char* _strCompanyName = "ipgp_fr";
    static constexpr const char* _strRecallAppName     = "stackp_recall";
    //
    static constexpr const char* _strKey_lastUsedDirectory = "lastUsedDirectory";

};

#endif //AppRecall_h

