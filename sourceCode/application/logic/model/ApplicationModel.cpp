#include "ApplicationModel.h"

#include "imageScene/customgraphicsscene_usingTPDO.h"
#include <QDebug>

//for debug only
#include <QMessageBox>

//for cursor wait
#include <QGuiApplication>

#include <OpenImageIO/imageio.h>
using namespace OIIO;
#include "../io/InputImageFormatChecker.h"

#include "../io/ZoomOutLevelImageBuilder.h"

#include "zoomLevelImage/zoomHandler.h"

#include "georef/georefModel_imagesSet.hpp"

#include "io/iojsoninfos.h"

#include "smallImageNavScene/smallimagenav_customgraphicsscene.h"
#include "tilesProvider/tileprovider_3.h"

#include "core/ComputationCore_inheritQATableModel.h"

#include "model/dialogProfilsCurves/CustomGraphicsScene_profil.h"
#include "model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp"

#include "StackedProfilInEdition.h"

#include "AppDiskPathCacheStorage/AppDiskPathCacheStorage.hpp"

#include <QCoreApplication>
#include <QDir>

#include <QElapsedTimer>

#include "core/valueDequantization.hpp"

#include "../toolbox/toolbox_pathAndFile.h"
#include "../toolbox/toolbox_json.h"

#include "mainAppState.hpp"

#include "toolbox/toolbox_math_geometry.h" //relocating point to addbox //@#LP maybe move this addbox process outside this file

#include "imageScene/S_ForbiddenRouteSection.h"

#include "../pathesAccess/InternalDataPathAccess.h"

#include "core/ComputationCore_structures.h"

S_boolAndStrErrorMessage_aboutLoad::S_boolAndStrErrorMessage_aboutLoad():_bFileDoesnotExistAtLocation(false) {
}

S_Fields_AddingBox::S_Fields_AddingBox():
    _qpfPointToUseAsBoxCenterPoint {.0, .0},
    _bValid_qpfPointToUseAsBoxCenterPoint { false },
    _idxSegmentOwner_ofQpfPointToUseAsBoxCenterPoint { -1 } {
    _sForbiddenRouteSectionsFromStartAndEnd.clear();
}

void S_Fields_AddingBox::clear() {
    _sForbiddenRouteSectionsFromStartAndEnd.clear();

    _qpfPointToUseAsBoxCenterPoint = {.0, .0};
    _bValid_qpfPointToUseAsBoxCenterPoint = { false };

    _idxSegmentOwner_ofQpfPointToUseAsBoxCenterPoint = { -1 };
}

//@LP error report just using a boolean, no error string
bool ApplicationModel::init_geoProj_onlyOneImageDisplayed(e_LayersAcces eLayerAccessForDisplayedImage) {

    qDebug() << __FUNCTION__ << "start";

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: if (!_ptrGeoRefModel_imagesSet)";
        return(false);
    }

    const QString strPathProjDB = getProjDBPath();

    bool bReport_geoRefModelImagesSet_createContext = _ptrGeoRefModel_imagesSet->createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    if (!_ptrGeoRefModel_imagesSet->createCrsToCrs_to_WGS84as4326()) {
        qDebug() << __FUNCTION__ << ": failed in createCrsToCrs_to_WGS84as4326()";
        return(false);
    }

    _geoUiInfos.setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_WGS84);
    _geoUiInfos.setPtrGeoRefModel_imagesSet(_ptrGeoRefModel_imagesSet);
    _geoUiInfos.setIndexLayerForValueDequantization(eLayerAccessForDisplayedImage);

    qDebug() << __FUNCTION__ << " done";
    return(true);
}

bool ApplicationModel::init_geoUiInfos_noGeoRefUsed_onlyOneImageDisplayed(e_LayersAcces eLayerAccessForDisplayedImage) {

    qDebug() << __FUNCTION__ << "start";

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: if (!_ptrGeoRefModel_imagesSet)";
        return(false);
    }

    _geoUiInfos.setCoordinateDisplayMode(GeoUiInfos::e_CoordinateDisplayMode::eCDM_pixelImageXY);
    _geoUiInfos.setPtrGeoRefModel_imagesSet(nullptr);
    _geoUiInfos.setIndexLayerForValueDequantization(eLayerAccessForDisplayedImage);

    qDebug() << __FUNCTION__ << " done";
    return(true);
}

//return false in case of fatal error (impossible to restore previous valid EPSG Code)
bool ApplicationModel::tryToChangeCurrentValidEPSGCode(const QString &strEPSGCode, bool& bChangeDone, QString &strErrorMsg) {

    qDebug() << __FUNCTION__ << "start";

    bChangeDone = false;
    strErrorMsg.clear();

    if (!_bUseGeoRefForTrace) {
        qDebug() << __FUNCTION__ << "if (!_bUseGeoRefForTrace) {";
        strErrorMsg = "internal dev-error: !_bUseGeoRefForTrace";
        return(false);
    }

    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << " error: if (!_ptrGeoRefModel_imagesSet)";
        strErrorMsg = "internal dev-error";
        return(false);
    }

    QString strCurrentValidEPSGCode = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();

    _ptrGeoRefModel_imagesSet->forceEPSGCode(strEPSGCode);
    bool bReport = init_geoProj_onlyOneImageDisplayed(eLA_PX1);

    if (!bReport) {
        _ptrGeoRefModel_imagesSet->forceEPSGCode(strCurrentValidEPSGCode);
        bReport = init_geoProj_onlyOneImageDisplayed(eLA_PX1);
        if (!bReport) {
            strErrorMsg = "internal error: failed to come back to previous EPSG Code ( " + strCurrentValidEPSGCode + " )";
            return(false);
        }
        strErrorMsg = "failed to set EPSG Code to " + strEPSGCode;
        return(true);
    }

    qDebug() << __FUNCTION__ << " done";

    setAppFileModifiedState(true);

    bChangeDone = true;
    return(true);
}



void ApplicationModel::getInputFilesAndCorrelationScoreFiles_withoutFlagsAboutCorrelScoreMapToUseForComp(S_InputFiles& inputFiles) const {
    inputFiles = _inputFiles;
}

bool ApplicationModel::setInputFilesAndCorrelationScoreFiles(const S_InputFiles& inputFiles,const QVector<bool>& qvectb_correlScoreMapToUseForComp) {
    if (qvectb_correlScoreMapToUseForComp.size() !=3 ) {
        return(false);
    }

   _inputFiles = inputFiles;
   _computationCore_iqaTableModel.setInputFilesAndCorrelationScoreFiles(_inputFiles);

   bool bReport = _computationCore_iqaTableModel.
           setcorrelScoreMapToUseForComp_checkInputFilesAccordingToMainComputationMode_andSetComputationLayersAndComponentFlags(
               qvectb_correlScoreMapToUseForComp);
   return(bReport);
}

bool ApplicationModel::set_micmacStepAndSpatialResolution_forModels(const QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                                                  const QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                                                  const QVector<bool>& qvectb_layersToCompute,
                                                  bool bClear_geoUiInfos_ieLayerAccesForValueDequantization,
                                                  bool bSetAppFileModifiedStateAtTrue) {

    bool bReport = setVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
             qvect_micmacStep_PX1_PX2_deltaZ,
             qvect_spatialResolution_PX1_PX2_deltaZ,
             qvectb_layersToCompute);

    if (!bReport) {
        qDebug() << __FUNCTION__ << " setVectDequantizationSteps_PX1_PX2_deltaZ failed";
        return(false);
    }

    /*qDebug() << __FUNCTION__ << " _vectValueDequantization_PX1_PX2_deltaZ:" ;
    for (auto iter: _vectValueDequantization_PX1_PX2_deltaZ) {
     qDebug() << __FUNCTION__ << "iter_loop  isDequantStepValid = " << iter.isDequantValid() << " dequant=" << iter.get_dequant();
    }*/

    _projectContentState.setState_spatialResolution(e_Project_spatialResolutionState::e_P_stackedProfilEditedS_changed);

    setPtrVectDequantization_forGeoUiInfosModel(bClear_geoUiInfos_ieLayerAccesForValueDequantization);

    if (bSetAppFileModifiedStateAtTrue) {
        setAppFileModifiedState(true);
    }

    return(true);
}

bool ApplicationModel::get_micmacStepAndSpatialResolution(QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                                                          QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
                                                          QVector<bool>& qvectb_layersToCompute) const {
    return(getVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
           qvect_micmacStep_PX1_PX2_deltaZ,
           qvect_spatialResolution_PX1_PX2_deltaZ,
           qvectb_layersToCompute));
}

void  ApplicationModel::setPtrVectDequantization_forGeoUiInfosModel(bool bClear_geoUiInfos_ieLayerAccesForValueDequantization) {
    _geoUiInfos.setPtrVectDequantization_PX1_PX2_deltaZ(&_vectDequantization_PX1_PX2_deltaZ, bClear_geoUiInfos_ieLayerAccesForValueDequantization);
}

bool ApplicationModel::setVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
        const QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
        const QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
        const QVector<bool>& qvectb_layersToCompute) {

    for (size_t iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        _vectDequantization_PX1_PX2_deltaZ[iela].clear();
    }

    int countTrue = qvectb_layersToCompute.count(true);
    if (!countTrue) {       
        qDebug() << __FUNCTION__ <<  "if (!countTrue)";
        return(true);
    }

    if (qvect_spatialResolution_PX1_PX2_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ <<  "if (qvect_spatialResolution_PX1_PX2_deltaZ.size() != 3)";
        return(false);
    }

    if (qvect_micmacStep_PX1_PX2_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ <<  "if (qvect_micmacStep_PX1_PX2_deltaZ.size() != 3)";
        return(false);
    }

    if (qvectb_layersToCompute.size() != 3) {
        qDebug() << __FUNCTION__ <<  " if (qvectb_layersToCompute.size() != 3)";
        return(false);
    }
    qDebug() << __FUNCTION__ << "qvectb_layersToCompute = " << qvectb_layersToCompute;
    qDebug() << __FUNCTION__ << "qvect_micmacStep_PX1_PX2_deltaZ = " << qvect_micmacStep_PX1_PX2_deltaZ;
    qDebug() << __FUNCTION__ << "qvect_spatialResolution_PX1_PX2_deltaZ = " << qvect_spatialResolution_PX1_PX2_deltaZ;

    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (qvectb_layersToCompute[iela]) {
            _vectDequantization_PX1_PX2_deltaZ[iela].set_micmacStep(qvect_micmacStep_PX1_PX2_deltaZ[iela]);
            _vectDequantization_PX1_PX2_deltaZ[iela].set_spatialResolution(qvect_spatialResolution_PX1_PX2_deltaZ[iela]);
        }
    }

    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        qDebug() << __FUNCTION__ <<  "_vectValueDequantization_PX1_PX2_deltaZ[" << iela << "].get_dequant() = " << _vectDequantization_PX1_PX2_deltaZ[iela].get_dequant();
    }

    return(true);
}


bool ApplicationModel::getVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
        QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
        QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ,
        QVector<bool>& qvectb_layersToCompute) const {

    qvect_micmacStep_PX1_PX2_deltaZ.fill(1.0,3);
    qvect_spatialResolution_PX1_PX2_deltaZ.fill(1.0,3);
    qvectb_layersToCompute.fill(false,3);

    qvectb_layersToCompute = _computationCore_iqaTableModel.get_qvectLayersInUse();

    if (!qvectb_layersToCompute.count(true)) {
        return(false);
    }

    for (size_t iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (qvectb_layersToCompute[iela]) {
            qvect_micmacStep_PX1_PX2_deltaZ[iela] = _vectDequantization_PX1_PX2_deltaZ[iela].get_micmacStep();
            qvect_spatialResolution_PX1_PX2_deltaZ[iela] = _vectDequantization_PX1_PX2_deltaZ[iela].get_spatialResolution();
        }
    }
    return(true);
}

ApplicationModel::ApplicationModel(): QObject(nullptr),
    _eAppModelState(eAModS_idle),
    _ptrGeoRefModel_imagesSet(nullptr),
    _qvectPtrCGScene_profil_Perp_Parall_deltaZ {nullptr, nullptr, nullptr},
    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ {nullptr, nullptr, nullptr},
    _qvectBoolComputedComponents_Perp_Parall_deltaZ {false, false, false},
    _qvectBoolComponentsToShow_Perp_Parall_deltaZ {false, false, false}
{

    //connect(&_watcherBool, &QFutureWatcher<bool>::finished, this, &ApplicationModel::zLIImageBuil_ended);

    QVector<QString> _qvectqstr_graphicUnits;

    //
    _qvectqstr_graphicTitle_Perp_Parall_deltaZ.resize(3);
    _qvectqstr_graphicTitle_Perp_Parall_deltaZ[e_CA_Perp  ] = "Perpendicular";
    _qvectqstr_graphicTitle_Perp_Parall_deltaZ[e_CA_Parall] = "Parallel";
    _qvectqstr_graphicTitle_Perp_Parall_deltaZ[e_CA_deltaZ] = "Delta Z";

    _qvectqstr_graphicUnits.append("pixel, meter");


    _vectDequantization_PX1_PX2_deltaZ.resize(3);
    setVectDequantization_micmacStepAndSpatialResolution_PX1_PX2_deltaZ({}, {}, {});

    _bStateAddADefaultRoute = false;

    _selectedRouteId = -1;

    _qsizeMax_smallImageNav = {320,320};    

    //@##LP layers allocated when never used

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
         CustomGraphicsScene_profil *ptrCGSCene_profil = new CustomGraphicsScene_profil(dynamic_cast<QObject*>(this));
         _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA] = ptrCGSCene_profil;
         qDebug() << __FUNCTION__ << "loop: _qvectPtrCGScene_profil_PX1_PX2_deltaZ = " << _qvectPtrCGScene_profil_Perp_Parall_deltaZ;

    }

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        connect(_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA], &CustomGraphicsScene_profil::signal_xAdjusterPosition_changed,
            this, &ApplicationModel::slot_xAdjusterPosition_changed_fromGraphics);
    }

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        connect(_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieCA], &CustomGraphicsScene_profil::signal_centerX0AdjusterPosition_changed,
            this, &ApplicationModel::slot_centerX0Position_changed_fromGraphics);
    }

    qDebug() << __FUNCTION__ << "_qvectPtrCGScene_profil_Perp_Parall_deltaZ = " << _qvectPtrCGScene_profil_Perp_Parall_deltaZ;

    //@##LP layers allocated when never used

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
         StackProfilEdit_textWidgetPartModel *ptrStackProfilEdit_textWidgetPart = new StackProfilEdit_textWidgetPartModel(dynamic_cast<QObject*>(this));
         _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA] = ptrStackProfilEdit_textWidgetPart;
         qDebug() << __FUNCTION__ << "loop: _qvectPtrStackProfilEdit_textWidgetPartModel_PX1_PX2_deltaZ = " << _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ;

    }

    for(int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
         connect(_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA],
                 &StackProfilEdit_textWidgetPartModel::signal_adjustersSetPositionValues_changed_fromTextEdition,
                 this, &ApplicationModel::slot_xAdjustersSetPositionValues_changed_fromTextEdition);

         connect(_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA],
                 &StackProfilEdit_textWidgetPartModel::signal_X0SetPositionValue_fromTextEdition/*and_adjustersSetPositionValues_changed_fromTextEdition*/,
                 this, &ApplicationModel::slot_centerX0SetPositionValue_changed_fromTextEdition/*_and_adjustersSetPositionValues*/);

         connect(_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieCA],
                 &StackProfilEdit_textWidgetPartModel::signal_setWarnFlag_fromTextEdition,
                 this, &ApplicationModel::slot_setWarnFlag_fromTextEdition);
    }



     qDebug() << __FUNCTION__ << "_qvectPtrStackProfilEdit_textWidgetPartModel_PX1_PX2_deltaZ = " << _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ;

     _currentBoxIdInVectOrderAlongRoute = -1;

     //_currentMeasureIndex at a cleared state from its default constructor)
     _bStackProfilEdit_syncYRange = false;

     _qszInputImageWidthHeight = {1.0,1.0};

     _bFeedValid_qhash_shift = false;

     _oiio_cacheAttribute_maxMemoryMB = 256; //smaller than 512 as a 'unset valid value'

     connect(&_computationCore_iqaTableModel, &ComputationCore_inheritQATableModel::signal_stepProgressionForOutside,
             this, &ApplicationModel::slot_stepProgressionFromComputationCore);  //@#LP signal->slot->signal for dev check purpose

     _eProfilAdjustMode = e_PAM_leftRight;

     _selectedRouteId_forRouteImport = -1;

     //inform ApplicationModel when a routeset is loaded
     connect(&_routeContainer, &RouteContainer::signal_routeCountAdded, this, &ApplicationModel::slot_routesetEditionFunction_routeCountAdded);
     connect(&_routeContainer, &RouteContainer::signal_routeRemovedSuccessfully, this, &ApplicationModel::slot_routesetEditionFunction_routeRemovedSuccessfully);
     connect(&_routeContainer, &RouteContainer::signal_routeAddedSuccessfully, this, &ApplicationModel::slot_routesetEditionFunction_routeAddedSuccessfully);

}

