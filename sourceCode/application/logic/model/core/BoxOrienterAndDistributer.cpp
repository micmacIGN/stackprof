#include <QDebug>
#include <QPointF>

#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "route.h"

#include "ComputationCore_inheritQATableModel.h"

#include "../../toolbox/toolbox_math_geometry.h"

#include "IDGenerator.h"

#include "BoxOrienterAndDistributer.h"

#include "../imageScene/S_ForbiddenRouteSection.h"

#include "../../toolbox/toolbox_conversion.h"

S_intBoolAndStrMsgStatus_aboutAutomaticDistribution::S_intBoolAndStrMsgStatus_aboutAutomaticDistribution():
    _nbBoxesCreated(0),
    _bNoneErrorOccured(false) {
}

void S_intBoolAndStrMsgStatus_aboutAutomaticDistribution::clear() {
    _nbBoxesCreated = 0;
    _bNoneErrorOccured = false;
    _strMessage.clear();
    _strMessage_details.clear();
}

BoxOrienterAndDistributer::BoxOrienterAndDistributer():
    _routePtr(nullptr),
    _automaticDistributionID(0),
    _bAutomaticDistributionID_valid(false),
    _idxBoxInSet(0),
    _eDBSBuAD_used(e_DBSBuDA_notSet) {
    _sibaStrMsgStatus_aboutAutomaticDistribution.clear();
}

void BoxOrienterAndDistributer::clear() {
    _routePtr = nullptr;

    _qvectProfBoxParameters_automatically_distributed.clear();

    _qvectQPointF_devdebug_centerBox.clear();
    _qvectQPointF_devdebug_firstProfil_centerBox.clear();
    _qvectQPointF_devdebug_firstPointAlongRoute.clear();
    _qvect_devdebug_unitVectorForEachPointAlongRoute.clear();
    _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute.clear();

    _qvect_unitVectorForEachPointAlongRoute.clear();

    _automaticDistributionID = 0;
    _bAutomaticDistributionID_valid = false;

    _idxBoxInSet = 0;
    _eDBSBuAD_used = e_DBSBuDA_notSet;

    _sibaStrMsgStatus_aboutAutomaticDistribution.clear();

}


bool BoxOrienterAndDistributer::get_automaticDistributionID(qint64& automaticDistributionID) {
    if (_bAutomaticDistributionID_valid) {
        automaticDistributionID = _automaticDistributionID;
    }
    return(_bAutomaticDistributionID_valid);
}

S_intBoolAndStrMsgStatus_aboutAutomaticDistribution BoxOrienterAndDistributer::get_sibaStrMsgStatus_aboutAutomaticDistribution() const {
    return(_sibaStrMsgStatus_aboutAutomaticDistribution);
}


bool BoxOrienterAndDistributer::computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
        Route *routePtr,
        int boxOddWidth, int boxOddLength,
        e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDBSBuAD_toUse,
        bool bOnlyComputeFirstBox,
        bool bFindCenterUsingLocationAdjustement,
        e_EvaluationResultAboutCenterBoxCandidateAlongRoute& e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered) {

    qDebug() << __FUNCTION__;

    e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = e_ERACBCAR_canNotFit; //default value for any error cases

    if (  (eDBSBuAD_toUse != e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel)
        &&(eDBSBuAD_toUse != e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel)) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #20";
        qDebug() << __FUNCTION__ << "Dev Error #20";
        qDebug() << __FUNCTION__ << "eDBSBuAD_toUse is invalid";
        return(false);
    }

    clear();

    _eDBSBuAD_used = eDBSBuAD_toUse;
    _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage = "Automatic distribution of Boxes";

    if ((boxOddWidth < 1)||(boxOddLength < 1)) { //@LP here we do not reject boxOddLength < 3

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #13";
        qDebug() << __FUNCTION__ << "Dev Error #13";
        return(false);
    }

    //@#LP considers box width at 1 as invalid. For width at 1, the algorithm needs to be evaluated if compatible with
    if (boxOddWidth == 1) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "boxOddWidth at 1 needs dev-test";
        return(false);
    }

    _routePtr = routePtr;

    //return the number of boxes created


    if (!_routePtr) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #14";
        return(false);
    }

    int nbSegment = _routePtr->segmentCount();
    if (!nbSegment) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #15";
        qDebug() << __FUNCTION__ << "error: if (!nbSegment) {";
        return(false);
    }

    //Considers that the content of the outside container which will receive the automatically generatered boxes
    //can have similar boxes that the generated boxes here (size, location, etc). This is to the user wich needs to know what he wants.
    //Hence, to remove boxes we can not rely on size and location along the route. A unique ID per box needs to exist.
    //Consequently, each box have an ID wich is a composition of :
    //. _automaticDistributionID
    //. box index in vector _qvectProfBoxParameters_automatically_distributed
    //The _automaticDistributionID is generated at each method call and use current msec since 1970.
    //Hence, each box generated by any call to this method will be unique
    //And consequently, any insertion of BoxAndStackedProfilWithMeasurements in _qlnklistBoxAndStackedProfilWithMeasurements
    //needs to set a valid box id for each BoxAndStackedProfilWithMeasurements.

    _automaticDistributionID = IDGenerator::generateABoxesSetID();
    _bAutomaticDistributionID_valid = true;


    int idxSegmentOfStartingPoint = 0;
    QPointF qpfStartingPoint =_routePtr->getSegment(idxSegmentOfStartingPoint)._ptA;

    unsigned int computationBoxId = 1;

    bool bEndOfRouteReachedBeforeDistanceReached = false;

    bool bFirstBox = true;

    while(1) { //sliding portion of route along the route

        qDebug() << __FUNCTION__<< "while(1) { // sliding portion of route along the route ";

        qDebug() << __FUNCTION__<< "idxSegmentOfStartingPoint: " << idxSegmentOfStartingPoint;
        qDebug() << __FUNCTION__<< "qpfStartingPoint         : " << qpfStartingPoint;

        bool bSucceed = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection(
                        qpfStartingPoint, idxSegmentOfStartingPoint, 44*boxOddWidth, bEndOfRouteReachedBeforeDistanceReached);


        if (!bSucceed) {
            qDebug() << __FUNCTION__ << "compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection said " << bSucceed;

            qDebug() << __FUNCTION__ << " compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection failed";

            return(false);
        }

        int distributedPointsCount = _qvect_unitVectorForEachPointAlongRoute.size();

        qDebug() << __FUNCTION__ << "distributedPointsCount = " << distributedPointsCount;

        if ( (boxOddWidth-1)*(static_cast<int>(_subDivisionOneUnitVectorAlongRoute)) > distributedPointsCount) {

            qDebug() << __FUNCTION__<< "route portion too short for one new box (distributedPointsCount = " << distributedPointsCount
                     << " boxOddWidth*(_intSubDivisionOneUnitVectorAlongRoute) = " << (boxOddWidth-1)*(static_cast<int>(_subDivisionOneUnitVectorAlongRoute));

            showContent_qvectProfBoxParameters_automatically_distributed();

            _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
            int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
            _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
            if (!nbBoxesCreated) {
                _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
            } else {
                _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
            }
            return(nbBoxesCreated > 0);
        }

        int intWidthDiv2inSubDiv = ((boxOddWidth-1)*static_cast<int>(static_cast<int>(_subDivisionOneUnitVectorAlongRoute)))/2;

        qDebug() << __FUNCTION__ << "intWidthDiv2inSubDiv = " << intWidthDiv2inSubDiv;

        bool bBoxWasAdjusted = false;
        bool bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;
        int idxNextPointAvailableOnRoute = 0;


        int idxCenterPointOnRoute = intWidthDiv2inSubDiv;

        Vec2<double> meanDirectionVectorForBox {.0,.0};

        while(1) { //while box location adjustement is requiered

            qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute << "and idxNextPointAvailableOnRoute = " << idxNextPointAvailableOnRoute;

           auto e_ERACBCAR_value2IfNeedsAdjustement =
           evaluateCenterPointOnRouteAsCenterBoxCandidate(
               idxCenterPointOnRoute,
               boxOddWidth/2,
               intWidthDiv2inSubDiv,
               idxNextPointAvailableOnRoute,
               true, false, //check if idxNextPointAvailableOnRoute is first point of route

               //--------
               bDebugShow_bFirstAdjustement,
               boxOddWidth,
               boxOddLength,
               //--------

               false,
               meanDirectionVectorForBox);

            if (bFirstBox) {
                e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = e_ERACBCAR_value2IfNeedsAdjustement;
            }

            if  (!bFindCenterUsingLocationAdjustement) {
                break;
            }

            if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {

                //qDebug() << __FUNCTION__<< "@@@ going further @@@";
                bBoxWasAdjusted = true;                
                bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;

                idxCenterPointOnRoute++;

                if (idxCenterPointOnRoute >= _qvect_unitVectorForEachPointAlongRoute.size()) {
                    int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
                    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
                    if (!nbBoxesCreated) {
                        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
                    } else {
                        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
                    }
                    qDebug() << __FUNCTION__ << " compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection return now #1";
                    qDebug() << __FUNCTION__ << " compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection nbBoxesCreated = " << nbBoxesCreated;
                    return(nbBoxesCreated > 0);
                }

                qpfStartingPoint =_qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point;
                idxSegmentOfStartingPoint = _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._idxSegmentOwner;

                static bool bDebugJustFirtBox = true;
                if (bDebugJustFirtBox) {
                    bDebugJustFirtBox= false;
                    _qvect_devdebug_unitVectorForEachPointAlongRoute+=_qvect_unitVectorForEachPointAlongRoute;//.mid(0, idxNextPointAvailableOnRoute); //, (((boxOddWidth-1)/2)*_intSubDivisionOneUnitVectorAlongRoute));
                }

                continue;
            }

            if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute) {
                //qDebug() << __FUNCTION__<< "_@_ need to go closer _@_";
                if (idxCenterPointOnRoute == intWidthDiv2inSubDiv) {
                    //qDebug() << __FUNCTION__<< "can not proceed on the first box";
                } else {
                    //qDebug() << __FUNCTION__<< "@@@ going closer @@@";
                    bBoxWasAdjusted = true;
                    bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;

                    break;
                    //continue;
                }
            }//@##LP infinite looping between two case aboves not handled. But going closer should never occurs.
             //      Hence it's a dev-debug test, not an implementation correct behavior to detect too far box, due to the distance choosen method.
            break; //location found
        }

        bool bNextAvailablePointAlongRouteExists = false;
        if (!bOnlyComputeFirstBox) {
            bNextAvailablePointAlongRouteExists = findNextAvailablePointAlongRouteFrom(idxCenterPointOnRoute, boxOddWidth/2, meanDirectionVectorForBox, idxNextPointAvailableOnRoute);
        }

        //final box:
        S_ProfilsBoxParameters profilsBoxParameters {

                                                     {_automaticDistributionID, _idxBoxInSet},
                                                     {_eDBSBuAD_used},


                                                     _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point,

                                                     {meanDirectionVectorForBox.x, meanDirectionVectorForBox.y},
                                                      boxOddWidth,
                                                      boxOddLength,

                                                      _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._idxSegmentOwner,

                                                      bBoxWasAdjusted,
                                                      false, false
                                                   };
        bBoxWasAdjusted = false; //reset flag;

        //limit precision to be sync with saved data in project json file
        QPointF qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(profilsBoxParameters._unitVectorDirection, 7, qpf_cuttedOut);
        profilsBoxParameters._unitVectorDirection = qpf_cuttedOut;

        qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(profilsBoxParameters._qpfCenterPoint, 3, qpf_cuttedOut);
        profilsBoxParameters._qpfCenterPoint = qpf_cuttedOut;


        _qvectProfBoxParameters_automatically_distributed.push_back(profilsBoxParameters);

        _idxBoxInSet++;

        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._id;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._qpfCenterPoint;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._unitVectorDirection;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._oddPixelWidth;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._oddPixelLength;

       computationBoxId++;

       _qvectQPointF_devdebug_centerBox.push_back(_qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point);

       if (!bNextAvailablePointAlongRouteExists) {
           //qDebug() << __FUNCTION__<< "if (!bNextAvailablePointAlongRouteExists) {";
           break;
       }

       qpfStartingPoint =_qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._point;
       idxSegmentOfStartingPoint = _qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._idxSegmentOwner;

       if (bOnlyComputeFirstBox) {
           break;
       }
       bFirstBox = false;

    }

    showContent_qvectProfBoxParameters_automatically_distributed();

    _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
    int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
    if (!nbBoxesCreated) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
    } else {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
    }

    qDebug() << __FUNCTION__ << " compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection return now #2";
    qDebug() << __FUNCTION__ << " compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection nbBoxesCreated = " << nbBoxesCreated;

    return(nbBoxesCreated > 0);
}

const QVector<S_ProfilsBoxParameters>& BoxOrienterAndDistributer::getConstRef_qvectProfBoxParameters_automatically_distributed() const {
    return(_qvectProfBoxParameters_automatically_distributed);
}


void BoxOrienterAndDistributer::showContent_qvectProfBoxParameters_automatically_distributed() {
    qDebug() << __FUNCTION__ << "_bAutomaticDistributionID_valid = " << _bAutomaticDistributionID_valid;
    qDebug() << __FUNCTION__ << "_automaticDistributionID = " << _automaticDistributionID;

    qDebug() << __FUNCTION__ << "_qvect..._automatically_distributed.count() = " << _qvectProfBoxParameters_automatically_distributed.count();

    for (const auto& iter: _qvectProfBoxParameters_automatically_distributed) {
        iter.showContent();
    }

    qDebug() << __FUNCTION__ << "end of method";
}


//to limit memory consumption, the point distribution along the route is made by 'sliding portion'.
//'distanceToReach' just have to be large enough to permit to compute enough points, to be able to estimate and adjust the profil box along the route.
//We consider that eleven * the box width should be far enough.
//@LP used by the caller for now: '44*box width' to be super very large enough
bool BoxOrienterAndDistributer::compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection(
        QPointF qpfStartingPoint,
        int idxSegmentOfStartingPoint,
        double distanceToReach,
        bool& bEndOfRouteReached_beforeDistanceReached) {

    _qvect_unitVectorForEachPointAlongRoute.clear();

    double distanceToChek_unused = distanceToReach * 2;
    bool bEndOfRouteReached_beforeDistanceToChek_unused = false;

    return(compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
        qpfStartingPoint, idxSegmentOfStartingPoint, distanceToReach,
        distanceToChek_unused,
        bEndOfRouteReached_beforeDistanceReached,
        bEndOfRouteReached_beforeDistanceToChek_unused));
}



