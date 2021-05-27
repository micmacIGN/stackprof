#ifndef COMPUTATIONCORE_STRUCTURES_H
#define COMPUTATIONCORE_STRUCTURES_H

#include <QVector>
#include <QPointF>

#include <QJsonObject>

#include "../../io/InputImageFormatChecker.h"

enum e_ProfilOrientation {
    e_PO_notSet = 0,
    e_PO_progressDirectionIsFromFirstToLastPointOfRoute = 1,
    e_PO_progressDirectionIsFromLastToFirstPointOfRoute = 2
};

//e_LayersAcces is used to access vectors with 3 as size
enum e_LayersAcces {
    eLA_PX1    = 0, eLA_CorrelScoreForPX1Alone    = 0, eLA_CorrelScoreForPX1PX2Together = 0,
    eLA_PX2    = 1, eLA_CorrelScoreForPX2Alone    = 1,
    eLA_deltaZ = 2, eLA_CorrelScoreForDeltaZAlone = 2,

    eLA_LayersCount = 3,
    eLA_Invalid = eLA_LayersCount
};

//e_CA is used to access vectors with 3 as size, or 2 (Perp, Parall)

enum e_ComponentsAccess {
    e_CA_Perp   = 0,
    e_CA_Parall = 1,
    e_CA_deltaZ = 2,

    e_CA_ComponentsCount = 3,
    e_CA_Invalid = e_CA_ComponentsCount
};


struct S_xDisplayRange {
    qreal xGraphMin;
    qreal xGraphMax;
};

enum e_mainComputationMode {
    e_mCM_notSet,
    e_mCM_Typical_PX1PX2Together_DeltazAlone,
    //e_mCM_AllIndependant_PX1_PX2_DeltaZ
};

enum e_vectValueDequantizationFeedState {
    e_vVDFS_notSet = -1,

    e_vVDFS_FirstAlone_routeSetEditing = eLA_PX1,

    e_vVDFS_PX1Alone = eLA_PX1,

    e_vVDFS_PX2Alone = eLA_PX2,
    e_vVDFS_deltaZAlone = eLA_deltaZ,

    e_vVDFS_PX1_PX2,
    e_vVDFS_PX1_PX2_deltaZ,

    e_vVDFS_clear
};

//if one inputFile is empty, the computation which needs it will be consider not wanted
struct S_InputFiles {
    QVector<QString> _qvectStr_inputFile_PX1_PX2_DeltaZ;  //@#LP rename field
    QVector<QString> _qvectStr_correlationScore_PX1_PX2_DeltaZ; //when e_mCM_Typical_PX1PX2Together_DeltazAlone, CorrelationScore PX1 = CorrelationScore PX2
    S_InputFiles();
    void clear();
    bool fromQJsonObject(const QJsonObject& qjsonObj, e_mainComputationMode eMainComputationMode);
    void showContent() const;
};

struct S_InputImagesFileAttributes {
    QVector<S_ImageFileAttributes> _qvectSImageFileAttributes_inputFile_PX1_PX2_DeltaZ;  //@#LP rename field
    QVector<S_ImageFileAttributes> _qvectSImageFileAttributes_correlationScore_PX1_PX2_DeltaZ; //when e_mCM_Typical_PX1PX2Together_DeltazAlone, CorrelationScore PX1 = CorrelationScore PX2
    S_InputImagesFileAttributes();
    void clear();
    bool fromQJsonObject(const QJsonObject& qjsonObj, e_mainComputationMode eMainComputationMode);
    void showContent();
    //bool compare(const S_InputImagesFileAttributes & sInputImagesFileAttributes, e_mainComputationMode eMainComputationMode);
};

//we do not put S_InputImagesFileAttributes inside S_InputFiles because
//the attributes can be loaded from json file and there is none warranty that these attributes
//really match with the filename (in S_InputFiles)
//Hence, to avoid ambiguity (attributes loaded from json or got from the physical files),
//the attributes fields are located outside S_InputFiles.
struct S_InputFilesWithImageFilesAttributes {
    S_InputFiles _inputFiles;
    S_InputImagesFileAttributes _inputImagesFileAttributes;
    void clear();

    void set_mainComputationMode(e_mainComputationMode eMainComputationMode);

