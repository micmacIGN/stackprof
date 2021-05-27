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

#include "../toolbox/toolbox_math_geometry.h"

#include "../toolbox/toolbox_json.h"

#include "mainAppState.hpp"

#include <QJsonDocument>
#include <QJsonObject>

#include "core/InputFilesCheckerForComputation.h"

#include "georef/geoRefImagesSetStatus.hpp"

#include "pathesAccess/InternalDataPathAccess.h"

#include "../toolbox/toolbox_math.h"
#include "../toolbox/toolbox_conversion.h"

#include <QDateTime>

#include "core/geoConvertRoute.h"

#include "core/exportResult.h"

#include "appVersionInfos/appVersionInfos.hpp"

S_boolAndStrMsgStatusAbout_geoRefImageSetStatus ApplicationModel::projectEditionFunction_get_sboolAndStrMsgStatusAbout_geoRefImageSetStatus() {
    return(_sbaStrMsg_gRIS_projectEdition);
}

bool ApplicationModel::projectEditionFunction_writeProject() { //use the current file

    _strDetailsAboutFail_projectWrite.clear();

    if (!_appFile_project.filenameSet()/*inUse*/) {
        qDebug() << __FUNCTION__ << " error: !_appFile_project.inUse()";
        _strDetailsAboutFail_projectWrite = "Dev Error #6 no filename used";
        return(false);
    }

    //write infos about the georef (no images filename)
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        qDebug() << __FUNCTION__ << "        Can not save without geoRef data";
        _strDetailsAboutFail_projectWrite = "Dev Error #7 GeoRefModel_imagesSet is nullptr";
        return(false);
    }

    QJsonObject QJsonObject_projectFileContent;
    QJsonObject_projectFileContent.insert("contentType", "project");

    QJsonObject_projectFileContent.insert("versionFormat", "0.8");

    //inserted here instead of in each route, to avoid useless repeated description
    QJsonObject_projectFileContent.insert("_descAbout_routePoint",
                                          "Points coordinates are [x, y] in pixel image coordinates. The top-let corner of top-left image pixel is [0.0, 0.0]. idPoint increases by 1 from first point to last point");
                                          //"idPoint: [x, y] coordinates from the image pixel top-let corner of top-left pixel = (0.0, 0.0). idPoint increase by 1 from first point to last point; first idPoint must be 0");

    /*QJsonObject::const_iterator qjsonobjCstIter = QJsonObject_aboutRouteSetFile.constBegin();
    for(; qjsonobjCstIter != QJsonObject_aboutRouteSetFile.constEnd(); ++qjsonobjCstIter) {
        qDebug() << __FUNCTION__ << "_qjsonObjInfos iter [key]= value:  [" << qjsonobjCstIter.key() << "] = " << qjsonobjCstIter.value();
    }*/

    //geo-referencement infos about project:
    QJsonObject qjsonObj_geoRefInfos;
    bool bReport_toQJsonObject = _ptrGeoRefModel_imagesSet->toQJsonObject(qjsonObj_geoRefInfos);
    if (!bReport_toQJsonObject) {
        qDebug() << __FUNCTION__ << " error: _ptrGeoRefModel_imagesSet->toQJsonObject failed";
        _strDetailsAboutFail_ProjectWrite = "Dev Error #8_1 GeoRefModel_imagesSet toQJsonObject() error";
        return(false);
    }
    QJsonObject_projectFileContent.insert("imagesSet_geoRefInfos", qjsonObj_geoRefInfos);
    QJsonObject_projectFileContent.insert("_noteAbout_imagesSet_geoRefInfos", "The image path and filename stored in imagesSet_geoRefInfos is not used at loading");

    QJsonObject_projectFileContent.insert("bUseGeoRef", _bUseGeoRefForProject);

    //qDebug() << "QJsonObject_routeSetFileContent: " << QJsonObject_routeSetFileContent;

    //inputFiles
    QJsonObject qjsonObj_inputFiles;

    //writing a project considers inputFiles valid and routeContainer with at least one route (should be only one)
    //write input files from the AppModel, the computationCore could not be feed with it

    S_InputFilesWithImageFilesAttributes sInputFilesWithImageFilesAttributes;
    bool bAttrGotFromFile = sInputFilesWithImageFilesAttributes.fromInputFiles(_inputFiles);
    if (!bAttrGotFromFile) {
        qDebug() << __FUNCTION__ << " error: failed to got attributes from an input file";
        _strDetailsAboutFail_ProjectWrite = "error: failed to got attributes from an input file";
        return(false);
    }
    //sInputFilesWithImageFilesAttributes.showContent(); //@#LP debug

    sInputFilesWithImageFilesAttributes.toQJsonObject(qjsonObj_inputFiles); //@#LP could requiered eMainComputationMode to know how to save correlation map usage (PX1PX2 together)

    QString strKey_inputFiles { "inputFiles" };
    QJsonObject_projectFileContent.insert(strKey_inputFiles, qjsonObj_inputFiles);

    //route; this has to be a routeset with only one route
    if (_routeContainer.routeCount() > 1) {
        qDebug() << __FUNCTION__ << " error: _routeContainer routeCount > 1";
        _strDetailsAboutFail_ProjectWrite = "route count > 1"; //if the user edit/hack the project file, this is not a 'Dev Error'
        return(false);
    }

    if (_projectContentState.getState_route() != e_Project_routeContentState::e_P_routeCS_oneRouteValid) {
        qDebug() << __FUNCTION__ << " error: route is in a invalid state. Can not save project.";
        _strDetailsAboutFail_ProjectWrite = "Route is an invalid state";
        return(false);
    }

    QJsonArray qjsonArray_routeset;
    bool bReport_toQJsonArray = _routeContainer.toQJsonArray(qjsonArray_routeset, _strDetailsAboutFail_projectWrite);
    if (!bReport_toQJsonArray) {
        qDebug() << __FUNCTION__ << " error: _routeContainer.toQJsonArray failed";
        _strDetailsAboutFail_ProjectWrite = "routeContainer to json array failed"; //should never happen
        return(false);
    }
    QJsonObject_projectFileContent.insert("route", qjsonArray_routeset);
    //@#LP stay on 'routeset tag for easiest copy/past if a user want to edit the project file by hand and paste a route from a .jtraces ?

    //@#LP namming orientationFault vs useRouteInReversePointOrder

    //micmac step value and spatial resolution
    QJsonObject qjsonObj_micmacStepValues_and_spatialResolutions;
    QString _tqstrLayerJSonDescName[3] {"PX1", "PX2", "ZOther"};
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        QJsonObject qjsonObj_micmacSV_and_sR_byLayer;
        if (!_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela].isEmpty()) { //@#LP add and replace with a QVector<bool> layer_to_compute ?
            bool bQjsonObj_micmacSV_and_sR_byLayerGot = _vectDequantization_PX1_PX2_deltaZ[iela].toQJsonObject(qjsonObj_micmacSV_and_sR_byLayer);
            if (!bQjsonObj_micmacSV_and_sR_byLayerGot) {
                qDebug() << __FUNCTION__ << " error: stackedProfiBoxes_toQJsonArray_ failed";
                _strDetailsAboutFail_projectWrite = "invalid micmacStep or spatial Resolution for " + _tqstrLayerJSonDescName[iela];
                return(false);
            }
            qjsonObj_micmacStepValues_and_spatialResolutions.insert(_tqstrLayerJSonDescName[iela], qjsonObj_micmacSV_and_sR_byLayer);
        }
    }
    QJsonObject_projectFileContent.insert("pixelValueResolution", qjsonObj_micmacStepValues_and_spatialResolutions);

    QJsonObject qjsonObj_computationMainMode;
    QString str_computationMainMode_key {"computationMainMode"};

    QJsonObject_projectFileContent.insert(str_computationMainMode_key, static_cast<int>(_computationCore_iqaTableModel.getMainComputationMode()));

    bool bComputationParametersAvailable =
            (_projectContentState.getState_computationParameters() != e_Project_computationParametersState::e_P_computationParametersS_notSet);
    if (bComputationParametersAvailable) {
        QJsonObject qjsonObj_computationSettings;
        _computationCore_iqaTableModel.computationParameters_toQJsonObject(qjsonObj_computationSettings);
        QString strKey_computationSettings { "computationParameters" };
        QJsonObject_projectFileContent.insert(strKey_computationSettings, qjsonObj_computationSettings);
    }

    bool bOneOrMoreBoxes = (_projectContentState.getState_boxes() == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
    if (bOneOrMoreBoxes) {
        //write stacked profil boxes
        QJsonArray qjsonArray_stackedProfilBoxes;
        bool bReport_toQJsonArray = _computationCore_iqaTableModel.stackedProfilBoxes_toQJsonArray_forProjectWrite(qjsonArray_stackedProfilBoxes, _strDetailsAboutFail_projectWrite);
        if (!bReport_toQJsonArray) {
            qDebug() << __FUNCTION__ << " error: stackedProfiBoxes_toQJsonArray_ failed";
            return(false);
        }
        QString strKey_stackedProfilBoxes { "stackedProfilBoxes" };
        QJsonObject_projectFileContent.insert(strKey_stackedProfilBoxes, qjsonArray_stackedProfilBoxes);
    }

    //write stacked profil parameters
    if (  (bOneOrMoreBoxes)
        ) {

        QJsonArray qjsonArray_stackedProfil;
        bool bReport_toQJsonArray = _computationCore_iqaTableModel.
                stackedProfilWithMeasurements_withoutLinearRegresionModelResult_toQJsonArray(
                    qjsonArray_stackedProfil, _strDetailsAboutFail_projectWrite);

        if (!bReport_toQJsonArray) {
            qDebug() << __FUNCTION__ << " error: stackedProfil_toQJsonArray_ failed";
            return(false);
        }
        QString strKey_stackedProfil { "stackedProfil" };
        QJsonObject_projectFileContent.insert(strKey_stackedProfil, qjsonArray_stackedProfil);
    }

    QJsonDocument JsonDoc(QJsonObject_projectFileContent);

    QString strOutFilename = _appFile_project.getFilename();

    QFile outFile(strOutFilename);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "error: (write open) Can not open file " << strOutFilename;
        return(false);
    }
    QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
    qint64 wroteBytes = outFile.write(qbArrayData);
    if (wroteBytes <= 0) {
         qDebug() << "error: writing byte to file " << strOutFilename;
         return(false);
    }

    _appFile_project.setState_modified(false);
    return(true);

}

bool ApplicationModel::projectEditionFunction_writeProject(const QString& strNeWFileToSaveProjectInto) { //will set on strNeWFileToSaveProjectInto and save on it
    _appFile_project.setFilename/*_inUse*/(strNeWFileToSaveProjectInto);
    bool bReport = projectEditionFunction_writeProject();
    return(bReport);
}

bool ApplicationModel::projectEditionFunction_setAndLoad_routesetFileForRouteImport(const QString& strRoutesetFilename) {

    qDebug() << __FUNCTION__ ;

    _sbaStrErrorMessage_aboutLoad = {};

    _strRoutesetFilenameForRouteImport = strRoutesetFilename;
    _selectedRouteId_forRouteImport = -1;

    _bStateAddADefaultRoute = false;

    bool bRoutesetFileLoaded = projectEditionFunction_loadRouteset(_strRoutesetFilenameForRouteImport);
    return(bRoutesetFileLoaded);
}

void ApplicationModel::projectEditionFunction_setStateAddADefaultRoute(bool bState) {
    qDebug() << __FUNCTION__ ;
    _bStateAddADefaultRoute = bState;
}

bool ApplicationModel::projectEditionFunction_getStateAddADefaultRoute() const {
    qDebug() << __FUNCTION__ ;
    return(_bStateAddADefaultRoute);
}

bool ApplicationModel::projectEditionFunction_loadRouteset(const QString& strFileToLoadRouteset) {

    qDebug() << __FUNCTION__;

    _forRouteImport_geoRefModelImgSetAboutRouteset.clear();

    //the routesetEditionFunction_loadRouteset will load the geo infos into _forRouteImport_geoRefModelImgSetAboutRouteset
    bool bRouteSetLoaded = routesetEditionFunction_loadRouteset(strFileToLoadRouteset, &_forRouteImport_geoRefModelImgSetAboutRouteset);
    if (!bRouteSetLoaded) {
        qDebug() << __FUNCTION__ << " error loading routeset for route import; be sure to clean models";
        return(false);
    }
    _forRouteImport_geoRefModelImgSetAboutRouteset.showContent();

    //here the routes from the loaded routeset are into _routeContainer
    //and the geo infos about routeset are into _forRouteImport_geoRefModelImgSetAboutRouteset
    return(bRouteSetLoaded);
}

int ApplicationModel::projectEditionFunction_get_selectedRouteId_forRouteImport() {
    return(_selectedRouteId_forRouteImport);
}

void ApplicationModel::projectEditionFunction_setAsInvalid_selectedRouteFromRouteset() {
    _selectedRouteId_forRouteImport = -1;
}


void ApplicationModel::projectEditionFunction_selectedRouteFromRouteset(int selectedIdRoute) {

    _selectedRouteId_forRouteImport = selectedIdRoute;

    if (!_routeContainer.checkRouteIdExists(_selectedRouteId_forRouteImport)) {
        _selectedRouteId_forRouteImport = -1;
        emit signal_projectEditionFunction_sendIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport_toView(
                    _selectedRouteId_forRouteImport, _strRoutesetFilenameForRouteImport,
                    "none selected", false);

        qDebug() << __FUNCTION__ << " error: invalid routeId ( " << _selectedRouteId_forRouteImport << " )";
        return;
    }
    emit signal_projectEditionFunction_sendIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport_toView(
                _selectedRouteId_forRouteImport, _strRoutesetFilenameForRouteImport,
                _routeContainer.getRouteRef(_selectedRouteId_forRouteImport).getName(), true);

}

S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_set_GeoRefModelImagesSet_fromInputImageFilenames(const QVector<QString>& qvectStr_inputFiles) {

    _segeoRefImageSetStatus_projectEdition = {};

    bool bSomeLoadedButNotUsable = false;
    bool bSomeLoaded = _ptrGeoRefModel_imagesSet->setAndLoad_imageWidthHeight_TFWDataIfAvailable(qvectStr_inputFiles, bSomeLoadedButNotUsable);

    if (  (bSomeLoadedButNotUsable)
        ||(!bSomeLoaded)) {
        if (bSomeLoadedButNotUsable) {
            _segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_loadedButNotUsable;
        } else {
            if (!bSomeLoaded) {
                _segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_noneFound;
            }
        }
        projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(_segeoRefImageSetStatus_projectEdition,
                                                                      _sbaStrMsg_gRIS_projectEdition);
        return(_segeoRefImageSetStatus_projectEdition);
    }

    projectEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus();

    //try to load EPSG codes from the cache:
    S_e_geoRefImagesSetStatus segeoRefImagesSetStatus = projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus();
    return(segeoRefImagesSetStatus);
}


S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_setDefaultValuesAsNoGeorefUsed_GeoRefModelImagesSet_fromInputImageFilenames(const QVector<QString>& qvectStr_inputFiles) {

    _segeoRefImageSetStatus_projectEdition = {};

    _ptrGeoRefModel_imagesSet->setDefaultValuesAsNoGeorefUsed_fromInputImageFilenames(qvectStr_inputFiles);

    return(_segeoRefImageSetStatus_projectEdition);
}


S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_checkTFWData_andUpdateGeoRefImageSetStatus() {

    _segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = _ptrGeoRefModel_imagesSet->check_TFWDataSet();

    projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(_segeoRefImageSetStatus_projectEdition,
                                                       _sbaStrMsg_gRIS_projectEdition);
    return(_segeoRefImageSetStatus_projectEdition);
};


//@#LP close code (diff: _segeoRefImageSetStatus_projectEdition usage) with routesetEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus
S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_checkEPSGCode_andUpdateGeoRefImageSetStatus() {

    qDebug() << __FUNCTION__ << __LINE__;

    e_imagesSetStatus_geoEPSG imagesSetStatus_geoEPSG = _ptrGeoRefModel_imagesSet->searchInCache_associatedEPSGCode();

    QString strEPSGCode_fromCache;

    if (imagesSetStatus_geoEPSG == e_iSS_geoEPSG_available) {
        strEPSGCode_fromCache = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
        qDebug() << __FUNCTION__ << ": unique EPSGCode found in cache: " << strEPSGCode_fromCache;
    } else {
        qDebug() << __FUNCTION__ << ": no unique EPSGCode found in cache (for some reason = " << imagesSetStatus_geoEPSG;
    }
    _segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = imagesSetStatus_geoEPSG;

    projectEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
                _ptrGeoRefModel_imagesSet,
                _segeoRefImageSetStatus_projectEdition, _sbaStrMsg_gRIS_projectEdition);

    return(_segeoRefImageSetStatus_projectEdition);
}

bool ApplicationModel::projectEditionFunction_storeInCacheTheCurrentEPSGCode_forImageWhichDoNotHave()  {

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: _ptrGeoRefModel_imagesSet is nullptr";
        return(false);
    }

    int imageCount_inGeoRefModel_imagesSet = _ptrGeoRefModel_imagesSet->imageCount();
    if (!imageCount_inGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: no image in rGeoRefModel_imagesSet";
        return(false);
   }

   return(_ptrGeoRefModel_imagesSet->storeInCache_EPSGCode_forImagesWhichDoNotHave());
}