bool BoxOrienterAndDistributer::compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
        QPointF qpfStartingPoint,
        int idxSegmentOfStartingPoint,
        double distanceToReach,
        double distanceToCheck, //typically boxWidth/2 used to know if there is enough points from the starting point
        bool& bEndOfRouteReached_beforeDistanceToReach,
        bool& bEndOfRouteReached_beforeDistanceToCheck) {

    qDebug() << __FUNCTION__ << " entering method";

    if (!_routePtr) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #16";
        qDebug() << __FUNCTION__ << "error: if (!_routePtr) {";
        return(false);
    }

    int nbSegment = _routePtr->segmentCount();
    if (!nbSegment) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #17";
        qDebug() << __FUNCTION__ << "error: if (!nbSegment) {";
        return(false);
    }

    if ((idxSegmentOfStartingPoint < 0)||(idxSegmentOfStartingPoint >= nbSegment)) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #18";
        qDebug() << __FUNCTION__ << "error: idxSegment out of range";
        return(false);
    }

    bEndOfRouteReached_beforeDistanceToReach = false;
    bEndOfRouteReached_beforeDistanceToCheck = false;

    //feed a points list with direction unit vector at each point just looking at the next point in the route
    //and distance between points along the route is 1.0 (the residual at end of a segment is used to find the next point in the next segment)

    //qDebug() << __FUNCTION__ << "* Points list with direction unit vector at each point just looking at the next point in the route:";

    //devcheck that the distance between two successives point in the route, along the route is 1.0
    //(the euclidian distance between successive points which are on different segments are not to be 1.0 as each segment has it own direction)
    qreal devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment = .0;
    qreal devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment = .0;

    double residualDistanceAtSegmentEnd = .0;
    bool bPreviousSegmentExists = false;

    double distanceAlongRouteFromStartingPoint = 0;

    for (int idxSegment = idxSegmentOfStartingPoint; idxSegment < nbSegment; idxSegment++) {

        S_Segment CurrentSegment = _routePtr->getSegment(idxSegment);

        //qDebug() << __FUNCTION__ << "PtA: " << CurrentSegment._ptA;

        //qDebug() << __FUNCTION__ << "PtB: " << CurrentSegment._ptB;
        //for conviencience, we call the StartingPoint 'C' here:
        Vec2<double> vecCB {
            CurrentSegment._ptB.x() - qpfStartingPoint.x(),
            CurrentSegment._ptB.y() - qpfStartingPoint.y()
        };

        //qDebug() << __FUNCTION__ << "vecAB: " << vecAB.x << ", " << vecAB.y;

        Vec2<double> vecCBNormalized = vecCB.normalized();
        Vec2<double> vecCBNormalizedSubDivided = {
            vecCBNormalized.x/_subDivisionOneUnitVectorAlongRoute,
            vecCBNormalized.y/_subDivisionOneUnitVectorAlongRoute
        };
        //qDebug() << __FUNCTION__ << "vectABNormalized: " << vecABNormalized.x << ", " << vecABNormalized.y;


        double segmentCBlength = vecCB.length();
        Vec2<double> pointProgressingAlongSegment { qpfStartingPoint.x(),
                                                    qpfStartingPoint.y()};

        //qDebug() << __FUNCTION__ << "pointProgressingAlongSegment before adjustement:"
        //         << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;

        if (bPreviousSegmentExists) {
            Vec2<double> jumpVecToCompensateResidualAtEndOfPreviousSegment = ((1.0/_subDivisionOneUnitVectorAlongRoute) - residualDistanceAtSegmentEnd) * vecCBNormalized;
            pointProgressingAlongSegment+=jumpVecToCompensateResidualAtEndOfPreviousSegment;

            /*qDebug() << __FUNCTION__ << "jumpVectorToCompensateResidualAtEndOfPreviousSegment:"
                     << jumpVecToCompensateResidualAtEndOfPreviousSegment.x << ", " << jumpVecToCompensateResidualAtEndOfPreviousSegment.y;*/
        } else {
            //qDebug() << __FUNCTION__ << "no Previous Segment => no adjustement";
        }
        //qDebug() << __FUNCTION__ << "adjusted vec2dbProgressingAlongSegment:"
        //         << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;

        double distanceFromC = .0;
        if (bPreviousSegmentExists) {
            distanceFromC = (1.0/_subDivisionOneUnitVectorAlongRoute)-residualDistanceAtSegmentEnd;
        }
        //qDebug() << __FUNCTION__ << "initial distanceFromA: " << distanceFromA;


        bool bFirstPoint = true;
        for (; distanceFromC <= segmentCBlength ; distanceFromC+=(1.0/_subDivisionOneUnitVectorAlongRoute)) {

            //qDebug() << __FUNCTION__ << " pointProgressingAlongSegment =" << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;
            //qDebug() << __FUNCTION__ << "  distanceFromC = " << distanceFromC << " cmp to segmentCBlength =" << segmentCBlength;

            QPointF qpf {vecCBNormalized.x, vecCBNormalized.y};
            S_qpf_point_and_unitVectorDirection ptAndUvd {{pointProgressingAlongSegment.x, pointProgressingAlongSegment.y}, qpf, idxSegment};
            _qvect_unitVectorForEachPointAlongRoute.push_back(ptAndUvd);

            //qDebug() << __FUNCTION__ << "  added point = " << ptAndUvd._point;

            distanceAlongRouteFromStartingPoint+=(1.0/_subDivisionOneUnitVectorAlongRoute); //equ to +=vecCBNormalizedSubDivided.length();

            if (distanceAlongRouteFromStartingPoint >= distanceToReach) {
                //qDebug() << __FUNCTION__ << "  distanceAlongRouteFromStartingPoint reached: " << distanceAlongRouteFromStartingPoint;
                return(true);
            }

            //devcheck ---
            bool bConsecutivePointsInSameSegment = true;
            if (bFirstPoint) {
                bConsecutivePointsInSameSegment = false;
                if (!bPreviousSegmentExists) {
                    //qDebug() << "not applicable #1";
                } else {
                    Vec2<double> v_ptc { qpfStartingPoint.x(), qpfStartingPoint.y() };
                    Vec2<double> delta = pointProgressingAlongSegment - v_ptc;
                    devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment = delta.length();

                    qreal distanceAlongRouteBetweenPoint_throughEditedSegmentPoint = devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment + devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment;
                    //qDebug() << "distanceAlongRouteBetweenPoint_throughEditedSegmentPoint = " << distanceAlongRouteBetweenPoint_throughEditedSegmentPoint;
                }
            }

            if (distanceFromC+(1.0/_subDivisionOneUnitVectorAlongRoute) >= segmentCBlength) {
                Vec2<double> v_ptb { CurrentSegment._ptB.x(), CurrentSegment._ptB.y() };
                Vec2<double> delta = v_ptb - pointProgressingAlongSegment;
                devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment = delta.length();
            }

            if (bConsecutivePointsInSameSegment) {
                if (!bFirstPoint) {
                    int sizeVect = _qvect_unitVectorForEachPointAlongRoute.size();
                    Vec2<double> v_previousPoint {
                        _qvect_unitVectorForEachPointAlongRoute.at(sizeVect-2)._point.x(),
                        _qvect_unitVectorForEachPointAlongRoute.at(sizeVect-2)._point.y()
                    };

                    Vec2<double> delta = pointProgressingAlongSegment - v_previousPoint;
                    qreal distanceAlongRouteBetweenPoint_inSameSegment = delta.length();

                    //qDebug() << "distanceAlongRouteBetweenPoint_inSameSegment = " << distanceAlongRouteBetweenPoint_inSameSegment;

                } else {
                    //qDebug() << "not applicable #2";
                }
            }
            //---

            bFirstPoint = false;

            pointProgressingAlongSegment += vecCBNormalizedSubDivided;

        }


        /*qDebug() << __FUNCTION__ << "exiting loop with distanceFromC  :" << distanceFromC;
        qDebug() << __FUNCTION__ << "exiting loop with segmentCBlength:" << segmentCBlength;
        qDebug() << __FUNCTION__ << "distanceFromC - segmentCBlength = " << distanceFromC - segmentCBlength;*/

        //qDebug() << __FUNCTION__ << "distanceFromA after the loop along the segment:" << distanceFromA;
        residualDistanceAtSegmentEnd = (1.0/_subDivisionOneUnitVectorAlongRoute) - (distanceFromC - segmentCBlength);

        qDebug() << __FUNCTION__ << "residual distance: " << residualDistanceAtSegmentEnd;

        qpfStartingPoint = CurrentSegment._ptB; //link to next segment

        bPreviousSegmentExists = true;

    }


    /*qDebug() << __FUNCTION__ << "result list: _testdev_qvect_unitVectorForEachPointAlongRoute:";
    for(auto iter: _qvect_unitVectorForEachPointAlongRoute) {
        qDebug() << __FUNCTION__ << " pt: " << iter._point << " ; " << iter._unitVectorDirection;
    }*/

    /*
    //check: compute euclidian distance between point. Hence, not along route when point are not on the same segment:
    for(int idx = 0; idx < _testdev_qvect_unitVectorForEachPointAlongRoute.size()-1; idx++) {
        Vec2<double> pta {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx)._point.x(),
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx)._point.y()};

        Vec2<double> ptb {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx+1)._point.x(),
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx+1)._point.y()};

        Vec2<double> vectDist = ptb-pta;
        qreal distanceBetweenTwoSuccessivePoints =vectDist.length();
        qDebug() << __FUNCTION__ << " idx pt: " << idx << "; euclidian distance to next = " << distanceBetweenTwoSuccessivePoints;
    }
    */


    /*
    //test dev:
    //  go from start to end along the route and take into account the unit vectors around the location to compute the resulting vector direction
    //
    qDebug() << __FUNCTION__ << "* From start to end along the route and take into account the unit vectors around the location:";

    _testdev_qvect_MeanDirectionForEachPointAlongRoute.clear();

    //setting #1:
    int nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint = 0;
    int nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint = 1;

    //setting #2:
    //int nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint = 1;
    //int nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint = 1;


    int sizeVector = _testdev_qvect_unitVectorForEachPointAlongRoute.size();
    for(auto idxPt = nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint;
        idxPt < sizeVector-nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint;
        idxPt++) {

        qDebug() << __FUNCTION__ << "idxPt =" << idxPt;
        qDebug() << __FUNCTION__ << "  pt =" << _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._point;
        qDebug() << __FUNCTION__ << "  uvd=" << _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._unitVectorDirection;

        //int iVecRangeTook = 0; //tryed 2/3 1/3 when using current + next vector
        Vec2<double> vecResult { .0, .0};
        for (auto idxPtFromBeforeToAfter = idxPt - nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint;
                  idxPtFromBeforeToAfter <= idxPt + nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint;
                  idxPtFromBeforeToAfter++) {
            //if (idxPtFromBeforeToAfter == idxPt) { } //@#LP do not use the current point ? con: its unit vector contributes also to to the main direction.

            qDebug() << __FUNCTION__ << "      (previous value of vec2dbResult => " << vecResult.x << ", " << vecResult.y << ")";
            vecResult+= (2.0/3.0) * Vec2<double>{
                _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
                _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            };

            //
            //  if (iVecRangeTook == 0) {
            //    vecResult+= (2.0/3.0) * Vec2<double>{
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            //    };
            //}
            //if (iVecRangeTook == 1) {
            //    vecResult+= (1.0/3.0) * Vec2<double>{
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            //    };
            //}
            //iVecRangeTook++;

            qDebug() << __FUNCTION__ << "    idxPtFromBeforeToAfter =" << idxPtFromBeforeToAfter;
            qDebug() << __FUNCTION__ << "        vec2dbResult => " << vecResult.x << ", " << vecResult.y;

        }

        vecResult.normalize(); //@#LP add normalization as the debug manage only unit vector for now
        S_qpf_point_and_unitVectorDirection ptAndUvd {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._point,
            {vecResult.x, vecResult.y}
        };
        _testdev_qvect_MeanDirectionForEachPointAlongRoute.push_back(ptAndUvd);

    }

    qDebug() << __FUNCTION__ << "result list: _testdev_qvect_MeanDirectionForEachPointAlongRoute:";
    for(auto iter: _testdev_qvect_MeanDirectionForEachPointAlongRoute) {
        qDebug() << __FUNCTION__ << " pt: " << iter._point << " ; " << iter._unitVectorDirection;
    }
    */

    //qDebug() << __FUNCTION__ << "----------";

    bEndOfRouteReached_beforeDistanceToReach = false;
    bEndOfRouteReached_beforeDistanceToCheck = (distanceAlongRouteFromStartingPoint <= distanceToCheck);

    //qDebug() << __FUNCTION__ << "end of route reached (bEndOfRouteReached_beforeDistanceToReach set to (b):" << bEndOfRouteReached_beforeDistanceToReach;
    //qDebug() << __FUNCTION__ << "distanceAlongRouteFromStartingPoint = " << distanceAlongRouteFromStartingPoint;
    //qDebug() << __FUNCTION__ << "bEndOfRouteReached_beforeDistanceToCheck is (b): " << bEndOfRouteReached_beforeDistanceToCheck;

    return(true);
}



bool BoxOrienterAndDistributer::compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirection(
        QPointF qpfStartingPoint,
        int idxSegmentOfStartingPoint,
        double distanceToReach,
        bool& bStartOfRouteReached_beforeDistanceReached) {

    _qvect_unitVectorForEachPointAlongRoute.clear();

    double distanceToChek_unused = distanceToReach * 2;
    bool bStartOfRouteReached_beforeDistanceToChek_unused = false;

    return(compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
        qpfStartingPoint, idxSegmentOfStartingPoint, distanceToReach,
        distanceToChek_unused,
        bStartOfRouteReached_beforeDistanceReached,
        bStartOfRouteReached_beforeDistanceToChek_unused));
}