void ApplicationModel::slot_routesetEditionFunction_routeCountAdded(int routeCount) {

    _graphicsItemContainer.routeSet_route_showAlone(-1, false);

    emit signal_routeCountAfterFeed(routeCount);

}

void ApplicationModel::slot_routesetEditionFunction_routeRemovedSuccessfully(int routeCountAfterRemove, int idRouteRemoved) {

    setAppFileModifiedState(true);

    _graphicsItemContainer.removeRouteWithRouteIdReorder(idRouteRemoved);

    int newSelectedRoute = -1;

    if (!routeCountAfterRemove) {
        newSelectedRoute = -1;
    } else {
       if ((idRouteRemoved >=0 ) && (idRouteRemoved < routeCountAfterRemove)) {
           newSelectedRoute = idRouteRemoved;
       } else {
           newSelectedRoute = idRouteRemoved-1;
       }
    }

    _graphicsItemContainer.routeSet_route_showAlone(newSelectedRoute);
    emit signal_routeRemovedSuccessfully(routeCountAfterRemove, newSelectedRoute);

}

void ApplicationModel::slot_routesetEditionFunction_routeAddedSuccessfully(int idRouteAdded) {
    if (!_ptrCustomGraphicsScene) {
        return;
    }
    _ptrCustomGraphicsScene->routeSet_route_showAlone(idRouteAdded);
    emit signal_routeAddedSuccessfully(idRouteAdded);
}




void ApplicationModel::setState_editingRoute(e_AppModelState eAppModelState, int ieRouteEditionState, int routeId) {

    qDebug() << __FUNCTION__;

    if (!_routeContainer.checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << " error: routeId ( " << routeId << " )";
        return;
    }
    _eAppModelState = eAppModelState;
    _selectedRouteId = routeId;

    qDebug() << __FUNCTION__ << " _selectedRouteId <= " << routeId;
    qDebug() << __FUNCTION__ << " ieRouteEditionState <= " << ieRouteEditionState;

    if (_ptrCustomGraphicsScene) {
        _ptrCustomGraphicsScene->setState_editingRoute(ieRouteEditionState, _selectedRouteId);
    }
}

void ApplicationModel::routeSet_route_showAlone(int routeId, bool bShowAlone) {

    if (_ptrCustomGraphicsScene) {
        _ptrCustomGraphicsScene->routeSet_route_showAlone(routeId, bShowAlone);
    }

}


AppFile ApplicationModel::getAppFile_routeset() {
    return(_appFile_routeset);
}

AppFile ApplicationModel::getAppFile_project() {
    return(_appFile_project);
}

//---
//@#LP refactor to use the model instead
//called only from controller when some files for the project were replaced due to input file location changes
//In all others cases, this is in the model that the appFiles state are triggered to true
void ApplicationModel::setAppFile_project_asModified() {
    _appFile_project.setState_modified(true);
}
//---

void ApplicationModel::sendStr_worldFileDataToView() {
    qDebug() << __FUNCTION__ ;
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << "error: _ptrGeoRefModel_imagesSet is nullptr";
        return;
    }
   _ptrGeoRefModel_imagesSet->sendStrWorldFileDataToView();
}

void ApplicationModel::sendStr_imagesFilename() {
    qDebug() << __FUNCTION__ ;
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << "error: _ptrGeoRefModel_imagesSet is nullptr";
        return;
    }
   _ptrGeoRefModel_imagesSet->sendImagesFilenameToView();
}

void ApplicationModel::sendEPSGCodeToView() {
    qDebug() << __FUNCTION__ ;
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << "error: _ptrGeoRefModel_imagesSet is nullptr";
        return;
    }
   _ptrGeoRefModel_imagesSet->sendEPSGCodeToView();
}

void ApplicationModel::sendAssociatedNameToEPSGCodeToView() {
    QString qstrEPSGCode_fromLoadedRouteSet = _ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
    qstrEPSGCode_fromLoadedRouteSet = qstrEPSGCode_fromLoadedRouteSet.remove("EPSG:");
    QString strAssociatedNameToEPSGCode;
    /*bool bReportEPSGCodeIsKnown_geoCRSSelect_inheritQATableModel =*/
    _geoCRSSelect_inheritQATableModel.checkStrEPSGCodeisKnown(qstrEPSGCode_fromLoadedRouteSet, strAssociatedNameToEPSGCode);
    //---------
    emit signal_sendAssociatedNameToEPSGCode(strAssociatedNameToEPSGCode);
}

void ApplicationModel::send_vectComputedComponentsInUse_toView() {
    qDebug() << __FUNCTION__ ;
   _computationCore_iqaTableModel.send_vectComputedComponentsInUse_toView();
}

bool ApplicationModel::setDefaultComputationModeAndParameters_ifNotSet() {
    bool bSomeWasNotSet = _computationCore_iqaTableModel.setDefaultComputationModeAndParameters_ifNotSet();
    if (bSomeWasNotSet) {
        _projectContentState.setState_computationParameters(e_Project_computationParametersState::e_P_computationParametersS_changed);
    } else {
        _projectContentState.setState_computationParameters(e_Project_computationParametersState::e_P_computationParametersS_set);
    }
    return(bSomeWasNotSet);
}

void ApplicationModel::setIndexLayerForValueDequantization(size_t ieLayerAcces) {
    if ((ieLayerAcces < eLA_PX1)||(ieLayerAcces > eLA_deltaZ)) {
        return;
    }
    _geoUiInfos.setIndexLayerForValueDequantization(ieLayerAcces);
}

bool ApplicationModel::setProfilOrientation(e_ProfilOrientation eProfilOrientation, bool& bReverseDoneOnBoxesContent, bool& bRejected_alreadyOnThisOrientation) {

    bool bNoFatalErrorReversing = false;
    bNoFatalErrorReversing = _computationCore_iqaTableModel.setProgressDirection(eProfilOrientation, bReverseDoneOnBoxesContent, bRejected_alreadyOnThisOrientation);

    if (!bNoFatalErrorReversing) { //fatal error, unsync content
        if (!bRejected_alreadyOnThisOrientation) {
            if (bReverseDoneOnBoxesContent) {
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ.clear();
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ.resize(3);
            }
        }
    } else {
        _appFile_project.setState_modified(true); //set modified at true only if no fatal, to help user to avoid mistake (saving unsync content)
    }

    return(bNoFatalErrorReversing);
}

e_ProfilOrientation ApplicationModel::getProgressDirection_settingToDefaultIfNotSet() {
    e_ProfilOrientation eProfilOrientation = _computationCore_iqaTableModel.getProgressDirection();
    if (eProfilOrientation == e_PO_notSet) {
        _computationCore_iqaTableModel.setDefaultProgressDirection();
        eProfilOrientation = _computationCore_iqaTableModel.getProgressDirection();
    }
    return(eProfilOrientation);
}

void ApplicationModel::sendComputationParametersToView() {
    qDebug() << __FUNCTION__ ;
    _computationCore_iqaTableModel.send_computationParametersToView();
}

S_boolAndStrErrorMessage_aboutLoad ApplicationModel::get_sbaStrErrorMessage_aboutLoad() {
    return(_sbaStrErrorMessage_aboutLoad);
}

//@#LP slot_selectionBoxChanged and slot_profilsBoxChanged contains partially common code

//update _currentBoxIdInVectOrderAlongRoute, inform _graphicsItemContainer and compute+send BoxMovePossibleShift
void ApplicationModel::slot_selectionBoxChanged(int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion) { //receives the indexes of the same box

    qDebug() << __FUNCTION__ << "(ApplicationModel) entering with indexBoxInVectOrderAlongRoute = " << indexBoxInVectOrderAlongRoute << ", indexBoxAtQMapInsertion = " <<  indexBoxAtQMapInsertion;

    _currentBoxIdInVectOrderAlongRoute = indexBoxInVectOrderAlongRoute;

    _graphicsItemContainer.updateSelectedBox_givenMapBoxId(indexBoxAtQMapInsertion); // the _graphicsItemContainer uses a qmap sync with the qmap in the logic side

    updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

}

//@LP change direct forward to signal->slot->signal for dev checking purpose
void ApplicationModel::slot_stepProgressionFromComputationCore(int v) { //the box in computation in the computaton core is transmitted through this signal
    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    emit signal_stepProgressionFromComputationCore(v);
}


void ApplicationModel::slot_profilsBoxChanged(int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion) {

    qDebug() << __FUNCTION__ << "(ApplicationModel)";

     qDebug() << __FUNCTION__ << "indexBoxInVectOrderAlongRoute =" << indexBoxInVectOrderAlongRoute;
     qDebug() << __FUNCTION__ << "indexBoxAtQMapInsertion =" << indexBoxAtQMapInsertion;

    _currentBoxIdInVectOrderAlongRoute = indexBoxInVectOrderAlongRoute; //should not be useful as slot_profilsBoxChanged should be called with indexBoxInVectOrderAlongRoute sync with _currentBoxIdInVectOrderAlongRoute

    S_ProfilsBoxParameters profilsBoxParameters;
    bool bGot = _computationCore_iqaTableModel.get_oneProfilsBoxParametersFromvectBoxId(_currentBoxIdInVectOrderAlongRoute, profilsBoxParameters);
    if (!bGot) {
        qDebug() << __FUNCTION__ << "(ApplicationModel)" << " error: get_OneProfilsBoxParametersFromvectBoxId failed on _currentBoxIdInVectOrderAlongRoute = " << _currentBoxIdInVectOrderAlongRoute;
        return;
    }

    qDebug() << __FUNCTION__ << "(ApplicationModel) profilsBoxParameters.showContent():";
    profilsBoxParameters.showContent();

    qDebug() << __FUNCTION__ << "(ApplicationModel) here #111";
    _graphicsItemContainer.updateOneBox_givenMapBoxId_and_profilBoxParameters(indexBoxAtQMapInsertion, profilsBoxParameters);


    updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

    emit signal_sendBoxId_and_sizeToView(indexBoxInVectOrderAlongRoute, profilsBoxParameters._oddPixelWidth, profilsBoxParameters._oddPixelLength);
}


void ApplicationModel::updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect() {

    QVector<int> qvectShiftToTest {-20, -5, -1, +1, +5, +20};

    QHash<int, bool> qhash_shift_bPossibleShift;  

    _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.clear();
    _bFeedValid_qhash_shift = false;

    for (int v: qvectShiftToTest) {
        qhash_shift_bPossibleShift.insert(v, false);

    }
    qDebug() << __FUNCTION__ << "initial false feed: qvectShiftToTest = " << qvectShiftToTest;

    if (_currentBoxIdInVectOrderAlongRoute == -1) {
       emit signal_sendBoxCenterPossibleShiftsToView(qhash_shift_bPossibleShift);
       return;
    }

    //check which box center move can be done going backward or forward along the route
    bool bReport = testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(qvectShiftToTest,
                                                                                            qhash_shift_bPossibleShift,
                                                                                            _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute);
    qDebug() << __FUNCTION__ << "after test which center box mvt are possible: qvectShiftToTest = " << qvectShiftToTest;

    _bFeedValid_qhash_shift = bReport;
    qDebug() << __FUNCTION__ << "_bFeedValid_qhash_shift set to: " <<  _bFeedValid_qhash_shift;


    qDebug() << __FUNCTION__ << "_qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute debug content:";
    for (signed int iShiftPossible: qvectShiftToTest) {

        qDebug() << __FUNCTION__ << "search state for iShiftPossible = " << iShiftPossible;

        auto iterFound_possibleShift = qhash_shift_bPossibleShift.find(iShiftPossible);
        if (iterFound_possibleShift == qhash_shift_bPossibleShift.end()) {
            qDebug() << __FUNCTION__ << "not found for iShiftPossible = " << iShiftPossible;
            continue;
        }
        qDebug() << __FUNCTION__ << "for v: " << iterFound_possibleShift.key() << " boolean is: " << iterFound_possibleShift.value();

        if (!iterFound_possibleShift.value()) {
            continue;
        }
        auto iterFound_profilBoxParam = _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.find(iterFound_possibleShift.key());
        if (iterFound_profilBoxParam == _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.end()) {
            qDebug() << __FUNCTION__ << "for v: " << iterFound_profilBoxParam.key() << " not found";
        } else {
            qDebug() << __FUNCTION__ << "for v: " << iterFound_profilBoxParam.key()
                     << " ProfilsBoxParameters.._bPossible = " << iterFound_profilBoxParam.value()._bPossible;

            qDebug() << __FUNCTION__ << "for v: " << iterFound_profilBoxParam.key()
                     << " ProfilsBoxParameters.._shiftFromInitialLocation = " << iterFound_profilBoxParam.value()._shiftFromInitialLocation;

            qDebug() << __FUNCTION__ << "for v: " << iterFound_profilBoxParam.key()
                     << " ProfilsBoxParameters.._profilsBoxParameters.showContent = "; iterFound_profilBoxParam.value()._profilsBoxParameters.showContent();
        }
    }

    send_currentBoxIdInVectOrderAlongRoute_and_boxSize_toView(_currentBoxIdInVectOrderAlongRoute);
    emit signal_sendBoxCenterPossibleShiftsToView(qhash_shift_bPossibleShift);

}

