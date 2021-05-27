#ifndef GEOCONVERTROUTE_H
#define GEOCONVERTROUTE_H

#include <QString>

#include "routeContainer.h"
#include "../georef/georefModel_imagesSet.hpp"

struct S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute {
    bool _bConversionStatus;
    bool _bError_routeContainerAndIdxSelectedRouteCleared;
    bool _bRejected_routeOusideTheImage; //this case set _bError_routeContainerAndIdxSelectedRouteCleared to false
    QString _strMessage;
    QString _strMessage_errorDetails;
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute();
};

bool geoConvertSelectedRouteFromRouteset_andKeepAlone(
        RouteContainer &routeContainer,
        int selectedRouteId_forRouteImport,
        GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet,
        GeoRefModel_imagesSet &forRouteImport_geoRefModelImgSetAboutRouteset,
        S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute &sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

#endif

