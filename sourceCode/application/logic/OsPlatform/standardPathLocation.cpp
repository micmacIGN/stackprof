#include <QtGlobal>

#include <QStandardPaths>
#include <QString>

#include "standardPathLocation.hpp"

QString StandardPathLocation::strStartingDefaultDir() {

    QStringList qtrList_standardPaths_standardPathLocations  = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString strStartingDir;
    if (!qtrList_standardPaths_standardPathLocations.size()) {

        // very default path if standardLocations(QStandardPaths::HomeLocation) give nothing
        /*#ifdef Q_OS_LINUX
            strStartingDir = "/home";
        #elseif Q_OS_OSX //Q_OS_DARWIN //Q_OS_MACOS
            strStartingDir = "/Users";
        #elseif Q_OS_WIN
            strStartingDir = "C:\\";
        #else
            #error very default dir path strStartingDir not defined for this platform, please indicate
        #endif*/

        //@LP for an unknown reason, #elseif and #elif don't seems to work properly using here with Q_OS_... Hence use separated #ifdef instead
        #ifdef Q_OS_LINUX
                #define DEF_FOUND_OS___standardPathLocation_CPP
                strStartingDir = "/home";
        #endif

        #ifdef Q_OS_OSX //Q_OS_DARWIN //Q_OS_MACOS
                #ifdef  DEF_FOUND_OS___standardPathLocation_CPP
                    #error OS defined macros conflict  (DEF_FOUND_OS___standardPathLocation_CPP)
                #endif
                #define DEF_FOUND_OS___standardPathLocation_CPP
                strStartingDir = "/Users";
        #endif

        #ifdef Q_OS_WIN
                #ifdef  DEF_FOUND_OS___standardPathLocation_CPP
                    #error OS defined macros conflict  (DEF_FOUND_OS___standardPathLocation_CPP)
                #endif
                #define DEF_FOUND_OS___standardPathLocation_CPP
                strStartingDir = "C:\\";
        #endif

        #ifndef DEF_FOUND_OS___standardPathLocation_CPP
                #error very default dir path strStartingDir not defined for this platform, please indicate
        #endif


    } else {
        strStartingDir = qtrList_standardPaths_standardPathLocations[0];
    }

    return(strStartingDir);
}