S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute ApplicationModel::get_sboolAndStrMsg_aboutGeoConvertAndImportRoute() {
    return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute);
}

//a big difference returning about it with _selectedRouteId_forRouteImport and _routeContainer status
bool ApplicationModel::projectEditionFunction_geoConvertSelectedRouteFromRouteset_andKeepAlone() {

    _sbaStrMsgStatus_aboutGeoConvertAndImportRoute = {};
    _sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage = "Trace importation";
    qDebug() << __FUNCTION__;

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: _ptrGeoRefModel_imagesSet is nullptr";
        _sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "Dev Error #11";
        return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
    }

    if (_bStateAddADefaultRoute) {

        QSizeF qsizeF_imageWH(.0,.0);
        bool bReport = _ptrGeoRefModel_imagesSet->getImageSizeF(qsizeF_imageWH);
        if (!bReport) {
            _sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "Dev Error #11_1 (_ptrGeoRefModel_imagesSet->getImageSizeF failed)";
            return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
        }

        _routeContainer.clear();
        _routeContainer.aboutAbstractTableModel_addNewRoute("");

        QPointF defaultStartPoint = {qsizeF_imageWH.width() * 0.25, qsizeF_imageWH.height() * 0.75};
        QPointF defaultEndPoint   = {qsizeF_imageWH.width() * 0.75, qsizeF_imageWH.height() * 0.25};

        bReport = true;
        bool bThisIsTheFirstPoint = false;
        bReport &= _routeContainer.tryAddPointAtRouteEnd(0, defaultStartPoint, bThisIsTheFirstPoint);
        bReport &= _routeContainer.tryAddPointAtRouteEnd(0, defaultEndPoint  , bThisIsTheFirstPoint);
        if (!bReport) {
            _sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "Dev Error #11_2";
            return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
        }
        _sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus = true;
        return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
    }

    /*bool bConversionStatus = */geoConvertSelectedRouteFromRouteset_andKeepAlone(
        _routeContainer,
        _selectedRouteId_forRouteImport,
        _ptrGeoRefModel_imagesSet,
        _forRouteImport_geoRefModelImgSetAboutRouteset,
        _sbaStrMsgStatus_aboutGeoConvertAndImportRoute);

    return(_sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);

}

/*
bool ApplicationModel::projectEditionFunction_noGeoRefUsedNoGeoConversion_SelectedRouteFromRouteset_andKeepAlone() {

}
*/



bool ApplicationModel::projectEditionFunction_checkInputFilesForComputationMatchWithRequierement(
        const S_InputFiles& inputFiles, e_mainComputationMode eMainComputationMode,
        QVector<bool> &qvectb_LayersForComputationFlag, int& nbLayersLayersForComputation,
        QVector<bool> &qvectb_correlScoreMapToUseForCompFlag, int& correlScoreMapToUseForComp,
        QString& strMsgErrorDetails) const {

    U_CorrelationScoreMapFileUseFlags correlationScoreUsageFlags;
    //because InputFilesForComputationMatchWithRequierement use the flags about correlationScoreUsageFlags to check the validity
    //of the correlation score map files; these flags needs to be set at true here to check the correlation score map files presence, format, size and others check
    correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2 =
            (!inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].isEmpty());

    correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ =
            (!inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].isEmpty());

    qDebug() << "correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2 = " << correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2;
    qDebug() << "correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ = " << correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ;


    InputFilesForComputationMatchWithRequierement inFilesForComputationMatchWithRequierement(inputFiles, eMainComputationMode, correlationScoreUsageFlags);
    bool bReport = inFilesForComputationMatchWithRequierement.check_PX1PX2Together_DeltazAlone(strMsgErrorDetails);

    //useful here to get and provide ?
    nbLayersLayersForComputation = inFilesForComputationMatchWithRequierement.getQVectorLayersForComputationFlag(qvectb_LayersForComputationFlag);

    correlScoreMapToUseForComp = inFilesForComputationMatchWithRequierement.getQVectorCorrelationScoreMapFilesAvailabilityFlag(qvectb_correlScoreMapToUseForCompFlag);

    return(bReport);
}

void ApplicationModel::projectEditionFunction_initState_andFeedGraphicsItemContainer(bool bIsANewProject) {

    qDebug() << __FUNCTION__ << " entering";

    _graphicsItemContainer.initMinimalElementSizeSide_fromBackgroundSizeAndPercentFactor(_qszInputImageWidthHeight, 1.0/32.0); //0.125);

    _graphicsItemContainer.initFromAndConnectTo_RouteContainer(&_routeContainer);
    _graphicsItemContainer.addAllRouteItem_and_TinyTargetPtNotDragable_toScene();

    _graphicsItemContainer.allocInitFromRouteAndAddHiddenToScene_locationOfSpecificGraphicsItemAboutRouteStartAndEnd();

    _graphicsItemContainer.adjustLocationAndSizeToZLI_AllRouteItem();

    const QMap<int, S_BoxAndStackedProfilWithMeasurements>& constRefMap_insertioBoxId_BASPWM =
            _computationCore_iqaTableModel.getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements();

    auto iter = constRefMap_insertioBoxId_BASPWM.begin();
    auto iterEnd = constRefMap_insertioBoxId_BASPWM.end();
    for (;iter != iterEnd;  iter++) {
        _graphicsItemContainer.addOneBox_from_profilBoxParameters(iter.key(), iter->_profilsBoxParameters);
    }

    _selectedRouteId = -1;
    _eAppModelState = eAModS_editingProject_idle;

    //for new project, we can have:
    // _ 0 route if no route imported
    // or
    // _ one present if imported
    //(when loaded from json: should be present (implementation design choice))
    if (bIsANewProject) {
        if (_routeContainer.routeCount() == 1) {
            if (_routeContainer.getRouteRef(0).segmentCount() >=1 ){
                _projectContentState.setState_route(e_Project_routeContentState::e_P_routeCS_oneRouteValid);
            } else { //should never occurs, route should be rejected at loading
                _projectContentState.setState_route(e_Project_routeContentState::e_P_routeCS_oneRouteButInvalid);
            }
        } else {
            _projectContentState.setState_route(e_Project_routeContentState::e_P_routeCS_noneRoute);
        }
    } else { //loaded from file
        //@LP setState_route set when loading the project file:
    }

    if (bIsANewProject) {
        _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_noneBox);
        _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable);
    } else {
        //@LP setState_stackedProfilEdited set when loading the project file
        //@LP setState_boxes set when loading the project file
    }

   _projectContentState.setState_computationParameters(e_Project_computationParametersState::e_P_computationParametersS_set); //default parameters set including in core computation at least
   _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_noneComputed);

   _projectContentState.setState_spatialResolution(e_Project_spatialResolutionState::e_P_stackedProfilEditedS_set);
}


bool ApplicationModel::projectEditionFunction_loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(
        const QVector<QString>& qvectStr_inputFile_PX1_PX2_DeltaZ,
        const QVector<bool>& qvectb_layersToDisplay,
        int idxLayerToDisplayAsBackgroundImage) {

    _sbaStrErrorMessage_aboutLoad = {};

    if (idxLayerToDisplayAsBackgroundImage == -1) {
        return(false);
    }

    QString qstrImageFilename = qvectStr_inputFile_PX1_PX2_DeltaZ[idxLayerToDisplayAsBackgroundImage];

    qDebug() << __FUNCTION__ << "qvectStr_inputFile_PX1_PX2_DeltaZ = " << qvectStr_inputFile_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << "qvectb_layersToDisplay = " << qvectb_layersToDisplay;
    qDebug() << __FUNCTION__ << "idxLayerToDisplayAsBackgroundImage = " << idxLayerToDisplayAsBackgroundImage;
    qDebug() << __FUNCTION__ << "qstrImageFilename = " << qstrImageFilename;

    S_ImageFileAttributes inputImageAttributes;
    if (inputImageAttributes.fromFile(qstrImageFilename)) {
        _qszInputImageWidthHeight = {
            static_cast<qreal>(inputImageAttributes._width),
            static_cast<qreal>(inputImageAttributes._height)};
    }

    return(loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(qstrImageFilename, idxLayerToDisplayAsBackgroundImage));

}


QString ApplicationModel::projectEditionFunction_strDetailsAboutFail_projectWrite() const {
    return(_strDetailsAboutFail_projectWrite);
}


// The project loading from json file needs to do extra check about content and
// values. Extra check which should not be useful when the file project was written by the app.
// The reason is that it's permitted to create a project file by hand.
// (but edition of an existing is not a good practice as the input file should really not changed if any computation was made with xAdjuster set by a human)
// @#LP Note that for now, this is no CRC protection on on json content
// The idea to add a CRC is that to be sure the file was not corrupted or altered. Note that alteration of a file could be permitted,
// letting the user which make alterations remove the CRC (and the json loader consider that a missing CRC means file altered or build by a human.