void ApplicationModel::send_currentBoxIdInVectOrderAlongRoute_and_boxSize_toView(int boxId) {

    S_ProfilsBoxParameters profilsBoxParameters;
    bool bGot = _computationCore_iqaTableModel.get_oneProfilsBoxParametersFromvectBoxId(boxId, profilsBoxParameters);
    if (!bGot) {
        qDebug() << __FUNCTION__ << "error get_OneProfilsBoxParametersFromvectBoxId on boxId = " << boxId;
        return;
    }
    emit signal_sendBoxId_and_sizeToView(boxId, profilsBoxParameters._oddPixelWidth, profilsBoxParameters._oddPixelLength);
}

bool ApplicationModel::moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(int vectBoxId, int shift) {

    qDebug() << __FUNCTION__ << "received method param: vectBoxId = " << vectBoxId << " shift = " << shift;
    qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute =" << _currentBoxIdInVectOrderAlongRoute;

    if (_currentBoxIdInVectOrderAlongRoute == -1) {
        qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute is " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }

    //we can move only the _currentBoxIdInVectOrderAlongRoute (_qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute content is about _currentBoxIdInVectOrderAlongRoute)
    if (vectBoxId != _currentBoxIdInVectOrderAlongRoute) {
        qDebug() << __FUNCTION__ << "can not move from here a different boxId than _currentBoxIdInVectOrderAlongRoute"
                 << "(vectBoxId =" << vectBoxId
                 << " _currentBoxId_... = " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }

    if (!_bFeedValid_qhash_shift) {
        qDebug() << "if (!_bFeedValid_qhash_shift) { ";
        return(false);
    }

    const auto cst_iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute = _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.constFind(shift);
    if (cst_iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute == _qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.cend()) {
        qDebug() << __FUNCTION__ << "iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute not found for shift : " << shift;
        return(false);
    }

    if (!cst_iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute.value()._bPossible) {
        //@LP dev error
        qDebug() << __FUNCTION__ << "iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute bPossible is false: dev error";
        return(false);
    }

    qDebug() << __FUNCTION__ << "L677:";
    S_ProfilsBoxParameters profilsBoxParametersWithTheMove;
    profilsBoxParametersWithTheMove = cst_iter_foundForShift_sProfilsBoxParametersForRelativeMoveAlongRoute.value()._profilsBoxParameters;

    profilsBoxParametersWithTheMove.showContent();

    int newVectBoxId_afterReorderAlongTheRoute = -1;

    bool bReport = _computationCore_iqaTableModel.updateProfilsBoxParameters_withBoxMove(
                    _currentBoxIdInVectOrderAlongRoute, profilsBoxParametersWithTheMove,
                    newVectBoxId_afterReorderAlongTheRoute);

    qDebug() << __FUNCTION__ << "updateProfilsBoxParameters_withBoxMove provides: newVectBoxId_afterReorderAlongTheRoute:" << newVectBoxId_afterReorderAlongTheRoute;

    if (!bReport) {
        qDebug() << __FUNCTION__ << " but updateProfilsBoxParameters_withBoxMove said false";
        qDebug() << __FUNCTION__ << " => _currentBoxIdInVectOrderAlongRoute is: " << newVectBoxId_afterReorderAlongTheRoute;
        qDebug() << __FUNCTION__ << "dev error #01";
        return(false);
    }

    _currentBoxIdInVectOrderAlongRoute = newVectBoxId_afterReorderAlongTheRoute;
    qDebug() << __FUNCTION__ << "and updateProfilsBoxParameters_withBoxMove said true";
    qDebug() << __FUNCTION__ << " => _currentBoxIdInVectOrderAlongRoute set to " << _currentBoxIdInVectOrderAlongRoute;

    int mapBoxId = -1;
    bool bGot = _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxId);
    if (!bGot) {
        qDebug() << __FUNCTION__ << "dev error #02";
        return(false);
    }
    qDebug() << __FUNCTION__ << "mapBoxId =" << mapBoxId;

   _graphicsItemContainer.updateOneBox_givenMapBoxId_and_profilBoxParameters(mapBoxId, profilsBoxParametersWithTheMove);
   updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

   _appFile_project.setState_modified(true);

   return(true);
}


//updateOneBox_givenVectBoxId_and_profilBoxParameters

bool ApplicationModel::init_appDiskPathCacheStorage_andLinkedInit(const QString& strRootAppDiskPathCacheStorage) {

    if (!_appDiskPathCacheStorage.setRootDir(strRootAppDiskPathCacheStorage)) {
        qInfo() << ": error: _appDiskPathCacheStorage.setRootDir() using:" << strRootAppDiskPathCacheStorage;
        return(false);
    }
    if (!_appDiskPathCacheStorage.makeSubDirectories()) {
        qInfo() << ": error: _appDiskPathCacheStorage.makeSubDirectories() in:" << strRootAppDiskPathCacheStorage;
        return(false);
    }

    _qstrRootForZLIStorage = _appDiskPathCacheStorage.getRootDir();
    qDebug() << "qstrRootForZLIStorage: " << _qstrRootForZLIStorage;

    /* //cache storage subdir for epsg code not used now, store in the same dir that the image
    //_qstrRootForEPSGCacheStorage = _appDiskPathCacheStorage.getSubDir_EPSGCodes();
    qDebug() << "qstrRootForEPSGCacheStorage: " << _qstrRootForEPSGCacheStorage;
    if (_ptrGeoRefModel_imagesSet) {
        _ptrGeoRefModel_imagesSet->setRootForEPSGCacheStorage(_qstrRootForEPSGCacheStorage);
    }*/

    return(true);
}


bool ApplicationModel::tryToLoadGraphicAppSettings() {
    bool loadedFromDefault = false;
    bool bReport = _graphicAppSettings.load_setMissingToDefaultPreset(loadedFromDefault);
    if (!bReport) {
        qDebug() << __FUNCTION__ << "load_setMissingToDefaultPreset failed";
        return(false);
    }
    if (loadedFromDefault) {
        _graphicAppSettings.save();
    }        
    _sGgraphicsParameters_profil = _graphicAppSettings.get_graphicsParametersProfil();
    _sGgraphicsParameters_itemOnImageView = _graphicAppSettings.get_graphicsParametersItemOnImageView();
    qDebug() << __FUNCTION__ << "tryToLoadGraphicAppSettings end of method reached";
    return(true);
}

S_GraphicsParameters_Profil ApplicationModel::get_graphicsParametersProfil() {
    return(_graphicAppSettings.get_graphicsParametersProfil());
}

S_GraphicsParameters_Profil ApplicationModel::get_graphicsParametersProfil_preset(int presetId) {
    return(_graphicAppSettings.get_graphicsParametersProfil_preset(presetId));
}

S_GraphicsParameters_ItemOnImageView ApplicationModel::get_graphicsParametersItemOnImageView() {
    return(_graphicAppSettings.get_graphicsParametersItemOnImageView());
}

bool ApplicationModel::init(int oiio_cacheAttribute_maxMemoryMB, bool& bLoadReportAbout_EPSGDataListFromProjDB) {

    if (oiio_cacheAttribute_maxMemoryMB < 2) {
        return(false);
    }
    _oiio_cacheAttribute_maxMemoryMB = oiio_cacheAttribute_maxMemoryMB;

    connect(&_routeContainer, &RouteContainer::signal_pointOfRouteMoved  , this, &ApplicationModel::slot_pointOfRoute_movedOrRemoved);
    connect(&_routeContainer, &RouteContainer::signal_pointOfRouteRemoved, this, &ApplicationModel::slot_pointOfRoute_movedOrRemoved);

    //connect(&_routeContainer, &RouteContainer::signal_routeAdded           , this,  &ApplicationModel::slot_routesetEditionFunction_selectionChanged);
    connect(&_routeContainer, &RouteContainer::signal_routeSelectionChanged, this,  &ApplicationModel::slot_routesetEditionFunction_selectionChanged);

    _graphicsItemContainer.initFromAndConnectTo_RouteContainer(&_routeContainer);

    _ptrCustomGraphicsScene = new CustomGraphicsScene(dynamic_cast<QObject*>(this));

    //@#LP: in a way, API and responsability between CustomGraphicsScene and graphicsItemContainer are shared
    _graphicsItemContainer.setPtrCustomGraphicsScene(_ptrCustomGraphicsScene);//to call addItem on _ptrCustomGraphicsScene
    _graphicsItemContainer.addAllRouteItem_and_TinyTargetPtNotDragable_toScene();

    connect(&_computationCore_iqaTableModel, &ComputationCore_inheritQATableModel::signal_selectionBoxChanged,
            this, &ApplicationModel::slot_selectionBoxChanged);
    //to send the vectBoxId to the controller
    connect(&_computationCore_iqaTableModel, &ComputationCore_inheritQATableModel::signal_selectionBoxChanged,
            this, &ApplicationModel::signal_vectBoxIdChanged_fromAppModel); //direct forward

    connect(&_computationCore_iqaTableModel, &ComputationCore_inheritQATableModel::signal_profilsBoxChanged,
            this, &ApplicationModel::slot_profilsBoxChanged);

    connect(&_computationCore_iqaTableModel, &ComputationCore_inheritQATableModel::signal_oneOrMoreBoxesExist,
            this, &ApplicationModel::signal_oneOrMoreBoxesExist); //direct forward

    _ptrCustomGraphicsScene->setGraphicsItemsContainerPtr(&_graphicsItemContainer);//to call adjustToZLI on _graphicsItemContainer, replace by signal/slot ?

    //to add point to selected route:
    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_addPointToSelectedRoute,
            this, &ApplicationModel::slot_addPointToSelectedRoute);

    //to remove the hovered point of a route (no route id filter when receiving)
    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_removeHoveredPointToSelectedRoute,
            this, &ApplicationModel::slot_removePointOfRoute);

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_tryInsertPointToCurrentSelectedRoute,
            this, &ApplicationModel::slot_tryInsertPointToCurrentSelectedRoute);


    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_mouseMove_currentLocation,
                this, &ApplicationModel::slot_displayPointAlongRouteAsPossibleCenterBox);

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_tryAddBox,
            this, &ApplicationModel::signal_tryAddBox);//direct forward (to the controller ,which needs to update its state depending on the tryAddBox success or fail)


    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_trySelectCurrentHoveredBox,
            this, &ApplicationModel::slot_trySelectCurrentHoveredBox);


    _ptrSmallImageNav_CustomGraphicsScene = new SmallImageNav_CustomGraphicsScene(
                _qsizeMax_smallImageNav, dynamic_cast<QObject*>(this));


    bLoadReportAbout_EPSGDataListFromProjDB = loadEPSGDataListFromProjDB();
    if (!bLoadReportAbout_EPSGDataListFromProjDB) {
        qDebug() << __FUNCTION__ << ": error on loadEPSGDataListFromProjDB";
        return(false);
    }

    const QString strPathProjDB = getProjDBPath();

    _ptrGeoRefModel_imagesSet = new GeoRefModel_imagesSet(this);
    if (!_ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error on new GeoRefModel_imagesSet; a bad alloc exception is probably available";
        return(false);
    }
    bool bReport_geoRefModelImagesSet_createContext = _ptrGeoRefModel_imagesSet->createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImagesSet_createContext) {
        qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_sceneMousePosF_with_pixelValue,
                       &_geoUiInfos, &GeoUiInfos::slot_sceneMousePosF_with_pixelValue);

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_sceneMousePosF_noPixelValue,
                       &_geoUiInfos, &GeoUiInfos::slot_sceneMousePosF_noPixelValue);

    connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_sceneMousePosF_outOfImage,
                       &_geoUiInfos, &GeoUiInfos::slot_sceneMousePosF_outOfImage);

    return(true);
}

void ApplicationModel::slot_trySelectCurrentHoveredBox() {

    qDebug() << __FUNCTION__ << "(ApplicationModel) entering";

    int mapBoxId_ofHoveredAndNotSelected = -1;
    bool bReport = _graphicsItemContainer.oneBoxIsCurrentlyHoveredAndNotSelected(mapBoxId_ofHoveredAndNotSelected);

    if (!bReport) {
        qDebug() << __FUNCTION__ << "(ApplicationModel) _graphicsItemContainer.oneBoxIsCurrentlyHoveredAndNotSelected said false";
        return;
    }
    int vectBoxId = -1;
    bReport = _computationCore_iqaTableModel.getIndexBoxInVectOrderAlongRoute_fromMapBoxId(mapBoxId_ofHoveredAndNotSelected, vectBoxId);

    if (!bReport) {
        qDebug() << __FUNCTION__ << "(ApplicationModel)  _computationCore_iqaTableModel.getIndexBoxInVectOrderAlongRoute_fromMapBoxId said false";
        return;
    }

    qDebug() << __FUNCTION__ << "(ApplicationModel) emit now signal_switchToBox_fromClickOnGraphicBoxItem("
             << "vectBoxId = " << vectBoxId << ", mapBoxId_ofHoveredAndNotSelected = " << mapBoxId_ofHoveredAndNotSelected << ")";

    emit signal_switchToBox_fromClickOnGraphicBoxItem(vectBoxId, mapBoxId_ofHoveredAndNotSelected);

}

void ApplicationModel::slot_xAdjusterPosition_changed_fromGraphics(
        uint ieComponent,
        uint ieLRsA_locationRelativeToX0CentralAxis,
        double xPos, int eLRSA_adjuster) {

    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "xPos: " << xPos;
    qDebug() << __FUNCTION__ << "i_eLRsA_locationRelativeToX0CentralAxis: " << ieLRsA_locationRelativeToX0CentralAxis;
    qDebug() << __FUNCTION__ << "eLRSA_adjuster: " << eLRSA_adjuster;

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr_forEdition =
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._measureIndex);

    if (!leftRightSides_linearRegressionModelPtr_forEdition) {
        qDebug() << __FUNCTION__ << "error: leftRightSides_linearRegressionModelPtr_forEdition is nullptr";
        return;
    }

    //get ranges to edit:
    S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation[2];
    xRangeForLinearRegressionComputation[e_LRsA_left] =
        leftRightSides_linearRegressionModelPtr_forEdition->
        _linearRegressionParam_Left_Right[e_LRsA_left]._xRangeForLinearRegressionComputation;

    xRangeForLinearRegressionComputation[e_LRsA_right] =
        leftRightSides_linearRegressionModelPtr_forEdition->
        _linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation;

    //modify:
    if (eLRSA_adjuster == e_LRsA_left) {
        xRangeForLinearRegressionComputation[ieLRsA_locationRelativeToX0CentralAxis]._xMin = xPos;
    } else {
        xRangeForLinearRegressionComputation[ieLRsA_locationRelativeToX0CentralAxis]._xMax = xPos;
    }

    //bool bReport =
    onCurrentBox_set_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(
        ieComponent, xRangeForLinearRegressionComputation);

}

