#include <QDebug>

#include "route.h"

#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "../../io/PixelPicker.h"

#include "../../toolbox/toolbox_math_geometry.h"

#include "InputFilesCheckerForComputation.h"

#include "../../toolbox/toolbox_pathAndFile.h"
#include "../../toolbox/toolbox_json.h"

#include "logic/mathComputation/meanMedian.h"

#include "../../mathComputation/linearRegression.h"

#include <limits> // for std::numeric_limits<double>::quiet_NaN()

#include "leftRight_sideAccess.hpp"

#include "ComputationCore_structures.h"

#include <QJsonObject>
#include <QJsonArray>

#include "IDGenerator.h"

#include "../../toolbox/toolbox_math.h"

#include "BoxOrienterAndDistributer.h"

#include "../../toolbox/toolbox_conversion.h"

#include "exportResult.h"

#include "ComputationCore.h"

S_InsertionIndex_SBASPWMPtr::S_InsertionIndex_SBASPWMPtr(): _insertionIndex(-1), _sBASPWMPtr(nullptr) {

}

S_InsertionIndex_SBASPWMPtr::S_InsertionIndex_SBASPWMPtr(int insertionIndex, S_BoxAndStackedProfilWithMeasurements* sBASPWMPtr):
    _insertionIndex(insertionIndex),
    _sBASPWMPtr(sBASPWMPtr) {

}

void S_InsertionIndex_SBASPWMPtr::showContent_insertionIdx_and_boxPointIdxSegmentOnly() const {
    qDebug() << "(S_InsertionIndex_SBASPWMPtr)" << __FUNCTION__;
    qDebug() << "(S_InsertionIndex_SBASPWMPtr)" << __FUNCTION__ << "_insertionIndex = " << _insertionIndex;
    if (!_sBASPWMPtr) {
        qDebug() << "(S_InsertionIndex_SBASPWMPtr)" << __FUNCTION__ << "_sBASPWMPtr is nullptr";
    } else {
        _sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();
    }
}

ComputationCore::ComputationCore() {
    clear();
}

/*void ComputationCore::stepProgressionForOutside(int v) {
    //@LP empty body
}

void ComputationCore::stepProgressionForOutside_terminated() {
    //@LP empty body
}*/

ComputationCore::~ComputationCore() {
    clear();
}

//@LP do not forget the layout changed stuff about QTableModel when called from inheritQTableModel
void ComputationCore::clear() {
    _routePtr = nullptr;
    _vectPtr_valueDequantization_PX1_PX2_deltaZ = nullptr;

    _eProfilOrientation = e_PO_notSet;
    _e_mainComputationMode = e_mCM_notSet;

    _vect_eComputationMethod_Perp_Parall_DeltaZ = {e_CM_notSet, e_CM_notSet, e_CM_notSet};
    qDebug() << __FUNCTION__<< "_vect_eComputationMethod_Perp_Parall_DeltaZ.size() = " << _vect_eComputationMethod_Perp_Parall_DeltaZ.size();

    _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ = {false, false, false};
    _correlationScoreMapFileAvailabilityFlags.clear(); //set all to false

    _computationParameters.clear();

    _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.clear();

    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();

    _qvectQPointF_devdebug_centerBox.clear();
    _qvectQPointF_devdebug_firstProfil_centerBox.clear();
    _qvectQPointF_devdebug_firstPointAlongRoute.clear();
    _qvect_devdebug_unitVectorForEachPointAlongRoute.clear();
    _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute.clear();

    _inputFiles.clear();

    clear_vectUptrPixelPickers();

    _qvectQPointF_devdebug_firstPointEverStackedProfil.clear();
    _qvectQPointF_devdebug_lastPoinOfFirstparallelelineStackedProfil.clear();
    _qvectQPointF_devdebug_inBetweenFirstAndLastFirstparallelelineStackedProfil.clear();

    _qvectQPointF_devdebug_firstPointsOfParalleles.clear();
    _qvectQPointF_devdebug_lastPointsOfParalleles.clear();
}


bool ComputationCore::removeSectionOfBoxes(int aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                                           int aboutQVectOrderAlongRoute_indexOfLastToRemove) {

    if (!vectBoxIdValid(aboutQVectOrderAlongRoute_indexOfFirstToRemove)) {
        return(false);
    }
    if (!vectBoxIdValid(aboutQVectOrderAlongRoute_indexOfLastToRemove)) {
        return(false);
    }
    if (aboutQVectOrderAlongRoute_indexOfFirstToRemove > aboutQVectOrderAlongRoute_indexOfLastToRemove) {
        return(false);
    }

    //remove in qmap
    for (int vectIdx = aboutQVectOrderAlongRoute_indexOfFirstToRemove; vectIdx <= aboutQVectOrderAlongRoute_indexOfLastToRemove; vectIdx++) {

        int qmapInsertionIndex = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectIdx)._insertionIndex;
        if (qmapInsertionIndex == -1) {
            continue;
        }

        //@#LP set to nullptr inside the vect as a revelator is there is an implementation error
        //_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectIdx]._sBASPWMPtr = nullptr;

        int iReport = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.remove(qmapInsertionIndex);
        qDebug() << __FUNCTION__ << "removed status in qmap: vectIdx: " << vectIdx << " => qmapInsertionIndex = " << qmapInsertionIndex <<  " iReport = " << iReport;
    }

    //update qvect along route
    QVector<S_InsertionIndex_SBASPWMPtr> qvectOf_BASPWMPtr_orderedAlongRoute_updated;

    int qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    for (int vectIdx = 0; vectIdx < qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size; vectIdx++) {
        if (  (vectIdx < aboutQVectOrderAlongRoute_indexOfFirstToRemove)
            ||(vectIdx > aboutQVectOrderAlongRoute_indexOfLastToRemove)) {
            qvectOf_BASPWMPtr_orderedAlongRoute_updated.push_back(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectIdx));
        }
    }

    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap = qvectOf_BASPWMPtr_orderedAlongRoute_updated;

    return(true);
}


//bClearOnlyBoxWhichNeedsToBeRecomputedFromImages is used when all computation parameters (including geo spatial values) are same than previous
//This is typically the case when only adding or changing existing boxes
void ComputationCore::clearComputedStakedProfilsContent(bool bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages) {

    if (bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages) {
        for (auto& iter:_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap) {
            iter._sBASPWMPtr->clearMeasurementsDataContent();
        }
        return;
    }

    for (auto& iter:_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap) {
        if (iter._sBASPWMPtr->_bNeedsToBeRecomputedFromImages) {
            iter._sBASPWMPtr->clearMeasurementsDataContent();
        }
    }
}

bool ComputationCore::isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages() {
    for (auto& iter:_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap) {
        if (iter._sBASPWMPtr->_bNeedsToBeRecomputedFromImages) {

            qDebug() << __FUNCTION__ << "first box in qvect which needs to be recomputed: _insertionIndex; _setID; _idxBoxInSet:"
                     << iter._insertionIndex
                     << iter._sBASPWMPtr->_profilsBoxParameters._sUniqueBoxID._setID
                     << iter._sBASPWMPtr->_profilsBoxParameters._sUniqueBoxID._idxBoxInSet;

            return(true);
        }
    }
    return(false);
}



void ComputationCore::setDefaultProgressDirection() {
    _eProfilOrientation = e_PO_progressDirectionIsFromFirstToLastPointOfRoute;
}

//return false if fatal error (restoration of box was impossible)
bool ComputationCore::setProgressDirection(e_ProfilOrientation eProfilOrientation, bool& bReverseDone, bool& bRejected_alreadyOnThisOrientation) {

    bRejected_alreadyOnThisOrientation = false;

    //@LP this test is very important here. Because adjustMeasurementsDueToProgressDirectionChanged just reverse content without knowing any profile orientation details
    if (eProfilOrientation == _eProfilOrientation) {
        bReverseDone = false;//
        bRejected_alreadyOnThisOrientation = true;
        return(true);//
    }

    bool bRestoredDueToDevError = false;
    bool bInternalErrorWhenComputinglRSidesLinearRegressionsModels = false;
    bool bNoFatalErrorReversing = adjustMeasurementsDueToProgressDirectionChanged(bRestoredDueToDevError,
                                                                                  bInternalErrorWhenComputinglRSidesLinearRegressionsModels);

    bReverseDone  = !bRestoredDueToDevError;

    if (bReverseDone) {
        _eProfilOrientation = eProfilOrientation;
    }

    return(bNoFatalErrorReversing);
}

e_ProfilOrientation ComputationCore::getProgressDirection() {
    return(_eProfilOrientation);
}

//@#LP call me not only at the last moment before sending values to ui
//return true if some was not set and default was applyed; false otherwise
bool ComputationCore::setDefaultComputationModeAndParameters_ifNotSet() {

    bool bSomeWasNotSet = false;

    if (_eProfilOrientation == e_PO_notSet) {
        setDefaultProgressDirection();
        bSomeWasNotSet = true;
    }

    if (_e_mainComputationMode == e_mCM_notSet) {
        _e_mainComputationMode = e_mCM_Typical_PX1PX2Together_DeltazAlone;
        bSomeWasNotSet = true;
    }

    if (_computationParameters._eBaseComputationMethod == e_BCM_notSet) {
        _computationParameters._eBaseComputationMethod = e_BCM_median;
        bSomeWasNotSet = true;
    }
    if (_computationParameters._ePixelExtractionMethod == e_PEM_notSet) {
        //@#LP this one could be set depending on the used automatic boxes distribution if only one automatic distribution was made
        _computationParameters._ePixelExtractionMethod = e_PEM_BilinearInterpolation2x2;
        bSomeWasNotSet = true;
    }

    return(bSomeWasNotSet);
}

void ComputationCore::set_baseComputationMethod(e_BaseComputationMethod baseComputationMethod) {
    _computationParameters._eBaseComputationMethod = baseComputationMethod;    
}

e_ComputationMethod ComputationCore::convert_eBaseComputationMethod_to_eComputationMethod(
        e_BaseComputationMethod baseComputationMethod,
        bool bCorrelationScoreMapFileAvailabilityFlag,
        bool bCorrelationScoreMapFileUsedForComputation
        ) {

    e_ComputationMethod eCompMethod = e_CM_notSet;

    if (  (baseComputationMethod != e_BCM_mean)
        &&(baseComputationMethod != e_BCM_median)
        ) {
        return(eCompMethod);
    }

    if (!bCorrelationScoreMapFileAvailabilityFlag) {
        if (baseComputationMethod == e_BCM_mean) {
            eCompMethod = e_CM_mean;
        } else {
            eCompMethod = e_CM_median;
        }
    } else { //correlation score map file available
        if (!bCorrelationScoreMapFileUsedForComputation) { //but not used
            if (baseComputationMethod == e_BCM_mean) {
                eCompMethod = e_CM_mean;
            } else {
                eCompMethod = e_CM_median;
            }
        } else { //and used
            if (baseComputationMethod == e_BCM_mean) {
                eCompMethod = e_CM_weightedMean;
            } else {
                eCompMethod = e_CM_weightedMedian;
            }
        }
    }

    return(eCompMethod);
}

//be sure to call this method only after convert_eBaseComputationMethod_to_eComputationMethod
e_ComputationMethod ComputationCore::get_computationMethod(int ie_CA) {
    return(_vect_eComputationMethod_Perp_Parall_DeltaZ[ie_CA]);
}

vector<e_ComputationMethod> ComputationCore::getVect_computationMethods() {
    return(_vect_eComputationMethod_Perp_Parall_DeltaZ);
}

void ComputationCore::set_vectDequantizationStepPtr(vector<ValueDequantization>* vectValueDequantization_PX1_PX2_deltaZ) {
    _vectPtr_valueDequantization_PX1_PX2_deltaZ = vectValueDequantization_PX1_PX2_deltaZ;
}

void ComputationCore::set_CorrelationScoreMapParameters_for(e_LayersAcces eLa_correlScoreLay, const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters) {
    if (eLa_correlScoreLay >= eLA_LayersCount) {
        //@#LP error msg
        return;
    }
    _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLa_correlScoreLay] = sCorrelationScoreMapParameters;
}

S_CorrelationScoreMapParameters ComputationCore::get_CorrelationScoreMapParameters_for(e_LayersAcces eLa_correlScoreLay) const {
    if (eLa_correlScoreLay >= eLA_LayersCount) {
        //@#LP error msg
        S_CorrelationScoreMapParameters sCorrScoreMapParameters_defautNoUse;
        return(sCorrScoreMapParameters_defautNoUse);
    }
    return(_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLa_correlScoreLay]);
}

void ComputationCore::setRoutePtr(Route *routePtr) {
    _routePtr = routePtr;
}

bool ComputationCore::add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
        const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
        const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
        bool bBoxesDirectionOrderIsFirstPointToLastPoint,
        int& vectIdxOfFirstInsertedBox,
        int& keyMapOfFirstInsertedBox,
        int& aboutQVectOrderAlongRoute_indexOfFirstChange,
        int& aboutQVectOrderAlongRoute_indexOfLastChange,
        bool& bCriticalErrorOccured) {

    qDebug() << __FUNCTION__ << "(ComputationCore) qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ.size() =" << qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ.size();

    bCriticalErrorOccured = false;

    if (qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ.size() != 3) {
        qDebug() << __FUNCTION__ << "(ComputationCore) qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ.size() != 3";
        return(false);
    }
    if (!qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ.count(true)) {
        qDebug() << __FUNCTION__ << "(ComputationCore) qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ all at false";
        return(false);
    }

    int nbProfilsBoxParametersToAdd = qvectProfilsBoxParameters.size();
    qDebug() << __FUNCTION__ << "(ComputationCore) nbProfilsBoxParametersToAdd: " << nbProfilsBoxParametersToAdd;
    //int vectIdxOfLastBoxBeforeAdd = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

    if (!nbProfilsBoxParametersToAdd) {
        qDebug() << __FUNCTION__ << "(ComputationCore) no profil to add";
        return(false);
    }

    int boxIdForFirstNewBoxAdd = 0;
    if (!_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.isEmpty()) {
        boxIdForFirstNewBoxAdd = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey();
        boxIdForFirstNewBoxAdd += 1;
    }
    keyMapOfFirstInsertedBox = boxIdForFirstNewBoxAdd;

    qDebug() << __FUNCTION__ << "(ComputationCore) will now insert all using first id box boxIdForFistNewBoxAdd : " << boxIdForFirstNewBoxAdd;

    if (bBoxesDirectionOrderIsFirstPointToLastPoint) {
        for (int idxProfilsBoxParameters = 0;
             idxProfilsBoxParameters < nbProfilsBoxParametersToAdd;
             idxProfilsBoxParameters++) {

            qDebug() << __FUNCTION__ << "(ComputationCore) adding BoxAndStackedProfilWithMeasurements #" << idxProfilsBoxParameters;

            qDebug() << __FUNCTION__ << "(ComputationCore) adding with content:";
            qvectProfilsBoxParameters[idxProfilsBoxParameters].showContent();

            _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.insert(boxIdForFirstNewBoxAdd + idxProfilsBoxParameters,
                                                                   S_BoxAndStackedProfilWithMeasurements(true, qvectProfilsBoxParameters[idxProfilsBoxParameters]));
        }
    } else {
        int idxProfilsBoxParametersForAdd = 0;
        for (int idxProfilsBoxParameters = nbProfilsBoxParametersToAdd-1;
             idxProfilsBoxParameters >= 0;
             idxProfilsBoxParameters--) {

            qDebug() << __FUNCTION__ << "(ComputationCore) adding BoxAndStackedProfilWithMeasurements #" << idxProfilsBoxParameters;

            qDebug() << __FUNCTION__ << "(ComputationCore) adding with content:";
            qvectProfilsBoxParameters[idxProfilsBoxParameters].showContent();

            _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.insert(
                        boxIdForFirstNewBoxAdd + idxProfilsBoxParametersForAdd,
                        S_BoxAndStackedProfilWithMeasurements(true, qvectProfilsBoxParameters[idxProfilsBoxParameters]));
            idxProfilsBoxParametersForAdd++;
        }
    }

    bool bCriticalErrorOccured_about_update_qvectOf_BASPWMPtr = false;
    bool bReport = update_qvectOf_BASPWMPtr_orderedAlongRoute_afterProfilBoxesAddedAtQMapEnd(nbProfilsBoxParametersToAdd,
                                                                              bBoxesDirectionOrderIsFirstPointToLastPoint,
                                                                              aboutQVectOrderAlongRoute_indexOfFirstChange,
                                                                              aboutQVectOrderAlongRoute_indexOfLastChange,
                                                                              bCriticalErrorOccured_about_update_qvectOf_BASPWMPtr);
    if (!bReport) {
        if (bCriticalErrorOccured_about_update_qvectOf_BASPWMPtr) {
            bCriticalErrorOccured = true;
            return(false);
        }
    }

    int firstInsertionMapIndexUsedToAddTheBoxes = boxIdForFirstNewBoxAdd;
    int lastInsertionmapIndexUsedToAddBoxes = boxIdForFirstNewBoxAdd + nbProfilsBoxParametersToAdd - 1;

    //we could set the default values in the loop above.
    //we use setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel as a unique way to set default values

    qDebug() << __FUNCTION__ << "(ComputationCore) firstInsertionMapIndexUsedToAddTheBoxes =" << firstInsertionMapIndexUsedToAddTheBoxes;
    qDebug() << __FUNCTION__ << "(ComputationCore) lastInsertionmapIndexUsedToAddBoxes = " << lastInsertionmapIndexUsedToAddBoxes;

    int qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    for (int idxVect = 0; idxVect < qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size; idxVect++) {

        if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxVect]._insertionIndex < firstInsertionMapIndexUsedToAddTheBoxes) {
            continue;
        }

        if ( _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxVect]._insertionIndex == firstInsertionMapIndexUsedToAddTheBoxes) {
            vectIdxOfFirstInsertedBox = idxVect;
        }

        if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxVect]._insertionIndex > lastInsertionmapIndexUsedToAddBoxes) {
            continue;
        }

        bool bSetDefault = setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(idxVect, qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ);
        if (!bSetDefault) {
            //@LP should never happen
            //but if it happens, consider that as a critical error. The user will have to leave the application as soon as possible.

            qWarning() << __FUNCTION__ << "(ComputationCore) setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel failed";
            qWarning() << __FUNCTION__ << "(ComputationCore) with idxVect = " << idxVect;

            bCriticalErrorOccured = true;
            return(false);
        }
    }
    return(true);
}

const QMap<int, S_BoxAndStackedProfilWithMeasurements>& ComputationCore::getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements() {
    return(_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements);
}

bool ComputationCore::setComputationParameters(const S_ComputationParameters& computationParameters) {
    //@#LP no check
    _computationParameters = computationParameters;

    qDebug() << __FUNCTION__ << "(ComputationCore)";
    _computationParameters.debugShow();

    return(true);
}

S_ComputationParameters ComputationCore::getComputationParameters() const {
    return(_computationParameters);
}

//check here that the minimal case is provided without big mistake like twice the same file for PX1-PX2 non independant computation
void ComputationCore::setMainComputationMode(e_mainComputationMode mainComputationMode) {
    _e_mainComputationMode = mainComputationMode;
}

e_mainComputationMode ComputationCore::getMainComputationMode() {
    return(_e_mainComputationMode);
}

void ComputationCore::setInputFilesAndCorrelationScoreFiles(const S_InputFiles& inputFiles) {

    _inputFiles = inputFiles;

    //follow symbolic links if any:
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ       [iela] = getPathAndFilenameFromPotentialSymlink(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ       [iela]);
        _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela] = getPathAndFilenameFromPotentialSymlink(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iela]);
    }

    qDebug() << __FUNCTION__ << " @ " <<__LINE__ ;
    _inputFiles.showContent(); //@#LP debug purpose
}

void ComputationCore::clear_vectUptrPixelPickers() {

    qDebug() << __FUNCTION__ << "#about uptr: after #1 _vectUptrPixelPickers_PX1_PX2_DeltaZ.size() = " << _vectUptrPixelPickers_PX1_PX2_DeltaZ.size();
    for(auto& IterUptrPixelPickers: _vectUptrPixelPickers_PX1_PX2_DeltaZ) {
        if (IterUptrPixelPickers != nullptr) {
            qDebug() << __FUNCTION__ << "#about uptr: listing; iter = @" << &IterUptrPixelPickers << ": PixelPicker: " << IterUptrPixelPickers.get();
        } else {
            qDebug() << __FUNCTION__ << "#about uptr: IterUptrPixelPickers is nullptr (" << &IterUptrPixelPickers << ")";
        }
    }

    for(auto& IterUptrPixelPickers: _vectUptrPixelPickers_PX1_PX2_DeltaZ) {
        IterUptrPixelPickers.reset(nullptr);
    }
    _vectUptrPixelPickers_PX1_PX2_DeltaZ.clear();

    qDebug() << __FUNCTION__ << "#about uptr: after #1 _vectUptrPixelPickers_PX1_PX2_DeltaZ.size() = " << _vectUptrPixelPickers_PX1_PX2_DeltaZ.size();
    for(auto& IterUptrPixelPickers: _vectUptrPixelPickers_PX1_PX2_DeltaZ) {
        if (IterUptrPixelPickers != nullptr) {
            qDebug() << __FUNCTION__ << "#about uptr: listing; iter = @" << &IterUptrPixelPickers << ": PixelPicker: " << IterUptrPixelPickers.get();
        } else {
            qDebug() << __FUNCTION__ << "#about uptr: IterUptrPixelPickers is nullptr (" << &IterUptrPixelPickers << ")";
        }
    }

    for(auto& IterUptrPixelPickers: _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ) {
        IterUptrPixelPickers.reset(nullptr);
    }
    _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ.clear();
}


//translate the mainmode and inputfile (possibly empty) in computable boolean
//An empty string for a file means that we do not want to compute this file. This method set boolean flags according to that.
bool ComputationCore::setcorrelScoreMapToUseForComp_checkInputFilesAccordingToMainComputationMode_andSetComputationLayersAndComponentFlags(const QVector<bool>& qvectb_correlScoreMapToUseForComp) {

    if (_e_mainComputationMode == e_mCM_notSet) {
        qDebug() << __FUNCTION__<< "error: _e_mainComputationMode is not set";
        return(false);
    }

    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        qDebug() << __FUNCTION__<< "error: invalid _e_mainComputationMode";
        return(false);
    }

    qDebug() << __FUNCTION__ << "qvectb_correlScoreMapToUseForComp = " << qvectb_correlScoreMapToUseForComp;
    if (qvectb_correlScoreMapToUseForComp.size() != 3) {
        qDebug() << __FUNCTION__<< "error: qvectb_correlScoreMapToUseForComp size not 3";
        return(false);
    }

    for (int ieLAC = eLA_CorrelScoreForPX1PX2Together; ieLAC <= eLA_CorrelScoreForDeltaZAlone; ieLAC++) {
        _correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[ieLAC] = qvectb_correlScoreMapToUseForComp[ieLAC];
        if (ieLAC == eLA_CorrelScoreForPX1PX2Together) {
            _correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[ieLAC+1] =
            _correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[ieLAC];
            ieLAC++;//go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }
    }

    _correlationScoreMapFileAvailabilityFlags.showContent(); //@#LP for debug purpose

    InputFilesForComputationMatchWithRequierement inputFilesMatchWithRequierement(_inputFiles, _e_mainComputationMode, _correlationScoreMapFileAvailabilityFlags);

    //check file sync with other, it will go before call at the end
    QString strCheckErrorDetails;
    if (!inputFilesMatchWithRequierement.check_PX1PX2Together_DeltazAlone(strCheckErrorDetails)) {
        return(false);
    }

    for (uint ielaA = eLA_PX1; ielaA <= eLA_deltaZ; ielaA++) {

        _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[ielaA] = inputFilesMatchWithRequierement.getLayerToUseForComputationFlag(static_cast<e_LayersAcces>(ielaA));
        qDebug() << __FUNCTION__ << "_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ for " << ielaA << "set to :"
                 << _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[ielaA];
    }

    qDebug() << __FUNCTION__ << "_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ = "
             << _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;

    return(computeAndInit_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode());

}

bool ComputationCore::vectBoxIdValid(int vectBoxId) const {
    if (vectBoxId < 0) {
        return(false);
    }

    if (vectBoxId >= _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.count()) {
        return(false);
    }
    return(true);
}


bool ComputationCore::compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode(
        const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ,
        const e_mainComputationMode eMainComputationMode,
        QVector<bool>& qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ) const {

    if (qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ.size() != 3 ) {
        qDebug() << __FUNCTION__<< "error: qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ size not 3";
        return(false);
    }

    qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ.fill(false, 3);

    if (eMainComputationMode == e_mCM_notSet) {
        qDebug() << __FUNCTION__<< "error: _e_mainComputationMode is not set";
        return(false);
    }

    if (eMainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        qDebug() << __FUNCTION__<< "error: invalid _e_mainComputationMode";
        return(false);
    }

    bool bPX1_set = qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ[eLA_PX1];
    bool bPX2_set = qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ[eLA_PX2];

    if (bPX1_set != bPX2_set) {
        qDebug() << __FUNCTION__<< "error: invalid PX1 PX2 flags for e_mCM_Typical_PX1PX2Together_DeltazAlone";
        return(false);
    }

    qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ[e_CA_Perp]   = bPX1_set && bPX2_set;
    qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ[e_CA_Parall] = bPX1_set && bPX2_set;

    qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ[e_CA_deltaZ] = qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ[eLA_deltaZ];

    return(true);
}

bool ComputationCore::computeAndInit_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode() {
    return(compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode(
        _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ, _e_mainComputationMode,
        _qvectb_componentsToCompute_Perp_Parall_DeltaZ));
}

