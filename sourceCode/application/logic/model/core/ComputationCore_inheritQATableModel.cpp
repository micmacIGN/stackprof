#include <QDebug>

#include <QAbstractTableModel>

#include "ComputationCore.h"

#include "ComputationCore_inheritQATableModel.h"

ComputationCore_inheritQATableModel::ComputationCore_inheritQATableModel(QObject *parent):
    QAbstractTableModel(parent), ComputationCore() {
}

//@#LP Be sure to update this when you will handle the insert !
//all update here just append at end like the vector is feed to the end
//no inserstion according to segment owner for now

void ComputationCore_inheritQATableModel::clearComputedStakedProfilsContent(bool bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages) {

    ComputationCore::clearComputedStakedProfilsContent(bClearAllBoxes_dontCareAboutOnlyWhichNeedsToBeRecomputedFromImages);

    //brut way; all cells for now:

    int qhashBASPWM_size  = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft = index(0, 0);
    QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size, e_SPTVC_lastColumn);

    emit dataChanged(qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight);
}




void ComputationCore_inheritQATableModel::stepProgressionForOutside(int v) { //@LP genereric way to inform the outside, virtual in ComputationCore
    qDebug() << __FUNCTION__;
    emit signal_stepProgressionForOutside(v);//generic way, the receiver has to know how to handle it
}

/*void ComputationCore_inheritQATableModel::stepProgressionForOutside_terminated() { //@LP genereric way to inform the outside, virtual in ComputationCore
    qDebug() << __FUNCTION__;
    emit signal_stepProgressionForOutside_terminated();//generic way, the receiver has to know how to handle it
}*/

void ComputationCore_inheritQATableModel::clear() {
    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    qDebug() << __FUNCTION__ << "be sure to remove also graphics item synced and models feed";

    int qhashBASPWM_size  = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();
    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        emit layoutAboutToBeChanged();

        QModelIndex qmodelIndexForRemoveAll =  index(0, 0);

        qDebug() << __FUNCTION__ << "qmodelIndexForRemoveAll= " << qmodelIndexForRemoveAll;
        qDebug() << __FUNCTION__ << "qlinkedlist_route_size= " << qhashBASPWM_size;
        qDebug() << __FUNCTION__ << "beginRemoveRows(..., = " << 0 << ", " << qhashBASPWM_size-1;

        int lastIndex = qhashBASPWM_size-1;

        beginRemoveRows(qmodelIndexForRemoveAll, 0, lastIndex);

        ComputationCore::clear();

        endRemoveRows();

        emit layoutChanged();

    } else {

        ComputationCore::clear();

    }

    emit signal_oneOrMoreBoxesExist(false);
}




bool ComputationCore_inheritQATableModel::compute_stakedProfilsAlongRoute() {
    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bool bReport = ComputationCore::compute_stakedProfilsAlongRoute();

    //e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_PX1__LRSide_linRegrModel_mainMeasure__modelValuesResults__bComputed
    //e_SPTVC_qvect_PX1_PX2_DeltaZ_StackedProfilWithMeasurements_PX1__LRSide_linRegrModel_mainMeasure_bValid,

    //brut way; all cells for now:

    int qhashBASPWM_size  = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft = index(0, 0);
        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight);
    }

    return(bReport);
}

//when adding from automatic distribution, we set the default values for the linear regression model
bool ComputationCore_inheritQATableModel::add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
        const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
        bool bBoxesDirectionOrderIsFirstPointToLastPoint,
        int& aboutQVectOrderAlongRoute_firstInsertedBox, int& keyMapOfFirstInsertedBox,
        bool& bCriticalErrorOccured) {

    return(add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(qvectProfilsBoxParameters,
                                                                                  _qvectb_componentsToCompute_Perp_Parall_DeltaZ,
                                                                                  bBoxesDirectionOrderIsFirstPointToLastPoint,
                                                                                  aboutQVectOrderAlongRoute_firstInsertedBox, keyMapOfFirstInsertedBox,
                                                                                  bCriticalErrorOccured));

}