void ApplicationModel::slot_xAdjustersSetPositionValues_changed_fromTextEdition(
        uint ieComponent,
        const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) {

    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax;

    //bool bReport =
    onCurrentBox_set_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(
        ieComponent,
        xRangeForLinearRegressionComputation_left_right_side);
}

bool ApplicationModel::onCurrentBox_set_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(
        uint ieComponent,
        const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) {

    //@#LP closing use case with component not used
    if (!_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieComponent]) {
        return(false);
    }

    bool bReport = onCurrentBox_set_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
                    ieComponent,
                    xRangeForLinearRegressionComputation_left_right_side);
    if (!bReport) {
        qDebug() << __FUNCTION__ << "error: something goes wrong in onCurrentBox__set_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression";
        return(false);
    }

    //update models for Ui
    //----------------
    double locationFromZeroOfFirstInsertedValueInStackedProfil = .0;
    bool bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil = _computationCore_iqaTableModel.
            get_locationFromZeroOfFirstInsertedValueInStackedProfil(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._boxId,
                                                                    locationFromZeroOfFirstInsertedValueInStackedProfil);
    if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {
        return(false);
    }

    qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil = " << locationFromZeroOfFirstInsertedValueInStackedProfil;

    //update graphic model
    if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieComponent]) {

        //feeding scene compute y range as a sole layer
        _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieComponent]->feedScene(
                     static_cast<e_ComponentsAccess>(ieComponent),
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent],


                    locationFromZeroOfFirstInsertedValueInStackedProfil,
                    -locationFromZeroOfFirstInsertedValueInStackedProfil,
                    static_cast<e_MeanMedianIndexAccess>(_computationCore_iqaTableModel.get_computationMethod(ieComponent)),
                    _sGgraphicsParameters_profil, _eProfilAdjustMode);

    }

    onStackedProfilScenes_alignComponentsForCurrentAlignMode();

    //feed textWidgetPartModel with all the measure
    if (_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieComponent]) {

        _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieComponent]->feed(
            static_cast<e_LayersAcces>(ieComponent),
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._stackedProfilWithMeasurementsPtr, //ptrStackedProfilWithMeasurements->_LRSide_linRegrModel_mainMeasure,
            locationFromZeroOfFirstInsertedValueInStackedProfil,
            -locationFromZeroOfFirstInsertedValueInStackedProfil,
            _qvectqstr_graphicTitle_Perp_Parall_deltaZ[ieComponent]);

        //update textWidgetPartModel
        _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieComponent]->setOnMeasure(
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._measureIndex);

    }
    return(true);
}


bool ApplicationModel::onCurrentBox_set_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
        uint ieComponent,
        const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) {

    _appFile_project.setState_modified(true);

    qDebug() << __FUNCTION__ << " ---*- " << ieComponent;
    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax;

    bool bLinearRegressionComputationRequiered =
        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._stackedProfilWithMeasurementsPtr->
            set_LRSidesLinearRegressionModel(
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._measureIndex,
                xRangeForLinearRegressionComputation_left_right_side);

    if (!bLinearRegressionComputationRequiered) {
        qDebug() << __FUNCTION__ << "if (!bLinearRegressionComputationRequiered) {";
        return(false);
    }

    qDebug() << __FUNCTION__;
    bool bComputationReport_leftSideLinearRegressions =
            _computationCore_iqaTableModel.compute_leftRightSidesLinearRegressionsModel_forBoxId(
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._boxId,
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._measureIndex);
    if (!bComputationReport_leftSideLinearRegressions) {
        qDebug() << __FUNCTION__ << "if (!bComputationReport_leftSideLinearRegressions)";
        //@#LP not cleared
    }

    return(bComputationReport_leftSideLinearRegressions);
}


void ApplicationModel::slot_centerX0Position_changed_fromGraphics(uint ieComponent, double centerX0Pos) {
    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "centerX0Pos : " << centerX0Pos;
    /*qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax;*/

    onCurrentBox_set_X0_and_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(ieComponent, centerX0Pos);
}



void ApplicationModel::slot_centerX0SetPositionValue_changed_fromTextEdition(uint ieComponent, double centerX0Pos) {
    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "centerX0Pos : " << centerX0Pos;
    onCurrentBox_set_X0_and_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(ieComponent, centerX0Pos);
}

void ApplicationModel::slot_setWarnFlag_fromTextEdition(uint ieComponent, bool bWarnFlag, bool bAlternativeModeActivated) {
    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "bWarningFlag : " << bWarnFlag;
    qDebug() << __FUNCTION__ << "bAlternativeModeActivated : " << bAlternativeModeActivated;
    onCurrentBox_set_warnFlag_andUpdateModelsForUi(ieComponent, bWarnFlag, bAlternativeModeActivated);
}


bool ApplicationModel::onCurrentBox_set_X0_and_xRangeForLRSide_ComputeLinearRegr_andUpdateModelsForUi(uint ieComponent, double centerX0Pos) {

    //@#LP closing use case with component not used
    if (!_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieComponent]) {
        return(false);
    }

    bool bReport = onCurrentBox_set_X0_and_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
                    ieComponent, centerX0Pos);
    if (!bReport) {
        qDebug() << __FUNCTION__ << "error: something goes wrong in onCurrentBox__set_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression";
        return(false);
    }

    //update models for Ui
    //----------------
    double locationFromZeroOfFirstInsertedValueInStackedProfil = .0;
    bool bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil = _computationCore_iqaTableModel.
            get_locationFromZeroOfFirstInsertedValueInStackedProfil(_qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieComponent]._boxId,
                                                                    locationFromZeroOfFirstInsertedValueInStackedProfil);
    if (!bReportForLocationFromZeroOfFirstInsertedValueInStackedProfil) {
        return(false);
    }

    qDebug() << __FUNCTION__ << " _eProfilAdjustMode : " << _eProfilAdjustMode;

    qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil = " << locationFromZeroOfFirstInsertedValueInStackedProfil;

    for (int ieLComp = e_CA_Perp; ieLComp <= e_CA_deltaZ; ieLComp++) {

        if (_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieLComp]) { //component which must be computed (and thus must be != nullptr)

            //update graphic model
            if (_qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieLComp]) {

                //feeding scene compute y range as a sole layer
                _qvectPtrCGScene_profil_Perp_Parall_deltaZ[ieLComp]->feedScene(
                             static_cast<e_ComponentsAccess>(ieLComp),
                            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp],
                            locationFromZeroOfFirstInsertedValueInStackedProfil,
                            -locationFromZeroOfFirstInsertedValueInStackedProfil,
                            static_cast<e_MeanMedianIndexAccess>(_computationCore_iqaTableModel.get_computationMethod(ieLComp)),
                            _sGgraphicsParameters_profil, _eProfilAdjustMode);

            }

            //feed textWidgetPartModel with all the measure
            if (_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieLComp]) {

                _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieLComp]->feed(
                    static_cast<e_LayersAcces>(ieLComp),
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr, //ptrStackedProfilWithMeasurements->_LRSide_linRegrModel_mainMeasure,
                    locationFromZeroOfFirstInsertedValueInStackedProfil,
                    -locationFromZeroOfFirstInsertedValueInStackedProfil,
                    _qvectqstr_graphicTitle_Perp_Parall_deltaZ[ieLComp]);

                //update textWidgetPartModel
                _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieLComp]->setOnMeasure(
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex);
            }
        }
    }

    onStackedProfilScenes_alignComponentsForCurrentAlignMode();

    return(true);
}




bool ApplicationModel::onCurrentBox_set_X0_and_xRangeForLinearRegressionComputation_left_right_side_andComputeLinearRegression(
        uint ieComponent, double centerX0Pos) {

    qDebug() << __FUNCTION__ << " ---*- " << ieComponent;
    qDebug() << __FUNCTION__ << "ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "centerX0Pos: " << centerX0Pos;
    /*qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin;
    qDebug() << __FUNCTION__ << xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax;*/

    bool bSyncX0ForAll_PerpParallDeltaZ = true;

    //check no access will occurs (if it would occur, unsync result between component would exist)
    for (int ieLComp = e_CA_Perp; ieLComp <= e_CA_deltaZ; ieLComp++) {

        if (_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieLComp]) { //component which must be computed (and thus must be != nullptr)

            S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr_forEdition =
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex);

            if (!leftRightSides_linearRegressionModelPtr_forEdition) { //if nullptr

                //if all three components are linked about X0 adjustement and one of them is nullptr => error
                if (bSyncX0ForAll_PerpParallDeltaZ) {
                    qDebug() << __FUNCTION__ << "error: if (bSyncX0ForAll_PerpParallDeltaZ) => leftRightSides_linearRegressionModelPtr_forEdition is nullptr";
                    return(false);
                }

                //perp&parall linked about X0 adjustement (mandatory), and deltaZ alone about X0 adjustement

                //the source component about the change is about Perp or Parall, and the nullptr is about one of them
                if (  (ieLComp     != e_CA_deltaZ)
                    &&(ieComponent != e_CA_deltaZ)) {
                    qDebug() << __FUNCTION__ << "info: leftRightSides_linearRegressionModelPtr_forEdition is nullptr for Perp or Parall";
                    return(false);
                }

                //the source component about the change is about deltaZ, and the nullptr is about deltaZ
                if (  (ieLComp     == e_CA_deltaZ)
                    &&(ieComponent == e_CA_deltaZ)) {
                    qDebug() << __FUNCTION__ << "info: leftRightSides_linearRegressionModelPtr_forEdition is nullptr for deltaZ";
                    return(false);
                }
            }
        }
    }

    _appFile_project.setState_modified(true);

    //check no access will occurs (if it would occur, unsync result between component would exist)
    for (int ieLComp = e_CA_Perp; ieLComp <= e_CA_deltaZ; ieLComp++) {

        if (_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieLComp]) { //component which must be computed (and thus must be != nullptr)

            S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr_forEdition =
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex);

            if (!leftRightSides_linearRegressionModelPtr_forEdition) { //if nullptr
                return(false); //should never occurs since error about this kind of use case was handled above
            }

            //get ranges to edit:
            S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2];

            xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left ] = leftRightSides_linearRegressionModelPtr_forEdition->_linearRegressionParam_Left_Right[e_LRsA_left ]._xRangeForLinearRegressionComputation;
            xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right] = leftRightSides_linearRegressionModelPtr_forEdition->_linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation;

            //constraint to be consistent with wanted X0
            bool bSomeConstraintApplyed = constraintRangeDueToX0Change_forLinearRegressionComputation(centerX0Pos, xRangeForLinearRegressionComputation_left_right_side);

            bool bLinearRegressionComputationRequiered_fromXRanges = _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->
                set_LRSidesLinearRegressionModel(
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex,
                    xRangeForLinearRegressionComputation_left_right_side);

            //set new X0
            bool bLinearRegressionComputationRequiered_fromCenterX0 =
                    _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->set_X0_forLinearRegressionModel(
                        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex, centerX0Pos);

            if (!bLinearRegressionComputationRequiered_fromCenterX0) {
                qDebug() << __FUNCTION__ << "if (!bLinearRegressionComputationRequiered_fromCenterX0) {";

            }
            //apply constraint xranges and compute

            if (  (!bSomeConstraintApplyed)
                &&(!bLinearRegressionComputationRequiered_fromCenterX0)

               ) {
                return(true); //do not compute if none change about centerX0 and xranges
            }

            bool bComputationReport_leftSideLinearRegressions =
                    _computationCore_iqaTableModel.compute_leftRightSidesLinearRegressionsModel_forBoxId(
                        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._boxId,
                        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex);
            if (!bComputationReport_leftSideLinearRegressions) { //@#LP should never happen
                qDebug() << __FUNCTION__ << "if (!bComputationReport_leftSideLinearRegressions)";
                return(false);
            }
        }
    }

    return(true);
}

//-----------------------------------------
bool ApplicationModel::onCurrentBox_set_warnFlag_andUpdateModelsForUi(uint ieComponent, bool bWarnFlag, bool bAlternativeModeActivated) {

    qDebug() << __FUNCTION__ << "(ApplicationModel) ieComponent: " << ieComponent;
    qDebug() << __FUNCTION__ << "(ApplicationModel) bWarnFlag: " << bWarnFlag;
    qDebug() << __FUNCTION__ << "(ApplicationModel) bAlternativeModeActivated: " << bAlternativeModeActivated;

    bool bSyncWarnFlagFor_PerpParall = false;
    if (ieComponent != e_CA_deltaZ) { //the flag change is about Px1 or Px2
        if (_computationCore_iqaTableModel.getMainComputationMode() == e_mCM_Typical_PX1PX2Together_DeltazAlone) {
            bSyncWarnFlagFor_PerpParall = bAlternativeModeActivated;
        }
    }

    //check no access will occurs (if it would occur, unsync result between component would exist)

    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr_forEdition = nullptr;
    S_LeftRightSides_linearRegressionModel *leftRightSides_linearRegressionModelPtr_forEdition_otherToSync = nullptr;

    int ieLComp = ieComponent;
    int idxCompCheckOther = eLA_Invalid;

    if (_qvectBoolComputedComponents_Perp_Parall_deltaZ[ieLComp]) { //component which must be computed (and thus must be != nullptr)

        leftRightSides_linearRegressionModelPtr_forEdition =
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex);

        if (!leftRightSides_linearRegressionModelPtr_forEdition) { //if nullptr
            qDebug() << __FUNCTION__ << "#1 if (!leftRightSides_linearRegressionModelPtr_forEdition) {";
            return(false);
        }

        //for (int ieLComp = e_CA_Perp; ieLComp <= e_CA_deltaZ; ieLComp++) {

        if (ieLComp != e_CA_deltaZ) { //the flag change is about Px1 or Px2
            if (bSyncWarnFlagFor_PerpParall) { //perp&parall linked about WarnFlag
                idxCompCheckOther = (ieLComp == eLA_PX1)?eLA_PX2:eLA_PX1;

                qDebug() << __FUNCTION__ << " idxCompCheckOther <- " << idxCompCheckOther;
                qDebug() << __FUNCTION__ << " _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._measureIndex._bOnMainMeasure = " <<
                            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._measureIndex._bOnMainMeasure;


                leftRightSides_linearRegressionModelPtr_forEdition_otherToSync =
                        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._stackedProfilWithMeasurementsPtr->getPtrOn_LRSidesLinearRegressionModel(
                        _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._measureIndex);

                if (!leftRightSides_linearRegressionModelPtr_forEdition_otherToSync) { //if nullptr
                    qDebug() << __FUNCTION__ << "#2 if (!leftRightSides_linearRegressionModelPtr_forEdition_otherToSync) {";
                    return(false);
                }
            }
        }/* else {
            //do nothing
        }*/

        _appFile_project.setState_modified(true);

        {
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._stackedProfilWithMeasurementsPtr->set_warnFlag(
            _qvectStackedProfilInEdition_Perp_Parall_deltaZ[ieLComp]._measureIndex, bWarnFlag);

            //feed textWidgetPartModel with all the measure
            if (_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieLComp]) {
                _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[ieLComp]->set_warningFlagByUser(bWarnFlag);
            }
        }


        qDebug() << __FUNCTION__ << " idxCompCheckOther = " << idxCompCheckOther;

        if (idxCompCheckOther != eLA_Invalid) {

            if (bSyncWarnFlagFor_PerpParall) {

                qDebug() << __FUNCTION__ << "@ptr = " << (void*)_qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._stackedProfilWithMeasurementsPtr;

                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._stackedProfilWithMeasurementsPtr->set_warnFlag(
                _qvectStackedProfilInEdition_Perp_Parall_deltaZ[idxCompCheckOther]._measureIndex, bWarnFlag);

                //feed textWidgetPartModel with all the measure
                if (_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[idxCompCheckOther]) {
                    _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ[idxCompCheckOther]->set_warningFlagByUser(bWarnFlag);
                }
            }
        }
    }
    return(true);
}
//-----------------------------------------