bool BoxOrienterAndDistributer::compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
        QPointF qpfStartingPoint,
        int idxSegmentOfStartingPoint,
        double distanceToReach,
        double distanceToCheck, //typically boxWidth/2 used to know if there is enough points from the starting point
        bool& bStartOfRouteReached_beforeDistanceToReach,
        bool& bStartOfRouteReached_beforeDistanceToCheck) {

    //qDebug() << __FUNCTION__ << "----------";

    if (!_routePtr) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #16";
        qDebug() << __FUNCTION__ << "error: if (!_routePtr) {";
        return(false);
    }

    int nbSegment = _routePtr->segmentCount();
    if (!nbSegment) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #17";
        qDebug() << __FUNCTION__ << "error: if (!nbSegment) {";
        return(false);
    }

    if ((idxSegmentOfStartingPoint < 0)||(idxSegmentOfStartingPoint >= nbSegment)) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #18";
        qDebug() << __FUNCTION__ << "error: idxSegment out of range";
        return(false);
    }

    bStartOfRouteReached_beforeDistanceToReach = false;
    bStartOfRouteReached_beforeDistanceToCheck = false;

    //feed a points list with direction unit vector at each point just looking at the next point in the route
    //and distance between points along the route is 1.0 (the residual at end of a segment is used to find the next point in the next segment)

    //qDebug() << __FUNCTION__ << "* Points list with direction unit vector at each point just looking at the next point in the route:";


    //devcheck that the distance between two successives point in the route, along the route is 1.0
    //(the euclidian distance between successive points which are on different segments are not to be 1.0 as each segment has it own direction)
    qreal devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment = .0;
    qreal devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment = .0;


    double residualDistanceAtSegmentEnd = .0;
    bool bPreviousSegmentExists = false;

    double distanceAlongRouteFromStartingPoint = 0;

    for (int idxSegment = idxSegmentOfStartingPoint; idxSegment >= 0; idxSegment--) {

        S_Segment CurrentSegment = _routePtr->getSegment(idxSegment);

        //qDebug() << __FUNCTION__ << "PtA: " << CurrentSegment._ptA;
        //qDebug() << __FUNCTION__ << "PtB: " << CurrentSegment._ptB;
        //for conviencience, we call the StartingPoint 'C' here:
        Vec2<double> vecCA {
            CurrentSegment._ptA.x() - qpfStartingPoint.x(),
            CurrentSegment._ptA.y() - qpfStartingPoint.y()
        };

        //qDebug() << __FUNCTION__ << "vecCA: " << vecCA.x << ", " << vecCA.y;

        Vec2<double> vecCANormalized = vecCA.normalized();
        Vec2<double> vecCANormalizedSubDivided = {
            vecCANormalized.x/_subDivisionOneUnitVectorAlongRoute,
            vecCANormalized.y/_subDivisionOneUnitVectorAlongRoute
        };
        //qDebug() << __FUNCTION__ << "vectABNormalized: " << vecABNormalized.x << ", " << vecABNormalized.y;

        double segmentCAlength = vecCA.length();
        Vec2<double> pointProgressingAlongSegment { qpfStartingPoint.x(),
                                                    qpfStartingPoint.y()};

        //qDebug() << __FUNCTION__ << "pointProgressingAlongSegment before adjustement:"
        //         << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;

        if (bPreviousSegmentExists) {
            Vec2<double> jumpVecToCompensateResidualAtEndOfPreviousSegment = ((1.0/_subDivisionOneUnitVectorAlongRoute) - residualDistanceAtSegmentEnd) * vecCANormalized;
            pointProgressingAlongSegment+=jumpVecToCompensateResidualAtEndOfPreviousSegment;
            //qDebug() << __FUNCTION__ << "jumpVectorToCompensateResidualAtEndOfPreviousSegment:"
            //     << jumpVecToCompensateResidualAtEndOfPreviousSegment.x << ", " << jumpVecToCompensateResidualAtEndOfPreviousSegment.y;
        } else {
            //qDebug() << __FUNCTION__ << "no Previous Segment => no adjustement";
        }
        //qDebug() << __FUNCTION__ << "adjusted vec2dbProgressingAlongSegment:"
        //         << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;

        double distanceFromC = .0;
        if (bPreviousSegmentExists) {
            distanceFromC = (1.0/_subDivisionOneUnitVectorAlongRoute)-residualDistanceAtSegmentEnd;
        }
        //qDebug() << __FUNCTION__ << "initial distanceFromA: " << distanceFromA;


        bool bFirstPoint = true;
        for (; distanceFromC <= segmentCAlength ; distanceFromC+=(1.0/_subDivisionOneUnitVectorAlongRoute)) {

            //qDebug() << __FUNCTION__ << " pointProgressingAlongSegment =" << pointProgressingAlongSegment.x << ", " << pointProgressingAlongSegment.y;
            //qDebug() << __FUNCTION__ << "  distanceFromC = " << distanceFromC << " cmp to segmentCAlength =" << distanceFromC;

            //@LP we progress backward but for each point we always store the unit vector direction to the segment point going forward
            QPointF qpf {-vecCANormalized.x, -vecCANormalized.y};
            S_qpf_point_and_unitVectorDirection ptAndUvd {{pointProgressingAlongSegment.x, pointProgressingAlongSegment.y}, qpf, idxSegment};
            _qvect_unitVectorForEachPointAlongRoute.push_back(ptAndUvd);

            distanceAlongRouteFromStartingPoint+=(1.0/_subDivisionOneUnitVectorAlongRoute); //equ to +=vecCANormalizedSubDivided.length();
            if (distanceAlongRouteFromStartingPoint >= distanceToReach) {
                qDebug() << __FUNCTION__ << "  distanceAlongRouteFromStartingPoint reached: " << distanceAlongRouteFromStartingPoint;
                return(true);
            }

            //qDebug() << __FUNCTION__ << "  added point = " << ptAndUvd._point;

            //devcheck ---
            bool bConsecutivePointsInSameSegment = true;
            if (bFirstPoint) {
                bConsecutivePointsInSameSegment = false;
                if (!bPreviousSegmentExists) {
                    //qDebug() << "not applicable #1";
                } else {
                    Vec2<double> v_ptc { qpfStartingPoint.x(), qpfStartingPoint.y() };
                    Vec2<double> delta = pointProgressingAlongSegment - v_ptc;
                    devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment = delta.length();

                    qreal distanceAlongRouteBetweenPoint_throughEditedSegmentPoint = devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment + devcheck_distanceFromFirstPointInSegmentToStartOfEditedSegment;
                    //qDebug() << "distanceAlongRouteBetweenPoint_throughEditedSegmentPoint = " << distanceAlongRouteBetweenPoint_throughEditedSegmentPoint;
                }
            }

            if (distanceFromC+(1.0/_subDivisionOneUnitVectorAlongRoute) >= segmentCAlength) {
                Vec2<double> v_pta { CurrentSegment._ptA.x(), CurrentSegment._ptA.y() };
                Vec2<double> delta = v_pta - pointProgressingAlongSegment;
                devcheck_distanceFromLastPointInSegmentToEndOfEditedSegment = delta.length();
            }

            if (bConsecutivePointsInSameSegment) {
                if (!bFirstPoint) {
                    int sizeVect = _qvect_unitVectorForEachPointAlongRoute.size();
                    Vec2<double> v_previousPoint {
                        _qvect_unitVectorForEachPointAlongRoute.at(sizeVect-2)._point.x(),
                        _qvect_unitVectorForEachPointAlongRoute.at(sizeVect-2)._point.y()
                    };
                    /*Vec2<double> v_lastPoint {
                        _testdev_qvect_unitVectorForEachPointAlongRoute.at(sizeVect-1)._point.x(),
                        _testdev_qvect_unitVectorForEachPointAlongRoute.at(sizeVect-1)._point.y()
                    };
                    Vec2<double> delta = v_lastPoint - v_previousPoint;
                    */
                    Vec2<double> delta = pointProgressingAlongSegment - v_previousPoint;
                    qreal distanceAlongRouteBetweenPoint_inSameSegment = delta.length();
                    //qDebug() << "distanceAlongRouteBetweenPoint_inSameSegment = " << distanceAlongRouteBetweenPoint_inSameSegment;

                } else {
                    //qDebug() << "not applicable #2";
                }
            }
            //---


            bFirstPoint = false;

            pointProgressingAlongSegment += vecCANormalizedSubDivided;

        }

        /*qDebug() << __FUNCTION__ << "exiting loop with distanceFromC  :" << distanceFromC;
        qDebug() << __FUNCTION__ << "exiting loop with segmentCAlength:" << segmentCAlength;
        qDebug() << __FUNCTION__ << "distanceFromC - segmentCAlength = " << distanceFromC - segmentCAlength;*/

        //qDebug() << __FUNCTION__ << "distanceFromA after the loop along the segment:" << distanceFromA;
        residualDistanceAtSegmentEnd = (1.0/_subDivisionOneUnitVectorAlongRoute) - (distanceFromC - segmentCAlength);
        //qDebug() << __FUNCTION__ << "residual distance: " << residualDistanceAtSegmentEnd;

        qpfStartingPoint = CurrentSegment._ptA; //link to next segment

        bPreviousSegmentExists = true;

    }

    /*qDebug() << __FUNCTION__ << "result list: _testdev_qvect_unitVectorForEachPointAlongRoute:";
    for(auto iter: _qvect_unitVectorForEachPointAlongRoute) {
        qDebug() << __FUNCTION__ << " pt: " << iter._point << " ; " << iter._unitVectorDirection;
    }*/

    /*
    //check: compute euclidian distance between point. Hence, not along route when point are not on the same segment:
    for(int idx = 0; idx < _testdev_qvect_unitVectorForEachPointAlongRoute.size()-1; idx++) {
        Vec2<double> pta {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx)._point.x(),
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx)._point.y()};

        Vec2<double> ptb {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx+1)._point.x(),
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idx+1)._point.y()};

        Vec2<double> vectDist = ptb-pta;
        qreal distanceBetweenTwoSuccessivePoints =vectDist.length();
        qDebug() << __FUNCTION__ << " idx pt: " << idx << "; euclidian distance to next = " << distanceBetweenTwoSuccessivePoints;
    }
    */

    /*
    //test dev:
    //  go from start to end along the route and take into account the unit vectors around the location to compute the resulting vector direction
    //
    qDebug() << __FUNCTION__ << "* From start to end along the route and take into account the unit vectors around the location:";

    _testdev_qvect_MeanDirectionForEachPointAlongRoute.clear();

    //setting #1:
    int nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint = 0;
    int nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint = 1;

    //setting #2:
    //int nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint = 1;
    //int nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint = 1;


    int sizeVector = _testdev_qvect_unitVectorForEachPointAlongRoute.size();
    for(auto idxPt = nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint;
        idxPt < sizeVector-nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint;
        idxPt++) {

        qDebug() << __FUNCTION__ << "idxPt =" << idxPt;
        qDebug() << __FUNCTION__ << "  pt =" << _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._point;
        qDebug() << __FUNCTION__ << "  uvd=" << _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._unitVectorDirection;

        //int iVecRangeTook = 0; //tryed 2/3 1/3 when using current + next vector
        Vec2<double> vecResult { .0, .0};
        for (auto idxPtFromBeforeToAfter = idxPt - nbUnitVectorDirectionToTakeIntoAccount_beforeCurrentPoint;
                  idxPtFromBeforeToAfter <= idxPt + nbUnitVectorDirectionToTakeIntoAccount_afterCurrentPoint;
                  idxPtFromBeforeToAfter++) {
            //if (idxPtFromBeforeToAfter == idxPt) { } //@#LP do not use the current point ? con: its unit vector contributes also to to the main direction.

            qDebug() << __FUNCTION__ << "      (previous value of vec2dbResult => " << vecResult.x << ", " << vecResult.y << ")";
            vecResult+= (2.0/3.0) * Vec2<double>{
                _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
                _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            };

            //
            //  if (iVecRangeTook == 0) {
            //    vecResult+= (2.0/3.0) * Vec2<double>{
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            //    };
            //}
            //if (iVecRangeTook == 1) {
            //    vecResult+= (1.0/3.0) * Vec2<double>{
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.x(),
            //        _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPtFromBeforeToAfter)._unitVectorDirection.y()
            //    };
            //}
            //iVecRangeTook++;

            qDebug() << __FUNCTION__ << "    idxPtFromBeforeToAfter =" << idxPtFromBeforeToAfter;
            qDebug() << __FUNCTION__ << "        vec2dbResult => " << vecResult.x << ", " << vecResult.y;

        }

        vecResult.normalize(); //@#LP add normalization as the debug manage only unit vector for now
        S_qpf_point_and_unitVectorDirection ptAndUvd {
            _testdev_qvect_unitVectorForEachPointAlongRoute.at(idxPt)._point,
            {vecResult.x, vecResult.y}
        };
        _testdev_qvect_MeanDirectionForEachPointAlongRoute.push_back(ptAndUvd);

    }

    qDebug() << __FUNCTION__ << "result list: _testdev_qvect_MeanDirectionForEachPointAlongRoute:";
    for(auto iter: _testdev_qvect_MeanDirectionForEachPointAlongRoute) {
        qDebug() << __FUNCTION__ << " pt: " << iter._point << " ; " << iter._unitVectorDirection;
    }
    */

    //qDebug() << __FUNCTION__ << "----------";

    qDebug() << __FUNCTION__ << "end of route reached (distanceAlongRouteFromStartingPoint not reached, distance from starting point =" << distanceAlongRouteFromStartingPoint;
    bStartOfRouteReached_beforeDistanceToReach = false;
    bStartOfRouteReached_beforeDistanceToCheck = (distanceAlongRouteFromStartingPoint <= distanceToCheck);

    return(true);
}


bool BoxOrienterAndDistributer::checkPointIsInsideOrientedRectBox(
    QPointF qpfCenterBox,
    Vec2<double> directionVectorForBox_normalized, //mean Direction Vector about the box must be normalized as checkPointInsideAOrientedRectBox method parameter
    double distance_BoxCenter_to_firstProfil, // = width box / 2.0
    double distance_boxCenter_to_lengthBorder, // = length box / 2.0
    QPointF qpfPointToTest,
    bool& bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2) {

    //distance between:
    //- the first available point along the route
    //- the projection of the point described above, on the perpendicular line to the direction vector, centered on the box center point

    //we compute the location of the first available point along the route in a coordinate system centered on the center point
    //
    Vec2<double> pointToProject = {
        qpfPointToTest.x() - qpfCenterBox.x(),
        qpfPointToTest.y() - qpfCenterBox.y()
    };

    qDebug() << __FUNCTION__ << "directionVectorForBox_normalized = " << directionVectorForBox_normalized.x << " , " << directionVectorForBox_normalized.y;
    qDebug() << __FUNCTION__ << "qpfCenterBox = " << qpfCenterBox;
    qDebug() << __FUNCTION__ << "qpfPointToTest = " << qpfPointToTest;
    qDebug() << __FUNCTION__ << " => pointToProject = " << pointToProject.x << " , " << pointToProject.y;

    //qDebug() << "  _ input vec:";
    //ShowAngleAboutVec(pointToProject);
    //reorient input:
    //qDebug() << "  _ reorient input:";
    Vec2<double> pointToProject_reoriented = reorientYToTop(pointToProject);
    //show inputvec_reoriented:
    //ShowAngleAboutVec(pointToProject_reoriented);

    Vec2<double> dstAxisVector = directionVectorForBox_normalized;
    //qDebug() << "  _ dstAxisVector:" << dstAxisVector.x << ", " << dstAxisVector.y;
    Vec2<double> vect_axisDestVector_reoriented = reorientYToTop(dstAxisVector);
    //qDebug() << "  _ vect_axisDestVector_reoriented:" << vect_axisDestVector_reoriented.x << ", " << vect_axisDestVector_reoriented.y;

    Vec2<double> projectedPointOnAxis = transformPointFromCoordinateSystemToAnother_rotationOfAxes(pointToProject_reoriented, vect_axisDestVector_reoriented);

    double distanceProjectedPointToXAxis = qAbs(projectedPointOnAxis.x);
    qDebug() << __FUNCTION__ << "distancePointToXAxis = " << distanceProjectedPointToXAxis;

    //distances difference permits to know where is located the first available point along the route: inside or outside the box
    //if this point:
    //- is inside: the box takes pixels before the point. The box needs to be adjusted as a correction
    //- is 'too much' outside the box: this should never occurs because the estimation consider the route as straight.
    //double deltaDistance = distance_BoxCenter_to_firstProfil - distanceProjectedPointToXAxis;

    //qDebug() << __FUNCTION__<< "deltaDistance = " << deltaDistance;

    if (distanceProjectedPointToXAxis < distance_BoxCenter_to_firstProfil) {

        //qDebug() << __FUNCTION__<< "detected: too close, needs to go further";


        qDebug() << __FUNCTION__ << "using projection: point to test is below width /2 to the center box";

        qDebug() << __FUNCTION__ << "if (distanceProjectedPointToXAxis < distance_BoxCenter_to_firstProfil) {";
        qDebug() << __FUNCTION__ << "   ( " << distanceProjectedPointToXAxis << " < " << distance_BoxCenter_to_firstProfil;

        bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2 = true;

        //distance between:
        //- the first available point along the route
        //- the projection of the point described above, on the direction vector axis, centered on the box center point

        double distanceProjectedPointToYAxis = qAbs(projectedPointOnAxis.y);

        qDebug() << __FUNCTION__ << "distancePointToYAxis = " << distanceProjectedPointToYAxis;

        //distances difference permits to know where is located the first available point along the route: inside or outside the box
        //if this point:
        //- is inside: the box takes pixels before the point. The box needs to be adjusted as a correction
        //- is 'too much' outside the box: this should never occurs because the estimation consider the route as straight.

        //qDebug() << __FUNCTION__<< "deltaDistance = " << deltaDistance;

        if (distanceProjectedPointToYAxis < distance_boxCenter_to_lengthBorder) {            
            qDebug() << __FUNCTION__ << "if (distanceProjectedPointToYAxis < distance_boxCenter_to_lengthBorder) {";
            qDebug() << __FUNCTION__ << "   ( " << distanceProjectedPointToYAxis << " < " << distance_boxCenter_to_lengthBorder;
            return(true);
        }
        qDebug() << __FUNCTION__ << "return(false) #1";
        return(false);
    }

    bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2 = false;
    qDebug() << __FUNCTION__ << "return(false) #2";
    return(false);
}

BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute BoxOrienterAndDistributer::evaluateCenterPointOnRouteAsCenterBoxCandidate(
        int idxCenterPointOnRoute, int intWidthDiv2, int intWidthDiv2inSubDiv,
        int idxNextPointAvailableOnRoute,
        bool bCheckIfNextPointAvailableOnRouteIsFirstPointOfRoute,
        bool bCheckIfNextPointAvailableOnRouteIsLasttPointOfRoute,
        //for debug purpose only:
        bool bDebugShow_bFirstAdjustement,
        int boxOddWidth,
        int boxOddLength,
        bool bCheckThatNexAvailablePointAlongTheRoute_isInRectBox,
        //-----------------------
        Vec2<double>& meanDirectionVectorForBox) {

    //compute the unit vector direction of the box computing the mean vector direction of
    //the unit vectors of each point along the route for width
    int idxFirstPointOnRouteForDirectionEstimation = idxCenterPointOnRoute - intWidthDiv2inSubDiv;
    int idxLastPointOnRouteForDirectionEstimation = idxCenterPointOnRoute + intWidthDiv2inSubDiv;

    qDebug() << __FUNCTION__<< "  idxFirstPointOnRouteForDirectionEstimation: " << idxFirstPointOnRouteForDirectionEstimation;
    qDebug() << __FUNCTION__<< "  idxLastPointOnRouteForDirectionEstimation : " << idxLastPointOnRouteForDirectionEstimation;

    qDebug() << __FUNCTION__<< "  _qvect_unitVectorForEachPointAlongRoute.size() = " << _qvect_unitVectorForEachPointAlongRoute.size();


    meanDirectionVectorForBox.x = .0;
    meanDirectionVectorForBox.y = .0;

    if (idxFirstPointOnRouteForDirectionEstimation < 0) {
        qDebug() << __FUNCTION__<< "if (idxFirstPointOnRouteForDirectionEstimation < 0) {";
        return(e_ERACBCAR_canNotFit);
    }
    if (idxLastPointOnRouteForDirectionEstimation >= _qvect_unitVectorForEachPointAlongRoute.size()) {
        qDebug() << __FUNCTION__<< "if (idxLastPointOnRouteForDirectionEstimation >= _qvect_unitVectorForEachPointAlongRoute.size()) {";
        return(e_ERACBCAR_canNotFit);
    }

    for (int idxInsideBox =  idxFirstPointOnRouteForDirectionEstimation;
             idxInsideBox <= idxLastPointOnRouteForDirectionEstimation;
         idxInsideBox++) {

        Vec2<double> unitVectorDirectionAtPoint {
            _qvect_unitVectorForEachPointAlongRoute.at(idxInsideBox)._unitVectorDirection.x(),
            _qvect_unitVectorForEachPointAlongRoute.at(idxInsideBox)._unitVectorDirection.y()
        };

        meanDirectionVectorForBox += unitVectorDirectionAtPoint;
    }

    meanDirectionVectorForBox.normalize();

    //at this step, the box center point along the route can be enough wrong to produce these errors:
    //- the first profil of the box can be before the start of the route (if it's the first box computed)
    //- the first profil of the box can overlap with the previous computed box along the route
    //This is typical when the route has curves as the estimation considers the route as straight

    //test if the first profil of the box is before or not the start of the route


    //check that the nextpointavailable is first or last point of the route
    //if this is is one of them, check that the point is not inside the box using projection of the point on the two axis of the box direction axis

    QPointF qPointFToTest = _qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._point;

    qDebug() << __FUNCTION__<< "qPointFToTest = " << qPointFToTest;

    //removed recomputation, as this is simply width/2
    double distance_BoxCenter_to_firstProfil = static_cast<double>(intWidthDiv2);

    double distance_boxCenter_to_lengthBorder = static_cast<double>(boxOddLength/2);

    bool bNextPointAvailableOnRouteIsFirstPointOfRoute = false;
    bool bNextPointAvailableOnRouteIsLastPointOfRoute = false;

    if (bCheckIfNextPointAvailableOnRouteIsFirstPointOfRoute) {

        S_Segment segmentRouteFirstPoint_to_nextPointAvailableOnRoute;
        segmentRouteFirstPoint_to_nextPointAvailableOnRoute._ptA = _routePtr->getPoint(0);
        segmentRouteFirstPoint_to_nextPointAvailableOnRoute._ptB = _qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._point;
        segmentRouteFirstPoint_to_nextPointAvailableOnRoute._bValid = true;


        double distanceLimitABoutOnesubDivision = 1.0/_subDivisionOneUnitVectorAlongRoute;
        //nextpointavailble is the first point of the route
        if (segmentRouteFirstPoint_to_nextPointAvailableOnRoute.length() <= distanceLimitABoutOnesubDivision) {
            bNextPointAvailableOnRouteIsFirstPointOfRoute = true;
            qPointFToTest = _routePtr->getPoint(0);
        }
    }

    if (bCheckIfNextPointAvailableOnRouteIsLasttPointOfRoute) {

        S_Segment segmentRouteLastPoint_to_nextPointAvailableOnRoute;
        segmentRouteLastPoint_to_nextPointAvailableOnRoute._ptA = _routePtr->getPoint(_routePtr->pointCount()-1);
        segmentRouteLastPoint_to_nextPointAvailableOnRoute._ptB = _qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._point;
        segmentRouteLastPoint_to_nextPointAvailableOnRoute._bValid = true;

        double distanceLimitABoutOnesubDivision = 1.0/_subDivisionOneUnitVectorAlongRoute;
        //nextpointavailble is the first point of the route
        if (segmentRouteLastPoint_to_nextPointAvailableOnRoute.length() <= distanceLimitABoutOnesubDivision) {
            bNextPointAvailableOnRouteIsLastPointOfRoute = true;
            qPointFToTest = _routePtr->getPoint(_routePtr->pointCount()-1);
        }
    }

    bool bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2 = false;
    bool bNextPointAvailableOnRouteIsInsideRectOfBox = checkPointIsInsideOrientedRectBox(
        _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point,
        meanDirectionVectorForBox, //mean Direction Vector about the box must be normalized as checkPointInsideAOrientedRectBox method parameter
        distance_BoxCenter_to_firstProfil,
        distance_boxCenter_to_lengthBorder,
        qPointFToTest,
        bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2);

    bool bDecisionTakenAsBoxFit = false;
    if (bCheckIfNextPointAvailableOnRouteIsFirstPointOfRoute) {
        if (bNextPointAvailableOnRouteIsFirstPointOfRoute) {
            if (bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2) {
                return(e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute);
            } else {
                //box fit
                bDecisionTakenAsBoxFit = true;
            }
        } else {
            //do nothing
        }
    }

    if (bCheckIfNextPointAvailableOnRouteIsLasttPointOfRoute) {
        if (bNextPointAvailableOnRouteIsLastPointOfRoute) {
            if (bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2) {
                return(e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute); //this case is used when trying to fit from the end of the route. In this case the going further mean going in the first point direction
            } else {
                //box fit
                bDecisionTakenAsBoxFit = true;
            }
        } else {
            //do nothing
        }
    }

    if (!bDecisionTakenAsBoxFit) {
        if (bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2) {
            //to go further to the next point along the route
            return(e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute);
        } else {
            //box fit
            //bDecisionTakenAsBoxFit = true;
        }
    }

    return(e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute);

}

bool BoxOrienterAndDistributer::findNextAvailablePointAlongRouteFrom(
        int idxCenterPointOnRoute, int intWidthDiv2, Vec2<double> meanDirectionVectorForBox,
        int& idxNextAvailablePoint) {

    if (_eDBSBuAD_used == e_DBSBuDA_notSet) {
        //@LP no error msg
        return (false);
    }

    int distributedPointsCount = _qvect_unitVectorForEachPointAlongRoute.size();

    double distanceBetweenSuccessivesBoxes = sqrt(2.0);
    if (_eDBSBuAD_used == e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel) {
        distanceBetweenSuccessivesBoxes = sqrt(8.0);
    }

    //we do not know how the route direction change going to here and there. However, we consider the route never goes back if we consider three successive points.
    //Hence, the method is: progress step by step along the route to find the first location which is over the distance centerpoint <-> last profil center point
    //From this found point, we progress step by step along the route to let a distance of sqrt(2) (or just greater (due to the step by step way to progress along the route).
    //sqrt(2) is the distance which garanty that (in any direction the route go from a pixel), the pixel will be a different one than the pixel at the center point of the end profil.
    //This point is the new available point to try to locate a new box (this point is the new 'first available point' along the route)

    //@LP sqrt(1*1 + 1*1) is valid for none interpolation for pixel value extraction
    //@LP sqrt(2*2 + 2*2) is valid for bilinear 2x2 for pixel value extraction

    for (int idxPoint = idxCenterPointOnRoute+1; idxPoint < distributedPointsCount; idxPoint++) {

        //qDebug() << __FUNCTION__ << "test with idxPoint= " << idxPoint;

        Vec2<double> pointToProject = {
            _qvect_unitVectorForEachPointAlongRoute.at(idxPoint)._point.x() - _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point.x(),
            _qvect_unitVectorForEachPointAlongRoute.at(idxPoint)._point.y() - _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point.y()
        };

        //qDebug() << "  _ input vec:";
        //ShowAngleAboutVec(pointToProject);
        //reorient input:
        //qDebug() << "  _ reorient input:";
        Vec2<double> pointToProject_reoriented = reorientYToTop(pointToProject);
        //show inputvec_reoriented:
        //ShowAngleAboutVec(pointToProject_reoriented);

        Vec2<double> dstAxisVector = meanDirectionVectorForBox;
        //qDebug() << "  _ dstAxisVector:" << dstAxisVector.x << ", " << dstAxisVector.y;
        Vec2<double> vect_axisDestVector_reoriented = reorientYToTop(dstAxisVector);
        //qDebug() << "  _ vect_axisDestVector_reoriented:" << vect_axisDestVector_reoriented.x << ", " << vect_axisDestVector_reoriented.y;

        Vec2<double> projectedPointOnAxis = transformPointFromCoordinateSystemToAnother_rotationOfAxes(pointToProject_reoriented, vect_axisDestVector_reoriented);

        double distanceProjectedPointToXAxis = qAbs(projectedPointOnAxis.x);
        //qDebug() << "distanceProjectedPointToXAxis = " << distanceProjectedPointToXAxis;

        ////if (distanceProjectedPointToXAxis > intWidthDiv2) {
        ////    qDebug() << "found at: " << idxPoint;
        ////    idxNextAvailablePoint = idxPoint;
        ////    return(true);
        ////}

        double deltaDistance = distanceProjectedPointToXAxis - static_cast<double>(intWidthDiv2);
        qDebug() << __FUNCTION__<< "deltaDistance = " << deltaDistance;

        if (deltaDistance+0.0001 > .0) { // @LP instead of 'if (deltaDistance >= .0)'

            //qDebug() << "found first out point at: " << idxPoint;

             Vec2<double> startingPointToFindDistanceAs1 {
                _qvect_unitVectorForEachPointAlongRoute.at(idxPoint)._point.x(),
                _qvect_unitVectorForEachPointAlongRoute.at(idxPoint)._point.y()
            };

            //now along the route find the point at distance >1.0 from the first point outside the box
            for (int idxPointFindDistAs1 = idxPoint; idxPointFindDistAs1 < distributedPointsCount; idxPointFindDistAs1++) {
                Vec2<double> TestPointToFindDistanceAs1 {
                    _qvect_unitVectorForEachPointAlongRoute.at(idxPointFindDistAs1)._point.x(),
                    _qvect_unitVectorForEachPointAlongRoute.at(idxPointFindDistAs1)._point.y()
                };

                _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute.push_back(_qvect_unitVectorForEachPointAlongRoute.at(idxPointFindDistAs1)._point);

                Vec2<double> vecStartingToTesting = TestPointToFindDistanceAs1 - startingPointToFindDistanceAs1;
                double distanceST = vecStartingToTesting.length();

                qDebug() << "starting -> testing (" << idxPoint << "-> " << idxPointFindDistAs1 <<  ": distance = " << distanceST;
                qDebug() << "distanceST+0.0001 = " << distanceST+0.0001;

                if (distanceST+0.0001 >= distanceBetweenSuccessivesBoxes) { //@LP test about close value to 1.0 instead of 'if (deltaDistance >= 1.0f)'
                    idxNextAvailablePoint = idxPointFindDistAs1;
                    qDebug() << "  distanceST+0.0001 >= distanceBetweenSuccessivesBoxes => found idxNextAvailablePoint: " << idxNextAvailablePoint;
                    qDebug() << "( " << distanceST+0.0001 << " >= " << distanceBetweenSuccessivesBoxes;
                    return(true);
                }
            }
        }
    }
    //@LP no error msg
    return(false);
}


