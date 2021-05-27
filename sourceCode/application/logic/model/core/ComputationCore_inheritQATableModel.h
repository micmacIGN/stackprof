#ifndef COMPUTATIONCORE_INHERITQATABLEMODEL_H
#define COMPUTATIONCORE_INHERITQATABLEMODEL_H

#include <QAbstractTableModel>
#include <QItemSelection>

#include "ComputationCore.h"

enum e_StackedProfilTableViewColumn {
    e_SPTVC_vectIdx = 0,
    //e_SPTVC_internalBoxID,

    e_SPTVC_bNeedsToBeRecomputedFromImages,

    e_SPTVC_qpfCenterPoint,
    e_SPTVC_unitVectorDirection,
    e_SPTVC_oddPixelWidth,
    e_SPTVC_oddPixelLength,

    e_SPTVC_sUniqueBoxID_setID,
    e_SPTVC_sUniqueBoxID_idxBoxInSet,

    e_SPTVC_idxSegmentOwnerOfCenterPoint,

    e_SPTVC_bDevDebug_wasAdjusted,
    e_SPTVC_bDevDebug_tooCloseBox_rejected_needGoFurther,
    e_SPTVC_bDevDebug_tooFarBox_rejected_needGoCloser,

    e_SPTVC_locationFromZeroOfFirstInsertedValueInStackedProfil_value,
    e_SPTVC_locationFromZeroOfFirstInsertedValueInStackedProfil_bValid,

    //Perp
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__x0,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__modelValuesResults__bComputed,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__modelValuesResults__yOffsetAtX0,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure__modelValuesResults__sigmabSum,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__LRSide_linRegrModel_mainMeasure_bValid,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Perp__bValid,

    //Parall
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__x0,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__modelValuesResults__bComputed,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__modelValuesResults__yOffsetAtX0,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure__modelValuesResults__sigmabSum,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__LRSide_linRegrModel_mainMeasure_bValid,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_Parall__bValid,

    //DeltaZ or Other
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__RangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_leftSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMin,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__linearRegressionParam_Left_Right_rightSide__xRangeForLinearRegressionComputation__xMax,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__bValid,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__x0ForYOffsetComputation__x0,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__modelValuesResults__bComputed,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__modelValuesResults__yOffsetAtX0,
    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure__modelValuesResults__sigmabSum,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__LRSide_linRegrModel_mainMeasure_bValid,

    e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_DeltaZ__bValid,

    e_SPTVC_columnCount,
    e_SPTVC_lastColumn = e_SPTVC_columnCount -1,
};

class ComputationCore_inheritQATableModel : public QAbstractTableModel, public ComputationCore {

    Q_OBJECT

public:
    ComputationCore_inheritQATableModel(QObject *parent = nullptr);

    //---------------------------------
    //QAbstractTableModel side -------- //@#LP split using inheritance ? (like class RouteContainerWithTableModel : public RouteContainer, public QAbstractTableModel ?)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
public slots:
    virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

public:
    //@LP genereric way to inform the outside, virtuals method in ComputationCore with empty body
    //@LP virtuals methods are not defined pure, to let ComputationCore instanciable (if required) without mandatory inhenritence.
    virtual void stepProgressionForOutside(int v) override;
    //virtual void stepProgressionForOutside_terminated() override;
signals:
    void signal_stepProgressionForOutside(int value);
    //void signal_stepProgressionForOutside_terminated();

public:
    void clear();
    void removeSectionOfBoxes(int aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                              int aboutQVectOrderAlongRoute_indexOfLastToRemove);
    void removeAllBoxes();

    void clearComputedStakedProfilsContent(bool bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages);
    //void clearComputedStakedProfilsContent();

    //(applying the default value for the linear regression)
    bool add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
                                                                                bool bBoxesDirectionOrderIsFirstPointToLastPoint,
                                                                                int& aboutQVectOrderAlongRoute_firstInsertedBox, int& keyMapOfFirstInsertedBox,
                                                                                bool& bCriticalErrorOccured);

    bool add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
            const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
            const QVector<bool> &qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
            bool bBoxesDirectionOrderIsFirstPointToLastPoint,
            int& aboutQVectOrderAlongRoute_firstInsertedBox, int& keyMapOfFirstInsertedBox,
            bool& bCriticalErrorOccured);

    bool get_oneProfilsBoxParametersFromvectBoxId(int boxID, S_ProfilsBoxParameters& profilsBoxParameters);

    bool compute_stakedProfilsAlongRoute();

    bool compute_leftRightSidesLinearRegressionsModel_forBoxId(int boxId,
                                                               const S_MeasureIndex& measureIndex);

    bool compute_leftRightSidesLinearRegressionsModels_forAllBoxes();

    bool setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel();
    bool setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(const QVector<bool>& qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ);

    bool setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(int vectIdxOfbox);
    bool setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(int vectIdxOfbox, const QVector<bool> qvectb_componentsToCompute_Perp_Parall_DeltaZ);

    //@LP this is about computationCore, but we avoid the computationCore to use Qt signals
    void send_computationParametersToView();

    //@LP this is about computationCore, but we avoid the computationCore to use Qt signals
    //void send_vectLayersInUse_toView();
    void send_vectComputedComponentsInUse_toView();

    bool stackedProfilBoxes_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfilBoxes,
                                           const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                           QString& strDetailsAboutFail_projectLoad);

    bool feed_stackedProfil_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfil,
                                          const QVector<bool>& qvectb_componentToCompute,
                                          QString& strDetailsAboutFail_projectLoad);

    //bool setWidth_ofOneProfilsBox(int vectBoxId, int oddPixelWidth);
    bool setLength_ofOneProfilsBox(int vectBoxId, int oddPixelLength);
    bool updateProfilsBoxParameters_noBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters);
    bool updateProfilsBoxParameters_withBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, int& newVectBoxId_afterReorderAlongTheRoute);

signals:
    //@#LP this is about computationCore:
    void signal_computationParameters(S_ComputationParameters computationParameters,
                                      const U_CorrelationScoreMapFileUseFlags& uCorrelationScoreUsageFlags,
                                      QVector<QString> qvectCorrelationScoreMapFile_PX1_PX2_DeltaZ);
    //void signal_vectLayersComputed(QVector<bool> qvectbLayersToCompute);
    void signal_vectComputedComponents(QVector<bool> qvectbComputedComponents);

public slots:

signals:
    //to sent the width and length to view instance which are not tableview
    void signal_selectionBoxChanged(int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion); //send indexes of the same box about the different containers
    void signal_profilsBoxChanged  (int indexBoxInVectOrderAlongRoute, int indexBoxAtQMapInsertion); //send indexes of the same box about the different containers

    void signal_oneOrMoreBoxesExist(bool bExist);

private:
    //int _indexBoxInVectOrderAlongRoute; //save the current box index, to be able to update after a disabled tableview

private:

};

#endif // COMPUTATIONCORE_INHERITQATABLEMODEL_H