bool ApplicationModel::constraintRangeDueToX0Change_forLinearRegressionComputation(
        double double_X0,
        S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]) {

    bool bSomeContraintsApplyed = false;

    int intValueOf_wantedX0 = static_cast<int>(double_X0);

    if (static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin) >= intValueOf_wantedX0) {
                         xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMin = double_X0 - 1.0;
                         bSomeContraintsApplyed = true;
    }
    if (static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax) > intValueOf_wantedX0) {
                         xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left]._xMax = double_X0;
                         bSomeContraintsApplyed = true;
    }

    if (static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin) < intValueOf_wantedX0) {
                         xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMin = double_X0;
                         bSomeContraintsApplyed = true;
    }
    if (static_cast<int>(xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax) <= intValueOf_wantedX0) {
                         xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right]._xMax = double_X0 + 1.0;
                         bSomeContraintsApplyed = true;
    }

    qDebug() << __FUNCTION__ << "e_LRsA_left =>";
    xRangeForLinearRegressionComputation_left_right_side[e_LRsA_left].showContent();

    qDebug() << __FUNCTION__ << "e_LRsA_right =>";
    xRangeForLinearRegressionComputation_left_right_side[e_LRsA_right].showContent();

    return(bSomeContraintsApplyed);
}

void ApplicationModel::setGlobalZoomFactorTo1() {
    _ptrCustomGraphicsScene->setGlobalZoomFactorTo1();
}

void ApplicationModel::setZoomStepIncDec(bool bStepIsIncrease) {
    _ptrCustomGraphicsScene->setZoomStepIncDec(bStepIsIncrease);
}


//return false in case of fatal error, true otherwise
bool ApplicationModel::setComputationParameters_evaluatingIfEqualToCurrent(const S_ComputationParameters& computationParameters_edited) {

    S_ComputationParameters computationParameters_current = ptrCoreComputation_inheritQATableModel()->getComputationParameters();

    qDebug() << __FUNCTION__ << "computationParameters_current:";
    computationParameters_current.debugShow();

    qDebug() << __FUNCTION__ << "computationParameters_edited:";
    computationParameters_edited.debugShow();

    bool bEqual = computationParameters_current.isEqualTo_withTolerance(
                    computationParameters_edited,
                    ptrCoreComputation_inheritQATableModel()->get_qvectLayersInUse());
    if (!bEqual) {
        bool bReport = _computationCore_iqaTableModel.setComputationParameters(computationParameters_edited);
        qDebug() << __FUNCTION__ <<  ": _computationCore_iqaTableModel.setComputationParametersAndRun said: " << bReport;
        if (!bReport) {
            return(false);
        }
        _appFile_project.setState_modified(true);
        _projectContentState.setState_computationParameters(e_Project_computationParametersState::e_P_computationParametersS_changed);
    }
    return(true);
}

bool ApplicationModel::isComputationRequiered_dueToParametersChanges() {

    qDebug() << __FUNCTION__ ;

    //- if :
    // . computationParameters is the same than previous
    // . spatial resolution has not changed
    // . e_Project_stackedProfilComputedState is e_P_stackedProfilComputedS_allComputed
    // . setState_stackedProfilEdited is :
    //       _ e_P_stackedProfilComputedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues
    //    or _ e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession
    // => do not recompute

    //if spatial resolution has changed, mark all the according component of all boxes as "needs to be recomputed"

    //if computationParameters is different than the previous => mark all boxes as "needs to be recomputed"
    auto ePrj_computationParametersState = _projectContentState.getState_computationParameters();
    auto ePrj_spatialResolutionState     = _projectContentState.getState_spatialResolution();
    auto ePrj_stackedProfilComputedState = _projectContentState.getState_stackedProfilComputed();
    auto ePrj_stackedProfilEditedState   = _projectContentState.getState_stackedProfilEdited();

    bool bComputationRequiered = true;

    if (ePrj_computationParametersState == e_Project_computationParametersState::e_P_computationParametersS_set) { //means: not changed (and valid)
        if (ePrj_spatialResolutionState == e_Project_spatialResolutionState::e_P_stackedProfilEditedS_set) {  //means: not changed (and valid)
            if (ePrj_stackedProfilComputedState == e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_allComputed) { //none box not computed
                if (ePrj_stackedProfilEditedState != e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable) {
                    //ePrj_stackedProfilEditedState is e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues
                    //                              or e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession
                    bComputationRequiered = false;
                }
            }
        }
    }
    return(bComputationRequiered);
}

bool ApplicationModel::isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages() {
    return(_computationCore_iqaTableModel.isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages());
}

CustomGraphicsScene *ApplicationModel::ptrCustomGraphicsScene() {
    return(_ptrCustomGraphicsScene);
}


ZoomHandler* ApplicationModel::ptrZoomHandler() {
    return(&_zoomHandler);
}

GeoUiInfos* ApplicationModel::ptrGeoUiInfos() {
    return(&_geoUiInfos);
}

GeoRefModel_imagesSet* ApplicationModel::ptrGeoRefModel_imagesSet() {
    return(_ptrGeoRefModel_imagesSet);
}

SmallImageNav_CustomGraphicsScene* ApplicationModel::ptrSmallImageNav_CustomGraphicsScene() {
    return(_ptrSmallImageNav_CustomGraphicsScene);
}

const QVector<CustomGraphicsScene_profil*>& ApplicationModel::ptrCGScene_profils() {
    qDebug() << __FUNCTION__ << _qvectPtrCGScene_profil_Perp_Parall_deltaZ;
    return(_qvectPtrCGScene_profil_Perp_Parall_deltaZ);
}

const QVector<StackProfilEdit_textWidgetPartModel *>& ApplicationModel::ptrStackProfilEdit_textWidgetPartModel() {
    return(_qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ);
}

RouteContainer *ApplicationModel::ptrRouteContainer() {
    return(&_routeContainer);
}

GraphicsItemsContainer* ApplicationModel::ptrGraphicsItemContainer() {
    return(&_graphicsItemContainer);
}

ComputationCore_inheritQATableModel *ApplicationModel::ptrCoreComputation_inheritQATableModel() {
    return(&_computationCore_iqaTableModel);
}

void ApplicationModel::slot_addPointToSelectedRoute(QPointF scenePosNoZLI) {

    qDebug() << __FUNCTION__;

    if (_selectedRouteId == -1) {
        qDebug() << __FUNCTION__ << " error: _selectedRouteId is -1";
        return;
    }

    bool bThisIsTheFirstPointForTheRoute = false;
    bool bAdded = _routeContainer.tryAddPointAtRouteEnd(_selectedRouteId, scenePosNoZLI, bThisIsTheFirstPointForTheRoute);
    if (bAdded) {        
        setAppFileModifiedState(true);

        _graphicsItemContainer.resetGraphicsRouteItemFromRoute(_selectedRouteId, bThisIsTheFirstPointForTheRoute);
    }
}

void ApplicationModel::setAppFileModifiedState(bool bmodifiedState) {
    if (_eAppModelState == eAModS_idle) {
        return;
    }
    if (_eAppModelState == eAModS_editingRouteSet_editingRoute) {
        qDebug()  << "call now _appFile_routeset.setState_modified( " << bmodifiedState << " )";
        _appFile_routeset.setState_modified(bmodifiedState);
    } else {
        qDebug()  << "call now _appFile_project.setState_modified( " << bmodifiedState << " )";
        _appFile_project.setState_modified(bmodifiedState);
    }
}

const Project_contentState& ApplicationModel::getConstRef_projectContentState() const {
    return(_projectContentState);
}


void ApplicationModel::slot_tryInsertPointToCurrentSelectedRoute(QPointF scenePosNoZLI, int currentZLI) {

    //when locked, _selectedRouteId is -1
    if (_selectedRouteId == -1) {
        qDebug() << __FUNCTION__ << " error: _selectedRouteId is -1";
        return;
    }

    qDebug() << __FUNCTION__ << " currentSelectedRouteId " << _selectedRouteId;

    bool bThisIsTheFirstPointForTheRoute = false;
    bool bInserted = _routeContainer.tryInsertPointInRoute(_selectedRouteId, scenePosNoZLI, currentZLI, bThisIsTheFirstPointForTheRoute);
    if (!bInserted) {
        qDebug() << __FUNCTION__ << " info: failed in tryInsertPointInRoute with _selectedRouteId = " << _selectedRouteId;
    } else {
        setAppFileModifiedState(true);
        //update the graphic items:
        _graphicsItemContainer.resetGraphicsRouteItemFromRoute(_selectedRouteId, false);
    }
}

void  ApplicationModel::slot_addingBoxModeActivated(bool bBoxWidthValid, int oddPixelWidth, int oddPixelLength) {

    qDebug() << __FUNCTION__ ;

    //if no states change, just return
    if (_bAddingBoxMode == bBoxWidthValid) {
        if (_oddPixelWidth_addingBoxMode == oddPixelWidth) {
            if (_oddPixelLength_addingBoxMode == oddPixelLength) {
                qDebug() << __FUNCTION__  << "no states change, just return";
                return;
            }
        }
    }

    _bAddingBoxMode = bBoxWidthValid;
    _ptrCustomGraphicsScene->setAddingBoxMode(_bAddingBoxMode);

    if (!_bAddingBoxMode) {
        _graphicsItemContainer.setAndShowForbiddenSectionAlongRouteForPointSelection({});//forbidden by default (see S_ForbiddenRouteSectionsFromStartAndEnd constructor)
        _graphicsItemContainer.setAddingBoxMode(false);        
        qDebug() << __FUNCTION__  << "if (!_bAddingBoxMode) {";
        return;
    }

    _oddPixelWidth_addingBoxMode = oddPixelWidth;
    _oddPixelLength_addingBoxMode = oddPixelLength;

    if (!_routeContainer.routeCount()) {
        qDebug() << __FUNCTION__  << "if (!_routeContainer.routeCount()) {";
        return;
    }

    Route& routeRef = _routeContainer.getRouteRef(0);
    if (routeRef.getId() == -1) {
        qDebug() << __FUNCTION__  << "if (routeRef.getId() == -1) {";
        return;
    }

    BoxOrienterAndDistributer boad;
    _fields_AddingBox.clear();

    //--- waitCursor
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //---
    //@#LP the display of the entered character in the width ui lineedit is made after the result of
    //     findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingDichotomicMethod(...) which can be a little bit long with large number
    bool bTwoPointsFoundOrOnlyOnePointFound = boad.findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingDichotomicMethod(
                                &routeRef, oddPixelWidth, oddPixelLength,
                                _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd);
    //--- restoreOverrideCursor
    QGuiApplication::restoreOverrideCursor();
    //---

    //@#LP stores in memory the forbiddensection for the different entered widthes to avoid recomputation ? computation is a little bit long with width > 2000 on curved traces
    if (!bTwoPointsFoundOrOnlyOnePointFound) {

        _graphicsItemContainer.setAndShowForbiddenSectionAlongRouteForPointSelection({});//forbidden by default (see S_ForbiddenRouteSectionsFromStartAndEnd constructor)
        _graphicsItemContainer.setAddingBoxMode(false);

        _bAddingBoxMode = false;
        _ptrCustomGraphicsScene->setAddingBoxMode(_bAddingBoxMode);

        qDebug() << __FUNCTION__ << "bTwoPointsFoundOrOnlyOnePointFound is false";
        return;
    }

    qDebug() << __FUNCTION__ << "..._sFrontierFromStart.qpf = "              << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._qpf;
    qDebug() << __FUNCTION__ << "..._sFrontierFromStart._idxSegmentOwner = " << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner;
    qDebug() << __FUNCTION__ << "..._sFrontierFromStart._bPointFeed = "      << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed;

    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd.qpf = "              << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._qpf;
    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd._idxSegmentOwner = " << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner;
    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd._bPointFeed = "      << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed;

    _graphicsItemContainer.setAndShowForbiddenSectionAlongRouteForPointSelection(_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd);
    _graphicsItemContainer.setAddingBoxMode(true);

}

void ApplicationModel::slot_addingBoxModeCanceled() {

    _bAddingBoxMode = false;
    _graphicsItemContainer.remove_gRouteItemWForbiddenSectionsFSAE();
    _graphicsItemContainer.setAddingBoxMode(false);

    _ptrCustomGraphicsScene->setAddingBoxMode(_bAddingBoxMode);


    if (_currentBoxIdInVectOrderAlongRoute == -1) {
        slot_selectionBoxChanged(-1, -1);
    } else {
        int mapBoxID = -1;
        bool bGot = _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxID);
        if (bGot) {
            qDebug() << __FUNCTION__ << "if (bGot) { mapBoxID = " << mapBoxID;
            slot_selectionBoxChanged(_currentBoxIdInVectOrderAlongRoute, mapBoxID);
        } else {
            //@##LP silent internal dev error
            qDebug() << __FUNCTION__ << "internal dev error #51000";
            slot_selectionBoxChanged(-1, -1);
        }
    }
}