//set a minimal context to check that default values for linear regression ranges are performed
bool ComputationCore::__forUnitTestOnly__set_layers_components_end_mainComputationMode(
        const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ,
        const QVector<bool>& qvectb_componentsToCompute_Perp_Parall_DeltaZ,
        e_mainComputationMode e_mainComputationMode) {

    qDebug() << __FUNCTION__ << "(ComputationCore) entering";

    bool bInputArgsOK = true;
    bInputArgsOK &= (qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ.size() == 3);
    bInputArgsOK &= (qvectb_componentsToCompute_Perp_Parall_DeltaZ.size() == 3);
    bInputArgsOK &= (e_mainComputationMode == e_mCM_Typical_PX1PX2Together_DeltazAlone);
    if (!bInputArgsOK) {
        qDebug() << __FUNCTION__ << "(ComputationCore) : input args not OK";
        return(false);
    }
    _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ = qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;
    _qvectb_componentsToCompute_Perp_Parall_DeltaZ = qvectb_componentsToCompute_Perp_Parall_DeltaZ;
    _e_mainComputationMode = e_mainComputationMode;
    return(true);
}

//used when loading a json file about input files not checked (existence, consistency, etc)
//this is due to the way of checking a json file when loading it, before doing the extended input files checking
/*bool ComputationCore::set_layersToUseForComputationFlags_and_mainComputationMode_AndCompute_componentsToComputeFlags(
       const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ,
       e_mainComputationMode eMainComputationMode) {
    _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ = qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;
    _e_mainComputationMode = eMainComputationMode;
    return(compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode());
}*/

bool ComputationCore::initPixelPickers() {

    clear_vectUptrPixelPickers();

    if (_e_mainComputationMode == e_mCM_notSet) {
        qDebug() << __FUNCTION__<< "error: _e_mainComputationMode is not set";
        return(false);
    }

    if (!_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ.count(true)) {
        qDebug() << __FUNCTION__<< "error: _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ.count(true) is zero";
        return(false);
    }

    qDebug() << __FUNCTION__ << "#about uptr: reset(nullptr) any existing in _vectUptrPixelPickers_PX1_PX2_DeltaZ";
    qDebug() << __FUNCTION__ << "#about uptr: before #1 _vectUptrPixelPickers_PX1_PX2_DeltaZ.size() = "
             << _vectUptrPixelPickers_PX1_PX2_DeltaZ.size();
    _vectUptrPixelPickers_PX1_PX2_DeltaZ.resize(3);
    qDebug() << __FUNCTION__ << "#about uptr: after resize(3) _vectUptrPixelPickers_PX1_PX2_DeltaZ.size() = "
             << _vectUptrPixelPickers_PX1_PX2_DeltaZ.size();

    //@#LP dev check:
    for(auto& IterUptrPixelPickers: _vectUptrPixelPickers_PX1_PX2_DeltaZ) {
        if (IterUptrPixelPickers != nullptr) { qDebug() << __FUNCTION__ << "#about uptr: iter: before .reset(nullptr) IterUptrPixelPickers = @" << &IterUptrPixelPickers << ": PixelPicker: " << IterUptrPixelPickers.get();
        } else { qDebug() << __FUNCTION__ << "#about uptr: iter: before .reset(nullptr) IterUptrPixelPickers is nullptr (" << &IterUptrPixelPickers << ")"; }

        IterUptrPixelPickers.reset(nullptr);

        if (IterUptrPixelPickers != nullptr) { qDebug() << __FUNCTION__ << "#about uptr: iter: after .reset(nullptr) IterUptrPixelPickers = @" << &IterUptrPixelPickers << ": PixelPicker: " << IterUptrPixelPickers.get();
        } else { qDebug() << __FUNCTION__ << "#about uptr: iter: after .reset(nullptr) IterUptrPixelPickers is nullptr (" << &IterUptrPixelPickers << ")";  }
    }


    for (uint iLayerToUseForComputation = eLA_PX1; iLayerToUseForComputation <= eLA_deltaZ; iLayerToUseForComputation++) {
        if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[iLayerToUseForComputation]) {

            qDebug() << __FUNCTION__ << "make_unique<PixelPicker> for iLayerToUseForComputation = " << iLayerToUseForComputation;

            _vectUptrPixelPickers_PX1_PX2_DeltaZ[iLayerToUseForComputation] = make_unique<PixelPicker>();

            //@#LP: remove 'WithDataTypeCheck'
            bool bReportDisMap = _vectUptrPixelPickers_PX1_PX2_DeltaZ[iLayerToUseForComputation]->setInputImage(
                        _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iLayerToUseForComputation].toStdString());

            if (!bReportDisMap) {
                clear_vectUptrPixelPickers();
                return(false);
            }
        }
    }
    //---

    _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ.resize(3);

    //unsigned char typeDescBaseTypeCorrelationFile = TypeDesc::UINT8;
    if (_e_mainComputationMode == e_mCM_Typical_PX1PX2Together_DeltazAlone) {

        if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
            _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = make_unique<PixelPicker>();
            //@#LP: remove 'WithDataTypeCheck'
            bool bReportPX1PX2Together = _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]->setInputImage(
                _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].toStdString());

            if (!bReportPX1PX2Together) {
                clear_vectUptrPixelPickers();
                return(false);
            }
        }
        if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ) {
            _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] = make_unique<PixelPicker>();
            //@#LP: remove 'WithDataTypeCheck'
            bool bReportDeltaZ = _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]->setInputImage(
                _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].toStdString());

            if (!bReportDeltaZ) {
                clear_vectUptrPixelPickers();
                return(false);
            }
        }
    } else {
        qDebug() << __FUNCTION__ << "#not implemented";
        return(false);
    }

    return(true);
}

int ComputationCore::countValuesNotFiltered(const QVector<e_filterMask>& qvect_filterMask) {
    int nbValuesNotFiltered = qvect_filterMask.size();
    for(auto iterFilterMask: qvect_filterMask) {
        if (iterFilterMask != e_fM_keepForComputation) {
            nbValuesNotFiltered--;
        }
    }
    return(nbValuesNotFiltered);
}

bool ComputationCore::setFilterMaskFlags_nan(
        const QVector<float>& qvect_notFilteredValues,
        QVector<e_filterMask>& qvect_filterMaskToSet,
        int& nbValuesWhichStayAfterFilter) {

    qDebug() << __FUNCTION__ << "at enter:";
    qDebug() << __FUNCTION__ << "qvect_notFilteredValues =" << qvect_notFilteredValues;
    qDebug() << __FUNCTION__ << "qvect_filterMaskToSet =" << qvect_filterMaskToSet;
    qDebug() << __FUNCTION__ << "nbValuesWhichStayAfterFilter =" << nbValuesWhichStayAfterFilter;

    if (!qvect_notFilteredValues.size()) {
        nbValuesWhichStayAfterFilter = 0;
        qDebug() << __FUNCTION__ << " nothing to filter (empty vector value)";
        return(true);
    }

    bool bSizeMatchCheck = true;

    bSizeMatchCheck &= qvect_notFilteredValues.size() == qvect_filterMaskToSet.size();


    if (!bSizeMatchCheck) {
        qDebug() << __FUNCTION__ << " error: some vectors size don't match";
        return(false);
    }

    //filter nan
    int idx = 0;
    int newlyFiltered = 0;
    for(auto iterValue: qvect_notFilteredValues) {
        if (qvect_filterMaskToSet[idx] != e_fM_keepForComputation) {
            idx++;
            continue;
        }
        if (std::isnan(iterValue)) {
            qvect_filterMaskToSet[idx] = e_fM_isNan;
            newlyFiltered++;
        }/* else {
           if (isinf(iterValue)) {
               qvect_filterMaskToSet[idx] = e_fM_isInf;
               newlyFiltered++;
           }
        }*/
        idx++;
    }

    nbValuesWhichStayAfterFilter -= newlyFiltered;
    if (!nbValuesWhichStayAfterFilter) {
        qDebug() << __FUNCTION__ << " (#1) none value stays after filter";
        return(true);
    }

    return(true);
}

#include <algorithm>

void ComputationCore::set_pixelExtractionMethod(e_PixelExtractionMethod pixelExtractionMethod) {
    _computationParameters._ePixelExtractionMethod = pixelExtractionMethod;
}

//here the _pixelPicker_* must be ready to be used
bool ComputationCore::compute_stakedProfilsAlongRoute() {

    if (_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.isEmpty()) {
        qDebug() << __FUNCTION__<< "error: _qmap_boxId_BoxAndStackedProfilWithMeasurements is empty";
        return(false);
    }

    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        qDebug() << __FUNCTION__<< "error: mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone";
        return(false);
    }

    if (_eProfilOrientation == e_PO_notSet) {
        qDebug() << __FUNCTION__<< "error: _eProfilOrientation is ePO_notSet";
        return(false);
    }

    if (_computationParameters._ePixelExtractionMethod == e_PEM_notSet) {
        qDebug() << __FUNCTION__<< "error: _e_pixelExtractionMethod not set";
        return(false);
    }

    qDebug() << __FUNCTION__<< "_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ.size() = " << _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ.size();
    qDebug() << __FUNCTION__<< "_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ = "        << _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;

    qDebug() << __FUNCTION__<< "_vect_eComputationMethod_Perp_Parall_DeltaZ.size() = " << _vect_eComputationMethod_Perp_Parall_DeltaZ.size();

    if (!_vectPtr_valueDequantization_PX1_PX2_deltaZ) {
        qDebug() << __FUNCTION__<< "error: _vectPtr_valueDequantization_PX1_PX2_deltaZ is nullptr";
        return(false);
    }

    if (_vectPtr_valueDequantization_PX1_PX2_deltaZ->size() != 3) {
        qDebug() << __FUNCTION__<< "error: _vectPtr_valueDequantization_PX1_PX2_deltaZ size is not 3";
        return(false);
    }

    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {
        bool bDequantValid = true;
        bDequantValid &= (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[eLA_PX1].isDequantValid();
        bDequantValid &= (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[eLA_PX2].isDequantValid();
        if (!bDequantValid) {
            qDebug() << __FUNCTION__<< "error: some valueDequantization for PX1PX2Together are invalid";
            return(false);
        }
    }
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {
        bool bDequantValid = true;
        bDequantValid &= (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[eLA_deltaZ].isDequantValid();
        if (!bDequantValid) {
            qDebug() << __FUNCTION__<< "error: valueDequantization for eLA_deltaZ is invalid";
            return(false);
        }
    }

    if (   _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]
        != _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX2]) {
        return(false);
    }
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {

        if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._weighting._bUse) {
            if ( (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._weighting._f_weightExponent < 1.0f)
               ||(_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._weighting._f_weightExponent > 6.0f)) {
                qDebug() << __FUNCTION__<< "error: _f_weightExponent out of range for eLA_CorrelScoreForPX1PX2Together";
                return(false);
            }
        }

        _vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp] =
            convert_eBaseComputationMethod_to_eComputationMethod(
                _computationParameters._eBaseComputationMethod,
                _correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2,
                _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._bUse
                );

        if (_vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp] == e_CM_notSet) {
            qDebug() << __FUNCTION__<< "error: ComputationMethod deduced as e_CM_notSet (layer:#" << eLA_PX1;
            return(false);
        }
        _vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Parall] = _vect_eComputationMethod_Perp_Parall_DeltaZ[e_CA_Perp];

    }

    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {

        if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]._weighting._bUse) {
            if ( (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]._weighting._f_weightExponent < 1.0f)
               ||(_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]._weighting._f_weightExponent > 6.0f)) {
                qDebug() << __FUNCTION__<< "error: _f_weightExponent out of range for eLA_CorrelScoreForDeltaZAlone";
                return(false);
            }
        }
        _vect_eComputationMethod_Perp_Parall_DeltaZ[eLA_deltaZ] =
            convert_eBaseComputationMethod_to_eComputationMethod(
                _computationParameters._eBaseComputationMethod,
                _correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ,
                _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]._bUse/*,
                _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]._weighting._bUse*/);
        if (_vect_eComputationMethod_Perp_Parall_DeltaZ[eLA_deltaZ] == e_CM_notSet) {
            qDebug() << __FUNCTION__<< "error: ComputationMethod deduced as e_CM_notSet (layer:#" << eLA_deltaZ;
            return(false);
        }
    }

    bool bInitPixelPickersReport = initPixelPickers();
    if (!bInitPixelPickersReport) {        
       //@#LP add msg error
       return(false);
    }

    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {
        //PX1 & PX2 PixelPickers
        for (uint ieLA = eLA_PX1; ieLA <= eLA_PX2; ieLA++) {
            if (_vectUptrPixelPickers_PX1_PX2_DeltaZ[ieLA] == nullptr) {
                qDebug() << __FUNCTION__<< "error: PixelPicker not allotated for layer #" << ieLA;
                return(false);
            }
        }
        //correlation score PixelPicker for PX1PX2Together
        if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
            if (_vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] == nullptr) {
                qDebug() << __FUNCTION__<< "error: correlation score PixelPicker not allocated PX1PX2 together";
                return(false);
            }
        }
    }

    //deltaZ PixelPicker
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {
        if (_vectUptrPixelPickers_PX1_PX2_DeltaZ[eLA_deltaZ] == nullptr) {
            qDebug() << __FUNCTION__<< "error: PixelPicker not allotated for deltaZ";
            return(false);
        }
        //correlationsCore deltaZ PixelPicker
        if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ) {
            if (_vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_deltaZ] == nullptr) {
                qDebug() << __FUNCTION__<< "error: correlation score PixelPicker not allocated for layer deltaZ";
                return(false);
            }
        }
    }

    int nbBox = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    int iBoxId = 0;
    for (iBoxId = 0; iBoxId < nbBox; iBoxId++) {

        stepProgressionForOutside(iBoxId);

        bool bDebugGettingContent = false;//@###LP remove me

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[iBoxId]._sBASPWMPtr;

        qDebug() << __FUNCTION__ << __LINE__ << "iBoxId = " << iBoxId << "; _locationFromZeroOfFirstInsertedValueInStackedProfil._value: " << SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value;

        if (!(SBASPWSPtr->_bNeedsToBeRecomputedFromImages)) {
            qDebug() << __FUNCTION__ << "no need to recompute boxId #" << iBoxId;
            continue;
        }

        qDebug() << __FUNCTION__ << " call SBASPWSPtr->clearMeasurementsDataContent()";
        SBASPWSPtr->clearMeasurementsDataContent();

        qDebug() << "compute box id #" << iBoxId; //boxIterator._id;

        SBASPWSPtr->showContent_mainMeasurement_linearRgressionModel(); //@#LP dbg here not cleared (it's ok !)

        //--- in qcs (qt image coordinate system) --
        //from box center point, compute from counter clock wise side to the other side, from behind to front

        Vec2<double> vec2dbNormalizedBoxUnitVectorDirection {
            SBASPWSPtr->_profilsBoxParameters._unitVectorDirection.x(),
            SBASPWSPtr->_profilsBoxParameters._unitVectorDirection.y()
        };

        //box unit vector direction from project file or in memory is always from start to end
        //for profil computation we have to take into account the profil orientation
        //and hence, reverse the box unit vector if the profil orientation if from end to start
        //Note that in the result export file, the box unit vector (and linked parameters) are sync with the profil orientation
        //and can be reversed from what is stored in the project file
        if (_eProfilOrientation == e_PO_progressDirectionIsFromLastToFirstPointOfRoute) {
            vec2dbNormalizedBoxUnitVectorDirection = -vec2dbNormalizedBoxUnitVectorDirection;
        }

        Vec2<double> vec2dblNormalized_perpendicularToSegmentCounterclockwise {
            -vec2dbNormalizedBoxUnitVectorDirection.y,
             vec2dbNormalizedBoxUnitVectorDirection.x
        };

        Vec2<double> vec2dblNormalized_perpendicularToSegmentClockwise = -vec2dblNormalized_perpendicularToSegmentCounterclockwise;

        int evenLengthOneSide = SBASPWSPtr->_profilsBoxParameters._oddPixelLength/2;
        //qDebug() << "evenWidthOneSide =" << evenWidthOneSide;

        //@LP to avoid test for each point if inside the image:
        //find the top-left, top-right, bottom-left and bottom-right corner of the box and check that all these corners
        //(dilated by one to take into account the get bilinear pixel) are inside the image.

        //minus from counter clock wise
        SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil.set(static_cast<double>(-evenLengthOneSide));

        qDebug() << __FUNCTION__ << "locationFromZeroOfFirstInsertedValueInStackedProfil._value = " << SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value;

        for (int lengthOneSideMove = evenLengthOneSide; lengthOneSideMove >=-evenLengthOneSide; lengthOneSideMove--) {

            qDebug() << "lengthOneSideMove =" << lengthOneSideMove;

            //from the center
            Vec2<double> vec2dbSlidingPoint_parallelsToBoxVectorDirection {
                SBASPWSPtr->_profilsBoxParameters._qpfCenterPoint.x(),
                SBASPWSPtr->_profilsBoxParameters._qpfCenterPoint.y()
            };
            //move to the parallel line to box vector direction corresponding to the length slice (across all the profils box)
            vec2dbSlidingPoint_parallelsToBoxVectorDirection+=static_cast<qreal>(lengthOneSideMove)*vec2dblNormalized_perpendicularToSegmentClockwise;

            //move to the starting point to slide along the parallele
            int maxWidthBehindCenter =  SBASPWSPtr->_profilsBoxParameters._oddPixelWidth/2;
            vec2dbSlidingPoint_parallelsToBoxVectorDirection-=static_cast<qreal>(maxWidthBehindCenter)*vec2dbNormalizedBoxUnitVectorDirection;
            //qDebug() << "boxIterator._oddPixelWidth =" << boxIterator._oddPixelWidth;


            if (lengthOneSideMove == evenLengthOneSide) {
                _qvectQPointF_devdebug_firstPointsOfParalleles.push_back(QPointF {vec2dbSlidingPoint_parallelsToBoxVectorDirection.x,
                                                                                  vec2dbSlidingPoint_parallelsToBoxVectorDirection.y } );
            }
            if (lengthOneSideMove == (-evenLengthOneSide)) {
                _qvectQPointF_devdebug_lastPointsOfParalleles.push_back(QPointF {vec2dbSlidingPoint_parallelsToBoxVectorDirection.x,
                                                                                 vec2dbSlidingPoint_parallelsToBoxVectorDirection.y } );
            }

            //Parallele Line To Box Vector Direction:
            vector<double> vect_correlationScore[eLA_LayersCount]; // correlation score values for one parallele (PX1 and PX2 will have the same)

            vector<double> vect_Perp_Parall_DeltaZ[e_CA_ComponentsCount];//


            //if the correlation score is invalid (nan or Inf) or rejected dut to minimum defined for computation nor reached (typically 0.70),
            //the correlation score is set to NAN and the pixel values are set to NAN (pushed in the parallele)

            //the correlation score is set to NAN also in case of pixel outside the image
            //the pixel values are set to NAN (pushed in the parallele) also

            qDebug() << " boxAndStackIterator._profilsBoxParameters._oddPixelWidth = " << SBASPWSPtr->_profilsBoxParameters._oddPixelWidth;

            for (int w = 0; w < SBASPWSPtr->_profilsBoxParameters._oddPixelWidth; w++) {


                //workaround to try to avoid to have unrefreshed black progresstask dialog for a too long time
                //(using a qDialog with exec() and using a thread to compute the stacked profile is an alternative which seems to works also. But first tests show that it need stabilization, and code priority was not on this improvement. Hence, not used.
                if ((iBoxId == 0) && (w == 3)) {
                    stepProgressionForOutside(iBoxId);
                }



                //debug ---
                /*if (lengthOneSideMove == evenWidthOneSide) {

                    if (w == 0) {
                        //add a debug point for w = 0
                        _qvectQPointF_devdebug_firstPointEverStackedProfil.push_back(QPointF {
                                                                                   vec2dbSlidingPoint_parallelsToBoxVectorDirection.x,
                                                                                   vec2dbSlidingPoint_parallelsToBoxVectorDirection.y } );
                    } else {
                        if (w == SBASPWSPtr->_profilsBoxParameters._oddPixelWidth-1) {
                            _qvectQPointF_devdebug_lastPoinOfFirstparallelelineStackedProfil.push_back(QPointF {
                                                                                       vec2dbSlidingPoint_parallelsToBoxVectorDirection.x,
                                                                                       vec2dbSlidingPoint_parallelsToBoxVectorDirection.y } );
                        } else {
                            _qvectQPointF_devdebug_inBetweenFirstAndLastFirstparallelelineStackedProfil.push_back(QPointF {
                                                                                       vec2dbSlidingPoint_parallelsToBoxVectorDirection.x,
                                                                                       vec2dbSlidingPoint_parallelsToBoxVectorDirection.y } );
                        }
                    }
                }*/
                //---

                if (_e_mainComputationMode == e_mCM_Typical_PX1PX2Together_DeltazAlone) {

                    bool bTryDeltaZ = true;

                    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {

                        qDebug() << __FUNCTION__<< "continousPixelXY: ("
                                 << vec2dbSlidingPoint_parallelsToBoxVectorDirection.x << ", "
                                 << vec2dbSlidingPoint_parallelsToBoxVectorDirection.y << " )";

                        //use _pixelPicker_PX1 to check that the pixel is inside the image
                        bool bOnBorder = false;
                        bool bPixelInsideOfImage = _vectUptrPixelPickers_PX1_PX2_DeltaZ[eLA_PX1]->imageContainsContinousPixelXY(vec2dbSlidingPoint_parallelsToBoxVectorDirection, bOnBorder);
                        if (  (!bPixelInsideOfImage)  //out of image
                            ||( (_computationParameters._ePixelExtractionMethod == e_PEM_BilinearInterpolation2x2) && bOnBorder ) ) { //bilinear and to close to border
                           //push none value
                           //Avoid any try about deltaZ
                            bTryDeltaZ = false;

                            qDebug() << __FUNCTION__<< "continousPixelXY is considered out of image";

                        } else { //pixel inside image

                            double correlationScoreValue = .0;

                            double Perp_Parall[2] = {.0 ,.0};
                            e_filterMask filterMask_Perp_Parall[2] = {e_fM_noSet, e_fM_noSet};

                            qDebug() << __FUNCTION__<< "continousPixelXY inside the image";

                            bool bPickPixels = pickPixels_correlationScore_PX1PX2Together(
                                        vec2dbSlidingPoint_parallelsToBoxVectorDirection,
                                        vec2dbNormalizedBoxUnitVectorDirection,
                                        correlationScoreValue,// correlation score values for one parallele  (PX1 and PX2 will have the same when non independant)
                                        Perp_Parall, //layerPixelValue_PX1_PX2,//values for one parallele by layers (PX1, PX2, DeltaZ)
                                        filterMask_Perp_Parall,
                                        bDebugGettingContent); //filterMaskValue_PX1_PX2);//filter mask for one parallele by layers (PX1, PX2, DeltaZ)
                            if (!bPickPixels) {
                                qDebug() << __FUNCTION__<< "error in pickPixels_correlationScore_PX1_PX2_DeltaZ(...)";

                                return(false);
                            }

                            bool bKeepPerp   = (filterMask_Perp_Parall[e_CA_Perp] == e_fM_keepForComputation);
                            bool bPerp_isnan = std::isnan(Perp_Parall[e_CA_Perp]);

                            //qDebug() << __FUNCTION__<< "layerPixelValue_PX1_PX2[eLA_PX1] = " << layerPixelValue_PX1_PX2[eLA_PX1] << " ; bKeepPX1 = " << bKeepPX1 << " ;  bPX1_isnan = " << bPX1_isnan;


                            if  (bKeepPerp && !bPerp_isnan) {
                                qDebug() << __FUNCTION__<< "if  (bKeepPerp && !bPerp_isnan) {";

                                bool bKeepParall   = (filterMask_Perp_Parall[e_CA_Parall] == e_fM_keepForComputation);
                                bool bParall_isnan = std::isnan(Perp_Parall[e_CA_Parall]);
                                if (bKeepParall && !bParall_isnan) {

                                    qDebug() << __FUNCTION__<< "if (bKeepParall && !bParall_isnan) {";

                                    if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
                                          if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._bUse) {
                                                vect_correlationScore[eLA_CorrelScoreForPX1PX2Together].push_back(correlationScoreValue);
                                          }
                                    }

                                    vect_Perp_Parall_DeltaZ[e_CA_Perp  ].push_back(Perp_Parall[e_CA_Perp]);
                                    vect_Perp_Parall_DeltaZ[e_CA_Parall].push_back(Perp_Parall[e_CA_Parall]);

                                    qDebug() << __FUNCTION__ << "Perp_Parall[e_CA_Perp]   = " << Perp_Parall[e_CA_Perp];
                                    qDebug() << __FUNCTION__ << "Perp_Parall[e_CA_Parall] = " << Perp_Parall[e_CA_Parall];

                                    //cout << "cout Perp   v=" << std::setprecision(14) << Perp_Parall[e_CA_Perp]  << "\n";
                                    //cout << "cout Parall v=" << std::setprecision(14) << Perp_Parall[e_CA_Parall]  << "\n";

                                } else {
                                    qDebug() << __FUNCTION__<< "rejected: bKeepParall  = " << bKeepParall << "; bParall_isnan = " << bParall_isnan;
                                }
                            } else {
                                qDebug() << __FUNCTION__<< "rejected: bKeepPerp  = " << bKeepPerp << "; bPerp_isnan = " << bPerp_isnan;
                            }
                        }
                    } else {

                        //check that the pixel is inside the image
                        bool bOnBorder = false;
                        bool bPixelInsideOfImage = _vectUptrPixelPickers_PX1_PX2_DeltaZ[eLA_deltaZ]->imageContainsContinousPixelXY(vec2dbSlidingPoint_parallelsToBoxVectorDirection, bOnBorder);
                        if (  (!bPixelInsideOfImage)  //out of image
                            ||( (_computationParameters._ePixelExtractionMethod == e_PEM_BilinearInterpolation2x2) && bOnBorder ) ) { //bilinear and to close to border
                           //push none value
                           //Avoid any try about deltaZ
                            bTryDeltaZ = false;
                        } else { //pixel inside image
                            //do nothing
                        }
                    }

                    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {

                        if (bTryDeltaZ) {

                            double correlationScoreValue = .0;
                            double layerPixelValue = .0;
                            e_filterMask filterMaskValue = e_fM_noSet;

                            bool bPickPixels = pickPixels_correlationScore_independantLayerDeltaZ(
                                                            vec2dbSlidingPoint_parallelsToBoxVectorDirection,
                                                            correlationScoreValue,// correlation score values for one parallele  (PX1 and PX2 will have the same when non independant)
                                                            layerPixelValue,
                                                            filterMaskValue);
                            if (!bPickPixels) {
                                qDebug() << __FUNCTION__<< "error in pickPixels_correlationScore_PX1_PX2_DeltaZ(...)";
                                return(false);
                            }

                            bool bKeepDeltaZ   = (filterMaskValue == e_fM_keepForComputation);
                            bool bDeltaZ_isnan = std::isnan(layerPixelValue);

                            qDebug() << __FUNCTION__<< "eLA_deltaZ layerPixelValue = " << layerPixelValue << " ; bKeepDeltaZ = " << bKeepDeltaZ << " ;  bDeltaZ_isnan = " << bDeltaZ_isnan;

                            if (bKeepDeltaZ && !bDeltaZ_isnan) {

                                if (_correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[eLA_deltaZ]) { //
                                    if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_deltaZ]._bUse) {
                                        vect_correlationScore[eLA_deltaZ].push_back(correlationScoreValue);
                                    }
                                }

                                vect_Perp_Parall_DeltaZ[e_CA_deltaZ].push_back(layerPixelValue);
                            }

                        } else {
                            qDebug() << __FUNCTION__ << " don't try to get DeltaZ value";
                        }
                    }
                }

                vec2dbSlidingPoint_parallelsToBoxVectorDirection += vec2dbNormalizedBoxUnitVectorDirection;

            }

            if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {
                qDebug() << __FUNCTION__ << "vect_Perp_Parall_DeltaZ[e_CA_Perp]  : " << vect_Perp_Parall_DeltaZ[e_CA_Perp];
                qDebug() << __FUNCTION__ << "vect_Perp_Parall_DeltaZ[e_CA_Parall]: " << vect_Perp_Parall_DeltaZ[e_CA_Parall];
                if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
                    if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._bUse) {
                        qDebug() << __FUNCTION__ << "vect_correlationScore[eLA_CorrelScoreForPX1PX2Together]: " << vect_correlationScore[eLA_CorrelScoreForPX1PX2Together];
                    }
                }
            }

            if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {
                qDebug() << __FUNCTION__ << "vect_Perp_Parall_DeltaZ[eLA_deltaZ]  : " << vect_Perp_Parall_DeltaZ[eLA_deltaZ];                
                if (_correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[eLA_deltaZ]) { //
                    if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_deltaZ]._bUse) {
                        qDebug() << __FUNCTION__ << "vect_correlationScore[eLA_deltaZ]: " << vect_correlationScore[eLA_deltaZ];
                    }
                }
            }

            //here pixel values for one parallele to the box vector direction are 'nan' filtered and have valid correlation score (>= to the defined value)

            //show vectors before trim:
            //
            //qDebug() << "filterMask PX1   :" << qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX1];
            //qDebug() << "filterMask PX2   :" << qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX2];
            //qDebug() << "filterMask deltaZ:" << qvect_filterMask_PX1_PX2_DeltaZ[eLA_deltaZ];

            /*qDebug() << "parallele line PX1:";
            qDebug() << vect_PX1_PX2_DeltaZ[eLA_PX1];

            qDebug() << "parallele line PX2:";
            qDebug() << vect_PX1_PX2_DeltaZ[eLA_PX2];*/

            /*if (_correlationScoreUsageFlags._uCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
                qDebug() << "parallele line correlationScore: for PX1PX2:";
                qDebug() << vect_correlationScore[eLA_CorrelScoreForPX1PX2Together];
            }

            if (_internal_layersToComputeFlags._vectb_PX1_PX2_DeltaZ[eLA_deltaZ]) {
                qDebug() << "parallele line deltaZ:";
                qDebug() << vect_PX1_PX2_DeltaZ[eLA_deltaZ];
                if (_correlationScoreUsageFlags._uCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ) {
                                qDebug() << "parallele line correlationScore for deltaZ:";
                                qDebug() << vect_correlationScore[eLA_deltaZ];
                }
            }*/

            qDebug() <<__FUNCTION__ << __LINE__;
            U_MeanMedian_computedValuesWithValidityFlag MMcomputedValuesPerp_Parall_DeltaZ[e_CA_ComponentsCount];

            bool bComputedStackedParalleleReport = computeStakedParallele_withSameCorrelScoreForPerpAndParall(vect_Perp_Parall_DeltaZ, vect_correlationScore, MMcomputedValuesPerp_Parall_DeltaZ);
            if (!bComputedStackedParalleleReport)  {
                return(false);
            }

            //allows to have different computationMethod results values storable at the same time
            for (int i_eCA = e_CA_Perp; i_eCA <= e_CA_deltaZ; i_eCA++) {
                if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[i_eCA]) {
                    e_MeanMedianIndexAccess meanMedianIndexAccess = static_cast<e_MeanMedianIndexAccess>(_vect_eComputationMethod_Perp_Parall_DeltaZ[i_eCA]);



                    SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[i_eCA].
                            _qvectqvect_aboutMeanMedian[meanMedianIndexAccess].
                            push_back(MMcomputedValuesPerp_Parall_DeltaZ[i_eCA]);

                    //qDebug() << "sz = " << stackedProfil_PX1_PX2_DeltaZ[i_eaSP]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].size();
                }
            }

            //@LP debug: //@#LP to update
            /*for (ulong i_eaSP = eLA_PX1; i_eaSP <= eLA_deltaZ; i_eaSP++) {
                if (_internal_layersToComputeFlags._vectb_PX1_PX2_DeltaZ[i_eaSP]) {
                    qDebug() << "stackedProfil_PX1_PX2_DeltaZ[" << i_eaSP << "._qvectqvect_aboutMeanMedian[" << meanMedianIndexAccess << "].size()= " <<
                    stackedProfil_PX1_PX2_DeltaZ[i_eaSP]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].size();
                }
            }*/
        }

        SBASPWSPtr->_bNeedsToBeRecomputedFromImages = false;
    }    

    stepProgressionForOutside(iBoxId);


    //@LP debug loop:
    /*
    for (int iBoxId = 0; iBoxId < nbBox; iBoxId++) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[iBoxId]._sBASPWMPtr;

        qDebug() << "debug show: box at boxId #" << iBoxId;

        qDebug() << " oddPixelWidth  :" << SBASPWSPtr->_profilsBoxParameters._oddPixelWidth;
        qDebug() << " oddPixelLength :" << SBASPWSPtr->_profilsBoxParameters._oddPixelLength;
        qDebug() << " qpfCenterPoint :" << SBASPWSPtr->_profilsBoxParameters._qpfCenterPoint;
        qDebug() << " unitVectorDirection :" << SBASPWSPtr->_profilsBoxParameters._unitVectorDirection;
        qDebug() << " idxSegmentOwnerOfCenterPoint :" << SBASPWSPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

        if (SBASPWSPtr->_bNeedsToBeRecomputedFromImages) {
            qDebug() << "Not computed (?!)";

            continue;
        }
        qDebug() << __FUNCTION__ << "Computed ";


        for (ulong ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
             qDebug() << __FUNCTION__ << " ieCA #" << ieCA;
             if (!_qvectb_componentsToCompute_Perp_Parall_DeltaZ[ieCA]) {
                qDebug() << __FUNCTION__ << " is not computed, according content should be empty:";
                qDebug() << __FUNCTION__ << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[e_MMIA_aboutMean].size();
                qDebug() << __FUNCTION__ << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[e_MMIA_aboutMedian].size();
                qDebug() << __FUNCTION__ << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[e_MMIA_aboutWMean].size();
                qDebug() << __FUNCTION__ << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[e_MMIA_aboutWMedian].size();

             } else {
                 e_MeanMedianIndexAccess meanMedianIndexAccess = static_cast<e_MeanMedianIndexAccess>(_vect_eComputationMethod_Perp_Parall_DeltaZ[ieCA]);

                 int valuesInProfils_count = SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].size();
                 for (int ivip = 0; ivip < valuesInProfils_count; ivip++) {
                     qDebug() << __FUNCTION__ << "qvectvalues: _major_centralValue = "      << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].at(ivip)._anonymMajorMinorWithValidityFlag._major_centralValue;
                     qDebug() << __FUNCTION__ << "qvectvalues: _minor_aroundCentralValue =" << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].at(ivip)._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;
                     qDebug() << __FUNCTION__ << "qvectvalues: _bValuesValid = "            << SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess].at(ivip)._anonymMajorMinorWithValidityFlag._bValuesValid;
                }
            }
        }
    }*/

    return(true);
}