    bool fromQJsonObject(const QJsonObject& qjsonObj);

    bool fromInputFiles(const S_InputFiles& inputFiles);

    bool toQJsonObject(QJsonObject& qjsonObj) const;

    void showContent();

    e_mainComputationMode _eMainComputationMode;

    S_InputFilesWithImageFilesAttributes();

    //bool compareAttributes(const S_InputFilesWithImageFilesAttributes& sInputFilesWithImageFilesAttributes_attributesFromFiles);

};

#include <vector>
using namespace std;

//works together:---
/*struct S_LayersToComputeFlags {
    QVector<bool> _qvectb_PX1_PX2_DeltaZ;
};*/


//these flags are deduced when the input files for correlation score map are empty or not.
//If a file is not empty, the according S_WeightingParameters will be used to know how weighting will be used
union U_CorrelationScoreMapFileUseFlags {
    struct S_CSF_PX1PX2Together_DeltazAlone {
        bool _b_onPX1PX2;
        bool _b_reservedForStructSync;
        bool _b_onDeltaZ;
    } _sCSF_PX1PX2Together_DeltazAlone;
    struct S_CSF_AllIndependant_PX1_PX2_DeltaZ {
        bool _b_PX1_PX2_DeltaZ[eLA_LayersCount];
    } _sCSF_AllIndependant_PX1_PX2_DeltaZ;
    //U_CorrelationScoreUsageFlags();
    void clear();
    void showContent();
};
//---
//--------

struct S_CorrelationScoreMapParameters {

    bool _bUse; //when true, the correlation score will be used. _thresholdRejection and _weighting are additional options

    struct S_ThresholdRejection {
        bool _bUse;
        float _f_rejectIfBelowValue;
    } _thresholdRejection;

    struct S_weighting {
        bool _bUse;
        float _f_weightExponent; //when _weighting is used, _f_weightExponent is > 1
    } _weighting;

    S_CorrelationScoreMapParameters();
    void setDefaultValues();
    bool toQJson(QJsonObject& qjsonObj) const;
    bool fromQJson(const QJsonObject qjsonObj);
    void show(const QString& strTitle) const;
    bool isEqualTo_withTolerance(const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters);
};

struct S_UniqueBoxID {
    qint64 _setID;
    int _idxBoxInSet;
};

enum e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution {
    e_DBSBuDA_notSet,
    e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel,
    e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel,
};

enum e_StartingPointForAutomaticDistribution {
    e_SPFAD_notSet,
    e_SPFAD_firstPoint,
    e_SPFAD_lastPoint,
};

struct S_infoAboutAutomaticDistribution {
    e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution _e_DBSBuAD_usedAndStillValid;
    //set to != e_DBSBuAD_square2_...  and != e_DBSBuAD_square8_... if moved by the user or distributed not using the automatic distribution with close as possible
};


//parameters which define a box of profils
struct S_ProfilsBoxParameters {

    S_UniqueBoxID _sUniqueBoxID; //used for remove operation
    S_infoAboutAutomaticDistribution _sInfoAboutAutomaticDistribution;

    QPointF _qpfCenterPoint;
    QPointF _unitVectorDirection;

    int _oddPixelWidth;
    int _oddPixelLength;

    int _idxSegmentOwnerOfCenterPoint;
    //@LP used for easy insertion of a new ProfilsBoxParameter along the route. _id follow box after box. Hence, for easy insertion,
    //    _idxSegmentOwnerOfCenterPoint is used to reduce the task to find the right id for the new box to insert (and shift the others after)

    //@#LP dev-debug purpose flags only:
    bool _bDevDebug_wasAdjusted;
    bool _bDevDebug_tooCloseBox_rejected_needGoFurther;
    bool _bDevDebug_tooFarBox_rejected_needGoCloser;

    S_ProfilsBoxParameters();
    S_ProfilsBoxParameters(const S_UniqueBoxID& sUniqueBoxID,
                           const S_infoAboutAutomaticDistribution& sInfoAboutAutomaticDistribution,
                           const QPointF& qpfCenterPoint,
                           const QPointF& unitVectorDirection,
                           int oddPixelWidth,
                           int oddPixelLength,
                           int idxSegmentOwnerOfCenterPoint,
                           bool bDevDebug_wasAdjusted,
                           bool bDevDebug_tooCloseBox_rejected_needGoFurther,
                           bool bDevDebug_tooFarBox_rejected_needGoCloser);