bool ApplicationModel::projectEditionFunction_loadProject(const QString& strFileToLoadProject,                                                          
                                                          S_InputFilesWithImageFilesAttributes &sInputFilesWithAttributes_fromJsonFile,
                                                          e_mainComputationMode& eMainComputationMode,
                                                          QVector<bool>& qvectb_layersToUseForComputation_justCheckingStringEmptiness,
                                                          QVector<bool>& qvectb_correlationMapUsage_justCheckingStringEmptiness) {

    sInputFilesWithAttributes_fromJsonFile.clear();

    eMainComputationMode = e_mCM_notSet;
    qvectb_layersToUseForComputation_justCheckingStringEmptiness.fill(false, 3);
    qvectb_correlationMapUsage_justCheckingStringEmptiness.fill(false, 3);

    _sbaStrErrorMessage_aboutLoad = {};

    if (!_ptrGeoRefModel_imagesSet) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "Dev error #101";
        qDebug() << __FUNCTION__ << " error: ptrGeoRefModel_imagesSet is nullptr";
        return(false);
    }
    if (_ptrGeoRefModel_imagesSet->imageCount()) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "Dev error #102";
        qDebug() << __FUNCTION__ << " error: ptrGeoRefModel_imagesSet imageCount is not zero";
        return(false);
    }

    if (_appFile_project.filenameSet()) {
        qDebug() << __FUNCTION__ << " error: _appFile_project.inUse()";
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "Dev error #103";
        return(false);
    }

    _appFile_project.setFilename(strFileToLoadProject);


    QFile inputFile(strFileToLoadProject);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error: (read open) Can not open file " << strFileToLoadProject;
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

    QJsonObject qjsonObjProject;
    qjsonObjProject = jsonDoc.object();

    qDebug() << __FUNCTION__ <<  "qjsonObjRouteset = " << qjsonObjProject;


    //first pass loading with check about many things
    //but none about files presence on disk and none about geo consistency from associated file found about file on disk

    //---- mandatory content for any kind of project states
    /* - contentType ("project")
       - versionFormat
       - imagesSet_geoRefInfos
       - inputFiles (all, empty field requiered for not used files, attributes not mandatory for human edited file)
       - route  (only one)
       - computationSettings.mainComputationMode
       - pixelValueResolution
    */

    //check content tag
    QString qstr_contentType_project[] { "contentType", "project"};
    if (!qjsonObjProject.contains(qstr_contentType_project[0])) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_contentType_project[0];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_contentType_project[0];
        return(false);
    }
    if (qjsonObjProject.value(qstr_contentType_project[0]) != qstr_contentType_project[1]) {
        qDebug() << __FUNCTION__ <<  ": error: invalid value for key " << qstr_contentType_project[0];
        qDebug() << __FUNCTION__ <<  "         It has to be: " << qstr_contentType_project[1];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "invalid value for key: " + qstr_contentType_project[0];
        return(false);
    }

    //check formatVersion
    QString qstr_versionFormat_value[] { "versionFormat", "0.8"};
    if (!qjsonObjProject.contains(qstr_versionFormat_value[0])) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_versionFormat_value[0];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_versionFormat_value[0];
        return(false);
    }
    if (qjsonObjProject.value(qstr_versionFormat_value[0]) != qstr_versionFormat_value[1]) {
        qDebug() << __FUNCTION__ <<  ": error: invalid value for key " << qstr_versionFormat_value[0];
        qDebug() << __FUNCTION__ <<  "         It has to be: " << qstr_versionFormat_value[1];
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "invalid value for key: " + qstr_versionFormat_value[0];
        return(false);
    }

    //geo-referencement infos about background image:
    QString qstr_imagesSet_geoRefInfos { "imagesSet_geoRefInfos" };
    if (!qjsonObjProject.contains(qstr_imagesSet_geoRefInfos)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_imagesSet_geoRefInfos;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_imagesSet_geoRefInfos;
        return(false);
    }


    bool bEPSGCodeMustBeAValidNumericValue = true;//true by default
    QString qstrKey_useGeoRef { "bUseGeoRef"};
    if (!qjsonObjProject.contains(qstrKey_useGeoRef)) {
        //do not contains: => georef used
        //
        //it's valid to do not have bUseGeoRef for georeferenced trace (but must contains backgroundImage_geoRefInfos)
        //Format 0.5 was used before the introduction of this key-value pair useGeoRef.
        //and the introduction of useGeoRef don't lead to version format change.
        //
     } else {
        //contains but failed to load:
        bool bGot = getBoolValueFromJson(qjsonObjProject, qstrKey_useGeoRef, _bUseGeoRefForProject);
        if (!bGot) {
            qDebug() << __FUNCTION__ <<  ": error getting value for key: " + qstrKey_useGeoRef;
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "error getting value for key: " + qstrKey_useGeoRef;
            return(false);
        }
        bEPSGCodeMustBeAValidNumericValue = _bUseGeoRefForProject;
    }


    QJsonObject qjsonObj_imagesSet_geoRefInfos  =  qjsonObjProject.value(qstr_imagesSet_geoRefInfos).toObject();

    qDebug() << __FUNCTION__ << "qjsonObj_imagesSet_geoRefInfos = " << qjsonObj_imagesSet_geoRefInfos;

    //store GeoRef data from json for futur comparison with geoRef data from inputfile
    bool bReportFromJson = _ptrGeoRefModel_imagesSet->fromQJsonObject(qjsonObj_imagesSet_geoRefInfos);
    if (!bReportFromJson) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject " + qstr_imagesSet_geoRefInfos;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    _ptrGeoRefModel_imagesSet->showContent();

    if (bEPSGCodeMustBeAValidNumericValue) {
        //---------
        //check here that the EPSG code is known (ie knonw by proj.db file)
        QString qstrEPSGCode_fromProject = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
        qstrEPSGCode_fromProject = qstrEPSGCode_fromProject.remove("EPSG:");
        bool bReportEPSGCodeIsKnown_geoCRSSelect_inheritQATableModel =
                _geoCRSSelect_inheritQATableModel.checkStrEPSGCodeisKnown(qstrEPSGCode_fromProject);
        if (!bReportEPSGCodeIsKnown_geoCRSSelect_inheritQATableModel) {
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson =
                    "EPSG code "
                    + qstrEPSGCode_fromProject
                    + " is unknown by PROJ database";
            setState_editingProject_closeState_clearProject();
            return(false);
        }
        _ptrGeoRefModel_imagesSet->set_bUseGeoRef(true);
        //---------
    } else {
        _ptrGeoRefModel_imagesSet->forceEPSGCode("EPSG:notSet");
        _ptrGeoRefModel_imagesSet->set_bUseGeoRef(false);
    }

    QString strMsgError;
    bool bcomputationMainModeGot = _computationCore_iqaTableModel.computationMainMode_fromQJsonObject(qjsonObjProject,strMsgError);
    if (!bcomputationMainModeGot) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = strMsgError;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    eMainComputationMode =_computationCore_iqaTableModel.getMainComputationMode();

    //load input files
    //load in the AppModel, the computationCore will be feed with it later  _computationCore_iqaTableModel.___();
    QString strKey_inputFiles { "inputFiles" };
    if (!qjsonObjProject.contains(strKey_inputFiles)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << strKey_inputFiles;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + strKey_inputFiles;
        return(false);
    }
    QJsonObject qjsonObj_inputFiles =  qjsonObjProject.value(strKey_inputFiles).toObject();

    sInputFilesWithAttributes_fromJsonFile.set_mainComputationMode(eMainComputationMode);
    bool bsInputFileWithAttributes_fromJsonFileGot = sInputFilesWithAttributes_fromJsonFile.fromQJsonObject(qjsonObj_inputFiles);
    if (!bsInputFileWithAttributes_fromJsonFileGot) {
        qDebug() << __FUNCTION__ <<  ": error: _inputImgFileAttributes_fromLoadedJsonProjectFile.fromQJsonObject failed " << strKey_inputFiles;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject : " + strKey_inputFiles;
        setState_editingProject_closeState_clearProject();
        return(false);
    }

    //about displacement file input files:
    QString strMsgErrorDetails;
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        qvectb_layersToUseForComputation_justCheckingStringEmptiness[iela] = !(sInputFilesWithAttributes_fromJsonFile._inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela].isEmpty());
    }
    int nbLayersForComputation = qvectb_layersToUseForComputation_justCheckingStringEmptiness.count(true);
    qDebug() << __FUNCTION__ << "qvectb_layersToCompute_justCheckingStringEmptiness = " << qvectb_layersToUseForComputation_justCheckingStringEmptiness;
    qDebug() << __FUNCTION__ << "nbLayersForComputation =" << nbLayersForComputation;
    if (!nbLayersForComputation)  {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "none input files set";

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    //considers eMainComputationMode  as e_mCM_Typical_PX1PX2Together_DeltazAlone
    //about correlation score map input files:
    //@#LP could provide a generic method which convert qvect<QString> emptyness to U_CorrelationScoreMapFileUseFlags
    for (int iela = eLA_CorrelScoreForPX1PX2Together; iela <= eLA_CorrelScoreForDeltaZAlone; iela++) {
        qvectb_correlationMapUsage_justCheckingStringEmptiness[iela] = !(sInputFilesWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela].isEmpty());

        qDebug() << __FUNCTION__ << "sInputFilesWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[" << iela << "] = "
                 <<  sInputFilesWithAttributes_fromJsonFile._inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela];

        qDebug() << __FUNCTION__ << "qvectb_correlationMapUsage_justCheckingStringEmptiness[" << iela << "] = "
                 << qvectb_correlationMapUsage_justCheckingStringEmptiness[iela];


        if (iela == eLA_CorrelScoreForPX1PX2Together) {
            qvectb_correlationMapUsage_justCheckingStringEmptiness[iela+1] = qvectb_correlationMapUsage_justCheckingStringEmptiness[iela];
            iela++;//go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }

    //here file emptiness are computed for input files and input correl score map files
    // -emptiness about input files permits to know what pixelValueResolution should be present of not in json and abot parall&perp;ZOther (e_mCM_Typical_PX1PX2Together_DeltazAlone)
    //- emptiness about input correl score map files permit to know what correlationScoreMapParameters should be present or not in json

    //micmac step value and spatial resolution
    QString qstrKey_pixelValueResolution { "pixelValueResolution" };
    if (!qjsonObjProject.contains(qstrKey_pixelValueResolution)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstrKey_pixelValueResolution;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstrKey_pixelValueResolution;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    QJsonObject qjsonObj_micmacStepValues_and_spatialResolutions = qjsonObjProject.value(qstrKey_pixelValueResolution).toObject();
    qDebug() << __FUNCTION__ <<  "qjsonObj_micmacStepValues_and_spatialResolutions = " << qjsonObj_micmacStepValues_and_spatialResolutions;

    QString _tqstrLayerJSonDescName[3] {"PX1", "PX2", "ZOther"};
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        bool bPresentForLayer = qjsonObj_micmacStepValues_and_spatialResolutions.contains(_tqstrLayerJSonDescName[iela]);
        if (!qvectb_layersToUseForComputation_justCheckingStringEmptiness[iela]) {
            if (bPresentForLayer) {
                _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = qstrKey_pixelValueResolution + " values should not be set for " + _tqstrLayerJSonDescName[iela];

                setState_editingProject_closeState_clearProject();
                return(false);
            }
        } else {
            if (!bPresentForLayer) {
                _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = qstrKey_pixelValueResolution + " values must be set for " + _tqstrLayerJSonDescName[iela];

                setState_editingProject_closeState_clearProject();
                return(false);
            }
        }
        if (qvectb_layersToUseForComputation_justCheckingStringEmptiness[iela] && bPresentForLayer) {
            QJsonObject qjsonObj_micmacStepValues_and_spatialResolutions_byLayer = qjsonObj_micmacStepValues_and_spatialResolutions.value(_tqstrLayerJSonDescName[iela]).toObject();
            bool bQjsonObj_micmacSV_and_sR_byLayerGot = _vectDequantization_PX1_PX2_deltaZ[iela].fromQJsonObject(qjsonObj_micmacStepValues_and_spatialResolutions_byLayer);
            if (!bQjsonObj_micmacSV_and_sR_byLayerGot) {
                qDebug() << __FUNCTION__ << " error: _vectDequantization_PX1_PX2_deltaZ.fromQJsonObject failed for " << _tqstrLayerJSonDescName[iela];
                _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "fail loading " + qstrKey_pixelValueResolution + " values for " +_tqstrLayerJSonDescName[iela];

                setState_editingProject_closeState_clearProject();
                return(false);
            }
        }
    }

    //computationParameters
    //try to load (with check) computationParameters if available
    //
    QString strKey_computationParameters { "computationParameters" };
    if (!qjsonObjProject.contains(strKey_computationParameters)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << strKey_computationParameters;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + strKey_computationParameters;

        setState_editingProject_closeState_clearProject();
        return(false);
    }
    QJsonObject qjsonObj_computationParameters = qjsonObjProject.value(strKey_computationParameters).toObject();

    qDebug() << __FUNCTION__ << "qjsonObj_computationParameters =" << qjsonObj_computationParameters;


    bool bContainsComputationParameters = false;
    bool bComputationParametersValidAndLoaded = false;
    //the computation core knowns the main mode computation , it was loaded and set above
    bool bReport_computationSettingsFromQJson = _computationCore_iqaTableModel.computationParameters_fromQJsonObject(
                                                    qjsonObj_computationParameters,
                                                    qvectb_layersToUseForComputation_justCheckingStringEmptiness,
                                                    qvectb_correlationMapUsage_justCheckingStringEmptiness,
                                                    bContainsComputationParameters,
                                                    bComputationParametersValidAndLoaded,
                                                    strMsgError);
    if (!bReport_computationSettingsFromQJson) {
        if (    bContainsComputationParameters
            &&(!bComputationParametersValidAndLoaded)) {
            QString strKey_computationParameters { "computationParameters" };
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject " + strKey_computationParameters;
        } else {
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = strMsgError; //"dev error #102: failed to load json from JsonObject " + strKey_computationParameters;
        }

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    //@LP to add at the next check step:
    //@LP check about file will be made later with inputFileChecker
    //@LP check about geo will be made later

    //load route
    _routeContainer.clear();

    //@LP The routeset json key  is "route" in the project file
    QString qstr_routeset { "route" };
    if (!qjsonObjProject.contains(qstr_routeset)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_routeset;
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "key not found: " + qstr_routeset;

        setState_editingProject_closeState_clearProject();
        return(false);
    }
    QJsonArray qjsonArray_routeset = qjsonObjProject.value(qstr_routeset).toArray();
    qDebug() << __FUNCTION__ << "qjsonValue_routeset = " << qjsonArray_routeset;

    //routeset:
    //route (routeset with only one route):
    //the routeId will be checked as 0 (because routeset in a projet as only one route with routeId at 0) in _routeContainer.fromQJsonArray(...)
    bool bReport_toQJsonArray = _routeContainer.fromQJsonArray(qjsonArray_routeset);
    if (!bReport_toQJsonArray) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed to load json from JsonObject " + qstr_routeset;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    int routeCount = _routeContainer.routeCount();

    if (!routeCount) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed: json " + qstr_routeset + "found, but trace not loaded";

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    if (routeCount != 1) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed: json " + qstr_routeset + " contains more than one trace";

        setState_editingProject_closeState_clearProject();
        return(false);
    }   

    Route& refRoute = _routeContainer.getRouteRef(0);
    if (refRoute.getId() == -1) { //@LP should never happen
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "internal dev error #106: _routeContainer.getRouteRef(0).getId() is -1";

        setState_editingProject_closeState_clearProject();
        return(false);
    }
    if (!refRoute.segmentCount()) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "failed: trace from json contains not enough points";

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    /*
        "computationSettings": {
           "correlationScoreMapsUsageFlags": {
           "DeltaZ_or_Other": false,
           "PX1": false,
           "PX2": false
        },
    */


    //@LP note:
    // _computationCore._qvectb_layersToComputeFlags_PX1_PX2_DeltaZ
    // and
    // _correlationScoreMapFileAvailabilityFlags
    //are not set at this point

    //at this point _computationCore settings can be partially set from the json:
    // - just the MainComputationMode (mandatory)
    // - or MainComputationMode + all computationSettings

    //georef infos loaded but none match check with inputfile
    //route loaded and checked ok

    //Sections which can be not present in json file; depending on app state when saving the file
    //stackedProfilBoxes and stackedProfil use "boxId" key in json to help human edition

    //* stackedProfilBoxes:
    // about human edited file we will check that:
    // - boxId has no gap, start from zero and unique
    // - idxSegmentOwnerOfCenterPoint:
    //   - refers to a existing route segment idx (this field is computed by the app when distrubuting boxes along the route, or when moving boxes along the route.
    //     it's used to simplify sort needs to show the boxes list "along the route" instead of in the list order (added, removed, etc)
    //   - or does not exist or -1: meaning that the user want a specific location of the boxes without using any route.
    //     boxes in this case will be unmovable as unassiocated to any segment route.
    // - center is recomputed using centerPoint and it's accorging associated segment to void any location offset due to precision loss from loaded values from json file
    //   if recomputed center is not inside the segment => error
    //
    // - at least one box as a center inside the image : this test will be made after that input images were checked with the same pixel size
    // - unitVectorDirection will be checked to be normalized; If not it will be normalized.
    //   this permits to the user to give a vector direction non normalized (easiest edition for human, providing [3 , 2] for example
    // - oddPixelLength is odd and >= 3
    // - oddPixelWidth is odd and >= 1

    //set the route for the computation core, to be able to check that profil boxes has centerpoint which refers to existing segment
    //and relocate accurately these centerpoints.

    _computationCore_iqaTableModel.setRoutePtr(&refRoute);

    _projectContentState.setState_route(e_Project_routeContentState::e_P_routeCS_oneRouteValid);

    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_noneBox);
    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable);

    QVector<bool> qvectb_componentToCompute_whichNeedsToBePresentInJsonFile {false, false, false};
    //@#LP: compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode;
    //      ambigous API as nor computationCore_iqaTableModel, nor computationCore will change any of its private fields calling this method
    //
    bool bReport = _computationCore_iqaTableModel.compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode(
                    qvectb_layersToUseForComputation_justCheckingStringEmptiness, eMainComputationMode,
                    qvectb_componentToCompute_whichNeedsToBePresentInJsonFile);
    if (!bReport) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "dev error #950";

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    QString strKey_stackedProfilBoxes { "stackedProfilBoxes" };
    if (!qjsonObjProject.contains(strKey_stackedProfilBoxes)) {
        return(true);
    }

    QString strDetailsAboutFail_projectLoad;
    QJsonArray qjsonArray_stackedProfilBoxes = qjsonObjProject.value(strKey_stackedProfilBoxes).toArray();
    qDebug() << __FUNCTION__ << "qjsonArray_stackedProfilBoxes = " << qjsonArray_stackedProfilBoxes;
    bool bReport_stackedProfilBoxes_fromQJsonArray =
        _computationCore_iqaTableModel.stackedProfilBoxes_fromQJsonArray(qjsonArray_stackedProfilBoxes,
                                                                         qvectb_componentToCompute_whichNeedsToBePresentInJsonFile,
                                                                         strDetailsAboutFail_projectLoad);
    if (!bReport_stackedProfilBoxes_fromQJsonArray) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = strDetailsAboutFail_projectLoad;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);

    //* stackedProfil:
    //- boxId no gap, start from zero and unique
    //- boxId sync with stackedProfilBoxes existence
    // - each stackedProfil contains full content for a used layer
    // - content for a used layer xMax, xMin x0 with valid value according to box length
    // - if some _bValid is false, the value will be set to the default value depending on the left/right side or 0 for x0

    //"perp", "parall" and "zOther" keys in json to help human edition

    QString strKey_stackedProfil { "stackedProfil" };
    if (!qjsonObjProject.contains(strKey_stackedProfil)) {

        //@#LP already set to default from previous add done in stackedProfilBoxes_fromQJsonArray ?

        //set default value for the boxes linear regression models
        bool bReport = _computationCore_iqaTableModel.
                setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(
                    qvectb_componentToCompute_whichNeedsToBePresentInJsonFile);

        if (!bReport) {
            _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = "setting default values for stacked profils linear regression models failed";

            setState_editingProject_closeState_clearProject();
            return(false);
        }
        _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::
                                                          //e_P_stackedProfilEditedS_noneEdited_allSetAsDefaultValues
                                                          e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues);
        return(true);
    }

    QJsonArray qjsonArray_stackedProfil = qjsonObjProject.value(strKey_stackedProfil).toArray();
    qDebug() << __FUNCTION__ << "qjsonArray_stackedProfil = " << qjsonArray_stackedProfil;
    bool bReport_stackedProfil_fromQJsonArray =
        _computationCore_iqaTableModel.feed_stackedProfil_fromQJsonArray(qjsonArray_stackedProfil,
                                                                         qvectb_componentToCompute_whichNeedsToBePresentInJsonFile,
                                                                         strDetailsAboutFail_projectLoad);

    if (!bReport_stackedProfil_fromQJsonArray) {
        _sbaStrErrorMessage_aboutLoad._strMessageAboutJson = strDetailsAboutFail_projectLoad;

        setState_editingProject_closeState_clearProject();
        return(false);
    }

    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession);

    //about json imagesSet_geoRefInfos.imageGeoRef, the image file from the route (image background) was not checked if present at the path
    //and this is fine as we do not care about this field

    return(true);
}

//@LP this comparison not involves AppModel, but put here for consistency with the other type of geoRef handling methods in appModel for now
S_e_geoRefImagesSetStatus ApplicationModel::projectEditionFunction_openingFile_compareWorldFileDataAndEPSGCode_fromInputFiles_betweenThem(
    const S_InputFiles& inputFiles,
    const QVector<bool>& qvectb_layersToCompute,
    ImageGeoRef& strUniqueImageGeoRefGot,
    QString& strUniqueEPSGCodeGot,
    S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition) const {

    strUniqueImageGeoRefGot.clear();
    strUniqueEPSGCodeGot.clear();
    S_e_geoRefImagesSetStatus segeoRefImageSetStatus_projectEdition = {};
    sbaStrMsg_gRIS_projectEdition = {};

    if (!qvectb_layersToCompute.count(true)) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unknowError;
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
        sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData = "Dev Error #701";
        return(segeoRefImageSetStatus_projectEdition);
    }

    GeoRefModel_imagesSet geoRefModelImagesSet;
    //geoRefModelImagesSet.setRootForEPSGCacheStorage(_qstrRootForEPSGCacheStorage); //cache storage subdir for epsg code not used now, store in the same dir that the image

    bool bSomeLoadedButNotUsable = false;
    bool bSomeLoaded = geoRefModelImagesSet.setAndLoad_imageWidthHeight_TFWDataIfAvailable(inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ, bSomeLoadedButNotUsable);

    if (  (bSomeLoadedButNotUsable)
        ||(!bSomeLoaded)) {
        if (bSomeLoadedButNotUsable) {
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_loadedButNotUsable;
        } else {
            if (!bSomeLoaded) {
                segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notAvailable_noneFound;
            }
        }
        projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImageSetStatus_projectEdition,
                                                                      sbaStrMsg_gRIS_projectEdition);
        return(segeoRefImageSetStatus_projectEdition);
    }

    segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = geoRefModelImagesSet.check_TFWDataSet();
    projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(segeoRefImageSetStatus_projectEdition, sbaStrMsg_gRIS_projectEdition);

    if (segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase == e_iSS_geoTFWData_available) {
        bool bGotUnique = geoRefModelImagesSet.getUniqueImageGeoRef(strUniqueImageGeoRefGot);
        if (!bGotUnique) {
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unknowError;
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData = "Dev Error #702";
            return(segeoRefImageSetStatus_projectEdition);
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << ": segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = " << segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase;


    segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = geoRefModelImagesSet.searchInCache_associatedEPSGCode();

    qDebug() << __FUNCTION__ << __LINE__ << ": segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = " << segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG;

    projectEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
                &geoRefModelImagesSet,//_ptrGeoRefModel_imagesSet,
                segeoRefImageSetStatus_projectEdition, sbaStrMsg_gRIS_projectEdition);

    qDebug() << __FUNCTION__ << __LINE__ << ": segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = " << segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG;

    if (segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG == e_iSS_geoEPSG_available) {
        strUniqueEPSGCodeGot = geoRefModelImagesSet.get_qstrEPSGCode();
        qDebug() << __FUNCTION__ << ": unique EPSGCode found in cache: " << strUniqueEPSGCodeGot;
    } else {
        qDebug() << __FUNCTION__ << ": no unique EPSGCode found in cache (for some reason = " << segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG;
    }

    return(segeoRefImageSetStatus_projectEdition);
}

