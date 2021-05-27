#include "ApplicationModel.h"

#include "imageScene/customgraphicsscene_usingTPDO.h"
#include <QDebug>

#include <OpenImageIO/imageio.h>
using namespace OIIO;
#include "../io/InputImageFormatChecker.h"

#include "../io/ZoomOutLevelImageBuilder.h"

#include "zoomLevelImage/zoomHandler.h"

#include "io/iojsoninfos.h"

#include "smallImageNavScene/smallimagenav_customgraphicsscene.h"
#include "tilesProvider/tileprovider_3.h"

#include "core/ComputationCore_inheritQATableModel.h"

#include "model/dialogProfilsCurves/CustomGraphicsScene_profil.h"
#include "model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp"

#include "AppDiskPathCacheStorage/AppDiskPathCacheStorage.hpp"

#include <QCoreApplication>
#include <QDir>

#include <QElapsedTimer>

#include "core/valueDequantization.hpp"

#include "../toolbox/toolbox_pathAndFile.h"

#include "mainAppState.hpp"

#include "toolbox/toolbox_json.h"

#include "georef/geoRefImagesSetStatus.hpp"

S_boolAndStrMsgStatusAbout_geoRefImageSetStatus ApplicationModel::routesetEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus() {
    return(_sbaStrMsg_gRIS_routeSetEdition);
}

#include <QJsonDocument>
#include <QJsonObject>

bool ApplicationModel::routesetEditionFunction_writeRouteset() {

    _strDetailsAboutFail_routesetWrite.clear();

    if (!_appFile_routeset.filenameSet/*inUse*/()) {
        qDebug() << __FUNCTION__ << " error: !_appFile_routeset.inUse()";
        _strDetailsAboutFail_routesetWrite = "Dev Error #6 no filename used";
        return(false);
    }

    //if (_bUseGeoRefForTrace) {
        //write infos about the georef or the background image used to draw routes into
        if (!_ptrGeoRefModel_imagesSet) {
            qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
            qDebug() << __FUNCTION__ << "        Can not save without geoRef data from image background";
            _strDetailsAboutFail_routesetWrite = "Dev Error #7 GeoRefModel_imagesSet is nullptr";
            return(false);
        }
    //}

    QJsonObject QJsonObject_routeSetFileContent;
    QJsonObject_routeSetFileContent.insert("contentType", "routeset");

    QJsonObject_routeSetFileContent.insert("versionFormat", "0.5");

    //inserted here instead of in each route, to avoid useless repeated description
    QJsonObject_routeSetFileContent.insert("_descAbout_routePoint",
                                           "Points coordinates are [x, y] in pixel image coordinates. The top-let corner of top-left image pixel is [0.0, 0.0]. idPoint increases by 1 from first point to last point");
                                           //"idPoint: [x, y] coordinates from the image pixel top-let corner of top-left pixel = (0.0, 0.0). idPoint increase by 1 from first point to last point; first idPoint must be 0");

    /*QJsonObject::const_iterator qjsonobjCstIter = QJsonObject_aboutRouteSetFile.constBegin();
    for(; qjsonobjCstIter != QJsonObject_aboutRouteSetFile.constEnd(); ++qjsonobjCstIter) {
        qDebug() << __FUNCTION__ << "_qjsonObjInfos iter [key]= value:  [" << qjsonobjCstIter.key() << "] = " << qjsonobjCstIter.value();
    }*/

    //if (_bUseGeoRefForTrace) {
        //geo-referencement infos about background image:
        //this is useful to import route in a image with different world file parameter(s) and/or epsg code
        QJsonObject qjsonObj_geoRefInfos;
        bool bReport_toQJsonObject = _ptrGeoRefModel_imagesSet->toQJsonObject(qjsonObj_geoRefInfos);
        if (!bReport_toQJsonObject) {
            qDebug() << __FUNCTION__ << " error: _ptrGeoRefModel_imagesSet->toQJsonObject failed";
            _strDetailsAboutFail_routesetWrite = "Dev Error #8_6 GeoRefModel_imagesSet toQJsonObject() error";
            return(false);
        }
        QJsonObject_routeSetFileContent.insert("backgroundImage_geoRefInfos", qjsonObj_geoRefInfos);
    //}

    QJsonObject_routeSetFileContent.insert("bUseGeoRef", _bUseGeoRefForTrace);

    //routeset:
    QJsonArray qjsonArray_routeset;
    bool bReport_toQJsonArray = _routeContainer.toQJsonArray(qjsonArray_routeset, _strDetailsAboutFail_routesetWrite);
    if (!bReport_toQJsonArray) {
        qDebug() << __FUNCTION__ << " error: _routeContainer.toQJsonArray failed";
        return(false);
    }
    QJsonObject_routeSetFileContent.insert("routeset", qjsonArray_routeset);

    //qDebug() << "QJsonObject_routeSetFileContent: " << QJsonObject_routeSetFileContent;

    QJsonDocument JsonDoc(QJsonObject_routeSetFileContent);

    QString strOutFilename = _appFile_routeset.getFilename();

    qDebug() << __FUNCTION__ << " strOutFilename = " << strOutFilename;

    QFile outFile(strOutFilename);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        _strDetailsAboutFail_routesetWrite = "Can not open file for writing: " + strOutFilename;
        qDebug() << "error: (write open) Can not open file " << strOutFilename;
        return(false);
    }
    QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
    qint64 wroteBytes = outFile.write(qbArrayData);
    if (wroteBytes <= 0) {
        _strDetailsAboutFail_routesetWrite = "fail to write in file: " + strOutFilename;
         qDebug() << "error: fail to write in file: " << strOutFilename;
         return(false);
    }

    _appFile_routeset.setState_modified(false);
    return(true);
}