bool ComputationCore::compute_leftRightSidesLinearRegressionsModel_forBoxId(int boxId,
                                                                            const S_MeasureIndex& measureIndex) {

    qDebug() << __FUNCTION__<< "(ComputationCore)" << __LINE__;
    qDebug() << __FUNCTION__<< "(ComputationCore)" << "_qmap_boxId_BoxAndStackedProfilWithMeasurements.size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    qDebug() << __FUNCTION__<< "(ComputationCore)" << ".size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    qDebug() << __FUNCTION__<< "(ComputationCore)" << "boxId = " << boxId;


    if (_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.isEmpty()) {

        qDebug() << __FUNCTION__<< " error: _qvectqlnklist_StackedProfil_PX1_PX2_DeltaZ is empty";
        return(false);
    }

    if (boxId < 0) {
        qDebug() << __FUNCTION__<< " error: boxId < 0";
        return(false);
    }
    if (boxId >= _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()) {
        qDebug() << __FUNCTION__<< " error: boxId > qvect.size";
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId]._sBASPWMPtr;

    qDebug() << __FUNCTION__<< "SBASPWSPtr @ = " << (void*)SBASPWSPtr;

    qDebug() << __FUNCTION__<< "SBASPWSPtr->showContent_mainMeasurement() :";
    SBASPWSPtr->showContent_mainMeasurement_linearRgressionModel();

    bool bComputedRegressionModel = compute_leftRightSidesLinearRegressionsModel(*SBASPWSPtr, measureIndex._bOnMainMeasure, measureIndex._secondaryMeasureIdx); //bComputeMainMeasure, secondaryMeasureIdxToCompute);
    return(bComputedRegressionModel);
}


bool ComputationCore::compute_leftRightSidesLinearRegressionsModels_forAllBoxes() {

    qDebug() << __FUNCTION__<< "(ComputationCore)" << __LINE__;
    qDebug() << __FUNCTION__<< "(ComputationCore)" << "_qmap_boxId_BoxAndStackedProfilWithMeasurements.size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    qDebug() << __FUNCTION__<< "(ComputationCore)" << ".size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    if (_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.isEmpty()) {

        qDebug() << __FUNCTION__<< " error: _qvectqlnklist_StackedProfil_PX1_PX2_DeltaZ is empty";
        return(false);
    }

    bool bComputedRegressionModel = true;
    int qvectSize = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    for (int iBoxId = 0; iBoxId < qvectSize; iBoxId++) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[iBoxId]._sBASPWMPtr;

        qDebug() << __FUNCTION__<< "SBASPWSPtr @ = " << (void*)SBASPWSPtr;

        qDebug() << __FUNCTION__<< "SBASPWSPtr->showContent_mainMeasurement() :";


        //@LP shield when coming from profil orientation change
        if (SBASPWSPtr->_bNeedsToBeRecomputedFromImages) {
            continue;
        }

        //on main measure
        bComputedRegressionModel &= compute_leftRightSidesLinearRegressionsModel(*SBASPWSPtr, true, -1);

        //on every secondary measures
        int secondaryMeasuresCount = getSecondaryMeasuresCount(iBoxId);
        for (int iSecondaryMeasure = 0; iSecondaryMeasure < secondaryMeasuresCount; iSecondaryMeasure++) {
            bComputedRegressionModel &= compute_leftRightSidesLinearRegressionsModel(*SBASPWSPtr, false, iSecondaryMeasure);
        }
    }

    return(bComputedRegressionModel);
}

int ComputationCore::getSecondaryMeasuresCount(int boxId) const {

    if (boxId < 0) {
        return(0);
    }
    if (boxId >= _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()){
        return(0);
    }

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId]._sBASPWMPtr;

    if (!SBASPWSPtr) {
        return(0);
    }

    int secondaryMeasuresCount = 0;

    //@LP we iterate on components but the value should be the same if there is more than one layer
    //for now, any secondary mesure exists for all components to compute

    //@#LP no check about same count values through all components
    for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[ieCA]) {
            secondaryMeasuresCount = SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]._LRSide_linRegrModel_secondaryMeasures.count();
        }
    }
    return(secondaryMeasuresCount);
}



bool ComputationCore::compute_leftRightSidesLinearRegressionsModel(
        S_BoxAndStackedProfilWithMeasurements& boxAndStackedProfilWithMeasurements,
        bool bComputeMainMeasure,
        int secondaryMeasureIdxToCompute) {

   qDebug() << __FUNCTION__<< __LINE__;

   boxAndStackedProfilWithMeasurements.showContent_mainMeasurement_linearRgressionModel();

    if (!boxAndStackedProfilWithMeasurements._locationFromZeroOfFirstInsertedValueInStackedProfil._bValid) {
        qDebug()  << __FUNCTION__<<  " error: _locationFromZeroOfFirstInsertedValueInStackedProfil is not valid" ;
        return(false);
    }

    if (!bComputeMainMeasure) {
        if (secondaryMeasureIdxToCompute < 0) {
            qDebug()  << __FUNCTION__<<  " error: secondaryMeasureIdxToCompute is invalid (<0) " ;
            return(false);
        }
        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[ieCA]) {

                int LRSide_linRegrModel_count =  boxAndStackedProfilWithMeasurements.
                        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA]
                        ._LRSide_linRegrModel_secondaryMeasures.count();

                if (secondaryMeasureIdxToCompute >= LRSide_linRegrModel_count) {
                    qDebug() << __FUNCTION__<< "error: secondaryMeasureIdxToCompute is >= LRSide_linRegrModel_count"
                             << __FUNCTION__<<  "( " << secondaryMeasureIdxToCompute << " >= " << LRSide_linRegrModel_count << ")";
                    return(false);
                }
            }
        }
    }

    //We use _anonymMajorMinorWithValidityFlag to get the values, but we permits to store value for more than one computation method at a time.
    //Hence we have to get the values from the according vector.

    for (int  ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
        //qDebug() << " layers #" << i_eaSP ;

        if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[ieCA]) {

            qDebug() << " make LRSide stuff for layer#" << ieCA ;

            e_MeanMedianIndexAccess meanMedianIndexAccess = static_cast<e_MeanMedianIndexAccess>(_vect_eComputationMethod_Perp_Parall_DeltaZ[ieCA]);

            double computedYForX0[2] { .0, .0};
            double stdErrorOfIntercept[2] { .0, .0};
            bool bReport_computedLinRegrParam[2] {false, false};

            for (int ieLRSA = e_LRsA_left;
                 ieLRSA <= e_LRsA_right;
                 ieLRSA++) {

                qDebug() << " ieLRSA =" << ieLRSA;

                //qDebug() << __FUNCTION__ << __LINE__;

                //use a ptr variable for easiest source code reading
                S_LinearRegressionParameters *ptrLinRegrParameters = nullptr;

                if (bComputeMainMeasure) {
                    ptrLinRegrParameters = &(boxAndStackedProfilWithMeasurements.
                                             _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                             _LRSide_linRegrModel_mainMeasure.
                                             _linearRegressionParam_Left_Right[ieLRSA]);
                } else { //@#LP secondary measures not used in the current implementation
                    ptrLinRegrParameters = &(boxAndStackedProfilWithMeasurements.
                                             _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                             _LRSide_linRegrModel_secondaryMeasures[secondaryMeasureIdxToCompute].
                                             _linearRegressionParam_Left_Right[ieLRSA]);
                }

                qDebug() << __FUNCTION__ << __LINE__ << " ptrLinRegrParameters->showContent(): ";
                ptrLinRegrParameters->showContent();

                bReport_computedLinRegrParam[ieLRSA] = computeLinearRegressionParametersOfAnonymMajor(

                    boxAndStackedProfilWithMeasurements.
                        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                        _qvectqvect_aboutMeanMedian[meanMedianIndexAccess],

                    boxAndStackedProfilWithMeasurements._locationFromZeroOfFirstInsertedValueInStackedProfil,

                    boxAndStackedProfilWithMeasurements.
                            _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                            _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation,

                    (*ptrLinRegrParameters),

                    computedYForX0[ieLRSA]);

                if (!bReport_computedLinRegrParam[ieLRSA]) {

                    qDebug() << __FUNCTION__ << __LINE__ << "if (!bReport_computedLinRegrParam[ " << ieLRSA << "]";

                } else {

                    stdErrorOfIntercept[ieLRSA] = (*ptrLinRegrParameters)._mathComp_LinearRegressionParameters._modelParametersErrors._stdErrorOfIntercept;

                    qDebug() << __FUNCTION__ << "stdErrorOfIntercept[ " << ieLRSA << " ] = " <<  stdErrorOfIntercept[ieLRSA];

                    //-------
                    qDebug() << "computedYForX0[ " << ieLRSA << "] = " << computedYForX0[ieLRSA];
                    qDebug() << "linRegrParameters_leftRightSide parameters: a _ b : "
                             << (*ptrLinRegrParameters)._mathComp_LinearRegressionParameters._GSLCompLinRegParameters._c1_a_slope
                             << " _"
                             << (*ptrLinRegrParameters)._mathComp_LinearRegressionParameters._GSLCompLinRegParameters._c0_b_intercept;

                    qDebug() << " ( approx ): " << "(linRegrParameters_leftRightSide._computed_a * "
                                << boxAndStackedProfilWithMeasurements.
                                _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                                _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0
                                << " + "
                                << (*ptrLinRegrParameters)._mathComp_LinearRegressionParameters._GSLCompLinRegParameters._c0_b_intercept;

                    qDebug() << ": xmin: " << (*ptrLinRegrParameters)._xRangeForLinearRegressionComputation._xMin;
                    qDebug() << ": xmax: " << (*ptrLinRegrParameters)._xRangeForLinearRegressionComputation._xMax;
                    //------

                }
            }

            if (   (!bReport_computedLinRegrParam[e_LRsA_left] )
                 ||(!bReport_computedLinRegrParam[e_LRsA_right])) {

                qDebug() << __FUNCTION__ << " error: some computedLinRegrParam failed";
                qDebug() << __FUNCTION__ << " => can not compute Linear regression model results";

                //clear, @LP previous run is cleaned here
                boxAndStackedProfilWithMeasurements.
                        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                        _LRSide_linRegrModel_mainMeasure._modelValuesResults.clear();

            } else {

                //
                //
                //compute model results value (yoffset at 0, and sigma sum)
                //
                //
                //delta between 'y from the line at right side for x0 (typically 0)' and 'y from the line at left side (for x0 also)
                qreal yOffsetAtX0 = computedYForX0[e_LRsA_right] - computedYForX0[e_LRsA_left];

                //sum of stdErrorOfIntercept
                qreal sigmabSum = stdErrorOfIntercept[e_LRsA_right] + stdErrorOfIntercept[e_LRsA_left];

                boxAndStackedProfilWithMeasurements.
                        _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                        _LRSide_linRegrModel_mainMeasure._modelValuesResults.set(yOffsetAtX0, sigmabSum);
            }
        }
    }
    return(true);
}

bool ComputationCore::computeStakedParallele_withSameCorrelScoreForPerpAndParall(

        const vector<double> vect_Perp_Parall_DeltaZ[e_CA_ComponentsCount],
        const vector<double> vectCorrelationScore_PX1_PX2_DeltaZ[eLA_LayersCount],
        U_MeanMedian_computedValuesWithValidityFlag computedValues[e_CA_ComponentsCount]) {

    int idxVectCorrelScoreParamters_togetherOrNot = eLA_CorrelScoreForPX2Alone;  //set on eLA_CorrelScoreForPX2Alone as invalid and empty vector (eLA_CorrelScoreForPX1PX2Together is 0)

    for (uint iComponentToCompute = e_CA_Perp; iComponentToCompute <= e_CA_deltaZ; iComponentToCompute++) {

        if (iComponentToCompute <= e_CA_Parall) {
            idxVectCorrelScoreParamters_togetherOrNot = eLA_CorrelScoreForPX1PX2Together;
        } else { //deltaZ
            idxVectCorrelScoreParamters_togetherOrNot = eLA_CorrelScoreForDeltaZAlone;
        }

        if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[iComponentToCompute]) {

            qDebug() << __FUNCTION__<< "now compute method on iComponentToCompute :" << iComponentToCompute;

            double weightingToUse = 1.0;
            if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[idxVectCorrelScoreParamters_togetherOrNot]._weighting._bUse) {
                weightingToUse = static_cast<double>(_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[idxVectCorrelScoreParamters_togetherOrNot]._weighting._f_weightExponent);
            }

            switch (_vect_eComputationMethod_Perp_Parall_DeltaZ[iComponentToCompute]) {
                case e_CM_mean:
                    qDebug() << __FUNCTION__<< "now call noWeight_mean_and_absdevAroundMean";
                    noWeight_mean_and_absdevAroundMean(vect_Perp_Parall_DeltaZ[iComponentToCompute],
                                                       computedValues[iComponentToCompute]._aboutMean._mean,
                                                       computedValues[iComponentToCompute]._aboutMean._absdevAroundMean,
                                                       computedValues[iComponentToCompute]._aboutMean._bValuesValid);
                    break;

                case e_CM_median:
                    qDebug() << __FUNCTION__<< "now call noWeight_median_and_absDevAroundMedian";
                    noWeight_median_and_absDevAroundMedian(vect_Perp_Parall_DeltaZ[iComponentToCompute],
                                                           computedValues[iComponentToCompute]._aboutMedian._median,
                                                           computedValues[iComponentToCompute]._aboutMedian._absDevAroundMedian,
                                                           computedValues[iComponentToCompute]._aboutMedian._bValuesValid);
                    break;

                case e_CM_weightedMean:
                    qDebug() << __FUNCTION__<< "now call withWeight_wMean_wAbsdevAroundWMean";
                    withWeight_wMean_wAbsdevAroundWMean(vect_Perp_Parall_DeltaZ[iComponentToCompute],
                                                       vectCorrelationScore_PX1_PX2_DeltaZ[idxVectCorrelScoreParamters_togetherOrNot],
                                                       weightingToUse,//powerWeight
                                                       computedValues[iComponentToCompute]._aboutWMean._wMean,
                                                       computedValues[iComponentToCompute]._aboutWMean._wAbsdevAroundWMean,
                                                       computedValues[iComponentToCompute]._aboutWMean._bValuesValid);
                    break;

                case e_CM_weightedMedian:
                    qDebug() << __FUNCTION__<< "now call withWeight_wMedian_and_absDevAroundWMedian";
                    withWeight_wMedian_and_absDevAroundWMedian(vect_Perp_Parall_DeltaZ[iComponentToCompute],
                                                               vectCorrelationScore_PX1_PX2_DeltaZ[idxVectCorrelScoreParamters_togetherOrNot],
                                                               weightingToUse,//powerWeight
                                                               computedValues[iComponentToCompute]._aboutWMedian._wMedian,
                                                               computedValues[iComponentToCompute]._aboutWMedian._absDevAroundWMedian,
                                                               computedValues[iComponentToCompute]._aboutWMedian._bValuesValid);
                    break;

                default:
                    qDebug() << __FUNCTION__<< " error: this case should never occurs";
                    return(false);
            }

            qDebug() << __FUNCTION__ << "computedValues[iComponentToCompute]..._anonymMMVF._bValuesValid = "
                     << computedValues[iComponentToCompute]._anonymMajorMinorWithValidityFlag._bValuesValid;

            qDebug() << __FUNCTION__ << "computedValues[iComponentToCompute]...._anonymMMVF._major_centralValue = "
                     << computedValues[iComponentToCompute]._anonymMajorMinorWithValidityFlag._major_centralValue;

            qDebug() << __FUNCTION__ << "computedValues[iComponentToCompute]..._anonymMMVF._minor_aroundCentralValue = "
                    << computedValues[iComponentToCompute]._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;

        }
    }
    return(true);
}


//fix double float point type usage

