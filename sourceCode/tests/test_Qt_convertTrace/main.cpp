#include <QCoreApplication>
#include <QDebug>

#include "logic/model/core/routeContainer.h"

#include "logic/model/georef/georefModel_imagesSet.hpp"

#include "logic/model/core/geoConvertRoute.h"

#include "logic/pathesAccess/InternalDataPathAccess.h"

bool test_WorldFileDataSame_EPSGCode_same();
bool test_WorldFileDataDifferent_EPSGCode_same();
bool test_WorldFileDataDifferent_EPSGCode_different();

bool test_WorldFileDataSame_EPSGCode_different();

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    test_WorldFileDataSame_EPSGCode_same();

    test_WorldFileDataDifferent_EPSGCode_same();

    test_WorldFileDataDifferent_EPSGCode_different();

    //exhaustive test:
    //test_WorldFileDataSame_EPSGCode_different();
}

bool test_WorldFileDataSame_EPSGCode_same() {

    RouteContainer routeCont;

    //---
    //insert three traces. The second trace will be used for the conversion test
    //---
    Route r0;
    double dW = 699.0;
    double dH = 929.0;
    bool bFirstPointForTheRoute = false;

    routeCont.addRoute();
    Route& r0Ref = routeCont.getRouteRef(0);
    r0Ref.tryAddPointAtRouteEnd({   10.0,  11.0}, bFirstPointForTheRoute);
    r0Ref.tryAddPointAtRouteEnd({  100.0, 110.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r1Ref = routeCont.getRouteRef(1);
    r1Ref.tryAddPointAtRouteEnd({   0.0,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({   0.0, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({ dW/2.0, dH/2.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r2Ref = routeCont.getRouteRef(2);
    r2Ref.tryAddPointAtRouteEnd({   20.0,  22.0}, bFirstPointForTheRoute);
    r2Ref.tryAddPointAtRouteEnd({  200.0, 220.0}, bFirstPointForTheRoute);

    //---
    //show routeContainer content
    //---
    routeCont.showContent();


    //---
    //create and inir ProjContext for GeoRefModel_imagesSets
    //---
    const QString strPathProjDB = getProjDBPath();

    GeoRefModel_imagesSet forRouteImport_geoRefModelImgSetAboutRouteset;
    GeoRefModel_imagesSet GeoRefM_imagesSet_dest;

    bool bReport_geoRefModelImagesSet_createContext  = false;
    bReport_geoRefModelImagesSet_createContext = GeoRefM_imagesSet_dest.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //---
    //load Geo data which match with the trace to convert (from this geo data) SP27GTIF 26771
    //---

    bool bSomeLoadedButNotUsable = false;
    bool bSetAndLoaded = forRouteImport_geoRefModelImgSetAboutRouteset.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/SP27GTIF.TIF" },
        bSomeLoadedButNotUsable);
    if (!bSetAndLoaded) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData = forRouteImport_geoRefModelImgSetAboutRouteset.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }

    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG = forRouteImport_geoRefModelImgSetAboutRouteset.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }

    //---
    //load Geo data which match with the destination for the trace conversion SP27GTIF_sameWF.TIF 26771
    //---
    bool bSomeLoadedButNotUsable_dest = false;
    bool bSetAndLoaded_dest = GeoRefM_imagesSet_dest.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/SP27GTIF_sameWF.TIF" },
        bSomeLoadedButNotUsable_dest);
    if (!bSetAndLoaded_dest) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData_dest = GeoRefM_imagesSet_dest.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData_dest != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG_dest = GeoRefM_imagesSet_dest.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG_dest != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //
    //---
    int selectedRouteId_forRouteImport = 1;
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaStrMsgStatus_aboutGeoConvertAndImportRoute;

    /*bool bConversionStatus = */geoConvertSelectedRouteFromRouteset_andKeepAlone(
        routeCont,
        selectedRouteId_forRouteImport,
        &GeoRefM_imagesSet_dest,
        forRouteImport_geoRefModelImgSetAboutRouteset,
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

    //---
    //show routeContainer content after trace conversion
    //---
    routeCont.showContent();

    return(true);
}




bool test_WorldFileDataDifferent_EPSGCode_same() {

    RouteContainer routeCont;

    //---
    //insert three traces. The second trace will be used for the conversion test
    //---
    Route r0;
    double dW = 699.0;
    double dH = 929.0;
    bool bFirstPointForTheRoute = false;

    routeCont.addRoute();
    Route& r0Ref = routeCont.getRouteRef(0);
    r0Ref.tryAddPointAtRouteEnd({   10.0,  11.0}, bFirstPointForTheRoute);
    r0Ref.tryAddPointAtRouteEnd({  100.0, 110.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r1Ref = routeCont.getRouteRef(1);
    r1Ref.tryAddPointAtRouteEnd({        0.5,        0.5}, bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({   dW-1+0.5,        0.5}, bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({   dW-1+0.5,   dH-1+0.5}, bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({        0.5,   dH-1+0.5}, bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({ 0.5+dW/2.0, 0.5+dH/2.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r2Ref = routeCont.getRouteRef(2);
    r2Ref.tryAddPointAtRouteEnd({   20.0,  22.0}, bFirstPointForTheRoute);
    r2Ref.tryAddPointAtRouteEnd({  200.0, 220.0}, bFirstPointForTheRoute);

    //---
    //show routeContainer content
    //---
    routeCont.showContent();


    //---
    //create and inir ProjContext for GeoRefModel_imagesSets
    //---
    const QString strPathProjDB = getProjDBPath();

    GeoRefModel_imagesSet forRouteImport_geoRefModelImgSetAboutRouteset;
    GeoRefModel_imagesSet GeoRefM_imagesSet_dest;

    bool bReport_geoRefModelImagesSet_createContext  = false;
    bReport_geoRefModelImagesSet_createContext = GeoRefM_imagesSet_dest.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //---
    //load Geo data which match with the trace to convert (from this geo data) SP27GTIF 26771
    //---

    bool bSomeLoadedButNotUsable = false;
    bool bSetAndLoaded = forRouteImport_geoRefModelImgSetAboutRouteset.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/SP27GTIF.TIF" },
        bSomeLoadedButNotUsable);
    if (!bSetAndLoaded) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData = forRouteImport_geoRefModelImgSetAboutRouteset.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG = forRouteImport_geoRefModelImgSetAboutRouteset.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //load Geo data which match with the destination for the trace conversion SP27GTIF_fakeDifferentWF.TIF 26771
    //---
    bool bSomeLoadedButNotUsable_dest = false;
    bool bSetAndLoaded_dest = GeoRefM_imagesSet_dest.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/fakeData/SP27GTIF_fakeDifferentWF.TIF" },
        bSomeLoadedButNotUsable_dest);
    if (!bSetAndLoaded_dest) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData_dest = GeoRefM_imagesSet_dest.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData_dest != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG_dest = GeoRefM_imagesSet_dest.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG_dest != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //
    //---
    int selectedRouteId_forRouteImport = 1;
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaStrMsgStatus_aboutGeoConvertAndImportRoute;

    qDebug() << __FUNCTION__ << "************* call now geoConvertSelectedRouteFromRouteset_andKeepAlone";

    /*bool bConversionStatus = */geoConvertSelectedRouteFromRouteset_andKeepAlone(
        routeCont,
        selectedRouteId_forRouteImport,
        &GeoRefM_imagesSet_dest,
        forRouteImport_geoRefModelImgSetAboutRouteset,
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

    //---
    //show routeContainer content after trace conversion
    //---
    routeCont.showContent();

    return(true);
}


//#LP missing error cases handling about test init
bool test_WorldFileDataDifferent_EPSGCode_different() {

    RouteContainer routeCont;

    //---
    //insert three traces. The second trace will be used for the conversion test
    //---
    Route r0;
    double dW = 699.0;
    double dH = 929.0;
    bool bFirstPointForTheRoute = false;

    routeCont.addRoute();
    Route& r0Ref = routeCont.getRouteRef(0);
    r0Ref.tryAddPointAtRouteEnd({   10.0,  11.0}, bFirstPointForTheRoute);
    r0Ref.tryAddPointAtRouteEnd({  100.0, 110.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r1Ref = routeCont.getRouteRef(1);
    r1Ref.tryAddPointAtRouteEnd({   0.0,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({   0.0, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({ dW/2.0, dH/2.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r2Ref = routeCont.getRouteRef(2);
    r2Ref.tryAddPointAtRouteEnd({   20.0,  22.0}, bFirstPointForTheRoute);
    r2Ref.tryAddPointAtRouteEnd({  200.0, 220.0}, bFirstPointForTheRoute);

    //---
    //show routeContainer content
    //---
    routeCont.showContent();


    //---
    //create and inir ProjContext for GeoRefModel_imagesSets
    //---
    const QString strPathProjDB = getProjDBPath();

    GeoRefModel_imagesSet forRouteImport_geoRefModelImgSetAboutRouteset;
    GeoRefModel_imagesSet GeoRefM_imagesSet_dest;

    bool bReport_geoRefModelImagesSet_createContext  = false;
    bReport_geoRefModelImagesSet_createContext = GeoRefM_imagesSet_dest.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //---
    //load Geo data which match with the trace to convert (from this geo data) SP27GTIF 26771
    //---

    bool bSomeLoadedButNotUsable = false;
    bool bSetAndLoaded = forRouteImport_geoRefModelImgSetAboutRouteset.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/SP27GTIF.TIF" },
        bSomeLoadedButNotUsable);
    if (!bSetAndLoaded) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData = forRouteImport_geoRefModelImgSetAboutRouteset.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG = forRouteImport_geoRefModelImgSetAboutRouteset.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //load Geo data which match with the destination for the trace conversion UTM2GTIF.TIF 26716
    //---
    bool bSomeLoadedButNotUsable_dest = false;
    bool bSetAndLoaded_dest = GeoRefM_imagesSet_dest.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/UTM2GTIF.TIF" },
        bSomeLoadedButNotUsable_dest);
    if (!bSetAndLoaded_dest) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData_dest = GeoRefM_imagesSet_dest.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData_dest != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG_dest = GeoRefM_imagesSet_dest.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG_dest != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //
    //---
    int selectedRouteId_forRouteImport = 1;
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaStrMsgStatus_aboutGeoConvertAndImportRoute;

    /*bool bConversionStatus = */geoConvertSelectedRouteFromRouteset_andKeepAlone(
        routeCont,
        selectedRouteId_forRouteImport,
        &GeoRefM_imagesSet_dest,
        forRouteImport_geoRefModelImgSetAboutRouteset,
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

    //---
    //show routeContainer content after trace conversion
    //---
    routeCont.showContent();

    return(true);
}

bool test_WorldFileDataSame_EPSGCode_different() {

    RouteContainer routeCont;

    //---
    //insert three traces. The second trace will be used for the conversion test
    //---
    Route r0;
    double dW = 699.0;
    double dH = 929.0;
    bool bFirstPointForTheRoute = false;

    routeCont.addRoute();
    Route& r0Ref = routeCont.getRouteRef(0);
    r0Ref.tryAddPointAtRouteEnd({   10.0,  11.0}, bFirstPointForTheRoute);
    r0Ref.tryAddPointAtRouteEnd({  100.0, 110.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r1Ref = routeCont.getRouteRef(1);
    r1Ref.tryAddPointAtRouteEnd({   0.0,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1,  0.0},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({  dW-1, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({   0.0, dH-1},    bFirstPointForTheRoute);
    r1Ref.tryAddPointAtRouteEnd({ dW/2.0, dH/2.0}, bFirstPointForTheRoute);

    routeCont.addRoute();
    Route& r2Ref = routeCont.getRouteRef(2);
    r2Ref.tryAddPointAtRouteEnd({   20.0,  22.0}, bFirstPointForTheRoute);
    r2Ref.tryAddPointAtRouteEnd({  200.0, 220.0}, bFirstPointForTheRoute);

    //---
    //show routeContainer content
    //---
    routeCont.showContent();


    //---
    //create and inir ProjContext for GeoRefModel_imagesSets
    //---
    const QString strPathProjDB = getProjDBPath();

    GeoRefModel_imagesSet forRouteImport_geoRefModelImgSetAboutRouteset;
    GeoRefModel_imagesSet GeoRefM_imagesSet_dest;

    bool bReport_geoRefModelImagesSet_createContext  = false;
    bReport_geoRefModelImagesSet_createContext = GeoRefM_imagesSet_dest.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //---
    //load Geo data which match with the trace to convert (from this geo data) SP27GTIF 26771
    //---

    bool bSomeLoadedButNotUsable = false;
    bool bSetAndLoaded = forRouteImport_geoRefModelImgSetAboutRouteset.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/SP27GTIF.TIF" },
        bSomeLoadedButNotUsable);
    if (!bSetAndLoaded) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData = forRouteImport_geoRefModelImgSetAboutRouteset.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }

    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG = forRouteImport_geoRefModelImgSetAboutRouteset.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //load Geo data which match with the destination for the trace conversion SP27GTIF (fake 26716)
    //---
    bool bSomeLoadedButNotUsable_dest = false;
    bool bSetAndLoaded_dest = GeoRefM_imagesSet_dest.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/fakeData/SP27GTIF_fakeEPSGDifferent.TIF" },
        bSomeLoadedButNotUsable_dest);
    if (!bSetAndLoaded_dest) {
        return(false);
    }
    //check Word file data
    e_imagesSetStatus_geoTFWData eimageSetStatus_geoTFWData_dest = GeoRefM_imagesSet_dest.check_TFWDataSet();
    if (eimageSetStatus_geoTFWData_dest != e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available) {
        return(false);
    }
    //get EPSG code
    e_imagesSetStatus_geoEPSG eImagesSetStatus_geoEPSG_dest = GeoRefM_imagesSet_dest.searchInCache_associatedEPSGCode();
    if  (eImagesSetStatus_geoEPSG_dest != e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available) {
        return(false);
    }
    //---
    //
    //---
    int selectedRouteId_forRouteImport = 1;
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute sbaStrMsgStatus_aboutGeoConvertAndImportRoute;

    /*bool bConversionStatus = */geoConvertSelectedRouteFromRouteset_andKeepAlone(
        routeCont,
        selectedRouteId_forRouteImport,
        &GeoRefM_imagesSet_dest,
        forRouteImport_geoRefModelImgSetAboutRouteset,
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

    //---
    //show routeContainer content after trace conversion
    //---
    routeCont.showContent();

    return(true);

}