// backward means that the progression along the route goes to the starting point of the route
// forward means that the progression along the route goes to the ending point of the route
//@LP: be careful: this method:
//- return false only in case of error cases
//- return true in case of non error cases (including that the box doest not fit => bProfilBoxFit contains the status about the fit status)
bool BoxOrienterAndDistributer::checkThatABoxFromAGivenLocationAndWidthFitInRouteWithNoLocationChange(
    Route *routePtr, const S_ProfilsBoxParameters& profilBoxParameters, int wantedWidth,
    S_ProfilsBoxParameters& profilBoxParametersWithChanges,
    bool &bProfilBoxFit,
    bool &bProfilBoxWithChangesIncludeCenterPointRelocation) {

    qDebug() << __FUNCTION__ << ": entering";

    clear();
    _routePtr = routePtr;

    bProfilBoxFit = false;

    profilBoxParametersWithChanges = {};

    //algorithm below considers that wantedWidth / 2 is > 0
    if (wantedWidth < 3) { //@LP 1/2 = 0;  2/2 = 1 but we need wantedWidth to be an odd value, hence reject if below 3
        return(false);
    }

    int boxOddWidth = wantedWidth;
    QPointF qpfStartingPoint = profilBoxParameters._qpfCenterPoint;
    int idxSegmentOfStartingPoint = profilBoxParameters._idxSegmentOwnerOfCenterPoint;

    int boxOddWidthDiv2 = boxOddWidth/2;


    //[StartingPoint, pointBeforeStartingPoint#1, pointBeforeStartingPoint#2, ...] going backward
    bool bStartOfRouteReachedBeforeWidthMul22_backward = false;
    bool bStartOfRouteReachedBeforeWidthDiv2_backward = false;
    bool bSucceed_backward = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
                qpfStartingPoint, idxSegmentOfStartingPoint,
                boxOddWidth*22,
                boxOddWidthDiv2,
                bStartOfRouteReachedBeforeWidthMul22_backward,
                bStartOfRouteReachedBeforeWidthDiv2_backward);

    qDebug() << __FUNCTION__ << " after compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed";
    qDebug() << __FUNCTION__ << "bStartOfRouteReachedBeforeWidthMul22_backward = " << bStartOfRouteReachedBeforeWidthMul22_backward;
    qDebug() << __FUNCTION__ << "bStartOfRouteReachedBeforeWidthDiv2_backward = " << bStartOfRouteReachedBeforeWidthDiv2_backward;

    if (!bSucceed_backward) { //error cases        
        qDebug() << __FUNCTION__ << "if (!bSucceed_backward) {";
        return(false);
    }

    //reverse result to have: [..., pointBeforeStartingPoint#2, pointBeforeStartingPoint#1, StartingPoint]
    int ivectSize = _qvect_unitVectorForEachPointAlongRoute.size();
    QVector<S_qpf_point_and_unitVectorDirection> qvect_unitVectorForEachPointAlongRoute_reversed;
    qDebug() << __FUNCTION__ << "ivectSize = " << ivectSize;

    if (ivectSize) {
        qvect_unitVectorForEachPointAlongRoute_reversed.resize(ivectSize);
        //avoiding iterator usage
        int idxOnReversed = 0;
        for (int idxOnFeed = ivectSize-1; idxOnFeed >= 0 ; idxOnFeed--) {
            qvect_unitVectorForEachPointAlongRoute_reversed[idxOnReversed] = _qvect_unitVectorForEachPointAlongRoute[idxOnFeed];
            idxOnReversed++;
        }
        //remove the last point (startingPoint) as it will be added again in the next step
        qvect_unitVectorForEachPointAlongRoute_reversed.remove(ivectSize-1);
    }

    _qvect_unitVectorForEachPointAlongRoute = qvect_unitVectorForEachPointAlongRoute_reversed;
    int nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = _qvect_unitVectorForEachPointAlongRoute.size();

    bool bEndOfRouteReachedBeforeWidthMul22_forward = false;
    bool bEndOfRouteReachedBeforeWidthDiv2_forward = false;

    //append [StartingPoint, pointAfterStartingPoint#1, pointAfterStartingPoint#2, ...] going forward
    bool bSucceed_forward = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
                            qpfStartingPoint, idxSegmentOfStartingPoint,
                            boxOddWidth*22,
                            boxOddWidthDiv2,
                            bEndOfRouteReachedBeforeWidthMul22_forward,
                            bEndOfRouteReachedBeforeWidthDiv2_forward);

    qDebug() << __FUNCTION__ << " after compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed";
    qDebug() << __FUNCTION__ << "bEndOfRouteReachedBeforeWidthMul22_forward = " << bEndOfRouteReachedBeforeWidthMul22_forward;
    qDebug() << __FUNCTION__ << "bEndOfRouteReachedBeforeWidthDiv2_forward = " << bEndOfRouteReachedBeforeWidthDiv2_forward;

    if (!bSucceed_forward) { //error cases
        qDebug() << __FUNCTION__ << "if (!bSucceed_forward) {";
        return(false);
    }

    int qvect_unitVectorForEachPointAlongRoute_twoSides_size = _qvect_unitVectorForEachPointAlongRoute.size();

    int nbPoints_forward_fromJustAfterStartingPoint_toLast = qvect_unitVectorForEachPointAlongRoute_twoSides_size - nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint - 1;

    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = " << nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint;

    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast = " << nbPoints_forward_fromJustAfterStartingPoint_toLast;

    qDebug() << __FUNCTION__ << "qvect_unitVectorForEachPointAlongRoute_twoSides_size = " << qvect_unitVectorForEachPointAlongRoute_twoSides_size;

    //here we have in _qvect_unitVectorForEachPointAlongRoute:
    //[..., pointBeforeStartingPoint#2, pointBeforeStartingPoint#1, StartingPoint, pointAfterStartingPoint#1, pointAfterStartingPoint#2, ...]

    qDebug() << __FUNCTION__ << "@@@ qvect_unitVectorForEachPointAlongRoute_twoSides:";
    qDebug() << "profilBoxParameters._qpfCenterPoint = qpfStartingPoint = " << qpfStartingPoint;
    qDebug() << "idxSegmentOfStartingPoint = " << idxSegmentOfStartingPoint;
    /*for (int i = 0; i < qvect_unitVectorForEachPointAlongRoute_twoSides_size; i++) {
        qDebug() << __FUNCTION__ << "i=" << i << ": " << _qvect_unitVectorForEachPointAlongRoute[i]._point << "segment owner: " << _qvect_unitVectorForEachPointAlongRoute[i]._idxSegmentOwner;
    }
    */

    qDebug() << __FUNCTION__ << "boxOddWidthDiv2 = " << boxOddWidthDiv2;

    qDebug() << __FUNCTION__ << "bEndOfRouteReachedBeforeWidthDiv2_forward = " << bEndOfRouteReachedBeforeWidthDiv2_forward;
    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast = " << nbPoints_forward_fromJustAfterStartingPoint_toLast;
    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute) = " <<
                nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute);

    qDebug() << __FUNCTION__ << "bStartOfRouteReachedBeforeWidthDiv2_backward = " << bStartOfRouteReachedBeforeWidthDiv2_backward;
    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = " << nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint;
    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute = " <<
                nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute);

    bool bAcceptAdjustementGoingForward  = (!bEndOfRouteReachedBeforeWidthDiv2_forward) && (nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute) > boxOddWidthDiv2);
    bool bAcceptAdjustementGoingBackWard = (!bStartOfRouteReachedBeforeWidthDiv2_backward) && (nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute) > boxOddWidthDiv2);

    if (  (!bAcceptAdjustementGoingForward)
        &&(!bAcceptAdjustementGoingBackWard)) {
        qDebug() << "no way to fit the box in the route, even with location adjustement backward or forward";

        return(true);
    }

    int idxCenterPointOnRoute = nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint /*+ 1*/;
    int intWidthDiv2inSubDiv = ((boxOddWidth-1)*static_cast<int>(_subDivisionOneUnitVectorAlongRoute))/2; //considers that boxOddWidth / 2 is > 0

    //qDebug() << __FUNCTION__ << "intWidthDiv2inSubDiv = " << intWidthDiv2inSubDiv;

    bool bBoxWasAdjusted = false;
    bool bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;
    int idxNextPointAvailableOnRoute = 0;

    Vec2<double> meanDirectionVectorForBox {.0,.0};

    int boxOddLength = profilBoxParameters._oddPixelLength; //used for debug only

    //if not enough space in one direction, a new try has to be done to check that a move in the opposite direction is possible with the space
    //available in this direction

    //qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute;

    qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute << "and idxNextPointAvailableOnRoute = " << idxNextPointAvailableOnRoute;

    auto e_ERACBCAR_value2IfNeedsAdjustement =
           evaluateCenterPointOnRouteAsCenterBoxCandidate(
               idxCenterPointOnRoute,
               boxOddWidthDiv2,
               intWidthDiv2inSubDiv,
               idxNextPointAvailableOnRoute,

               true, false, //check if idxNextPointAvailableOnRoute is first point of route

               //--------
               bDebugShow_bFirstAdjustement,
               boxOddWidth,
               boxOddLength,
               //--------
               true,//false
               meanDirectionVectorForBox);

    qDebug() << __FUNCTION__<< "checking start: e_ERACBCAR_value2IfNeedsAdjustement = " << e_ERACBCAR_value2IfNeedsAdjustement;

    if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_canNotFit) {
        qDebug() << __FUNCTION__<< " e_ERACBCAR_canNotFit ";      
        return(true); //profilBoxFit at false
    }

    //the first point of the route is inside the box.
    if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {
        qDebug() << __FUNCTION__<< " e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute";
        return(true); //profilBoxFit at false
    }

    if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute) {
        qDebug() << __FUNCTION__<< " e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute ";
    }

    //
    //
    //check now that the box does not go over the last point of the route
    //
    //

    idxNextPointAvailableOnRoute = qvect_unitVectorForEachPointAlongRoute_twoSides_size - 1; //idxCenterPointOnRoute - intWidthDiv2inSubDiv;

    Vec2<double> meanDirectionVectorForBox_again {.0,.0};

    qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute << "and idxNextPointAvailableOnRoute = " << idxNextPointAvailableOnRoute;

    auto e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd =
           evaluateCenterPointOnRouteAsCenterBoxCandidate(
               idxCenterPointOnRoute,
               boxOddWidthDiv2,
               intWidthDiv2inSubDiv,
               idxNextPointAvailableOnRoute,

               false, true, //check if idxNextPointAvailableOnRoute is last point of route

               //--------
               bDebugShow_bFirstAdjustement,
               boxOddWidth,
               boxOddLength,
               //--------
               true,//false
               meanDirectionVectorForBox_again);

    qDebug() << __FUNCTION__<< "checkingEnd: e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd = " << e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd;

    if (e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd == e_ERACBCAR_canNotFit) {
        qDebug() << __FUNCTION__<< "e_ERACBCAR_canNotFit ";
        return(true); //profilBoxFit at false;
    }

    if (e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {
        qDebug() << __FUNCTION__<< "e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute";
        return(true); //profilBoxFit at false;
   }

    if (e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd == e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute) {
        qDebug() << __FUNCTION__<< "e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute";
    }

    //the last point of the route is inside the box.
    if (e_ERACBCAR_value2IfNeedsAdjustement_checkingEnd == e_ERACBCAR_fit) {
        qDebug() << __FUNCTION__<< " e_ERACBCAR_fit ";
    }

    //the box fits

    //final box:

    profilBoxParametersWithChanges = profilBoxParameters;
    profilBoxParametersWithChanges._oddPixelWidth = wantedWidth;
    profilBoxParametersWithChanges._unitVectorDirection = { meanDirectionVectorForBox.x, meanDirectionVectorForBox.y };

    //remove link to any automatic distribution if any:
    profilBoxParametersWithChanges._sUniqueBoxID._setID = IDGenerator::generateABoxesSetID();
    profilBoxParametersWithChanges._sUniqueBoxID._idxBoxInSet = 0;

    profilBoxParametersWithChanges._sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid = e_DBSBuDA_notSet;

    profilBoxParametersWithChanges._bDevDebug_wasAdjusted = false;
    profilBoxParametersWithChanges._bDevDebug_tooFarBox_rejected_needGoCloser = false;
    profilBoxParametersWithChanges._bDevDebug_tooCloseBox_rejected_needGoFurther = false;

    bProfilBoxWithChangesIncludeCenterPointRelocation = false;


    //limit precision to be sync with saved data in project json file
    QPointF qpf_cuttedOut = {.0,.0};
    qPointFtoNumberFixedPrecision(profilBoxParametersWithChanges._unitVectorDirection, 7, qpf_cuttedOut);
    profilBoxParametersWithChanges._unitVectorDirection = qpf_cuttedOut;

    qpf_cuttedOut = {.0,.0};
    qPointFtoNumberFixedPrecision(profilBoxParametersWithChanges._qpfCenterPoint, 3, qpf_cuttedOut);
    profilBoxParametersWithChanges._qpfCenterPoint = qpf_cuttedOut;

    bProfilBoxFit = true;

    return(true);
}

