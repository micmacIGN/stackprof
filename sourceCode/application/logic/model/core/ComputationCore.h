#ifndef COMPUTATIONCORE_H
#define COMPUTATIONCORE_H

#include "route.h"

#include <QVector>

#include <QHash>

//use IlmBase vector operations, which comes with openerx (openerx used by oiio)

#include "ImathVec.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "ComputationCore_structures.h"

#include "../../io/PixelPicker.h"

#include <QJsonObject>

#include "valueDequantization.hpp"

#include "exportResult.h"

struct S_InsertionIndex_SBASPWMPtr {
    int _insertionIndex;
    S_BoxAndStackedProfilWithMeasurements* _sBASPWMPtr;
    S_InsertionIndex_SBASPWMPtr();
    explicit S_InsertionIndex_SBASPWMPtr(int insertionIndex, S_BoxAndStackedProfilWithMeasurements* sBASPWMPtr);
    void showContent_insertionIdx_and_boxPointIdxSegmentOnly() const;
};

class ComputationCore {

public:

    ComputationCore();

    //@LP genereric way to inform the outside, virtuals method in ComputationCore
    virtual void stepProgressionForOutside(int v)= 0;
    // virtual void stepProgressionForOutside_terminated()= 0;
    //You have to inherit from ComputationCore to do something with stepProgressionForOutside(int v)
    //and stepProgressionForOutside_terminated.
    //This is designed like that to let ComputationCore without QObject inheritance
    //Hence, typically usage is: ComputationCore_inheritQATableModel inherit from ComputationCore.
    //and ComputationCore_inheritQATableModel will emit a signal about the value v in informOutside

    virtual ~ComputationCore();

    void showContent_qmap_qvectSyncWithQMap(const QString& strMsg);

    bool __forUnitTestOnly__set_layers_components_end_mainComputationMode(
        const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ,
        const QVector<bool>& qvectb_componentsToCompute_Perp_Parall_DeltaZ,
        e_mainComputationMode e_mainComputationMode);

    void clear();

    bool removeSectionOfBoxes(int aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                              int aboutQVectOrderAlongRoute_indexOfLastToRemove);

    bool setDefaultComputationModeAndParameters_ifNotSet();

    bool setProgressDirection(e_ProfilOrientation eProfilOrientation, bool& bReverseDone, bool& bRejected_alreadyOnThisOrientation);

    e_ProfilOrientation getProgressDirection();  
    void setDefaultProgressDirection();


    void setRoutePtr(Route *routePtr);