void ApplicationModel::slot_displayPointAlongRouteAsPossibleCenterBox(QPointF qpfNoZLI, int currentZLI) {

    qDebug() << __FUNCTION__ << "qpfNoZLI = " << qpfNoZLI;
    qDebug() << __FUNCTION__ << "currentZLI = " << currentZLI;

    _fields_AddingBox._bValid_qpfPointToUseAsBoxCenterPoint = false;

    if (!_bAddingBoxMode) {
        return;
    }

    if (!_routeContainer.routeCount()) {
        return;
    }

    Route& routeRef = _routeContainer.getRouteRef(0);
    if (routeRef.getId() == -1) {
        return;
    }

    qDebug() << __FUNCTION__ << "_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed = "
             << _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed;

    qDebug() << __FUNCTION__ << " _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner = "
             <<  _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner;

    QPointF relocatedPoint {.0,.0};
    int idxOfSegmentOwner = -1;
    bool bReport = routeRef.findRelocatedPointAlongRouteForAFarPoint(qpfNoZLI, relocatedPoint, idxOfSegmentOwner);

    qDebug() << __FUNCTION__ << "idxOfSegmentOwner = " << idxOfSegmentOwner;

    bool bPointUsable = bReport;

    if (bPointUsable) {
        if (_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed) {
            if (idxOfSegmentOwner <= _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner) {

                if (idxOfSegmentOwner < _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner) {
                    bPointUsable = false;                    
                    qDebug() << __FUNCTION__ << "bPointUsable = false  case #1 (frontier from start feed && idxOfSegmentOwner < _sFrontierFromStart._idxSegmentOwner";

                } else {

                    qDebug() << __FUNCTION__ << "Frontier from start feed && idxOfSegmentOwner <= _sFrontierFromStart._idxSegmentOwner";

                    S_Segment Segment_A_to_firstPoint;
                    Segment_A_to_firstPoint._ptA = routeRef.getSegment(_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner)._ptA;
                    Segment_A_to_firstPoint._ptB = _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._qpf;
                    Segment_A_to_firstPoint._bValid = true;
                    double distance_A_to_firstPoint = Segment_A_to_firstPoint.length();

                    qDebug() << __FUNCTION__ << "Segment_A_to_firstPoint = " << Segment_A_to_firstPoint._ptA << " => " << Segment_A_to_firstPoint._ptB;
                    qDebug() << __FUNCTION__ << "distance_A_to_firstPoint = " << distance_A_to_firstPoint;

                    S_Segment Segment_A_to_relocatedPoint;
                    Segment_A_to_relocatedPoint._ptA = Segment_A_to_firstPoint._ptA;
                    Segment_A_to_relocatedPoint._ptB = relocatedPoint;
                    Segment_A_to_relocatedPoint._bValid = true;
                    double distance_A_to_relocatedPoint = Segment_A_to_relocatedPoint.length();

                    qDebug() << __FUNCTION__ << "Segment_A_to_relocatedPoint = " << Segment_A_to_relocatedPoint._ptA << " => " << Segment_A_to_relocatedPoint._ptB;
                    qDebug() << __FUNCTION__ << "distance_A_to_relocatedPoint = " << distance_A_to_relocatedPoint;

                    if (distance_A_to_relocatedPoint < distance_A_to_firstPoint) {
                        bPointUsable = false;
                        qDebug() << __FUNCTION__ << "bPointUsable = false  case #2 ( if (distance_A_to_relocatedPoint < distance_A_to_firstPoint) { )";
                    }
                }
            }
        }

        if (_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed) {
            if (idxOfSegmentOwner >= _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner
                    ) {
                if (idxOfSegmentOwner > _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner) {
                    bPointUsable = false;
                } else {

                    S_Segment Segment_A_to_lastPoint;
                    Segment_A_to_lastPoint._ptA = routeRef.getSegment(_fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner)._ptA;
                    Segment_A_to_lastPoint._ptB = _fields_AddingBox._sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._qpf;
                    Segment_A_to_lastPoint._bValid = true;

                    double distance_A_to_lastPoint = Segment_A_to_lastPoint.length();

                    S_Segment Segment_A_to_relocatedPoint;
                    Segment_A_to_relocatedPoint._ptA = Segment_A_to_lastPoint._ptA;
                    Segment_A_to_relocatedPoint._ptB = relocatedPoint;
                    Segment_A_to_relocatedPoint._bValid = true;

                    double distance_A_to_relocatedPoint = Segment_A_to_relocatedPoint.length();

                    if (distance_A_to_relocatedPoint > distance_A_to_lastPoint) {
                        bPointUsable = false;
                    }
                }
            }
        }
    } else {
        qDebug() << __FUNCTION__ << "findRelocatedPointAlongRouteForAFarPoint failed to relocate point: " << qpfNoZLI;
    }

    _fields_AddingBox._bValid_qpfPointToUseAsBoxCenterPoint = bPointUsable;

    if (bPointUsable) {

        qDebug() << __FUNCTION__ << "final: bPointUsable is true";

        _fields_AddingBox._qpfPointToUseAsBoxCenterPoint = relocatedPoint;
        _fields_AddingBox._idxSegmentOwner_ofQpfPointToUseAsBoxCenterPoint = idxOfSegmentOwner;
        _graphicsItemContainer.setAndShow_targetCursorLocationOfPossibleCenterForAddBox(relocatedPoint);

    } else {

        qDebug() << __FUNCTION__ << "final: bPointUsable is false";
        _graphicsItemContainer.hide_targetCursorLocationOfPossibleCenterForAddBox();
    }

}

//addbox use the sole route in _routeContainer
bool ApplicationModel::tryAddBox(QPointF qpfNoZLI, int currentZLI, bool& bCriticalErrorOccured) {

    bCriticalErrorOccured = false;

    if (!_bAddingBoxMode) {
        return(false);
    }

    if (!_routeContainer.routeCount()) {
        return(false);
    }

    Route& routeRef = _routeContainer.getRouteRef(0);
    if (routeRef.getId() == -1) {
        return(false);
    }

    if (!_fields_AddingBox._bValid_qpfPointToUseAsBoxCenterPoint) {
        return(false);
    }

    S_ProfilsBoxParameters sProfilsBoxParameters;
    BoxOrienterAndDistributer boad;
    bool bReport = boad.makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(
        &routeRef,
        _fields_AddingBox._qpfPointToUseAsBoxCenterPoint,
        _fields_AddingBox._idxSegmentOwner_ofQpfPointToUseAsBoxCenterPoint,
        _oddPixelWidth_addingBoxMode,
        _oddPixelLength_addingBoxMode,
        sProfilsBoxParameters);

    if (!bReport) {        
        return(false);
    }

    bool bCriticalErrorOccured_about_addBSWPWM_fromqvectPBP = false;

    int vectBoxIdFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox = -1;
    bool bAddReport = _computationCore_iqaTableModel.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                {sProfilsBoxParameters},
                true,  // (true or false is here irrevelant as the boxes order does not make sens when inserting only one box
                vectBoxIdFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured_about_addBSWPWM_fromqvectPBP);

    if (!bAddReport) {        
        if (bCriticalErrorOccured_about_addBSWPWM_fromqvectPBP) {
            bCriticalErrorOccured = true;
        }
        return(false);
    }

    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes);
    _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_mixedComputedAndNotComputed); //consider the first insertion as a mixed also
    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues);

    //set the route for the computation core to be able to resize boxes //@#LP resize check condition should be made out the computationCore
    _computationCore_iqaTableModel.setRoutePtr(&routeRef);

    bool bCriticalErrorOccured_aboutAddBoxFromMapBASPWM = false;

    int boxesCountToAdd = 1;
    bool bAddReport_graphicsItemContainer = _graphicsItemContainer.addBoxes_from_mapBoxAndStackedProfilWithMeasurements_fromKeyToKey(_computationCore_iqaTableModel.getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements(),
                                                                               keyMapOfFirstInsertedBox,
                                                                               keyMapOfFirstInsertedBox + boxesCountToAdd - 1,
                                                                               bCriticalErrorOccured_aboutAddBoxFromMapBASPWM);
    if (!bAddReport_graphicsItemContainer) {
        if (bCriticalErrorOccured_aboutAddBoxFromMapBASPWM) {
            bCriticalErrorOccured = true;
        }
        return(false);
    }

    slot_selectionBoxChanged(vectBoxIdFirstInsertedBox, keyMapOfFirstInsertedBox);
    _graphicsItemContainer.updateSelectedBox_givenMapBoxId(keyMapOfFirstInsertedBox);

    _appFile_project.setState_modified(true);

    return(true);
}


bool ApplicationModel::removeBoxFromVectId(int vectBoxId_toRemove, bool &bBoxContainerNotEmpty) {

    int boxCountBeforeRemove = _computationCore_iqaTableModel.getBoxCount();

    bBoxContainerNotEmpty = (boxCountBeforeRemove != 0);//initial state before trying to remove

    int mapBoxId = -1;
    bool bGotMapIdToRemove = _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxId);

    if (!bGotMapIdToRemove) {
        qDebug() << __FUNCTION__ << "internal dev error #01";
        return(false);
    }

    _computationCore_iqaTableModel.removeSectionOfBoxes(vectBoxId_toRemove, vectBoxId_toRemove);
    int boxCountAfterRemove = _computationCore_iqaTableModel.getBoxCount();

    _graphicsItemContainer.removeSelectionOfBoxes({mapBoxId}); // the _graphicsItemContainer uses a qmap sync with the qmap in the logic side

    //set the selected box a box along the route near the removed box
    int boxIdToSelectAfterRemoveCurrentBox = -1;
    if (boxCountAfterRemove) { //one or more boxes still in the container
        if (boxCountAfterRemove == 1) { //only one box
            boxIdToSelectAfterRemoveCurrentBox = 0;
        } else { //more than one box
            if (vectBoxId_toRemove == 0) { //the removed box  was the first
                boxIdToSelectAfterRemoveCurrentBox = 0; //set to the newly first
            } else {
                if (vectBoxId_toRemove == (boxCountBeforeRemove - 1)) { //the removed box was the last
                    boxIdToSelectAfterRemoveCurrentBox = boxCountAfterRemove - 1; //set to the newly last
                } else {
                    boxIdToSelectAfterRemoveCurrentBox = vectBoxId_toRemove; //set to the closest box inheriting the box Id
                }
            }
        }
    } /* else {
        //do nothing, boxIdToSelectAfterRemoveCurrentBox already set to -1
    }*/

    _currentBoxIdInVectOrderAlongRoute = boxIdToSelectAfterRemoveCurrentBox;

    //here we need to:
    //- select this new currentbox in the tableview if not -1 and in any case update the currently selected BoxId in the dialog box edit
    //- feed dialog box edit infos about the currentbox with the possible movements from the current location or clear and disabled the widget
    //- update the selected box in the graphicscontainer side

    //@LP these three methods handle _currentBoxIdInVectOrderAlongRoute at -1 correctly:
    //

    updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

    _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(_currentBoxIdInVectOrderAlongRoute, mapBoxId); //it sets mapBoxId to -1 if _currentBoxIdInVectOrderAlongRoute is -1

    _graphicsItemContainer.updateSelectedBox_givenMapBoxId(mapBoxId); // the _graphicsItemContainer uses a qmap sync with the qmap in the logic side
                                                                      // mapBoxId at -1 will produce no newly selected box

    bBoxContainerNotEmpty = (boxCountAfterRemove != 0);//state after the remove

    _appFile_project.setState_modified(true);

    return(true);
}


bool ApplicationModel::removeAllBoxes() {

    int boxCountBeforeRemove = _computationCore_iqaTableModel.getBoxCount();

    bool bBoxContainerNotEmpty = (boxCountBeforeRemove != 0);//initial state before trying to remove

    if (!bBoxContainerNotEmpty) { //already empty
        return(false);
    }

    _computationCore_iqaTableModel.removeAllBoxes();
    _graphicsItemContainer.removeAllBoxesAndRelatedItems();


    //set the selected box a box along the route near the removed box
    _currentBoxIdInVectOrderAlongRoute = -1;

    //here we need to:
    //- select this new currentbox in the tableview if not -1 and in any case update the currently selected BoxId in the dialog box edit
    //- feed dialog box edit infos about the currentbox with the possible movements from the current location or clear and disabled the widget
    //- update the selected box in the graphicscontainer side

    //@LP these methods handle _currentBoxIdInVectOrderAlongRoute at -1 correctly:
    updateAndSendBoxMovePossibleShift_aboutCurrentBoxIdInVect();

     _appFile_project.setState_modified(true);

    return(true);
}

void ApplicationModel::slot_removePointOfRoute(int idPoint, int idRoute) {

    int nbRemainingPointInRoute = 0;
    if (!_routeContainer.getRouteRef(idRoute).segmentCount()) {
        qDebug() << __FUNCTION__ << "remove of last point of a route is a forbidden operation. Remove the route instead";
        return;
    }

    bool bPointRemoved = _routeContainer.removePointOfRoute(idPoint, idRoute, nbRemainingPointInRoute);
    if (!bPointRemoved) {
        qDebug() << __FUNCTION__ << "error: removing Point " << idPoint << "of Route " << idRoute;
        return;
    }


    setAppFileModifiedState(true);


    if (!nbRemainingPointInRoute) {
        qDebug() << __FUNCTION__ << __LINE__;
        removeRoute(idRoute);

    } else {
        qDebug() << __FUNCTION__ << __LINE__;
        _graphicsItemContainer.resetGraphicsRouteItemFromRoute(idRoute, false);
    }
}

void ApplicationModel::addNewRoute(QString strRouteName) {
    _routeContainer.aboutAbstractTableModel_addNewRoute(strRouteName);

    //an added route has no point, hence no change in graphics container at this point

    setAppFileModifiedState(true);

}

void ApplicationModel::renameRoute(int idRoute, const QString& strRouteName, bool bAboutRouteInsideProject) {
    if (bAboutRouteInsideProject) {
        _appFile_project.setState_modified(true);
    } else {
        _appFile_routeset.setState_modified(true);
    }
    _routeContainer.renameRoute(idRoute, strRouteName);
}

QString ApplicationModel::getProjectRouteName() {
    Route& refRoute = _routeContainer.getRouteRef(0);
    if (refRoute.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: no route !";
        return("<no route>");
    }
    return(refRoute.getName());
}

void ApplicationModel::removeRoute(int idRoute) {
    _routeContainer.aboutAbstractTableModel_removeRoute(idRoute); //this method will emit a singnal about remove successfully
}



void ApplicationModel::setState_editingRoute_closeState_clearRouteset() {
    qDebug() << __FUNCTION__;
    clearSomeModels_forNewAppFunctionStart();
    _eAppModelState = eAModS_idle;
}

void ApplicationModel::setState_editingProject_closeState_clearProject() {
    qDebug() << __FUNCTION__;
    clearSomeModels_forNewAppFunctionStart();

    _eAppModelState = eAModS_idle;
}

void ApplicationModel::clearStringsError() {
    _segeoRefImageSetStatus_routeSetEdition = {};
    _sbaStrMsg_gRIS_routeSetEdition = {};

    _sbaStrErrorMessage_aboutLoad = {};
}


void ApplicationModel::updateLocationOfSpecificItemsOfInterest_firstAndLastPointOfRoute() {
    _graphicsItemContainer.removeItemFromSceneAndDelete_CGSpecificItemsOfInterest();
    _graphicsItemContainer.allocInitFromRouteAndAddHiddenToScene_locationOfSpecificGraphicsItemAboutRouteStartAndEnd();
    _graphicsItemContainer.adjustLocationAndSizeToCurrentZLI_aboutSpecificGraphicsItemAboutRouteStartAndEnd();
    _graphicsItemContainer.adjustGraphicsItemWidth_aboutSpecificGraphicsItemAboutRouteStartAndEnd();
}