bool ComputationCore_inheritQATableModel::get_oneProfilsBoxParametersFromvectBoxId(int boxId, S_ProfilsBoxParameters& profilsBoxParameters) {

    if (!vectBoxIdValid(boxId)) {
        return(false);
    }

    profilsBoxParameters = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[boxId]._sBASPWMPtr->_profilsBoxParameters;

    qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel";
    profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();

    return(true);
}

void ComputationCore_inheritQATableModel::removeSectionOfBoxes(int aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                                                               int aboutQVectOrderAlongRoute_indexOfLastToRemove) {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    qDebug() << __FUNCTION__ << "be sure to remove also graphics item synced and models feed";

    int qvectBASPWM_size  = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!qvectBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
        return;
    }

    if (!vectBoxIdValid(aboutQVectOrderAlongRoute_indexOfFirstToRemove)) {
        return;
    }
    if (!vectBoxIdValid (aboutQVectOrderAlongRoute_indexOfLastToRemove)) {
        return;
    }    
    if (aboutQVectOrderAlongRoute_indexOfFirstToRemove > aboutQVectOrderAlongRoute_indexOfLastToRemove) {
        return;
    }

    emit layoutAboutToBeChanged();

    QModelIndex qmodelIndexForRemovePart = index(aboutQVectOrderAlongRoute_indexOfFirstToRemove, 0);

    beginRemoveRows(qmodelIndexForRemovePart, aboutQVectOrderAlongRoute_indexOfFirstToRemove, aboutQVectOrderAlongRoute_indexOfLastToRemove);

    ComputationCore::removeSectionOfBoxes(aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                                          aboutQVectOrderAlongRoute_indexOfLastToRemove);

    qDebug() << __FUNCTION__ << "qvectBASPWM_size= " << qvectBASPWM_size;
    qDebug() << __FUNCTION__ << "qmodelIndexForRemovePart= " << qmodelIndexForRemovePart;
    qDebug() << __FUNCTION__ << "firstIndex: " << aboutQVectOrderAlongRoute_indexOfFirstToRemove;
    qDebug() << __FUNCTION__ << "lastIndex : " << aboutQVectOrderAlongRoute_indexOfLastToRemove;

    endRemoveRows();

    emit layoutChanged();

    qvectBASPWM_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    bool bOneOrMoreBoxesExist = (qvectBASPWM_size !=0);
    emit signal_oneOrMoreBoxesExist(bOneOrMoreBoxesExist);
}


void ComputationCore_inheritQATableModel::removeAllBoxes() {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    int qvectBASPWM_size  = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    if (!qvectBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
        return;
    }

    int aboutQVectOrderAlongRoute_indexOfFirstToRemove = 0;
    int aboutQVectOrderAlongRoute_indexOfLastToRemove = qvectBASPWM_size-1;

    emit layoutAboutToBeChanged();

    QModelIndex qmodelIndexForRemovePart = index(aboutQVectOrderAlongRoute_indexOfFirstToRemove, 0);

    beginRemoveRows(qmodelIndexForRemovePart, aboutQVectOrderAlongRoute_indexOfFirstToRemove, aboutQVectOrderAlongRoute_indexOfLastToRemove);

    ComputationCore::removeSectionOfBoxes(aboutQVectOrderAlongRoute_indexOfFirstToRemove,
                                          aboutQVectOrderAlongRoute_indexOfLastToRemove);

    qDebug() << __FUNCTION__ << "qvectBASPWM_size= " << qvectBASPWM_size;
    qDebug() << __FUNCTION__ << "qmodelIndexForRemovePart= " << qmodelIndexForRemovePart;
    qDebug() << __FUNCTION__ << "firstIndex: " << aboutQVectOrderAlongRoute_indexOfFirstToRemove;
    qDebug() << __FUNCTION__ << "lastIndex : " << aboutQVectOrderAlongRoute_indexOfLastToRemove;

    endRemoveRows();

    emit layoutChanged();

    qvectBASPWM_size = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    bool bOneOrMoreBoxesExist = (qvectBASPWM_size !=0);
    emit signal_oneOrMoreBoxesExist(bOneOrMoreBoxesExist);

}


