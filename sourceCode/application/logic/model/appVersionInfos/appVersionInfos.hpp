#ifndef APP_VERSION_INFOS_HPP
#define APP_VERSION_INFOS_HPP

#include <QStringList>

class AppVersionInfos {

  public:    

    static constexpr const char* _sctct_softwareName = "StackProf";
    static constexpr const char* _sctct_appVersion = "1.02";
    static constexpr const char* _sctct_appVersionDeliveryDate = "11 June 2021";

    static constexpr const char* _sctct_mainContact = "delorme@ipgp.fr"; //contact: *developperEmail*, delorme@ipgp.fr

    static constexpr const char* _sctct_developpementCreditsText =
"This software was developed at Institut de physique du globe de Paris (IPGP)\
 \
by Laurent Pourchet, in collaboration with IGN. It was funded by TOSCA, CNES and IPGP.";

    static constexpr const char* _sctct_licenceType = "CeCILL-B";

    static QStringList get_thirdPartyOpenSourceLibsUsed_nameAndVersion() {
        return {
            "Qt framework version 5.12 (on linux) / 5.13 (on Mac OS X)",
            "OpenImageIO library 2.1.20",
            "OpenEXR library 2.4.0",
            "GNU Scientific Library 2.6",
            "PROJ library 6.3.0 (with its proj.db database file)",
            "SQLite 3.33.0",
            "Boost C++ Libraries 1.70.0",
            "jpeg library release 8b",
            "png library 1.6.37",
            "tiff library 4.1.0",
        };
    }
};

#endif // APP_VERSION_INFOS_HPP
