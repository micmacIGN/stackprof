#include "ComputationCore_structures.h"

//use IlmBase vector operations, which comes with openerx (openerx used by oiio)
#include "ImathVec.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "route.h"

#include "../imageScene/S_ForbiddenRouteSection.h"

struct S_intBoolAndStrMsgStatus_aboutAutomaticDistribution {
    int _nbBoxesCreated;
    bool _bNoneErrorOccured;
    QString _strMessage;
    QString _strMessage_details; //includes error details if error occured
    S_intBoolAndStrMsgStatus_aboutAutomaticDistribution();
    void clear();
};

class BoxOrienterAndDistributer {

    public:

        BoxOrienterAndDistributer();

        void clear();

        enum e_EvaluationResultAboutCenterBoxCandidateAlongRoute {
            e_ERACBCAR_fit = 1,

            e_ERACBCAR_extremityIsInsideTheBox_boxNeedsLocationAdjustementGoingFurtherAlongTheRoute = 2,
            e_ERACBCAR_extremityIsOutsideTheBox_boxNeedsLocationAdjustementBecomingCloserAlongTheRoute = 3,

            e_ERACBCAR_canNotFit = 6
        };

        //each call will clear _qvectProfBoxParameters_automatically_distributed at start
        //
        //usingAutomaticDistribution_closeAsPossible
        bool computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(
                Route *routePtr,
                int boxOddWidth, int boxOddLength,
                e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDBSBuAD_toUse,
                bool bOnlyComputeFirstBox/*= false*/,
                bool bFindCenterUsingLocationAdjustement/* = true*/,
                e_EvaluationResultAboutCenterBoxCandidateAlongRoute& e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);

        bool computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromLastPoint(
                Route *routePtr,
                int boxOddWidth, int boxOddLength,
                e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution eDBSBuAD_toUse,
                bool bOnlyComputeFirstBox/* = false*/,
                bool bFindCenterUsingLocationAdjustement/* = true*/,
                e_EvaluationResultAboutCenterBoxCandidateAlongRoute& e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);


        //usingAutomaticDistribution_distanceBetweenCenter
        bool computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromFirstPoint(Route *routePtr, int boxOddWidth, int boxOddLength, int distanceBetweenCenter);
        bool computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromLastPoint(Route *routePtr , int boxOddWidth, int boxOddLength, int distanceBetweenCenter);

        S_intBoolAndStrMsgStatus_aboutAutomaticDistribution get_sibaStrMsgStatus_aboutAutomaticDistribution() const;
        const QVector<S_ProfilsBoxParameters>& getConstRef_qvectProfBoxParameters_automatically_distributed() const;
        bool get_automaticDistributionID(qint64& automaticDistributionID);

        bool makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(Route *routePtr,
                                                                         QPointF qpfCenterPoint,
                                                                         int idxSegmentOwner,
                                                                         int boxOddWidth, int boxOddLength,
                                                                         S_ProfilsBoxParameters& profilsBoxParameters);
        //---------





        bool checkThatABoxFromAGivenLocationAndWidthFitInRouteWithNoLocationChange(
            Route *routePtr, const S_ProfilsBoxParameters& profilBoxParameters, int wantedWidth,
            S_ProfilsBoxParameters& profilBoxParametersWithChanges,
            bool &bProfilBoxFit,
            bool &bProfilBoxWithChangesIncludeCenterPointRelocation);


        bool testWhichCenterBoxMovementsArePossibleFromCurrentLocationForCurrentBoxId(
            Route *routePtr, const S_ProfilsBoxParameters& profilBoxParameters, const QVector<int>& qvectShiftToTest,
            //QVector<bool>& qvectbPossibleShift, QVector<S_ProfilsBoxParametersForRelativeMoveAlongRoute>& qvectSProfilsBoxParametersForRelativeMoveAlongRoute);
            QHash<int, bool>& qhash_shift_bPossibleShift,
            QHash<int, S_ProfilsBoxParametersForRelativeMoveAlongRoute>& qhash_shift_sProfilsBoxParametersForRelativeMoveAlongRoute);


        //return false if none point available (meaning that the box width is too big to fit in route)
        //bOnlyOnePointAvailable indicated if qpfPointFoundFromFirstPointOfRoute equal to qpfPointFoundFromLastPointofRoute (only one box can fit)
        bool findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingSlidingWindowMethod(
            Route *routePtr, int boxOddWidth, int boxOddLength,
            QPointF& qpfFirstPointAlongRoute, int& idxSegmentOwner_ofFirstPointAlongRoute,
            QPointF& qpflastPointAlongRoute, int& idxSegmentOwner_ofLastPointAlongRoute,
            bool& bFirstPointFeed, bool& bLastPointFeed);