bool ApplicationModel::routesetEditionFunction_writeRouteset(const QString& strNeWFileToSaveRoutesetInto) {

    qDebug() << __FUNCTION__ << " strNeWFileToSaveRoutesetInto = " << strNeWFileToSaveRoutesetInto;

    _appFile_routeset.setFilename(strNeWFileToSaveRoutesetInto);
    bool bReport = routesetEditionFunction_writeRouteset();
    return(bReport);
}

//this method is called typically when loading a route set
bool ApplicationModel::routesetEditionFunction_loadRouteset(const QString& strFileToLoadRouteset) {

    bool bReport = routesetEditionFunction_loadRouteset(strFileToLoadRouteset, _ptrGeoRefModel_imagesSet);

    if (!bReport) {
        return(false);
    }

    qDebug() << __FUNCTION__ << "#1:";
    _ptrGeoRefModel_imagesSet->showContent();

    return(true);
}

//this method is called directly (giving a different than _ptrGeoRefModel_imagesSet)  when loading a routeset for a new projet
bool ApplicationModel::routesetEditionFunction_loadRouteset(const QString& strFileToLoadRouteset, GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet) {

    if (!ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: ptrGeoRefModel_imagesSet is nullptr";
        return(false);
    }
    if (ptrGeoRefModel_imagesSet->imageCount()) {
        qDebug() << __FUNCTION__ << " error: ptrGeoRefModel_imagesSet imageCount is not zero";
        return(false);
    }

    _sbaStrErrorMessage_aboutLoad = {};

    if (_appFile_routeset.filenameSet()) {
        qDebug() << __FUNCTION__ << " error: _appFile_routeset.inUse()";
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "Dev error #1";
        return(false);
    }

    _appFile_routeset.setFilename(strFileToLoadRouteset);

    QFile inputFile(strFileToLoadRouteset);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error: (read open) Can not open file " << strFileToLoadRouteset;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "Can not open file";
        _sbaStrErrorMessage_aboutLoad._bFileDoesnotExistAtLocation = true;
        return(false);
    }
    QByteArray qbArrayData = inputFile.readAll();
    if (qbArrayData.isEmpty()) {
        qDebug() << "error: readAll() got no data (";
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "file is empty";
        return(false);
    }
    inputFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(qbArrayData); //from text
    if (jsonDoc.isNull()) {
        qDebug() << __FUNCTION__ <<  ": error: failed to create jsonDoc" ;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json; error#1";
        return(false);
    }
    if (!jsonDoc.isObject()) {
        qDebug() << __FUNCTION__ <<  ": error: jsonDoc is not an object";
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json; error#2";
        return(false);
    }

    QJsonObject qjsonObjRouteset;
    qjsonObjRouteset = jsonDoc.object();

    qDebug() << __FUNCTION__ <<  "qjsonObjRouteset = " << qjsonObjRouteset;

    //check content tag
    QString qstr_contentType_routeset[] { "contentType", "routeset"};
    if (!qjsonObjRouteset.contains(qstr_contentType_routeset[0])) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_contentType_routeset[0];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_contentType_routeset[0];
        return(false);
    }
    if (qjsonObjRouteset.value(qstr_contentType_routeset[0]) != qstr_contentType_routeset[1]) {
        qDebug() << __FUNCTION__ <<  ": error: invalid value for key " << qstr_contentType_routeset[0];
        qDebug() << __FUNCTION__ <<  "         It has to be: " << qstr_contentType_routeset[1];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "invalid value for key: " + qstr_contentType_routeset[0];
        return(false);
    }

    //check formatVersion
    QString qstr_versionFormat_value[] { "versionFormat", "0.5"};
    if (!qjsonObjRouteset.contains(qstr_versionFormat_value[0])) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_versionFormat_value[0];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_versionFormat_value[0];
        return(false);
    }
    if (qjsonObjRouteset.value(qstr_versionFormat_value[0]) != qstr_versionFormat_value[1]) {
        qDebug() << __FUNCTION__ <<  ": error: invalid value for key " << qstr_versionFormat_value[0];
        qDebug() << __FUNCTION__ <<  "         It has to be: " << qstr_versionFormat_value[1];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "invalid value for key: " + qstr_versionFormat_value[0];
        return(false);
    }


    bool bEPSGCodeMustBeAValidNumericValue = true;//true by default
    QString qstrKey_useGeoRef { "bUseGeoRef"};
    if (!qjsonObjRouteset.contains(qstrKey_useGeoRef)) {
        //do not contains: => georef used
        //
        //it's valid to do not have useGeoRef for georeferenced trace (must contains backgroundImage_geoRefInfos)
        //Format 0.5 was used before the introduction of this key-value pair useGeoRef.
        //and the introduction of useGeoRef don't lead to version format change.
        //
     } else {
        //contains but failed to load:
        bool bGot = getBoolValueFromJson(qjsonObjRouteset, qstrKey_useGeoRef, _bUseGeoRefForTrace);
        if (!bGot) {
            qDebug() << __FUNCTION__ <<  ": error getting value for key: " + qstrKey_useGeoRef;
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "error getting value for key: " + qstrKey_useGeoRef;
            return(false);
        }
        bEPSGCodeMustBeAValidNumericValue = _bUseGeoRefForTrace;
    }

    GeoRefModel_imagesSet geoRefModel_fromLoadedRouteSet;
    //if (bMustContainsGeoRefInfos) {

    //geo-referencement infos about background image:
    QString qstr_backgroundImage_geoRefInfos { "backgroundImage_geoRefInfos" };
    if (!qjsonObjRouteset.contains(qstr_backgroundImage_geoRefInfos)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_backgroundImage_geoRefInfos;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_backgroundImage_geoRefInfos;
        return(false);
    }

    QJsonObject qjsonObj_backgroundImage_geoRefInfos  =  qjsonObjRouteset.value(qstr_backgroundImage_geoRefInfos).toObject();

    qDebug() << __FUNCTION__ << "qjsonObj_backgroundImage_geoRefInfos = " << qjsonObj_backgroundImage_geoRefInfos;

    //##LP behave differently if loading routeset or loading only one route
    //     store locally for now; it will be use to relocated the route to the used GeoRefModel_imagesSet context
    //GeoRefModel_imagesSet geoRefModel_fromLoadedRouteSet;
    bool bReportFromJson = geoRefModel_fromLoadedRouteSet.fromQJsonObject(qjsonObj_backgroundImage_geoRefInfos);
    if (!bReportFromJson) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject " + qstr_backgroundImage_geoRefInfos;
        return(false);
    }

    if (bEPSGCodeMustBeAValidNumericValue) {
        //---------
        //check here that the EPSG code is known (ie knonw by proj.db file)
        QString qstrEPSGCode_fromLoadedRouteSet = geoRefModel_fromLoadedRouteSet.get_qstrEPSGCode();
        qstrEPSGCode_fromLoadedRouteSet = qstrEPSGCode_fromLoadedRouteSet.remove("EPSG:");
        bool bReportEPSGCodeIsKnown_geoCRSSelect_inheritQATableModel =
                _geoCRSSelect_inheritQATableModel.checkStrEPSGCodeisKnown(qstrEPSGCode_fromLoadedRouteSet);
        if (!bReportEPSGCodeIsKnown_geoCRSSelect_inheritQATableModel) {
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson =
                    "EPSG code "
                    + qstrEPSGCode_fromLoadedRouteSet
                    + " is unknown by PROJ database";
            return(false);
        }
        geoRefModel_fromLoadedRouteSet.set_bUseGeoRef(true);
        //---------
    } else {
        geoRefModel_fromLoadedRouteSet.forceEPSGCode("EPSG:notSet");
        geoRefModel_fromLoadedRouteSet.set_bUseGeoRef(false);
    }

    //}

    setVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {true, false, false});

    QString qstr_routeset { "routeset" };
    if (!qjsonObjRouteset.contains(qstr_routeset)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_routeset;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_routeset;
        return(false);
    }
    QJsonArray qjsonArray_routeset = qjsonObjRouteset.value(qstr_routeset).toArray();
    qDebug() << __FUNCTION__ << "qjsonValue_routeset = " << qjsonArray_routeset;

    //routeset:
    bool bReport_toQJsonArray = _routeContainer.fromQJsonArray(qjsonArray_routeset);
    if (!bReport_toQJsonArray) {
        setState_editingRoute_closeState_clearRouteset(); //clean route load if partial
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject " + qstr_routeset;
        return(false);
    }

    //here there is:
    //. _routeContainer loaded from a routeset
    //. a GeoRefModel_imagesSet is ready to be used
    //
    //note: the image file from the routeset (.jtraces file) was not checked if present at the path
    bool bSetReport = ptrGeoRefModel_imagesSet->setDataFromAnotherGeoRefModelImagesSet(geoRefModel_fromLoadedRouteSet);
    if (!bSetReport) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "setData to GeoRefModel_imagesSet from loaded json said invalid";
        return(false);
    }


    /*qDebug() << __FUNCTION__ << "( eimagesSetStatus_geoTFWData is " << eimagesSetStatus_geoTFWData;

    }*/
    //ptrGeoRefModel_imagesSet->showContent();

    return(true);
}



