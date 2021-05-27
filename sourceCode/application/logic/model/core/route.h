#ifndef ROUTE_H
#define ROUTE_H

#include <QVector>
#include <QPointF>

#include <QPolygonF>

#include <QJsonObject>

#include "S_Segment.h"

class Route {

public:
    Route();

    static QString defaultStrRouteName();

    //just to check what of them are involved in current code implementation
    /*Route(const Route& copyMe);
    Route(Route&& moveMe);
    Route& operator=(const Route& copyMe);
    Route& operator=(Route&& moveMe);*/

    bool operator ==(const Route &compareToMe) const;

    //update the graphic items:

    bool tryAddPointAtRouteEnd(QPointF qpf, bool& bThisIsTheFirstPointForTheRoute);
    bool tryInsertPointInRoute(QPointF qpfNoZLI,  int currentZLI, bool& bThisIsTheFirstPointForTheRoute);

    bool removePoint(int pointId); //return success report
    void clear();
    bool insertPointInSegment(QPointF qptf, int idSegmentWherePointNeedsToBeInserted);

    void setId(int id);
    int getId() const;

    void setName(const QString& strName);
    QString getName() const;

    void showContent() const;

    QPointF getPoint(int index) const;
    const QVector<QPointF>& getCstRefVectPoint() const;

    int pointCount() const;
    int segmentCount() const;
    S_Segment getSegment(int index) const;

    bool setPointAt(int idPoint, QPointF qpf, bool& bEmitSignalAboutRejectedOperation);

    bool toQJsonObject(QJsonObject &qJsonObjOut) const;
    bool fromQJsonObject(const QJsonObject& qJsonObj) ;

    //this method can be helpful to choose the box side size around each point of the route
    qreal distanceBetweenClosestSuccessivePoints() const;

    //bool isPointCloseToASegment(const QPointF& qpointFToTestNOZLI, int currentZLI, int& idClosestSegment) const;
    bool isPointCloseToASegment(const QPointF& qpointFToTestNOZLI, int currentZLI, bool bFindClosestPointWithDistanceNotBelow1, int& idClosestSegment) const;

    bool isPointCloseToTheLastPointOfRoute(const QPointF& pos) const;

    //for debug purpose, propose to get the qpolygfOfConsideredAreaAroundSegment
    /*bool isPointCloseToASegment(const QPointF& qpointFToTest,
                                bool bGetConsideredAreaAroundSegment,
                                QPolygonF& qpolygfOfConsideredAreaAroundSegment) const;*/

    //in public for debug purpose
    bool rectangularShapeFromSegment_containsPoint(
            const S_Segment &segment,
            double widthDiv2perpendicularToSegment,
            const QPointF& qpointFToTest,
            bool bGetConsideredAreaAroundSegment,
            QPolygonF& qpolygfOfConsideredAreaAroundSegment) const;

    bool findRelocatedPointAlongRouteForAFarPoint(const QPointF& qpfPos, QPointF& qpfRelocated, int& idxOfSegmentOwner) const;

    bool findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
        const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint,
        const QPointF& qpfEndingPoint, int idxSegmentOfEndingPoint,
        QPointF& qpfMiddlePoint, int& idxSegmentOfMiddlePoint,
        bool& bDistanceFromMiddlePointToOthersIsUnderLimit);

    bool findPointAtADistanceFromAnotherPoint_forwardDirection(
        const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint, qreal distanceFromStartingPoint,
        QPointF& qpfPointAtDistance, int& idxSegmentOfPointAtDistance);

    bool findPointAtADistanceFromAnotherPoint_backwardDirection(
        const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint, qreal distanceFromStartingPoint,
        QPointF& qpfPointAtDistance, int& idxSegmentOfPointAtDistance);

    bool computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection(
        const QPointF& qpfRoutePoint, int idxSegmentOfRoutePoint,
        qreal &distanceFromStartingRoutePointToRoutePoint) const;

private:     

    //bool findSegmentsAroundPos(const QPointF& pos, S_Segment segment_before_after[2], int& idOfCentralPoint) const;
    bool findSegmentsAroundPos(const QPointF& pos, bool bFindClosestPointWithDistanceNotBelow1, S_Segment segment_before_after[2], int& idOfCentralPoint) const;

    bool findClosestPointToPos_butWithDistanceNotBelow1(const QPointF& pos, int& idPoint, bool& bDistanceToClosestPointIsBelow1) const;

    bool checkDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne(int idPoint, QPointF qpf,  bool &bRejectedPoint);

    bool checkIndexPointExists(int index) const;
    void updateSegmentCount();

    bool checkIndexSegmentExists(int index) const;

    int _id;
    QString _strName;
    //ordered map could match with the need also
    QVector<QPointF> _qvctqptf_connectedSegmentPoints;

    int _segmentCount;

    static const QString _json_versionFormat_keyString;
    static const QString _json_versionFormat_value;
};

#endif // ROUTE_H


