#ifndef ZLIStorageContentManager_h
#define ZLIStorageContentManager_h

#include <QString>
#include <QDir>
#include <QVariantMap>

#include <QDebug>

#include "../toolbox/toolbox_pathAndFile.h"
#include "iojsoninfos.h"

class ZLIStorageContentManager {
  
  public:
    ZLIStorageContentManager();

    bool setRootPath(const QString& qstrRootPath);

    bool load_ZLIStorageContentFile(bool& bFileDoesNotExist);
    bool save_ZLIStorageContentFile();

    bool checkExistsInDBandGetId(const QString& qstrPathAndFilename, qlonglong &qll_uniqueID, bool& bExist);

    qlonglong get_nextAvailableUniqueIDForDirName();
    bool add_pair_strPathAndFilename_uniqueIdForZLIRelativePath(const QString&strPathAndFilename, qlonglong uniqueIdForZLIRelativePath);
    void increase_currentUsedMaximalUniqueID();

    //
    //bool addAndMakeDir(const QString& qstrPathAndFilename);

    bool findZLIStorageAbsoluteDirectoryForPathAndFilename(const QString& qstrPathAndFilename,
                                                           QString& qstrAbsolutePathOfZLIforFile);

    //bool add(const QString& qstrPathAndFilename, const QString& qstrRelativePathOfZLIforFile);
    //bool removeRelativePathOfZLIforFile_forAccordingDisappearedDirectories();

    bool removeRelativePathOfZLIforFile_forAccordingDisappearedDirectories(bool& bSomeRemoved);

private:
  QString _strRootPath;

  static constexpr const char* _strZLIStorageContentFilename = "ZLIstorageContent.json";

  //IOJsonInfos _IOJsoFileContent;

  //QVariantMap _qVariantMap_strPathAndFilename_accordingVariantStrRelativePathOfZLIforFile;

  QVariantMap _qVariantMap_strPathAndFilename_uniqueIdForZLIRelativePath;
  qlonglong _qll_currentUsedMaximalUniqueID;

};

#endif //ZLIStorageContentManager_h