//this method consider that the point exist inside the image
//this method consider that _vectPtr_valueDequantization_PX1_PX2_deltaZ was fully checked valid
bool ComputationCore::pickPixels_correlationScore_PX1PX2Together(
            Vec2<double> vec2dbPoint,
            Vec2<double> vec2dbNormalizedBoxUnitVectorDirection,        

            double& correlationScore,// correlation score values for one parallele (same for PX1 and PX2 when non independant)
            double Perp_Parall[2],
            e_filterMask filterMask_Perp_Parall[2],
            bool bDebugGettingContent) {

    //Start getting the correlationScore and filter it to avoid to get pixels from PX1, PX2 and deltaZ if correlation scores said 'useless'
    if (_correlationScoreMapFileAvailabilityFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
        if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._bUse) {

            float f_correlationScore_pixelvalue_ForPX1PX2 = .0;

            //qDebug() << __FUNCTION__<< "get correlationScore_pixelvalue_ForPX1PX2";

            bool bReport_correlationScoreGot = false;

            if (_computationParameters._ePixelExtractionMethod == e_PEM_NearestPixel) {
                //float correlationScore_onePixelvalue = .0;
                bReport_correlationScoreGot =  _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]->
                         getNearestPixelValue_fromContinousPixelXY             (vec2dbPoint, f_correlationScore_pixelvalue_ForPX1PX2);
                qDebug() << __FUNCTION__ << "CorrelScoreForPX1PX2Together getNearestPixelValue(...) :"  << f_correlationScore_pixelvalue_ForPX1PX2;

            } else { //e_PEM_BilinearInterpolation (e_PEM_notSet filtered at start of this method)
                bReport_correlationScoreGot =  _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]->
                         getBilinearInterpolatedPixelValue_fromContinousPixelXY(vec2dbPoint, f_correlationScore_pixelvalue_ForPX1PX2);
                qDebug() << __FUNCTION__ << "CorrelScoreForPX1PX2Together getBilinearInterpolatedPixelValue(...) :"  << f_correlationScore_pixelvalue_ForPX1PX2;
            }
            if (!bReport_correlationScoreGot) {
                qDebug() << __FUNCTION__<< "error: (#1) correlation score: failed to getPixel for pixel:"
                         << vec2dbPoint.x << ", "
                         << vec2dbPoint.y;
                return(false);
            }

            bool bValuesToFilterDueToLowCorrelScore = false;
            //
            //In any case, filter value if the correlation score is too low.
            //
            /*We have to avoid to have value with according zero correlation score in final vector.
              Because the median search algorihtm, used later, search the median in an ordered values vector.
              Lettings values in the vector with according correlation score at zero can generate use case like this example, due to equal values:
               Values  | According weight (after normalization)
               6.65882 | w= 0.166667
               6.65882 | w= 0.166667
               6.70588 | w= 0.166667
               6.75294 | w= 0
               6.75294 | w= 0.166667
               6.75294 | w= 0.166667
               6.75294 | w= 0
               6.75294 | w= 0.166667
               //
               //finding the lower median is ok in this example, the lower media index will be 2.
               //but finding the higher median produce an index value which is 4, which is not (index of lower median + 1)
               //Which is incorrect. The median search algorithm considers this case as an error.
               //This is why filtering of value if the correlation score is too low is important in any case.
            */
            if (f_correlationScore_pixelvalue_ForPX1PX2 < 0.0001f) {  //the lowest accepted value for standard filter by user set value is 0.001. Hence we set a value to filter 'in any case' which is lower
                qDebug() << __FUNCTION__<< "if (f_correlationScore_pixelvalue_ForPX1PX2 < 0.0001f) {";
                bValuesToFilterDueToLowCorrelScore = true;
            }

            if (!bValuesToFilterDueToLowCorrelScore) {

                if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]._thresholdRejection._bUse) {

                    /*qDebug() << "compare PX1PX2 f_correlationScore_pixelvalue_ForPX1PX2 with " << _computationParameters.
                                _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].
                                _thresholdRejection._f_rejectIfBelowValue;*/

                    if (f_correlationScore_pixelvalue_ForPX1PX2 < _computationParameters.
                            _correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].
                            _thresholdRejection._f_rejectIfBelowValue) {

                        bValuesToFilterDueToLowCorrelScore = true;                        
                        qDebug() << __FUNCTION__<< "f_correlationScore_pixelvalue_ForPX1PX2 is < _thresholdRejection._f_rejectIfBelowValue ";
                    }
                }
            }

            if (bValuesToFilterDueToLowCorrelScore) {                
                qDebug() << __FUNCTION__<< "if (bValuesToFilterDueToLowCorrelScore) {";
                correlationScore = std::numeric_limits<double>::quiet_NaN();
                for (uint iPOP = e_CA_Perp; iPOP <= e_CA_Parall; iPOP++) {
                    filterMask_Perp_Parall[iPOP] = e_fM_correlationScoreToLow;
                    Perp_Parall           [iPOP] = std::numeric_limits<double>::quiet_NaN();
                }
                return(true);
            }

            correlationScore = static_cast<double>(f_correlationScore_pixelvalue_ForPX1PX2);
        }
    }

    //qDebug() << "compare PX1PX2 correlationScore_pixelvalue_ForPX1PX2]0,1[ with " << _computationParameters.

    //get PX1, PX2
    //qDebug() << __FUNCTION__ << "get PX1, PX2 from ContinousPixelXY Coord: " << vec2dbPoint.x << ", " << vec2dbPoint.y;

    float f_pixelValue_PX1_PX2[2] = {.0, .0};

    for (ulong i_eaSP = eLA_PX1; i_eaSP <= eLA_PX2; i_eaSP++) {

        bool bReport = false;
        if (_computationParameters._ePixelExtractionMethod == e_PEM_NearestPixel) {
            bReport = _vectUptrPixelPickers_PX1_PX2_DeltaZ[static_cast<unsigned int>(i_eaSP)]->getNearestPixelValue_fromContinousPixelXY(
                        vec2dbPoint, f_pixelValue_PX1_PX2[i_eaSP]);
            qDebug() << __FUNCTION__ << "f_pixelValue getNearestPixelValue(...) => f_pixelValue_PX1_PX2[ " << i_eaSP << " ] = " << f_pixelValue_PX1_PX2[i_eaSP];
        } else { //e_PEM_BilinearInterpolation

            bReport = _vectUptrPixelPickers_PX1_PX2_DeltaZ[static_cast<unsigned int>(i_eaSP)]->getBilinearInterpolatedPixelValue_fromContinousPixelXY(
                        vec2dbPoint, f_pixelValue_PX1_PX2[i_eaSP]);
            qDebug() << __FUNCTION__ << "f_pixelValue getBilinearInterpolatedPixelValue(...) => f_pixelValue_PX1_PX2[ " << i_eaSP << " ] = " << f_pixelValue_PX1_PX2[i_eaSP];
        }
        if (!bReport) {
            qDebug() << __FUNCTION__<< "error: failed to getPixel for pixel:"
                     << vec2dbPoint.x << ", "
                     << vec2dbPoint.y
                     << "about 'layer' #" << i_eaSP;
            return(false);
        }        
    }

    if (bDebugGettingContent) {
        qDebug() << __FUNCTION__<< "from image and before reprojection: PX1, PX2 f_:";
        //qDebug() << __FUNCTION__<< f_pixelValue_PX1_PX2[eLA_PX1] << ", " << f_pixelValue_PX1_PX2[eLA_PX2];
        //cout << "cout f_pixV PX1 = " << std::setprecision(14) << f_pixelValue_PX1_PX2[eLA_PX1] << "\n";
        //cout << "cout f_pixV PX2 = " << std::setprecision(14) << f_pixelValue_PX1_PX2[eLA_PX2] << "\n";
    }

    //qDebug() << __FUNCTION__<< "reproject now PX1, PX2:";

    f_pixelValue_PX1_PX2[eLA_PX1] = (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[eLA_PX1].dequantize(f_pixelValue_PX1_PX2[eLA_PX1]);

    f_pixelValue_PX1_PX2[eLA_PX2] = (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[eLA_PX2].dequantize(f_pixelValue_PX1_PX2[eLA_PX2]);


    double dbl_pixelValue_PX1_PX2[2] { static_cast<double>(f_pixelValue_PX1_PX2[eLA_PX1]),
                                       static_cast<double>(f_pixelValue_PX1_PX2[eLA_PX2]) };

    //cout << "cout dbl_pixelValue PX1 = " << std::setprecision(14) << dbl_pixelValue_PX1_PX2[0] << "\n";
    //cout << "cout dbl_pixelValue PX2 = " << std::setprecision(14) << dbl_pixelValue_PX1_PX2[1] << "\n";

    //projection axis of (PX1Value, PX2Value) to box vector direction axis


// ///////#define DEF_useMinusPX2 // used only in a very specific test to try to be close to the ENVI plugin
// ///////@LP DEF_useMinusPX2 must never be used with micmac input files !
//#ifdef DEF_useMinusPX2
//    Vec2<double> vec2_Hor_Vert_fromImage_inImageCoordinateSystem {
//        dbl_pixelValue_PX1_PX2[eLA_PX1], //horizontal component W/E
//        -dbl_pixelValue_PX1_PX2[eLA_PX2]  //vertical component N/S
//    };
//    #warning "minus PX2 for ENVI comparison test"
//#else
    Vec2<double> vec2_Hor_Vert_fromImage_inImageCoordinateSystem {
        dbl_pixelValue_PX1_PX2[eLA_PX1], //horizontal component W/E
        dbl_pixelValue_PX1_PX2[eLA_PX2]  //vertical component N/S
    };
//#endif

    //qDebug() << __FUNCTION__<< "vec2PX1PX2_fromImage_inImageCoordinateSystem:"
    //         << vec2PX1PX2_fromImage_inImageCoordinateSystem.x << ", "
    //         << vec2PX1PX2_fromImage_inImageCoordinateSystem.y;

    Vec2<double> vect_axisDestVector_reoriented = reorientYToTop(vec2dbNormalizedBoxUnitVectorDirection);
    Vec2<double> vec2_Hor_Vert_reoriented = reorientYToTop(vec2_Hor_Vert_fromImage_inImageCoordinateSystem);

    if (bDebugGettingContent) {
        /*cout << __FUNCTION__ << "vec2dbNormalizedBoxUnitVectorDirection:" << std::setprecision(14)
             << vec2dbNormalizedBoxUnitVectorDirection.x << ", "
             << vec2dbNormalizedBoxUnitVectorDirection.y << "\n";

        cout << __FUNCTION__ << "vect_axisDestVector_reoriented:" << std::setprecision(14)
             << vect_axisDestVector_reoriented.x << ", "
             << vect_axisDestVector_reoriented.y << "\n";*/

      //qDebug() << __FUNCTION__<< "vec2dbNormalizedBoxUnitVectorDirection:" << vec2dbNormalizedBoxUnitVectorDirection.x << ", " << vec2dbNormalizedBoxUnitVectorDirection.y;
      //qDebug() << __FUNCTION__<< "vect_axisDestVector_reoriented:" << vect_axisDestVector_reoriented.x << ", " << vect_axisDestVector_reoriented.y;
    }

    //Note: dstAxisUnitVector has to be normalized
    Vec2<double> vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis {.0,.0};
    vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis =
            transformPointFromCoordinateSystemToAnother_rotationOfAxes(
                vec2_Hor_Vert_reoriented,
                vect_axisDestVector_reoriented);

    if (bDebugGettingContent) {
        /*cout << __FUNCTION__ << "vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis:" << std::setprecision(14)
             << vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis.x << ", "
             << vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis.y << "\n";
        */
        qDebug() << __FUNCTION__<< "vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis :"
                 << vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis.x << ", "
                 << vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis.y;
    }

    Vec2<double> transformed_reorientYToDown =
            reorientYToDown(vec2_Hor_Vert_projectedToBoxUnitVectorDirectionAxis);

    if (bDebugGettingContent) {
        qDebug() << __FUNCTION__<< "(final) transformed_reorientYToDown :"
                 << transformed_reorientYToDown.x << ", "
                 << transformed_reorientYToDown.y;

        /*cout << __FUNCTION__ << "(final) transformed_reorientYToDown : :"
            << transformed_reorientYToDown.x << ", "
            << transformed_reorientYToDown.y << "\n";*/
    }

    Perp_Parall[e_CA_Perp] =  transformed_reorientYToDown.y;

    filterMask_Perp_Parall[e_CA_Perp] = e_fM_keepForComputation;

    Perp_Parall[e_CA_Parall] = transformed_reorientYToDown.x;

    filterMask_Perp_Parall[e_CA_Parall] = e_fM_keepForComputation;

#ifdef debug_check_sizevectPX1PX2stuffs  //@LP dev check: check that all vectors have the same size
    //#@LP variables names need to be updated if you want to use it

    int qvectSizePX1    = qvect_PX1_PX2_DeltaZ[eLA_PX1].size();
    int qvectSizePX2    = qvect_PX1_PX2_DeltaZ[eLA_PX2].size();
    int qvectSizeFMaskPX1 = qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX1].size();
    int qvectSizeFMaskPX2 = qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX2].size();
    int qvectSizeCorrelationScore = qvect_correlationScore.size();

    QVector<int> qvectSizeAllVectors;
    qvectSizeAllVectors.push_back(qvectsizePX1);
    qvectSizeAllVectors.push_back(qvectsizePX2);
    qvectSizeAllVectors.push_back(qvectsizeFMaskPX1);
    qvectSizeAllVectors.push_back(qvectsizeFMaskPX2);
    qvectSizeAllVectors.push_back(qvectsizeCorrelationScore);

    bool bSomeSizeMismatch = false;
    int sizeAt0 = qvectSizeAllVectors[0];
    int idx = 1;
    for(auto iterSz = qvectSizeAllVectors.cbegin()+1;
             iterSz != qvectSizeAllVectors.cend();
           ++iterSz) {
      if (*iterSz != sizeAt0) {
          bSomeSizeMismatch = true;
          qDebug() << __FUNCTION__<< "error: vectors sizes differs: [" << idx << "] =" << *iterSz << "!=" << sizeAt0 << "( [0] )";
      }
      idx++;
    }
    qDebug() << __FUNCTION__ << "at the end:";
    qDebug() << __FUNCTION__ << "correlationScore;filterMaskPx1;filterMaskPx2;PX1;PX2;";
    for (idx = 0; idx < sizeAt0; idx++) {
        qDebug() << __FUNCTION__ <<
                   qvect_correlationScore[idx]                       << ";" <<
                   qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX1].at(idx)  << ";" <<
                   qvect_filterMask_PX1_PX2_DeltaZ[eLA_PX2] .at(idx) << ";" <<
                   qvect_PX1_PX2_DeltaZ[eLA_PX1].at(idx)             << ";" <<
                   qvect_PX1_PX2_DeltaZ[eLA_PX2].at(idx)             << ";";
    }
#endif
    return(true);
}


//this method consider that the point exist inside the image
//@LP mixed method in transition: deltaZ / ela_consideredlayer denomination but use deltaZ correlationScore map in any case
//this method consider that _vectPtr_valueDequantization_PX1_PX2_deltaZ was fully checked valid
bool ComputationCore::pickPixels_correlationScore_independantLayerDeltaZ(
        Vec2<double> vec2dbPoint,
        double& correlationScore,// correlation score values for one parallele (PX1 and PX2 will have the same when non independant)
        double& pixelValue,//values for one parallele by layers (PX1, PX2, DeltaZ)
        e_filterMask& filterMask) { //filter mask for one parallele by layers (PX1, PX2, DeltaZ)

    e_LayersAcces ela_consideredlayer = eLA_deltaZ;

    //Start getting the correlationScore and filter it to avoid to get pixels from PX1, PX2 and deltaZ if correlation scores said 'useless'
    if (_correlationScoreMapFileAvailabilityFlags._sCSF_AllIndependant_PX1_PX2_DeltaZ._b_PX1_PX2_DeltaZ[ela_consideredlayer]) { //
        if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[ela_consideredlayer]._bUse) {

            float f_correlationScore_pixelvalue = .0;

            //qDebug() << __FUNCTION__<< "get correlationScore_pixelvalue deltaZ";

            bool bReport_correlationScoreGot = false;
            //vector<float> vectCorrelationScoreBlock; //1 or 4 pixels depending of the get pixel method
            if (_computationParameters._ePixelExtractionMethod == e_PEM_NearestPixel) {

                bReport_correlationScoreGot =  _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[ela_consideredlayer]->
                         getNearestPixelValue_fromContinousPixelXY             (vec2dbPoint, f_correlationScore_pixelvalue);
                qDebug() << __FUNCTION__ << "correlationScore getNearestPixelValue(...) => layer: " << ela_consideredlayer << " pixelValue:" << f_correlationScore_pixelvalue;

            } else { //e_PEM_BilinearInterpolation (e_PEM_notSet filtered at start of this method)
                bReport_correlationScoreGot =  _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ[ela_consideredlayer]->
                         getBilinearInterpolatedPixelValue_fromContinousPixelXY(vec2dbPoint, f_correlationScore_pixelvalue);
                qDebug() << __FUNCTION__ << "correlationScore getBilinearInterpolatedPixelValue(...) => layer: " << ela_consideredlayer << " pixelValue:" << f_correlationScore_pixelvalue;
            }
            if (!bReport_correlationScoreGot) {
                qDebug() << __FUNCTION__<< "error: (#1) correlation score: failed to getPixel for pixel:"
                         << vec2dbPoint.x << ", "
                         << vec2dbPoint.y;
                return(false);
            }

            bool bValuesToFilterDueToLowCorrelScore = false;
            //
            //In any case, filter value if the correlation score is too low.
            //
            /*We have to avoid to have value with according zero correlation score in final vector.
            Because the median search algorihtm, used later, search the median in an ordered values vector.
            Lettings values in the vector with according correlation score at zero can generate use case like this example, due to equal values:
            Values  | According weight (after normalization)
            6.65882 | w= 0.166667
            6.65882 | w= 0.166667
            6.70588 | w= 0.166667
            6.75294 | w= 0
            6.75294 | w= 0.166667
            6.75294 | w= 0.166667
            6.75294 | w= 0
            6.75294 | w= 0.166667
            //
            //finding the lower median is ok in this example, the lower media index will be 2.
            //but finding the higher median produce an index value which is 4, which is not (index of lower median + 1)
            //Which is incorrect. The median search algorithm considers this case as an error.
            //This is why filtering of value if the correlation score is too low is important in any case.
            */
            if (f_correlationScore_pixelvalue < 0.0001f) {  //the lowest accepted value for standard filter by user set value is 0.001. Hence we set a value to filter 'in any case' which is lower
                qDebug() << __FUNCTION__<< "if (f_correlationScore_pixelvalue < 0.0001f) {";
                bValuesToFilterDueToLowCorrelScore = true;
            }

            if (!bValuesToFilterDueToLowCorrelScore) {
                if (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[ela_consideredlayer]._thresholdRejection._bUse) {

                    if (f_correlationScore_pixelvalue
                        < _computationParameters.
                            _correlationScoreMapParameters_PX1_PX2_DeltaZ[ela_consideredlayer].
                            _thresholdRejection._f_rejectIfBelowValue) {

                        qDebug() << __FUNCTION__<< "f_correlationScore_pixelvalue is < _thresholdRejection._f_rejectIfBelowValue";
                        bValuesToFilterDueToLowCorrelScore = true;
                    }
                }
            }

            if (bValuesToFilterDueToLowCorrelScore) {
                qDebug() << __FUNCTION__<< "if (bValuesToFilterDueToLowCorrelScore) {";
                filterMask = e_fM_correlationScoreToLow;
                correlationScore = std::numeric_limits<double>::quiet_NaN();
                pixelValue = std::numeric_limits<double>::quiet_NaN();
                return(true);
            }

            correlationScore = static_cast<double>(f_correlationScore_pixelvalue);
        }
    }

    //get pixel value
    //qDebug() << __FUNCTION__<< "get deltaZ";

    float f_pixelValue { .0 };

    bool bReport = false;
    if (_computationParameters._ePixelExtractionMethod == e_PEM_NearestPixel) {
        bReport = _vectUptrPixelPickers_PX1_PX2_DeltaZ[static_cast<unsigned int>(ela_consideredlayer)]->getNearestPixelValue_fromContinousPixelXY(
                    vec2dbPoint, f_pixelValue);
        qDebug() << __FUNCTION__ << "f_pixelValue getNearestPixelValue(...) => layer: " << ela_consideredlayer << " pixelValue:" << f_pixelValue;

    } else { //e_PEM_BilinearInterpolation

        bReport = _vectUptrPixelPickers_PX1_PX2_DeltaZ[static_cast<unsigned int>(ela_consideredlayer)]->getBilinearInterpolatedPixelValue_fromContinousPixelXY(
                    vec2dbPoint, f_pixelValue);
        qDebug() << __FUNCTION__ << "f_pixelValue getBilinearInterpolatedPixelValue(...) => layer: " << ela_consideredlayer << " pixelValue:" << f_pixelValue;
    }
    if (!bReport) {
        qDebug() << __FUNCTION__<< "error: failed to getPixel for pixel:"
                 << vec2dbPoint.x << ", "
                 << vec2dbPoint.y
                 << "about 'layer' #" << ela_consideredlayer;
        return(false);
    }

    f_pixelValue = (*_vectPtr_valueDequantization_PX1_PX2_deltaZ)[ela_consideredlayer].dequantize(f_pixelValue);

    pixelValue = static_cast<double>(f_pixelValue);
    filterMask = e_fM_keepForComputation;

    return(true);
}

bool ComputationCore::computeLinearRegressionParametersOfAnonymMajor(
        const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_AnonymMajorMinorWithValidityFlag,
        S_LocationFromZeroOfFirstInsertedValueInStackedProfil locationFromZeroOfFirstInsertedValueInStackedProfil,
        S_X0ForYOffsetComputation x0ForYOffsetComputation,
        S_LinearRegressionParameters &linearRegressionParameters,
        double& computedYForGivenXLocation) {

    vector<double> vectX;
    vector<double> vectY;



    /*linearRegressionParameters.
            _mathComp_LinearRegressionParameters.
            _GSLCompLinRegParameters._bComputed = false;
    */
    //better cleanup:
    linearRegressionParameters._mathComp_LinearRegressionParameters.clear();



    qDebug() << __FUNCTION__ << "check content state linearRegressionParameters._mathComp_LinearRegressionParameters.showContent() #enter now";
    linearRegressionParameters._mathComp_LinearRegressionParameters.showContent();
    qDebug() << __FUNCTION__ << "check content state linearRegressionParameters._mathComp_LinearRegressionParameters.showContent() #exited now";


    if (!linearRegressionParameters._xRangeForLinearRegressionComputation._bValid) {
        qDebug() << __FUNCTION__ << "error: _xRangeForLinearRegressionComputation._bValid is false";
        return(false);
    }

    if (!locationFromZeroOfFirstInsertedValueInStackedProfil._bValid) {
        qDebug() << __FUNCTION__ << "error: locationFromZeroOfFirstInsertedValueInStackedProfil._bValid is false";
        return(false);
    }

    int idxFirstValue = static_cast<int>(linearRegressionParameters._xRangeForLinearRegressionComputation._xMin - locationFromZeroOfFirstInsertedValueInStackedProfil._value);
    int idxLastValue  = static_cast<int>(linearRegressionParameters._xRangeForLinearRegressionComputation._xMax - locationFromZeroOfFirstInsertedValueInStackedProfil._value);

    qDebug() << __FUNCTION__ << "idxFirstValue = " << idxFirstValue;
    qDebug() << __FUNCTION__ << "idxLastValue = " << idxLastValue;

    //-----
    //@LP shield for code developpement
    //    these errors cases shouls never happend in correct code usage
    if (idxFirstValue >= qvect_AnonymMajorMinorWithValidityFlag.size()) {
        qDebug() << __FUNCTION__ << "index idxFirstValue mismatch: dev: probable missing previous computation run";
        return(false);
    }
    if (idxLastValue >= qvect_AnonymMajorMinorWithValidityFlag.size()) {
        qDebug() << __FUNCTION__ << "index idxLastValue mismatch: dev: probable missing previous computation run";
        return(false);
    }
    //-----

    double increasingX_fromFirstValueToUse = locationFromZeroOfFirstInsertedValueInStackedProfil._value + idxFirstValue;

    //qDebug() << __FUNCTION__ << "increasingX_fromFirstValueToUse = " << increasingX_fromFirstValueToUse;

    for (int idx = idxFirstValue; idx <= idxLastValue; idx++) {
        /*qDebug() << __FUNCTION__
            << "  major: "        << qvect_AnonymMajorMinorWithValidityFlag.at(idx)._anonymMajorMinorWithValidityFlag._major_centralValue
            << "  minor: "        << qvect_AnonymMajorMinorWithValidityFlag.at(idx)._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue
            << "  bValuesValid: " << qvect_AnonymMajorMinorWithValidityFlag.at(idx)._anonymMajorMinorWithValidityFlag._bValuesValid;*/

        if (qvect_AnonymMajorMinorWithValidityFlag.at(idx)._anonymMajorMinorWithValidityFlag._bValuesValid) {

            vectX.push_back(increasingX_fromFirstValueToUse);
            vectY.push_back(qvect_AnonymMajorMinorWithValidityFlag.at(idx)._anonymMajorMinorWithValidityFlag._major_centralValue);
        }
        increasingX_fromFirstValueToUse += 1.0;
    }

    //debug
    /*if (vectX.size() != vectY.size()) {
        qDebug() << "error: vectX_forLinearRegression and vectY_forLinearRegression sizes are different";
        return(false);
    }*/

    /*for (unsigned long int ix= 0; ix < vectX.size(); ix++) {
        qDebug() << "  (x,y) for linearRegression: " << vectX.at(ix) << ", " << vectY.at(ix);
    }*/

    S_MathComp_LinearRegressionParameters mathComp_LinearRegressionParameters;
    bool bReport_linRegComputed = computeLinearRegressionParametersFor(vectX, vectY, mathComp_LinearRegressionParameters);
    if (!bReport_linRegComputed) {
        qDebug() << " computeLinearRegressionParameters failed"
                 << "from " << linearRegressionParameters._xRangeForLinearRegressionComputation._xMin
                 << "to "   << linearRegressionParameters._xRangeForLinearRegressionComputation._xMax;
    } else {

        //set the result:
        linearRegressionParameters._mathComp_LinearRegressionParameters = mathComp_LinearRegressionParameters;

        if (!x0ForYOffsetComputation._bValid) {
            qDebug() << __FUNCTION__
                     << " computeYForGivenX_usingComputedLinearRegressionParameters warning: _x0ForYOffsetComputation is not set as valid";
        } else {
            bool bReport_computedYFor0 =
            computeYForGivenX_usingComputedLinearRegressionParameters_GSLMethod(
                        mathComp_LinearRegressionParameters._GSLCompLinRegParameters,
                        x0ForYOffsetComputation._x0,
                        computedYForGivenXLocation);
            if (!bReport_computedYFor0) {
                qDebug() << __FUNCTION__
                         << " computeYForGivenX_usingComputedLinearRegressionParameters failed (for givenX: "
                         << x0ForYOffsetComputation._x0;
                return(false);
            }
        }
    }
    return(bReport_linRegComputed);
}

bool ComputationCore::setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel() {
    return(setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(_qvectb_componentsToCompute_Perp_Parall_DeltaZ));
}

