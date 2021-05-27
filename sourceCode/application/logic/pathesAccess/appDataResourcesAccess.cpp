#include "appDataResourcesAccess.h"

#include <QApplication>

#include <QString>
#include <QDir>

QString getAppDataResourcesPath() {

#if defined(Q_OS_WIN)

    #error "about dataResourcesAccess source code: getResourcesPath() directory not defined for windows platform"

#elif defined(Q_OS_OSX)
    return QApplication::applicationDirPath() + "/../Resources";

#elif defined(Q_OS_LINUX)
    return QApplication::applicationDirPath();

#else
    //return QApplication::applicationDirPath();
    #error "about dataResourcesAccess source code: getResourcesPath() directory is undefined for the current unknown platform"

#endif
}