    void showContent() const;
    void showContent_centerPointAndIdxSegmentOnly() const;

    bool toQJsonObject(int idBox, QJsonObject &qJsonObjOut) const;
    bool fromQJsonObject(const QJsonObject &qJsonObj);

};
#include <QMetaType>
Q_DECLARE_METATYPE(S_ProfilsBoxParameters);

#include "../../mathComputation/linearRegression.h"

//used when moving the centerbox of a box along the route
struct S_ProfilsBoxParametersForRelativeMoveAlongRoute {
   int _shiftFromInitialLocation;
   bool _bPossible;
   S_ProfilsBoxParameters _profilsBoxParameters;
   S_ProfilsBoxParametersForRelativeMoveAlongRoute();
};

struct S_QPointF_withValidityFlag {
  QPointF _pointF;
  bool _bValid;
};

struct S_CoupleOfQPointF_withValidityFlag { //@#LP rename structure for better meaning ( smallestX/biggestX content)
  QPointF _pointWithSmallestX;
  QPointF _pointWithBiggestX;
  bool _bValid;
  void clear();
};

struct S_CoupleOfQPointF {
  QPointF _point1;
  QPointF _point2;
};


// Q_DECLARE_METATYPE( Type) :
// ' This macro makes the type Type known to QMetaType as long as it provides a public default constructor,
//   a public copy constructor and a public destructor. It is needed to use the type Type as a custom type in QVariant. '

struct S_XRangeForLinearRegressionComputation {
    bool _bValid;
    double _xMin;
    double _xMax;

    S_XRangeForLinearRegressionComputation();
    S_XRangeForLinearRegressionComputation(qreal xMin, qreal xMax);
    S_XRangeForLinearRegressionComputation(const S_XRangeForLinearRegressionComputation& copyMe) = default;
    void clear();
    void set(qreal xMin, qreal xMax);
    bool toQJsonObject(QJsonObject &qJsonObjOut) const;
    bool fromQJsonObject(const QJsonObject &qJsonObj);
    void showContent() const;
};

#include <QMetaType>
Q_DECLARE_METATYPE(S_XRangeForLinearRegressionComputation);


struct S_MeasureIndex {
    bool _bOnMainMeasure;
    int _secondaryMeasureIdx;
    S_MeasureIndex(bool bOnMainMeasure, int secondaryMeasureIdx = -1);
    S_MeasureIndex();
    void clear();
};

struct S_LinearRegressionParameters {

    S_XRangeForLinearRegressionComputation _xRangeForLinearRegressionComputation;
    S_MathComp_LinearRegressionParameters _mathComp_LinearRegressionParameters;

    S_LinearRegressionParameters();
    S_LinearRegressionParameters(qreal xMin, qreal xMax);
    void set_xRange(qreal xMin, qreal xMax);

    void clear();

    bool XRangeForLinearRegressionComputation_toQJsonObject(QJsonObject &qjsonObj) const;
    bool XRangeForLinearRegressionComputation_fromQJsonObject(const QJsonObject &qJsonObj);

    bool linearRegrModel_toQJsonObject(QJsonObject &qjsonObj) const;

    bool linearRegrModel_toASciiStringlist(bool bSetLinearRegressionData_asEmpty,
                                           QStringList& qstrList_sideOfLinearRegressionModel) const;

    void showContent() const;

};

struct S_X0ForYOffsetComputation {
    bool _bValid;
    double _x0;
    S_X0ForYOffsetComputation();
    void set(double x0);

    void clear();

    void showContent() const;
};


struct S_LRSide_modelValuesResults {
    bool _bComputed;
    qreal _yOffsetAtX0;//delta between 'y from the line at left side for x0 (typically 0)' and 'y from the line at right side for x0 (typically 0)'
    qreal _sigmabSum;//sum of std errors of intercepts of the two lines (similar to the cossicor sigma computation)
    S_LRSide_modelValuesResults();
    void set(qreal yOffsetAtX0, qreal sigmabSum);
    void clear();
    void showContent() const;
};