bool ComputationCore_inheritQATableModel::add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
        const QVector<S_ProfilsBoxParameters>& qvectProfilsBoxParameters,
        const QVector<bool> &qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
        bool bBoxesDirectionOrderIsFirstPointToLastPoint,
        int& aboutQVectOrderAlongRoute_firstInsertedBox, int& keyMapOfFirstInsertedBox,
        bool& bCriticalErrorOccured) {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bCriticalErrorOccured = false;

    int toAdd_size_qvectProfilsBoxParameters = qvectProfilsBoxParameters.size();

    if (!toAdd_size_qvectProfilsBoxParameters) {
        qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel) if (!toAdd_size_qvectProfilsBoxParameters)";
        return(false);
    }

    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0; //<=> ] first() to aboutQVectForOrderAlongRoute_indexMaxAboutPartBeforeWhichDoesNotNeedUpdate [
    int aboutQVectOrderAlongRoute_indexOfLastChange = 0;  //<=> ] aboutQVectForOrderAlongRoute_indexMinAboutPartAfterWhichDoesNotNeedUpdate to last() [

    bool bErrorOccured_about_add_BASPWM = false;
    bool bReport = ComputationCore::add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(qvectProfilsBoxParameters,
                                                                                                           qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                                                                                           bBoxesDirectionOrderIsFirstPointToLastPoint,

                                                                                                           aboutQVectOrderAlongRoute_firstInsertedBox,
                                                                                                           keyMapOfFirstInsertedBox,

                                                                                                           aboutQVectOrderAlongRoute_indexOfFirstChange,
                                                                                                           aboutQVectOrderAlongRoute_indexOfLastChange,

                                                                                                           bErrorOccured_about_add_BASPWM);
    if (!bReport) {

        if (bErrorOccured_about_add_BASPWM) {
            bCriticalErrorOccured = true;
        }

        qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel) ComputationCore::add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters report is " << bReport;
        return(false);
    }

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel) aboutQVectOrderAlongRoute_indexOfFirstChange: " << aboutQVectOrderAlongRoute_indexOfFirstChange;
    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel) aboutQVectOrderAlongRoute_indexOfLastChange:  " << aboutQVectOrderAlongRoute_indexOfLastChange;

    emit layoutAboutToBeChanged();

    //update removing and inserting the whole section to update
    QModelIndex qmodelIndexForRemovePart = index(aboutQVectOrderAlongRoute_indexOfFirstChange, 0);
    beginRemoveRows(qmodelIndexForRemovePart, aboutQVectOrderAlongRoute_indexOfFirstChange, aboutQVectOrderAlongRoute_indexOfLastChange);
    endRemoveRows();

    QModelIndex qmodelIndexForInsert = index(aboutQVectOrderAlongRoute_indexOfFirstChange, 0);
    beginInsertRows(qmodelIndexForInsert, aboutQVectOrderAlongRoute_indexOfFirstChange, aboutQVectOrderAlongRoute_indexOfLastChange);
    endInsertRows();

    emit layoutChanged();

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel) now emit signal_oneOrMoreBoxesExist(true)";
    emit signal_oneOrMoreBoxesExist(true);

    return(true);
}



bool ComputationCore_inheritQATableModel::compute_leftRightSidesLinearRegressionsModel_forBoxId(int boxId,
                                                                                                const S_MeasureIndex& measureIndex) {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bool bReport = ComputationCore::compute_leftRightSidesLinearRegressionsModel_forBoxId(boxId, measureIndex);
    if (!bReport) {
        return(bReport);
    }

    //brut way; all cells for now:

    int qhashBASPWM_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft = index(0, 0);
        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight);
    }

    return(true);
}

bool ComputationCore_inheritQATableModel::compute_leftRightSidesLinearRegressionsModels_forAllBoxes() {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bool bReport = ComputationCore::compute_leftRightSidesLinearRegressionsModels_forAllBoxes();
    if (!bReport) {
        return(bReport);
    }

    //brut way; all cells for now:

    int qhashBASPWM_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft = index(0, 0);
        QModelIndex qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_compute_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_compute_stakedProfilsAlongRouteBottomRight);
    }

    return(bReport);
}