//called when creating a new routeset
//set the imageFilename and try to load the tfw file, and the EPSG code from cache
S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_set_GeoRefModelImagesSet_fromBackgroundImageFilename(const QString& qstrImageFilename) {

    qDebug() << __FUNCTION__ << "qstrImageFilename = " << qstrImageFilename;

    _segeoRefImageSetStatus_routeSetEdition = {};

    bool bSomeLoadedButNotUsable = false;
    bool bSomeLoaded = _ptrGeoRefModel_imagesSet->setAndLoad_imageWidthHeight_TFWDataIfAvailable({ qstrImageFilename}, bSomeLoadedButNotUsable);

    if (  (bSomeLoadedButNotUsable)
        ||(!bSomeLoaded)) {
        if (bSomeLoadedButNotUsable) {
            _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_loadedButNotUsable;
        } else {
            if (!bSomeLoaded) {
                _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_noneFound;
            }
        }
        routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(_segeoRefImageSetStatus_routeSetEdition,
                                                                       _sbaStrMsg_gRIS_routeSetEdition);
        return(_segeoRefImageSetStatus_routeSetEdition);
    }

    routesetEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus();

    //image change, try to load its EPSG code from the cache:
    S_e_geoRefImagesSetStatus segeoRefImagesSetStatus = routesetEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus();

    qDebug() << __FUNCTION__ << "segeoRefImagesSetStatus._eiSS_geoEPSG = " << segeoRefImagesSetStatus._eiSS_geoEPSG;

    return(segeoRefImagesSetStatus);
}