struct S_LeftRightSides_linearRegressionModel {

    S_LinearRegressionParameters _linearRegressionParam_Left_Right[2];

    S_X0ForYOffsetComputation _x0ForYOffsetComputation;
    S_LRSide_modelValuesResults _modelValuesResults;

    bool _bWarningFlagByUser;

    S_LeftRightSides_linearRegressionModel();
    void clearResults();
    //S_StackedProfil_LeftRightSides_linearRegressionModel(const S_StackedProfil_LeftRightSides_linearRegressionModel& copyMe) = default;
    //S_StackedProfil_LeftRightSides_linearRegressionModel(S_StackedProfil_LeftRightSides_linearRegressionModel&& moveMe) = default;
    bool XRangesForLinRegr_withX0_toQJsonObject(QJsonObject& qjsonObjOut) const;
    bool XRangesForLinRegr_withX0_fromQJsonObject(const QJsonObject& qjsonObjOut);

    bool linearRegrModel_toQJsonObject(QJsonObject& qjsonObjOut) const;

    void warnFlagByUser_toQJsonObject(QJsonObject& qjsonObjOut) const;
    bool warnFlagByUser_fromQJsonObject(const QJsonObject &qJsonObj, QString& strMsgErrorDetails);

    QJsonObject qJsonObjOut_LRmainMeasure_offsetAndSigmaSum;
    bool bReport_LRMainMeasure_offsetAndSigmaSum = false;

    bool modelValuesResults_toQJsonObject(QJsonObject& qjsonObjOut) const;

    void clearResultsAndReverseSign();

    void showContent();

};


struct S_aboutMean {
    double _mean;
    double _absdevAroundMean;
    bool _bValuesValid;
};

struct S_aboutMedian {
    double _median;
    double _absDevAroundMedian;
    bool _bValuesValid;
};

struct S_aboutWMean {
    double _wMean;
    double _wAbsdevAroundWMean;
    bool _bValuesValid;
};

struct S_aboutWMedian {
    double _wMedian;
    double _absDevAroundWMedian;
    bool _bValuesValid;   
};


struct S_AnonymMajorMinorWithValidityFlag {
    double _major_centralValue;       //central tendency
    double _minor_aroundCentralValue; //dispersion measure
    bool _bValuesValid;
};

union U_MeanMedian_computedValuesWithValidityFlag {
   S_aboutMean     _aboutMean;
   S_aboutMedian   _aboutMedian;
   S_aboutWMean    _aboutWMean;
   S_aboutWMedian  _aboutWMedian;
   S_AnonymMajorMinorWithValidityFlag _anonymMajorMinorWithValidityFlag;
   U_MeanMedian_computedValuesWithValidityFlag();
   void clear();
};

//-------
//e_MeanMedianIndexAccess and e_ComputationMethod works together
enum e_MeanMedianIndexAccess {  e_MMIA_invalid = -1,  // sync with e_ComputationMethod

                                e_MMIA_aboutMean = 0, //vector index, has to start to 0
                                e_MMIA_aboutMedian,
                                e_MMIA_aboutWMean,
                                e_MMIA_aboutWMedian,

                                e_MMIA_count
};

//e_ComputationMethod sync with e_MeanMedianIndexAccess for easyest usage from computationMethod to meanMedian vector insertion
enum e_ComputationMethod { e_CM_notSet = -1,

                           e_CM_mean = e_MMIA_aboutMean,
                           e_CM_median = e_MMIA_aboutMedian,
                           e_CM_weightedMean = e_MMIA_aboutWMean,
                           e_CM_weightedMedian = e_MMIA_aboutWMedian,
};
//-------

enum e_BaseComputationMethod {
    e_BCM_notSet = e_CM_notSet,

    e_BCM_mean   = e_CM_mean,
    e_BCM_median = e_CM_median
};


struct S_StackedProfilWithMeasurements {

    //allows to have different computationMethod results values storable at the same time (*)
    //@LP: Use e_MeanMedianIndexAccess to get/set value into this field:
    QVector<QVector<U_MeanMedian_computedValuesWithValidityFlag>> _qvectqvect_aboutMeanMedian;
    //Note that none of these vectors contains informations about location of the values along through the profil