//this method use the internal qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ
bool ComputationCore_inheritQATableModel::setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel() {
    return(setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(_qvectb_componentsToCompute_Perp_Parall_DeltaZ));
}

//use external qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ. Typically to set default profils linear regression just loading the json
//and when the core computation has not already set its internal flags about intput files and components to compute
bool ComputationCore_inheritQATableModel::setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(
        const QVector<bool>& qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ) {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bool bReport = ComputationCore::setDefaultMinMaxCenterValues_onAll_for_mainMeasureLeftRightSidesLinearRegressionsModel(qvectb_componentsToComputeFlags_Perp_Parall_DeltaZ);

    //brut way; all cells for now:

    int qhashBASPWM_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft = index(0, 0);
        QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight);
    }

    return(bReport);

}


bool ComputationCore_inheritQATableModel::setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(int vectIdxOfbox) {
    return(ComputationCore_inheritQATableModel::setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(
               vectIdxOfbox, _qvectb_componentsToCompute_Perp_Parall_DeltaZ));
}

bool ComputationCore_inheritQATableModel::setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(
        int vectIdxOfbox, const QVector<bool> qvectb_componentsToCompute_Perp_Parall_DeltaZ) {

    qDebug() << __FUNCTION__ << "(ComputationCore_inheritQATableModel)";

    bool bReport = ComputationCore::setDefaultMinMaxCenterValues_onOne_for_mainMeasureLeftRightSidesLinearRegressionsModel(vectIdxOfbox, qvectb_componentsToCompute_Perp_Parall_DeltaZ);

    //brut way; all cells for now:

    int qhashBASPWM_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (!qhashBASPWM_size) {
        qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    }

    if (qhashBASPWM_size) {

        QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft = index(vectIdxOfbox, 0);
        QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight= index(vectIdxOfbox, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft, qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight);
    }

    return(bReport);
}

int ComputationCore_inheritQATableModel::rowCount(const QModelIndex &parent) const {
    return(_qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.count());
}

int ComputationCore_inheritQATableModel::columnCount(const QModelIndex &parent) const {
    return(e_SPTVC_columnCount);
}

QVariant ComputationCore_inheritQATableModel::data(const QModelIndex &index, int role) const {

    int indexRow = index.row();
    //'Note: If you do not have a value to return, return an invalid QVariant instead of returning 0.'
    QVariant qvariantEmpty;

   //qDebug() << __FUNCTION__ << "RouteContainer::data indexRow = " << indexRow;

   QString str;
    if (role == Qt::DisplayRole) {
        if (indexRow < _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.count()) {

            switch(index.column()) {
            case e_SPTVC_vectIdx:
                return(QString::number(indexRow+1));
                //break;

            case e_SPTVC_bNeedsToBeRecomputedFromImages:
                return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_bNeedsToBeRecomputedFromImages);
                //break;

            case e_SPTVC_qpfCenterPoint:
                str = QString::number(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint.x())
                      + ", " +
                      QString::number(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._qpfCenterPoint.y());
                return(str);
                //break;

            case e_SPTVC_unitVectorDirection:
                return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._unitVectorDirection);
                //break;

            case e_SPTVC_oddPixelWidth:
                return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._oddPixelWidth);
                //break;

            case e_SPTVC_oddPixelLength:
                return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._oddPixelLength);
                 //break;

            case e_SPTVC_sUniqueBoxID_setID:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._sUniqueBoxID._setID);
               //break;

            case e_SPTVC_sUniqueBoxID_idxBoxInSet:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._sUniqueBoxID._idxBoxInSet);
               //break;

            case e_SPTVC_idxSegmentOwnerOfCenterPoint:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._idxSegmentOwnerOfCenterPoint);
               //break;

            case e_SPTVC_bDevDebug_wasAdjusted:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._bDevDebug_wasAdjusted);
               //break;

            case e_SPTVC_bDevDebug_tooCloseBox_rejected_needGoFurther:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._bDevDebug_tooCloseBox_rejected_needGoFurther);
               //break;

            case e_SPTVC_bDevDebug_tooFarBox_rejected_needGoCloser:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_profilsBoxParameters._bDevDebug_tooFarBox_rejected_needGoCloser);
               //break;

            case e_SPTVC_locationFromZeroOfFirstInsertedValueInStackedProfil_value:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._value);
               //break;

            case e_SPTVC_locationFromZeroOfFirstInsertedValueInStackedProfil_bValid:
               return(_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap[indexRow]._sBASPWMPtr->_locationFromZeroOfFirstInsertedValueInStackedProfil._bValid);
               //break;

            default:;
            }
        }
    }
    return(qvariantEmpty);

}

