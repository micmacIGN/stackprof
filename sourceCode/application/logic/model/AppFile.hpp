#ifndef AppFile_hpp
#define AppFile_hpp

#include <QString>

//Generic class to handle application files modifications state
//typically: one instance used for a route application file; one used for a project application file

class AppFile {
public:
    AppFile();
    void setFilename(const QString& strFilename);
    void setState_modified(bool bModified);

    bool filenameSet() const;
    bool modified() const;
    QString getFilename() const;
    void clear();

private:
    bool _bFilenameSet;
    bool _bModifiedSinceLastSavedToDisk;
    QString _strFilename;
};


#endif
