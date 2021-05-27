#ifndef APP_DISK_PATH_CACHE_STORAGE_HPP
#define APP_DISK_PATH_CACHE_STORAGE_HPP

#include <QString>

class AppDiskPathCacheStorage {

  public:
    AppDiskPathCacheStorage();
    
    bool setRootDir(const QString& qstrRootDir);
    
    bool makeSubDirectories();
    
    QString getRootDir();
    QString getSubDir_EPSGCodes();
    
  private:
  
    QString _qstrRootDir;
    bool _bRootDirValid;

    QString _qstrSubDir_EPSGCodesAssociatedToImages;
    bool _bSubDirsValid;
        
};

/* To handle the case of existence of different files with the same name,
 * and because we consider that making a hash on the files content can be to time-consuming to detect identical files,
 * we accept that the cache could contains duplicated images pyramid for two identicals files located in two different directories.
 * Hence, moving a file lead to its image pyramid rebuilding, and the app does not know that the file was located somewhere else before with an existing image pyramid.
 * This is considered a better solution than accepting only one image pyramid for any file with the same name, without location consideration.
 * To simplified the cleanup of the cache storage (by user for now), the user just need to delete the directories matching with the image pyramid files.
 * Hence, the idea is to make images pyramid directory names as the source image filenames + a unique numeric value wich will permit to match with a list of directory
 * where the filenames is located. This directory list will be stored in a file at the root directory of the cachestorage directory
*/

class CacheStorageContent {

    public:
        CacheStorageContent();

        bool loadContentFile();

        bool writeContentFile();

        bool cleanupContentFile_checkingDiskContent(); //this permits to update the content, letting the user simply remove the directories

        QString getZLIDirectoryForPathAndFilename(QString strPathFilename);

};

#endif // APP_DISK_PATH_CACHE_STORAGE_HPP