        bool findFirstAndLastPointAlongRouteWhichCanBeCenterBoxForASpecificWidthBox_usingDichotomicMethod(
            Route *routePtr, int boxOddWidth, int boxOddLength,
            S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRouteSectionsFromStartAndEn);

    private:

        enum e_SearchGoingCloserTo {
            e_SGCT_toFirstPoint,
            e_SGCT_toLastPoint
        };

        //includes a clear on qvect result container before feeding it
        bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirection(
            QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
            bool& bEndOfRouteReached_beforeDistanceReached);

        //does not include a clear on qvect result container before feeding it
        /*bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
                QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
                bool& bEndOfRouteReached_beforeDistanceReached);*/

        bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_forwardDirectionFeed(
                QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
                double distanceToCheck, //typically boxWidth/2 used to know if there is enough points from the starting point
                bool& bEndOfRouteReached_beforeDistanceToReach,
                bool& bEndOfRouteReached_beforeDistanceToCheck);


        bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirection(
                QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
                bool& bStartOfRouteReached_beforeDistanceReached);

        bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
                QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
                double distanceToCheck, //typically boxWidth/2 used to know if there is enough points from the starting point
                bool& bStartOfRouteReached_beforeDistanceToReach,
                bool& bStartOfRouteReached_distanceToCheck);

        //does not include a clear on qvect result container before feeding it
        /*bool compute_unitVectorDirectionAtEachPointAlongRoute_fromPointOfASpecificSegment_untilDistanceReachAValue_backwardDirectionFeed(
                QPointF qpfStartingPoint, int idxSegmentOfStartingPoint, double distanceToReach,
                bool& bStartOfRouteReached_beforeDistanceReached);*/

        e_EvaluationResultAboutCenterBoxCandidateAlongRoute evaluateCenterPointOnRouteAsCenterBoxCandidate(
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
                Vec2<double>& meanDirectionVectorForBox);

        bool findNextAvailablePointAlongRouteFrom(
                int idxCenterPointOnRoute, int intWidthDiv2, Vec2<double> meanDirectionVectorForBox,
                int& idxNextAvailablePoint);

        void showContent_qvectProfBoxParameters_automatically_distributed();


        bool findFirstPossibleCenterPointUsingDichotomicSearh(Route *routePtr,
                                                             e_SearchGoingCloserTo eSearchGoingCloserTo,
                                                             int boxOddWidth, int boxOddLength,
                                                             QPointF qpfStartingPoint, int idxSegmentOwner_ofStartingPoint,
                                                             QPointF qpfEndingPoint, int idxSegmentOwner_ofEndingPoint,
                                                             QPointF& qpfFoundCenterPoint, int& idxSegmentOwner_ofFoundCenterPoint);

        bool checkPointIsInsideOrientedRectBox(
            QPointF qpfCenterBox,
            Vec2<double> directionVectorForBox_normalized, //mean Direction Vector about the box must be normalized as checkPointInsideAOrientedRectBox method parameter
            double distance_BoxCenter_to_firstProfil, // = width box / 2.0
            double distance_boxCenter_to_lengthBorder, // = length box / 2.0
            QPointF qpfPointToTest,
            bool& bDistanceBetweenNextPointAvailableAndItsProjectionOnBoxAxisDirectionIsUnderWidthDiv2);

    private:

        Route *_routePtr;

        QVector<S_ProfilsBoxParameters> _qvectProfBoxParameters_automatically_distributed;

        const double _subDivisionOneUnitVectorAlongRoute = 10.0;
        //const int  _intSubDivisionOneUnitVectorAlongRoute = 10;

        QVector<QPointF> _qvectQPointF_devdebug_centerBox;
        QVector<QPointF> _qvectQPointF_devdebug_firstProfil_centerBox;
        QVector<QPointF> _qvectQPointF_devdebug_firstPointAlongRoute;
        QVector<S_qpf_point_and_unitVectorDirection> _qvect_devdebug_unitVectorForEachPointAlongRoute;
        QVector<QPointF> _qvectQPointF_devdebug_testedPointsToFindNextAvailablePointAlongRoute;

        QVector<S_qpf_point_and_unitVectorDirection> _qvect_unitVectorForEachPointAlongRoute;
        qint64 _automaticDistributionID;
        bool _bAutomaticDistributionID_valid;
        int _idxBoxInSet;
        e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution _eDBSBuAD_used;

        S_intBoolAndStrMsgStatus_aboutAutomaticDistribution _sibaStrMsgStatus_aboutAutomaticDistribution;

        //static constexpr int _cstExpr_distanceMultiplierforPointsDistribution = 44;
};