//@LP always return true
bool ComputationCore::setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(
     const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ) {

    qDebug() << __FUNCTION__<< "ComputationCore" << __LINE__;

    if (_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.isEmpty()) {
        qDebug() << __FUNCTION__ << "warning: _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        return(true);
    }

    for (QMap<int, S_BoxAndStackedProfilWithMeasurements>::iterator iterBASPWM = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
         iterBASPWM != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
         ++iterBASPWM) {

        double min = (*iterBASPWM)._locationFromZeroOfFirstInsertedValueInStackedProfil._value;
        double max = -min;
        double center = .0;

        qDebug() << __FUNCTION__ << "min :" << min;
        qDebug() << __FUNCTION__ << "max :" << max;
        qDebug() << __FUNCTION__ << "center :" << center;

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {

            if (qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ[ieCA]) {

                qDebug() << __FUNCTION__ << "min, center ="  << min << ", " << center;
                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure.
                    _linearRegressionParam_Left_Right[e_LRsA_left].
                    set_xRange(min, center);


                qDebug() << __FUNCTION__ << "center, max ="  << center << ", " << max;
                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure.
                    _linearRegressionParam_Left_Right[e_LRsA_right].
                    set_xRange(center, max);


                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                        _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation.set(center);
           }
        }
    }

    qDebug() << __FUNCTION__ << "after:";

    for (QMap<int, S_BoxAndStackedProfilWithMeasurements>::iterator iterBASPWM = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
         iterBASPWM != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
             ++iterBASPWM) {

        for (int ieCA = e_CA_Perp; ieCA <= e_CA_deltaZ; ieCA++) {
            if (qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ[ieCA]) {

                /*qDebug() << __FUNCTION__ << " _LRSide_linRegrModel_mainMeasure._bValid : " << (*iterBASPWM).
                            _qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements[i_eaSP].
                            _LRSide_linRegrModel_mainMeasure._bValid;*/

                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure.
                    _linearRegressionParam_Left_Right[e_LRsA_left].
                    showContent();

                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure.
                    _linearRegressionParam_Left_Right[e_LRsA_right].
                    showContent();

               qDebug() << __FUNCTION__ << " _x0ForYOffsetComputation._bValid = " <<
               (*iterBASPWM).
                   _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                       _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._bValid;

                qDebug() << __FUNCTION__ << " _x0ForYOffsetComputation._x0 = " <<
                (*iterBASPWM).
                    _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[ieCA].
                    _LRSide_linRegrModel_mainMeasure._x0ForYOffsetComputation._x0;

           }
        }
    }

    return(true);
}


bool ComputationCore::setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(
        int vectIdxOfbox, const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ) {

    qDebug() << __FUNCTION__ << "qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ = " << qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ;
    qDebug() << __FUNCTION__ << "qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size() = " << qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size();

    if (!vectBoxIdValid(vectIdxOfbox)) {
        qDebug() << __FUNCTION__ << "error: vectBoxIdValid(vectIdxOfbox)) said false";
        return(false);
    }

    if (qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.size() != 3) {
        qDebug() << __FUNCTION__ << "error: qvectb_layersToSetToDefaultValues_PX1_PX2_deltaZ size != 3";
        return(false);
    }

    if (!qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ.count(true)) {
        qDebug() << __FUNCTION__ << "error: qvectb_layersToSetToDefaultValues_PX1_PX2_deltaZ has none true";
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements* sBoxAndStackedProfilWithMeasurementsPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectIdxOfbox]._sBASPWMPtr;

    qDebug() << __FUNCTION__<< " for vectIdxOfbox = " << vectIdxOfbox <<  " sBoxAndStackedProfilWithMeasurementsPtr is @" << (void*)sBoxAndStackedProfilWithMeasurementsPtr;

    if (!sBoxAndStackedProfilWithMeasurementsPtr) {
        qDebug() << __FUNCTION__ << "warning: sBoxAndStackedProfilWithMeasurementsPtr is nullptr";
        return(false);
    }

    bool bSet = sBoxAndStackedProfilWithMeasurementsPtr->setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel(
                    qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ);
    if (!bSet) {
        qDebug() << __FUNCTION__ << "sBoxAndStackedProfilWithMeasurementsPtr->setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel failed";
    }

    return(bSet);
}

bool ComputationCore::get_locationFromZeroOfFirstInsertedValueInStackedProfil(int boxId, double& locationFromZeroOfFirstInsertedValue) {

    if (boxId < 0) {
        //@#LP error msg
        return(false);
    }
    if (boxId >= _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()) {
        //@#LP error msg
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId]._sBASPWMPtr;

    locationFromZeroOfFirstInsertedValue = SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value;
    return(true);
}

bool ComputationCore::get_ptrStackedProfilWithMeasurements_fromBoxIdAndComponent(int boxId, e_ComponentsAccess componentAccess,
                                                                             S_StackedProfilWithMeasurements*& ptrStackedProfilWithMeasurements) {
    if (componentAccess >= e_CA_ComponentsCount) {
        qDebug() << __FUNCTION__ << "error: invalid componentAccess: " << componentAccess;
        return(false);
    }   

    if (!vectBoxIdValid(boxId)) {
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId]._sBASPWMPtr;

    ptrStackedProfilWithMeasurements = &(SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[componentAccess]); //&(iterFound->_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements[layersAccess]);
    return(true);
}


bool ComputationCore::getMapBoxId_fromIndexBoxInVectOrderAlongRoute(int vectBoxId, int &mapBoxId) {

    if (!vectBoxIdValid(vectBoxId)) {
        mapBoxId = -1;
        return(false);
    }
    mapBoxId = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectBoxId)._insertionIndex;
    return(true);
}


//@LP: O(n) loop :|
bool ComputationCore::getIndexBoxInVectOrderAlongRoute_fromMapBoxId(int mapBoxId, int &vectBoxId) {

    vectBoxId = -1;
    int _sizeQVect = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    for (int idxInVect = 0; idxInVect < _sizeQVect; idxInVect++) {

        if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxInVect]._insertionIndex == mapBoxId) {
            vectBoxId = idxInVect;
            return(true);
        }
    }
    return(false);
}

int ComputationCore::getBoxCount() const {
    return(_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size());
}


QVector<bool> ComputationCore::get_qvectLayersInUse() const {
    return(_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ);
}

QVector<bool> ComputationCore::get_qvectComputedComponents() const {
    return(_qvectb_componentsToCompute_Perp_Parall_DeltaZ);
}

bool ComputationCore::computationParameters_toQJsonObject(QJsonObject& qjsonObj) const {

    //qjsonObj.insert("noteAbout_pixelExtractionMethod", "This is a string value of an integer: 1: NearestPixel; 2: 2x2 BilinearInterpolation; 0: notSet");
    //qjsonObj.insert("noteAbout_mainComputationMode", "This is a string value of an integer: 1: typical PX1,PX2 together, deltaz or other alone; 0: notSet");
    //qjsonObj.insert("noteAbout_useRouteInReversePointOrder; //changes the orientation fault as the box will be oriented in the reverse direction of the route
    //qjsonObj.insert("noteAbout_computationMethod: 0: mean; 1: median; 2: weightedMean; 3 weightedMedian; -1: notSet");

    //at usage, for typical: PX1,PX2 together, be sure to set the value for _b_PX1_PX2_DeltaZ[eLA_PX2] as the values are stored like independent layers in any selected mainComputationMode
    //at loading a check will be done to reject file with unsync boolean value between eLA_PX1 and eLA_PX2
    //the goal is to reject an edited file by user with a mistake

    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        return(false);
    }

    if (_eProfilOrientation == e_PO_notSet) {
        return(false);
    }

    //insert _correlationScoreMapParameters only for used layers
    //(no S_ComputationParameters.toQJSon(...))


    QJsonObject qjsonObj_computationParameters;

    qjsonObj_computationParameters.insert("pixelExtractionMethod", ePixelExtractionMethod_toJsonProjectString(_computationParameters._ePixelExtractionMethod));

    qjsonObj_computationParameters.insert("baseComputationMethod", eBaseComputationMethod_toJsonProjectString(_computationParameters._eBaseComputationMethod));

    qjsonObj_computationParameters.insert("profilOrientation", eProfilOrientation_toJsonProjectString(_eProfilOrientation));

    QJsonObject qjsonObj_correlationScoreMapParameters;
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1] !=
        _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX2]) {
        return(false);
    }

    QString _tqstrDescName_correlScoreMap[3] {"PX1_PX2", "PX1_PX2", "ZOther"};
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1]) {
        QJsonObject qjsonObj_correlationScoreMapParameters_byLayer;
        _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].toQJson(qjsonObj_correlationScoreMapParameters_byLayer);
            qjsonObj_correlationScoreMapParameters.insert(_tqstrDescName_correlScoreMap[eLA_CorrelScoreForPX1PX2Together], qjsonObj_correlationScoreMapParameters_byLayer);
    }
    if (_qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_deltaZ]) {
        QJsonObject qjsonObj_correlationScoreMapParameters_byLayer;
        _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].toQJson(qjsonObj_correlationScoreMapParameters_byLayer);
            qjsonObj_correlationScoreMapParameters.insert(_tqstrDescName_correlScoreMap[eLA_CorrelScoreForDeltaZAlone], qjsonObj_correlationScoreMapParameters_byLayer);
    }

    qjsonObj_computationParameters.insert("correlationScoreMapParameters", qjsonObj_correlationScoreMapParameters);

    qjsonObj = qjsonObj_computationParameters;

    return(true);
}

//---
//return false in case of any load error
//return true if:
// - mainComputationMode got and computationModeParameters not found
// - mainComputationMode got and computationModeParameters got

bool ComputationCore::computationMainMode_fromQJsonObject(const QJsonObject& qJsonObj, QString &strMsgError) {

    //get mainComputationMode
    QString str_computationMainMode_key {"computationMainMode"};
    int int_computationMainMode;
    bool bGot = getIntValueFromJson(qJsonObj, str_computationMainMode_key, int_computationMainMode);
    if (!bGot) {
        strMsgError = "fail getting" + str_computationMainMode_key;
        return(false);
    }

    qDebug() << __FUNCTION__ << "int_computationMainMode=[" << int_computationMainMode << "]";

    if (int_computationMainMode != static_cast<int>(e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        strMsgError = str_computationMainMode_key + ": got unknow value";
        return(false);
    }

    _e_mainComputationMode = e_mCM_Typical_PX1PX2Together_DeltazAlone;

    return(true);
}

bool ComputationCore::computationParameters_fromQJsonObject(const QJsonObject& qjsonObj,
                                                            const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ,
                                                            const QVector<bool>& qvectb_correlScoreMapToUseForComputationFlags_PX1PX2_PX1PX2_DeltaZ,
                                                            bool& bContainsComputationParametersSection,
                                                            bool& bComputationParametersSectionValidAndLoaded,
                                                            QString &strMsgError) {
    bContainsComputationParametersSection = false;
    bComputationParametersSectionValidAndLoaded = false;

    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        strMsgError = "internal dev error";
        return(false);
    }

    qDebug() << __FUNCTION__ << "qjsonObj=" << qjsonObj;

    qDebug() << __FUNCTION__ << "qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ=" << qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;

    //----
    //pixelExtractionMethod
    QString strKey_pixelExtractionMethod {"pixelExtractionMethod"};
    QString str_pixelExtractionMethod;
    bool bPixelExtractionMethodGot = getStringValueFromJson(qjsonObj, strKey_pixelExtractionMethod, str_pixelExtractionMethod);
    if (!bPixelExtractionMethodGot) {
        strMsgError = "fail getting" + strKey_pixelExtractionMethod;
        return(false);
    }
    _computationParameters._ePixelExtractionMethod = projectJsonString_to_ePixelExtractionMethod(str_pixelExtractionMethod);

    //baseComputationMethod
    QString strKey_baseComputationMethod {"baseComputationMethod"};
    QString str_baseComputationMethod;
    bool bBaseComputationMethodGot = getStringValueFromJson(qjsonObj, strKey_baseComputationMethod, str_baseComputationMethod);
    if (!bBaseComputationMethodGot) {
        strMsgError = "fail getting" + strKey_baseComputationMethod;
        return(false);
    }
    _computationParameters._eBaseComputationMethod = projectJsonString_to_eBaseComputationMethod(str_baseComputationMethod);

    //profil orientation
    QString strKey_profilOrientation {"profilOrientation"};
    QString str_profilOrientation;
    bool bProfilOrientationGot = getStringValueFromJson(qjsonObj, strKey_profilOrientation, str_profilOrientation);
    if (!bProfilOrientationGot) {
        strMsgError = "fail getting" + strKey_profilOrientation;
        return(false);
    }
    _eProfilOrientation = projectJsonString_to_eProfilOrientation(str_profilOrientation);

    if (  (_computationParameters._ePixelExtractionMethod == e_PEM_notSet)
        ||(_computationParameters._eBaseComputationMethod == e_BCM_notSet)
        ||(_eProfilOrientation == e_PO_notSet)) {
        strMsgError = "fail translating " + strKey_pixelExtractionMethod + " or " + strKey_baseComputationMethod +  " or " + strKey_profilOrientation;
        return(false);
    }
    //----

    //correlationScoreMapParameters
    QString strKey_correlationScoreMapParameters {"correlationScoreMapParameters"};
    QJsonObject qjsonObj_correlationScoreMapParameters;
    bool bcorrelationScoreMapParametersGot = getQJsonObjectFromJson(qjsonObj, strKey_correlationScoreMapParameters, qjsonObj_correlationScoreMapParameters);
    if (!bcorrelationScoreMapParametersGot) {
        strMsgError = "fail getting  " + strKey_correlationScoreMapParameters;
        return(false);
    }

    if (qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX1] !=
        qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[eLA_PX2]) {
        return(false);
    }

    //"correlationScoreMapParameters" must be present in json for non empty file used
    //even if the correl score file is empty
    //if the correl score file is empty, the according booleans about correlationScoreMapParameters must be set to false

    QString _tqstrDescName_correlScoreMap[3] {"PX1_PX2", "PX1_PX2", "ZOther"};
    for (int ieLAC = eLA_CorrelScoreForPX1PX2Together; ieLAC <= eLA_CorrelScoreForDeltaZAlone; ieLAC++) {

        QJsonObject qjsonObj_correlationScoreMapParameters_byGroup;
        bool bqjsonObj_correlationScoreMapParametersGot =  getQJsonObjectFromJson(qjsonObj_correlationScoreMapParameters, _tqstrDescName_correlScoreMap[ieLAC],
                                                                                  qjsonObj_correlationScoreMapParameters_byGroup);
        int idxbLayerToUseForComputation = eLA_Invalid;
        if (ieLAC == eLA_CorrelScoreForPX1PX2Together) {
            idxbLayerToUseForComputation = eLA_PX1;
        } else {
            idxbLayerToUseForComputation = ieLAC; //deltaZ case
        }

        //PX1 and PX2 together at true (or false) when computationMainMode is 'PX1PX2 together, deltaZ alone'
        if (!qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[idxbLayerToUseForComputation]) {
            if (bqjsonObj_correlationScoreMapParametersGot) {
                strMsgError = strKey_correlationScoreMapParameters + " for " + _tqstrDescName_correlScoreMap[ieLAC] + " should not exist";
                return(false); //error: should not exist
            }
        }

        if (qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[idxbLayerToUseForComputation]) {
            if (!bqjsonObj_correlationScoreMapParametersGot) {
                strMsgError = strKey_correlationScoreMapParameters + " for " + _tqstrDescName_correlScoreMap[ieLAC] + " should exist";
                return(false); //error: should exist
            }
        }

        if (qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ[idxbLayerToUseForComputation] && bqjsonObj_correlationScoreMapParametersGot) {
            bool bGot = _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[idxbLayerToUseForComputation].fromQJson(qjsonObj_correlationScoreMapParameters_byGroup);
            if (!bGot) {
                strMsgError = "fail getting " + strKey_correlationScoreMapParameters + " for " + _tqstrDescName_correlScoreMap[ieLAC];
                return(false);
            }

            if (!qvectb_correlScoreMapToUseForComputationFlags_PX1PX2_PX1PX2_DeltaZ[ieLAC]) {
                if (  (_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[ieLAC]._thresholdRejection._bUse)
                    ||(_computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[ieLAC]._weighting._bUse)) {
                    strMsgError = "correlationScoreMapParameters boolean states at true but correlation score map file is empty for " + _tqstrDescName_correlScoreMap[ieLAC];
                    return(false);
                }
            }

            if (ieLAC == eLA_CorrelScoreForPX1PX2Together) {
                _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX2] =
                    _computationParameters._correlationScoreMapParameters_PX1_PX2_DeltaZ[eLA_PX1];
            }
        }

        if (ieLAC == eLA_CorrelScoreForPX1PX2Together) {
            ieLAC++; //go directly to eLA_CorrelScoreForDeltaZAlone at the next loop
        }

    }

    bComputationParametersSectionValidAndLoaded = true;
    return(true);
}

bool ComputationCore::get_oneStackedProfilBoxParameters(int vectBoxId, S_ProfilsBoxParameters& SProfilsBoxParameters) {

    int qvectOf_BASPWMPtr_orderedAlongRoute_syncWithqlinkedlist_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!qvectOf_BASPWMPtr_orderedAlongRoute_syncWithqlinkedlist_size) {
        return(false);
    }
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }
    SProfilsBoxParameters = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId]._sBASPWMPtr->_profilsBoxParameters;
    return(true);
}

bool ComputationCore::get_oneStackedProfilBoxParametersToQJsonObjectWithoutBoxSetInfos(int vectBoxId, QJsonObject& qjsonObj) {

    int qvectOf_BASPWMPtr_orderedAlongRoute_syncWithqlinkedlist_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!qvectOf_BASPWMPtr_orderedAlongRoute_syncWithqlinkedlist_size) {
        return(false);
    }
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    const auto& constRef_SBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId];
    bool bReport_toQJsonObj_profilsBoxParameters = constRef_SBoxASPWM._sBASPWMPtr->_profilsBoxParameters.toQJsonObject(vectBoxId, qjsonObj);
    if (!bReport_toQJsonObj_profilsBoxParameters) {
        return(false);
    }

    return(true);
}

//merging the values in a array could be not the best idea as median and weighted median can be used (example PX1PX2:median ; deltaZ: weigthedMedian)
//or mean and weighted mean at the same time (example PX1PX2:mean ; deltaZ: weigthedMean)

bool ComputationCore::profilCurvesForOneStackedProfilBox_toQJsonObject(int vectBoxId, bool bSetProfilesCurvesData_asEmpty, QJsonObject& qjsonObj_profilCurves) const {

    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    const auto& constRef_SBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId];

    QVector<bool> qvectbSetProfilesCurvesData_asEmpty(3, false);

    if (bSetProfilesCurvesData_asEmpty) {
        for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
            if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[iComponent]) {
                S_MeasureIndex measureIndex(true);
                bool bWarnFlag = false;
                bool bGotReport =
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId].
                    _sBASPWMPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].get_warnFlag(measureIndex, bWarnFlag);
                if (!bGotReport) {
                    return(false);
                }
                qvectbSetProfilesCurvesData_asEmpty[iComponent] = bWarnFlag;
            }
        }
    }

    bool bReport_toQJsonObj_profilsBoxParameters =
        constRef_SBoxASPWM._sBASPWMPtr->profilCurves_toQJsonObject(vectBoxId,
                                                                   _vect_eComputationMethod_Perp_Parall_DeltaZ,
                                                                   _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
                                                                   qvectbSetProfilesCurvesData_asEmpty,
                                                                   qjsonObj_profilCurves);

    return(bReport_toQJsonObj_profilsBoxParameters);
}

bool ComputationCore::someFlaggedProfilExistForBoxId(int boxId) const {

    if (!_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.count()) {
        return(false);
    }

    if (!vectBoxIdValid(boxId)) {
        return(false);
    }

    for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
        if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[iComponent]) {
            S_MeasureIndex measureIndex(true);
            bool bWarnFlag = false;
            bool bGotReport =
                _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId].
                _sBASPWMPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].get_warnFlag(measureIndex, bWarnFlag);
            if (!bGotReport) {
                return(false);
            }
            if (bWarnFlag) {
                return(true);
            }
        }
    }
    return(false);
}

bool ComputationCore::someFlaggedProfilExist() const {

    if (!_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.count()) {
        return(false);
    }

    int vectBoxId = 0;
    auto iterSBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.cbegin();
    for (; iterSBoxASPWM != _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.cend(); ++iterSBoxASPWM) {
        for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
            if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[iComponent]) {
                S_MeasureIndex measureIndex(true);
                bool bWarnFlag = false;
                bool bGotReport =
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId].
                    _sBASPWMPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].get_warnFlag(measureIndex, bWarnFlag);
                if (!bGotReport) {
                    return(false);
                }
                if (bWarnFlag) {
                    return(true);
                }
            }
        }
        vectBoxId++;
    }
    return(false);
}

bool ComputationCore::computedComponentsToHeader_toQJsonObject(
        QJsonObject& qjsonObj_computedComponentsToHeader) const {

    int vectBoxId = 0;
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    const auto& constRef_SBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId];

    bool bReport_toQJsonObj_computedComponentsToHeader =
        constRef_SBoxASPWM._sBASPWMPtr->computedComponentsToHeader_toQJsonObject_aboutProfilCurves(
            _vect_eComputationMethod_Perp_Parall_DeltaZ,
            _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
            qjsonObj_computedComponentsToHeader);

    return(bReport_toQJsonObj_computedComponentsToHeader);
}

bool ComputationCore::stackedProfilBoxes_toQJsonArray_forProjectWrite(QJsonArray& qjsonArray_stackedProfilBoxes,
                                                      QString& strDetailsAboutFail_projectWrite) const {

    if (!_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size()) {
        strDetailsAboutFail_projectWrite = "none stacked profil boxes";
        return(false);
    }

    //in json project file boxId start at 0 and increase boxes after boxes (no restart at 0 for each new boxesSet)
    int idxBox = 0;

    int idxForJsonArrayBoxSetInsert = 0;
    auto iterSBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.cbegin();

    while (1) {

        if (iterSBoxASPWM == _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.cend()) {
            break;
        }

        //save box in the order along the route. Hence a boxSet can be splitted in the json

        QJsonObject qjsonObject_boxesSet;

        S_BoxAndStackedProfilWithMeasurements* sBASPWMPtr = iterSBoxASPWM->_sBASPWMPtr;

        if (!sBASPWMPtr) {
            continue;
        }

        qint64 boxSetID = sBASPWMPtr->_profilsBoxParameters._sUniqueBoxID._setID;
        QString qstrBoxSetID = QString::number(static_cast<qulonglong>(boxSetID));

        int ieDBSBuAD   = sBASPWMPtr->_profilsBoxParameters._sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid;

        //we put _ for very internal usage fields. Disencouraging human to play with in edition and keeping quiet obscur naming convention
        //in the same way, we save only if it's useful for internal use later
        if (ieDBSBuAD != e_DBSBuDA_notSet) {
            QString qstrKey_boxSetID {"_boxesSetID"};
            qjsonObject_boxesSet.insert(qstrKey_boxSetID, qstrBoxSetID);

            QString qstrKey_ieDistanceBSBUAD {"_ieDistanceBSBUAD"};
            qjsonObject_boxesSet.insert(qstrKey_ieDistanceBSBUAD, ieDBSBuAD);
        }

        QJsonArray qjsonArrayBoxes;
        int idxForJsonArrayBoxInsert = 0;

        for (; iterSBoxASPWM != _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.cend(); ++iterSBoxASPWM) {

            S_BoxAndStackedProfilWithMeasurements* sBASPWMPtr_sameBoxSet = iterSBoxASPWM->_sBASPWMPtr;

            if (!sBASPWMPtr_sameBoxSet) {
                continue;
            }

            if (sBASPWMPtr_sameBoxSet->_profilsBoxParameters._sUniqueBoxID._setID != boxSetID) {
                break;
            }

            QJsonObject qjsonObject_profilsBoxParameters;
            bool bReport_toQJsonObj_profilsBoxParameters = sBASPWMPtr_sameBoxSet->_profilsBoxParameters.toQJsonObject(idxBox, qjsonObject_profilsBoxParameters);
            if (!bReport_toQJsonObj_profilsBoxParameters) {
                strDetailsAboutFail_projectWrite = "stacked profil box parameters: failed to toQJsonObject on box #"+QString::number(idxBox);
                return(false);
            }
            idxBox++;

            qjsonArrayBoxes.insert(idxForJsonArrayBoxInsert, qjsonObject_profilsBoxParameters);
            idxForJsonArrayBoxInsert++;
        }
        if (!qjsonArrayBoxes.empty()) {
            qjsonObject_boxesSet.insert("boxes", qjsonArrayBoxes);
            qjsonArray_stackedProfilBoxes.insert(idxForJsonArrayBoxSetInsert, qjsonObject_boxesSet);
            idxForJsonArrayBoxSetInsert++;
        }
    }

    return(true);
}


//@LP do not forget to used through inheritQATableModel
//
//@@LP if return false, have to call:
//     _qlnklistBoxAndStackedProfilWithMeasurements.clear();
//     and
//     .clear();
//@@LP + needs layout changed and stuff about it
//