bool ApplicationModel::getQvectStrAndBoolFlagForComputedComponents_profilGraphicMeanings(
    QVector<bool>& qvectBool_componentToCompute,
    QVector<QString>& qvectStr_majorCurveMeaning,
    QVector<QString>& qvectStr_minorEnveloppeAreaMeaning) {

    qvectBool_componentToCompute = _computationCore_iqaTableModel.get_qvectComputedComponents();

    return(getVectStr_aboutProfilCurvesMeanings(
        _computationCore_iqaTableModel.getMainComputationMode(),
        qvectBool_componentToCompute,
        _computationCore_iqaTableModel.getVect_computationMethods(),
        qvectStr_majorCurveMeaning,
        qvectStr_minorEnveloppeAreaMeaning));
}



void ApplicationModel::clearSomeModels_forNewAppFunctionStart() { //@#LP clearSomeModels_ naming do not say which models

    _routeContainer.clear();
    _bStateAddADefaultRoute = false;
    _selectedRouteId = -1;
    _selectedRouteId_forRouteImport = -1;

    _graphicsItemContainer.removeAllRouteItem();
    _graphicsItemContainer.removeAllBoxesAndRelatedItems();
    _graphicsItemContainer.removeAllDebugItems();
    _graphicsItemContainer.remove_gRouteItemWForbiddenSectionsFSAE();
    _graphicsItemContainer.hide_targetCursorLocationOfPossibleCenterForAddBox(); //@LP: SpecificItemsOfInterest (first and last point) exists until the exit of the app
    _graphicsItemContainer.removeItemFromSceneAndDelete_CGSpecificItemsOfInterest();
    //@LP possible location target point exists until the exit of the app

    _ptrCustomGraphicsScene->clearContent_letPtrToAssociatedInstances();
    _ptrSmallImageNav_CustomGraphicsScene->clear();

    _geoCRSSelect_inheritQATableModel.slot_keywordListCleared();

    _appFile_routeset.clear();
    _appFile_project.clear();

    if (_ptrGeoRefModel_imagesSet) {
        _ptrGeoRefModel_imagesSet->clear();
    }

    _vectDequantization_PX1_PX2_deltaZ.resize(3);
    _vectDequantization_PX1_PX2_deltaZ[eLA_PX1].clear();
    _vectDequantization_PX1_PX2_deltaZ[eLA_PX2].clear();
    _vectDequantization_PX1_PX2_deltaZ[eLA_deltaZ].clear();

    _segeoRefImageSetStatus_routeSetEdition = {};
    _sbaStrMsg_gRIS_routeSetEdition = {};

    _computationCore_iqaTableModel.clear();

    _zoomHandler.clear();

    _projectContentState.clear();

    _qszInputImageWidthHeight = {1.0,1.0};

    _geoUiInfos.clear();

    _qvectBoolComponentsToShow_Perp_Parall_deltaZ.fill(false,3);

    _bUseGeoRefForTrace = true;//default is true
    _bUseGeoRefForProject = true;//default is true
}

void ApplicationModel::slot_pointOfRoute_movedOrRemoved() {
    setAppFileModifiedState(true);
}

S_boolAndStrErrorMessage_aboutLoad ApplicationModel::get_sboolAndStrMsgAbout_aboutLoad() {
    return(_sbaStrErrorMessage_aboutLoad);
}

void ApplicationModel::set_stackProfilEdit_syncYRange(bool bSyncYRangeState) {
    _bStackProfilEdit_syncYRange = bSyncYRangeState;
}

//@LP: this method considers formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver()
//This is an other part of the app (and computation core also) which check that the file is formatIsAnAcceptedFormatForDisplacementMap()
bool ApplicationModel::loadBackgroundImageBuildingImagePyramidIfNeeds_loadBackgroundImageForSmallNav(const QString& qstrRootImageDisplay, int idxLayer) {

    if ((idxLayer < eLA_PX1)||(idxLayer > eLA_deltaZ)) {
        return(false);
    }

    QString qstrZLIStoragePath;
    bool bReport = buildImagePyramidifNeedsForImageDisplay(qstrRootImageDisplay, qstrZLIStoragePath);
    if (!bReport) {
        return(false);
    }

    bool bSuccess = _ptrCustomGraphicsScene->createAndInitTilesProvider(qstrRootImageDisplay, qstrZLIStoragePath, _oiio_cacheAttribute_maxMemoryMB, idxLayer);
    if (!bSuccess) {
        qDebug() << __FUNCTION__ << __FUNCTION__ << ": error with createAndInitTilesProvider";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "create and init tiles provider failed";
        return(false);
    }

    qDebug() << __FUNCTION__ << ": success with createAndInitTilesProvider";

    int maximumZoomLevelImgAvailable = _ptrCustomGraphicsScene->get_maximumZoomLevelImg();

    _zoomHandler.initZoomRange_nonLinear_increasePreviousValueByXXXPercent(maximumZoomLevelImgAvailable);

    _zoomHandler.buildViewSideFullScalesForEachZLIWhenNoZLISwitch(); //@LP used only when a separated thread is used to load the tiles requiered by a new incoming ZLI request

    _zoomHandler.setToZoomLevelImage1_scale1();

    _ptrCustomGraphicsScene->setZoomHandlerPtr(&_zoomHandler);

    //steps:
    //- get the filename of the maximum ZLI to build the background nav image with its dim
    //it can be the input image if no ZLI was built on due to too small input image
    S_zoomLevelImgInfos SZLIInfosJustAboveSmallImageNavMaxSize =
    _ptrCustomGraphicsScene->get_SzLIInfosJustAboveWH(_qsizeMax_smallImageNav.width(), _qsizeMax_smallImageNav.height(), idxLayer);
    SZLIInfosJustAboveSmallImageNavMaxSize.showContent();

    //- compute the size of the background nav image
    //- resize/rescale maximum ZLI image to size of the background nav image
    //- load and contrast remap

    //- keep small nav image in RAM in scene side
    //- resize view
    //- set initial location of location/size of the rect view

    QSize imageSizeNoZoom;
    bool bImageSizeNoZoomGot = _ptrCustomGraphicsScene->getImageSizeNoZoom(imageSizeNoZoom);
    if (!bImageSizeNoZoomGot) {
        qDebug() << __FUNCTION__ << ": error: failed to get ImageSizeNoZoom. Hence, can't init SmallImageNav scene";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "failed to get imageSizeNoZoom from TilesProvider";
        return(false);
    } else {

        //@#LP: failed to create the small image nav not considered as a critical error. Just be sure to warn the user if you want.
        _ptrSmallImageNav_CustomGraphicsScene->createImageBackgroundAndInit(SZLIInfosJustAboveSmallImageNavMaxSize, imageSizeNoZoom, idxLayer);

        connect(_ptrCustomGraphicsScene, &CustomGraphicsScene::signal_currentExposedRectFChanged,
            _ptrSmallImageNav_CustomGraphicsScene, &SmallImageNav_CustomGraphicsScene::slot_currentExposedRectFChanged);
    }

    _qszInputImageWidthHeight = { static_cast<qreal>(imageSizeNoZoom.width()),
                                  static_cast<qreal>(imageSizeNoZoom.height())};

    return(true);
}

//@LP: this method considers formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver()
//This is an other part of the app (and computation core also) which check that the file is formatIsAnAcceptedFormatForDisplacementMap()
bool ApplicationModel::loadAdditionnalLayerImageBuildingImagePyramidIfNeeds_loadAlsoForSmallNav(int idxLayer) {

    if ((idxLayer < eLA_PX1)||(idxLayer > eLA_deltaZ)) {
        return(false);
    }

    QString qstrRootImageDisplay = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[idxLayer];

    QString qstrZLIStoragePath;
    bool bReport = buildImagePyramidifNeedsForImageDisplay(qstrRootImageDisplay, qstrZLIStoragePath);
    if (!bReport) {
        return(false);
    }

    bool bSuccess = _ptrCustomGraphicsScene->createAndInitTilesProvider(qstrRootImageDisplay, qstrZLIStoragePath, _oiio_cacheAttribute_maxMemoryMB, idxLayer);
    if (!bSuccess) {
        qDebug() << __FUNCTION__ << __FUNCTION__ << ": error with createAndInitTilesProvider";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "create and init tiles provider failed";
        return(false);
    }

    qDebug() << __FUNCTION__ << ": success with createAndInitTilesProvider";


    //steps:
    //- get the filename of the maximum ZLI to build the background nav image with its dim
    S_zoomLevelImgInfos SZLIInfosJustAboveSmallImageNavMaxSize =
     _ptrCustomGraphicsScene->get_SzLIInfosJustAboveWH(_qsizeMax_smallImageNav.width(), _qsizeMax_smallImageNav.height(), idxLayer);

    //- compute the size of the background nav image
    //- resize/rescale maximum ZLI image to size of the background nav image
    //- load and contrast remap


    //- keep small nav image in RAM in scene side
    //- resize view
    //-set initial location of location/size of the rect view

    QSize imageSizeNoZoom;
    bool bImageSizeNoZoomGot = _ptrCustomGraphicsScene->getImageSizeNoZoom(imageSizeNoZoom);
    if (!bImageSizeNoZoomGot) {
        qDebug() << __FUNCTION__ << ": error: failed to get ImageSizeNoZoom. Hence, can't init SmallImageNav scene";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "failed to get imageSizeNoZoom from TilesProvider";
        return(false);
    } else {
        //@#LP: failed to create the small image nav not considered as a critical error. Just be sure to warn the user if you want.
        bool bReport_createAdditionnalImageAndInit = _ptrSmallImageNav_CustomGraphicsScene->createAdditionnalImageAndInit(SZLIInfosJustAboveSmallImageNavMaxSize._strFilename, idxLayer);
        qDebug() << __FUNCTION__ << " bReport_createAdditionnalImageAndInit = " << bReport_createAdditionnalImageAndInit;
    }

    return(true);
}

#include "io/ZLIStorageContentManager.h"
/*
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

QFuture<bool> futureBool = QtConcurrent::run(zLIImageBuilder,
                                         &ZoomOutLevelImageBuilder::build_untilWidthOrHeightGreaterThan,
                                         1024,
                                         stepAmount);
_watcherBool.setFuture(futureBool); //be informed when loading terminated
*/

bool ApplicationModel::buildImagePyramidifNeedsForImageDisplay(const QString& qstrRootImageDisplay, QString& qstrZLIStoragePath) {

    //-------
    qDebug()<< __FUNCTION__ << " qstrRootImageDisplay: " << qstrRootImageDisplay;

    _sbaStrErrorMessage_aboutLoad = {};

    if (!_ptrCustomGraphicsScene) {
        qDebug()<< __FUNCTION__ << " _ptrCustomGraphicsScene is nullptr";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "internal dev error: GraphicsScene is nullptr";
        _sbaStrErrorMessage_aboutLoad._bFileDoesnotExistAtLocation = false;
        return(false);
    }

    if (!fileExists(qstrRootImageDisplay)) {
        qDebug() << __FUNCTION__ << ": qstrImageFilename does not exist: " << qstrRootImageDisplay;
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "file does not exist";
        _sbaStrErrorMessage_aboutLoad._bFileDoesnotExistAtLocation = true;
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage_details_filename = qstrRootImageDisplay;
        return(false);
    }

    InputImageFormatChecker iifc;
    bool bReport_iifc = iifc.formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(qstrRootImageDisplay.toStdString());
    if (!bReport_iifc) {
        qDebug() << __FUNCTION__ << ": error: InputImageFormatChecker rejects file: " << qstrRootImageDisplay;
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "File format not supported by the application";
        return(false);
    }

    //handle same filenames with different location as different files
    //(instead of the previous implementation which considers that for a filename, any file location is about the same file
    //Note that the content of the file itself to replace ZLI of a dir/location/filename is not used
    ZLIStorageContentManager ZLISContentMgr;
    ZLISContentMgr.setRootPath(_qstrRootForZLIStorage);

    bool bFileDoesNotExist = true;
    bool bNoError = ZLISContentMgr.load_ZLIStorageContentFile(bFileDoesNotExist);
    if (!bNoError) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to load the content file";
        qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
        return(false);
    }
    qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile(...) bFileDoesNotExist = " << bFileDoesNotExist;

    //update content, removing in loaded content the removed directories (valid operation by the user)
    bool bSomeRemoved = false;
    bool bDone = ZLISContentMgr.removeRelativePathOfZLIforFile_forAccordingDisappearedDirectories(bSomeRemoved);
    if (!bDone) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to cleanup the content file";
        qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
        return(false);
    }
    if (bSomeRemoved) {
        bDone = ZLISContentMgr.save_ZLIStorageContentFile();
        if (!bDone) {
            _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to save cleaned content file";
            qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
            return(false);
        }
    }

    ZoomOutLevelImageBuilder zLIImageBuilder(qstrRootImageDisplay, _qstrRootForZLIStorage);

    bool bExistInDBFile = false;
    qlonglong idAboutDir = -1;
    bNoError = ZLISContentMgr.checkExistsInDBandGetId(qstrRootImageDisplay, idAboutDir, bExistInDBFile);
    if (!bNoError) {
        qDebug() <<  __FUNCTION__ << "bFatalError ZLISCM.checkExistsAndGetIdInDB(...) strDirToto = " << qstrRootImageDisplay;
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to get id for file " + qstrRootImageDisplay;
        qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
        return(false);
    }
    qDebug() <<  __FUNCTION__ << "bExistInDBFile = " << bExistInDBFile;
    if (!bExistInDBFile) {
        idAboutDir = ZLISContentMgr.get_nextAvailableUniqueIDForDirName();
    }

    bool bReport = zLIImageBuilder.makeStorageDirectory(idAboutDir); //addAndMakeDirForImage(qstrRootImageDisplay);
    if (!bReport) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to make ZLI directory for file " + qstrRootImageDisplay;
        qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
        return(false);
    }

    ZLISContentMgr.add_pair_strPathAndFilename_uniqueIdForZLIRelativePath(qstrRootImageDisplay, idAboutDir);
    ZLISContentMgr.increase_currentUsedMaximalUniqueID();
    bool bZLIStorageContentFileSaved = ZLISContentMgr.save_ZLIStorageContentFile();
    if (!bZLIStorageContentFileSaved) {
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "storage cache: failed to save update adding file " + qstrRootImageDisplay;
        qDebug() <<  __FUNCTION__ << "ZLISCM.load_ZLIStorageContentFile error";
        return(false);
    }

    bool bNeedCleanAndInitDirectory = false;
    if (!zLIImageBuilder.evaluateContentStateOfZLIStoragePath(bNeedCleanAndInitDirectory)) {
        qDebug() << __FUNCTION__ << ": error: failed in zLIImageBuilder.evaluateContentStateOfZLIStoragePath()";
        _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "image pyramid: storage: content and state evaluation failed";
        return(false);
    }

    if (bNeedCleanAndInitDirectory) {
        if (!zLIImageBuilder.cleanAndInitDirectory()) {
            qDebug() << __FUNCTION__ << ": error: failed int zLIImageBuilder.cleanAndInitDirectory()";
            _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "image pyramid: storage: cleaning and init directory failed (#2)";
            return(false);
        }

//#define def_useLegacyCode //(legacy code do not handle NAN)
#ifdef def_useLegacyCode
        int stepAmount = 0;
        bool bStepAmoutComputed = zLIImageBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan(1024, stepAmount);
        if (!bStepAmoutComputed) {
#else

        int stepAmount = 0;
        int stepAmount_usingRootImgForEachLvl = 0;
        int stepAmount_usingRootImgForEachLvl_startAtZLI4 = 0;

        bool bComputeStepAmountForProgressBar = true;
        bComputeStepAmountForProgressBar &= zLIImageBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan(
                 1024,
                 stepAmount);
        bComputeStepAmountForProgressBar &= zLIImageBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(
                 1024,
                 stepAmount_usingRootImgForEachLvl,
                 stepAmount_usingRootImgForEachLvl_startAtZLI4);

        if (!bComputeStepAmountForProgressBar) {
#endif
            qDebug() << __FUNCTION__ << ": error: failed in zLIImageBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan()";
            _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "image pyramid: building failed (#1)";
            return(false);
        }

        //emit signal_stepAmoutForProgressBar(stepAmount);

        connect(&zLIImageBuilder, &ZoomOutLevelImageBuilder::signal_stepProgressionForOutside,
                this, &ApplicationModel::signal_stepProgressionForProgressBar); //direct forward
        //connect(&zLIImageBuilder, &ZoomOutLevelImageBuilder::signal_stepProgressionForOutside_terminated,
        //        this, &ApplicationModel::signal_stepProgressionForProgressBar_terminated); //direct forward

        emit signal_openDialog_taskInProgress_forLongComputation("processing...", "Building image pyramid", 0, 100);

        bool bContainsNan = false;
        bool bReport = false;

#ifdef def_useLegacyCode
        bReport = zLIImageBuilder.build_untilWidthOrHeightGreaterThan(1024, stepAmount, bContainsNan);
#else
        bReport = zLIImageBuilder.build_untilWidthOrHeightGreaterThan(1024, stepAmount, bContainsNan);
        if (bContainsNan) {
            qDebug() << __FUNCTION__ << "restart from zoomLevel4 handling Nan";
            bReport = zLIImageBuilder.build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_fromLevel(
                        4, 1024, stepAmount_usingRootImgForEachLvl_startAtZLI4);
        }
#endif

        disconnect(&zLIImageBuilder, &ZoomOutLevelImageBuilder::signal_stepProgressionForOutside,
                   this, &ApplicationModel::signal_stepProgressionForProgressBar);

        emit signal_closeDialog_taskInProgress_forLongComputation();

        if (!bReport) {
            qDebug() << __FUNCTION__ << ": error: failed in zLIImageBuilder.build_untilWidthOrHeightGreaterThan()";
            _sbaStrErrorMessage_aboutLoad._strMessagAboutImage = "image pyramid: building failed (#2)";
            return(false);
        }
    }
    //-------

    qstrZLIStoragePath = zLIImageBuilder.getZLIStoragePath();
    return(true);
}