bool ApplicationModel::projectEditionFunction_compareWorldFileDataAndEPSGCode_toCurrentImageGeoRefInfos(
    bool bCompareEPSGCodeAlso, const ImageGeoRef& uniqueImageGeoRef, const QString& strUniqueEPSGCodeGot, QString& strErrorMsg) const {

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        strErrorMsg = "Dev Error #705";
        return(false);
    }

    ImageGeoRef imageGeoRef_fromCurrent;
    bool bGotCurrent = _ptrGeoRefModel_imagesSet->getUniqueImageGeoRef(imageGeoRef_fromCurrent);
    if (!bGotCurrent) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet igetUniqueImageGeoRef failed";
        strErrorMsg = "Dev Error #706";
        return(false);
    }

    bool bSameEPSGCode = false;
    if (bCompareEPSGCodeAlso) {
        if (strUniqueEPSGCodeGot.isEmpty()) {
            strErrorMsg = "EPSG code is empty";
            return(false);
        }
        bSameEPSGCode = (0 == _ptrGeoRefModel_imagesSet->get_qstrEPSGCode().compare(strUniqueEPSGCodeGot));
    }

    bool bSameWorldFileData = imageGeoRef_fromCurrent.checkSame_geoTFWDataconst(uniqueImageGeoRef);

    QString strWFD {"World File Data"};
    QString strEPSGCode {"EPSG Code"};

    if (bCompareEPSGCodeAlso) {

        bool bWorldFileDataAndEPSGCodeAreDifferent = (!bSameWorldFileData)&&(!bSameEPSGCode);
        if (bWorldFileDataAndEPSGCodeAreDifferent) {
            strErrorMsg = strWFD + " and " + strEPSGCode + " don't match with project";
            return(false);
        }

        if (bSameWorldFileData && (!bSameEPSGCode)) {
            strErrorMsg = strEPSGCode
                        + "[ " + strUniqueEPSGCodeGot + " ]"
                        + " doesn't match with project "
                        + "[ " + _ptrGeoRefModel_imagesSet->get_qstrEPSGCode() + " ]" ;
            return(false);
        }

        if ((!bSameWorldFileData) && bSameEPSGCode) {
            strErrorMsg = strWFD + " doesn't match with project";
            return(false);
        }
        return(true);
    }

    if (!bSameWorldFileData) {
        strErrorMsg = strWFD + " doesn't match with project";
        return(false);
    }
    return(true);
}


bool ApplicationModel::projectEditionFunction_inGeoRefModelImagesSet_populateFilenamesAndSyncGeoInfosFromAlreadyInPlace(
        const QVector<QString>& qvectStrInputFiles,
        const QVector<bool>& vectb_layersForComputation,
        bool bIsAboutFilesReplacement) {

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        return(false);
    }

    qDebug() << __FUNCTION__ << " entering with: _ptrGeoRefModel_imagesSet->showContent():";
    _ptrGeoRefModel_imagesSet->showContent();

    int count_imagesIn_GeoRefModelImagesSet = _ptrGeoRefModel_imagesSet->imageCount();

    if (!count_imagesIn_GeoRefModelImagesSet) {
        qDebug() << __FUNCTION__ << " error: no ImageGeoRef in set; can't duplicate geo infos.";
        return(false);
    }

    if (qvectStrInputFiles.size() != 3) {
        qDebug() << __FUNCTION__ << " error: qvectStrInputFiles.size() != 3";
        return(false);
    }

    int countLayersForComputation = vectb_layersForComputation.count(true);
    if (!countLayersForComputation) {
        qDebug() << __FUNCTION__ << " error: countLayersForComputation is zero";
        return(false);
    }

    if (count_imagesIn_GeoRefModelImagesSet != countLayersForComputation) {
        bool bReport = _ptrGeoRefModel_imagesSet->alignContent_vectImageGeoRefSize_andGeoInfosOnFirst(countLayersForComputation);
        if (!bReport) {
            return(false);
        }
    }

    QVector<QString> qvectStrImageFilename;
    for (int iela = eLA_PX1; iela < eLA_LayersCount; iela++) {
        if (vectb_layersForComputation[iela]) {
            qvectStrImageFilename.push_back(qvectStrInputFiles[iela]);
        }
    }

    qDebug() << __FUNCTION__ << "qvectStrImageFilename = " << qvectStrImageFilename;

    _ptrGeoRefModel_imagesSet->replace_imagesFilename(qvectStrImageFilename);

    if (bIsAboutFilesReplacement) {

        _appFile_project.setState_modified(true);
    }

    qDebug() << __FUNCTION__ << " exiting with replacement done: _ptrGeoRefModel_imagesSet->showContent():";
    _ptrGeoRefModel_imagesSet->showContent();

    return(true);
}

bool ApplicationModel::projectEditionFunction_compareWorldFileDataFromInputFiles_toCurrentWorldFileData(
   const ImageGeoRef& uniqueImageGotFromInputFiles, QString& strErrorMsg) const {

    S_e_geoRefImagesSetStatus segeoRefImageSetStatus_projectEdition = {};

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unknowError;
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
        strErrorMsg = "Dev Error #703";
        return(false);
    }

    ImageGeoRef imageGeoRef_fromCurrentGeoRefModel;
    bool bGotCurrent = _ptrGeoRefModel_imagesSet->getUniqueImageGeoRef(imageGeoRef_fromCurrentGeoRefModel);
    if (!bGotCurrent) {
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase = e_iSS_geoTFWData_notAvailable_unknowError;
        segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
        strErrorMsg = "Dev Error #704";
        return(false);
    }

    bool bSame = uniqueImageGotFromInputFiles.checkSame_geoTFWDataconst(imageGeoRef_fromCurrentGeoRefModel);
    return(bSame);
}

bool ApplicationModel::addBoxesAutomaticDistribution_closeAsPossible(int boxWidth, int boxlength,
                                                                     int ieStartingPointPFAD_selected, int ieDistanceBSBuAD_selected,
                                                                     QString& strErrorReason, bool& bCriticalError_pleaseExitApp) {

    strErrorReason.clear();
    bCriticalError_pleaseExitApp = false;

    if (  (ieDistanceBSBuAD_selected != e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel)
        &&(ieDistanceBSBuAD_selected != e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel)) {
        strErrorReason = "internal dev error (invalid ieDistanceBSBuAD_selected)";
        return(false);
    }

    if (ieStartingPointPFAD_selected == e_SPFAD_notSet) {
        strErrorReason = "internal dev error (ieStartingPointPFAD_selected is e_SPFAD_notSet)";
        return(false);
    }

    qDebug() << __FUNCTION__<< "ieDistanceBSBuAD_selected = " << ieDistanceBSBuAD_selected;

    //@LP boxWidth and boxlength need to be odd values

    Route& refRoute1 = _routeContainer.getRouteRef(0);
    if (refRoute1.getId() == -1) {
        strErrorReason = "internal dev error (trace #0 not found)";
        return(false);
    }

    refRoute1.showContent();

    bool bBoxesDistributedFromFirstPoint = (ieStartingPointPFAD_selected == e_SPFAD_firstPoint);

    BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_notUsedReport;
    bool bReport = false;
    if (bBoxesDistributedFromFirstPoint) {
         bReport =_boxOrienterAndDistributer.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
            &refRoute1, boxWidth, boxlength, static_cast<e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution>(ieDistanceBSBuAD_selected),
            false, true,
            e_ERACBCAR_notUsedReport);


    } else { //e_SPFAD_lastPoint
         bReport =_boxOrienterAndDistributer.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
           &refRoute1, boxWidth, boxlength,  static_cast<e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution>(ieDistanceBSBuAD_selected),
           false, true,
           e_ERACBCAR_notUsedReport);
    }

    //@#LP _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated can be 1 (one) and returned report value false due to "can not fit"
    if (!bReport) {
        //none box to add
        strErrorReason = "None box added";
        return(false);
    }
    _sibaStrMsgStatus_aboutAutomaticDistribution = _boxOrienterAndDistributer.get_sibaStrMsgStatus_aboutAutomaticDistribution();
    if (  (!_sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured)
        ||(!_sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated)) {
        strErrorReason = "None box added";
        return(false);
    }

    //store mapBoxId of current to be able to restore its according (new) vectboxId after the boxes insertion
    int mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion = -1;
    if (_currentBoxIdInVectOrderAlongRoute != -1) {
        _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
    }

    int vectBoxIdOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox = -1;
    int distributedBoxCount = _boxOrienterAndDistributer.getConstRef_qvectProfBoxParameters_automatically_distributed().count();

    bool bCriticalError_aboutcomputationCore_addBASPWM = false;

    bool bAddReport = _computationCore_iqaTableModel.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                        _boxOrienterAndDistributer.getConstRef_qvectProfBoxParameters_automatically_distributed(), bBoxesDistributedFromFirstPoint,
                        vectBoxIdOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                        bCriticalError_aboutcomputationCore_addBASPWM);

    if (!bAddReport) {
        if (bCriticalError_aboutcomputationCore_addBASPWM) {
            bCriticalError_pleaseExitApp = true;
            strErrorReason = "Fatal error adding boxes";
        } else {
            strErrorReason = "error adding boxes";
        }
        return(false);
    }

    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
    _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_mixedComputedAndNotComputed); //consider the first insertion as a mixed also
    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues);

    //set the route for the computation core to be able to resize boxes //@#LP resize check condition should be made out the computationCore
    _computationCore_iqaTableModel.setRoutePtr(&refRoute1);


    bool bErrorOccured_about_graphicsItemContainer_addBoxFromMapPASPWM = false;
    bAddReport = _graphicsItemContainer.addBoxes_from_mapBoxAndStackedProfilWithMeasurements_fromKeyToKey(_computationCore_iqaTableModel.getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements(),
                                                                               keyMapOfFirstInsertedBox,
                                                                               keyMapOfFirstInsertedBox + distributedBoxCount - 1,
                                                                               bErrorOccured_about_graphicsItemContainer_addBoxFromMapPASPWM);
    if (!bAddReport) {
        if (bErrorOccured_about_graphicsItemContainer_addBoxFromMapPASPWM) {
            bCriticalError_pleaseExitApp = true;
            strErrorReason = "Fatal error adding boxes";
        } else {
            strErrorReason = "error adding boxes";
        }
        return(false);
    }

    if (mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion != -1) {
        _computationCore_iqaTableModel.getIndexBoxInVectOrderAlongRoute_fromMapBoxId(mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion, _currentBoxIdInVectOrderAlongRoute);
        qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute <= " << _currentBoxIdInVectOrderAlongRoute;
         slot_selectionBoxChanged(_currentBoxIdInVectOrderAlongRoute, mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
        _graphicsItemContainer.updateSelectedBox_givenMapBoxId(mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
    }

    _appFile_project.setState_modified(true);
    return(true);
}


bool ApplicationModel::addBoxesAutomaticDistribution_distanceBetweenCenter(int boxWidth, int boxlength,
                                                                           int ieStartingPointPFAD_selected, int pixelDistanceBetweenCenter,
                                                                            QString &strErrorReason, bool& bCriticalError_pleaseExitApp) {

    bCriticalError_pleaseExitApp = false;
    strErrorReason.clear();

    if (pixelDistanceBetweenCenter < 1) {
        strErrorReason = "internal dev error (pixelDistanceBetweenCenter < 1)";
        return(false);
    }

    if (ieStartingPointPFAD_selected == e_SPFAD_notSet) {
        strErrorReason = "internal dev error (ieStartingPointPFAD_selected is e_SPFAD_notSet)";
        return(false);
    }

    //@LP boxWidth and boxlength needs to be odd values

    Route& refRoute1 = _routeContainer.getRouteRef(0);
    if (refRoute1.getId() == -1) {
        strErrorReason = "internal dev error (trace #0 not found)";
        return(false);
    }

    refRoute1.showContent();

    bool bBoxesDistributedFromFirstPoint = (ieStartingPointPFAD_selected == e_SPFAD_firstPoint);

    bool bReport = false;

    if (bBoxesDistributedFromFirstPoint) {
         bReport =_boxOrienterAndDistributer.computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromFirstPoint(
            &refRoute1, boxWidth, boxlength, pixelDistanceBetweenCenter);

    } else { //e_SPFAD_lastPoint
        bReport =_boxOrienterAndDistributer.computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromLastPoint(
           &refRoute1, boxWidth, boxlength, pixelDistanceBetweenCenter);
    }

    //@#LP _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated can be 1 (one) and returned report value false due to "can not fit"
    if (!bReport) {
        //none box to add
        strErrorReason = "None box added";
        return(false);
    }
    _sibaStrMsgStatus_aboutAutomaticDistribution = _boxOrienterAndDistributer.get_sibaStrMsgStatus_aboutAutomaticDistribution();
    if (  (!_sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured)
        ||(!_sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated)) {
        //none box to add
        strErrorReason = "None box added";
        return(false);
    }

    //store mapBoxId of current to be able to restore its according (new) vectboxId after the boxes insertion
    int mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion = -1;
    if (_currentBoxIdInVectOrderAlongRoute != -1) {
        _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
    }

    int vectBoxIdOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox = -1;
    int distributedBoxCount = _boxOrienterAndDistributer.getConstRef_qvectProfBoxParameters_automatically_distributed().count();

    bool bCriticalErrorOccured = false;

    bool bAddReport = _computationCore_iqaTableModel.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                _boxOrienterAndDistributer.getConstRef_qvectProfBoxParameters_automatically_distributed(), bBoxesDistributedFromFirstPoint,
                vectBoxIdOfFirstInsertedBox, keyMapOfFirstInsertedBox, bCriticalErrorOccured);
    if (!bAddReport) {

        if (bCriticalErrorOccured) {
            bCriticalError_pleaseExitApp = true;
            strErrorReason = "Fatal error adding boxes";
        } else {
             strErrorReason = "error adding boxes";
        }
        return(false);
    }

    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
    _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_mixedComputedAndNotComputed); //consider the first insertion as a mixed also
    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues);

    //set the route for the computation core to be able to resize boxes //@#LP resize check condition should be made out the computationCore
    _computationCore_iqaTableModel.setRoutePtr(&refRoute1);

    bool bErrorOcccured_about_graphicsItemContainer_addBoxFromMapBASPW = false;
    bReport = _graphicsItemContainer.addBoxes_from_mapBoxAndStackedProfilWithMeasurements_fromKeyToKey(_computationCore_iqaTableModel.getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements(),
                                                                               keyMapOfFirstInsertedBox,
                                                                               keyMapOfFirstInsertedBox + distributedBoxCount - 1,
                                                                               bErrorOcccured_about_graphicsItemContainer_addBoxFromMapBASPW);

    if (!bReport) {

        if (bErrorOcccured_about_graphicsItemContainer_addBoxFromMapBASPW) {
            bCriticalErrorOccured = true;
            strErrorReason = "Fatal error adding boxes";
            return(false);
        } else {
            // do nothing
        }
    }

    if (mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion != -1) {
        _computationCore_iqaTableModel.getIndexBoxInVectOrderAlongRoute_fromMapBoxId(mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion, _currentBoxIdInVectOrderAlongRoute);
        qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute <= " << _currentBoxIdInVectOrderAlongRoute;
         slot_selectionBoxChanged(_currentBoxIdInVectOrderAlongRoute, mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
        _graphicsItemContainer.updateSelectedBox_givenMapBoxId(mapBoxId_of_currentBoxIdInVectOrderAlongRoute_beforeInsertion);
    }

    _appFile_project.setState_modified(true);
    return(bReport);
}

bool ApplicationModel::runComputation() {

    _computationCore_iqaTableModel.set_vectDequantizationStepPtr(&_vectDequantization_PX1_PX2_deltaZ);

    bool bClearAllBoxes = isComputationRequiered_dueToParametersChanges();

    _computationCore_iqaTableModel.clearComputedStakedProfilsContent(bClearAllBoxes);
    _projectContentState.setState_computationParameters(e_Project_computationParametersState::e_P_computationParametersS_set);

    emit signal_openDialog_taskInProgress_forLongComputation(
        "processing...", "Computing stacked profile boxes",
        0, ptrCoreComputation_inheritQATableModel()->getBoxCount());

    //emit signal_stepProgressionForProgressBar(0);

    bool bReport = _computationCore_iqaTableModel.compute_stakedProfilsAlongRoute();

    emit signal_closeDialog_taskInProgress_forLongComputation();

    qDebug() << __FUNCTION__ <<  ": compute_stakedProfilsAlongRoute report said: " << bReport;
    if (!bReport) {
        //@#LP add msg error
        return(false);
    }

    _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_allComputed); //considers the first insertion as a mixed also

    _appFile_project.setState_modified(true);

    return(true);
}


bool ApplicationModel::componentsToShowChanged(const QVector<bool> &qvectBoolComponentsToShow) {

    _qvectBoolComponentsToShow_Perp_Parall_deltaZ = qvectBoolComponentsToShow;

    qDebug() << __FUNCTION__ <<" _qvectBoolComponentsToShow_Perp_Parall_DeltaZ = " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ;

    //try to comeback on sole to solve the missing curve update
    if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ.count(true) == 1) {
        bool bReport = prepareComputedBoxForStackEdition(_currentBoxIdInVectOrderAlongRoute, _currentMeasureIndex);
        if (!bReport) {
            //@#LP error msg
            qDebug() << __FUNCTION__ << " for #0 (main mesure) returned false";
            return(false);
        }

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {
                _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->setOffAnyAlignementMode();
            }
        }
        emit signal_setComponentsVisible_fromQvectBoolComponents(_qvectBoolComponentsToShow_Perp_Parall_deltaZ);
        return(true);
    }

    return(onStackedProfilScenes_alignComponentsForCurrentAlignMode());

}