bool ComputationCore::stackedProfilBoxes_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfilBoxes,
                                                        const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                                        QString& strDetailsAboutFail_projectLoad) {

    strDetailsAboutFail_projectLoad.clear();

    if (!_routePtr) {
        strDetailsAboutFail_projectLoad = "internal dev error #107: _routePtr is nullptr";
        return(false);
    }
    if (_routePtr->getId() != 0) {
        strDetailsAboutFail_projectLoad = "internal dev error #108: _routePtr getId is not zero";
        return(false);
    }

    QVector<S_ProfilsBoxParameters> qvectProfilsBoxParameters;

    //as boxset can be splitted, we keep the loaded BoxSetID with its according eDBSBuAD to chech that new boxset section with the same BoxSetID has the same eDBSBuAD
    QHash<QString, int> _qhashLoaded_strBoxSetID_ieDBSBuAD;

    if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()) {
        strDetailsAboutFail_projectLoad = "internal error #103: stacked profil boxes list not cleared";
        return(false);
    }

    int iQjsonArray_stackedProfilBoxes_size = qjsonArray_stackedProfilBoxes.size();

    if (!iQjsonArray_stackedProfilBoxes_size) {
        //no boxes (in boxesSet) to load
        strDetailsAboutFail_projectLoad = "stacked profil boxesSet array is empty";
        return(false);
    }

    qint64 maximalLimitOfValidValueOfBoxSetIDFromJson_fromCurrentTime = IDGenerator::generateABoxesSetID();

    //QList<qint64> qlistqint64_boxesSetIDLoaded;

    int idxBoxForCheck = 0;

    for(int idxInArrayBoxesSet = 0; idxInArrayBoxesSet < iQjsonArray_stackedProfilBoxes_size; idxInArrayBoxesSet++) {

        //boxes are in boxesSet with optionnal _boxSetID and _ieDistanceBSBUAD
        QJsonObject qjsonObject_boxesSet = qjsonArray_stackedProfilBoxes.at(idxInArrayBoxesSet).toObject();

        QString qstrKey_boxSetID {"_boxesSetID"};
        QString qstrBoxSetID;
        bool b_qstrBoxSetIDGot = getStringValueFromJson(qjsonObject_boxesSet, qstrKey_boxSetID, qstrBoxSetID);

        qint64 boxSetID = 0;
        e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDBSBuAD = e_DBSBuDA_notSet;

        if (b_qstrBoxSetIDGot) {

            boxSetID = static_cast<qint64>(qstrBoxSetID.toULongLong());
            if (boxSetID < 0) {
                strDetailsAboutFail_projectLoad = "boxSetID can not be < 0 ; got: " + qstrBoxSetID;
                return(false);
            }

            if (boxSetID >= maximalLimitOfValidValueOfBoxSetIDFromJson_fromCurrentTime) {
                strDetailsAboutFail_projectLoad = "boxSetID can not refers to a futur datetime ; got: " + qstrBoxSetID;
                return(false);
            }
            //qlistqint64_boxesSetIDLoaded.push_back(boxSetID);
        }

        QString qstrKey_ieDistanceBSBUAD {"_ieDistanceBSBUAD"};
        int ieDBSBuAD = e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution::e_DBSBuDA_notSet;
        bool b_ieDBSBuADGot = getIntValueFromJson(qjsonObject_boxesSet, qstrKey_ieDistanceBSBUAD, ieDBSBuAD);
        if (b_ieDBSBuADGot) {
            eDBSBuAD = projectJsonInt_to_e_DistanceBetweenSuccessivesBoxesUAD(ieDBSBuAD);
            if (eDBSBuAD == e_DBSBuDA_notSet) {
                strDetailsAboutFail_projectLoad = "key "+ qstrKey_ieDistanceBSBUAD + "can not exist with value << " + e_DBSBuDA_notSet + " in json file";
                return(false);
            }
        }

        if (b_ieDBSBuADGot && !b_qstrBoxSetIDGot) {
            strDetailsAboutFail_projectLoad =  "key " + qstrKey_ieDistanceBSBUAD + " needs to come with an associated key" + qstrKey_boxSetID;
            return(false);
        }

        //
        if (b_qstrBoxSetIDGot) {
            auto iterFound = _qhashLoaded_strBoxSetID_ieDBSBuAD.constFind(qstrBoxSetID);
            if (iterFound == _qhashLoaded_strBoxSetID_ieDBSBuAD.cend()) {
                _qhashLoaded_strBoxSetID_ieDBSBuAD.insert(qstrBoxSetID, ieDBSBuAD);
            } else {
                if (iterFound.value() != ieDBSBuAD) {
                    strDetailsAboutFail_projectLoad = "_ieDistanceBSBUAD has to be the same for boxSetID " + qstrBoxSetID + " in json file";
                    return(false);
                }
            }
        }

        //get boxes array:
        QString qstrKey_boxesArray {"boxes"};
        if (!qjsonObject_boxesSet.contains(qstrKey_boxesArray)) {
            strDetailsAboutFail_projectLoad = "key not found:" + qstrKey_boxesArray;
            return(false);
        }
        QJsonArray qjsonArrayBoxes = qjsonObject_boxesSet.value(qstrKey_boxesArray).toArray();

        int iqJsonArrayBoxes_size = qjsonArrayBoxes.size();
        if (!iqJsonArrayBoxes_size) {
            //no boxes (in boxesSet) to load
            strDetailsAboutFail_projectLoad = "stacked profil boxes array is empty in Boxeset #"+ QString::number(idxInArrayBoxesSet);
            return(false);
        }

        for(int idxInArrayBoxes = 0; idxInArrayBoxes < iqJsonArrayBoxes_size; idxInArrayBoxes++) {

            S_ProfilsBoxParameters profilsBoxParameters;
            bool bGot = profilsBoxParameters.fromQJsonObject(qjsonArrayBoxes.at(idxInArrayBoxes).toObject());
            if (!bGot) {
                strDetailsAboutFail_projectLoad = "failed loading stackedProfilBoxesx #" + QString::number(idxBoxForCheck);
                return(false);
            }

            if (profilsBoxParameters._sUniqueBoxID._idxBoxInSet != idxBoxForCheck) {
                strDetailsAboutFail_projectLoad = "failed loading stackedProfilBoxesx #" + QString::number(idxBoxForCheck) + "(boxId does not match)";
                return(false);
            }

            //check and relocate accurately the centerPoint if associated to a existing segment index
            //(- if idxSegmentOwnerOfCenterPoint != -1:
            //   . idxSegmentOwnerOfCenterPoint matches with an existing route segment
            //   . set again the centerPoint using centerPoint as base location and idxSegmentOwnerOfCenterPoint)

            if (profilsBoxParameters._idxSegmentOwnerOfCenterPoint != -1) {
                int segmentCount = _routePtr->segmentCount();
                if (!segmentCount) {
                    strDetailsAboutFail_projectLoad = "route does not contain any segment";
                    return(false);
                }
                if (profilsBoxParameters._idxSegmentOwnerOfCenterPoint >= segmentCount) {
                    strDetailsAboutFail_projectLoad =  "boxe with boxId: " + QString::number(idxBoxForCheck)
                                                      + " refers to segment index: " + QString::number(profilsBoxParameters._idxSegmentOwnerOfCenterPoint)
                                                      + "but does not exist in route";
                    return(false);
                }

                qDebug() << __FUNCTION__ << "idxInArrayBoxes = " << idxInArrayBoxes;
                S_Segment segmentOwner = _routePtr->getSegment(profilsBoxParameters._idxSegmentOwnerOfCenterPoint);
                bool bCheckThatPointIsAlignedWithSegmentOwnerPoints = checkThatThreePointsAreAligned(segmentOwner._ptA, segmentOwner._ptB, profilsBoxParameters._qpfCenterPoint);
                //@LP if not aligned enough and if relocation using perpendicular projection gives a too different location => reject the input file


                QPointF relocatedPoint {.0,.0};
                //relocate now the center point, using centerpoint and associated segment
                e_ResultDetailsOfRelocatedPointInSegment eRDORPIS = relocatePointInSegment(profilsBoxParameters._qpfCenterPoint,
                                                                                          _routePtr->getSegment(profilsBoxParameters._idxSegmentOwnerOfCenterPoint),
                                                                                          relocatedPoint);

                if (eRDORPIS == e_RDORPIS_pointABCnotAligned) {
                    strDetailsAboutFail_projectLoad = "dev error #800: alignement points check error. boxId: "+ QString::number(idxBoxForCheck)
                                                     + "segment index: " + QString::number(profilsBoxParameters._idxSegmentOwnerOfCenterPoint);
                    return(false);
                }

                if (  (eRDORPIS == e_RDORPIS_notRelocated_BisCloser)
                    ||(eRDORPIS == e_RDORPIS_notRelocated_AisCloser)) {

                    strDetailsAboutFail_projectLoad =  "CenterPoint too far to segment to be relocated. boxId: "+ QString::number(idxBoxForCheck)
                                                     + "segment index: " + QString::number(profilsBoxParameters._idxSegmentOwnerOfCenterPoint);
                    return(false);
                }

                //@LP using relocated centerpoint can produce little difference in result, hence keep the loaded point
                //if considered not so far to relocated point
                //
                // // //profilsBoxParameters._qpfCenterPoint = relocatedPoint;

                S_Segment segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint;
                segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._ptA = profilsBoxParameters._qpfCenterPoint;
                segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._ptB = relocatedPoint;
                segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._bValid = true;

                double distance_relocatedCenterPoint_to_centerPoint = segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint.length();

                //@#LP 0.1 is enough ?
                if (distance_relocatedCenterPoint_to_centerPoint > 0.1) {
                    strDetailsAboutFail_projectLoad =  "Checking centerPoint relocation said distance > 0.1 ("
                                                        + QString::number(distance_relocatedCenterPoint_to_centerPoint) + ") "
                                                        + "boxId: " + QString::number(idxBoxForCheck)
                                                        + "segment index: " + QString::number(profilsBoxParameters._idxSegmentOwnerOfCenterPoint);
                    return(false);
                }
            }

            if (b_qstrBoxSetIDGot) {
                profilsBoxParameters._sUniqueBoxID._setID = boxSetID;
            } else {
                 profilsBoxParameters._sUniqueBoxID._setID = -1;//profilsBoxParameters._sUniqueBoxID._setID will be set below
            }

            profilsBoxParameters._sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid = eDBSBuAD;

            qvectProfilsBoxParameters.push_back(profilsBoxParameters);

            idxBoxForCheck++;
        }
    }

    //check that BoxSetIDs from json are all unique
    /*QSet<qint64> qsetqint64_boxesSetIDLoaded  = qlistqint64_boxesSetIDLoaded.toSet();
    if (qsetqint64_boxesSetIDLoaded.size() != qlistqint64_boxesSetIDLoaded.size()) {
        strDetailsAboutFail_projectLoad = "boxesSetID are not all unique";
        return(false);
    }*/

    //set the setID for boxes without value from the loading current time
    for(auto& profilsBoxParam: qvectProfilsBoxParameters) {
        if (profilsBoxParam._sUniqueBoxID._setID == -1) {
            profilsBoxParam._sUniqueBoxID._setID = maximalLimitOfValidValueOfBoxSetIDFromJson_fromCurrentTime;
        }
    }

    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
    int aboutQVectOrderAlongRoute_indexOfLastChange = 0;

    bool bCriticalErrorOccured = false;

    int vectBoxIdFirstInsert = -1;
    int keyMapFirstInsert = -1;
    bool bAddReport = add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(qvectProfilsBoxParameters,
                                                                                             qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                                                                             true,
                                                                                             vectBoxIdFirstInsert, keyMapFirstInsert,
                                                                                             //boxes saved and loaded from first point to last point of the route (do not take into account the orientation fault)
                                                                                             aboutQVectOrderAlongRoute_indexOfFirstChange,
                                                                                             aboutQVectOrderAlongRoute_indexOfLastChange,
                                                                                             bCriticalErrorOccured);

    return(bAddReport);
}


//@LP do not forget to used through inheritQATableModel
//
//@@LP if return false, have to call:
//     _qlnklistBoxAndStackedProfilWithMeasurements.clear();
//     and
//     .clear();
//@@LP + needs layout changed and stuff about it
bool ComputationCore::feed_stackedProfil_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfil,
                                                       const QVector<bool>& qvectb_componentToCompute,
                                                       QString& strDetailsAboutFail_projectLoad) {

    if (!qvectb_componentToCompute.count(true))  {
        strDetailsAboutFail_projectLoad = "internal dev error #109: no layersToCompute";
        return(false);
    }

    int qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();
    if (!qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements_size) {
        strDetailsAboutFail_projectLoad = "internal dev error #110: BoxAndStackedProfilWithMeasurements map is empty";
        return(false);
    }

    int iQjsonArray_stackedProfil_size = qjsonArray_stackedProfil.size();
    if (iQjsonArray_stackedProfil_size != qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements_size) {
        strDetailsAboutFail_projectLoad =  "StackedProfil count mismatch with for existing Boxes count. Got " + QString::number(iQjsonArray_stackedProfil_size)
                                         + ", should be " + QString::number(qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements_size);
        return(false);
    }

    QMap<int, S_BoxAndStackedProfilWithMeasurements>::iterator iterSBASPWM = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
    int boxId_toSet = 0;

    for (int idxInArrayStackedProfil = 0; idxInArrayStackedProfil < iQjsonArray_stackedProfil_size; idxInArrayStackedProfil++) {

        iterSBASPWM = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.find(boxId_toSet);
        if (iterSBASPWM == _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end()) {
            //mismatching content
            return(false);
        }

        qDebug() << __FUNCTION__ << "(*) idxInArrayStackedProfil = " << idxInArrayStackedProfil;

        //qDebug() << __FUNCTION__ << "(*) qjsonArray_stackedProfil[idxInArrayStackedProfil]).toObject() = " << (qjsonArray_stackedProfil[idxInArrayStackedProfil]).toObject();

        bool bStackedProfilFeedFromJSon = iterSBASPWM.value().
                profilWithMeasurements_withoutLinearRegresionModelResult_fromQJsonObject(
                    (qjsonArray_stackedProfil[idxInArrayStackedProfil]).toObject(),
                    qvectb_componentToCompute,
                    idxInArrayStackedProfil,
                    strDetailsAboutFail_projectLoad);

        if (!bStackedProfilFeedFromJSon) {

            return(false);
        }

        iterSBASPWM.value().showContent_mainMeasurement_linearRgressionModel();

        boxId_toSet++;

    }

    return(true);
}

bool ComputationCore::stackedProfilWithMeasurements_withoutLinearRegresionModelResult_toQJsonArray(
        QJsonArray& qjsonArray_stackedProfilBoxesWithMeasurements,
        QString& strDetailsAboutFail_projectWrite) const {

    if (!_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size()) {
        strDetailsAboutFail_projectWrite = "none stacked profil boxes";
        return(false);
    }

    int nbBox = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    for (int idxBox = 0; idxBox < nbBox; idxBox++) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxBox]._sBASPWMPtr;

        if (!SBASPWSPtr) {
            return(false);

        }

        QJsonObject qjsonObject_profilWithMeasurements_withoutLRMR;
        bool bReport_toQJsonObj_profilsWithMeasurements =
                SBASPWSPtr->profilWithMeasurements_withoutLinearRegresionModelResult_toQJsonObject(
                    _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
                    idxBox,
                    qjsonObject_profilWithMeasurements_withoutLRMR);

        if (!bReport_toQJsonObj_profilsWithMeasurements) {
            strDetailsAboutFail_projectWrite = "stacked profil withMeasurements: failed to toQJsonObject on box #"+QString::number(idxBox);
            return(false);
        }

        qjsonArray_stackedProfilBoxesWithMeasurements.insert(idxBox, qjsonObject_profilWithMeasurements_withoutLRMR);
    }

    return(true);
}

bool ComputationCore::stackedProfilWithMeasurements_withLinearRegresionModelResult_toQJsonArray(
        QJsonArray& qjsonArray_stackedProfilBoxesWithMeasurements,
        bool bReverseOrderInQJsonArray,
        const S_ExportSettings &sExportSettings,
        bool bAddOneToBoxIds,
        QString& strDetailsAboutFail_projectWrite) const {

    if (!_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size()) {
        strDetailsAboutFail_projectWrite = "none stacked profil boxes";
        return(false);
    }

    int nbBox = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    //boxID stay like in the project,
    //if the progress Direction is from last to first point of the route:
    // - we reverse the unit vector direction (in the sofware it's always stored from first to last point)
    // - we write boxes in the reverse order of the qvect
    int idxJsonArrayExport = 0;
    int idxBox = 0;
    int idxBoxProgressIncrement = +1;
    if (bReverseOrderInQJsonArray) {
        idxBox = nbBox - 1;
        idxBoxProgressIncrement = -1;
    }

    //if export only the current box, set on it, and set the increment to be over the boxId range
    if (sExportSettings._ieExportResult_boxesContent == e_ERbC_onlyCurrent) {
        idxBox = sExportSettings._idOfCurrentBox;
        idxBoxProgressIncrement *= nbBox;
        if ((idxBox < 0)||(idxBox >= nbBox)) {
            strDetailsAboutFail_projectWrite = "invalid current Box Id";
            return(false);
        }
    }

    for (;vectBoxIdValid(idxBox); idxBox+=idxBoxProgressIncrement) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxBox]._sBASPWMPtr;

        if (!SBASPWSPtr) {
            return(false);

        }

        QVector<bool> qvectbSetLinearRegressionData_asEmpty(3, false);

        if (sExportSettings._bIfWarnFlagByUser_setLinearRegressionData_asEmpty) {
            for (int iComponent = e_CA_Perp; iComponent <= e_CA_deltaZ; iComponent++) {
                if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[iComponent]) {
                    S_MeasureIndex measureIndex(true);
                    bool bWarnFlag = false;
                    bool bGotReport = SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[iComponent].get_warnFlag(measureIndex, bWarnFlag);
                    if (!bGotReport) {
                        return(false);
                    }
                    qvectbSetLinearRegressionData_asEmpty[iComponent] = bWarnFlag;
                }
            }
        }

        QJsonObject qjsonObject_profilWithMeasurements_withLRMR;
        bool bReport_toQJsonObj_profilsWithMeasurements =
                SBASPWSPtr->profilWithMeasurements_withLinearRegresionModelResult_toQJsonObject(
                    _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
                    idxBox,
                    qvectbSetLinearRegressionData_asEmpty,
                    qjsonObject_profilWithMeasurements_withLRMR);

        if (!bReport_toQJsonObj_profilsWithMeasurements) {
            strDetailsAboutFail_projectWrite = "stacked profil withMeasurements: failed to toQJsonObject on box #"+QString::number(idxBox);
            return(false);
        }

        if (bAddOneToBoxIds) { //let a boolean to control this case
            //
            //for json result export the boxId matches with what is displayed in User Interface: boxId from logic +1. It's not the boxId from logic (like in project json file)
            //hence json and ascii result export files are sync about boxId alignment
            QString qstrKey_boxId {"boxId"};
            qjsonObject_profilWithMeasurements_withLRMR.insert(qstrKey_boxId, idxBox+1);
            //
            //
        }

        qjsonArray_stackedProfilBoxesWithMeasurements.insert(idxJsonArrayExport, qjsonObject_profilWithMeasurements_withLRMR);
        idxJsonArrayExport++;
    }

    return(true);
}

bool ComputationCore::stackedProfilLinearRegressionModelResult_forBoxId_andComponent_toASciiStringlist(
        int vectBoxId, e_ComponentsAccess eComponentsAccess, bool bIfWarnFlagByUser_setLinearRegressionData_asEmpty,
        QStringList &strList_data) const {

    if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ.size() < 3) {
        return(false);
    }
    if (!_qvectb_componentsToCompute_Perp_Parall_DeltaZ[eComponentsAccess]) {
        return(false);
    }
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    /*
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
    */

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId]._sBASPWMPtr;

    if (!SBASPWSPtr) {
        return(false);
    }

    S_MeasureIndex sMeasureIndex(true);

    S_LeftRightSides_linearRegressionModel *ptr_S_LeftRightSides_linearRegressionModel =
            SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[eComponentsAccess].
            getPtrOn_LRSidesLinearRegressionModel(sMeasureIndex);
    if (!ptr_S_LeftRightSides_linearRegressionModel) {
        return(false);
    }

    bool bSetLinearRegressionData_asEmpty = false;
    if (ptr_S_LeftRightSides_linearRegressionModel->_bWarningFlagByUser) {
        if (bIfWarnFlagByUser_setLinearRegressionData_asEmpty) {
            bSetLinearRegressionData_asEmpty = true;
        }
    }

    bool bNoWarningFlagByUser = !(ptr_S_LeftRightSides_linearRegressionModel->_bWarningFlagByUser);
    strList_data.push_back(boolToAsciiBoolString(bNoWarningFlagByUser));

    if (bSetLinearRegressionData_asEmpty) {
        strList_data.push_back("N/A"); //'N/A' value for boolean modelValuesResults._bComputed if bSetLinearRegressionData_asEmpty
        strList_data.push_back(""); //empty yOffsetAtX0
        strList_data.push_back(""); //empty sigmabSum
    } else {
        bool bModelComputed = ptr_S_LeftRightSides_linearRegressionModel->_modelValuesResults._bComputed;
        strList_data.push_back(boolToAsciiBoolString(bModelComputed));

        if (!bModelComputed) {
            strList_data.push_back(""); //empty yOffsetAtX0
            strList_data.push_back(""); //empty sigmabSum
        } else {
            strList_data.push_back(doubleToQStringPrecision_g(ptr_S_LeftRightSides_linearRegressionModel->_modelValuesResults._yOffsetAtX0,14));
            strList_data.push_back(doubleToQStringPrecision_g(ptr_S_LeftRightSides_linearRegressionModel->_modelValuesResults._sigmabSum  ,14));
        }
    }


    bool bReport = true;
    QStringList qstrList_sideOfLinearRegressionModel_left;
    bReport &= ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_left].
        linearRegrModel_toASciiStringlist(bSetLinearRegressionData_asEmpty,
                                          qstrList_sideOfLinearRegressionModel_left);

    QStringList qstrList_sideOfLinearRegressionModel_right;
    bReport &= ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_right].
        linearRegrModel_toASciiStringlist(bSetLinearRegressionData_asEmpty,
                                          qstrList_sideOfLinearRegressionModel_right);

    if (!bReport) {
        return(false);
    }

    strList_data.append(qstrList_sideOfLinearRegressionModel_left);
    strList_data.append(qstrList_sideOfLinearRegressionModel_right);

    return(true);
}



bool ComputationCore::stackedProfilLinearRegressionParametersWithoutResult_forBoxId_andComponent_toASciiStringlist(
        int vectBoxId, e_ComponentsAccess eComponentsAccess, QStringList &strList_data) const {

    if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ.size() < 3) {
        return(false);
    }
    if (!_qvectb_componentsToCompute_Perp_Parall_DeltaZ[eComponentsAccess]) {
        return(false);
    }
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    /*
    "leftSide_xMin",
    "leftSide_xMax",
    "rightSide_xMin",
    "rightSide_xMax",
    "X0"
    */

    S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId]._sBASPWMPtr;

    if (!SBASPWSPtr) {
        return(false);
    }

    S_MeasureIndex sMeasureIndex(true);

    S_LeftRightSides_linearRegressionModel *ptr_S_LeftRightSides_linearRegressionModel =
            SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[eComponentsAccess].
            getPtrOn_LRSidesLinearRegressionModel(sMeasureIndex);
    if (!ptr_S_LeftRightSides_linearRegressionModel) {
        return(false);
    }

    //@#LP considers all these field are valid:
    strList_data.push_back(doubleToQStringPrecision_justFit(ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_left ]._xRangeForLinearRegressionComputation._xMin, 14));
    strList_data.push_back(doubleToQStringPrecision_justFit(ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_left ]._xRangeForLinearRegressionComputation._xMax, 14));
    strList_data.push_back(doubleToQStringPrecision_justFit(ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._xMin, 14));
    strList_data.push_back(doubleToQStringPrecision_justFit(ptr_S_LeftRightSides_linearRegressionModel->_linearRegressionParam_Left_Right[e_LRsA_right]._xRangeForLinearRegressionComputation._xMax, 14));
    strList_data.push_back(doubleToQStringPrecision_justFit(ptr_S_LeftRightSides_linearRegressionModel->_x0ForYOffsetComputation._x0, 14));

    return(true);
}


bool ComputationCore::computedComponentsToHeader_toAscii(QStringList& qstrList_computedComponentsToHeader,
                                                         QStringList& qstrList_title_tuple_X_Perp_Parall_DeltaZ) const {

    int vectBoxId = 0;
    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    const auto& constRef_SBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId];

    bool bReport_toQJsonObj_computedComponentsToHeader =
        constRef_SBoxASPWM._sBASPWMPtr->computedComponentsToHeader_toAscii_aboutProfilCurves(
            _vect_eComputationMethod_Perp_Parall_DeltaZ,
            _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
            qstrList_computedComponentsToHeader,
            qstrList_title_tuple_X_Perp_Parall_DeltaZ);

    return(bReport_toQJsonObj_computedComponentsToHeader);
}


bool ComputationCore::profilCurvesForOneStackedProfilBox_toAscii(int vectBoxId,
                                                                 bool bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                                                                 QVector<QStringList>& qvectQstrList_profilCurveContent) const {

    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    const auto& constRef_SBoxASPWM = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId];

    bool bReport_toQJsonObj_profilsBoxParameters =
        constRef_SBoxASPWM._sBASPWMPtr->profilCurves_toAscii(vectBoxId, _vect_eComputationMethod_Perp_Parall_DeltaZ,
                                                             _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
                                                             bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                                                             qvectQstrList_profilCurveContent);

    return(bReport_toQJsonObj_profilsBoxParameters);
}

//@LP setting default min max center values
bool ComputationCore::setLength_ofOneProfilsBox(int vectBoxId, int oddPixelLength) {

    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements* boxAndStackedProfilWithMeasurementsPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId]._sBASPWMPtr;

    boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters._oddPixelLength = oddPixelLength;

    int evenLengthOneSide = oddPixelLength/2;
    //minus from counter clock wise
    boxAndStackedProfilWithMeasurementsPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil.set(static_cast<double>(-evenLengthOneSide));

    boxAndStackedProfilWithMeasurementsPtr->clearMeasurementsDataContent();
    boxAndStackedProfilWithMeasurementsPtr->setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel(_qvectb_componentsToCompute_Perp_Parall_DeltaZ);

    return(true);
}

//@LP setting default min max center values
bool ComputationCore::updateProfilsBoxParameters_noBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, bool bForceBreakAutomaticDistributionLink) {

    if (!vectBoxIdValid(vectBoxId)) {
        qDebug() << __FUNCTION__ << "reject case #1";
        return(false);
    }

    S_BoxAndStackedProfilWithMeasurements* boxAndStackedProfilWithMeasurementsPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[vectBoxId]._sBASPWMPtr;

    boxAndStackedProfilWithMeasurementsPtr->clearMeasurementsDataContent();

    qDebug() << __FUNCTION__ << "(ComputationCore) replace now boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters:";
    boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters.showContent();
    qDebug() << __FUNCTION__ << "(ComputationCore) by :";
    profilsBoxParameters.showContent();

    S_ProfilsBoxParameters udpated_profilsBoxParameters = profilsBoxParameters;

    if (!bForceBreakAutomaticDistributionLink) {
        //if the box was located relativ to other boxes (close as possible):
        if (boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters._sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid != e_DBSBuDA_notSet) {
            //if new width is different than old width:
            if (profilsBoxParameters._oddPixelWidth == boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters._oddPixelWidth) {
                udpated_profilsBoxParameters._sUniqueBoxID = boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters._sUniqueBoxID;
                udpated_profilsBoxParameters._sInfoAboutAutomaticDistribution = boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters._sInfoAboutAutomaticDistribution;
            }
        }
    }

    boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters = udpated_profilsBoxParameters;

    int evenLengthOneSide = profilsBoxParameters._oddPixelLength/2;
    //minus from counter clock wise
    boxAndStackedProfilWithMeasurementsPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil.set(static_cast<double>(-evenLengthOneSide));

    qDebug() << __FUNCTION__ << __LINE__ << "_qvectb_componentsToCompute_Perp_Parall_DeltaZ = " << _qvectb_componentsToCompute_Perp_Parall_DeltaZ;

    boxAndStackedProfilWithMeasurementsPtr->setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel(_qvectb_componentsToCompute_Perp_Parall_DeltaZ);

    qDebug() << __FUNCTION__ << "(ComputationCore) after replacement: boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters is:";
    boxAndStackedProfilWithMeasurementsPtr->_profilsBoxParameters.showContent();

    return(true);
}