S_e_geoRefImagesSetStatus ApplicationModel::
routesetEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_withProvidedBackgroundImageFilename(const QString& qstrImageFilename) {

    qDebug() << __FUNCTION__ << "qstrImageFilename = " << qstrImageFilename;

    _segeoRefImageSetStatus_routeSetEdition = {};

    _ptrGeoRefModel_imagesSet->setDefaultValuesAsNoGeorefUsed(qstrImageFilename);

    return(_segeoRefImageSetStatus_routeSetEdition);

}


S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus() {

    _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = _ptrGeoRefModel_imagesSet->check_TFWDataSet();

    qDebug() << "_segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase =" << _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase;

    routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(_segeoRefImageSetStatus_routeSetEdition,
                                                       _sbaStrMsg_gRIS_routeSetEdition);

    return(_segeoRefImageSetStatus_routeSetEdition);
}


//setting EPSG code write the EPSG code in cache and reload it from cache
S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(const QString& strEPSGCode) {
    _ptrGeoRefModel_imagesSet->setEPSGCode_and_storeInCacheForAllImagesSet(strEPSGCode);
    return(routesetEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus());
}

//setting EPSG code write the EPSG code in cache and reload it from cache
S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_set_EPSGCodePartof_GeoRefModelImagesSet_fromStrEPSGCode(const QString& strEPSGCode) {
    _ptrGeoRefModel_imagesSet->setEPSGCode_and_storeInCacheForAllImagesSet(strEPSGCode);
    return(projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus());
}