bool BoxOrienterAndDistributer::testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(
    Route *routePtr, const S_ProfilsBoxParameters& profilBoxParameters, const QVector<int>& qvectShiftToTest,

    QHash<int, bool>& qhash_shift_bPossibleShift,
    QHash<int, S_ProfilsBoxParametersForRelativeMoveAlongRoute>& qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute) {

    int qvectShiftToTest_size = qvectShiftToTest.size();
    if (!qvectShiftToTest_size) {
        qDebug() << __FUNCTION__ << "qvectShiftToTest_size = " << qvectShiftToTest_size;
        return(false);
    }

    clear();
    _routePtr = routePtr;

    int boxOddWidth = profilBoxParameters._oddPixelWidth;

    if (boxOddWidth < 3) {
        return(false);
    }

    double maxShiftBackward = *std::min_element(qvectShiftToTest.constBegin(), qvectShiftToTest.constEnd());
    double maxShiftForward  = *std::max_element(qvectShiftToTest.constBegin(), qvectShiftToTest.constEnd());

    //compute point along the route backward and forward, large enough to fit the box with the biggest shifts

    QPointF qpfStartingPoint = profilBoxParameters._qpfCenterPoint;
    int idxSegmentOfStartingPoint = profilBoxParameters._idxSegmentOwnerOfCenterPoint;

    int boxOddWidthDiv2 = boxOddWidth/2;

    //[StartingPoint, pointBeforeStartingPoint#1, pointBeforeStartingPoint#2, ...] going backward
    bool bStartOfRouteReachedBeforeMaxShiftBackWardPlusWidthMul22 = false;
    bool bStartOfRouteReachedBeforeWidthDiv2_backward = false;
    bool bEndOfRouteReachedBeforeMaxShiftForwardPlusWidthMul22 = false;
    bool bEndOfRouteReachedBeforeWidthDiv2_forward = false;

    double distanceToReach_backward = boxOddWidth*22 + qAbs(maxShiftBackward); //@LP abs(maxShiftBackward) because maxShiftBackward considered as a negative value
    double distanceToReach_forward  = boxOddWidth*22 + qAbs(maxShiftForward);  //@LP abs(maxShiftForward) because maxShiftForward could be negative if qvectShiftToTest contains only negative values
                                                                               //@LP (standard usage of qvectShiftToTest is: -20, -5, -1, +1, +5, +20 )

    bool bSucceed_backward = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
                qpfStartingPoint, idxSegmentOfStartingPoint,
                distanceToReach_backward,
                boxOddWidthDiv2,
                bStartOfRouteReachedBeforeMaxShiftBackWardPlusWidthMul22,
                bStartOfRouteReachedBeforeWidthDiv2_backward);
    if (!bSucceed_backward) { //error cases
        qDebug() << __FUNCTION__ << "if (!bSucceed_backward) {";
        return(false);
    }

    //reverse result to have: [..., pointBeforeStartingPoint#2, pointBeforeStartingPoint#1, StartingPoint]
    int ivectSize = _qvect_unitVectorForEachPointAlongRoute.size();
    QVector<S_qpf_point_and_unitVectorDirection> qvect_unitVectorForEachPointAlongRoute_reversed;
    qDebug() << __FUNCTION__ << "ivectSize = " << ivectSize;

    /*qDebug() << __FUNCTION__ << "@@@ after backwardDirectionFeed:";
    qDebug() << "profilBoxParameters._qpfCenterPoint = qpfStartingPoint = " << qpfStartingPoint;
    qDebug() << "idxSegmentOfStartingPoint = " << idxSegmentOfStartingPoint;
    for (int i = 0; i < ivectSize; i++) {
        qDebug() << __FUNCTION__ << "i=" << i << ": " << _qvect_unitVectorForEachPointAlongRoute[i]._point << "segment owner: " << _qvect_unitVectorForEachPointAlongRoute[i]._idxSegmentOwner;
    }
    qDebug() << __FUNCTION__;*/

    if (ivectSize) {
        qvect_unitVectorForEachPointAlongRoute_reversed.resize(ivectSize);
        //avoiding iterator usage
        int idxOnReversed = 0;
        for (int idxOnFeed = ivectSize-1; idxOnFeed >= 0 ; idxOnFeed--) {
            qvect_unitVectorForEachPointAlongRoute_reversed[idxOnReversed] = _qvect_unitVectorForEachPointAlongRoute[idxOnFeed];
            idxOnReversed++;
        }
        //remove the last point (startingPoint) as it will be added again in the next step
        qvect_unitVectorForEachPointAlongRoute_reversed.remove(ivectSize-1);
    }

    _qvect_unitVectorForEachPointAlongRoute = qvect_unitVectorForEachPointAlongRoute_reversed;
    int nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = _qvect_unitVectorForEachPointAlongRoute.size();

    /*qDebug() << __FUNCTION__ << "@@@ after reversed (with the last point (ie the startingPoint) removed):";
    qDebug() << "profilBoxParameters._qpfCenterPoint = qpfStartingPoint = " << qpfStartingPoint;
    qDebug() << "idxSegmentOfStartingPoint = " << idxSegmentOfStartingPoint;
    for (int i = 0; i < nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint; i++) {
        qDebug() << __FUNCTION__ << "i=" << i << ": " << _qvect_unitVectorForEachPointAlongRoute[i]._point << "segment owner: " << _qvect_unitVectorForEachPointAlongRoute[i]._idxSegmentOwner;
    }
    qDebug() << __FUNCTION__;*/


    //append [StartingPoint, pointAfterStartingPoint#1, pointAfterStartingPoint#2, ...] going forward
    bool bSucceed_forward = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
                            qpfStartingPoint, idxSegmentOfStartingPoint,
                            distanceToReach_forward,
                            boxOddWidthDiv2,
                            bEndOfRouteReachedBeforeMaxShiftForwardPlusWidthMul22,
                            bEndOfRouteReachedBeforeWidthDiv2_forward);

    if (!bSucceed_forward) { //error cases
        qDebug() << __FUNCTION__ << "if (!bSucceed_forward) {";
        return(false);
    }

    int qvect_unitVectorForEachPointAlongRoute_twoSides_size = _qvect_unitVectorForEachPointAlongRoute.size();

    int nbPoints_forward_fromJustAfterStartingPoint_toLast = qvect_unitVectorForEachPointAlongRoute_twoSides_size - nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint - 1;

    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = " << nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint;

    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast = " << nbPoints_forward_fromJustAfterStartingPoint_toLast;

    qDebug() << __FUNCTION__ << "qvect_unitVectorForEachPointAlongRoute_twoSides_size = " << qvect_unitVectorForEachPointAlongRoute_twoSides_size;

    //here we have in _qvect_unitVectorForEachPointAlongRoute:
    //[..., pointBeforeStartingPoint#2, pointBeforeStartingPoint#1, StartingPoint, pointAfterStartingPoint#1, pointAfterStartingPoint#2, ...]


    /*qDebug() << __FUNCTION__ << "@@@ after forwardDirectionFeed:";
    for (int i = 0; i < qvect_unitVectorForEachPointAlongRoute_twoSides_size; i++) {
        qDebug() << __FUNCTION__ << "i=" << i << ": " << _qvect_unitVectorForEachPointAlongRoute[i]._point << "segment owner: " << _qvect_unitVectorForEachPointAlongRoute[i]._idxSegmentOwner;
    }
    qDebug() << __FUNCTION__;*/

    qDebug() << __FUNCTION__ << "boxOddWidthDiv2 = " << boxOddWidthDiv2;

    qDebug() << __FUNCTION__ << "bEndOfRouteReachedBeforeWidthDiv2_forward = " << bEndOfRouteReachedBeforeWidthDiv2_forward;
    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast = " << nbPoints_forward_fromJustAfterStartingPoint_toLast;
    qDebug() << __FUNCTION__ << "nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute) = " <<
                nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute);

    qDebug() << __FUNCTION__ << "bStartOfRouteReachedBeforeWidthDiv2_backward = " << bStartOfRouteReachedBeforeWidthDiv2_backward;
    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint = " << nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint;
    qDebug() << __FUNCTION__ << "nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute = " <<
                nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute);

    bool bAcceptAdjustementGoingForward  = (!bEndOfRouteReachedBeforeWidthDiv2_forward) && (nbPoints_forward_fromJustAfterStartingPoint_toLast*(1.0/_subDivisionOneUnitVectorAlongRoute) > boxOddWidthDiv2);
    bool bAcceptAdjustementGoingBackWard = (!bStartOfRouteReachedBeforeWidthDiv2_backward) && (nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint*(1.0/_subDivisionOneUnitVectorAlongRoute) > boxOddWidthDiv2);

    if (  (!bAcceptAdjustementGoingForward)
        &&(!bAcceptAdjustementGoingBackWard)) {
        qDebug() << __FUNCTION__ << "no way to fit the box in the route; hence no way no move it also";

        return(true);
    }

    int idxCenterPointOnRoute = nbPoints_backward_fromFirst_toLastJustBeforeStartingPoint/* + 1*/;
    int intWidthDiv2inSubDiv = ((boxOddWidth-1)*static_cast<int>(_subDivisionOneUnitVectorAlongRoute))/2;

    for (int idxShift = 0; idxShift < qvectShiftToTest_size; idxShift++) {

        int shift = qvectShiftToTest[idxShift];
        qDebug() << "testing with shift as = " << shift;

        bool bBoxWasAdjusted = false;
        bool bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;
        int idxNextPointAvailableOnRoute = 0; //idxCenterPointOnRoute - intWidthDiv2inSubDiv;

        Vec2<double> meanDirectionVectorForBox {.0,.0};

        int boxOddLength = profilBoxParameters._oddPixelLength; //used for debug only

        //qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute << "and idxNextPointAvailableOnRoute = " << idxNextPointAvailableOnRoute;

        int idxCenterPointOnRouteToTest = idxCenterPointOnRoute + ( static_cast<int>(_subDivisionOneUnitVectorAlongRoute) * shift);

        qDebug() << "idxNextPointAvailableOnRoute (start of route) = " << idxNextPointAvailableOnRoute;
        qDebug() << "idxCenterPointOnRoute (of input box) = " << idxCenterPointOnRoute;
        qDebug() << "idxCenterPointOnRouteToTest (for shifted input box) = " << idxCenterPointOnRouteToTest;

        //out of route (point idx before start)
        if (idxCenterPointOnRouteToTest < 0) {
            //do nothing (_bPossible already set at false below)
            qDebug() << __FUNCTION__ << "idxCenterPointOnRouteToTest outside route (backward)";
            continue;
        }
        //out of route (point idx after the last point)
        if (idxCenterPointOnRouteToTest >= qvect_unitVectorForEachPointAlongRoute_twoSides_size) {
            //do nothing (_bPossible already set at false below)
            qDebug() << __FUNCTION__ << "idxCenterPointOnRouteToTest outside route (forward)";
            continue;
        }

        auto eEvaluationResult =
               evaluateCenterPointOnRouteAsCenterBoxCandidate(
                   idxCenterPointOnRouteToTest,
                   boxOddWidthDiv2,
                   intWidthDiv2inSubDiv,
                   idxNextPointAvailableOnRoute,

                   true, false, //check if idxNextPointAvailableOnRoute is first point of route

                   //--------
                   bDebugShow_bFirstAdjustement,
                   boxOddWidth,
                   boxOddLength,
                   //--------
                   true,//false
                   meanDirectionVectorForBox);

        qDebug() << __FUNCTION__ << "(testing start of route) eEvaluationResult = " << eEvaluationResult;

        if (eEvaluationResult == e_ERACBCAR_canNotFit) {
            qDebug() << __FUNCTION__ << "e_ERACBCAR_canNotFit";
            continue;
        }

        if (eEvaluationResult == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {
            qDebug() << __FUNCTION__ << "e_ERACBCAR_needsAdjustement_goingFurtherAlongTheRoute";
            continue;
        }

        //
        //
        //check now that the box does not go over the last point of the route
        //
        //

        //qDebug() << __FUNCTION__ << "intWidthDiv2inSubDiv = " << intWidthDiv2inSubDiv;

        idxNextPointAvailableOnRoute = qvect_unitVectorForEachPointAlongRoute_twoSides_size - 1;

        Vec2<double> meanDirectionVectorForBox_again {.0,.0};

        //int boxOddLength = profilBoxParameters._oddPixelLength; //used for debug only

        qDebug() << "idxNextPointAvailableOnRoute (end of route)= " << idxNextPointAvailableOnRoute;
        qDebug() << "idxCenterPointOnRoute (of input box) = " << idxCenterPointOnRoute;
        qDebug() << "idxCenterPointOnRouteToTest (for shifted input box) = " << idxCenterPointOnRouteToTest;

        auto eEvaluationResult_checkingEnd =
            evaluateCenterPointOnRouteAsCenterBoxCandidate(
               idxCenterPointOnRouteToTest,
               boxOddWidthDiv2,
               intWidthDiv2inSubDiv,
               idxNextPointAvailableOnRoute,

               false, true, //check if idxNextPointAvailableOnRoute is last point of route

               //--------
               bDebugShow_bFirstAdjustement,
               boxOddWidth,
               boxOddLength,
               //--------
               true,//false
               meanDirectionVectorForBox_again);

        qDebug() << __FUNCTION__ << "(testing end of route) eEvaluationResult = " << eEvaluationResult_checkingEnd;

        //the last point of the route is inside the box. Maybe with a center point location shift, going further (forward) the box could fit
        if (eEvaluationResult_checkingEnd == e_ERACBCAR_canNotFit) {
            qDebug() << __FUNCTION__ << "e_ERACBCAR_canNotFit";
            continue;
        }
        if (eEvaluationResult_checkingEnd == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {
            qDebug() << __FUNCTION__ << "e_ERACBCAR_needsAdjustement_goingFurtherAlongTheRoute";
            continue;
        }

        //the box fits

        qDebug() << __FUNCTION__ << "e_ERACBCAR_fit! :";

        qhash_shift_bPossibleShift.insert(shift, true);


        qDebug() << __FUNCTION__ << "qhash_shift_bPossibleShift.insert( "  << shift << ", " << true << ")";
        qDebug() << __FUNCTION__ << "qhash_shift_bPossibleShift content: " << qhash_shift_bPossibleShift;

        S_ProfilsBoxParametersForRelativeMoveAlongRoute sProfilsBoxParametersForRelativeMoveAlongRoute;

        sProfilsBoxParametersForRelativeMoveAlongRoute._bPossible = true;
        sProfilsBoxParametersForRelativeMoveAlongRoute._shiftFromInitialLocation = shift;

        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._oddPixelWidth  = profilBoxParameters._oddPixelWidth;
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._oddPixelLength = profilBoxParameters._oddPixelLength;

        //remove link to any automatic distribution if any:
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._sUniqueBoxID._setID = IDGenerator::generateABoxesSetID();
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._sUniqueBoxID._idxBoxInSet = 0;

        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._qpfCenterPoint = _qvect_unitVectorForEachPointAlongRoute[idxCenterPointOnRouteToTest]._point;
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._unitVectorDirection = { meanDirectionVectorForBox.x, meanDirectionVectorForBox.y };
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._idxSegmentOwnerOfCenterPoint = _qvect_unitVectorForEachPointAlongRoute[idxCenterPointOnRouteToTest]._idxSegmentOwner;
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._sInfoAboutAutomaticDistribution._e_DBSBuAD_usedAndStillValid = e_DBSBuDA_notSet;




        //limit precision to be sync with saved data in project json file
        QPointF qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._unitVectorDirection, 7, qpf_cuttedOut);
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._unitVectorDirection = qpf_cuttedOut;

        qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._qpfCenterPoint, 3, qpf_cuttedOut);
        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters._qpfCenterPoint = qpf_cuttedOut;

        qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute.insert(shift, sProfilsBoxParametersForRelativeMoveAlongRoute);

        sProfilsBoxParametersForRelativeMoveAlongRoute._profilsBoxParameters.showContent();

        //qDebug() << __FUNCTION__ << "qvectbPossibleShift[iShift] =" << qvectbPossibleShift[idxShift];
        //qDebug() << __FUNCTION__ << "_bPossible =" << qvectSProfilsBoxParametersForRelativeMoveAlongRoute[idxShift]._bPossible;
        //qDebug() << __FUNCTION__ << "_shiftFromInitialLocation = " << qvectSProfilsBoxParametersForRelativeMoveAlongRoute[idxShift]._shiftFromInitialLocation;

        //qvectSProfilsBoxParametersForRelativeMoveAlongRoute[idxShift]._profilsBoxParameters.showContent();

    }

    return(true);
}


bool BoxOrienterAndDistributer::makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(Route *routePtr,
                                                                                            QPointF qpfCenterPoint,
                                                                                            int idxSegmentOwner,
                                                                                            int boxOddWidth, int boxOddLength,
                                                                                            S_ProfilsBoxParameters& profilsBoxParameters) {

    //@#LP 1 or 3 as a min limit for box width ?
    if (boxOddWidth < 1 ) {
        return(false);
    }
    if (boxOddLength < 1 ) {
        return(false);
    }

    clear();
    _routePtr = routePtr;
    if (!_routePtr) {
        return(false);
    }

    bool bEndOfRouteReachedBeforeDistanceReached = false;
    bool bSucceed = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection(
                    qpfCenterPoint, idxSegmentOwner, 44*boxOddWidth, bEndOfRouteReachedBeforeDistanceReached);

    if (!bSucceed) {
        return(false);
    }

    QPointF relocatedPoint { .0, .0};
    e_ResultDetailsOfRelocatedPointInSegment eRDORPIS = relocatePointInSegment(qpfCenterPoint, _routePtr->getSegment(idxSegmentOwner), relocatedPoint);
    //points not aligned:
    if (eRDORPIS == e_RDORPIS_pointABCnotAligned) {//should never happen
        return(false);
    }

    //points over the segment //should never happen
    if (  (eRDORPIS == e_RDORPIS_notRelocated_AisCloser)
        ||(eRDORPIS == e_RDORPIS_notRelocated_BisCloser)) {
        return(false);
    }

    //e_RDORPIS_relocatedToA,
    //e_RDORPIS_relocatedToB,
    //e_RDORPIS_relocatedInsideSegment

    //@#LP use the input point, not the relocated to avoid accuracy location changes
    profilsBoxParameters._oddPixelWidth = boxOddWidth;
    profilsBoxParameters._oddPixelLength = boxOddLength;
    profilsBoxParameters._qpfCenterPoint = qpfCenterPoint;
    profilsBoxParameters._idxSegmentOwnerOfCenterPoint = idxSegmentOwner;

    S_ProfilsBoxParameters profilBoxParametersWithChanges;
    bool bProfilBoxFit = false;
    bool bProfilBoxWithChangesIncludeCenterPointRelocation = false;

    bool bNoErrorCase = checkThatABoxFromAGivenLocationAndWidthFitInRouteWithNoLocationChange(
                _routePtr, profilsBoxParameters, boxOddWidth,
                profilBoxParametersWithChanges,
                bProfilBoxFit,
                bProfilBoxWithChangesIncludeCenterPointRelocation);

    qDebug() << __FUNCTION__ << " bNoErrorCase  = " << bNoErrorCase;
    qDebug() << __FUNCTION__ << " bProfilBoxFit = " << bProfilBoxFit;

    if ((!bNoErrorCase)||(!bProfilBoxFit)) {
        qDebug() << __FUNCTION__ << "if ((!bNoErrorCase)||(!bProfilBoxFit)) {";
        return(false);
    }

    qDebug() << __FUNCTION__ << " considers box fitting";
    profilBoxParametersWithChanges.showContent();

    profilsBoxParameters = profilBoxParametersWithChanges;


    //limit precision to be sync with saved data in project json file
    QPointF qpf_cuttedOut = {.0,.0};
    qPointFtoNumberFixedPrecision(profilsBoxParameters._unitVectorDirection, 7, qpf_cuttedOut);
    profilsBoxParameters._unitVectorDirection = qpf_cuttedOut;

    qpf_cuttedOut = {.0,.0};
    qPointFtoNumberFixedPrecision(profilsBoxParameters._qpfCenterPoint, 3, qpf_cuttedOut);
    profilsBoxParameters._qpfCenterPoint = qpf_cuttedOut;

    return(true);
}


//@LP: the sliding window method can not detect that none box will fit along the route before the end
//Hence to find the first and last point, the usage ot the dichotomicMethod is prefered
bool BoxOrienterAndDistributer::findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingSlidingWindowMethod(
        Route *routePtr, int boxOddWidth, int boxOddLength,
        QPointF& qpfFirstPointAlongRoute, int& idxSegmentOwner_ofFirstPointAlongRoute,
        QPointF& qpflastPointAlongRoute, int& idxSegmentOwner_ofLastPointAlongRoute,
        bool& bFirstPointFeed, bool& bLastPointFeed) {

    qDebug() << __FUNCTION__;

    e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_notUsedReport;
    bool bFirstBoxFromFirstPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            true, //use adjustement
            e_ERACBCAR_notUsedReport);

    if (bFirstBoxFromFirstPointComputed) {
        qpfFirstPointAlongRoute = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
        idxSegmentOwner_ofFirstPointAlongRoute = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;
        bFirstPointFeed = true;
    }

    bool bFirstBoxFromLastPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            true, //use adjustement
            e_ERACBCAR_notUsedReport);

    if (bFirstBoxFromLastPointComputed) {
        qpflastPointAlongRoute = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
        idxSegmentOwner_ofLastPointAlongRoute = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;
        bLastPointFeed = true;
    }

    qDebug() << __FUNCTION__ << "bFirstBoxFromFirstPointComputed = " << bFirstBoxFromFirstPointComputed;
    qDebug() << __FUNCTION__ << "qpfFirstPointAlongRoute = " << qpfFirstPointAlongRoute;
    qDebug() << __FUNCTION__ << "idxSegmentOwner_ofFirstPointAlongRoute = " << idxSegmentOwner_ofFirstPointAlongRoute;
    qDebug() << __FUNCTION__ << "bFirstPointFeed = " << bFirstPointFeed;

    qDebug() << __FUNCTION__ << "bFirstBoxFromLastPointComputed  = " << bFirstBoxFromLastPointComputed;
    qDebug() << __FUNCTION__ << "qpflastPointAlongRoute = " << qpflastPointAlongRoute;
    qDebug() << __FUNCTION__ << "idxSegmentOwner_ofLastPointAlongRoute = " << idxSegmentOwner_ofLastPointAlongRoute;
    qDebug() << __FUNCTION__ << "bLastPointFeed = " << bLastPointFeed;

    return( bFirstPointFeed || bLastPointFeed );
}