QVariant ComputationCore_inheritQATableModel::headerData(int section, Qt::Orientation orientation, int role) const {

    //Returns the data for the given role and section in the header with the specified orientation.
    //For horizontal headers, the section number corresponds to the column number. Similarly, for vertical headers, the section number corresponds to the row number.
    QVariant qvariantEmpty;
    if (role != Qt::DisplayRole)     { return(qvariantEmpty); }
    if (orientation == Qt::Vertical) { return(qvariantEmpty); }
    //if (section!= 0)               { return(qvariantEmpty); }

    QVector<QString> qvectStrHeaderTitle {
        "devdbg_vectIdx",
        "needsToBeRecomputedFromImages",

        "center point",
        "unit vector direction",
        "pixel width",
        "pixel length",

        "devdbg_sUniqueBoxID._setID",
        "devdbg_sUniqueBoxID._idxBoxInSet",

        "devdbg_idxSegmentOwnerOfCenterPoint",

        "devdbg_wasAdjusted",
        "devdbg_bDevDebug_tooCloseBox_rejected_needGoFurther",
        "devdbg_bDevDebug_tooFarBox_rejected_needGoCloser",

        "devdbg_locFrom0OfFirstInsValueInSP._value",
        "devdbg_locFrom0OfFirstInsValueInSP._bValid"
    };

    if ( section < qvectStrHeaderTitle.size()) {
        return(qvectStrHeaderTitle[section]);
    }
    return(qvariantEmpty);
}


void ComputationCore_inheritQATableModel::send_computationParametersToView() {

    qDebug() << __FUNCTION__ << "at start: _computationParameters._eBaseComputationMethod = " << _computationParameters._eBaseComputationMethod;
    qDebug() << __FUNCTION__ << "at start: _computationParameters._ePixelExtractionMethod = " << _computationParameters._ePixelExtractionMethod;



    qDebug() << __FUNCTION__ << " @ " <<__LINE__ ;
    _correlationScoreMapFileAvailabilityFlags.showContent();//@#LP debug purpose
    _inputFiles.showContent();//@#LP debug purpose

    //@#LP check how enum survive through signal/slot/qvariant
    emit signal_computationParameters(
                _computationParameters,
                _correlationScoreMapFileAvailabilityFlags,
                _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ);
}


void ComputationCore_inheritQATableModel::send_vectComputedComponentsInUse_toView() {

    qDebug() << "_qvectb_componentsToCompute_Perp_Parall_DeltaZ = " << _qvectb_componentsToCompute_Perp_Parall_DeltaZ;
    emit signal_vectComputedComponents(_qvectb_componentsToCompute_Perp_Parall_DeltaZ);
}