QString ApplicationModel::routesetEditionFunction_strDetailsAboutFail_routesetWrite() const {
    return(_strDetailsAboutFail_routesetWrite);
}


//valid for routeset and project:
void ApplicationModel::routesetEditionFunction_sendStr_geoRefImagesSetStatus() {

    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    _sbaStrMsg_gRIS_routeSetEdition.showContent();

    emit signal_sendStr_geoRefImagesSetStatus_aboutRoutesetEdition(
        _sbaStrMsg_gRIS_routeSetEdition._bWorldFileData_available,
        _sbaStrMsg_gRIS_routeSetEdition._bEPSG_available,

        _sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData,
        _sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData_errorDetails,

        _sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG,
        _sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG_errorDetails);
}


bool ApplicationModel::routesetEditionFunction_routesetContentIsCompatibleWithWriteOperation(QString &strDetailsAboutNotCompatible) const {
    return(_routeContainer.allRoutesHaveSegment(strDetailsAboutNotCompatible));
}

bool ApplicationModel::projectEditionFunction_projectContentIsCompatibleWithWriteOperation(QString &strDetailsAboutNotCompatible) const {
    return(_routeContainer.allRoutesHaveSegment(strDetailsAboutNotCompatible));
}

//@#LP close code (diff: _segeoRefImageSetStatus_routeSetEdition usage) with projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus
S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus() {

    e_imagesSetStatus_geoEPSG imagesSetStatus_geoEPSG = _ptrGeoRefModel_imagesSet->searchInCache_associatedEPSGCode();

    qDebug() << __FUNCTION__ << ":imagesSetStatus_geoEPSG: " << imagesSetStatus_geoEPSG;

    QString strEPSGCode_fromCache;

    if (imagesSetStatus_geoEPSG == e_iSS_geoEPSG_available) {
        strEPSGCode_fromCache = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
        qDebug() << __FUNCTION__ << ": unique EPSGCode found in cache: " << strEPSGCode_fromCache;
    } else {
        qDebug() << __FUNCTION__ << ": no unique EPSGCode found in cache (for some reason = " << imagesSetStatus_geoEPSG;
    }
    _segeoRefImageSetStatus_routeSetEdition._eiSS_geoEPSG = imagesSetStatus_geoEPSG;

    routesetEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
                _segeoRefImageSetStatus_routeSetEdition, _sbaStrMsg_gRIS_routeSetEdition);


     qDebug() << __FUNCTION__ << "_segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase =" << _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase;
     qDebug() << __FUNCTION__ << "_segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases =" << _segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases;
     qDebug() << __FUNCTION__ << "_segeoRefImageSetStatus_routeSetEdition._eiSS_geoEPSG =" << _segeoRefImageSetStatus_routeSetEdition._eiSS_geoEPSG;

    return(_segeoRefImageSetStatus_routeSetEdition);
}

