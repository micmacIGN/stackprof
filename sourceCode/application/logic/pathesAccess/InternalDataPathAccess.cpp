#include "InternalDataPathAccess.h"

#include "appDataResourcesAccess.h"

#include <QString>
#include <QDir>

QString getProjDBPath() {
    return( getAppDataResourcesPath() + QDir::separator() + "projdb");
}