bool ApplicationModel::onStackedProfilScenes_alignComponentsForCurrentAlignMode() {
    if (_bStackProfilEdit_syncYRange) {
        return(onStackedProfilScenes_alignComponentsOnXAndYRange());
    } else {
        return(onStackedProfilScenes_alignComponentsOnXRange());
    }
}

bool ApplicationModel::onStackedProfilScenes_alignComponentsOnXRange() {

    int countTrue = _qvectBoolComponentsToShow_Perp_Parall_deltaZ.count(true);

    qDebug() << __FUNCTION__ << "_qvectBoolComponentsToShow_Perp_Parall_DeltaZ: " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ;

    if (!countTrue) {
        qDebug() << __FUNCTION__ << "warning: countTrue is zero";
        return(false);
    }

    bool bMoreThanOneLayerOnScreen = (countTrue > 1);
    if (bMoreThanOneLayerOnScreen) {

        qreal throughAllScene_left_pixelDim  = 999999;
        qreal throughAllScene_right_pixelDim = -999999;

        double throughAllScene_wSceneSpaceAtLeft_pixelDim = 0;
        double throughAllScene_wSceneSpaceAtRight_pixelDim = 0;

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {

                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {

                    QRectF qrectF_areaOfLayerWhenAlone_pixelDim;

                    double wSceneSpaceAtLeft_pixelDim = 0;
                    double wSceneSpaceAtRight_pixelDim = 0;

                    if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->get_sceneRectF_areaOfLayerWhenAlone_pixelDim(
                                qrectF_areaOfLayerWhenAlone_pixelDim,
                                wSceneSpaceAtLeft_pixelDim,
                                wSceneSpaceAtRight_pixelDim)) {

                        qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "qrectF_areaOfLayerWhenAlone_pixelDim : " << qrectF_areaOfLayerWhenAlone_pixelDim;

                        if (qrectF_areaOfLayerWhenAlone_pixelDim.left() < throughAllScene_left_pixelDim) {
                            throughAllScene_left_pixelDim = qrectF_areaOfLayerWhenAlone_pixelDim.left();
                        }
                        if (qrectF_areaOfLayerWhenAlone_pixelDim.right() > throughAllScene_right_pixelDim) {
                            throughAllScene_right_pixelDim = qrectF_areaOfLayerWhenAlone_pixelDim.right();
                        }

                        if (wSceneSpaceAtLeft_pixelDim > throughAllScene_wSceneSpaceAtLeft_pixelDim) {
                            throughAllScene_wSceneSpaceAtLeft_pixelDim = wSceneSpaceAtLeft_pixelDim;
                        }
                        if (wSceneSpaceAtRight_pixelDim > throughAllScene_wSceneSpaceAtRight_pixelDim) {
                            throughAllScene_wSceneSpaceAtRight_pixelDim = wSceneSpaceAtRight_pixelDim;
                        }

                    }
                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "=> throughAllScene_left_pixelDim: " << throughAllScene_left_pixelDim;
                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "=> troughAllScene_right_pixelDim: " << throughAllScene_right_pixelDim;
                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "=> throughAllScene_wSceneSpaceAtLeft: " << throughAllScene_wSceneSpaceAtLeft_pixelDim;
                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "=> throughAllScene_wSceneSpaceAtRight: " << throughAllScene_wSceneSpaceAtRight_pixelDim;
                }
            }
        }


        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            qDebug() << __FUNCTION__ << "1828 ieCA : " <<  ieCA << "_qvectBoolLayersToShow_PX1_PX2_DeltaZ[ieLA]= " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA];

            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {

                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {

                    qDebug() << __FUNCTION__ << "1834 ieCA : " <<  ieCA << "_qvectPtrCGScene_profil_PX1_PX2_deltaZ[ieLA]= " << _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA];

                    double locationFromZeroOfFirstInsertedValueInStackedProfil = .0;
                    bool bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil = _computationCore_iqaTableModel.
                            get_locationFromZeroOfFirstInsertedValueInStackedProfil(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._boxId,
                                                                                    locationFromZeroOfFirstInsertedValueInStackedProfil);
                    if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {
                        qDebug() << __FUNCTION__ << "if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {"; //@#LP error msg ?
                        return(false);
                    }

                    qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil = " << locationFromZeroOfFirstInsertedValueInStackedProfil;

                    _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->feedScene_withExtendedXRange_sceneSpaceValuesInsteadOfPixel(
                            static_cast<e_ComponentsAccess>(ieCA),
                            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA],

                            throughAllScene_left_pixelDim,
                            throughAllScene_right_pixelDim,
                            throughAllScene_wSceneSpaceAtLeft_pixelDim,
                            throughAllScene_wSceneSpaceAtRight_pixelDim,

                            locationFromZeroOfFirstInsertedValueInStackedProfil,
                            -locationFromZeroOfFirstInsertedValueInStackedProfil,

                             static_cast<e_MeanMedianIndexAccess>(_computationCore_iqaTableModel.get_computationMethod(ieCA)),
                             _sGgraphicsParameters_profil, _eProfilAdjustMode);
                }
            }
        }
    } else {
        //will set only one in fact //@#LP improve way
        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {
                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {
                    _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->setOffAnyAlignementMode();
                }
            }
        }
    }

    emit signal_setComponentsVisible_fromQvectBoolComponents(_qvectBoolComponentsToShow_Perp_Parall_deltaZ);
    return(true);
}


bool ApplicationModel::onStackedProfilScenes_alignComponentsOnXAndYRange() {

    int countTrue = _qvectBoolComponentsToShow_Perp_Parall_deltaZ.count(true);

    qDebug() << __FUNCTION__ << "_qvectBoolLayersToShow_PX1_PX2_DeltaZ: " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ;

    if (!countTrue) {
        qDebug() << __FUNCTION__ << "warning: countTrue is zero";
        return(false);
    }

    bool bMoreThanOneLayerOnScreen = (countTrue > 1);
    if (bMoreThanOneLayerOnScreen) {

        qreal throughAllScene_yMin = 999999;
        qreal throughAllScene_yMax = -999999;

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {

                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {

                    qreal iComponent_yMin =  999999;
                    qreal iComponent_yMax = -999999;

                    if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->getYRangeAsSoleLayer(iComponent_yMin, iComponent_yMax)) {

                        qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "ilayer_yMin : " << iComponent_yMin;
                        qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "ilayer_yMax : " << iComponent_yMax;

                        if (iComponent_yMin < throughAllScene_yMin) {
                            throughAllScene_yMin = iComponent_yMin;
                        }
                        if (iComponent_yMax > throughAllScene_yMax) {
                            throughAllScene_yMax = iComponent_yMax;
                        }
                    }

                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "throughAllScene_yMin => " << throughAllScene_yMin;
                    qDebug() << __FUNCTION__ << "ieCA : " <<  ieCA << "throughAllScene_yMax => " << throughAllScene_yMax;
                }
            }
        }

        qDebug() << __FUNCTION__ << "final : " << "throughAllScene_yMin => " << throughAllScene_yMin;
        qDebug() << __FUNCTION__ << "final : " << "throughAllScene_yMax => " << throughAllScene_yMax;

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            qDebug() << __FUNCTION__ << "1828 ieCA : " <<  ieCA << "_qvectBoolLayersToShow_PX1_PX2_DeltaZ[ieLA]= " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA];

            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {

                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {

                    qDebug() << __FUNCTION__ << "1834 ieCA : " <<  ieCA << "_qvectPtrCGScene_profil_PX1_PX2_deltaZ[ieLA]= " << _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA];

                    double locationFromZeroOfFirstInsertedValueInStackedProfil = .0;
                    bool bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil = _computationCore_iqaTableModel.
                            get_locationFromZeroOfFirstInsertedValueInStackedProfil(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._boxId,
                                                                                    locationFromZeroOfFirstInsertedValueInStackedProfil);
                    if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {
                        qDebug() << __FUNCTION__ << "if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {"; //@#LP error msg ?
                        return(false);
                    }

                    qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil = " << locationFromZeroOfFirstInsertedValueInStackedProfil;

                    _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->feedScene_withExtendedYRange(
                            static_cast<e_ComponentsAccess>(ieCA),
                            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA],

                            throughAllScene_yMin, throughAllScene_yMax,

                            locationFromZeroOfFirstInsertedValueInStackedProfil,
                            -locationFromZeroOfFirstInsertedValueInStackedProfil,

                            static_cast<e_MeanMedianIndexAccess>(_computationCore_iqaTableModel.get_computationMethod(ieCA)),
                            _sGgraphicsParameters_profil, _eProfilAdjustMode);
                }
            }
        }
    } else {
        //will set only one in fact //@#LP improve way
        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
            if (_qvectBoolComponentsToShow_Perp_Parall_deltaZ[ieCA]) {
                if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {
                    _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->setOffAnyAlignementMode();
                }
            }
        }
    }

    emit signal_setComponentsVisible_fromQvectBoolComponents(_qvectBoolComponentsToShow_Perp_Parall_deltaZ);
    return(true);

}

bool ApplicationModel::prepareComputedBoxForStackEdition(int boxId) {
    return(prepareComputedBoxForStackEdition(boxId, _currentMeasureIndex));
}

//we reach this method when we want to show a new stacked profil layers group (one or more)
//this method handle the range extension on y if this what the user want (Ui check box)
//the alignement in X (when vertical layout used) is computed somewhere else as it needs a pixel ratio computation
bool ApplicationModel::prepareComputedBoxForStackEdition(int boxId, S_MeasureIndex measureIndex) {

    _currentBoxIdInVectOrderAlongRoute = boxId;
    _currentMeasureIndex  = measureIndex;

    _qvectStackedProfilInEdition_Perp_Parall_deltaZ.clear();
    _qvectStackedProfilInEdition_Perp_Parall_deltaZ.resize(3);
     for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._boxId = _currentBoxIdInVectOrderAlongRoute;
        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._measureIndex = _currentMeasureIndex;
    }

    _qvectBoolComputedComponents_Perp_Parall_deltaZ = _computationCore_iqaTableModel.get_qvectComputedComponents();

    qDebug() << __FUNCTION__ << "_qvectBoolComputedComponents_Perp_Parall_deltaZ = " << _qvectBoolComputedComponents_Perp_Parall_deltaZ;
    qDebug() << __FUNCTION__ << "_qvectBoolComponentsToShow_Perp_Parall_deltaZ   = " << _qvectBoolComponentsToShow_Perp_Parall_deltaZ;

    if (!_qvectBoolComponentsToShow_Perp_Parall_deltaZ.count(true)) {
         qDebug() << __FUNCTION__ << "if (!_qvectBoolComponentsToShow_Perp_Parall_deltaZ.count(true)) {";
        _qvectBoolComponentsToShow_Perp_Parall_deltaZ = _qvectBoolComputedComponents_Perp_Parall_deltaZ;
    }

    bool bComputationReport_leftRightSideLinearRegressions =
            _computationCore_iqaTableModel.compute_leftRightSidesLinearRegressionsModel_forBoxId(
            _currentBoxIdInVectOrderAlongRoute,
            _currentMeasureIndex);

    if (!bComputationReport_leftRightSideLinearRegressions) {
        qDebug() << __FUNCTION__ <<  ":bComputationReport_leftRightSideLinearRegressions report is: " <<  bComputationReport_leftRightSideLinearRegressions;
        //@#LP no clear
        qDebug() << __FUNCTION__ << "if (!bComputationReport_leftRightSideLinearRegressions)";
        return(false);
    }

    //for(int ieLA = eLA_PX1; ieLA <= eLA_deltaZ; ieLA++) {
    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

        qDebug() << __FUNCTION__ << " ieCA = " << ieCA;

        //@LP closing use case with component not used
        if (!_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieCA]) {
           continue;
        }


        bool bReport_stackedProfilWithMeasurementsFound = _computationCore_iqaTableModel.get_ptrStackedProfilWithMeasurements_fromBoxIdAndComponent
                ( _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._boxId,
                  static_cast<e_ComponentsAccess>(ieCA),
                  _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._stackedProfilWithMeasurementsPtr);

        qDebug() << __FUNCTION__ << " bReport_stackedProfilWithMeasurementsFound = " << bReport_stackedProfilWithMeasurementsFound;
        qDebug() << __FUNCTION__ << " ptrStackedProfilWithMeasurements @ = " << (void*) _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._stackedProfilWithMeasurementsPtr;

        if (!bReport_stackedProfilWithMeasurementsFound) {
            qDebug() << __FUNCTION__ << " error in get_stackedProfilWithMeasurements_fromBoxIdAndLayer(...)";
        } else {
            qDebug() << __FUNCTION__ << "(void*)ptrStackedProfilWithMeasurements = " << (void*) _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._stackedProfilWithMeasurementsPtr;

            double locationFromZeroOfFirstInsertedValueInStackedProfil = .0;
            bool bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil = _computationCore_iqaTableModel.
                    get_locationFromZeroOfFirstInsertedValueInStackedProfil(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._boxId,
                                                                            locationFromZeroOfFirstInsertedValueInStackedProfil);

            qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil = " << locationFromZeroOfFirstInsertedValueInStackedProfil;


            if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {                
                qDebug() << __FUNCTION__ << "if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {";
                return(false);
            }

            if (_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._stackedProfilWithMeasurementsPtr) {

                if (!_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]) {
                    qDebug() << __FUNCTION__ << "if (!_qvectPtrCGScene_profil_PX1_PX2_deltaZ[ieLA]) {";

                } else {

                    qDebug() << __FUNCTION__;
                    //feed graphic on a particular measure like a sole layer on screen
                    _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA]->feedScene(static_cast<e_ComponentsAccess>(ieCA),
                                                                               _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA],
                                                                               locationFromZeroOfFirstInsertedValueInStackedProfil,
                                                                               -locationFromZeroOfFirstInsertedValueInStackedProfil,
                                                                               static_cast<e_MeanMedianIndexAccess>(_computationCore_iqaTableModel.get_computationMethod(ieCA)),
                                                                               _sGgraphicsParameters_profil, _eProfilAdjustMode);
                }

                if (!_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA]) {
                    qDebug() << __FUNCTION__ << " if (!_qvectPtrStackProfilEdit_textWidgetPartModel_PX1_PX2_deltaZ[ieLA]) {";

                } else {

                    //feed textWidgetPartModel with all the measure
                    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA]->feed(static_cast<e_LayersAcces>(ieCA),
                                                                                               _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._stackedProfilWithMeasurementsPtr, //ptrStackedProfilWithMeasurements->_LRSide_linRegrModel_mainMeasure,
                                                                                               locationFromZeroOfFirstInsertedValueInStackedProfil,
                                                                                               -locationFromZeroOfFirstInsertedValueInStackedProfil,
                                                                                               _qvectqstr_graphicTitle_Perp_Parall_deltaZ[ieCA]);
                    //set textWidgetPartModel on a particular measure
                    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA]->setOnMeasure(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieCA]._measureIndex);
                }

            } else {
                qDebug() << __FUNCTION__ << "_qvectStackedProfilInEdition__PX1_PX2_deltaZ[" << ieCA << "]._stackedProfilWithMeasurementsPtr is nullpr";
                //set layer to not computed #LP probable dev error
            }
        }
    }
    return(true);
}


bool ApplicationModel::project_exportResult(const QString& strOutFileName, const S_ExportSettings &sExportSettings) {

    if (sExportSettings._ieExportResult_fileType == e_ERfT_json) {
        return(project_exportResult_json(strOutFileName, sExportSettings));
    }
    return(project_exportResult_ascii(strOutFileName, sExportSettings));
}