ApplicationModel::~ApplicationModel() {

}

bool ApplicationModel::setBoxSizeOfCurrentBoxCheckingThatBoxFitInRoute(int vectBoxId, int oddPixelWidth, int oddPixelLength) {


    qDebug() << __FUNCTION__ << "received method param: vectBoxId = " << vectBoxId << " oddPixelWidth = " << oddPixelWidth << " oddPixelLength = " << oddPixelLength;
    qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute =" << _currentBoxIdInVectOrderAlongRoute;

    if (_currentBoxIdInVectOrderAlongRoute == -1) {
        qDebug() << __FUNCTION__ << "_currentBoxIdInVectOrderAlongRoute is " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }

    //we can move only the _currentBoxIdInVectOrderAlongRoute (_qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute content is about _currentBoxIdInVectOrderAlongRoute)
    if (vectBoxId != _currentBoxIdInVectOrderAlongRoute) {
        qDebug() << __FUNCTION__ << "can not move from here a different boxId than _currentBoxIdInVectOrderAlongRoute"
                 << "(vectBoxId =" << vectBoxId
                 << " _currentBoxId_... = " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }


    S_ProfilsBoxParameters profilsBoxParameters;
    bool bGot = _computationCore_iqaTableModel.get_oneProfilsBoxParametersFromvectBoxId(_currentBoxIdInVectOrderAlongRoute, profilsBoxParameters);
    if (!bGot) {
        qDebug() << __FUNCTION__ << "error: fail to get_OneProfilsBoxParametersFromvectBoxId with _currentBoxIdInVectOrderAlongRoute = " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }
    int current_oddPixelWidth  = profilsBoxParameters._oddPixelWidth;
    int current_oddPixelLength = profilsBoxParameters._oddPixelLength;

    if (  (oddPixelWidth  == current_oddPixelWidth)
        &&(oddPixelLength == current_oddPixelLength)) {
        qDebug() << __FUNCTION__ << "same oddPixelWidth and oddPixelLength";
        return(true); //no change; it's ok
    }

    //@LP altering length does not remove the automatic distribution ID
    //handle case with only length changed
    if (  (oddPixelLength != current_oddPixelLength)
        &&(oddPixelWidth  == current_oddPixelWidth)) {
        qDebug() << __FUNCTION__ << "oddPixelLength != current_oddPixelLength";
        _computationCore_iqaTableModel.setLength_ofOneProfilsBox(_currentBoxIdInVectOrderAlongRoute, oddPixelLength);

        _appFile_project.setState_modified(true);
        return(true);
    }

    //handle width change
    Route& refRoute = _routeContainer.getRouteRef(0);
    if (refRoute.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: no route !";
        return(false);
    }

    //update the the length on the profilsBoxParameters; which will be provided to checkThatABoxFromAGivenLocationAndWidthFitInRouteWithNoLocationChange
    profilsBoxParameters._oddPixelLength = oddPixelLength;

    S_ProfilsBoxParameters profilBoxParametersWithChanges;
    bool bProfilBoxFit = false;
    bool bProfilBoxWithChangesIncludeCenterPointRelocation = false;

    BoxOrienterAndDistributer boxOrientAndDistrib;
    bool bReport = boxOrientAndDistrib.checkThatABoxFromAGivenLocationAndWidthFitInRouteWithNoLocationChange(
        &refRoute, profilsBoxParameters, oddPixelWidth,
        profilBoxParametersWithChanges,
        bProfilBoxFit, bProfilBoxWithChangesIncludeCenterPointRelocation);

    if (!bReport) {
        qDebug() << __FUNCTION__ << " boxOrientAndDistrib.checkThatABoxFromAGivenLocationAndLengthFitInRouteWithNoLocationChange() said " << bReport;
        return(false);
    }

    if (!bProfilBoxFit) {
        qDebug() << __FUNCTION__ << " bProfilBoxFit got: " << bProfilBoxFit;

        send_currentBoxIdInVectOrderAlongRoute_and_boxSize_toView(_currentBoxIdInVectOrderAlongRoute);//replace in ui the incorrect value from the user
        emit signal_messageBoxForUser("Box width",  QString::number(oddPixelWidth) + " is too big to fit");

        return(false);
    }

    //@LP altering width will remove the automatic distribution ID relationship if it was 'box close a possible'
    _computationCore_iqaTableModel.updateProfilsBoxParameters_noBoxMove(_currentBoxIdInVectOrderAlongRoute, profilBoxParametersWithChanges);

    _appFile_project.setState_modified(true);

    return(true);
}



bool ApplicationModel::testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(
    const QVector<int>& qvectShiftToTest,

    QHash<int, bool>& qhash_shift_bPossibleShift,
    QHash<int, S_ProfilsBoxParametersForRelativeMoveAlongRoute>& qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute) {

    qDebug() << __FUNCTION__ << "(ApplicationModel) entering";

    if (_currentBoxIdInVectOrderAlongRoute == -1) {
        qDebug() << __FUNCTION__ << "(ApplicationModel) _currentBoxId is -1";
        return(false);
    }

    int qhash_shift_bPossibleShift_size = qhash_shift_bPossibleShift.size();
    if (!qhash_shift_bPossibleShift_size) {
        qDebug() << __FUNCTION__ << "(ApplicationModel) qhash_shift_bPossibleShift_size is empty";
        return(false);
    }


    S_ProfilsBoxParameters profilsBoxParameters;

    bool bGot = _computationCore_iqaTableModel.get_oneProfilsBoxParametersFromvectBoxId(_currentBoxIdInVectOrderAlongRoute, profilsBoxParameters);
    if (!bGot) {
        qDebug() << __FUNCTION__ << "(ApplicationModel) error: fail to get_OneProfilsBoxParametersFromvectBoxId with _currentBoxIdInVectOrderAlongRoute = " << _currentBoxIdInVectOrderAlongRoute;
        return(false);
    }

    qDebug() << __FUNCTION__ << "(ApplicationModel) profilsBoxParameters of _currentBoxId ( " << _currentBoxIdInVectOrderAlongRoute << " ) is: ";
    profilsBoxParameters.showContent();

    //handle width change
    Route& refRoute = _routeContainer.getRouteRef(0);
    if (refRoute.getId() == -1) {
        qDebug() << __FUNCTION__ << "(ApplicationModel) error: no route !";
        return(false);
    }

    BoxOrienterAndDistributer boxOrientAndDistrib;
    bool bReport = boxOrientAndDistrib.testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(
        &refRoute, profilsBoxParameters, qvectShiftToTest,
        qhash_shift_bPossibleShift, qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute);

    qDebug() << __FUNCTION__ << "(ApplicationModel) boxOrientAndDistrib.testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId() said " << bReport;

    return(bReport);
}

void ApplicationModel::setVisible_startAndEndofRoute(bool state) {
    _graphicsItemContainer.setVisible_startAndEndOfRoute(state);
}

void ApplicationModel::set_projectContentStateS_as_noneBox() {
    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    _projectContentState.setState_boxes(e_Project_boxesContentState::e_P_boxesCS_noneBox);
    _projectContentState.setState_stackedProfilComputed(e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_noneComputed); //consider the first insertion as a mixed also
    _projectContentState.setState_stackedProfilEdited(e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable);
}

void ApplicationModel::set_graphicsSettings_profil(S_GraphicsParameters_Profil sGP_profil) {
    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    //sGP_profil.debugShow();
    _graphicAppSettings.setFrom_graphicsParameters_Profil(sGP_profil);
    _sGgraphicsParameters_profil = sGP_profil;
}

void ApplicationModel::save_graphicsSettings() {
    _graphicAppSettings.save();
}

void ApplicationModel::set_graphicsSettings_itemOnImageView(S_GraphicsParameters_ItemOnImageView sGP_itemOnImageView) {
    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    sGP_itemOnImageView.debugShow_sgpp();
    _graphicAppSettings.setFrom_graphicsParameters_ItemOnImageView(sGP_itemOnImageView);
    _sGgraphicsParameters_itemOnImageView = sGP_itemOnImageView;

    _graphicsItemContainer.setGraphicsSettings(sGP_itemOnImageView);

}

void ApplicationModel::slot_profilAdjustMode_changed(int ieProfilAdjustMode) { //(signal emited from ui)
    qDebug() << __FUNCTION__ << "(ApplicationModel) received ieProfilAdjustMode = " << ieProfilAdjustMode;
    if (ieProfilAdjustMode == e_PAM_notSet) {
        return;
    }
    _eProfilAdjustMode = static_cast<e_ProfilAdjustMode>(ieProfilAdjustMode);
}

void ApplicationModel::selectBox(int vectBoxId) {

   qDebug() << __FUNCTION__ << "(ApplicationModel) vectBoxId = " << vectBoxId;


   int mapBoxID = -1;
   bool bGot = _computationCore_iqaTableModel.getMapBoxId_fromIndexBoxInVectOrderAlongRoute(/*_currentBoxIdInVectOrderAlongRoute*/vectBoxId, mapBoxID);

   qDebug() << __FUNCTION__ << "(ApplicationModel) bGot = " << bGot;
   qDebug() << __FUNCTION__ << "(ApplicationModel) mapBoxID = " << mapBoxID;

   if (!bGot) {
       qDebug() << __FUNCTION__ << "(ApplicationModel) if (!bGot) { return; }";
       return;
   }

   slot_selectionBoxChanged(vectBoxId, mapBoxID);
}


bool ApplicationModel::projectEditFct_aboutRoute_checkRouteIsValid(QString& strError) {
    bool bReport = _routeContainer.allRoutesHaveSegment(strError);
    if (!bReport) {
        strError = "The trace must have at least one segment";
    }
    return(bReport);
}

bool ApplicationModel::loadEPSGDataListFromProjDB() {
    const QString strPathProjDB = getProjDBPath();
    return(_geoCRSSelect_inheritQATableModel.loadEPSGDataListFromProjDB(strPathProjDB));
}

geoCRSSelect_inheritQATableModel *ApplicationModel::ptrGeoCRSSelect_inheritQATableModel() {
    return(&_geoCRSSelect_inheritQATableModel);
}


void ApplicationModel::set_useGeoRefForTrace(bool bUseGeoRefForTrace) {
    _bUseGeoRefForTrace = bUseGeoRefForTrace;
}

bool ApplicationModel::get_useGeoRefForTrace() {
    return(_bUseGeoRefForTrace);
}

void ApplicationModel::set_useGeoRefForProject(bool bUseGeoRefForProject) {
    _bUseGeoRefForProject = bUseGeoRefForProject;
}

bool ApplicationModel::get_useGeoRefForProject() {
    return(_bUseGeoRefForProject);
}

void ApplicationModel::projectEditionFunction_forRouteImport_routeSet_states_bContainsRoute_bUseGeoRef(bool& bContainsRoute, bool& bRouteSetUseGeoRef) const {
    qDebug() << __FUNCTION__ <<  "'ApplicationModel)";
    _routeContainer.showContent();
    bContainsRoute = (_routeContainer.routeCount() != 0);
    bRouteSetUseGeoRef = _forRouteImport_geoRefModelImgSetAboutRouteset.get_bUseGeoRef();
}

void ApplicationModel::projectEditionFunction_forRouteImport_clearTraceSetAndTraceSelection_forRouteImport() {
    _routeContainer.clear();
    _forRouteImport_geoRefModelImgSetAboutRouteset.clear();
    _strRoutesetFilenameForRouteImport.clear();
    _selectedRouteId_forRouteImport = -1;
}

//valid for routeset and project:
void ApplicationModel::projectEditionFunction_sendStr_geoRefImagesSetStatus() {

    qDebug() << __FUNCTION__ << "(ApplicationModel)";
    _sbaStrMsg_gRIS_projectEdition.showContent();

    emit signal_sendStr_geoRefImagesSetStatus_aboutRoutesetEdition(
        _sbaStrMsg_gRIS_projectEdition._bWorldFileData_available,
        _sbaStrMsg_gRIS_projectEdition._bEPSG_available,

        _sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData,
        _sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails,

        _sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG,
        _sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails);
}



void ApplicationModel::slot_routesetEditionFunction_selectionChanged(int newlySelectedRouteId) {
    if (!_ptrCustomGraphicsScene) {
        return;
    }
    _ptrCustomGraphicsScene->routeSet_route_showAlone(newlySelectedRouteId);
}