    bool add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
            const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
            bool bBoxesDirectionOrderIsFirstPointToLastPoint,
            int& vectIdxOfFirstInsertedBox,
            int& keyMapOfFirstInsertedBox,
            int& aboutQVectOrderAlongRoute_indexOfFirstChange,
            int& aboutQVectOrderAlongRoute_indexOfLastChange,
            bool &bCriticalErrorOccured);

    const QMap<int, S_BoxAndStackedProfilWithMeasurements>& getConstRefMap_insertionBoxId_BoxAndStackedProfilWithMeasurements();


    bool get_ptrStackedProfilWithMeasurements_fromBoxIdAndComponent(int boxId, e_ComponentsAccess componentAccess, S_StackedProfilWithMeasurements*& ptrStackedProfilWithMeasurements);

    bool getMapBoxId_fromIndexBoxInVectOrderAlongRoute(int vectBoxId, int &mapBoxId);
    bool getIndexBoxInVectOrderAlongRoute_fromMapBoxId(int  mapBoxId, int &vectBoxId);


    const QVector<QPointF>& getDevDebug_centerBox()             { return _qvectQPointF_devdebug_centerBox; }
    const QVector<QPointF>& getDevDebug_firstProfil_centerBox() { return _qvectQPointF_devdebug_firstProfil_centerBox; }
    const QVector<QPointF>& getDevDebug_firstPointAlongRoute()  { return _qvectQPointF_devdebug_firstPointAlongRoute; }
    const QVector<QPointF>& getDevDebug_testedPointsToFindNextAvailablePointAlongRoute() { return _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute; }

    const QVector<QPointF>& getDevDebug_qvectQPointF_devdebug_firstPointEverStackedProfil() { return _qvectQPointF_devdebug_firstPointEverStackedProfil; }
    const QVector<QPointF>& getDevDebug_qvectQPointF_devdebug_lastPoinOfFirstparallelelineStackedProfil() { return _qvectQPointF_devdebug_lastPoinOfFirstparallelelineStackedProfil; }
    const QVector<QPointF>& getDevDebug_qvectQPointF_devdebug_inBetweenFirstAndLastFirstparallelelineStackedProfil() { return _qvectQPointF_devdebug_inBetweenFirstAndLastFirstparallelelineStackedProfil; }

    const QVector<QPointF>& getDevDebug_qvectQPointF_devdebug_firstPointsOfParalleles() { return _qvectQPointF_devdebug_firstPointsOfParalleles; }
    const QVector<QPointF>& getDevDebug_qvectQPointF_devdebug_lastPointsOfParalleles() { return _qvectQPointF_devdebug_lastPointsOfParalleles; }


    void setInputFilesAndCorrelationScoreFiles(const S_InputFiles& inputFiles);

    bool setComputationParameters(const S_ComputationParameters& computationParameters);
    S_ComputationParameters getComputationParameters() const;

    //@#LP check useless or not:
    void setMainComputationMode(e_mainComputationMode mainComputationMode);

    //@#LP check useless or not:
    void set_baseComputationMethod(e_BaseComputationMethod baseComputationMethod);

    e_mainComputationMode getMainComputationMode();
    vector<e_ComputationMethod> getVect_computationMethods();
    e_ComputationMethod get_computationMethod(int ie_CA);


    void set_CorrelationScoreMapParameters_for(e_LayersAcces eLa_correlScoreLay, const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters);
    S_CorrelationScoreMapParameters get_CorrelationScoreMapParameters_for(e_LayersAcces eLa_correlScoreLay) const;

    void set_vectDequantizationStepPtr(vector<ValueDequantization>* vectValueDequantization_PX1_PX2_deltaZ);


    void set_pixelExtractionMethod(e_PixelExtractionMethod pixelExtractionMethod);

    bool compute_stakedProfilsAlongRoute();


    //overloaded (and called) by inheritQATableModel
    bool compute_leftRightSidesLinearRegressionsModel_forBoxId(int boxId,
                                                               const S_MeasureIndex& measureIndex);

    bool compute_leftRightSidesLinearRegressionsModels_forAllBoxes();

    bool setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel();
    bool setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ);


    bool setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(int boxId, const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ);


    bool get_locationFromZeroOfFirstInsertedValueInStackedProfil(int boxId, double& locationFromZeroOfFirstInsertedValue);


    void clearComputedStakedProfilsContent(bool bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages);
    bool isComputationRequiered_dueToAtLeastOneBoxNeedsToBeRecomputedFromImages();

    bool computationParameters_toQJsonObject(QJsonObject& qjsonObj) const;

    bool computationMainMode_fromQJsonObject(const QJsonObject& qJsonObj, QString &strMsgError);

    bool computationParameters_fromQJsonObject(const QJsonObject& qjsonObj,
                                               const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ,
                                               const QVector<bool>& qvectb_correlScoreMapToUseForComputationFlags_PX1PX2_PX1PX2_DeltaZ,
                                               bool& bContainsComputationParametersSection,
                                               bool& bComputationParametersSectionValidAndLoaded,
                                               QString &strMsgError);

    bool stackedProfilBoxes_toQJsonArray_forProjectWrite(QJsonArray& qjsonArray_stackedProfilBoxes, QString& strDetailsAboutFail_projectWrite) const ;
    bool stackedProfilWithMeasurements_withoutLinearRegresionModelResult_toQJsonArray(QJsonArray& qjsonArray_stackedProfilBoxesWithMeasurements,
                                                                                      QString& strDetailsAboutFail_projectWrite) const;

    bool stackedProfilWithMeasurements_withLinearRegresionModelResult_toQJsonArray(QJsonArray& qjsonArray_stackedProfilBoxesWithMeasurements,
                                                                                   bool bReverseOrderInQJsonArray,
                                                                                   const S_ExportSettings &sExportSettings,
                                                                                   bool bAddOneToBoxIds,
                                                                                   QString& strDetailsAboutFail_projectWrite) const;

    bool stackedProfilBoxes_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfilBoxes,
                                           const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                           QString& strDetailsAboutFail_projectLoad);

    //feed existing BoxAndStackedProfilWithMeasurements in _qlnklistBoxAndStackedProfilWithMeasurements with stackedProfil parameters
    bool feed_stackedProfil_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfil,
                                           const QVector<bool>& qvectb_layersToCompute,
                                           QString& strDetailsAboutFail_projectLoad);


    //to ascii:
    bool stackedProfilLinearRegressionModelResult_forBoxId_andComponent_toASciiStringlist(
            int vectBoxId, e_ComponentsAccess eComponentsAccess,
            bool bIfWarnFlagByUser_setLinearRegressionData_asEmpty,
            QStringList &strList_data) const;

    bool stackedProfilLinearRegressionParametersWithoutResult_forBoxId_andComponent_toASciiStringlist(int vectBoxId, e_ComponentsAccess eComponentsAccess, QStringList &strList_data) const;


    int getBoxCount() const;

    int getSecondaryMeasuresCount(int boxId) const;

    QVector<bool> get_qvectLayersInUse() const;
    QVector<bool> get_qvectComputedComponents() const;


    bool setcorrelScoreMapToUseForComp_checkInputFilesAccordingToMainComputationMode_andSetComputationLayersAndComponentFlags(const QVector<bool>& qvectb_correlScoreMapToUseForComp);

    bool initPixelPickers();

    bool compute_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode(
        const QVector<bool>& qvectb_layersToUseForComputationFlags_PX1_PX2_deltaZ,
        const e_mainComputationMode eMainComputationMode,
        QVector<bool>& qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ) const;


    bool vectBoxIdValid(int vectBoxId) const;

    bool get_oneStackedProfilBoxParametersToQJsonObjectWithoutBoxSetInfos(int vectBoxId, QJsonObject& qjsonObj);
    bool get_oneStackedProfilBoxParameters(int vectBoxId, S_ProfilsBoxParameters& SProfilsBoxParameters);



    bool setLength_ofOneProfilsBox(int vectBoxId, int oddPixelLength);

    //updateProfilsBoxParameters_... methods update also additionnal field S_LocationFromZeroOfFirstInsertedValueInStackedProfil and set the flag needRecomputation
    bool updateProfilsBoxParameters_noBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, bool bForceBreakAutomaticDistributionLink = false);
    //moving the move along the route lead to qvect reordering:
    bool updateProfilsBoxParameters_withBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, int& newVectBoxId_afterReorderAlongTheRoute);


    bool profilCurvesForOneStackedProfilBox_toQJsonObject(int vectBoxId, bool bSetProfilesCurvesData_asEmpty, QJsonObject& qjsonObj_profilCurves) const;
    bool computedComponentsToHeader_toQJsonObject(QJsonObject& qjsonObj_computedComponentsToHeader) const;

    bool profilCurvesForOneStackedProfilBox_toAscii(int vectBoxId,
                                                    bool bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                                                    QVector<QStringList>& qvectQstrList_profilCurveContent) const;

    bool computedComponentsToHeader_toAscii(QStringList& qstrList_computedComponentsToHeader,
                                            QStringList& qstrList_title_tuple_X_Perp_Parall_DeltaZ) const;

    bool someFlaggedProfilExist() const;
    bool someFlaggedProfilExistForBoxId(int boxId) const;