bool ApplicationModel::project_exportResult_json(const QString& strOutFileName, const S_ExportSettings &sExportSettings) {

    //complete json for now

    //write infos about the georef (no images filename)
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        qDebug() << __FUNCTION__ << "        Can not save without geoRef data";
        _strDetailsAboutFail_projectWrite = "Dev Error #7 GeoRefModel_imagesSet is nullptr";
        return(false);
    }

    QJsonObject QJsonObject_exportFileContent;
    QJsonObject_exportFileContent.insert("contentType", "resultExport");
    QJsonObject_exportFileContent.insert("versionFormat", "0.8");
    QJsonObject_exportFileContent.insert("GeneratedByAppRelease", AppVersionInfos::_sctct_appVersion); //"2021-march-18_a");

    if (sExportSettings._ieExportResult_boxesContent == e_ERbC_all) {
        QJsonObject_exportFileContent.insert("boxContent", "allBoxes");
    } else {
        QJsonObject_exportFileContent.insert("boxContent", "onlyOneBox");
    }

    QJsonObject_exportFileContent.insert("bIncludeProfilesCurvesData", sExportSettings._bIncludeProfilesCurvesData);

    QJsonObject_exportFileContent.insert("bWarningFlagByUser_setLinearRegressionData_asEmpty", sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty);
    QJsonObject_exportFileContent.insert("bWarningFlagByUser_setProfilesCurvesData_asEmpty"  , sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty);

    //export date
    QDateTime localDateTime(QDateTime::currentDateTime());
    QDateTime UTCDateTime = localDateTime.toUTC();
    QString strDateFormat = "yyyy.MM.dd";
    QString strUTCDate = UTCDateTime.date().toString(strDateFormat);
    QString strTimeFormat = "hh:mm:ss";
    QString strUTCTime = UTCDateTime.time().toString(strTimeFormat);
    QString strDateTime = strUTCDate + " " +strUTCTime;
    QJsonObject_exportFileContent.insert("exportDate", strDateTime);
    QJsonObject_exportFileContent.insert("exportDateFormat", strDateFormat + " " + strTimeFormat);

    //geo-referencement infos about project:
    QJsonObject qjsonObj_geoRefInfos;
    bool bReport_toQJsonObject = _ptrGeoRefModel_imagesSet->toQJsonObject(qjsonObj_geoRefInfos);
    if (!bReport_toQJsonObject) {
        qDebug() << __FUNCTION__ << " error: _ptrGeoRefModel_imagesSet->toQJsonObject failed";
        _strDetailsAboutFail_ProjectWrite = "Dev Error #8_2 GeoRefModel_imagesSet toQJsonObject() error";
        return(false);
    }
    QJsonObject_exportFileContent.insert("imagesSet_geoRefInfos", qjsonObj_geoRefInfos);

    QJsonObject_exportFileContent.insert("bUseGeoRef", _bUseGeoRefForProject);

    //qDebug() << "QJsonObject_routeSetFileContent: " << QJsonObject_routeSetFileContent;

    //inputFiles
    QJsonObject qjsonObj_inputFiles;

    //writing a project consider inputFiles valid and routeContainer with at least one route (should be only one)
    //write input files from the AppModel, the computationCore could not be feed with it

    S_InputFilesWithImageFilesAttributes sInputFilesWithImageFilesAttributes;
    bool bAttrGotFromFile = sInputFilesWithImageFilesAttributes.fromInputFiles(_inputFiles);
    if (!bAttrGotFromFile) {
        qDebug() << __FUNCTION__ << " error: failed to got attributes from an input file";
        _strDetailsAboutFail_ProjectWrite = "error: failed to got attributes from an input file";
        return(false);
    }
    sInputFilesWithImageFilesAttributes.showContent(); //@LP debug

    sInputFilesWithImageFilesAttributes.toQJsonObject(qjsonObj_inputFiles); //@#LP could requiered eMainComputationMode to know how to save correlation map usage (PX1PX2 together)

    QString strKey_inputFiles { "inputFiles" };
    QJsonObject_exportFileContent.insert(strKey_inputFiles, qjsonObj_inputFiles);

    //route; this has to be a routeset with only one route
    if (_routeContainer.routeCount() > 1) {
        qDebug() << __FUNCTION__ << " error: _routeContainer routeCount > 1";
        _strDetailsAboutFail_ProjectWrite = "route count > 1"; //if the user edit/hack the project file, this is not a 'Dev Error'
        return(false);
    }

    if (_projectContentState.getState_route() != e_Project_routeContentState::e_P_routeCS_oneRouteValid) {
        qDebug() << __FUNCTION__ << " error: route is in a invalid state. Can not save project.";
        _strDetailsAboutFail_ProjectWrite = "Route is an invalid state";
        return(false);
    }

    //route name
    QString str_routeName_key {"routeName"};
    Route& refRoute1 = _routeContainer.getRouteRef(0);
    //refRoute1.showContent();
    if (refRoute1.getId() == -1) {
        qDebug() << __FUNCTION__ << " error: route is invalid ";
        _strDetailsAboutFail_ProjectWrite = "route is invalid";
        return(false);
    }
    QJsonObject_exportFileContent.insert(str_routeName_key, refRoute1.getName());

    //@#LP namming orientationFault vs useRouteInReversePointOrder

    //micmac step value and spatial resolution
    QJsonObject qjsonObj_micmacStepValues_and_spatialResolutions;
    QString _tqstrLayerJSonDescName[3] {"PX1", "PX2", "ZOther"};
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        QJsonObject qjsonObj_micmacSV_and_sR_byLayer;
        if (!_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela].isEmpty()) { //@#LP add and replace with a QVector<bool> layer_to_compute ?
            bool bQjsonObj_micmacSV_and_sR_byLayerGot = _vectDequantization_PX1_PX2_deltaZ[iela].toQJsonObject(qjsonObj_micmacSV_and_sR_byLayer);
            if (!bQjsonObj_micmacSV_and_sR_byLayerGot) {
                qDebug() << __FUNCTION__ << " error: stackedProfiBoxes_toQJsonArray_ failed";
                _strDetailsAboutFail_projectWrite = "invalid micmacStep or spatial Resolution for " + _tqstrLayerJSonDescName[iela];
                return(false);
            }
            qjsonObj_micmacStepValues_and_spatialResolutions.insert(_tqstrLayerJSonDescName[iela], qjsonObj_micmacSV_and_sR_byLayer);
        }
    }
    QJsonObject_exportFileContent.insert("pixelValueResolution", qjsonObj_micmacStepValues_and_spatialResolutions);

    QJsonObject qjsonObj_computationMainMode;
    QString str_computationMainMode_key {"computationMainMode"};

    QJsonObject_exportFileContent.insert(str_computationMainMode_key, static_cast<int>(_computationCore_iqaTableModel.getMainComputationMode()));

    bool bComputationParametersAvailable =
            (_projectContentState.getState_computationParameters() != e_Project_computationParametersState::e_P_computationParametersS_notSet);
    if (bComputationParametersAvailable) {
        QJsonObject qjsonObj_computationSettings;
        _computationCore_iqaTableModel.computationParameters_toQJsonObject(qjsonObj_computationSettings);
        QString strKey_computationSettings { "computationParameters" };
        QJsonObject_exportFileContent.insert(strKey_computationSettings, qjsonObj_computationSettings);
    }

    bool bOneOrMoreBoxes = (_projectContentState.getState_boxes() == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);

    if (bOneOrMoreBoxes) {

        e_ProfilOrientation eProfilOrientation = _computationCore_iqaTableModel.getProgressDirection();
        if (eProfilOrientation == e_PO_notSet) {
            return(false);
        }

        int nbBoxes = _computationCore_iqaTableModel.getBoxCount();

        bool bReverseOrderInQJsonArray = false;
        int idxJsonArrayExport_ = 0;
        int idxBox_ = 0;
        int idxBoxProgressIncrement_ = +1;

        if (eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {
            bReverseOrderInQJsonArray = true;
            idxBox_ = nbBoxes -1;
            idxBoxProgressIncrement_ = -1;
        }

        //if export only the current box, set on it, and set the increment to be over the boxId range
        if (sExportSettings._ieExportResult_boxesContent == e_ERbC_onlyCurrent) {
            idxBox_ = sExportSettings._idOfCurrentBox;
            idxBoxProgressIncrement_ *= nbBoxes;
            if ((idxBox_ < 0)||(idxBox_ >= nbBoxes)) {
                _strDetailsAboutFail_ProjectWrite = "invalid current Box Id";
                return(false);
            }
        }

        bool bExportGeo = _bUseGeoRefForProject;
        bool bExportDistanceOfBoxCenterFromStartOfTheRoute = true;

        {
            //write stacked profil boxes
            QJsonArray qjsonArray_stackedProfilBoxes;

            int idxJsonArrayExport = idxJsonArrayExport_;
            int idxBox = idxBox_;
            int idxBoxProgressIncrement = idxBoxProgressIncrement_;

            //boxID stay like in the project,
            //if the progress Direction is from last to first point of the route:
            // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
            // - we write boxex in the reverse order of the qvect
            for (; _computationCore_iqaTableModel.vectBoxIdValid(idxBox); idxBox+=idxBoxProgressIncrement) {

                //get QJsonObject of stacked ProfilBoxParameters without Box Set Infos (we do not need for result export)
                QJsonObject qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos;
                bool bReport_stackedProfilBoxToQJsonObjectWithoutBoxSetInfos =
                        _computationCore_iqaTableModel.get_oneStackedProfilBoxParametersToQJsonObjectWithoutBoxSetInfos(idxBox, qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos);

                qDebug() << __FUNCTION__ << "bReport_stackedProfilBoxToQJsonObjectWithoutBoxSetInfos = " << bReport_stackedProfilBoxToQJsonObjectWithoutBoxSetInfos;

                //but we have to alter and add some infos
                //get the SProfilsBoxParameters of the box
                S_ProfilsBoxParameters SProfilsBoxParameters;
                bool bReport_stackedProfilBoxGot =
                        _computationCore_iqaTableModel.get_oneStackedProfilBoxParameters(idxBox, SProfilsBoxParameters);
                qDebug() << __FUNCTION__ << "bReport_stackedProfilBoxGot = " << bReport_stackedProfilBoxGot;

                if (  (!bReport_stackedProfilBoxToQJsonObjectWithoutBoxSetInfos)
                    ||(!bReport_stackedProfilBoxGot)) {
                    qDebug() << __FUNCTION__ << "bReport aaa or bbb";
                    return(false);
                }

                //reverse the unit vector direction if requiered
                if (eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {

                    //reverse
                    SProfilsBoxParameters._unitVectorDirection = -SProfilsBoxParameters._unitVectorDirection;

                    //and replace in qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos:
                    //----------
                    QJsonArray qjsonArray_unitVectorDirection;
                    bool bReportUVDToQJsonArray = qrealPoint_toQJsonArray(SProfilsBoxParameters._unitVectorDirection.x(),
                                                                          SProfilsBoxParameters._unitVectorDirection.y(),
                                                                          7, qjsonArray_unitVectorDirection);
                    if (!bReportUVDToQJsonArray) {
                        return(false);
                    }
                    //@LP: normalized vector direction
                    QString qstrKey_unitVectorDirection {"unitVectorDirection"};
                    qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(qstrKey_unitVectorDirection, qjsonArray_unitVectorDirection);
                    //----------

                }

                //
                //for json result export the boxId matches with what is displayed in User Interface: boxId from logic +1. It's not the boxId from logic (like in project json file)
                //hence json and ascii result export files are sync about boxId alignment
                QString qstrKey_boxId {"boxId"};
                qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(qstrKey_boxId, idxBox+1);
                //
                //


                //compute the azimuth FromTopVertical of the unit vector direction (reversed if requiered)
                double azimuthDegreeFromTopVertical = convert_qcsVectorDirectionToAzimuthtFromTopVertical(SProfilsBoxParameters._unitVectorDirection);
                qreal qrAzimuthDegreeFixedPrecicision =.0;
                bool bOK = true;
                bOK &= qrealToRealNumberFixedPrecision_f(azimuthDegreeFromTopVertical, 3, qrAzimuthDegreeFixedPrecicision);
                if (!bOK) {
                    return(false);
                }
                QString strKey_azimuthDegreeFromTopVertical = "azimuthDegree";
                qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(strKey_azimuthDegreeFromTopVertical, qrAzimuthDegreeFixedPrecicision);

                if (bExportGeo) {
                    if (!_ptrGeoRefModel_imagesSet) {
                        qDebug() << __FUNCTION__ << "_ptrGeoRefModel_imagesSet is nullptr";
                        return(false);
                    } else {
                        S_lonlat sLonlat {.0, .0};
                        bool bReport_projXYtoLonLat = _ptrGeoRefModel_imagesSet->projXY_fromQcs_toLonLat(SProfilsBoxParameters._qpfCenterPoint, sLonlat);
                        if (!bReport_projXYtoLonLat) {
                            qDebug() << __FUNCTION__ << "bReport_projXYtoLonLat: " <<  bReport_projXYtoLonLat;
                            return(false);
                        }
                        qreal qrLonFixedPrecicision=.0, qrLatFixedPrecicision=.0;
                        bool bOK = true;
                        bOK &= qrealToRealNumberFixedPrecision_f(sLonlat._lon, 7, qrLonFixedPrecicision);
                        bOK &= qrealToRealNumberFixedPrecision_f(sLonlat._lat, 7, qrLatFixedPrecicision);
                        if (!bOK) {
                            qDebug() << __FUNCTION__ << "qrealToRealNumberFixedPrecision: " <<  bOK;
                            return(false);
                        }
                        QJsonObject qjsonObj_centerPoint_lonlat;
                        qjsonObj_centerPoint_lonlat.insert("lon", qrLonFixedPrecicision);
                        qjsonObj_centerPoint_lonlat.insert("lat", qrLatFixedPrecicision);

                        QString strKey_centerPoint_lonlat = "centerPoint_lon_lat";
                        qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(strKey_centerPoint_lonlat, qjsonObj_centerPoint_lonlat);
                    }
                } else {
                    //do nothing
                }

                if (bExportDistanceOfBoxCenterFromStartOfTheRoute) {

                    qreal pixel_distanceFromStartingRoutePointToRoutePoint = .0;

                    bool bDistanceComputed = refRoute1.computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection(
                        SProfilsBoxParameters._qpfCenterPoint,
                        SProfilsBoxParameters._idxSegmentOwnerOfCenterPoint,
                        pixel_distanceFromStartingRoutePointToRoutePoint);

                    if (!bDistanceComputed) {
                        qDebug() << __FUNCTION__ << "computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection failed";
                        return(false);
                    }

                    qDebug() << __FUNCTION__ << "Distance_alongRoute_fromStartingRoutePointToARoutePoint :";
                    qDebug() << __FUNCTION__ << "  idxBox = " << idxBox;
                    qDebug() << __FUNCTION__ << "  _qpfCenterPoint = " << SProfilsBoxParameters._qpfCenterPoint;
                    qDebug() << __FUNCTION__ << "  _idxSegmentOwnerOfCenterPoint = " << SProfilsBoxParameters._idxSegmentOwnerOfCenterPoint;
                    qDebug() << __FUNCTION__ << "  pixel_distanceFromStartingRoutePointToRoutePoint = " << pixel_distanceFromStartingRoutePointToRoutePoint;

                    QJsonObject qjsonObj_centerPoint_distanceFromStartingRoutePoint; //meter or pixel

                    if (bExportGeo) {

                        qreal WF_distanceFromStartingRoutePointToRoutePoint = .0;
                        bool bconvertDistance_toWF = _ptrGeoRefModel_imagesSet->
                                convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
                                    pixel_distanceFromStartingRoutePointToRoutePoint,
                                    WF_distanceFromStartingRoutePointToRoutePoint);

                        if (!bconvertDistance_toWF) {
                            qDebug() << __FUNCTION__ << "convertDistance_fromQcs_toWF_withSameXScaleAndYScale failed";
                            return(false);
                        }

                        qDebug() << __FUNCTION__ << "WF_distanceFromStartingRoutePointToRoutePoint = " << WF_distanceFromStartingRoutePointToRoutePoint;

                        qreal qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision = .0;
                        bool bOK = true;
                        bOK &= qrealToRealNumberFixedPrecision_f(WF_distanceFromStartingRoutePointToRoutePoint, 3,
                                                                 qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision);
                        if (!bOK) {
                            qDebug() << __FUNCTION__ << "WF_distance; qrealToRealNumberFixedPrecision_f: " <<  bOK;
                            return(false);
                        }
                        qjsonObj_centerPoint_distanceFromStartingRoutePoint.insert(
                            "meter",
                            qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision);

                    } else {

                        qreal pixel_distanceFromStartingRoutePointToRoutePoint_fixedPrecicision = .0;
                        bool bOK = true;
                        bOK &= qrealToRealNumberFixedPrecision_f(pixel_distanceFromStartingRoutePointToRoutePoint, 3,
                                                                 pixel_distanceFromStartingRoutePointToRoutePoint_fixedPrecicision);
                        if (!bOK) {
                            qDebug() << __FUNCTION__ << "pixel_distance; qrealToRealNumberFixedPrecision_f: " <<  bOK;
                            return(false);
                        }
                        qjsonObj_centerPoint_distanceFromStartingRoutePoint.insert(
                            "pixel",
                            pixel_distanceFromStartingRoutePointToRoutePoint_fixedPrecicision);
                    }

                    QString strKey_centerPoint_distanceFromStartingRoutePoint = "distanceFromFirstPointOfTrace";
                    qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(
                        strKey_centerPoint_distanceFromStartingRoutePoint,
                        qjsonObj_centerPoint_distanceFromStartingRoutePoint);
                }

                qjsonArray_stackedProfilBoxes.insert(idxJsonArrayExport, qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos);
                idxJsonArrayExport++;
            }
            QString strKey_stackedProfilBoxes { "stackedProfilBoxes" };
            QJsonObject_exportFileContent.insert(strKey_stackedProfilBoxes, qjsonArray_stackedProfilBoxes);
        }

        {
            //write stacked profil parameters (ranges, X0 and LinearRegresionModelResult)
            QJsonArray qjsonArray_stackedProfil;

            //boxID stay like in the project,
            //if the progress Direction is from last to first point of the route:
            // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
            // - we write the box in the reverse order of the qvect
            bool bReport_toQJsonArray = _computationCore_iqaTableModel.
                    stackedProfilWithMeasurements_withLinearRegresionModelResult_toQJsonArray(
                        qjsonArray_stackedProfil,
                        bReverseOrderInQJsonArray,
                        sExportSettings,
                        true, //sync json export result boxId with ascci export result boxId
                        _strDetailsAboutFail_projectWrite);

            if (!bReport_toQJsonArray) {
                qDebug() << __FUNCTION__ << " error: stackedProfil_toQJsonArray_ failed";
                return(false);
            }
            QString strKey_stackedProfil { "stackedProfil_rangesX0AndLinearRegressionModels" };
            QJsonObject_exportFileContent.insert(strKey_stackedProfil, qjsonArray_stackedProfil);
        }

        //write each profil curves content (depending on computed components):
        //by profil
        //   x, Perp, Parall
        //or x, deltaZOrOther
        //or x, Perp, Parall, deltaZOrOther

        if (sExportSettings._bIncludeProfilesCurvesData) {

            QJsonObject qjsonObj_profilCurves;

            //columns title
            QJsonObject qjsonObj_profilCurvesColumnsInfo;
            bool bReport_profilCurvesQjsonObjByBox = _computationCore_iqaTableModel.computedComponentsToHeader_toQJsonObject(qjsonObj_profilCurvesColumnsInfo);
            if (!bReport_profilCurvesQjsonObjByBox) {
                return(false);
            }
            qjsonObj_profilCurves.insert("columnsInfo", qjsonObj_profilCurvesColumnsInfo);

            //profil curves content
            QJsonArray qjsonArray_profilCurvesForAllStackedProfilBoxes;

            int idxJsonArrayExport = idxJsonArrayExport_;
            int idxBox = idxBox_;
            int idxBoxProgressIncrement = idxBoxProgressIncrement_;
            //boxID stay like in the project,
            //if the progress Direction is from last to first point of the route:
            // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
            // - we write the box in the reverse order of the qvect
            for (; _computationCore_iqaTableModel.vectBoxIdValid(idxBox); idxBox+=idxBoxProgressIncrement) {
                QJsonObject qjsonObj_profilCurveContent;
                bool bReport_profilCurvesContentQjsonObjByBox = _computationCore_iqaTableModel.profilCurvesForOneStackedProfilBox_toQJsonObject(
                            idxBox, sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty, qjsonObj_profilCurveContent);
                if (!bReport_profilCurvesContentQjsonObjByBox) {
                    return(false);
                }

                //
                //for json result export the boxId matches with what is displayed in User Interface: boxId from logic +1. It's not the boxId from logic (like in project json file)
                //hence json and ascii result export files are sync about boxId alignment
                QString qstrKey_boxId {"boxId"};
                qjsonObj_profilCurveContent.insert(qstrKey_boxId, idxBox+1);
                //
                //

                qjsonArray_profilCurvesForAllStackedProfilBoxes.insert(idxJsonArrayExport, qjsonObj_profilCurveContent);
                idxJsonArrayExport++;
            }

            qjsonObj_profilCurves.insert("content", qjsonArray_profilCurvesForAllStackedProfilBoxes);

            QJsonObject_exportFileContent.insert("profilCurves", qjsonObj_profilCurves);
        }
    }

    QJsonDocument JsonDoc(QJsonObject_exportFileContent);

    QFile outFile(strOutFileName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "error: (write open) Can not open file " << strOutFileName;
        return(false);
    }
    QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
    qint64 wroteBytes = outFile.write(qbArrayData);
    if (wroteBytes <= 0) {
         qDebug() << "error: writing byte to file " << strOutFileName;
         return(false);
    }
    return(true);
}