    //(*) But only one set of linearRegressionModel stored at any time:
    S_LeftRightSides_linearRegressionModel _LRSide_linRegrModel_mainMeasure;

    //we use a qvector to handle secondary measures
    QVector<S_LeftRightSides_linearRegressionModel> _LRSide_linRegrModel_secondaryMeasures;

    S_StackedProfilWithMeasurements();

    S_LeftRightSides_linearRegressionModel* getPtrOn_LRSidesLinearRegressionModel(const S_MeasureIndex& measureIndex);

    //@#LP set on the two sides for now:
    bool set_LRSidesLinearRegressionModel(
            const S_MeasureIndex& measureIndex,
            const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation_left_right_side[2]);

    bool set_X0_forLinearRegressionModel(const S_MeasureIndex& measureIndex, const double& X0Pos);

    bool set_warnFlag(const S_MeasureIndex& measureIndex, bool bWarnFlag);
    bool get_warnFlag(const S_MeasureIndex& measureIndex, bool& bWarnFlag);

    void clear_dataContent();
};

struct S_ThresholdValues_forRejection {
    float _valueDown_rejectionIfBelow;
    float _valueUp_rejectionIfAbove;
    bool _bUseValueUp_forRejection;
    bool _bUseValueDown_forRejection;
};

enum e_PixelExtractionMethod {
    e_PEM_notSet,
    e_PEM_NearestPixel,            //pixel distance between end and start of folling profils box has to be > sqrt(2) (1²+1²) to avoid common pixel
    e_PEM_BilinearInterpolation2x2 //pixel distance between end and start of folling profils box has to be > sqrt(9) (3²+3²) to avoid common pixel
};


struct S_ComputationParameters {
    e_PixelExtractionMethod _ePixelExtractionMethod;

    e_BaseComputationMethod _eBaseComputationMethod;

    S_CorrelationScoreMapParameters _correlationScoreMapParameters_PX1_PX2_DeltaZ[3];

    S_ComputationParameters();

    void clear();

    bool isEqualTo_withTolerance(const S_ComputationParameters& sComputationParameters, const QVector<bool>& qvectBool_usedLayer);

    void debugShow() const;

};

//-------

struct S_qpf_point_and_unitVectorDirection {
    QPointF _point;
    QPointF _unitVectorDirection;
    int _idxSegmentOwner; //used to compute point distribution along route by 'sliding portion of the route' instead of from start to end of the route (which can be very long)
};

struct S_LocationFromZeroOfFirstInsertedValueInStackedProfil {
    double _value;
    bool _bValid;
    S_LocationFromZeroOfFirstInsertedValueInStackedProfil();
    void set(double value);
    void clear();
};

struct S_FloatWithValidityFlag {
  float _f;
  bool _bValid;
  S_FloatWithValidityFlag();
  void clear();
  void set(float f);
};


struct S_BoxAndStackedProfilWithMeasurements {

    bool _bNeedsToBeRecomputedFromImages;

    S_ProfilsBoxParameters _profilsBoxParameters;

    S_LocationFromZeroOfFirstInsertedValueInStackedProfil _locationFromZeroOfFirstInsertedValueInStackedProfil;

    QVector<S_StackedProfilWithMeasurements> _qvect_Perp_Parall_DeltaZ_StackedProfilWithMeasurements;

    explicit S_BoxAndStackedProfilWithMeasurements();

    //explicit S_BoxAndStackedProfilWithMeasurements(bool bNeedToBeRecomputedFromImages, const S_ProfilsBoxParameters& profilsBoxParameters);

    //@LP constructeur set _locationFromZeroOfFirstInsertedValueInStackedProfil
    explicit S_BoxAndStackedProfilWithMeasurements(bool bNeedsToBeRecomputedFromImages,
                                                   const S_ProfilsBoxParameters& profilsBoxParameters/*,
                                                   const S_UniqueBoxID& sUniqueBoxID*/);

    explicit S_BoxAndStackedProfilWithMeasurements(const S_BoxAndStackedProfilWithMeasurements& copyMe);
    S_BoxAndStackedProfilWithMeasurements& operator=(const S_BoxAndStackedProfilWithMeasurements& BoxAndStackedProfilWithMeasurements);