protected: //private:

    bool adjustMeasurementsDueToProgressDirectionChanged(bool& bRestoredDueToDevError, bool& bInternalErrorWhenComputinglRSidesLinearRegressionsModels);
    //bool adjustMeasurementsDueToProgressDirectionChanged(bool& bRestoredDueToDevError);

    bool reverseBoxMeasurements(S_BoxAndStackedProfilWithMeasurements *SBASPWSPtr);

    bool computeAndInit_componentsToComputeFlags_from_layersToUseForComputationFlags_and_mainComputationMode();




    bool compute_leftRightSidesLinearRegressionsModel(
            S_BoxAndStackedProfilWithMeasurements& boxAndStackedProfilWithMeasurements,
            bool bComputeMainMeasure,
            int secondaryMeasureIdxToCompute);

    bool computeLinearRegressionParametersOfAnonymMajor(
            const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_AnonymMajorMinorWithValidityFlag,
            S_LocationFromZeroOfFirstInsertedValueInStackedProfil locationFromZeroOfFirstInsertedValueInStackedProfil,
            S_X0ForYOffsetComputation x0ForYOffsetComputation, //double givenXLocationForYComputation,
            S_LinearRegressionParameters &linearRegressionParameters,
            double& computedYForGivenXLocation);


    void clear_vectUptrPixelPickers();


    bool computeStakedParallele_withSameCorrelScoreForPerpAndParall(
            const vector<double> vect_Perp_Parall_DeltaZ[e_CA_ComponentsCount],
            const vector<double> vectCorrelationScore_PX1_PX2_DeltaZ[eLA_LayersCount],
            U_MeanMedian_computedValuesWithValidityFlag computedValues[e_CA_ComponentsCount]);


    bool update_qvectOf_BASPWMPtr_orderedAlongRoute_afterProfilBoxesAddedAtQMapEnd(int nbAddedProfilsBoxParametersAtEndOfQMap,
                                                                                   bool bBoxesDirectionOrderIsFirstPointToLastPoint,
                                                                                   int& aboutQVectOrderAlongRoute_indexOfFirstChange,
                                                                                   int& aboutQVectOrderAlongRoute_indexOfLastChange,
                                                                                   bool& bCriticalErrorOccured);

    bool update_qvectOf_BASPWMPtr_orderedAlongRoute_afterOneBoxMove(int vectBoxId_toReorder,
                                                                    int& newVectBoxId_afterReorderAlongTheRoute);