bool ComputationCore::updateProfilsBoxParameters_withBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, int& newVectBoxId_afterReorderAlongTheRoute) {

    if (!vectBoxIdValid(vectBoxId)) {
        qDebug() << __FUNCTION__ << "reject case #1";
        return(false);
    }

    //update the fields like if no box move
    bool bUpdateProfilsBoxParameters_noBoxMove = updateProfilsBoxParameters_noBoxMove(vectBoxId, profilsBoxParameters, true);
    if (!bUpdateProfilsBoxParameters_noBoxMove) {
        qDebug() << __FUNCTION__ << "reject case #2";
        return(false);
    }

    //now re-order the qvect to take into account the new box locations along the route relative to the others boxes:
    //update qvect along route
    newVectBoxId_afterReorderAlongTheRoute = -1;
    update_qvectOf_BASPWMPtr_orderedAlongRoute_afterOneBoxMove(vectBoxId, newVectBoxId_afterReorderAlongTheRoute);

    return(true);
}

//update qvectOf considering that the inserted boxes at the end of the qmap were inserted in the route order, along the route.
//This is the case with automatic distribution
//nbAddedProfilsBoxParametersAtEndOfQMap is the count of this boxes set inserted
bool ComputationCore::update_qvectOf_BASPWMPtr_orderedAlongRoute_afterProfilBoxesAddedAtQMapEnd(
        int nbAddedProfilsBoxParametersAtEndOfQMap,
        bool bBoxesDirectionOrderIsFirstPointToLastPoint,
        int& aboutQVectOrderAlongRoute_indexOfFirstChange,
        int& aboutQVectOrderAlongRoute_indexOfLastChange,
        bool& bCriticalErrorOccured) {

    qDebug() << __FUNCTION__ << "nbAddedProfilsBoxParametersAtEndOfQMap = " << nbAddedProfilsBoxParametersAtEndOfQMap;

    bCriticalErrorOccured = false;

    if (!nbAddedProfilsBoxParametersAtEndOfQMap) {
        qDebug() << __FUNCTION__ << "nbAddedProfilsBoxParameters is zero";
        //add error msg
        return(false);
    }

    showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "entering with:");

    qDebug() << __FUNCTION__<< "___part of qmap to insert along the route inside the vect____";

    int firstQmapIndex_ofFirstBoxInAddedSet = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey() - nbAddedProfilsBoxParametersAtEndOfQMap + 1;

    //qDebug() << __FUNCTION__<< "idxQmapCount= " << idxQmapCount;
    qDebug() << __FUNCTION__<< "firstQmapIndex_ofFirstBoxInAddedSet= " << firstQmapIndex_ofFirstBoxInAddedSet;

    auto iterPair_addedBox_boxId_BASPWM = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.find(firstQmapIndex_ofFirstBoxInAddedSet);
    if (iterPair_addedBox_boxId_BASPWM == _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end()) {
        //@LP if we are here, it's a major algorithm failure. Hence we make a cleanup removing the added boxes!
        //(should really never never happens)
        qDebug() << __FUNCTION__ << "major algorithm failure #3, need cleanup !";
        bCriticalErrorOccured = true;
        return(false);
    }

    auto& firstBoxToAddInVect = iterPair_addedBox_boxId_BASPWM.value();
    auto& lastBoxToAddInVect = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.last();

    qDebug() << "@@@@@@@@@@@";
    int idxQMap = firstQmapIndex_ofFirstBoxInAddedSet;
    auto iterPair_addedBox_boxId_BASPWM__ = iterPair_addedBox_boxId_BASPWM;
    for (; iterPair_addedBox_boxId_BASPWM__ != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end(); ++iterPair_addedBox_boxId_BASPWM__) {
        qDebug() << "idxQMap = " << idxQMap;
        iterPair_addedBox_boxId_BASPWM__.value()._profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();
        idxQMap++;
    }

    //example with _idxSegmentOwnerOfCenterPoint by box:
    //
    //in vector:
    // vector idx      : 0  1  2  3  4  5  6  7  8  9  10
    // idxSegmentOwner : 1  2  2  2  3  3  4  4  4  4   5
    //                   a  b  c  d  e  f  g  h  j  k   m  ( IDs for this example, to understand more easily the result )
    //
    //qmap with _idxSegmentOwnerOfCenterPoint:
    //
    //                    ___________previously sync______ | __added boxes__
    // idxSegmentOwner :  1  2  2  2  3  3  4  4  4  4   5 | 0  1  3  7  8
    //                    a  b  c  d  e  f  g  h  j  k   m | P  R  S  T  U ( IDs for this example, to understand more easily the result )
    //
    // required result in vector:
    //                    ________________________________________________
    //  idxSegmentOwner : 0  1  1  2  2  2  3  3  3  4   4   4  4  5  7  8
    //                    P  a  R  b  c  d  e  f  S  g   h   j  k  m  T  U  // the exact location of R and S in the vector depends of the
    //                                                                      // relative point location with the other box center on the same segment
    //

    int count_qvectOf_BASPWMPtr_beforeUpdate = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.count();

    //simple case: the first insertion of box in the qmap
    if (!count_qvectOf_BASPWMPtr_beforeUpdate) {


        qDebug() << __FUNCTION__ << "if (!count_qvectOf_BASPWMPtr_beforeUpdate) {";
        int idxInsertionInQMap = 0;
        for (auto iter = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
                 iter != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
             ++iter) {
            _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(S_InsertionIndex_SBASPWMPtr {idxInsertionInQMap, &(*iter)});
            idxInsertionInQMap++;
        }

        showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting case #000 with:");

        aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
        aboutQVectOrderAlongRoute_indexOfLastChange = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

        return(true);
    }

    //the qvector is not empty, hence inserted boxes can be anywhere along the route, before or after any existing boxes

    qDebug() << __FUNCTION__ << ": line " << __LINE__ << "count_qvectOf_BASPWMPtr_beforeUpdate = " << count_qvectOf_BASPWMPtr_beforeUpdate;

    //stores the boxes ptr vector part which will not be altered by the update
    //because the boxes from this vect part is for sure after the last box to add according to _idxSegmentOwnerOfCenterPoint

    //example with idxSegmentOwner of the boxes:
    //- qmap content:                [ 0  0  1  1  1  2  2  3  4  5  5  6 <2  3  4  5> ] (<2 3 4 5> is the new added boxes set)
    //- the qvect content wanted is: [ 0  0  1  1  1  2 <2> 2 <3> 3  4 <4> 5  5 <5> 6  ]
    //  (the exact location in the qvect of <2> <3> <4> <5> depends on their relative center point location to the other boxes center point)
    //Hence:
    //- partBefore_whichNeedsNoUpdate = [ 0 0 1 1 1 ]
    //- partAfter_whichNeedsNoUpdate  = [ 6 ]
    //- partInBetween                 = [ 2  2  3  4  5  5 ]

    QVector<S_InsertionIndex_SBASPWMPtr> partBefore_whichNeedsNoUpdate;
    int idxqvect_firstBoxOfInBetween = -1;
    bool bGot_idxqvect_firstBoxOfInBetween = false;

    QVector<S_InsertionIndex_SBASPWMPtr> partAfter_whichNeedsNoUpdate;
    int idxqvect_lastBoxOfInBetween = -1;
    bool bGot_idxqvect_lastBoxOfInBetween = false;

    for (int idxqvect = 0; idxqvect < count_qvectOf_BASPWMPtr_beforeUpdate; idxqvect++) {

        qDebug() << "cmp: IdxSegmentOwner of vect[ " << idxqvect << "]= " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                 << " with firstBoxToAddInVect =" << firstBoxToAddInVect._profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

        int diff_idxSegmentOwner_current_firstBoxToAddInVect =
                _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                - firstBoxToAddInVect._profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

        qDebug() << "diff_idxSegmentOwner_current_firstBoxToAddInVect = " << diff_idxSegmentOwner_current_firstBoxToAddInVect;

        if (diff_idxSegmentOwner_current_firstBoxToAddInVect < 0) {
            qDebug() << "smaller => insert vect[ " << idxqvect << "] into partBefore_whichNeedsNoUpdate";
            partBefore_whichNeedsNoUpdate.push_back(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]);

        } else {

            qDebug() << "bigger or equal";

            if (!bGot_idxqvect_firstBoxOfInBetween) {
                     idxqvect_firstBoxOfInBetween = idxqvect;
                bGot_idxqvect_firstBoxOfInBetween = true;
                qDebug() << "init idxqvect_firstBoxOfInBetween with : " << idxqvect << "(" << idxqvect_firstBoxOfInBetween << ")";
            }

            int diff_idxSegmentOwner_current_lastBoxToAddInVect =
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                    - lastBoxToAddInVect._profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

            qDebug() << "cmp SegmentOwner: vect[ " << idxqvect << "]= " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                     << " > " << "qmap[lastBoxToAddInVect]= " << lastBoxToAddInVect._profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

            if (diff_idxSegmentOwner_current_lastBoxToAddInVect > 0) {

                partAfter_whichNeedsNoUpdate.push_back(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[idxqvect]);
                qDebug() << "smaller => insert vect[ " << idxqvect << "] into partAfter_whichNeedsNoUpdate";

                if (!bGot_idxqvect_lastBoxOfInBetween) {
                         idxqvect_lastBoxOfInBetween = idxqvect-1; //can be -1
                    bGot_idxqvect_lastBoxOfInBetween = true;
                    qDebug() << "init idxqvect_lastBoxOfInBetween with : " << idxqvect-1 << "(" << idxqvect_lastBoxOfInBetween << ")";
                }
            }
        }
    }

    int partBefore_whichNeedsNoUpdate_count = partBefore_whichNeedsNoUpdate.count();
    int partAfter_whichNeedsNoUpdate_count = partAfter_whichNeedsNoUpdate.count();

    qDebug() << __FUNCTION__ << "partBefore_whichNeedsNoUpdate_count = " << partBefore_whichNeedsNoUpdate_count;
    qDebug() << __FUNCTION__ << "partAfter_whichNeedsNoUpdate_count = " << partAfter_whichNeedsNoUpdate_count;

    if (partBefore_whichNeedsNoUpdate_count == count_qvectOf_BASPWMPtr_beforeUpdate) {

        qDebug() << __FUNCTION__ << "partBefore_whichNeedsNoUpdate_count == count_qvectOf_BASPWMPtr_beforeUpdate = " << partBefore_whichNeedsNoUpdate_count;

        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();

        //insert partBefore_whichNeedsNoUpdate
        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partBefore_whichNeedsNoUpdate);

        //insert the added boxes after


        int firstQmapIndex_ofFirstBoxInAddedSet = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey() - nbAddedProfilsBoxParametersAtEndOfQMap + 1;

        int idxOnQmap = 0;
        for (auto iter = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
             iter != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
             ++iter) {
            if (idxOnQmap >= firstQmapIndex_ofFirstBoxInAddedSet) {
                _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(S_InsertionIndex_SBASPWMPtr{idxOnQmap, &(*iter)});
            }
            idxOnQmap++;
        }

        aboutQVectOrderAlongRoute_indexOfFirstChange = partBefore_whichNeedsNoUpdate_count;
        aboutQVectOrderAlongRoute_indexOfLastChange = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

        showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting case #0:");

        return(true);
    }

    if (partAfter_whichNeedsNoUpdate_count == count_qvectOf_BASPWMPtr_beforeUpdate) {

        qDebug() << __FUNCTION__ << "partAfter_whichNeedsNoUpdate_count == count_qvectOf_BASPWMPtr_beforeUpdate = " << partAfter_whichNeedsNoUpdate_count;

        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();

        //insert the added boxes before

        int firstQmapIndex_ofFirstBoxInAddedSet = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey() - nbAddedProfilsBoxParametersAtEndOfQMap + 1;

        int idxOnQmap = 0;
        for (auto iter = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
             iter != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
             ++iter) {
            if (idxOnQmap >= firstQmapIndex_ofFirstBoxInAddedSet) {
                _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(S_InsertionIndex_SBASPWMPtr{idxOnQmap, &(*iter)});
            }
            idxOnQmap++;
        }

        //insert partAfter_whichNeedsNoUpdate
        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partAfter_whichNeedsNoUpdate);

        aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
        aboutQVectOrderAlongRoute_indexOfLastChange = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

        showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting case #1:");

        return(true);
    }


    if (!bGot_idxqvect_firstBoxOfInBetween) {
              idxqvect_firstBoxOfInBetween = 0;
    }
    if (idxqvect_lastBoxOfInBetween == -1) {
        idxqvect_lastBoxOfInBetween = count_qvectOf_BASPWMPtr_beforeUpdate-1;
        bGot_idxqvect_lastBoxOfInBetween = true;
    }
    if (!bGot_idxqvect_lastBoxOfInBetween) {
              idxqvect_lastBoxOfInBetween = count_qvectOf_BASPWMPtr_beforeUpdate-1;
    }

    int sizeInBetween = idxqvect_lastBoxOfInBetween - idxqvect_firstBoxOfInBetween + 1;

    qDebug() << __FUNCTION__ << "idxqvect_firstBoxOfInBetween: " << idxqvect_firstBoxOfInBetween;
    qDebug() << __FUNCTION__ << "idxqvect_lastBoxOfInBetween: " << idxqvect_lastBoxOfInBetween;
    qDebug() << __FUNCTION__ << "sizeInBetween: " << sizeInBetween;

    if (!sizeInBetween) { //when the added boxes do not have segment owner idx present in the qvect

        qDebug() << __FUNCTION__ << "if (!sizeInBetween) {";

        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();

        //insert partBefore_whichNeedsNoUpdate
        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partBefore_whichNeedsNoUpdate);

        //insert the added boxes

        int firstQmapIndex_ofFirstBoxInAddedSet = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey() - nbAddedProfilsBoxParametersAtEndOfQMap + 1;

        int idxOnQmap = 0;
        for (auto iter = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.begin();
             iter != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end();
             ++iter) {
            if (idxOnQmap >= firstQmapIndex_ofFirstBoxInAddedSet) {
                _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(S_InsertionIndex_SBASPWMPtr{idxOnQmap, &(*iter)});
            }
            idxOnQmap++;
        }

        //insert partAfter_whichNeedsNoUpdate
        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partAfter_whichNeedsNoUpdate);

        aboutQVectOrderAlongRoute_indexOfFirstChange = partBefore_whichNeedsNoUpdate_count;
        aboutQVectOrderAlongRoute_indexOfLastChange = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

        showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting case #2:");

        return(true);
    }

    //here when the new added boxes set can not be added in one block
    //here it's still possible to have segment owner of added boxes which are not used by any boxes in the qvect
    //exemple: [0, 0, 1, 2, 4, 6, 6, 8, 9, 10]
    //and added boxes segmentOwner: <3, 5, 6, 7>
    //Hence:
    //- partBefore_whichNeedsNoUpdate  = [ 0, 0, 1, 2 ]
    //- partAfter_whichNeedsNoUpdate   = [ 8, 9, 10 ]
    //- partInBetween                  = [ 4, 6, 6 ]
    //=> <3> will be before all the partInBetween
    //=> <7> will be after all the partInBetween
    //=> <5> will be inserted but without need of center point location test as there is none boxes with this segmentOwner
    //=> <6> will be inserted with center point location test with the other boxes with the same segmentOwner
    //here when the added boxes needs to be inserted depending the center point location in the segment,
    //relative to the other boxes with the same segment owner
    //==> partInBetween will become [<3> 4 <5> 6 <6> 6 <7>]
    //    partToInsertBeforeInBetween = [<3>]
    //    partToInsertAfterInBetween  = [<7>]

    QVector<S_InsertionIndex_SBASPWMPtr> qvectPartInBetween;
    qvectPartInBetween = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.mid(idxqvect_firstBoxOfInBetween, sizeInBetween);

    qDebug() << __FUNCTION__ << "____ qvectPartInBetween content: ____";
    for(const auto& baspwmPtr: qvectPartInBetween) {
        qDebug() << __FUNCTION__ << "baspwmPtr =" << (void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }
    qDebug() << __FUNCTION__ << "_______";

    QVector<S_InsertionIndex_SBASPWMPtr> qvectPartToInsertBeforeInBetween;
    QVector<S_InsertionIndex_SBASPWMPtr> qvectPartToInsertAfterInBetween;
    QVector<S_InsertionIndex_SBASPWMPtr> qvectPartToInsertInsideInBetween;

    firstQmapIndex_ofFirstBoxInAddedSet = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.lastKey() - nbAddedProfilsBoxParametersAtEndOfQMap + 1;

    int idxInsertionInQmap = firstQmapIndex_ofFirstBoxInAddedSet;
    //separate the box to add in three parts: before, inside and after the qvectPartInBetween
    for(; iterPair_addedBox_boxId_BASPWM != _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.end(); ++iterPair_addedBox_boxId_BASPWM) {

        qDebug() << "comp " << "qmap[xxx]..._idxSegmentOwnerOfCenterPoint = " << iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                 << "with qvect[0]..._idxSegmentOwnerOfCenterPoint = " << qvectPartInBetween.at(0)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

        qDebug() << "center points qmap[xxx]: " << iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._qpfCenterPoint;
        qDebug() << "center points qvect[0] : " << qvectPartInBetween.at(0)._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;

        //if box is before the first of the qvectPartInBetween:
        if (iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._idxSegmentOwnerOfCenterPoint < qvectPartInBetween.at(0)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint) {
            qDebug() << "smaller => insert qmap[xxx] into qvectPartToInsertBeforeInBetween";
            qvectPartToInsertBeforeInBetween.push_back(S_InsertionIndex_SBASPWMPtr{idxInsertionInQmap , &(iterPair_addedBox_boxId_BASPWM.value())});
        } else {
            qDebug() << "bigger or equal";

            qDebug() << "comp " << "qmap[xxx]..._idxSegmentOwnerOfCenterPoint = " << iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                     << "with qvect[sizeInBetween-1]..._idxSegmentOwnerOfCenterPoint = " << qvectPartInBetween.at(sizeInBetween-1)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;
            qDebug() << "center points qmap[xxx]: " << iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._qpfCenterPoint;
            qDebug() << "center points qvect[sizeInBetween-1] : " << qvectPartInBetween.at(sizeInBetween-1)._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;

            //if box is after the last of the qvectPartInBetween;
            if (iterPair_addedBox_boxId_BASPWM.value()._profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                > qvectPartInBetween.at(sizeInBetween-1)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint) {
                qDebug() << "bigger => insert qmap[xxx] into qvectPartToInsertAfterInBetween";
                qvectPartToInsertAfterInBetween.push_back(S_InsertionIndex_SBASPWMPtr{idxInsertionInQmap, &(iterPair_addedBox_boxId_BASPWM.value())});
            } else {                
                //the box needs to be located somewhere inside the partInBetween
                qDebug() << "smaller or equal => insert qmap[xxx] into qvectPartToInsertInsideInBetween";
                qvectPartToInsertInsideInBetween.push_back(S_InsertionIndex_SBASPWMPtr{idxInsertionInQmap, &(iterPair_addedBox_boxId_BASPWM.value())});
            }
        }
        idxInsertionInQmap++;
    }

    //from here we do not have to use the _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements
    //the three part contains ptr on the boxes

    int qvectPartToInsertInsideInBetween_size = qvectPartToInsertInsideInBetween.size();

    qDebug() << __FUNCTION__ << "----- 5112 x__ qvectPartToInsertBeforeInBetween content:";
    qDebug() << __FUNCTION__ << "qvectPartToInsertBeforeInBetween.size = " << qvectPartToInsertBeforeInBetween.size();
    for(const auto& baspwmPtr: qvectPartToInsertBeforeInBetween) {
        qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    qDebug() << __FUNCTION__ << "---- 5119 _x_ qvectPartToInsertAfterInBetween content:";
    qDebug() << __FUNCTION__ << "qvectPartToInsertAfterInBetween.size  = " << qvectPartToInsertAfterInBetween.size();
    for(const auto& baspwmPtr: qvectPartToInsertAfterInBetween) {
        qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    qDebug() << __FUNCTION__ << "---- 5126 __x qvectPartToInsertInsideInBetween content:";
    qDebug() << __FUNCTION__ << "qvectPartToInsertInsideInBetween_size = " << qvectPartToInsertInsideInBetween_size;
    for(const auto& baspwmPtr: qvectPartToInsertInsideInBetween) {
        qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    QVector<S_InsertionIndex_SBASPWMPtr> qvectPartInBetween_updated;

    //now handle the added boxes which need to be inserted inside partInBeetwen (but not at the extremities of partInBeetwen)
    if (qvectPartToInsertInsideInBetween_size) {

        int idxBoxQvect_partToInsertInsideInBetween = 0;
        int idxBoxQvect_partInBetween = 0;

        while (1) {

            qDebug() << "?=> idxBoxQvect_partToInsertInsideInBetween < qvectPartToInsertInsideInBetween_size :  "
                     << idxBoxQvect_partToInsertInsideInBetween << " < " << qvectPartToInsertInsideInBetween_size;

            qDebug() << "?=> idxBoxQvect_partInBetween < sizeInBetween :  "
                     << idxBoxQvect_partInBetween << " < " << sizeInBetween;

            if (  (idxBoxQvect_partToInsertInsideInBetween < qvectPartToInsertInsideInBetween_size)
                &&(idxBoxQvect_partInBetween < sizeInBetween)) {

                qDebug() << "the two tests said true ";

                int idx_segmentOwner_compare_partInBetween_with_partToInsertInsideInBetween =
                qvectPartInBetween.at(idxBoxQvect_partInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                - qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;



                qDebug() << "cmp idx_segmentOwner: qvectPartInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << ")...= " << qvectPartInBetween.at(idxBoxQvect_partInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                         << " - " << "qvectPartToInsertInsideInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << ")... =" << qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                         << " = " << idx_segmentOwner_compare_partInBetween_with_partToInsertInsideInBetween;

                if (idx_segmentOwner_compare_partInBetween_with_partToInsertInsideInBetween == 0) { //same segmentOwner

                    qDebug() << "cmp said: = 0";

                    qDebug() << "qvectPartInBetween.at(" << idxBoxQvect_partInBetween << "). .._idxSegmentOwnerOfCenterPoint = "
                             <<  qvectPartInBetween.at(idxBoxQvect_partInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;

                    qDebug() << "@_routePtr = " << (void*)_routePtr;

                    S_Segment segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween;
                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween._ptA = _routePtr->getSegment( qvectPartInBetween[idxBoxQvect_partInBetween]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint)._ptA;

                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween._ptB = qvectPartInBetween.at(idxBoxQvect_partInBetween)._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;
                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween._bValid = true;
                    double distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween = segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween.length();

                    qDebug() << "distance from qvectPartInBetween.at(" << idxBoxQvect_partInBetween << ")._ptA to qvectPartInBetween.at(" << idxBoxQvect_partInBetween << " ).centerPoint "
                             << " = " << distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween;

                    S_Segment segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween;
                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween._ptA = _routePtr->getSegment(qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween)._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint)._ptA;
                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween._ptB = qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween)._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;
                    segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween._bValid = true;
                    double distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween = segment_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween.length();

                    qDebug() << "distance from qvectPartToInsertInsideInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << ")._ptA to qvectPartToInsertInsideInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << " ).centerPoint "
                             << " = " << distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween;

                    if (distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partInBetween <= distance_fromAssociatedFirstSegmentPoint_ToCenterBox_partToInsertInsideInBetween) {

                        qDebug() << "distance_...partInBetween <= distance_...partToInsertInsideInBetween";
                        qDebug() << "insert qvectPartInBetween.at(" << idxBoxQvect_partInBetween << ") into qvectPartInBetween_updated";

                        qvectPartInBetween_updated.push_back(qvectPartInBetween.at(idxBoxQvect_partInBetween));

                        idxBoxQvect_partInBetween++;
                        qDebug() << "next idxBoxQvect_partInBetween => now is" << idxBoxQvect_partInBetween;

                    } else {

                        qDebug() << "distance_...partInBetween > distance_...partToInsertInsideInBetween";
                        qDebug() << "insert qvectPartToInsertInsideInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << ") into qvectPartInBetween_updated";

                        qvectPartInBetween_updated.push_back(qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween));
                        idxBoxQvect_partToInsertInsideInBetween++;

                        qDebug() << "next idxBoxQvect_partToInsertInsideInBetween => now is" << idxBoxQvect_partToInsertInsideInBetween;

                    }

                } else {

                    qDebug() << "cmp said: != 0";

                    if (idx_segmentOwner_compare_partInBetween_with_partToInsertInsideInBetween > 0) {  //box from partInBetween has segment owner after box from partToInsertInsideInBetween

                        qDebug() << "segment owners: qvectPartInBetween > qvectPartToInsertInsideInBetween";
                        qDebug() << "insert qvectPartToInsertInsideInBetween.at(" << idxBoxQvect_partToInsertInsideInBetween << "into qvectPartInBetween_updated";

                        qvectPartInBetween_updated.push_back(qvectPartToInsertInsideInBetween.at(idxBoxQvect_partToInsertInsideInBetween));
                        idxBoxQvect_partToInsertInsideInBetween++;

                        qDebug() << "next idxBoxQvect_partToInsertInsideInBetween => now is" << idxBoxQvect_partToInsertInsideInBetween;

                    } else { //box from partInBetween has segment owner before box from partToInsertInsideInBetween

                        qDebug() << "segment owners: qvectPartInBetween < qvectPartToInsertInsideInBetween";
                        qDebug() << "insert qvectPartInBetween.at(" << idxBoxQvect_partInBetween << "into qvectPartInBetween_updated";

                        qvectPartInBetween_updated.push_back(qvectPartInBetween.at(idxBoxQvect_partInBetween));

                        idxBoxQvect_partInBetween++;

                        qDebug() << "next idxBoxQvect_partInBetween => now is" << idxBoxQvect_partInBetween;

                    }
                }

            } else {

                qDebug() << "the two tests are not true";

                //remaining some boxes to insert from partToInsertInsideInBetween
                if (  (idxBoxQvect_partToInsertInsideInBetween < qvectPartToInsertInsideInBetween_size)
                    &&(idxBoxQvect_partInBetween >= sizeInBetween)) {

                    qDebug() << "idxBoxQvect_partToInsertInsideInBetween < qvectPartToInsertInsideInBetween_size :" << idxBoxQvect_partToInsertInsideInBetween << " < " << qvectPartToInsertInsideInBetween_size;
                    qDebug() << "idxBoxQvect_partInBetween >= sizeInBetween :" << idxBoxQvect_partInBetween << " >= " << sizeInBetween;

                    if (idxBoxQvect_partToInsertInsideInBetween == 0) {
                        qDebug() << "qvectPartInBetween_updated.append(qvectPartToInsertInsideInBetween)";
                        qvectPartInBetween_updated.append(qvectPartToInsertInsideInBetween);
                    } else {
                        qDebug() << "qvectPartInBetween_updated.append(qvectPartToInsertInsideInBetween.mid("
                                 << idxBoxQvect_partToInsertInsideInBetween << ", "
                                 << qvectPartToInsertInsideInBetween_size - idxBoxQvect_partToInsertInsideInBetween;

                        qvectPartInBetween_updated.append(qvectPartToInsertInsideInBetween.mid(idxBoxQvect_partToInsertInsideInBetween,
                                                                                               qvectPartToInsertInsideInBetween_size - idxBoxQvect_partToInsertInsideInBetween));
                    }
                    break;
                }

               //remaining some boxes to insert from partInBetween
                if (  (idxBoxQvect_partToInsertInsideInBetween >= qvectPartToInsertInsideInBetween_size)
                    &&(idxBoxQvect_partInBetween < sizeInBetween)) {

                    qDebug() << "idxBoxQvect_partToInsertInsideInBetween >= qvectPartToInsertInsideInBetween_size :" << idxBoxQvect_partToInsertInsideInBetween << " >= " << qvectPartToInsertInsideInBetween_size;
                    qDebug() << "idxBoxQvect_partInBetween < sizeInBetween :" << idxBoxQvect_partInBetween << " < " << sizeInBetween;

                    if (idxBoxQvect_partInBetween == 0) {
                        qDebug() << "qvectPartInBetween_updated.append(qvectPartInBetween)";
                        qvectPartInBetween_updated.append(qvectPartInBetween);
                    } else {
                        qDebug() << "qvectPartInBetween_updated.append(qvectPartInBetween.mid("
                                 << idxBoxQvect_partInBetween << ", "
                                 << sizeInBetween - idxBoxQvect_partInBetween;
                        qvectPartInBetween_updated.append(qvectPartInBetween.mid(idxBoxQvect_partInBetween,
                                                                                 sizeInBetween - idxBoxQvect_partInBetween));
                    }
                    break;
                }

                //other case:
                break;
            }
        }
    } else {
        qDebug() << "------ 5030 qvectPartToInsertInsideInBetween_size is zero";

        if (sizeInBetween) {

            //for (int idx = idxqvect_firstBoxOfInBetween; idx <= idxqvect_lastBoxOfInBetween; idx++) {
            //qvectPartInBetween_updated.push_back()

            qvectPartInBetween_updated = qvectPartInBetween;

        }
    }

    qDebug() << __FUNCTION__ << "------------ 5304";

    qDebug() << __FUNCTION__ << "__x qvectPartToInsertBeforeInBetween content:";
    for(const auto& baspwmPtr: qvectPartToInsertBeforeInBetween) {
        //qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    qDebug() << __FUNCTION__ << "------------ 5313";

    qDebug() << __FUNCTION__ << "__x qvectPartInBetween_updated content:";
    for(const auto& baspwmPtr: qvectPartInBetween_updated) {
        //qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    qDebug() << __FUNCTION__ << "------------ 5322";

    qDebug() << __FUNCTION__ << "__x qvectPartToInsertAfterInBetween content:";
    for(const auto& baspwmPtr: qvectPartToInsertAfterInBetween) {
        //qDebug() << __FUNCTION__ << "@iter:" <<(void*)baspwmPtr._sBASPWMPtr;
        baspwmPtr._sBASPWMPtr->_profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly(); //showContent_noMeasurements();
    }

    qDebug() << __FUNCTION__ << "------------ 5331";

    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partBefore_whichNeedsNoUpdate);
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(qvectPartToInsertBeforeInBetween);
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(qvectPartInBetween_updated);
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(qvectPartToInsertAfterInBetween);
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(partAfter_whichNeedsNoUpdate);

    aboutQVectOrderAlongRoute_indexOfFirstChange = partBefore_whichNeedsNoUpdate_count;
    aboutQVectOrderAlongRoute_indexOfLastChange = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()-1;

    showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting case #3:");

    return(true);
}