bool BoxOrienterAndDistributer::computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
        Route *routePtr,
        int boxOddWidth, int boxOddLength,
        e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDBSBuAD_toUse,
        bool bOnlyComputeFirstBox,
        bool bFindCenterUsingLocationAdjustement,
        e_EvaluationResultAboutCenterBoxCandidateAlongRoute& e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered) {

    qDebug() << __FUNCTION__;

    e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = e_ERACBCAR_canNotFit; //default value for any error cases

    if (  (eDBSBuAD_toUse != e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel)
        &&(eDBSBuAD_toUse != e_DBSBuAD_square8_compatibleWithBiLinearInterpolation2x2ToGetPixel)) {
        //@#LP error msg
        return(false);
    }

    clear();

    _eDBSBuAD_used = eDBSBuAD_toUse;
    _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage = "Automatic distribution of Boxes";

    if ((boxOddWidth < 1)||(boxOddLength < 1)) { //@LP here we do not reject boxOddLength < 3

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #13";
        return(false);
    }

    //@#LP check that code behaves correctly with boxOddWidth at 1 or needs specific code
    if (boxOddWidth == 1) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "boxOddWidth at 1 needs dev-test";
        return(false);
    }

    _routePtr = routePtr;

    //return the number of boxes created

    if (!_routePtr) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #14";
        return(false);
    }

    int nbSegment = _routePtr->segmentCount();
    if (!nbSegment) {

        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Dev Error #15";
        qDebug() << __FUNCTION__ << "error: if (!nbSegment) {";
        return(false);
    }

    //Considers that the content of the outside container which will receive the automatically generatered boxes
    //can have similar boxes that the generated boxes here (size, location, etc). This is to the user wich needs to know what he wants.
    //Hence, to remove boxes we can not rely on size and location along the route. A unique ID per box needs to exist.
    //Consequently, each box have an ID wich is a composition of :
    //. _automaticDistributionID
    //. box index in vector _qvectProfBoxParameters_automatically_distributed
    //The _automaticDistributionID is generated at each method call and use current msec since 1970.
    //Hence, each box generated by any call to this method will be unique
    //And consequently, any insertion of BoxAndStackedProfilWithMeasurements in _qlnklistBoxAndStackedProfilWithMeasurements
    //needs to set a valid box id for each BoxAndStackedProfilWithMeasurements.
    //

    _automaticDistributionID = IDGenerator::generateABoxesSetID();
    _bAutomaticDistributionID_valid = true;

    int idxSegmentOfStartingPoint = _routePtr->segmentCount()-1;
    QPointF qpfStartingPoint =_routePtr->getSegment(idxSegmentOfStartingPoint)._ptB;

    unsigned int computationBoxId = 1;

    bool bStartOfRouteReachedBeforeDistanceReached = false;

    bool bFirstBox = true;

    while(1) { //sliding portion of route along the route

        qDebug() << __FUNCTION__<< "idxSegmentOfStartingPoint: " << idxSegmentOfStartingPoint;
        qDebug() << __FUNCTION__<< "qpfStartingPoint         : " << qpfStartingPoint;

        bool bSucceed = compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirection(
                qpfStartingPoint,
                idxSegmentOfStartingPoint,
                44*boxOddWidth,
                bStartOfRouteReachedBeforeDistanceReached);

        if (!bSucceed) {
            qDebug() << __FUNCTION__ << "error: in devtest_compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue()";

            return(false);
        }

        int distributedPointsCount = _qvect_unitVectorForEachPointAlongRoute.size();

        qDebug() << __FUNCTION__ << "distributedPointsCount = " << distributedPointsCount;

        if ( (boxOddWidth-1)*(static_cast<int>(_subDivisionOneUnitVectorAlongRoute)) > distributedPointsCount) {

            qDebug() << __FUNCTION__<< "route portion too short for one new box (distributedPointsCount = " << distributedPointsCount
                     << " boxOddWidth*(_intSubDivisionOneUnitVectorAlongRoute) = " << (boxOddWidth-1)*(static_cast<int>(_subDivisionOneUnitVectorAlongRoute));

            showContent_qvectProfBoxParameters_automatically_distributed();

            _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
            int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
            _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
            if (!nbBoxesCreated) {
                _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
            } else {
                _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
            }
            return(nbBoxesCreated > 0);
        }

        int intWidthDiv2inSubDiv = ((boxOddWidth-1)*static_cast<int>(static_cast<int>(_subDivisionOneUnitVectorAlongRoute)))/2;

        qDebug() << __FUNCTION__ << "intWidthDiv2inSubDiv = " << intWidthDiv2inSubDiv;

        bool bBoxWasAdjusted = false;
        bool bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;
        int idxNextPointAvailableOnRoute = 0;

        int idxCenterPointOnRoute = intWidthDiv2inSubDiv;

        Vec2<double> meanDirectionVectorForBox {.0,.0};

        while(1) { //while box location adjustement is requiered

            qDebug() << __FUNCTION__<< "will evaluate with idxCenterPointOnRoute: "<< idxCenterPointOnRoute << "and idxNextPointAvailableOnRoute = " << idxNextPointAvailableOnRoute;

           auto e_ERACBCAR_value2IfNeedsAdjustement =
           evaluateCenterPointOnRouteAsCenterBoxCandidate(
               idxCenterPointOnRoute,
               boxOddWidth/2,
               intWidthDiv2inSubDiv,
               idxNextPointAvailableOnRoute,

               false, true, //check if idxNextPointAvailableOnRoute is last point of route

               //--------
               bDebugShow_bFirstAdjustement,
               boxOddWidth,
               boxOddLength,
               //--------
               false,
               meanDirectionVectorForBox);

            if (bFirstBox) {
                e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = e_ERACBCAR_value2IfNeedsAdjustement;
            }

            if  (!bFindCenterUsingLocationAdjustement) {
                break;
            }

            if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute) {

                //qDebug() << __FUNCTION__<< "@@@ boxNeedsLocationAdjustementGoingFurtherAlongTheRoute @@@";
                bBoxWasAdjusted = true;
                bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;

                idxCenterPointOnRoute++;

                if (idxCenterPointOnRoute >= _qvect_unitVectorForEachPointAlongRoute.size()) {
                    int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
                    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
                    if (!nbBoxesCreated) {
                        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
                    } else {
                        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
                    }

                    return(nbBoxesCreated > 0);
                }

                qpfStartingPoint =_qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point;
                idxSegmentOfStartingPoint = _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._idxSegmentOwner;

                static bool bDebugJustFirtBox = true;
                if (bDebugJustFirtBox) {
                    bDebugJustFirtBox= false;
                    _qvect_devdebug_unitVectorForEachPointAlongRoute+=_qvect_unitVectorForEachPointAlongRoute;//.mid(0, idxNextPointAvailableOnRoute); //, (((boxOddWidth-1)/2)*_intSubDivisionOneUnitVectorAlongRoute));
                }

                continue;
            }

            if (e_ERACBCAR_value2IfNeedsAdjustement == e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute /*3*/) {

                qDebug() << __FUNCTION__<< "_@_ boxNeedsLocationAdjustementBecomingCloserAlongTheRoute _@_";

                if (idxCenterPointOnRoute == intWidthDiv2inSubDiv) {
                    //qDebug() << __FUNCTION__<< "can not proceed on the first box";
                } else {
                    //qDebug() << __FUNCTION__<< "@@@ going closer @@@";
                    bBoxWasAdjusted = true;
                    bDebugShow_bFirstAdjustement = !bBoxWasAdjusted;

                    break;
                    //continue;
                }
            }//@##LP infinite looping between two case aboves not handled. But going closer should never occurs.
             //      Hence it's a dev-debug test, not an implementation correct behavior to detect too far box, due to the distance choosen method.
            break; //location found
        }

        bool bNextAvailablePointAlongRouteExists = false;
        if (!bOnlyComputeFirstBox) {
            bNextAvailablePointAlongRouteExists = findNextAvailablePointAlongRouteFrom(idxCenterPointOnRoute, boxOddWidth/2, meanDirectionVectorForBox, idxNextPointAvailableOnRoute);
        }

        //final box:
        S_ProfilsBoxParameters profilsBoxParameters {

                                                     {_automaticDistributionID, _idxBoxInSet},
                                                     {_eDBSBuAD_used},


                                                     _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point,

                                                     {meanDirectionVectorForBox.x, meanDirectionVectorForBox.y},
                                                     //no need to reverse unit vector direction: the points along route distribution
                                                     //has already made it

                                                      boxOddWidth,
                                                      boxOddLength,

                                                      _qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._idxSegmentOwner,

                                                      bBoxWasAdjusted,
                                                      false, false
                                                   };
        bBoxWasAdjusted = false; //reset flag;


        //limit precision to be sync with saved data in project json file
        QPointF qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(profilsBoxParameters._unitVectorDirection, 7, qpf_cuttedOut);
        profilsBoxParameters._unitVectorDirection = qpf_cuttedOut;

        qpf_cuttedOut = {.0,.0};
        qPointFtoNumberFixedPrecision(profilsBoxParameters._qpfCenterPoint, 3, qpf_cuttedOut);
        profilsBoxParameters._qpfCenterPoint = qpf_cuttedOut;

        _qvectProfBoxParameters_automatically_distributed.push_back(/*S_BoxAndStackedProfilWithMeasurements(true, */profilsBoxParameters/*)*/);

        _idxBoxInSet++;

        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._id;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._qpfCenterPoint;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._unitVectorDirection;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._oddPixelWidth;
        //qDebug() << __FUNCTION__<< "added: " << cBox.getComputationBoxSettings()._oddPixelLength;

       computationBoxId++;

       _qvectQPointF_devdebug_centerBox.push_back(_qvect_unitVectorForEachPointAlongRoute.at(idxCenterPointOnRoute)._point);

       if (!bNextAvailablePointAlongRouteExists) {
           //qDebug() << __FUNCTION__<< "if (!bNextAvailablePointAlongRouteExists) {";
           break;
       }

       qpfStartingPoint =_qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._point;
       idxSegmentOfStartingPoint = _qvect_unitVectorForEachPointAlongRoute.at(idxNextPointAvailableOnRoute)._idxSegmentOwner;

       if (bOnlyComputeFirstBox) {
           break;
       }

       bFirstBox = false;

    }

    showContent_qvectProfBoxParameters_automatically_distributed();

    _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
    int nbBoxesCreated = _qvectProfBoxParameters_automatically_distributed.count();
    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
    if (!nbBoxesCreated) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
    } else {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
    }
    return(nbBoxesCreated > 0);

}

//---
//@LP: the dichotomic method considers that the route do not have curves with back direction.
//If the route has some, the dichotomic method can easyly fails to find start and end because each point
//checked using the dichotomic method is considered as a valid result about the fact that a box fit or not.
//It can be not the case with route with curves with back direction.
//But for the application goal and considered trace fault, this case should not happen.
bool BoxOrienterAndDistributer::findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingDichotomicMethod(
        Route *routePtr, int boxOddWidth, int boxOddLength,
        S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRouteSectionsFromStartAndEnd) {

    qDebug() << __FUNCTION__ << " starting ";

    sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed = false;
    sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed = false;
    sForbiddenRouteSectionsFromStartAndEnd._bOnlyOnePointPossible = false;
    sForbiddenRouteSectionsFromStartAndEnd._bFullRouteIsForbidden = true;

    if (!routePtr) {
        return(false);
    }

    //check from first point along the route, without any adjustements.
    e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_ReportAboutBoxFromTheStart = e_ERACBCAR_canNotFit;
    bool bFirstBoxFromFirstPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            false,//none adjustement
            e_ERACBCAR_ReportAboutBoxFromTheStart);

    qDebug() << __FUNCTION__ << "bFirstBoxFromFirstPointComputed = " << bFirstBoxFromFirstPointComputed << "; e_ERACBCAR_ReportAboutBoxFromTheStart = " << e_ERACBCAR_ReportAboutBoxFromTheStart;

    //simplest case about start
    if (e_ERACBCAR_ReportAboutBoxFromTheStart == e_ERACBCAR_fit) {

        qDebug() << __FUNCTION__ << "if (e_ERACBCAR_ReportAboutBoxFromTheStart == e_ERACBCAR_fit) {";

        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._qpf = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed = true;
    }

    //check from last point along the route, without any adjustements.
    e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_ReportAboutBoxFromTheEnd = e_ERACBCAR_canNotFit;
    bool bFirstBoxFromLastPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            false,//none adjustement
            e_ERACBCAR_ReportAboutBoxFromTheEnd);

    qDebug() << __FUNCTION__ << "bFirstBoxFromLastPointComputed = " << bFirstBoxFromLastPointComputed << "; e_ERACBCAR_ReportAboutBoxFromTheEnd = " << e_ERACBCAR_ReportAboutBoxFromTheEnd;

    //simplest case about end
    if (e_ERACBCAR_ReportAboutBoxFromTheEnd == e_ERACBCAR_fit) {

        qDebug() << __FUNCTION__ << "if (e_ERACBCAR_ReportAboutBoxFromTheEnd== e_ERACBCAR_fit) {";

        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._qpf = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed = true;
    }


    //simplest cases:
    //start & end available
    if (   sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed
        && sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed) {
        qDebug() << __FUNCTION__ << "if (bFirstPointFeed && bLastPointFeed) {";        
        sForbiddenRouteSectionsFromStartAndEnd._bFullRouteIsForbidden = false;
        return(true);
    }
    //start & end Not available
    if (  (e_ERACBCAR_ReportAboutBoxFromTheStart == e_ERACBCAR_canNotFit)
        ||(e_ERACBCAR_ReportAboutBoxFromTheEnd   == e_ERACBCAR_canNotFit)) {        
        qDebug() << __FUNCTION__ << "(...==e_ERACBCAR_canNotFit || ...==e_ERACBCAR_canNotFit)";
        return(false);
    }

    //check middle point of the route
    int idxSegmentOfStartingPoint = 0;
    QPointF qpfStartingPoint = routePtr->getSegment(idxSegmentOfStartingPoint)._ptA;

    int idxSegmentOfEndingPoint = routePtr->segmentCount()-1;
    QPointF qpfEndingPoint = routePtr->getSegment(idxSegmentOfEndingPoint)._ptB;

    int idxSegmentOfMiddlePoint =-1;
    QPointF qpfMiddlePoint = {.0, .0};

    bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;

    //if middle is false => considers all the route as impossible for box center point
    bool bReport = routePtr->findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
                qpfStartingPoint, idxSegmentOfStartingPoint,
                qpfEndingPoint, idxSegmentOfEndingPoint,
                qpfMiddlePoint, idxSegmentOfMiddlePoint,
                bDistanceFromMiddlePointToOthersIsUnderLimit);

    if (!bReport) {
        qDebug() << __FUNCTION__ << "#1 findMiddlePointOfRoute_betweenProvidenPoints report is False";
        return(false);
    }

    //middle found
    qDebug() << __FUNCTION__ << "#1 middle got = " << qpfMiddlePoint << "( idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

    //check if a box with middle as center box fits
    S_ProfilsBoxParameters profilsBoxParametersAtMiddlePoint;
    bReport = makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(routePtr,
                                                                          qpfMiddlePoint,
                                                                          idxSegmentOfMiddlePoint,
                                                                          boxOddWidth, boxOddLength,
                                                                          profilsBoxParametersAtMiddlePoint);
    if (!bReport) {        
        qDebug() << __FUNCTION__ << "#1 makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute report is false";
        return(false);
    }

    if (bDistanceFromMiddlePointToOthersIsUnderLimit) {

        qDebug() << __FUNCTION__ << "#1 bDistanceFromMiddlePointToOthersIsUnderLimit is true";

        //feed the two points with the same content
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._qpf = profilsBoxParametersAtMiddlePoint._qpfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner = profilsBoxParametersAtMiddlePoint._idxSegmentOwnerOfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed = true;

        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._qpf = profilsBoxParametersAtMiddlePoint._qpfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner = profilsBoxParametersAtMiddlePoint._idxSegmentOwnerOfCenterPoint;
        sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed = true;

        sForbiddenRouteSectionsFromStartAndEnd._bOnlyOnePointPossible = true;
        sForbiddenRouteSectionsFromStartAndEnd._bFullRouteIsForbidden = false;

        return(true);
    }


    qDebug() << __FUNCTION__ << "#2222222222222222222222222222222222222";

    //search first point from the start using dichotomic search
    if (!sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed) {

        qDebug() << __FUNCTION__ << "#2 if (!bFirstPointFeed) {";

        QPointF qpfFoundCenterPoint {.0, .0};
        int idxSegmentOwner_ofFoundCenterPoint = -1;
        bool bFound = findFirstPossibleCenterPointUsingDichotomicSearh(routePtr,
                                                                       e_SGCT_toFirstPoint,
                                                                       boxOddWidth, boxOddLength,
                                                                       qpfStartingPoint, idxSegmentOfStartingPoint,
                                                                       qpfMiddlePoint, idxSegmentOfMiddlePoint,
                                                                       qpfFoundCenterPoint, idxSegmentOwner_ofFoundCenterPoint);
        if (bFound) {
            qDebug() << __FUNCTION__ << "#2 bFound";
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._qpf = qpfFoundCenterPoint;
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._idxSegmentOwner = idxSegmentOwner_ofFoundCenterPoint;
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed = true;

            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart.debug_show();
        }
    }

    qDebug() << __FUNCTION__ << "#3333333333333333333333333333333333333";

    //search last point from the start using dichotomic search
    if (!sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed) {

        qDebug() << __FUNCTION__ << "#3 if (!bLastPointFeed) {";

        QPointF qpfFoundCenterPoint {.0, .0};
        int idxSegmentOwner_ofFoundCenterPoint = -1;
        bool bFound = findFirstPossibleCenterPointUsingDichotomicSearh(routePtr,
                                                                       e_SGCT_toLastPoint,
                                                                       boxOddWidth, boxOddLength,
                                                                       qpfMiddlePoint, idxSegmentOfMiddlePoint,
                                                                       qpfEndingPoint, idxSegmentOfEndingPoint,
                                                                       qpfFoundCenterPoint, idxSegmentOwner_ofFoundCenterPoint);
        if (bFound) {
            qDebug() << __FUNCTION__ << "#3 bFound";
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._qpf = qpfFoundCenterPoint;
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner = idxSegmentOwner_ofFoundCenterPoint;
            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed = true;

            sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd.debug_show();
        }
    }

    qDebug() << __FUNCTION__ << "sForbiddenRouteSectionsFromStartAndEnd._sForbiddenRSFromStart._bPointFeed =" << sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed;
    qDebug() << __FUNCTION__ << "sForbiddenRouteSectionsFromStartAndEnd._sForbiddenRSFromEnd._bPointFeed =" << sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed;

    bool bTwoPointsFound =
        (   sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromStart._bPointFeed
         && sForbiddenRouteSectionsFromStartAndEnd._sFrontierFromEnd._bPointFeed);

    qDebug() << __FUNCTION__ << "bTwoPointsFound =" << bTwoPointsFound;

    if (bTwoPointsFound) {
        sForbiddenRouteSectionsFromStartAndEnd._bFullRouteIsForbidden = false;
        qDebug() << __FUNCTION__ << "_bFullRouteIsForbidden set to " << sForbiddenRouteSectionsFromStartAndEnd._bFullRouteIsForbidden;
    }

    //if two points are not found (or only one point found), considers this case as an error case
    return (bTwoPointsFound);
}