S_e_geoRefImagesSetStatus ApplicationModel::routesetOpenFunction_checkThatGeoDataOfInputImageFileMatchesWithGeoDataLoadedFromJson(
        const QString& strInputImageFileForComparisonWithGeoDataLoadedFromJson,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus &sbASMsgStatusA_gRISS_aboutRouteSetEdition) {

    //check that the tfw for the selected equivalent background image is present and equal to the loaded world file data from the .jrouteset
    sbASMsgStatusA_gRISS_aboutRouteSetEdition = {};
    S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus_TFW =
            routesetEditionFunction_compareCurrentWorldFileData_withDataFromFile(strInputImageFileForComparisonWithGeoDataLoadedFromJson,
                                                                                 sbASMsgStatusA_gRISS_aboutRouteSetEdition);

    if (se_geoRefImagesSetStatus_TFW._eiSSgTFW_reducedToBigCases != e_iSS_geoTFWData_rTBC_available) {
        return(se_geoRefImagesSetStatus_TFW);
    }

    //world file data are equal to the world file data from the loaded .jrouteset

    //check that EPSG is same
    //if EPSG code the new background exist and is different => consider as en error
    //if EPSG code the new background does not exist => propose to the user to store this code for the new background
    //

    S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus_EPSG = routesetEditionFunction_compareCurrentEPSGCode_withEPSGCodeFromFile(
                    strInputImageFileForComparisonWithGeoDataLoadedFromJson,
                    sbASMsgStatusA_gRISS_aboutRouteSetEdition);

    se_geoRefImagesSetStatus_EPSG._eiSSgTFW_allStatusCase     = se_geoRefImagesSetStatus_TFW._eiSSgTFW_allStatusCase;
    se_geoRefImagesSetStatus_EPSG._eiSSgTFW_reducedToBigCases = se_geoRefImagesSetStatus_TFW._eiSSgTFW_reducedToBigCases;

    return(se_geoRefImagesSetStatus_EPSG);
}

//use the filename from the loadedjson routeset
bool ApplicationModel::routesetEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav() {

    _sbaStrErrorMessage_aboutLoad = {};

    if (!_ptrGeoRefModel_imagesSet) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "Dev error #4";
        return(false);
    }

    auto qvectOfStrImagePathAndFilenameSet = _ptrGeoRefModel_imagesSet->get_qvectOfStrImagePathAndFilenameSet();
    if (qvectOfStrImagePathAndFilenameSet.isEmpty()) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "Dev error #5";
        return(false);
    }

    bool bLoaded = loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                qvectOfStrImagePathAndFilenameSet[0], 0);

    //@LP: routesetEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(...) feed _sbaStrErrorMessage_aboutLoad

    return(bLoaded);
}

//return false in case of fatal error
bool ApplicationModel::routesetOpenFunction_getBackgroundImageFilenameGotFromJsonLoad(QString& strBackgroundImageFilename, bool& bExists) {

    bExists = false;
    if (!_ptrGeoRefModel_imagesSet) {
        return(false);
    }

    auto qvectOfStrImagePathAndFilenameSet = _ptrGeoRefModel_imagesSet->get_qvectOfStrImagePathAndFilenameSet();
    if (qvectOfStrImagePathAndFilenameSet.isEmpty()) {
        return(false);
    }

    strBackgroundImageFilename = qvectOfStrImagePathAndFilenameSet[0];
    return(routesetOpenFunction_checkThatInputImageFileExists(strBackgroundImageFilename, bExists));
}

bool ApplicationModel::routesetOpenFunction_checkThatInputImageFileExists(const QString& strFilename, bool& bExist) {
    QString qstrFileFollowingSymbolicLink = getPathAndFilenameFromPotentialSymlink(strFilename);
    bExist = fileExists(qstrFileFollowingSymbolicLink);
    return(true);
}