void ComputationCore::showContent_qmap_qvectSyncWithQMap(const QString& strMsg) {
    qDebug() << __FUNCTION__ << ": " << strMsg;
    qDebug() << __FUNCTION__ << "---| qmap content |--- :";
    for(const auto& iter: _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements) {
        qDebug() << __FUNCTION__ << "@iter:" <<(void*)&iter;
        iter._profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();
    }
    qDebug() << __FUNCTION__ << "---[ qvect content ]--- :";
    for(const auto& iter: _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap) {
        qDebug() << __FUNCTION__ << "@ptr:" << (void*)iter._sBASPWMPtr;
        iter.showContent_insertionIdx_and_boxPointIdxSegmentOnly();
        //iter._sBASPWMPtr->_profilsBoxParameters.showContent_pointIdxSegmentOnly();
    }
    qDebug() << __FUNCTION__ << "_- end -_";
}


bool ComputationCore::update_qvectOf_BASPWMPtr_orderedAlongRoute_afterOneBoxMove(int vectBoxId_toReorder, int& newVectBoxId_afterReorderAlongTheRoute) {

    //entering with the qvect still on the old order. ie the vectBoxId_toReorder is still in the qvect at its location before its center point update
    newVectBoxId_afterReorderAlongTheRoute = -1;

    if (!vectBoxIdValid(vectBoxId_toReorder)) {
        qDebug() << __FUNCTION__ << "if (!vectBoxIdValid(vectBoxId_toReorder)) {";
        return(false);
    }

    int qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size) {
        qDebug() << __FUNCTION__ << "if (!qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size) {";
        return(false);
    }

    //only one box in the vect
    if (qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size == 1) {
        newVectBoxId_afterReorderAlongTheRoute = 0;
        qDebug() << __FUNCTION__ << "only one box in the vect";
        return(true);
    }

    S_InsertionIndex_SBASPWMPtr sInsertionIndex_sBASPWMPtr_ofBoxToMove = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectBoxId_toReorder);
    if (!sInsertionIndex_sBASPWMPtr_ofBoxToMove._sBASPWMPtr) {
        qDebug() << __FUNCTION__ << "if (!sInsertionIndex_SBASPWMPtrsBASPWMPtr_ofBoxToMove._sBASPWMPtr)";
        return(false);
    }
    qDebug() << __FUNCTION__ << "vectBoxId_toReorder:" << vectBoxId_toReorder;
    qDebug() << __FUNCTION__ << "box (to move):";


    S_Segment segment_fromAssociatedFirstSegmentPoint_toCenterBoxToMove;
    segment_fromAssociatedFirstSegmentPoint_toCenterBoxToMove._ptA = _routePtr->getSegment(sInsertionIndex_sBASPWMPtr_ofBoxToMove._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint)._ptA;
    segment_fromAssociatedFirstSegmentPoint_toCenterBoxToMove._ptB = sInsertionIndex_sBASPWMPtr_ofBoxToMove._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;
    segment_fromAssociatedFirstSegmentPoint_toCenterBoxToMove._bValid = true;
    double distance_fromAssociatedFirstSegmentPoint_ToCenterBoxToMove = segment_fromAssociatedFirstSegmentPoint_toCenterBoxToMove.length();

    //int firstIdx_of_partBeforeBoxToMove = -1;
    int size_of_partBeforeBoxToMove = vectBoxId_toReorder;
    int firstIdx_of_partAfterBoxToMove = vectBoxId_toReorder + 1;
    int size_afterBoxToMove = qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size - firstIdx_of_partAfterBoxToMove;

    qDebug() << __FUNCTION__ << "size_of_partBeforeBoxToMove:" << size_of_partBeforeBoxToMove;
    qDebug() << __FUNCTION__ << "firstIdx_of_partAfterBoxToMove:" << firstIdx_of_partAfterBoxToMove;
    qDebug() << __FUNCTION__ << "size_afterBoxToMove:" << size_afterBoxToMove;

    QVector<S_InsertionIndex_SBASPWMPtr> qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate(std::move(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap));//@LP move
    qDebug() << "after move (c++ move semantics, not about the box along the route):"
             << "_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()=" << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.clear();

    int vectIdxOfInsertion = 0;
    bool bBoxToMoveWasInserted = false;

    for (int idxqvect = 0; idxqvect < qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_size; idxqvect++) {

        qDebug() << __FUNCTION__ << "| loop: idxqvect = " << idxqvect;
        qDebug() << __FUNCTION__ << "| (vectIdxOfInsertion = " << vectIdxOfInsertion;

        //boxes which are not the one we want to relocate in the vect:
        if (idxqvect != vectBoxId_toReorder) {

            qDebug() << __FUNCTION__ << "idxqvect != vectBoxId_toReorder : " << idxqvect << "!=" << vectBoxId_toReorder;

            S_BoxAndStackedProfilWithMeasurements* sBASPWMPtr = qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.at(idxqvect)._sBASPWMPtr;
            if (!sBASPWMPtr) {
                qDebug() << __FUNCTION__ << __LINE__ << " if (!sBASPWMPtr) {";
                continue; //@@##LP
            }

            int diffOfIdxSegmentOwner = sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint - sInsertionIndex_sBASPWMPtr_ofBoxToMove._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint;
            qDebug() << __FUNCTION__ << "diffOfIdxSegmentOwner= " << diffOfIdxSegmentOwner;

            //any boxes with a different segment owner
            if (diffOfIdxSegmentOwner) {

                //IdxSegmentOwner of current box < box to insert
                if (diffOfIdxSegmentOwner < 0 ) {

                    qDebug() << __FUNCTION__ << "diffOfIdxSegmentOwner < 0";

                    qDebug() << __FUNCTION__ << "insert current";
                    //simply insert the current
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.at(idxqvect));
                    vectIdxOfInsertion++;

                } else { //diffOfIdxSegmentOwner > 0 (IdxSegmentOwner of current box > box to insert)

                    if (!bBoxToMoveWasInserted) {

                        qDebug() << __FUNCTION__ << "diffOfIdxSegmentOwner > 0";

                        //insert the box to move
                        qDebug() << __FUNCTION__ << "insert the box to move";
                        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(sInsertionIndex_sBASPWMPtr_ofBoxToMove);
                        newVectBoxId_afterReorderAlongTheRoute = vectIdxOfInsertion;
                        vectIdxOfInsertion++;

                        bBoxToMoveWasInserted = true;
                    }

                    //and push all the remainings, after:
                    if (idxqvect < vectBoxId_toReorder) {

                        qDebug() << __FUNCTION__ << "insert the part before";
                        qDebug() << __FUNCTION__ << "(remaining from current to just before vectBoxId_toReorder)";

                        qDebug() << __FUNCTION__ << "mid( " << idxqvect << ", " << size_of_partBeforeBoxToMove - idxqvect << " )";
                        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                    qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                    mid(idxqvect, size_of_partBeforeBoxToMove - idxqvect));

                        qDebug() << __FUNCTION__ << " => insert the part after";
                        qDebug() << __FUNCTION__ << "(remaining from just after vectBoxId_toReorder to the end";
                        if (size_afterBoxToMove) {
                            qDebug() << __FUNCTION__ << "mid( " << firstIdx_of_partAfterBoxToMove << ", " << size_afterBoxToMove << " )";
                            _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                        qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                        mid(firstIdx_of_partAfterBoxToMove, size_afterBoxToMove));
                        } else {
                            qDebug() << __FUNCTION__ << " => size_afterBoxToMove is zero";
                        }

                    } else {
                        qDebug() << __FUNCTION__ << " => all the boxes before vectBoxId_toReorder already inserted:";
                        qDebug() << __FUNCTION__ << "insert the remaining after from idxqvect to the end";
                        int sizePartAfterToInsert = size_afterBoxToMove - (idxqvect - firstIdx_of_partAfterBoxToMove);
                        if (sizePartAfterToInsert > 0) {
                            qDebug() << __FUNCTION__ << " => insert remaining from idxqvect to the end";
                            qDebug() << __FUNCTION__ << "mid( " << idxqvect << ", " << sizePartAfterToInsert << " )";
                            _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                        qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                        mid(idxqvect, sizePartAfterToInsert));
                        } else {
                           qDebug() << __FUNCTION__ << " => size_afterBoxToMove - idxqvect  is zero";
                        }
                    }
                    break;
                }

            } else {

                qDebug() << __FUNCTION__ << "same segment";

                //box not inserted in the same segment. Needs distance test.
                //boxes wich are in the same segment than the box to relocate
                S_Segment segment_fromAssociatedFirstSegmentPoint_toCenterBox;
                segment_fromAssociatedFirstSegmentPoint_toCenterBox._ptA = _routePtr->getSegment(sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint)._ptA;
                segment_fromAssociatedFirstSegmentPoint_toCenterBox._ptB = sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint;
                segment_fromAssociatedFirstSegmentPoint_toCenterBox._bValid = true;
                double distance_fromAssociatedFirstSegmentPoint_toCenterBox = segment_fromAssociatedFirstSegmentPoint_toCenterBox.length();

                qDebug() << __FUNCTION__ << "compare distance: distance_fromAssociatedFirstSegmentPoint_ToCenterBoxToMove = " << distance_fromAssociatedFirstSegmentPoint_ToCenterBoxToMove;
                qDebug() << __FUNCTION__ << "compare distance: distance_fromAssociatedFirstSegmentPoint_toCenterBox = " << distance_fromAssociatedFirstSegmentPoint_toCenterBox;
                qDebug() << __FUNCTION__ << "current box:";
                //qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.at(idxqvect).showContent_insertionIdx_and_boxPointIdxSegmentOnly();

                //box to relocate more closer to PtA than the current box
                if (distance_fromAssociatedFirstSegmentPoint_ToCenterBoxToMove < distance_fromAssociatedFirstSegmentPoint_toCenterBox) {

                    qDebug() << __FUNCTION__ << "distance_fromAssociatedFirstSegmentPoint_ToCenterBoxToMove < distance_fromAssociatedFirstSegmentPoint_toCenterBox";

                    //push the box to relocate
                    qDebug() << __FUNCTION__ << "push the box to relocate";
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(sInsertionIndex_sBASPWMPtr_ofBoxToMove);
                    newVectBoxId_afterReorderAlongTheRoute = vectIdxOfInsertion;
                    vectIdxOfInsertion++;

                    bBoxToMoveWasInserted = true;

                    //and push all the remainings, after:
                    if (idxqvect < vectBoxId_toReorder) {

                        qDebug() << __FUNCTION__ << "insert the part before";
                        qDebug() << __FUNCTION__ << "(remaining from current to just before vectBoxId_toReorder)";

                        qDebug() << __FUNCTION__ << "mid( " << idxqvect << ", " << size_of_partBeforeBoxToMove - idxqvect << " )";
                        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                    qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                    mid(idxqvect, size_of_partBeforeBoxToMove - idxqvect));

                        qDebug() << __FUNCTION__ << " => insert the part after";
                        qDebug() << __FUNCTION__ << "(remaining from just after vectBoxId_toReorder to the end";
                        if (size_afterBoxToMove) {
                            qDebug() << __FUNCTION__ << "mid( " << firstIdx_of_partAfterBoxToMove << ", " << size_afterBoxToMove << " )";
                            _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                        qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                        mid(firstIdx_of_partAfterBoxToMove, size_afterBoxToMove));
                        } else {
                            qDebug() << __FUNCTION__ << " => size_afterBoxToMove is zero";
                        }

                    } else {
                        qDebug() << __FUNCTION__ << " => all the boxes before vectBoxId_toReorder already inserted:";
                        qDebug() << __FUNCTION__ << "insert the remaining after from idxqvect to the end";
                        int sizePartAfterToInsert = size_afterBoxToMove - (idxqvect - firstIdx_of_partAfterBoxToMove);
                        if (sizePartAfterToInsert > 0) {
                            qDebug() << __FUNCTION__ << " => insert remaining from idxqvect to the end";
                            qDebug() << __FUNCTION__ << "mid( " << idxqvect << ", " << sizePartAfterToInsert << " )";
                            _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.append(
                                        qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.
                                        mid(idxqvect, sizePartAfterToInsert));
                        } else {
                           qDebug() << __FUNCTION__ << " => size_afterBoxToMove - idxqvect  is zero";
                        }
                    }
                    break;

                } else { //current box more closer to PtA  than the box to relocate

                    qDebug() << __FUNCTION__ << "push current box";
                    _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap_beforeUpdate.at(idxqvect));
                    vectIdxOfInsertion++;
                }
            }

        } else {
            qDebug() << __FUNCTION__ << "idxqvect == vectBoxId_toReorder: " << idxqvect << "==" << vectBoxId_toReorder;
            qDebug() << __FUNCTION__ <<  "<=> on the box to move => do nothing";
        }

        /*qDebug() << __FUNCTION__ <<  "-*- after loop -*-";
        showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "");
        qDebug() << __FUNCTION__ <<  "------ *** ------";*/

    }

    if (!bBoxToMoveWasInserted) {

        //if some boxes were inserted and located after the box to move => error
        if (!_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.isEmpty()) {
            if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.last()._sBASPWMPtr == nullptr) {
                return(false);
            }
            if (_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.last()._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint
                >  sInsertionIndex_sBASPWMPtr_ofBoxToMove._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint) {
                return(false);
            }
        }

        qDebug() << __FUNCTION__ << "push the box to relocate (at the end of the qvect)";
        _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.push_back(sInsertionIndex_sBASPWMPtr_ofBoxToMove);
        newVectBoxId_afterReorderAlongTheRoute = vectIdxOfInsertion;

        bBoxToMoveWasInserted = true;
    }

    showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + " exiting with:");

    return(true);
}

//return false in case of impossible not reversed restoration
bool ComputationCore::adjustMeasurementsDueToProgressDirectionChanged(bool& bRestoredDueToDevError, bool& bInternalErrorWhenComputinglRSidesLinearRegressionsModels) {

    //handle unreversed case which should never happpen, but we want to avoid any unsync content.
    bRestoredDueToDevError = false;
    bInternalErrorWhenComputinglRSidesLinearRegressionsModels = false;

    bool bNeedRestoreNotReversedState_dueToInternalError = false;

    int nbBox = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!nbBox) {
        return(true);
    }

    //reverse the measures vect and x limits for any computed box
    //when the other boxes will be computed, the new orientation will be used
    int iBoxId = 0;
    for (iBoxId = 0; iBoxId < nbBox; iBoxId++) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[iBoxId]._sBASPWMPtr;

        if (!SBASPWSPtr) {
            bNeedRestoreNotReversedState_dueToInternalError = true;
            break;
        }
        qDebug() << __FUNCTION__ << __LINE__
                 << " iBoxId = " << iBoxId
                 << "; _locationFromZeroOfFirstInsertedValueInStackedProfil._value: "
                 << SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value;

        if (!reverseBoxMeasurements(SBASPWSPtr)) {
            bNeedRestoreNotReversedState_dueToInternalError = true;
            break;
        }
    }

    //succeed to reverse:
    if (!bNeedRestoreNotReversedState_dueToInternalError) {
        //compute linear regression models:
        bool bReport = compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
        if (!bReport) {
            bInternalErrorWhenComputinglRSidesLinearRegressionsModels = true;

            //@LP here we return false with:
            //- bRestoredDueToDevError: false
            //- bInternalErrorWhenComputinglRSidesLinearRegressionsModels: true

            return(false);
        }

        //here is is the nominal case which should always happen:
        //@LP here we return true with:
        //- bRestoredDueToDevError: false
        //- bInternalErrorWhenComputinglRSidesLinearRegressionsModels: false

        return(true);
    }

    //here when bNeedRestoreNotReversedState_dueToInternalError is true
    //try to restore the inital state
    //- bRestoredDueToDevError: false
    //- bInternalErrorWhenComputinglRSidesLinearRegressionsModels: false
    //- bNeedRestoreNotReversedState_dueToInternalError: true

    //try from boxId #0 to iBoxId-1 (iBoxId: the box where the error occured above)
    for (int iRestoreBoxId = 0; iRestoreBoxId < iBoxId; iRestoreBoxId++) {

        S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[iRestoreBoxId]._sBASPWMPtr;

        if (!SBASPWSPtr) {
            //should never happen as already checked in the loop above
            return(false);
        }
        qDebug() << __FUNCTION__ << __LINE__
                 << " iRestoreBoxId = " << iRestoreBoxId
                 << "; _locationFromZeroOfFirstInsertedValueInStackedProfil._value: "
                 << SBASPWSPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value;

        if (!reverseBoxMeasurements(SBASPWSPtr)) {
            //should never happend as already checked in the loop above
            return(false);
        }
    }

    bool bReport = compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
    if (!bReport) {
        bInternalErrorWhenComputinglRSidesLinearRegressionsModels = true;
        return(false);
    }

    bRestoredDueToDevError = true;
    return(true);
}

bool ComputationCore::reverseBoxMeasurements(S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr) {

    if (!SBASPWSPtr) {
        return(false);
    }

    //allows to have different computationMethod results values storable at the same time
    for (int i_eCA = e_CA_Perp; i_eCA <= e_CA_deltaZ; i_eCA++) {
        if (_qvectb_componentsToCompute_Perp_Parall_DeltaZ[i_eCA]) {
            e_MeanMedianIndexAccess meanMedianIndexAccess = static_cast<e_MeanMedianIndexAccess>(_vect_eComputationMethod_Perp_Parall_DeltaZ[i_eCA]);

            //reverse linear regression model x limits and x0
            //we try this part before the measures vect, because in case of internal dev error about x limits and x0,
            //the measures vect does not need to be reversed a second time (restoring the box content)
            S_MeasureIndex SMeasureIndex_mainMeasure {true};
            S_LeftRightSides_linearRegressionModel* SLRSides_linearRegrModelPtr =
                    (SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[i_eCA]).getPtrOn_LRSidesLinearRegressionModel(SMeasureIndex_mainMeasure);
            if (!SLRSides_linearRegrModelPtr) {
                //internal error
                return(false);
            }
            SLRSides_linearRegrModelPtr->clearResultsAndReverseSign();



            //reverse measures vect
            if (SBASPWSPtr->_bNeedsToBeRecomputedFromImages) {
                qDebug() << __FUNCTION__ << "do not reverse measures vect due to not feed from images";

            } else {

                QVector<U_MeanMedian_computedValuesWithValidityFlag> qvect_adjustedProfilDueToProgressDirectionChanged;

                QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvectRef_src = (SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[i_eCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess]);
                int sizeQvect_aboutMeanMedian = qvectRef_src.size();

                U_MeanMedian_computedValuesWithValidityFlag anonymMajorMinorWithValidityFlag;// is {.0,.0,false} zero values and the invalid flag set
                qvect_adjustedProfilDueToProgressDirectionChanged.fill(anonymMajorMinorWithValidityFlag, sizeQvect_aboutMeanMedian); //fill with invalid

                int idxSrc = 0;
                int idxDest = sizeQvect_aboutMeanMedian-1;

                if (i_eCA < e_CA_deltaZ) {
                    for (; idxSrc < sizeQvect_aboutMeanMedian; idxSrc++) {

                        //reverse profil along axis and reverse values due as the equivalent to a reverse projection axis directions change
                        qvect_adjustedProfilDueToProgressDirectionChanged[idxDest]._anonymMajorMinorWithValidityFlag._bValuesValid             =   qvectRef_src[idxSrc]._anonymMajorMinorWithValidityFlag._bValuesValid;
                        qvect_adjustedProfilDueToProgressDirectionChanged[idxDest]._anonymMajorMinorWithValidityFlag._major_centralValue       = - qvectRef_src[idxSrc]._anonymMajorMinorWithValidityFlag._major_centralValue;
                        qvect_adjustedProfilDueToProgressDirectionChanged[idxDest]._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue = /*-*/ qvectRef_src[idxSrc]._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;

                        idxDest--;
                    }
                } else {
                    for (; idxSrc < sizeQvect_aboutMeanMedian; idxSrc++) {
                        //reverse profil along x axis. No projection for one component value. Hence, no projection axis direction change
                        qvect_adjustedProfilDueToProgressDirectionChanged[idxDest]._anonymMajorMinorWithValidityFlag = qvectRef_src[idxSrc]._anonymMajorMinorWithValidityFlag;

                        idxDest--;
                    }
                }

                //update profil data curve content:
                (SBASPWSPtr->_qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements[i_eCA]._qvectqvect_aboutMeanMedian[meanMedianIndexAccess]) = qvect_adjustedProfilDueToProgressDirectionChanged;
            }
        }
    }
    return(true);
}