bool ApplicationModel::project_exportResult_ascii(const QString& strOutFileName, const S_ExportSettings &sExportSettings) {

    //complete json for now

    //write infos about the georef (no images filename)
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: GeoRefModel_imagesSet is nullptr";
        qDebug() << __FUNCTION__ << "        Can not save without geoRef data";
        _strDetailsAboutFail_projectWrite = "Dev Error #7 GeoRefModel_imagesSet is nullptr";
        return(false);
    }

    QFile outFile(strOutFileName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "error: (write open) Can not open file " << strOutFileName;
        return(false);
    }

    {
        QJsonObject qJsonObject_exportFileContent;
        qJsonObject_exportFileContent.insert("contentType", "resultExport");
        qJsonObject_exportFileContent.insert("versionFormat", "0.8");
        qJsonObject_exportFileContent.insert("GeneratedByAppRelease", AppVersionInfos::_sctct_appVersion); //"2021-march-18_a");

        if (sExportSettings._ieExportResult_boxesContent == e_ERbC_all) {
            qJsonObject_exportFileContent.insert("boxContent", "allBoxes");
        } else {
            qJsonObject_exportFileContent.insert("boxContent", "onlyOneBox");
        }
        qJsonObject_exportFileContent.insert("bIncludeProfilesCurvesData", boolToAsciiBoolString(sExportSettings._bIncludeProfilesCurvesData));

        qJsonObject_exportFileContent.insert("bWarningFlagByUser_setLinearRegressionData_asEmpty", sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty);
        qJsonObject_exportFileContent.insert("bWarningFlagByUser_setProfilesCurvesData_asEmpty"  , sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty);

        //export date
        QDateTime localDateTime(QDateTime::currentDateTime());
        QDateTime UTCDateTime = localDateTime.toUTC();
        QString strDateFormat = "yyyy.MM.dd";
        QString strUTCDate = UTCDateTime.date().toString(strDateFormat);
        QString strTimeFormat = "hh:mm:ss";
        QString strUTCTime = UTCDateTime.time().toString(strTimeFormat);
        QString strDateTime = strUTCDate + " " +strUTCTime;
        qJsonObject_exportFileContent.insert("exportDate", strDateTime);
        qJsonObject_exportFileContent.insert("exportDateFormat", strDateFormat + " " + strTimeFormat);

        //geo-referencement infos about project:
        QJsonObject qjsonObj_geoRefInfos;
        bool bReport_toQJsonObject = _ptrGeoRefModel_imagesSet->toQJsonObject(qjsonObj_geoRefInfos);
        if (!bReport_toQJsonObject) {
            qDebug() << __FUNCTION__ << " error: _ptrGeoRefModel_imagesSet->toQJsonObject failed";
            _strDetailsAboutFail_ProjectWrite = "Dev Error #8_3 GeoRefModel_imagesSet toQJsonObject() error";
            return(false);
        }
        qJsonObject_exportFileContent.insert("imagesSet_geoRefInfos", qjsonObj_geoRefInfos);

        qJsonObject_exportFileContent.insert("bUseGeoRef", _bUseGeoRefForProject);


        QByteArray QByteArray_ascii;
        bool bReport = qJsonObject_to_asciiFormat_sectionWithIdentation(qJsonObject_exportFileContent, QByteArray_ascii);
        if (!bReport) {
            qDebug() << __FUNCTION__ << " error:qJsonObject_to_asciiFormat_sectionWithIdentation #1 failed";
            _strDetailsAboutFail_ProjectWrite = "Dev Error #8_4 qJsonObject_to_asciiFormat_sectionWithIdentation #1 error";
            return(false);
        }

        //QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
        qint64 wroteBytes = outFile.write(QByteArray_ascii);
        if (wroteBytes <= 0) {
             qDebug() << "error: writing byte to file " << strOutFileName;
             return(false);
        }
    }

    Route& refRoute1 = _routeContainer.getRouteRef(0);
    //refRoute1.showContent();
    if (refRoute1.getId() == -1) {
        qDebug() << __FUNCTION__ << " error: route is invalid ";
        _strDetailsAboutFail_ProjectWrite = "trace is invalid";
        return(false);
    }

    {
        QJsonObject qJsonObject_exportFileContent;

        QJsonObject qjsonObj_inputFiles;

        //writing a project consider inputFiles valid and routeContainer with at least one route (should be only one)
        //write input files from the AppModel, the computationCore could not be feed with it

        S_InputFilesWithImageFilesAttributes sInputFilesWithImageFilesAttributes;
        bool bAttrGotFromFile = sInputFilesWithImageFilesAttributes.fromInputFiles(_inputFiles);
        if (!bAttrGotFromFile) {
            qDebug() << __FUNCTION__ << " error: failed to got attributes from an input file";
            _strDetailsAboutFail_ProjectWrite = "error: failed to got attributes from an input file";
            return(false);
        }
        sInputFilesWithImageFilesAttributes.showContent(); //@LP debug

        sInputFilesWithImageFilesAttributes.toQJsonObject(qjsonObj_inputFiles); //@#LP could requiered eMainComputationMode to know how to save correlation map usage (PX1PX2 together)

        QString strKey_inputFiles { "inputFiles" };
        qJsonObject_exportFileContent.insert(strKey_inputFiles, qjsonObj_inputFiles);

        //route; this has to be a routeset with only one route
        if (_routeContainer.routeCount() > 1) {
            qDebug() << __FUNCTION__ << " error: _routeContainer routeCount > 1";
            _strDetailsAboutFail_ProjectWrite = "trace count > 1"; //if the user edit/hack the project file, this is not a 'Dev Error'
            return(false);
        }

        if (_projectContentState.getState_route() != e_Project_routeContentState::e_P_routeCS_oneRouteValid) {
            qDebug() << __FUNCTION__ << " error: route is in a invalid state. Can not save project.";
            _strDetailsAboutFail_ProjectWrite = "trace is in an invalid state";
            return(false);
        }

        //route name
        QString str_routeName_key {"routeName"};
        /*Route& refRoute1 = _routeContainer.getRouteRef(0);
        //refRoute1.showContent();
        if (refRoute1.getId() == -1) {
            qDebug() << __FUNCTION__ << " error: route is invalid ";
            _strDetailsAboutFail_ProjectWrite = "trace is invalid";
            return(false);
        }*/
        qJsonObject_exportFileContent.insert(str_routeName_key, refRoute1.getName());

        //@#LP namming orientationFault vs useRouteInReversePointOrder

        //micmac step value and spatial resolution
        QJsonObject qjsonObj_micmacStepValues_and_spatialResolutions;
        QString _tqstrLayerJSonDescName[3] {"PX1", "PX2", "ZOther"};
        for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
            QJsonObject qjsonObj_micmacSV_and_sR_byLayer;
            if (!_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela].isEmpty()) { //@#LP add and replace with a QVector<bool> layer_to_compute ?
                bool bQjsonObj_micmacSV_and_sR_byLayerGot = _vectDequantization_PX1_PX2_deltaZ[iela].toQJsonObject(qjsonObj_micmacSV_and_sR_byLayer);
                if (!bQjsonObj_micmacSV_and_sR_byLayerGot) {
                    qDebug() << __FUNCTION__ << " error: stackedProfiBoxes_toQJsonArray_ failed";
                    _strDetailsAboutFail_projectWrite = "invalid micmacStep or spatial Resolution for " + _tqstrLayerJSonDescName[iela];
                    return(false);
                }
                qjsonObj_micmacStepValues_and_spatialResolutions.insert(_tqstrLayerJSonDescName[iela], qjsonObj_micmacSV_and_sR_byLayer);
            }
        }
        qJsonObject_exportFileContent.insert("pixelValueResolution", qjsonObj_micmacStepValues_and_spatialResolutions);

        QJsonObject qjsonObj_computationMainMode;
        QString str_computationMainMode_key {"computationMainMode"};

        qJsonObject_exportFileContent.insert(str_computationMainMode_key, static_cast<int>(_computationCore_iqaTableModel.getMainComputationMode()));

        bool bComputationParametersAvailable =
                (_projectContentState.getState_computationParameters() != e_Project_computationParametersState::e_P_computationParametersS_notSet);
        if (bComputationParametersAvailable) {
            QJsonObject qjsonObj_computationSettings;
            _computationCore_iqaTableModel.computationParameters_toQJsonObject(qjsonObj_computationSettings);
            QString strKey_computationSettings { "computationParameters" };
            qJsonObject_exportFileContent.insert(strKey_computationSettings, qjsonObj_computationSettings);
        }

        QByteArray QByteArray_ascii;
        bool bReport = qJsonObject_to_asciiFormat_sectionWithIdentation(qJsonObject_exportFileContent, QByteArray_ascii);
        if (!bReport) {
            qDebug() << __FUNCTION__ << " error:qJsonObject_to_asciiFormat_sectionWithIdentation #1 failed";
            _strDetailsAboutFail_ProjectWrite = "Dev Error #8_5 qJsonObject_to_asciiFormat_sectionWithIdentation #1 error";
            return(false);
        }

        //QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
        qint64 wroteBytes_1 = outFile.write("\n");
        qint64 wroteBytes_2 = outFile.write(QByteArray_ascii);
        if ((wroteBytes_1 <= 0)||(wroteBytes_2 <= 0)) {
             qDebug() << "error: writing byte to file " << strOutFileName;
             return(false);
        }
    }


    QVector<QString> qvectQString_componentTitle {"Perp", "Parall", "deltaZ"};
    QVector<bool> qvectComputedComponents = _computationCore_iqaTableModel.get_qvectComputedComponents();
    if (qvectComputedComponents.size() < 3) {
        return(false);
    }

    bool bOneOrMoreBoxes = (_projectContentState.getState_boxes() == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
    if (bOneOrMoreBoxes) {
        qint64 wroteBytes_commentAboutBoxIdOrder = outFile.write("\nBoxes are ordered along the trace, taking into account the selected profil orientation. Azimuth degree and unit vector direction values also.");
        if (wroteBytes_commentAboutBoxIdOrder <= 0) {
             qDebug() << "error: writing byte to file " << strOutFileName;
             return(false);
        }
    }

    //measurement results by components for each box, with associated linear regression results and parameters, and box parameters
    for (int ieComponentsAccess = e_CA_Perp; ieComponentsAccess <= e_CA_deltaZ; ieComponentsAccess++) {

        if (!qvectComputedComponents[ieComponentsAccess]) {
            continue;
        }
        QStringList strlist_headerComponent = {"component", qvectQString_componentTitle[ieComponentsAccess]};
        QString str_headerComponent = strlist_headerComponent.join(";");
        qint64 wroteBytes_crlf = outFile.write("\n");
        qint64 wroteBytes_headerComponent = outFile.write(str_headerComponent.toStdString().c_str());
        if ((wroteBytes_crlf <= 0)||(wroteBytes_headerComponent <= 0)) {
             qDebug() << "error: writing byte to file " << strOutFileName;
             return(false);
        }


        if (bOneOrMoreBoxes) {

            e_ProfilOrientation eProfilOrientation = _computationCore_iqaTableModel.getProgressDirection();
            if (eProfilOrientation == e_PO_notSet) {
                return(false);
            }

            int nbBoxes = _computationCore_iqaTableModel.getBoxCount();

            bool bReverseOrderInArray = false;

            int idxBox_ = 0;
            int idxBoxProgressIncrement_ = +1;

            if (eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {
                bReverseOrderInArray = true;
                idxBox_ = nbBoxes -1;
                idxBoxProgressIncrement_ = -1;
            }

            //if export only the current box, set on it, and set the increment to be over the boxId range
            if (sExportSettings._ieExportResult_boxesContent == e_ERbC_onlyCurrent) {
                idxBox_ = sExportSettings._idOfCurrentBox;
                idxBoxProgressIncrement_ *= nbBoxes;
                if ((idxBox_ < 0)||(idxBox_ >= nbBoxes)) {
                    _strDetailsAboutFail_ProjectWrite = "invalid current Box Id";
                    return(false);
                }
            }


            //---------------
            // column bloc #1: box id
            //---------------
            //
            QStringList strlist_headerBloc1 = {"boxId"};

            //--------------
            //column bloc #2 : box data
            //--------------
            /*
            "oddPixelLength",
            "oddPixelWidth",
            "centerPoint_lat", "centerPoint_lon",
            "distanceFromFirstPointOfTrace_in_meter" or "distanceFromFirstPointOfTrace_in_pixel",
            "azimuthDegree",
            "centerPoint_pixel_x", "centerPoint_pixel_y",
            "unitVectorDirection_x", "unitVectorDirection_y"
            */

            /*
            QStringList strlist_headerBloc2 = {
                "oddPixelLength",
                "oddPixelWidth",
                "centerPoint_lat", "centerPoint_lon",
                "distanceFromFirstPointOfTrace_in_meter",
                "azimuthDegree",
                "centerPoint_pixel_x", "centerPoint_pixel_y",
                "unitVectorDirection_x", "unitVectorDirection_y"
            };
            */

            QStringList strlist_headerBloc2_part1 = {
                "oddPixelLength",
                "oddPixelWidth",
                "centerPoint_lat", "centerPoint_lon", //"N/A" will be set as value for project not using georeferencing
            };

            QStringList strlist_headerBloc2_part2;
            if (_bUseGeoRefForProject) {
                strlist_headerBloc2_part2.append("distanceFromFirstPointOfTrace_in_meter");
            } else {
                strlist_headerBloc2_part2.push_back("distanceFromFirstPointOfTrace_in_pixel");
            }

            QStringList strlist_headerBloc2_part3 = {
                "azimuthDegree",
                "centerPoint_pixel_x", "centerPoint_pixel_y",
                "unitVectorDirection_x", "unitVectorDirection_y"
            };

            QStringList strlist_headerBloc2 =
                  strlist_headerBloc2_part1
                + strlist_headerBloc2_part2
                + strlist_headerBloc2_part3;

            //--------------
            //column bloc #3 : LinearRegressionModels results data
            //--------------
            QStringList strlist_headerBloc3 = { //@LP get the headerbloc from according struct/class (using static method in acccording struct/class)

                "bNoWarningFlagByUser", //Reverse the meaning anf the value (from  bWarningFlagByUser) to have 'true' as 'OK' like many others true value in the export.
                                        //Instead of having false due to if using bWarningFlagByUser

                "linearRegression_isComputed",
                "yOffsetAtX0",
                "sigmabSum",

                "leftSide_isComputed",
                "leftSide_a_slope",
                "leftSide_b_intercept",

                "leftSide_stdErrorOfSlope",
                "leftSide_stdErrorOfIntercept",
                "leftSide_stdErrorOfTheRegression",
                "leftSide_sumsq",


                "rightSide_isComputed",
                "rightSide_a_slope",
                "rightSide_b_intercept",

                "rightSide_stdErrorOfSlope",
                "rightSide_stdErrorOfIntercept",
                "rightSide_stdErrorOfTheRegression",
                "rightSide_sumsq",
            };

            //--------------
            //column bloc #4 : LinearRegressionModels parameters data
            //--------------
            QStringList strlist_headerBloc4 = { //@LP get the headerbloc from according struct/class (using static method in acccording struct/class)
                "leftSide_xMin",
                "leftSide_xMax",
                "rightSide_xMin",
                "rightSide_xMax",
                "X0"
            };

            QString str_headerBloc_1324 = strlist_headerBloc1.join(";") + ";"
                                        + strlist_headerBloc3.join(";") + ";"
                                        + strlist_headerBloc2.join(";") + ";"
                                        + strlist_headerBloc4.join(";");

            qint64 wroteBytes_crlf = outFile.write("\n");
            qint64 wroteBytes_header1324 = outFile.write(str_headerBloc_1324.toStdString().c_str());
            if ((wroteBytes_crlf <= 0)||(wroteBytes_header1324 <= 0)) {
                 qDebug() << "error: writing byte to file " << strOutFileName;
                 return(false);
            }

            bool bExportGeo = _bUseGeoRefForProject;
            bool bExportDistanceOfBoxCenterFromStartOfTheRoute = true;

            {

                int idxBox = idxBox_;
                int idxBoxProgressIncrement = idxBoxProgressIncrement_;

                //boxID stay like in the project,
                //if the progress Direction is from last to first point of the route:
                // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
                // - we write the box in the reverse order of the qvect
                for (; _computationCore_iqaTableModel.vectBoxIdValid(idxBox); idxBox+=idxBoxProgressIncrement) {

                    //get the SProfilsBoxParameters of the box
                    S_ProfilsBoxParameters SProfilsBoxParameters;
                    bool bReport_stackedProfilBoxGot =
                            _computationCore_iqaTableModel.get_oneStackedProfilBoxParameters(idxBox, SProfilsBoxParameters);
                    qDebug() << __FUNCTION__ << "bReport_stackedProfilBoxGot = " << bReport_stackedProfilBoxGot;

                    if (!bReport_stackedProfilBoxGot) {
                        qDebug() << __FUNCTION__ << "bReport_stackedProfilBoxGot said failed";
                        return(false);
                    }

                    //reverse the unit vector direction if requiered
                    if (eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {
                        //reverse
                        SProfilsBoxParameters._unitVectorDirection = -SProfilsBoxParameters._unitVectorDirection;
                    }

                    //---------------
                    // column bloc #1 BoxId data
                    //---------------
                    QStringList strlist_dataBloc1 = {QString::number(idxBox+1)}; //in ascii export file, boxId start at 1 like in User interface

                    //--------------
                    //column bloc #2 : box data
                    //--------------
                    QStringList strlist_dataBloc2;

                    //oddPixelLength
                    //oddPixelWidth
                    strlist_dataBloc2.push_back(QString::number(SProfilsBoxParameters._oddPixelLength));
                    strlist_dataBloc2.push_back(QString::number(SProfilsBoxParameters._oddPixelWidth));

                    //centerPoint lat, lon
                    if (bExportGeo) {
                        if (!_ptrGeoRefModel_imagesSet) {
                            qDebug() << __FUNCTION__ << "_ptrGeoRefModel_imagesSet is nullptr";
                            return(false);
                        } else {
                            S_lonlat lonlat {.0, .0};
                            bool bReport_projXYtoLonLat = _ptrGeoRefModel_imagesSet->projXY_fromQcs_toLonLat(SProfilsBoxParameters._qpfCenterPoint, lonlat);
                            if (!bReport_projXYtoLonLat) {
                                qDebug() << __FUNCTION__ << "bReport_projXYtoLonLat: " <<  bReport_projXYtoLonLat;
                                return(false);
                            }

                            strlist_dataBloc2.push_back(doubleToQStringPrecision_f_amountOfDecimal(lonlat._lat,7));
                            strlist_dataBloc2.push_back(doubleToQStringPrecision_f_amountOfDecimal(lonlat._lon,7));
                        }
                    } else {
                        strlist_dataBloc2.push_back("N/A");
                        strlist_dataBloc2.push_back("N/A");
                    }

                    if (bExportDistanceOfBoxCenterFromStartOfTheRoute) {

                        qreal pixel_distanceFromStartingRoutePointToRoutePoint = .0;

                        bool bDistanceComputed = refRoute1.computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection(
                            SProfilsBoxParameters._qpfCenterPoint,
                            SProfilsBoxParameters._idxSegmentOwnerOfCenterPoint,
                            pixel_distanceFromStartingRoutePointToRoutePoint);

                        if (!bDistanceComputed) {
                            qDebug() << __FUNCTION__ << "computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection failed";
                            return(false);
                        }

                        qDebug() << __FUNCTION__ << "Distance_alongRoute_fromStartingRoutePointToARoutePoint :";
                        qDebug() << __FUNCTION__ << "  idxBox = " << idxBox;
                        qDebug() << __FUNCTION__ << "  _qpfCenterPoint = " << SProfilsBoxParameters._qpfCenterPoint;
                        qDebug() << __FUNCTION__ << "  _idxSegmentOwnerOfCenterPoint = " << SProfilsBoxParameters._idxSegmentOwnerOfCenterPoint;
                        qDebug() << __FUNCTION__ << "  pixel_distanceFromStartingRoutePointToRoutePoint = " << pixel_distanceFromStartingRoutePointToRoutePoint;

                        if (bExportGeo) {
                            qreal WF_distanceFromStartingRoutePointToRoutePoint = .0;
                            bool bconvertDistance_toWF = _ptrGeoRefModel_imagesSet->
                                    convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
                                        pixel_distanceFromStartingRoutePointToRoutePoint,
                                        WF_distanceFromStartingRoutePointToRoutePoint);

                            if (!bconvertDistance_toWF) {
                                qDebug() << __FUNCTION__ << "convertDistance_fromQcs_toWF_withSameXScaleAndYScale failed";
                                return(false);
                            }

                            qDebug() << __FUNCTION__ << "WF_distanceFromStartingRoutePointToRoutePoint = " << WF_distanceFromStartingRoutePointToRoutePoint;

                            /*qreal qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision = .0;
                            bool bOK = true;
                            bOK &= qrealToRealNumberFixedPrecision_f(WF_distanceFromStartingRoutePointToRoutePoint, 3, qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision);
                            if (!bOK) {
                                qDebug() << __FUNCTION__ << "WF_distance; qrealToRealNumberFixedPrecision_f: " <<  bOK;
                                return(false);
                            }
                            QJsonObject qjsonObj_centerPoint_distanceFromStartingRoutePoint_meter;
                            qjsonObj_centerPoint_distanceFromStartingRoutePoint_meter.insert(
                                "meter",
                                qrWFDistanceFromStartingRoutePointToRoutePoint_fixedPrecicision);

                            QString strKey_centerPoint_distanceFromStartingRoutePoint = "distanceFromFirstPointOfTrace";
                            qjsonObj_stackedProfilBoxParametersWithoutBoxSetInfos.insert(
                                strKey_centerPoint_distanceFromStartingRoutePoint,
                                qjsonObj_centerPoint_distanceFromStartingRoutePoint_meter);
                            */

                            strlist_dataBloc2.push_back(doubleToQStringPrecision_f_amountOfDecimal(WF_distanceFromStartingRoutePointToRoutePoint,3));

                        } else {

                            strlist_dataBloc2.push_back(doubleToQStringPrecision_f_amountOfDecimal(pixel_distanceFromStartingRoutePointToRoutePoint,3));

                        }
                    }

                    //azimuth
                    //compute the azimuth FromTopVertical of the unit vector direction (reversed if requiered)
                    double azimuthDegreeFromTopVertical = convert_qcsVectorDirectionToAzimuthtFromTopVertical(SProfilsBoxParameters._unitVectorDirection);

                    strlist_dataBloc2.push_back(doubleToQStringPrecision_justFit(azimuthDegreeFromTopVertical,7));

                    //centerPoint_pixel
                    int outFixedPrecision_forCenterPoint = 7; //@#LP check when used 7 and 3
                    QString strCenterPointX = doubleToQStringPrecision_justFit(SProfilsBoxParameters._qpfCenterPoint.x(), outFixedPrecision_forCenterPoint);
                    QString strCenterPointY = doubleToQStringPrecision_justFit(SProfilsBoxParameters._qpfCenterPoint.y(), outFixedPrecision_forCenterPoint);
                    strlist_dataBloc2.push_back(strCenterPointX);
                    strlist_dataBloc2.push_back(strCenterPointY);

                    //unitVectorDirection
                    int outFixedPrecision_forUnitVectorDirection = 7; //@#LP check when used 7, 3 or other
                    QString strUnitVectorDirectionX = doubleToQStringPrecision_justFit(SProfilsBoxParameters._unitVectorDirection.x(), outFixedPrecision_forUnitVectorDirection);
                    QString strUnitVectorDirectionY = doubleToQStringPrecision_justFit(SProfilsBoxParameters._unitVectorDirection.y(), outFixedPrecision_forUnitVectorDirection);
                    strlist_dataBloc2.push_back(strUnitVectorDirectionX);
                    strlist_dataBloc2.push_back(strUnitVectorDirectionY);


                    //--------------
                    //column bloc #3 : LinearRegressionModels results data
                    //--------------
                    QStringList strlist_dataBloc3;
                    bool bReport = _computationCore_iqaTableModel.stackedProfilLinearRegressionModelResult_forBoxId_andComponent_toASciiStringlist(
                                    idxBox, static_cast<e_ComponentsAccess>(ieComponentsAccess),
                                    sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty,
                                    strlist_dataBloc3);
                    if (!bReport) {
                        return(false);
                    }


                    //--------------
                    //column bloc #4 : LinearRegressionModels parameters data
                    //--------------
                    QStringList strlist_dataBloc4;
                    bReport = _computationCore_iqaTableModel.stackedProfilLinearRegressionParametersWithoutResult_forBoxId_andComponent_toASciiStringlist(
                                    idxBox, static_cast<e_ComponentsAccess>(ieComponentsAccess), strlist_dataBloc4);
                    if (!bReport) {
                        return(false);
                    }

                    QString str_dataBloc_1324 = strlist_dataBloc1.join(";") + ";"
                                              + strlist_dataBloc3.join(";") + ";"
                                              + strlist_dataBloc2.join(";") + ";"
                                              + strlist_dataBloc4.join(";");

                    qDebug() << __FUNCTION__ << str_dataBloc_1324;

                    wroteBytes_crlf = outFile.write("\n");
                    qint64 wroteBytes_data1324 = outFile.write(str_dataBloc_1324.toStdString().c_str());
                    if ((wroteBytes_crlf <= 0)||(wroteBytes_data1324 <= 0)) {
                         qDebug() << "error: writing byte to file " << strOutFileName;
                         return(false);
                    }
                } //box loop
            } //sub code section
        } //if boxes

        wroteBytes_crlf = outFile.write("\n");
        if (wroteBytes_crlf <= 0) {
            qDebug() << "error: writing byte to file " << strOutFileName;
            return(false);
        }
    }

    //write each profil curves content (depending on computed components):
    //by profil
    //   x, Perp, Parall
    //or x, deltaZOrOther
    //or x, Perp, Parall, deltaZOrOther

    if (sExportSettings._bIncludeProfilesCurvesData) {

        bool bOneOrMoreBoxes = (_projectContentState.getState_boxes() == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
        if (bOneOrMoreBoxes) {

            e_ProfilOrientation eProfilOrientation = _computationCore_iqaTableModel.getProgressDirection();
            if (eProfilOrientation == e_PO_notSet) {
                return(false);
            }

            int nbBoxes = _computationCore_iqaTableModel.getBoxCount();

            bool bReverseOrderInArray = false;

            int idxBox_ = 0;
            int idxBoxProgressIncrement_ = +1;

            if (eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {
                bReverseOrderInArray = true;
                idxBox_ = nbBoxes -1;
                idxBoxProgressIncrement_ = -1;
            }

            //if export only the current box, set on it, and set the increment to be over the boxId range
            if (sExportSettings._ieExportResult_boxesContent == e_ERbC_onlyCurrent) {
                idxBox_ = sExportSettings._idOfCurrentBox;
                idxBoxProgressIncrement_ *= nbBoxes;
                if ((idxBox_ < 0)||(idxBox_ >= nbBoxes)) {
                    _strDetailsAboutFail_ProjectWrite = "invalid current Box Id";
                    return(false);
                }
            }

            qint64 wroteBytes_profilCurvesTitleSection = outFile.write("\nprofilCurves\n");
            if (wroteBytes_profilCurvesTitleSection <= 0) {
                qDebug() << "error: writing byte to file " << strOutFileName;
                return(false);

            }

            int idxBox = idxBox_;
            int idxBoxProgressIncrement = idxBoxProgressIncrement_;
            for (; _computationCore_iqaTableModel.vectBoxIdValid(idxBox); idxBox+=idxBoxProgressIncrement) {

                //---------------
                // column bloc #1: box id
                //---------------
                //
                QVector<QStringList> qvectQStringList_blocs_boxId_headersProfils;
                qvectQStringList_blocs_boxId_headersProfils.push_back( {"boxId", QString::number(idxBox+1)}); //in ascii export file, boxId start at 1 like in User interface

                //colums title
                QStringList qstrList_profilCurvesHeader_bloc2;
                QStringList qstrList_profilCurvesHeader_bloc3;
                bool bReport_profilCurvesQjsonObjByBox = _computationCore_iqaTableModel.computedComponentsToHeader_toAscii(qstrList_profilCurvesHeader_bloc2,
                                                                                                                           qstrList_profilCurvesHeader_bloc3);
                if (!bReport_profilCurvesQjsonObjByBox) {
                    return(false);
                }

                qvectQStringList_blocs_boxId_headersProfils.push_back(qstrList_profilCurvesHeader_bloc2);
                qvectQStringList_blocs_boxId_headersProfils.push_back(qstrList_profilCurvesHeader_bloc3);

                for (int iBloc = 0; iBloc < 3; iBloc++) {
                     qint64 wroteBytes_crlf = outFile.write("\n");
                     QString qstrBlocToWrite = qvectQStringList_blocs_boxId_headersProfils.at(iBloc).join(";");
                     qint64 wroteBytes_strProfilCurvesHeader_bloc = outFile.write(qstrBlocToWrite.toStdString().c_str());
                     if ((wroteBytes_crlf <= 0)||(wroteBytes_strProfilCurvesHeader_bloc <= 0)) {
                         qDebug() << "error: writing byte to file " << strOutFileName;
                         return(false);
                     }
                }

                //profil curves content
                //QJsonArray qjsonArray_profilCurvesForAllStackedProfilBoxes;

                //boxID stay like in the project,
                //if the progress Direction is from last to first point of the route:
                // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
                // - we write the box in the reverse order of the qvect

                QVector<QStringList> qvectQstrList_profilCurveContent;

                bool bReport_profilCurvesContentQjsonObjByBox =
                        _computationCore_iqaTableModel.profilCurvesForOneStackedProfilBox_toAscii(idxBox,
                                                                                                  sExportSettings._bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                                                                                                  qvectQstrList_profilCurveContent);
                if (!bReport_profilCurvesContentQjsonObjByBox) {
                    return(false);
                }
                for (auto& iStrList: qvectQstrList_profilCurveContent) {

                    QString strProfilCurveContent_forOneX = iStrList.join(";");

                    qint64 wroteBytes_crlf = outFile.write("\n");
                    qint64 wroteBytes_strProfilCurveContent_oneProfil = outFile.write(strProfilCurveContent_forOneX.toStdString().c_str());
                    if ((wroteBytes_crlf <= 0) || (wroteBytes_strProfilCurveContent_oneProfil <= 0)) {
                        qDebug() << "error: writing byte to file " << strOutFileName;
                        return(false);
                    }
                }
            }
        }
    }
    return(true);
}

bool ApplicationModel::compute_leftRightSidesLinearRegressionsModels_forAllBoxes() {

    bool bReport = _computationCore_iqaTableModel.compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
    return(bReport);
}