bool ApplicationModel::routesetOpenFunction_checkThatInputImageFileExists(const QString& strInputImageFileToCheckExistence) {

    _sbaStrErrorMessage_aboutLoad = {};

    if (!_ptrGeoRefModel_imagesSet) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "Dev error #4";
        return(false);
    }

    auto qvectOfStrImagePathAndFilenameSet = _ptrGeoRefModel_imagesSet->get_qvectOfStrImagePathAndFilenameSet();
    if (qvectOfStrImagePathAndFilenameSet.isEmpty()) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "Dev error #5";
        return(false);
    }

    bool bLoaded = loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
                qvectOfStrImagePathAndFilenameSet[0], 0);

    //@LP: routesetEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(...) feed _sbaStrErrorMessage_aboutLoad

    return(bLoaded);
}



S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_compareCurrentWorldFileData_withDataFromFile(
        const QString& strImageBackgroundReplacement,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sboolAndStrMsgStatusAbout_geoRefImageSetStatus) const {

    GeoRefModel_imagesSet geoRefModel_fromImageBackgroundReplacement;
    S_e_geoRefImagesSetStatus segeoRefImageSetStatus_routeSetEdition;

    bool bSomeLoadedButNotUsable = false;
    bool bSomeLoaded = geoRefModel_fromImageBackgroundReplacement.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strImageBackgroundReplacement}, bSomeLoadedButNotUsable);

    if (  (bSomeLoadedButNotUsable)
        ||(!bSomeLoaded)) {
        if (bSomeLoadedButNotUsable) {
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_loadedButNotUsable;
        } else {
            if (!bSomeLoaded) {
                segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_noneFound;
            }
        }
        routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImageSetStatus_routeSetEdition,
                                                                       sboolAndStrMsgStatusAbout_geoRefImageSetStatus);
        return(segeoRefImageSetStatus_routeSetEdition);
    }

    segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = geoRefModel_fromImageBackgroundReplacement.check_TFWDataSet();

    routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImageSetStatus_routeSetEdition,
                                                       sboolAndStrMsgStatusAbout_geoRefImageSetStatus);

    if (!_ptrGeoRefModel_imagesSet) {
        segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
        segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unknowError;
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._bWorldFileData_available = false;
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutWorldFileData_errorDetails = "Dev error #2";
        return(segeoRefImageSetStatus_routeSetEdition);
    }

    //check that world file data are equal
    if (segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases ==e_iSS_geoTFWData_rTBC_available) {

        //comparison between GeoRefModel_imagesSet considerd that each of them are consitent by themselves (no  GeoRefModel_imagesSet with unsync data between ImageGeoRef)
        bool bSameWorldFileDAta = _ptrGeoRefModel_imagesSet->compareWorldFileDataWith(geoRefModel_fromImageBackgroundReplacement);

        if (!bSameWorldFileDAta) {
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unsyncTFWData;
            sboolAndStrMsgStatusAbout_geoRefImageSetStatus._bWorldFileData_available = false;
            sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutWorldFileData_errorDetails = "The world file Data of the selected image is different";
            return(segeoRefImageSetStatus_routeSetEdition);
        }
    }

    return(segeoRefImageSetStatus_routeSetEdition);
}


