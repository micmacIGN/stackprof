#ifndef GEOREFIMAGESSETSTATUS_HPP
#define GEOREFIMAGESSETSTATUS_HPP

#include <QString>

#include "georefModel_imagesSet.hpp"

struct S_boolAndStrMsgStatusAbout_geoRefImageSetStatus {
    bool _bWorldFileData_available;
    QString _strMsgAboutWorldFileData;
    QString _strMsgAboutWorldFileData_errorDetails;

    bool _bEPSG_available;
    QString _strMsgAboutEPSG;
    QString _strMsgAboutEPSG_errorDetails;
    QString _strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation;
    S_boolAndStrMsgStatusAbout_geoRefImageSetStatus();    
    void showContent();
};


enum e_imagesSetStatus_geoTFWData_reducedToBigCases {
    e_iSS_geoTFWData_rTBC_notSetValue,

    e_iSS_geoTFWData_rTBC_notAvailable_noneFound,
    e_iSS_geoTFWData_rTBC_available,

    e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData, //for project only (multiple iimage as input possible) , never for routeset (only one image for background)

    e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason,

};

struct S_e_geoRefImagesSetStatus {
    e_imagesSetStatus_geoTFWData _eiSSgTFW_allStatusCase;
    e_imagesSetStatus_geoEPSG _eiSS_geoEPSG;

    e_imagesSetStatus_geoTFWData_reducedToBigCases _eiSSgTFW_reducedToBigCases;

    S_e_geoRefImagesSetStatus();
};

void routesetEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_routeSetEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_routeSetEdition);

void routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_routeSetEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_routeSetEdition);

void projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_projectEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition);

void projectEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
        GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet,
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_projectEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition);


#endif // GEOREFIMAGESSETSTATUS_HPP
