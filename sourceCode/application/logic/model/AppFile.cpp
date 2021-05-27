#include <QString>

#include <QDebug>

#include "AppFile.hpp"

AppFile::AppFile(): _bFilenameSet(false), _bModifiedSinceLastSavedToDisk(false) {

}

void AppFile::clear() {
    _strFilename.clear();
    _bFilenameSet = false;
    _bModifiedSinceLastSavedToDisk = false;
}

//call this after the first write on disk
void AppFile::setFilename(const QString& strFilename) {
    _strFilename = strFilename;
    _bFilenameSet = true;
}

void AppFile::setState_modified(bool bModified) {
    qDebug() << __FUNCTION__ << "(_strFilename = " << _strFilename;
    _bModifiedSinceLastSavedToDisk = bModified;
}

bool AppFile::filenameSet() const {
    return(_bFilenameSet);
}

bool AppFile::modified() const {
    return(_bModifiedSinceLastSavedToDisk);
}

QString AppFile::getFilename() const {
    return(_strFilename);
}