S_e_geoRefImagesSetStatus ApplicationModel::routesetEditionFunction_compareCurrentEPSGCode_withEPSGCodeFromFile(
        const QString& strImageBackgroundReplacement,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sboolAndStrMsgStatusAbout_geoRefImageSetStatus) const {

    S_e_geoRefImagesSetStatus se_geoRefImagesSetStatus;

    if (!_ptrGeoRefModel_imagesSet) {
        se_geoRefImagesSetStatus._eiSS_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._bEPSG_available = false;
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutEPSG_errorDetails = "Dev error #3";
        return(se_geoRefImagesSetStatus);
    }

    GeoRefModel_imagesSet geoRefModel_fromImageBackgroundReplacement;

    //cache storage subdir for epsg code not used now, store in the same dir that the image
    /*geoRefModel_fromImageBackgroundReplacement.setRootForEPSGCacheStorage(_qstrRootForEPSGCacheStorage);
    qDebug() << __FUNCTION__ << ": _qstrRootForEPSGCacheStorage =" << _qstrRootForEPSGCacheStorage;*/

    QString strFoundEPSGCode_forImageBackgroundReplacementCandidate;
    se_geoRefImagesSetStatus._eiSS_geoEPSG = geoRefModel_fromImageBackgroundReplacement.setToOneImage_and_getEPSGCodeFromCacheIfAvailable(
               strImageBackgroundReplacement,
               strFoundEPSGCode_forImageBackgroundReplacementCandidate);

    qDebug() << __FUNCTION__ << ": strFoundEPSGCode_forImageBackgroundReplacementCandidate =" << strFoundEPSGCode_forImageBackgroundReplacementCandidate;

    routesetEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
            se_geoRefImagesSetStatus,
            sboolAndStrMsgStatusAbout_geoRefImageSetStatus);

    if (se_geoRefImagesSetStatus._eiSS_geoEPSG == e_iSS_geoEPSG_available) {

        int  intDiffEPSG = strFoundEPSGCode_forImageBackgroundReplacementCandidate.compare(_ptrGeoRefModel_imagesSet->get_qstrEPSGCode());

        qDebug() << __FUNCTION__ << ": compare: [" << strFoundEPSGCode_forImageBackgroundReplacementCandidate << "] and [" << _ptrGeoRefModel_imagesSet->get_qstrEPSGCode() << "]";

        if (intDiffEPSG) {
            se_geoRefImagesSetStatus._eiSS_geoEPSG = e_iSS_geoEPSG_notAvailable_unsyncEPSGData;
            sboolAndStrMsgStatusAbout_geoRefImageSetStatus._bEPSG_available = false;            
            sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutEPSG_errorDetails = "The EPSG code of the selected image is different";
        }
    }
    if (se_geoRefImagesSetStatus._eiSS_geoEPSG == e_iSS_geoEPSG_notAvailable_noneFound) {
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutEPSG_errorDetails = "EPSG code of the selected image not found";
        sboolAndStrMsgStatusAbout_geoRefImageSetStatus._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
    }

    return(se_geoRefImagesSetStatus);
}


bool ApplicationModel::routesetEditionFunction_storeInCacheTheCurrentEPSGCode_forImage(const QString& strImage) {

    qDebug() << __FUNCTION__;

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: _ptrGeoRefModel_imagesSet is nullptr";
        return(false);
    }

    int imageCount_inGeoRefModel_imagesSet = _ptrGeoRefModel_imagesSet->imageCount();
    if (!imageCount_inGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: no image in rGeoRefModel_imagesSet";
        return(false);
   }

    return(_ptrGeoRefModel_imagesSet->storeInCache_EPSGCode_forImage(strImage));
}


bool ApplicationModel::routesetEditionFunction_replaceBackgroundImage(const QString& strImageBackgroundReplacement) {

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: _ptrGeoRefModel_imagesSet is nullptr";
        return(false);
    }

    int imageCount_inGeoRefModel_imagesSet = _ptrGeoRefModel_imagesSet->imageCount();
    if (!imageCount_inGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: no image in rGeoRefModel_imagesSet";
        return(false);
    }

    bool bReplaced = _ptrGeoRefModel_imagesSet->replace_soleImageFilename(strImageBackgroundReplacement);

    if (!_appFile_routeset.modified()) {
        _appFile_routeset.setState_modified(bReplaced); //the image file was replaced
    }

    return(true);
}

void ApplicationModel::routesetEditionFunction_initState_andFeedGraphicsItemContainer() {

    _graphicsItemContainer.initMinimalElementSizeSide_fromBackgroundSizeAndPercentFactor(_qszInputImageWidthHeight, 1.0/32.0);

    _graphicsItemContainer.initFromAndConnectTo_RouteContainer(&_routeContainer);
    _graphicsItemContainer.addAllRouteItem_and_TinyTargetPtNotDragable_toScene();
    _graphicsItemContainer.adjustLocationAndSizeToZLI_AllRouteItem();

    _selectedRouteId = -1;
    _eAppModelState = eAModS_editingRouteSet_editingRoute;
}