//consider the qlnklistBox and associated vect cleared before starting to load
bool ComputationCore_inheritQATableModel::stackedProfilBoxes_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfilBoxes,
                                                                            const QVector<bool>& qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                                                            QString& strDetailsAboutFail_projectLoad) {

    int qhashBASPWM_size_countBeforeInsertion_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

    if (qhashBASPWM_size_countBeforeInsertion_size) {
        strDetailsAboutFail_projectLoad = "dev error #105: qlinkedlistBASPWM is not empty";
        return(false);
    }

    //brut way; all cells for now:

    QModelIndex qmodelIndexForInsert = index(qhashBASPWM_size_countBeforeInsertion_size, 0);

    emit layoutAboutToBeChanged();

    bool bReport = ComputationCore::stackedProfilBoxes_fromQJsonArray(qjsonArray_stackedProfilBoxes,
                                                                      qvectBool_ComponentToSetToDefault_Perp_Parall_deltaZ,
                                                                      strDetailsAboutFail_projectLoad);
    if (!bReport) {
        emit layoutChanged();
        clear();
        return(false);
    }

    int qhashBASPWM_size_CountAfterInsertionWillBe = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.count();

    int lastIndexWillBe = qhashBASPWM_size_CountAfterInsertionWillBe-1;

    beginInsertRows(qmodelIndexForInsert, qhashBASPWM_size_countBeforeInsertion_size, lastIndexWillBe);

    endInsertRows();

    emit layoutChanged();

    emit signal_oneOrMoreBoxesExist((qhashBASPWM_size_CountAfterInsertionWillBe != 0));

    return(true);
}

bool ComputationCore_inheritQATableModel::feed_stackedProfil_fromQJsonArray(const QJsonArray& qjsonArray_stackedProfil,
                                                                           const QVector<bool>& qvectb_componentToCompute,
                                                                           QString& strDetailsAboutFail_projectLoad) {
    //@@LP if return false, have to call:
    //     _qlnklistBoxAndStackedProfilWithMeasurements.clear();
    //     and
    //     .clear();

    bool bReport = ComputationCore::feed_stackedProfil_fromQJsonArray(qjsonArray_stackedProfil,
                                                                      qvectb_componentToCompute,
                                                                      strDetailsAboutFail_projectLoad);

    if (bReport) {

        //brut way; all cells for now:

        int qhashBASPWM_size = _qmap_insertionBoxId_BoxAndStackedProfilWithMeasurements.size();

        if (!qhashBASPWM_size) {
            qDebug() << __FUNCTION__ << "ComputationCore_inheritQATableModel _qlnklistBoxAndStackedProfilWithMeasurements is empty";
            qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
        }

        if (qhashBASPWM_size) {

            QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft = index(0, 0);
            QModelIndex qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight= index(qhashBASPWM_size-1, e_SPTVC_lastColumn);

            emit dataChanged(qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteTopLeft,
                             qmodelIndexFor_setDefaultMinMaxCenterValues_stakedProfilsAlongRouteBottomRight);
        }
    }

    return(bReport);
}

void ComputationCore_inheritQATableModel::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {

    qDebug() << __FUNCTION__  << "selected  :" << selected;
    qDebug() << __FUNCTION__  << "deselected:" << deselected;

    int indexBoxInVectOrderAlongRoute = selected.indexes().first().row();

    if (!vectBoxIdValid(/*_*/indexBoxInVectOrderAlongRoute)) {
        return;
    }
    int indexBoxAtQMapInsertion = _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(/*_*/indexBoxInVectOrderAlongRoute)._insertionIndex;

    qDebug() << __FUNCTION__  << "emit now signal_selectionBoxChanged(" << indexBoxInVectOrderAlongRoute << ", " << indexBoxAtQMapInsertion;
    emit signal_selectionBoxChanged(indexBoxInVectOrderAlongRoute, indexBoxAtQMapInsertion);
}


bool ComputationCore_inheritQATableModel::setLength_ofOneProfilsBox(int vectBoxId, int oddPixelLength) {

    if (!vectBoxIdValid(vectBoxId)) {
        return(false);
    }

    bool bReport = ComputationCore::setLength_ofOneProfilsBox(vectBoxId, oddPixelLength);

    if (bReport) {

        QModelIndex qmodelIndexFor_setLength_TopLeft = index(vectBoxId, e_SPTVC_oddPixelLength);
        QModelIndex qmodelIndexFor_setLength_BottomRight  = index(vectBoxId, e_SPTVC_oddPixelLength+1);

        emit dataChanged(qmodelIndexFor_setLength_TopLeft,
                         qmodelIndexFor_setLength_BottomRight);

        emit signal_profilsBoxChanged(vectBoxId, _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectBoxId)._insertionIndex/*, oddPixelWidth, oddPixelLength*/);
    }

    return(true);
}