    void showContent_noMeasurements() const;
    void showContent_mainMeasurement_linearRgressionModel() const;

    void clearMeasurementsDataContent();

    //does not include _profilsBoxParameters:
    bool profilWithMeasurements_withoutLinearRegresionModelResult_toQJsonObject(
            const QVector<bool>& qvectb_computedComponentsFlags_Perp_Parall_deltaZ,
            int idBox,
            QJsonObject &qJsonObjOut) const;

    bool profilWithMeasurements_withLinearRegresionModelResult_toQJsonObject(
            const QVector<bool>& qvectb_computedComponentsFlags_Perp_Parall_deltaZ,
            int idBox,
            const QVector<bool>& qvectbSetLinearRegressionData_asEmpty,
            QJsonObject &qJsonObjOut) const;

    bool profilWithMeasurements_withoutLinearRegresionModelResult_fromQJsonObject(
            const QJsonObject &qJsonObj, const QVector<bool>& qvectb_layersToComputeFlags_PX1_PX2_DeltaZ,
            int boxId,
            QString& strMsgErrorDetails);

    bool setDefaultMinMaxCenterValues_for_mainMeasureLeftRightSidesLinearRegressionsModel(const QVector<bool>& qvectb_componentsToSetToDefault_Perp_Parall_DeltaZ);

    bool computedComponentsToHeader_toQJsonObject_aboutProfilCurves(
        const vector<e_ComputationMethod>& vector_eComputationMethod,
        const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
        QJsonObject& qjsonObj_computedComponentsToHeader) const;

    bool computedComponentsToHeader_toAscii_aboutProfilCurves(
        const vector<e_ComputationMethod>& vector_eComputationMethod,
        const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
        QStringList& qstrList_computedComponentsToHeader, QStringList& qstrList_title_tuple_X_Perp_Parall_DeltaZ) const;

    bool profilCurves_toQJsonObject(
        int vectBoxId,
        const vector<e_ComputationMethod>& vector_eComputationMethod,
        const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
        const QVector<bool>& qvectbSetProfilesCurvesData_asEmpty,
        QJsonObject& qjsonObj_profilCurves) const;

    bool profilCurves_toAscii(int vectBoxId,
                              const vector<e_ComputationMethod>& vector_eComputationMethod,
                              const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                              bool bIfWarnFlagByUser_setProfilesCurvesData_asEmpty,
                              QVector<QStringList>& qvectQstrList_profilCurveContent) const;
private:
    //@LP avoid affectation, avoid move
    S_BoxAndStackedProfilWithMeasurements& operator=(S_BoxAndStackedProfilWithMeasurements&& BoxAndStackedProfilWithMeasurements);
    S_BoxAndStackedProfilWithMeasurements(S_BoxAndStackedProfilWithMeasurements&& BoxAndStackedProfilWithMeasurements);
};

QString ePixelExtractionMethod_toJsonProjectString(e_PixelExtractionMethod ePixelExtractionMethod);
QString eBaseComputationMethod_toJsonProjectString(e_BaseComputationMethod eBaseComputationMethod);
QString eProfilOrientation_toJsonProjectString(e_ProfilOrientation eProfilOrientation);

e_BaseComputationMethod projectJsonString_to_eBaseComputationMethod(const QString& strBaseComputationMethod);
e_PixelExtractionMethod projectJsonString_to_ePixelExtractionMethod(const QString& strPixelExtractionMethod);
e_ProfilOrientation projectJsonString_to_eProfilOrientation(const QString& strProfilOrientation);

e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution projectJsonInt_to_e_DistanceBetweenSuccessivesBoxesUAD(const int ieDistanceBSBAD);

bool getVectStr_aboutProfilCurvesMeanings(e_mainComputationMode eMainComputationMode,
                                          const QVector<bool>& qvectb_componentsToUse_Perp_Parall_DeltaZ,
                                          const vector<e_ComputationMethod>& vect_eComputationMethod_Perp_Parall_DeltaZ,
                                          QVector<QString>& qvectStr_majorCurveMeaning,
                                          QVector<QString>& qvectStr_minorEnveloppeAreaMeaning);

#endif // COMPUTATIONCORE_STRUCTURES_H