protected:

    Route *_routePtr; //for computation

    vector<ValueDequantization>* _vectPtr_valueDequantization_PX1_PX2_deltaZ;

    QMap<int, S_BoxAndStackedProfilWithMeasurements> _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements;

    //Ui view see and edit profilBox through access to it:
    QVector<S_InsertionIndex_SBASPWMPtr> _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap; //from the first point of the route, independant to the selected orientation fault.

    QVector<QPointF> _qvectQPointF_devdebug_centerBox;
    QVector<QPointF> _qvectQPointF_devdebug_firstProfil_centerBox;
    QVector<QPointF> _qvectQPointF_devdebug_firstPointAlongRoute;
    QVector<S_qpf_point_and_unitVectorDirection> _qvect_devdebug_unitVectorForEachPointAlongRoute;
    QVector<QPointF> _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute;


    e_ProfilOrientation _eProfilOrientation;

    e_mainComputationMode _e_mainComputationMode;


    S_InputFiles _inputFiles;

    //deduced flags using _computationMainMode and _inputFiles

    QVector<bool> _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ;
    QVector<bool> _qvectb_componentsToCompute_Perp_Parall_DeltaZ; //according to _qvectb_layersToUseForComputationFlags_PX1_PX2_DeltaZ and _e_mainComputationMode

    U_CorrelationScoreMapFileUseFlags _correlationScoreMapFileAvailabilityFlags;//flags indicating if the corel map files are available for computation usage (it does not indicate that it's used in computation)

    S_ComputationParameters _computationParameters;

    vector<e_ComputationMethod> _vect_eComputationMethod_Perp_Parall_DeltaZ; //@LP we keep this outside _computationParameters to be sure only computationCore can set this value according to _computationParameters and others fields involved

    vector<unique_ptr<PixelPicker>> _vectUptrPixelPickers_PX1_PX2_DeltaZ;


    vector<unique_ptr<PixelPicker>> _vectUptrPixelPickers_correlationScoreFor_PX1_PX2_DeltaZ;

    //--- #LP could go in a dedicated class
    //filter mask:
    //value at 0 means pixel that the pixel can be used for computation
    //values different than 0 means pixel has to be removed for computation
    //each values differents than 0 have a different meaning about the filter reason:
    // //- pixel outside the image (case handled differently)
    //  - pixel value is Nan
    // //- pixel value is Inf
    //  - pixel value is below the clip range
    //  - pixel value is above the clip range
    // //- stand by: 6: pixel is a value to be filtered (from the vector about specific value
    //  - correlationscore to low

    enum e_filterMask {
        e_fM_noSet = 0,
        e_fM_keepForComputation,
        //e_fM_outOfImage,
        e_fM_isNan,//@#LP standby: -Nan
        //e_fM_isInf,//@#LP standby: -Inf
        //e_fM_isNan_or_isInf,//@#LP standby
        e_fM_isBelowClipRange,
        e_fM_isAboveClipRange,
        e_fM_correlationScoreToLow
    };

    bool pickPixels_correlationScore_PX1PX2Together(
                Vec2<double> vec2dbPoint,
                Vec2<double> vec2dbNormalizedBoxUnitVectorDirection,
                double& correlationScore,// correlation score values for one parallele (same for PX1 and PX2 when non independant)
                double Perp_Parall[2],
                e_filterMask filterMask_Perp_Parall[2],
                bool bDebugGettingContent);

    bool pickPixels_correlationScore_independantLayerDeltaZ(
            Vec2<double> vec2dbPoint,
            double& correlationScore,// correlation score values for one parallele (PX1 and PX2 will have the same when non independant)
            double& pixelValue,//values for one parallele by layers (PX1, PX2, DeltaZ)
            e_filterMask& ilterMask);

    int countValuesNotFiltered(const QVector<e_filterMask>& qvect_filterMask);
    bool setFilterMaskFlags_nan(const QVector<float>& qvect_notFilteredValues,
                                 QVector<e_filterMask>& qvect_filterMask,
                                 int& nbValuesWhichStayAfterFilter);

    e_ComputationMethod convert_eBaseComputationMethod_to_eComputationMethod(
            e_BaseComputationMethod baseComputationMethod,
            bool bCorrelationScoreMapFileAvailabilityFlag,
            bool bCorrelationScoreMapFileUsedForComputation);

    QVector<QPointF> _qvectQPointF_devdebug_firstPointEverStackedProfil;
    QVector<QPointF> _qvectQPointF_devdebug_lastPoinOfFirstparallelelineStackedProfil;
    QVector<QPointF> _qvectQPointF_devdebug_inBetweenFirstAndLastFirstparallelelineStackedProfil;

    QVector<QPointF> _qvectQPointF_devdebug_firstPointsOfParalleles;
    QVector<QPointF> _qvectQPointF_devdebug_lastPointsOfParalleles;

};

#endif // COMPUTATIONCORE_H