bool BoxOrienterAndDistributer::findFirstPossibleCenterPointUsingDichotomicSearh(Route *routePtr,
                                                                                 e_SearchGoingCloserTo eSearchGoingCloserTo,
                                                                                 int boxOddWidth, int boxOddLength,
                                                                                 QPointF qpfStartingPoint, int idxSegmentOwner_ofStartingPoint,
                                                                                 QPointF qpfEndingPoint, int idxSegmentOwner_ofEndingPoint,
                                                                                 QPointF& qpfFoundCenterPoint, int& idxSegmentOwner_ofFoundCenterPoint) {
    if (!routePtr) {
        return(false);
    }

    bool bReport = false;

    bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;

    bool bPossibleCenterPointFound = false;

    while(1) {

        qDebug() << __FUNCTION__ << "while(1) {";

        //if middle is false => considers all the route as impossible for box center point
        bReport = routePtr->findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
                    qpfStartingPoint, idxSegmentOwner_ofStartingPoint,
                    qpfEndingPoint, idxSegmentOwner_ofEndingPoint,
                    qpfFoundCenterPoint, idxSegmentOwner_ofFoundCenterPoint,
                    bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << __FUNCTION__ << "findMiddlePointOfRoute_betweenProvidedPoints said : " << bReport;
        if (!bReport) {
            //break;
            return(false);
        }

        if (bDistanceFromMiddlePointToOthersIsUnderLimit) {
            qDebug() << __FUNCTION__ << "bDistanceFromMiddlePointToOthersIsUnderLimit is : " << bDistanceFromMiddlePointToOthersIsUnderLimit;
            break;
        }

        //middle found
        qDebug() << __FUNCTION__ << " middle got = " << qpfFoundCenterPoint << "( idxSegmentOwner_ofFoundCenterPoint = " << idxSegmentOwner_ofFoundCenterPoint;

        //check if a box with middle as center box fits
        S_ProfilsBoxParameters profilsBoxParametersAtMiddlePoint;
        bReport = makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(routePtr,
                                                                              qpfFoundCenterPoint,
                                                                              idxSegmentOwner_ofFoundCenterPoint,
                                                                              boxOddWidth, boxOddLength,
                                                                              profilsBoxParametersAtMiddlePoint);

        qDebug() << __FUNCTION__ << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute said : " << bReport;

        if (!bReport) {

            qDebug() << __FUNCTION__ << "bReport is false ( " << bReport;

            if (eSearchGoingCloserTo == e_SGCT_toFirstPoint) {

                //progress going closer to the ending point
                qpfStartingPoint = qpfFoundCenterPoint;
                idxSegmentOwner_ofStartingPoint = idxSegmentOwner_ofFoundCenterPoint;

                qDebug() << __FUNCTION__ << "e_SGCT_toFirstPoint:";
                qDebug() << __FUNCTION__ << "  qpfStartingPoint is now: " << qpfStartingPoint;
                qDebug() << __FUNCTION__ << "  idxSegmentOwner_ofStartingPoint is now: " << idxSegmentOwner_ofStartingPoint;

            } else {

                //progress going closer to the starting point
                qpfEndingPoint = qpfFoundCenterPoint;
                idxSegmentOwner_ofEndingPoint = idxSegmentOwner_ofFoundCenterPoint;

                qDebug() << __FUNCTION__ << "e_SGCT_toLastPoint:";
                qDebug() << __FUNCTION__ << "  qpfEndingPoint is now: " << qpfEndingPoint;
                qDebug() << __FUNCTION__ << "  idxSegmentOwner_ofEndingPoint is now: " << idxSegmentOwner_ofEndingPoint;
            }
            continue;
        }

        qDebug() << __FUNCTION__ << "bReport is true ( " << bReport;

        if (eSearchGoingCloserTo == e_SGCT_toFirstPoint) {
            //progress going closer to the starting point
            qpfEndingPoint = qpfFoundCenterPoint;
            idxSegmentOwner_ofEndingPoint = idxSegmentOwner_ofFoundCenterPoint;

            qDebug() << __FUNCTION__ << "e_SGCT_toFirstPoint:";
            qDebug() << __FUNCTION__ << "  qpfEndingPoint is now: " << qpfEndingPoint;
            qDebug() << __FUNCTION__ << "  idxSegmentOwner_ofEndingPoint is now: " << idxSegmentOwner_ofEndingPoint;

        } else {
            //progress going closer to the ending point
            qpfStartingPoint = qpfFoundCenterPoint;
            idxSegmentOwner_ofStartingPoint = idxSegmentOwner_ofFoundCenterPoint;

            qDebug() << __FUNCTION__ << "e_SGCT_toLastPoint:";
            qDebug() << __FUNCTION__ << "qpfStartingPoint is now: " << qpfStartingPoint;
            qDebug() << __FUNCTION__ << "idxSegmentOwner_ofStartingPoint is now: " << idxSegmentOwner_ofStartingPoint;
        }

    }

    return(true);
}


//usingAutomaticDistribution_distanceBetweenCenter
bool BoxOrienterAndDistributer::computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromFirstPoint(
        Route *routePtr, int boxOddWidth, int boxOddLength, int distanceBetweenCenter) {

    if (!routePtr) {
        return(false);
    }

    e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_ReportAboutBoxFromTheStart = e_ERACBCAR_canNotFit;
    bool bFirstBoxFromFirstPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            true,//make adjustements to fit the box
            e_ERACBCAR_ReportAboutBoxFromTheStart);

    qDebug() << __FUNCTION__ << "bFirstBoxFromFirstPointComputed = " << bFirstBoxFromFirstPointComputed;


    if (!bFirstBoxFromFirstPointComputed) {
        qDebug() << __FUNCTION__ << __LINE__ << " if (!bFirstBoxFromFirstPointComputed) {";
        return(false);
    }
    if (e_ERACBCAR_ReportAboutBoxFromTheStart == e_ERACBCAR_canNotFit) {
        qDebug() << __FUNCTION__ << __LINE__ << " if (e_ERACBCAR_ReportAboutBoxFromTheStart == e_ERACBCAR_canNotFit) {";
        return(false);
    }


    QVector<S_ProfilsBoxParameters> qvectProfBoxParameters_automatically_distributed;
    qvectProfBoxParameters_automatically_distributed.push_back(_qvectProfBoxParameters_automatically_distributed[0]);

    QPointF qpfLastBoxCenter = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
    int idxSegmentOwnerOf_lastBoxCenter = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;

    while(1) {

        qDebug() << __FUNCTION__ << "while(1) { with qpfLastBoxCenter = " << qpfLastBoxCenter;

        QPointF qpfPointAsBoxCenter {.0, .0};
        int idxSegmentOwnerOfPointAsBoxCenter = -1;

        bool bReport = routePtr->findPointAtADistanceFromAnotherPoint_forwardDirection(qpfLastBoxCenter, idxSegmentOwnerOf_lastBoxCenter, static_cast<double>( distanceBetweenCenter),
                                                                                       qpfPointAsBoxCenter, idxSegmentOwnerOfPointAsBoxCenter);
        qDebug() << __FUNCTION__ << "findPointAtADistanceFromAnotherPoint_forwardDirection said: " << bReport;
        if (!bReport) {
            break;
        }

        S_ProfilsBoxParameters sProfilsBoxParameters;
        bReport = makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(routePtr, qpfPointAsBoxCenter, idxSegmentOwnerOfPointAsBoxCenter, boxOddWidth, boxOddLength,
                                                                              sProfilsBoxParameters);
        qDebug() << __FUNCTION__ << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute said: " << bReport;

        if (!bReport) {
            break;
        }

        qvectProfBoxParameters_automatically_distributed.push_back(sProfilsBoxParameters);

        qDebug() << __FUNCTION__ << "qvectProfBoxParameters_automatically_distributed.size() = " << qvectProfBoxParameters_automatically_distributed.size();

        qpfLastBoxCenter = qpfPointAsBoxCenter;
        idxSegmentOwnerOf_lastBoxCenter = idxSegmentOwnerOfPointAsBoxCenter;

    }

    //copy generated boxes to _qvectProfBoxParameters_automatically_distributed
    _qvectProfBoxParameters_automatically_distributed = qvectProfBoxParameters_automatically_distributed;

    //and associate a unique boxSetID and set _idxBoxInSet for each boxes in the set
    int feed_qvectProfBoxParameters_automatically_distributed_size = _qvectProfBoxParameters_automatically_distributed.size();

    qDebug() << __FUNCTION__ << "feed_qvectProfBoxParameters_automatically_distributed_size: " << feed_qvectProfBoxParameters_automatically_distributed_size;

    qint64 boxSetID = IDGenerator::generateABoxesSetID();

    for (int i = 0; i < feed_qvectProfBoxParameters_automatically_distributed_size; i++) {
        _qvectProfBoxParameters_automatically_distributed[i]._sUniqueBoxID._setID = boxSetID;
        _qvectProfBoxParameters_automatically_distributed[i]._sUniqueBoxID._idxBoxInSet = i;
    }

    int nbBoxesCreated = feed_qvectProfBoxParameters_automatically_distributed_size;
    _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
    if (!nbBoxesCreated) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
    } else {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
    }
    return(nbBoxesCreated > 0);
}

bool BoxOrienterAndDistributer::computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromLastPoint(
        Route *routePtr, int boxOddWidth, int boxOddLength, int distanceBetweenCenter) {


    if (!routePtr) {
        return(false);
    }

    e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_ReportAboutBoxFromTheEnd = e_ERACBCAR_canNotFit;
    bool bFirstBoxFromLastPointComputed = computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
            routePtr, boxOddWidth,
            boxOddLength,
            e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, //@##LP needs to be adjusted if square8 used, to be able to shift the box,
                                                                             //to be sure to do not take pixel before the first point
            true, //OnlyComputeFirstBox
            true,//make adjustements to fit the box
            e_ERACBCAR_ReportAboutBoxFromTheEnd);

    qDebug() << __FUNCTION__ << "bFirstBoxFromLastPointComputed = " << bFirstBoxFromLastPointComputed;



    if (!bFirstBoxFromLastPointComputed) {
        qDebug() << __FUNCTION__ << __LINE__ << " if (!bFirstBoxFromLastPointComputed) {";
        return(false);
    }
    if (e_ERACBCAR_ReportAboutBoxFromTheEnd == e_ERACBCAR_canNotFit) {
        qDebug() << __FUNCTION__ << __LINE__ << " if (e_ERACBCAR_ReportAboutBoxFromTheEnd == e_ERACBCAR_canNotFit) {";
        return(false);
    }

    QVector<S_ProfilsBoxParameters> qvectProfBoxParameters_automatically_distributed;
    qvectProfBoxParameters_automatically_distributed.push_back(_qvectProfBoxParameters_automatically_distributed[0]);

    QPointF qpfLastBoxCenter = _qvectProfBoxParameters_automatically_distributed[0]._qpfCenterPoint;
    int idxSegmentOwnerOf_lastBoxCenter = _qvectProfBoxParameters_automatically_distributed[0]._idxSegmentOwnerOfCenterPoint;

    while(1) {

        qDebug() << __FUNCTION__ << "while(1) { with qpfLastBoxCenter = " << qpfLastBoxCenter;

        QPointF qpfPointAsBoxCenter {.0, .0};
        int idxSegmentOwnerOfPointAsBoxCenter = -1;

        bool bReport = routePtr->findPointAtADistanceFromAnotherPoint_backwardDirection(qpfLastBoxCenter, idxSegmentOwnerOf_lastBoxCenter, static_cast<double>( distanceBetweenCenter),
                                                                                       qpfPointAsBoxCenter, idxSegmentOwnerOfPointAsBoxCenter);
        qDebug() << __FUNCTION__ << "findPointAtADistanceFromAnotherPoint_forwardDirection said: " << bReport;
        if (!bReport) {
            break;
        }

        S_ProfilsBoxParameters sProfilsBoxParameters;
        bReport = makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(routePtr, qpfPointAsBoxCenter, idxSegmentOwnerOfPointAsBoxCenter, boxOddWidth, boxOddLength,
                                                                              sProfilsBoxParameters);
        qDebug() << __FUNCTION__ << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute said: " << bReport;

        if (!bReport) {
            break;
        }

        qvectProfBoxParameters_automatically_distributed.push_back(sProfilsBoxParameters);

        qDebug() << __FUNCTION__ << "qvectProfBoxParameters_automatically_distributed.size() = " << qvectProfBoxParameters_automatically_distributed.size();

        qpfLastBoxCenter = qpfPointAsBoxCenter;
        idxSegmentOwnerOf_lastBoxCenter = idxSegmentOwnerOfPointAsBoxCenter;

    }

    //copy generated boxes to _qvectProfBoxParameters_automatically_distributed
    _qvectProfBoxParameters_automatically_distributed = qvectProfBoxParameters_automatically_distributed;

    //and associate a unique boxSetID and set _idxBoxInSet for each boxes in the set
    int feed_qvectProfBoxParameters_automatically_distributed_size = _qvectProfBoxParameters_automatically_distributed.size();

    qDebug() << __FUNCTION__ << "feed_qvectProfBoxParameters_automatically_distributed_size: " << feed_qvectProfBoxParameters_automatically_distributed_size;

    qint64 boxSetID = IDGenerator::generateABoxesSetID();

    for (int i = 0; i < feed_qvectProfBoxParameters_automatically_distributed_size; i++) {
        _qvectProfBoxParameters_automatically_distributed[i]._sUniqueBoxID._setID = boxSetID;
        _qvectProfBoxParameters_automatically_distributed[i]._sUniqueBoxID._idxBoxInSet = i;
    }

    int nbBoxesCreated = feed_qvectProfBoxParameters_automatically_distributed_size;
    _sibaStrMsgStatus_aboutAutomaticDistribution._bNoneErrorOccured = true;
    _sibaStrMsgStatus_aboutAutomaticDistribution._nbBoxesCreated = nbBoxesCreated;
    if (!nbBoxesCreated) {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = "Can not distribute boxes along the route with the selected parameters";
    } else {
        _sibaStrMsgStatus_aboutAutomaticDistribution._strMessage_details = QString::number(nbBoxesCreated) + " boxes created";
    }
    return(nbBoxesCreated > 0);
}