bool ComputationCore_inheritQATableModel::updateProfilsBoxParameters_noBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters) {

    if (!vectBoxIdValid(vectBoxId)) {
        qDebug() << "invalid vectBoxId: " << vectBoxId;
        return(false);
    }

    bool bReport = ComputationCore::updateProfilsBoxParameters_noBoxMove(vectBoxId, profilsBoxParameters);

    if (bReport) {

        QModelIndex qmodelIndexFor_setLength_TopLeft = index(vectBoxId, 0);
        QModelIndex qmodelIndexFor_setLength_BottomRight  = index(vectBoxId, e_SPTVC_lastColumn);

        emit dataChanged(qmodelIndexFor_setLength_TopLeft,
                         qmodelIndexFor_setLength_BottomRight);



        qDebug() << __FUNCTION__  << "emit now signal_profilsBoxChanged(" << vectBoxId << ", " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectBoxId)._insertionIndex;
        emit signal_profilsBoxChanged(vectBoxId, _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(vectBoxId)._insertionIndex);
    }

    return(true);
}

bool ComputationCore_inheritQATableModel::updateProfilsBoxParameters_withBoxMove(int vectBoxId, const S_ProfilsBoxParameters& profilsBoxParameters, int& newVectBoxId_afterReorderAlongTheRoute) {

    qDebug() << __FUNCTION__ <<  "vectBoxId: " << vectBoxId;

    if (!vectBoxIdValid(vectBoxId)) {
        qDebug() << __FUNCTION__ <<  "invalid vectBoxId: " << vectBoxId;
        return(false);
    }

    qDebug() << __FUNCTION__ <<  "will replace with profilsBoxParameters: ";
    profilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();


    bool bReport = ComputationCore::updateProfilsBoxParameters_withBoxMove(vectBoxId, profilsBoxParameters, newVectBoxId_afterReorderAlongTheRoute);

    qDebug() << __FUNCTION__ << "newVectBoxId_afterReorderAlongTheRoute: " << newVectBoxId_afterReorderAlongTheRoute;

    if (!bReport) {
        qDebug() << __FUNCTION__ << " if (!bReport) {";
        return(false);
    }

    //find min and max for QModelIndex for model section update
    int min = vectBoxId;
    int max = newVectBoxId_afterReorderAlongTheRoute;
    if (max < min) {
        max = min;
        min = newVectBoxId_afterReorderAlongTheRoute;
    }
    qDebug() << __FUNCTION__ << "min: " << min;
    qDebug() << __FUNCTION__ << "max: " << max;

    QModelIndex qmodelIndexFor_setLength_TopLeft = index(min, 0);
    QModelIndex qmodelIndexFor_setLength_BottomRight  = index(max, e_SPTVC_lastColumn);

    emit dataChanged(qmodelIndexFor_setLength_TopLeft,
                     qmodelIndexFor_setLength_BottomRight);

    qDebug() << __FUNCTION__ << "_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();
    qDebug() << __FUNCTION__ <<  "_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size() = " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size();

    if (newVectBoxId_afterReorderAlongTheRoute >= _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.size()) {
        qDebug() << __FUNCTION__ <<  "dev intern error (newboxId >= qvect size";
        return(false);
    }

    qDebug() << "_qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(newVectBoxId_afterReorderAlongTheRoute)._insertionIndex = "
             << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(newVectBoxId_afterReorderAlongTheRoute)._insertionIndex;


    qDebug() << __FUNCTION__  << "emit now signal_selectionBoxChanged(" << newVectBoxId_afterReorderAlongTheRoute << ", " << _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(newVectBoxId_afterReorderAlongTheRoute)._insertionIndex;
    emit signal_selectionBoxChanged(newVectBoxId_afterReorderAlongTheRoute, _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(newVectBoxId_afterReorderAlongTheRoute)._insertionIndex);

    emit signal_profilsBoxChanged(newVectBoxId_afterReorderAlongTheRoute, _qvectOf_BASPWMPtr_orderedAlongRoute_syncWithTheQmap.at(newVectBoxId_afterReorderAlongTheRoute)._insertionIndex);

    return(true);
}

