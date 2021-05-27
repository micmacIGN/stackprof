#include <QDebug>

#include <QtMath>

#include "toolbox/toolbox_conversion.h"
#include "toolbox/toolbox_json.h"
#include "toolbox/toolbox_string.h"
#include "toolbox/toolbox_math_geometry.h"

#include <QJsonArray>

using namespace std;

#include <QPolygonF>

#include "S_Segment.h"

#include "route.h"

Route::Route(): _id(-1), _segmentCount(0) {
}

QString Route::defaultStrRouteName() {
    return("no name");
}

bool Route::operator ==(const Route &compareToMe) const {
    return(_id == compareToMe._id);
}

void Route::setName(const QString& strName) {
    _strName = strName;
    qDebug() << __FUNCTION__ << "set _strName as:" << _strName;
}

QString Route::getName() const {
    return(_strName);
}

void Route::setId(int id) {
    _id = id;
}

int Route::getId() const {

    qDebug() << __FUNCTION__ << "_id = " << _id;

    return(_id);
}


bool Route::tryAddPointAtRouteEnd(QPointF qpf, bool& bThisIsTheFirstPointForTheRoute) {

    bThisIsTheFirstPointForTheRoute = (pointCount() == 0);

    bool bAdd = false;
    if (bThisIsTheFirstPointForTheRoute) {
        bAdd = true;
    } else {
        if (!isPointCloseToTheLastPointOfRoute(qpf)) {
            bAdd = true;
        }
    }
    if (bAdd) {
        //add the point and update segment count
        _qvctqptf_connectedSegmentPoints.push_back(qpf);
        updateSegmentCount();
    } else {
        qDebug() << __FUNCTION__ << "add the point rejected because to close to last point of route";
    }
    return(bAdd);
}

bool Route::tryInsertPointInRoute(QPointF qpfNoZLI, int currentZLI, bool& bThisIsTheFirstPointForTheRoute) {

    bThisIsTheFirstPointForTheRoute = (pointCount() == 0);
    if (bThisIsTheFirstPointForTheRoute) {
        qDebug() << __FUNCTION__ << " info for the end user: can't insert point on an empty route, use the add function instead";
        return(false);
    }

    bool bReportInserted = false;

    int idClosestSegment = -1;
    bool bReport_PointCloseToASegmentForRoute = isPointCloseToASegment(qpfNoZLI, currentZLI, true, idClosestSegment);
    if (bReport_PointCloseToASegmentForRoute) {
      if (!insertPointInSegment(qpfNoZLI, idClosestSegment)) {
          qDebug() << __FUNCTION__ << " info: can not insert point using idClosestSegment = " << idClosestSegment;
      } else {
          bReportInserted = true;
      }
    }

    return(bReportInserted);
}


bool Route::insertPointInSegment(QPointF qptf, int idSegmentWherePointNeedsToBeInserted) {

    if (  (idSegmentWherePointNeedsToBeInserted < 0)
        ||(idSegmentWherePointNeedsToBeInserted >= _segmentCount)) {
        return(false);
    }

    int idxPointForInsertion = idSegmentWherePointNeedsToBeInserted+1;
    qDebug() << __FUNCTION__ << " (Route) : idxPointForInsertion = "  << idxPointForInsertion;

    qDebug() << __FUNCTION__ << " (Route) : before: _qvctqptf_connectedSegmentPoints = "  << _qvctqptf_connectedSegmentPoints;

    //insert point only if the point location is far enough two the point around (>1.0 to each)
    //always the same contraint: the automatic stacked profil box distribution along the path need that distance between successive point > 1.0



    bool bRejectedOperation = false;
    bool bCheckDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne =
        checkDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne(idxPointForInsertion, qptf, bRejectedOperation);

    if (!bCheckDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne) {
        return(false);
    }
    if (bRejectedOperation == true) {
        return(false);
    }

    //qDebug() << __FUNCTION__ << " (Route) : after updateSegmentCount: _segmentCount = "  << _segmentCount;
    qDebug() << __FUNCTION__ << " (Route) : after: _qvctqptf_connectedSegmentPoints = "  << _qvctqptf_connectedSegmentPoints;
    _qvctqptf_connectedSegmentPoints.insert(idxPointForInsertion, qptf);
    updateSegmentCount();
    qDebug() << __FUNCTION__ << " (Route) : after updateSegmentCount: _segmentCount = "  << _segmentCount;
    return(true);

}

bool Route::removePoint(int index) {
    if (!checkIndexPointExists(index)) {
        return(false);
    }

    _qvctqptf_connectedSegmentPoints.remove(index);
    updateSegmentCount();
    return(true);
}

void Route::clear() {
    _qvctqptf_connectedSegmentPoints.clear();
    updateSegmentCount();
}

int Route::segmentCount() const {
    return(_segmentCount);
}

int Route::pointCount() const {
    qDebug() << __FUNCTION__ << " (_qvctqptf_connectedSegmentPoints.size() = " << _qvctqptf_connectedSegmentPoints.size();
    return(_qvctqptf_connectedSegmentPoints.size());
}

QPointF Route::getPoint(int index) const {
    if (!checkIndexPointExists(index)) {
        qDebug() << __FUNCTION__ << ": error: invalid index (" << index;
        return(QPointF{-1.0, -1.0}); ////invalid point
    }
    return(_qvctqptf_connectedSegmentPoints.at(index));
}

const QVector<QPointF>& Route::getCstRefVectPoint() const {
    return(_qvctqptf_connectedSegmentPoints);
}


//do not take into account two points not consecutive but which can be closest that two successive points
qreal Route::distanceBetweenClosestSuccessivePoints() const {
    int nbPoint = _qvctqptf_connectedSegmentPoints.size();
    if (nbPoint < 2) {
        qDebug() << __FUNCTION__ << ": no point or only one in Route";
        return(-1.0); //no point or only one
    }
    qreal min_distanceBetweenSuccesivePoint = 99999999.0; //@LP arbitraty huge starting min value
    for (int i=0; i< nbPoint-1; i++) {

        QPointF qpfDiff = _qvctqptf_connectedSegmentPoints.at(i) - _qvctqptf_connectedSegmentPoints.at(i+1);
        qreal dist = qSqrt(QPointF::dotProduct(qpfDiff, qpfDiff));

        /*qDebug() << __FUNCTION__ << "point #" << i
                 << " ; dist from" << _qvctqptf_connectedSegmentPoints.at(i)
                 << " to :"        << _qvctqptf_connectedSegmentPoints.at(i+1)
                 << " = " << dist;                 
        qDebug() << __FUNCTION__ << " (qpfDiff = " << qpfDiff;*/

        if (dist < min_distanceBetweenSuccesivePoint) {
            min_distanceBetweenSuccesivePoint = dist;            
        }
    }

    //qDebug() << __FUNCTION__ << "final min_dist=" << min_distanceBetweenSuccesivePoint;

    return(min_distanceBetweenSuccesivePoint);
}



S_Segment Route::getSegment(int index) const {
    S_Segment segment; //invalid by default
    if ((index < 0)||(index >= _segmentCount)) {
        return(segment);
    }    
    segment._ptA = _qvctqptf_connectedSegmentPoints.at(index);
    segment._ptB = _qvctqptf_connectedSegmentPoints.at(index+1);
    segment._bValid = true;
    return(segment);
}

void Route::updateSegmentCount() {
    int qvctSize = _qvctqptf_connectedSegmentPoints.size();
    if (qvctSize <= 1) {
        _segmentCount = 0;
    } else {
        _segmentCount = qvctSize -1;
    }
    qDebug() << __FUNCTION__ << " _segmentCount = " << _segmentCount;
}


bool Route::setPointAt(int idPoint, QPointF qpf, bool& bEmitSignalAboutRejectedOperation) {

    bEmitSignalAboutRejectedOperation = false;

    bool bCheckDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne =
        checkDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne(idPoint, qpf, bEmitSignalAboutRejectedOperation);

    if (!bCheckDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne) {
        return(false);
    }

    if (!bEmitSignalAboutRejectedOperation) {
      qDebug() << __FUNCTION__ << "change from: " << _qvctqptf_connectedSegmentPoints.at(idPoint) << "to : " << qpf;
      _qvctqptf_connectedSegmentPoints[idPoint] = qpf;
      qDebug() << __FUNCTION__ << "  checking change: " << _qvctqptf_connectedSegmentPoints.at(idPoint);
    }

    return(true);
}



//return false in case of major error like out of bound point index
//@LP: use bRejectedPoint for computation status
bool Route::checkDistancesBetweenPointToInsertAndExtremitiesSegmentAreMoreThanOne(int idPoint, QPointF qpf,  bool &bRejectedPoint) {

    bRejectedPoint = false;

    if (!checkIndexPointExists(idPoint)) {
        qDebug() << __FUNCTION__ << "if (!checkPointExistsAt(idPoint)) {";
        return(false);
    }

    QPointF points_before_after[2] {{.0,.0},{.0,.0}};
    bool testDistToPoints_before_after[2] {false, false};

    if (checkIndexPointExists(idPoint-1)) {
       points_before_after[0] = getPoint(idPoint-1);
       testDistToPoints_before_after[0] = true;
       qDebug() << __FUNCTION__ << "points_before_after[0] valid: << " << points_before_after[0];
    }
    if (checkIndexPointExists(idPoint+1)) {
       points_before_after[1] = getPoint(idPoint+1);
       testDistToPoints_before_after[1] = true;
       qDebug() << __FUNCTION__ << "points_before_after[1] valid: << " << points_before_after[1];
    }

    S_Segment potential_futur_segment[2];
    double dist_potential_futur_segment[2] {.0, .0};

    for (int i=0; i<2; i++) {

        if (testDistToPoints_before_after[i]) {

            potential_futur_segment[i]._ptA = points_before_after[i];
            potential_futur_segment[i]._ptB = qpf;
            potential_futur_segment[i]._bValid = true;
            dist_potential_futur_segment[i] = potential_futur_segment[i].length();

            qDebug() << __FUNCTION__ << "dist_potential_futur_segment[ " << i << " ]:";
            qDebug() << __FUNCTION__ << "_ptA      : " << potential_futur_segment[i]._ptA;
            qDebug() << __FUNCTION__ << "_ptB (qpf): " << potential_futur_segment[i]._ptB;


            dist_potential_futur_segment[i] = potential_futur_segment[i].length();
            qDebug() << __FUNCTION__ << "dist_potential_futur_segment[i]=" << dist_potential_futur_segment[i];

        }
    }

    for (int i=0; i<2; i++) {
        if (testDistToPoints_before_after[i]) {
            if (dist_potential_futur_segment[i] < 1.0) {
                qDebug() << __FUNCTION__ << "dist_potential_futur_segment[ " << i << " ]=" << dist_potential_futur_segment[i];

                qDebug() << __FUNCTION__ << " new location too close to another point => rejected new location";
                bRejectedPoint = true;
                return(true); //@LP; be aware of this case: rejection due to distance return true
            }
        }
    }

    return(true); //return false in case of major error like out of bound point index
}

bool Route::checkIndexPointExists(int index) const {
    if (index < 0) {
        qDebug() << __FUNCTION__ << "invalid pointId (index < 0) (" << index;
        return(false);
    }
    if (index >= _qvctqptf_connectedSegmentPoints.size()) {
        qDebug() << __FUNCTION__ << "invalid pointId (index > size)(" << index;
        return(false);
    }
    qDebug() << __FUNCTION__  << "qvctsize = " << _qvctqptf_connectedSegmentPoints.size();
    qDebug() << __FUNCTION__  << "pointId = " << index;
   return(true);
}

bool Route::checkIndexSegmentExists(int index) const {
    if (index < 0) {
        return(false);
    }
    if (index >= segmentCount()) {
        return(false);
    }
    return(true);
}


void Route::showContent() const {
    qDebug() << __FUNCTION__ << " id = " << _id;
    qDebug() << __FUNCTION__ << " name: " << _strName;
    qDebug() << __FUNCTION__ << " points list";
    int idx = 0;    
    for(auto iter: _qvctqptf_connectedSegmentPoints) {
        qDebug() << "  [" << idx << "] " << iter.x() << ", " << iter.y();
        idx++;
    }
    if (!idx)  {
        qDebug() << "  no point in route";
    }
    int nbSegment = segmentCount();
    qDebug() << "  nbSegment = " << nbSegment;
    for(int iLoop = 0; iLoop < nbSegment; iLoop++) {
        S_Segment segment = getSegment(iLoop);
        qDebug() << "    [" << iLoop << "] " <<
                    segment._ptA.x() << ", " << segment._ptA.y() <<
                    " -> " <<
                    segment._ptB.x() << ", " << segment._ptB.y();
    }
}


bool Route::toQJsonObject(QJsonObject &qJsonObjOut) const {

    QString str_routeName_key {"routeName"};
    QString str_routeId_key {"routeId"};

    qDebug() << __FUNCTION__ << _strName;

    qJsonObjOut.insert(str_routeName_key, _strName);
    qJsonObjOut.insert(str_routeId_key, _id);

    QString str_nbPoints_key {"nbPoints"};
    qJsonObjOut.insert(str_nbPoints_key, _qvctqptf_connectedSegmentPoints.size());

    QJsonArray qJsonArrayPoints;
    int pointIdx = 0;
    for(auto iter: _qvctqptf_connectedSegmentPoints) {
        //qDebug() << "  [" << idx << "] " << iter.x() << ", " << iter.y();

        QJsonArray qJsonArray_pointXYvaluesDouble;
        bool bReportPointtoQJsonArray = qrealPoint_toQJsonArray(iter.x(), iter.y(), 7, qJsonArray_pointXYvaluesDouble);
        if (!bReportPointtoQJsonArray) {
            qDebug() << __FUNCTION__ << ": error: formatting double for pointIdx #" << pointIdx;
            return(false);
        }

        qJsonArrayPoints.push_back(qJsonArray_pointXYvaluesDouble);
        qDebug() << __FUNCTION__ << "###" << qJsonObjOut;

        pointIdx++;
    }
    QString str_routePointArray_key {"points"};
    qJsonObjOut.insert(str_routePointArray_key, qJsonArrayPoints);

    qDebug() << "qJsonObjOut: " << qJsonObjOut;
    return(true);
}



bool Route::fromQJsonObject(const QJsonObject& qJsonObj) {

    bool bValueGot = false;

    //get route name
    QString str_routeName_key {"routeName"};
    QString strName;
    bValueGot = getStringValueFromJson(qJsonObj, str_routeName_key, strName);
    if (!bValueGot) {
        return(false);
    }
    strName = strName.trimmed();
    if (strName.isEmpty()) {
        strName = defaultStrRouteName();
    }
    if (!stringValidAs_azAZ09_dotSpace(strName)) { //@LP apply also if defaultStrRouteName() above is used, to prevent dev error
         return(false);
    }
    setName(strName);

    //get route id
    QString str_routeId_key {"routeId"};
    signed int routeId = -1; //qjson_routeName.toInt();
    bValueGot = getSignedIntValueFromJson(qJsonObj, str_routeId_key, routeId);
    if (!bValueGot) {
        return(false);
    }
    if (routeId < 0) {
        qDebug() << __FUNCTION__ << ": error: routeId < 0 (value got: " << routeId << " )";
        return(false);
    }
    setId(routeId);

    //get nbPoint
    QString str_npPoints_key {"nbPoints"};
    int intNbPointToGet = 0;
    bValueGot = getSignedIntValueFromJson(qJsonObj, str_npPoints_key, intNbPointToGet);
    if (!bValueGot) {
        return(false);
    }
    if (intNbPointToGet <= 0) {
        qDebug() << __FUNCTION__ << ": error: getting nbPoint value (" << intNbPointToGet;
        return(false);
    }

    //get point array:
    QString str_routePointArray_key {"points"};
    if (!qJsonObj.contains(str_routePointArray_key)) {
        qDebug() << __FUNCTION__ << ": error: key not found:" << str_routePointArray_key;
        return(false);
    }
    QJsonArray qJsonArrayPoints = qJsonObj.value(str_routePointArray_key).toArray();
    if (qJsonArrayPoints.size() != intNbPointToGet) {
        qDebug() << __FUNCTION__ << ": error: qJsonArrayPoints.size() does not match with intNbPointToGet";
        qDebug() << __FUNCTION__ << ":       " << qJsonArrayPoints.size() << "!= " << intNbPointToGet;
        return(false);
    }

    //check point list format
    for(int pointIdx = 0; pointIdx < intNbPointToGet; pointIdx++) {
        QJsonValue qjsonPointXYForIdx = qJsonArrayPoints.at(pointIdx);
        qDebug() << __FUNCTION__ << qjsonPointXYForIdx;

        if (qjsonPointXYForIdx.isNull() || qjsonPointXYForIdx.isUndefined()) {
            qDebug() << __FUNCTION__ << ": error: x, y value not found for pointIdx #" << pointIdx;
            return(false);
        }
        if (qjsonPointXYForIdx.type() != QJsonValue::Array) {
            qDebug() << __FUNCTION__ << ": error: type error for x, y value for pointIdx #" << pointIdx;
            return(false);
        }

        QJsonArray qjsonArray_pointXY = qjsonPointXYForIdx.toArray();
        if (qjsonArray_pointXY.size() != 2) {
            qDebug() << __FUNCTION__ << ": error: incorrect array size for pointIdx #" << pointIdx << "(got:" << qjsonArray_pointXY.size();
            return(false);
        }

        if (  (qjsonArray_pointXY.at(0).type() != QJsonValue::Double)
            ||(qjsonArray_pointXY.at(1).type() != QJsonValue::Double) ) {
            qDebug() << __FUNCTION__ << ": error: incorrect type(s) for array content for pointIdx #" << pointIdx;
            return(false);
        }

    }

    //@LP distance check, between successive points, 'not below < 1.0' is done in tryAddPointAtRouteEnd(...)

    //at this point the content is considered fully ok
    //replace now the content of the route
    clear();
    for(int pointIdx = 0; pointIdx < intNbPointToGet; pointIdx++) {
        QJsonValue qjsonPointXYForIdx = qJsonArrayPoints.at(pointIdx);//qJsonObj.value(QString::number(pointIdx));
        qDebug() << __FUNCTION__ << qjsonPointXYForIdx;
        QJsonArray qjsonArray_pointXY = qjsonPointXYForIdx.toArray();

        double doubleX = qjsonArray_pointXY.at(0).toDouble();
        double doubleY = qjsonArray_pointXY.at(1).toDouble();

        qDebug() << __FUNCTION__ << "a0:" << doubleX;
        qDebug() << __FUNCTION__ << "a1:" << doubleY;

        QPointF qpf(doubleX, doubleY);

        qDebug() << __FUNCTION__ << "qpf:" << qpf;

        bool bPointSeenAsFirstPoint = false;
        bool bAdded = tryAddPointAtRouteEnd(qpf, bPointSeenAsFirstPoint);
        if (bPointSeenAsFirstPoint) {
            if (!bAdded) {
                qDebug() << " error: should never happen; fail to tryAddPointAtRouteEnd (firstPoint) point#" << pointIdx << " for trace #" << _id;
                return(false);
            }/* else {
                //do nothing
            }*/
        } else {
            if (!bAdded) {
                qDebug() << " error: the distance between point #"<< pointIdx << "and its previous point is < 1.0";
                qDebug() << "        This can occurs typically if the input file was edited outside this sofware";
                qDebug() << "        This is important to have all distances between successives points >= 1.0 to be able to distribute profil boxes along the route";
                clear();
                return(false);
            }
        }
    }

    return(true);
}


#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

bool Route::isPointCloseToASegment(const QPointF& qpointFToTestNOZLI, int currentZLI, bool bFindClosestPointWithDistanceNotBelow1, int& idClosestSegment) const {

    qDebug() << __FUNCTION__ << "qpointFToTestNOZLI = " << qpointFToTestNOZLI;
    qDebug() << __FUNCTION__ << "currentZLI = " << currentZLI;

    qreal widthDiv2perpendicularToSegment = 8.0; // 2.5;

    idClosestSegment = -1;

    S_Segment segment_before_after[2];
    int idOfCentralPoint = -1;

    bool bReport_findClosestSegmentToPos = findSegmentsAroundPos(qpointFToTestNOZLI, bFindClosestPointWithDistanceNotBelow1, segment_before_after, idOfCentralPoint);
    if (!bReport_findClosestSegmentToPos) {
        qDebug() << __FUNCTION__ << "if (!bReport_findClosestSegmentToPos) {";
        return(false);
    }

    bool segmentContainsPos_before_after[2] { false, false };

    for (int i = 0; i < 2; i++) {
        if (segment_before_after[i]._bValid) {
            bool bGetConsideredAreaAroundSegment = false;
            QPolygonF qpolygfOfConsideredAreaAroundSegment;
            segmentContainsPos_before_after[i]= rectangularShapeFromSegment_containsPoint(
                segment_before_after[i], widthDiv2perpendicularToSegment * currentZLI, qpointFToTestNOZLI,
                bGetConsideredAreaAroundSegment, qpolygfOfConsideredAreaAroundSegment);
        }
    }

    if (  !segmentContainsPos_before_after[0]
        &&!segmentContainsPos_before_after[1]) {
        qDebug() << " none of the two segments contains the point";
        return(false);
    }

    if (segmentContainsPos_before_after[0] && segmentContainsPos_before_after[1]) {
        //the two rectangularShapeFromSegment
        //this is possible when the angle between the consecutive segments is tight enough

        //possible solution is to find the smallest distance between the point and its projected point on the lines from the segments
        //@#LP consider this case as minor and return always segment before
        qDebug() << "warning: no smarter processing here, return none segment";
        return(false);
    }

    if (segmentContainsPos_before_after[0]) {
        idClosestSegment = idOfCentralPoint-1;
        return(true);
    }

    idClosestSegment = idOfCentralPoint;
    return(true);
}

bool Route::isPointCloseToTheLastPointOfRoute(const QPointF& pos) const {
    int ptCount = pointCount();
    if (!ptCount) {
        return(false);
    }
    S_Segment segment;
    segment._ptA = pos;
    segment._ptB = _qvctqptf_connectedSegmentPoints.at(ptCount-1);
    segment._bValid = true;
    return(segment.length() < 1.0);
}

bool Route::rectangularShapeFromSegment_containsPoint(
        const S_Segment &segment,
        double widthDiv2perpendicularToSegment,
        const QPointF& qpointFToTest,
        bool bGetConsideredAreaAroundSegment,
        QPolygonF& qpolygfOfConsideredAreaAroundSegment) const {

    if (!segment._bValid) {
        return(false);
    }

    Vec2<double> ptA { segment._ptA.x(), segment._ptA.y()};
    Vec2<double> ptB { segment._ptB.x(), segment._ptB.y()};

    qDebug() << __FUNCTION__ << "segment._ptA:" << segment._ptA;
    qDebug() << __FUNCTION__ << "segment._ptB:" << segment._ptB;

    //vector along segment
    Vec2<double> vecAB {
        ptB.x - ptA.x,
        ptB.y - ptA.y,
    };

    //@#LP this comment needs an update because it seems that the constraint about 1.0 is in fact 1.0/division, with division = 10.0
    //but for point insertion or move, distance not below 1.0 could be considered as a valid feature limit for edition (not a math constraint):
    //
    //  this is important to not have vecAB.length() < 1.0 because the box distribution use 1.0 step to progress along the route
    //  any use cases where vecAB.length() could became < 1.0 need to be handled.
    //  Hence, this should not happens here but here is a safety test:
    if (vecAB.length() < 1.0) {
        qDebug() << __FUNCTION__ << " warning: the distance between segments points is < 1.0";
        return(false);
    }

    //qDebug() << __FUNCTION__ << "vecAB (non):" << vecAB.x << "," << vecAB.y;
    vecAB.normalize();


    //the rectangular shape around the segment take into account the parts where adding a point is not possible:
    Vec2<double> extremityFromA {
        ptA.x,// + vecAB.x,
        ptA.y,// + vecAB.y
    };

    Vec2<double> extremityFromB {
        ptB.x,// - vecAB.x,
        ptB.y,// - vecAB.y
    };

    //qDebug() << __FUNCTION__ << "testing with segment #" << idxpt
    //         << "( " << _qvectPointList[idxpt].x() << "," << _qvectPointList[idxpt].y() << ")"
    //         << "=>"
    //         << "( " << _qvectPointList[idxpt+1].x() << "," << _qvectPointList[idxpt+1].y() << ")";

    //qDebug() << __FUNCTION__ << "vecAB (nor):" << vecAB.x << "," << vecAB.y;

    Vec2<double> vec2dblNormalized_perpendicularToSegmentABCounterclockwise { -vecAB.y , vecAB.x };
    Vec2<double> vec2dblNormalized_perpendicularToSegmentABClockwise  = -vec2dblNormalized_perpendicularToSegmentABCounterclockwise;

    double perpDist = widthDiv2perpendicularToSegment;

    Vec2<double> vec2dbl_point_c  = extremityFromA + perpDist * vec2dblNormalized_perpendicularToSegmentABCounterclockwise;
    Vec2<double> vec2dbl_point_d  = extremityFromA + perpDist * vec2dblNormalized_perpendicularToSegmentABClockwise;

    Vec2<double> vec2dbl_point_e  = extremityFromB + perpDist * vec2dblNormalized_perpendicularToSegmentABCounterclockwise;
    Vec2<double> vec2dbl_point_f  = extremityFromB + perpDist * vec2dblNormalized_perpendicularToSegmentABClockwise;

    //qDebug() << __FUNCTION__ << "vec2dbl_point_c:" << vec2dbl_point_c.x << "," <<  vec2dbl_point_c.y;
    //qDebug() << __FUNCTION__ << "vec2dbl_point_d:" << vec2dbl_point_d.x << "," <<  vec2dbl_point_d.y;
    //qDebug() << __FUNCTION__ << "vec2dbl_point_e:" << vec2dbl_point_e.x << "," <<  vec2dbl_point_e.y;
    //qDebug() << __FUNCTION__ << "vec2dbl_point_f:" << vec2dbl_point_f.x << "," <<  vec2dbl_point_f.y;

    //     qDebug() << __FUNCTION__ << "vec2db_ce (nor):" << vec2db_ce.x << "," << vec2db_ce.y;
    //     qDebug() << __FUNCTION__ << "vec2db_df (nor):" << vec2db_df.x << "," << vec2db_df.y;

    QPolygonF qpolygf;
    //(in counterclockwise point order:)
    qpolygf << QPointF(vec2dbl_point_d.x, vec2dbl_point_d.y);
    qpolygf << QPointF(vec2dbl_point_f.x, vec2dbl_point_f.y);
    qpolygf << QPointF(vec2dbl_point_e.x, vec2dbl_point_e.y);
    qpolygf << QPointF(vec2dbl_point_c.x, vec2dbl_point_c.y);

    qDebug() << __FUNCTION__ << "qpolygf:" << qpolygf;

    qDebug() << __FUNCTION__ << "width (#1) of rectangular shape around segment: " << sqrt(
                 ((vec2dbl_point_c.x - vec2dbl_point_d.x)*(vec2dbl_point_c.x - vec2dbl_point_d.x))
                +((vec2dbl_point_c.y - vec2dbl_point_d.y)*(vec2dbl_point_c.y - vec2dbl_point_d.y)));
    qDebug() << __FUNCTION__ << "width (#2) of rectangular shape around segment: " << sqrt(
                 ((vec2dbl_point_e.x - vec2dbl_point_f.x)*(vec2dbl_point_e.x - vec2dbl_point_f.x))
                +((vec2dbl_point_e.y - vec2dbl_point_f.y)*(vec2dbl_point_e.y - vec2dbl_point_f.y)));

    qDebug() << __FUNCTION__ << "length (#1) of rectangular shape around segment: " << sqrt(
                 ((vec2dbl_point_c.x - vec2dbl_point_e.x)*(vec2dbl_point_c.x - vec2dbl_point_e.x))
                +((vec2dbl_point_c.y - vec2dbl_point_e.y)*(vec2dbl_point_c.y - vec2dbl_point_e.y)));
    qDebug() << __FUNCTION__ << "length (#2) of rectangular shape around segment: " << sqrt(
                 ((vec2dbl_point_d.x - vec2dbl_point_f.x)*(vec2dbl_point_d.x - vec2dbl_point_f.x))
                +((vec2dbl_point_d.y - vec2dbl_point_f.y)*(vec2dbl_point_d.y - vec2dbl_point_f.y)));


    if (bGetConsideredAreaAroundSegment) {
        qpolygfOfConsideredAreaAroundSegment = qpolygf;
    }

    return(qpolygf.containsPoint(qpointFToTest,Qt::FillRule::OddEvenFill));

}

//the way to distribute boxes along the route implies to have distance between any successives segments extremities always equal or greater than 1.0
//(#LP comment to update => 1.0 / step division)
//stay on 1.0 minimum for now
bool Route::findSegmentsAroundPos(const QPointF& pos, bool bFindClosestPointWithDistanceNotBelow1, S_Segment segment_before_after[2], int& idOfCentralPoint) const {

    if (!_segmentCount) {
        return(false);
    }

    idOfCentralPoint = -1;
    bool bDistanceToClosestPointIsBelow1 = false;
    bool bReportFoundClosestPointToPos = findClosestPointToPos_butWithDistanceNotBelow1(pos, idOfCentralPoint, bDistanceToClosestPointIsBelow1);
    if (!bReportFoundClosestPointToPos) {
          qDebug() << __FUNCTION__ << "no point to test in findClosestPointToPos_butWithDistanceNotBelow1()";
          return(false);
    }
    if (idOfCentralPoint == -1) {
        qDebug() << __FUNCTION__ << "no closest point found with distance > 1.0 in findClosestPointToPos_butWithDistanceNotBelow1()";
        return(false);
    }

    if (bFindClosestPointWithDistanceNotBelow1) {
        if (bDistanceToClosestPointIsBelow1) {
            qDebug() << __FUNCTION__ << "bFindClosestPointWithDistanceNotBelow1 && bDistanceToClosestPointIsBelow1 fail case";
            return(false);
        }
    }

    qDebug() << __FUNCTION__ << "idOfCentralPoint = " << idOfCentralPoint;

    segment_before_after[0]= getSegment(idOfCentralPoint-1);
    segment_before_after[1]= getSegment(idOfCentralPoint);

    qDebug() << __FUNCTION__ << " segment_before_after[0]._bValid = " << segment_before_after[0]._bValid;
    qDebug() << __FUNCTION__ << " segment_before_after[0]._ptA = " << segment_before_after[0]._ptA;
    qDebug() << __FUNCTION__ << " segment_before_after[0]._ptB = " << segment_before_after[0]._ptB;

    qDebug() << __FUNCTION__ << " segment_before_after[1]._bValid = " << segment_before_after[1]._bValid;
    qDebug() << __FUNCTION__ << " segment_before_after[1]._ptA = " << segment_before_after[1]._ptA;
    qDebug() << __FUNCTION__ << " segment_before_after[1]._ptB = " << segment_before_after[1]._ptB;

    return(segment_before_after[0]._bValid || segment_before_after[1]._bValid);

}


//the way to distribute boxes along the route implies to have distance between any successives segments extremities always equal or greater than 1.0
bool Route::findClosestPointToPos_butWithDistanceNotBelow1(const QPointF& pos, int& idPoint, bool& bDistanceToClosestPointIsBelow1) const { //for intersection point-rectangle dev-test

    if (!pointCount()) {
        return(false);
    }

    double distanceToPoint = 9999999;
    idPoint = -1;

    //@LP first way, simple way, but with a missing use case: point close to a segment line but closer to a point extremity of an another segment
    Vec2<double> vecPtA { pos.x() , pos.y() };

    int idIterPoint = 0;
    for (const auto& iter:_qvctqptf_connectedSegmentPoints) {

        Vec2<double> vec2dblAB {
            iter.x() - vecPtA.x,
            iter.y() - vecPtA.y
        };
        double distAB = vec2dblAB.length();

        if (distAB < distanceToPoint) {
            distanceToPoint = distAB;
            idPoint = idIterPoint;
            qDebug() << __FUNCTION__ << "pos :" << pos << " => loop closest point: " << iter << " distance: " << distanceToPoint << " idPoint <= " << idPoint;
        }
        idIterPoint++;
    }

    bDistanceToClosestPointIsBelow1 = (distanceToPoint < 1.0);
    return(true);
}


//method used to insert a center point along the route
//

bool Route::findRelocatedPointAlongRouteForAFarPoint(const QPointF& qpfPos, QPointF& qpfRelocated, int& idxOfSegmentOwner) const {



    S_Segment segment_before_after[2];
    int idOfCentralPoint = -1;

    bool bReport_findClosestSegmentToPos = findSegmentsAroundPos(qpfPos, false, segment_before_after, idOfCentralPoint);
    if (!bReport_findClosestSegmentToPos) {
        qDebug() << __FUNCTION__ << "if (!bReport_findClosestSegmentToPos) {";
        return(false);
    }



    //try to project the point on the two segments

    QPointF relocatedPoint_onSegments_before_after[2] = { {.0,.0},
                                                          {.0,.0} };
    e_ResultDetailsOfRelocatedPointInSegment eRDORPIS_before_after[2] = { e_RDORPIS_notSet, e_RDORPIS_notSet };

    qDebug() << __FUNCTION__ << "idOfCentralPoint = " << idOfCentralPoint;

    //relocate the qpfNoZLI in the closest segments
    if (segment_before_after[0]._bValid) {
        qDebug() << __FUNCTION__ << "if (segment_before_after[0]._bValid) { call relocatePointInSegment(...) with: qpfPos = " << qpfPos
                 << "; getSegment(idOfCentralPoint-1) = [A,B]: "
                 << getSegment(idOfCentralPoint-1)._ptA << " , " << getSegment(idOfCentralPoint-1)._ptB;

        eRDORPIS_before_after[0] = relocatePointInSegment(qpfPos, getSegment(idOfCentralPoint-1), relocatedPoint_onSegments_before_after[0]);
    }

    if (segment_before_after[1]._bValid) {
        qDebug() << __FUNCTION__ << "segment_before_after[1]._bValid) { call relocatePointInSegment(...) with: qpfPos = " << qpfPos
                 << "; getSegment(idOfCentralPoint) = [A,B]: "
                 << getSegment(idOfCentralPoint)._ptA << " , " << getSegment(idOfCentralPoint)._ptB;

        eRDORPIS_before_after[1] = relocatePointInSegment(qpfPos, getSegment(idOfCentralPoint), relocatedPoint_onSegments_before_after[1]);
    }

    if (  (eRDORPIS_before_after[0] <= e_RDORPIS_notRelocated_BisCloser)
        &&(eRDORPIS_before_after[1] <= e_RDORPIS_notRelocated_BisCloser)) {        
        qDebug() << __FUNCTION__ << "eRDORPIS_before_after[0] and [1] <= BisCloser";
        return(false);
    }

    if (  (eRDORPIS_before_after[0] > e_RDORPIS_notRelocated_BisCloser)
        &&(eRDORPIS_before_after[1] > e_RDORPIS_notRelocated_BisCloser)) {

        //considers the closest projected point between the two as the result

        S_Segment segment_pos_to_projectedPointOnSegmentBefore;
        segment_pos_to_projectedPointOnSegmentBefore._ptA = qpfPos;
        segment_pos_to_projectedPointOnSegmentBefore._ptB = relocatedPoint_onSegments_before_after[0];
        segment_pos_to_projectedPointOnSegmentBefore._bValid = true;
        double distance_pos_to_projectedPointOnSegmentBefore = segment_pos_to_projectedPointOnSegmentBefore.length();

        S_Segment segment_pos_to_projectedPointOnSegmentAfter;
        segment_pos_to_projectedPointOnSegmentAfter._ptA = qpfPos;
        segment_pos_to_projectedPointOnSegmentAfter._ptB = relocatedPoint_onSegments_before_after[1];
        segment_pos_to_projectedPointOnSegmentAfter._bValid = true;

        double distance_pos_to_projectedPointOnSegmentAfter = segment_pos_to_projectedPointOnSegmentAfter.length();

        if ( distance_pos_to_projectedPointOnSegmentBefore <= distance_pos_to_projectedPointOnSegmentAfter) {
            qpfRelocated = relocatedPoint_onSegments_before_after[0];
            idxOfSegmentOwner = idOfCentralPoint-1;
        } else {
            qpfRelocated = relocatedPoint_onSegments_before_after[1];
            idxOfSegmentOwner = idOfCentralPoint;
        }
        return(true);
    }

    if (eRDORPIS_before_after[0] > e_RDORPIS_notRelocated_BisCloser) {
        qpfRelocated = relocatedPoint_onSegments_before_after[0];
        idxOfSegmentOwner = idOfCentralPoint-1;
        return(true);
    }


    qpfRelocated = relocatedPoint_onSegments_before_after[1];
    idxOfSegmentOwner = idOfCentralPoint;
    return(true);
}


//followed direction is from the route first point to the last point
bool Route::findPointAtADistanceFromAnotherPoint_forwardDirection(
    const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint, qreal distanceFromStartingPoint,
    QPointF& qpfPointAtDistance, int& idxSegmentOfPointAtDistance) {

    qpfPointAtDistance = {.0, .0};
    idxSegmentOfPointAtDistance = -1;

    int segCount = segmentCount();
    if (!segCount) {
        return(false);
    }

    if (  (idxSegmentOfStartingPoint < 0)
        ||(idxSegmentOfStartingPoint >= segCount)) {
        return(false);
    }

    qreal startingSeg_startToB_length = .0;
    S_Segment startingSeg_startToB;
    startingSeg_startToB._ptA = qpfStartingPoint;
    startingSeg_startToB._ptB = getSegment(idxSegmentOfStartingPoint)._ptB;
    startingSeg_startToB._bValid = true;
    startingSeg_startToB_length = startingSeg_startToB.length();

    double distanceFromStartExcludingSegmentContainingPointAtDistance = .0;
    double distanceFromStart = .0;


    int idxSegmentContainingPointAtDistance = -1;

    for (int iSeg = idxSegmentOfStartingPoint; iSeg < segCount; iSeg++) {

        if (iSeg == idxSegmentOfStartingPoint) {
            distanceFromStart += startingSeg_startToB_length;
        } else {
            S_Segment segCurrent = getSegment(iSeg);
            distanceFromStart += segCurrent.length();
        }

        if (distanceFromStart > distanceFromStartingPoint ) {
            idxSegmentContainingPointAtDistance = iSeg;
            break;
        }
        distanceFromStartExcludingSegmentContainingPointAtDistance = distanceFromStart;
    }

    if (idxSegmentContainingPointAtDistance == -1) {
        return(false);
    }

    S_Segment segmentABofSegmentContainingPointAtDistance = getSegment(idxSegmentContainingPointAtDistance);
    Vec2<double> vecABofSegmentContainingPointAtDistance {
        segmentABofSegmentContainingPointAtDistance._ptB.x() - segmentABofSegmentContainingPointAtDistance._ptA.x(),
        segmentABofSegmentContainingPointAtDistance._ptB.y() - segmentABofSegmentContainingPointAtDistance._ptA.y(),
    };

    Vec2<double> vecABofSegmentContaingPointNormalized = vecABofSegmentContainingPointAtDistance.normalized();

    if (idxSegmentContainingPointAtDistance == idxSegmentOfStartingPoint) {

        qpfPointAtDistance = {qpfStartingPoint.x() + distanceFromStartingPoint * vecABofSegmentContaingPointNormalized.x,
                              qpfStartingPoint.y() + distanceFromStartingPoint * vecABofSegmentContaingPointNormalized.y};

        idxSegmentOfPointAtDistance = idxSegmentContainingPointAtDistance;
        return(true);
    }


    double distanceFromPtA_inSegmentContainingPointAtDistance_toReachPoint = distanceFromStartingPoint - distanceFromStartExcludingSegmentContainingPointAtDistance;

    //qDebug() << __FUNCTION__ << "vecABofSegmentContaingMiddle: " << vecABofSegmentContaingMiddle.x << ", " << vecABofSegmentContaingMiddle.y;

    Vec2<double> vec2_ptAofSegmentContainingPoint {
        segmentABofSegmentContainingPointAtDistance._ptA.x(),
        segmentABofSegmentContainingPointAtDistance._ptA.y()
    };

    Vec2<double> vec2_PointAtDistanceFromPtA = vec2_ptAofSegmentContainingPoint + distanceFromPtA_inSegmentContainingPointAtDistance_toReachPoint * vecABofSegmentContaingPointNormalized;

    qpfPointAtDistance = {vec2_PointAtDistanceFromPtA.x, vec2_PointAtDistanceFromPtA.y};
    idxSegmentOfPointAtDistance = idxSegmentContainingPointAtDistance;

    return(true);

}



//followed direction is from the route last point to the first point
bool Route::findPointAtADistanceFromAnotherPoint_backwardDirection(
    const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint, qreal distanceFromStartingPoint,
    QPointF& qpfPointAtDistance, int& idxSegmentOfPointAtDistance) {

    qpfPointAtDistance = {.0, .0};
    idxSegmentOfPointAtDistance = -1;

    int segCount = segmentCount();
    if (!segCount) {
        return(false);
    }

    if (  (idxSegmentOfStartingPoint < 0)
        ||(idxSegmentOfStartingPoint >= segCount)) {
        return(false);
    }

    qreal startingSeg_startToA_length = .0;
    S_Segment startingSeg_startToA;
    startingSeg_startToA._ptA = getSegment(idxSegmentOfStartingPoint)._ptA;
    startingSeg_startToA._ptB = qpfStartingPoint;
    startingSeg_startToA._bValid = true;
    startingSeg_startToA_length = startingSeg_startToA.length();

    double distanceFromStartExcludingSegmentContainingPointAtDistance = .0;
    double distanceFromStart = .0;


    int idxSegmentContainingPointAtDistance = -1;

    for (int iSeg = idxSegmentOfStartingPoint; iSeg >= 0; iSeg--) {

        if (iSeg == idxSegmentOfStartingPoint) {
            distanceFromStart += startingSeg_startToA_length;
        } else {
            S_Segment segCurrent = getSegment(iSeg);
            distanceFromStart += segCurrent.length();
        }

        if (distanceFromStart > distanceFromStartingPoint ) {
            idxSegmentContainingPointAtDistance = iSeg;
            break;
        }
        distanceFromStartExcludingSegmentContainingPointAtDistance = distanceFromStart;
    }

    if (distanceFromStart < distanceFromStartingPoint) {
        return(false);
    }

    S_Segment segmentABofSegmentContainingPointAtDistance = getSegment(idxSegmentContainingPointAtDistance);
    //we use direction vector from B to A here
    Vec2<double> vecBAofSegmentContainingPointAtDistance {
        segmentABofSegmentContainingPointAtDistance._ptA.x() - segmentABofSegmentContainingPointAtDistance._ptB.x(),
        segmentABofSegmentContainingPointAtDistance._ptA.y() - segmentABofSegmentContainingPointAtDistance._ptB.y(),
    };

    Vec2<double> vecBAofSegmentContaingPointNormalized = vecBAofSegmentContainingPointAtDistance.normalized();

    if (idxSegmentContainingPointAtDistance == idxSegmentOfStartingPoint) {

        qpfPointAtDistance = {qpfStartingPoint.x() + distanceFromStartingPoint * vecBAofSegmentContaingPointNormalized.x,
                              qpfStartingPoint.y() + distanceFromStartingPoint * vecBAofSegmentContaingPointNormalized.y};

        idxSegmentOfPointAtDistance = idxSegmentContainingPointAtDistance;
        return(true);
    }


    double distanceFromPtB_inSegmentContainingPointAtDistance_toReachPoint = distanceFromStartingPoint - distanceFromStartExcludingSegmentContainingPointAtDistance;

    //qDebug() << __FUNCTION__ << "vecABofSegmentContaingMiddle: " << vecABofSegmentContaingMiddle.x << ", " << vecABofSegmentContaingMiddle.y;

    Vec2<double> vec2_ptBofSegmentContainingPoint {
        segmentABofSegmentContainingPointAtDistance._ptB.x(),
        segmentABofSegmentContainingPointAtDistance._ptB.y()
    };

    Vec2<double> vec2_PointAtDistanceFromPtB = vec2_ptBofSegmentContainingPoint + distanceFromPtB_inSegmentContainingPointAtDistance_toReachPoint * vecBAofSegmentContaingPointNormalized;

    qpfPointAtDistance = {vec2_PointAtDistanceFromPtB.x, vec2_PointAtDistanceFromPtB.y};
    idxSegmentOfPointAtDistance = idxSegmentContainingPointAtDistance;

    return(true);

}


bool Route::findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
    const QPointF& qpfStartingPoint, int idxSegmentOfStartingPoint,
    const QPointF& qpfEndingPoint, int idxSegmentOfEndingPoint,
    QPointF& qpfMiddlePoint, int& idxSegmentOfMiddlePoint,
    bool& bDistanceFromMiddlePointToOthersIsUnderLimit) {

    qDebug() << __FUNCTION__ << " entering";
    qDebug() << __FUNCTION__ << " qpfStartingPoint = " << qpfStartingPoint << " idxSegmentOfStartingPoint = " << idxSegmentOfStartingPoint;
    qDebug() << __FUNCTION__ << " qpfEndingPoint   = " << qpfEndingPoint   << " idxSegmentOfEndingPoint   = " << idxSegmentOfEndingPoint;


    int segCount = segmentCount();
    if (!segCount) {
       qDebug() << __FUNCTION__ << " if (!segCount) {";
       return(false);
    }
    if (!checkIndexSegmentExists(idxSegmentOfStartingPoint)) {
        qDebug() << __FUNCTION__ << " if  (!checkIndexSegmentExists(idxSegmentOfStartingPoint))";
        return(false);
    }
    if (!checkIndexSegmentExists(idxSegmentOfEndingPoint)) {
        qDebug() << __FUNCTION__ << " if  (!checkIndexSegmentExists(idxSegmentOfEndingPoint)) {";
        return(false);
    }
    if (idxSegmentOfStartingPoint > idxSegmentOfEndingPoint) {
        qDebug() << __FUNCTION__ << " if (idxSegmentOfStartingPoint > idxSegmentOfEndingPoint) {";
        return(false);
    }

    double routeLengthStartingPointToEndingPoint = .0;

    double startingSeg_startToB_length = .0;
    double endingSeg_AtoEnd_length = .0;


    double minLengthLimit = 0.001;

    //same segment case
    if (idxSegmentOfStartingPoint == idxSegmentOfEndingPoint) {
        S_Segment seg;
        seg._ptA = qpfStartingPoint;
        seg._ptB = qpfEndingPoint;
        seg._bValid = true;
        routeLengthStartingPointToEndingPoint = seg.length();

        qDebug() << __FUNCTION__ << " if (idxSegmentOfStartingPoint == idxSegmentOfEndingPoint) {";
        qDebug() << __FUNCTION__ << " ( idxSegmentOfStartingPoint == " << idxSegmentOfStartingPoint;
        qDebug() << __FUNCTION__ << " ( idxSegmentOfEndingPoint == " << idxSegmentOfEndingPoint;

        bool bUnderLimit = false;
        //check if length from start to end is under the limit
        if (routeLengthStartingPointToEndingPoint <= minLengthLimit) {
            bUnderLimit = true;
        }

        //compute middle point
        //if routeLengthStartingPointToEndingPoint is under limit, the segmentMiddleToOthers will be also
        //but if routeLengthStartingPointToEndingPoint is over limit, the segmentMiddleToOthers can be under
        qpfMiddlePoint =  (qpfStartingPoint + qpfEndingPoint) / 2.0;
        idxSegmentOfMiddlePoint = idxSegmentOfStartingPoint;

        S_Segment segmentMiddleToOthers;
        segmentMiddleToOthers._ptA = qpfMiddlePoint;
        segmentMiddleToOthers._ptB = qpfStartingPoint;
        segmentMiddleToOthers._bValid = true;

        //check if middle is under the limit (when length from ptA to ptB is > minLengthLimit)
        bUnderLimit = (segmentMiddleToOthers.length() <= minLengthLimit);

        bDistanceFromMiddlePointToOthersIsUnderLimit = bUnderLimit;

        qDebug() << __FUNCTION__ << " final #1 qpfMiddlePoint:" << qpfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #1 idxSegmentOfMiddlePoint: " << idxSegmentOfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #1 bDistanceFromMiddlePointToOthersIsUnderLimit set to " << bDistanceFromMiddlePointToOthersIsUnderLimit;

        return(true);
    }


    qDebug() << __FUNCTION__ << " ( if (idxSegmentOfStartingPoint != idxSegmentOfEndingPoint) { )";
    qDebug() << __FUNCTION__ << " idxSegmentOfStartingPoint = " << idxSegmentOfStartingPoint;
    qDebug() << __FUNCTION__ << " idxSegmentOfEndingPoint   = " << idxSegmentOfEndingPoint;

    //compute the length between the start and end point

    //exemple:
    // with:
    //  + : points
    //  S : qpfStartingPoint
    //  E : qpfEndingPoint
    //
    // segments    +-----S--+---------+----+--------+--E-------------+
    // idxsegment: +   #0   +   #1    + #2 +   #3   +       #4       +
    //             +        +         +    +        +                +
    //             +     xxxx                       zzzz
    //
    // startingSeg_startToB_length : S to segment#0._ptB <=> xxx above
    // endingSeg_AtoEnd_length     : segment#4._ptA to E <=> zzz above
    //
    //

    qDebug() << __FUNCTION__ << " routeLengthStartingPointToEndingPoint = " << routeLengthStartingPointToEndingPoint;

    S_Segment startingSeg_startToB;
    startingSeg_startToB._ptA = qpfStartingPoint;
    startingSeg_startToB._ptB = getSegment(idxSegmentOfStartingPoint)._ptB;
    startingSeg_startToB._bValid = true;
    startingSeg_startToB_length = startingSeg_startToB.length();
    routeLengthStartingPointToEndingPoint += startingSeg_startToB_length;

    qDebug() << __FUNCTION__ << " startingSeg_startToB._ptA: " << startingSeg_startToB._ptA;
    qDebug() << __FUNCTION__ << " startingSeg_startToB._ptB: " << startingSeg_startToB._ptB;

    qDebug() << __FUNCTION__ << " startingSeg_startToB_length: " << startingSeg_startToB_length;
    qDebug() << __FUNCTION__ << " routeLengthStartingPointToEndingPoint += startingSeg_startToB_length " << routeLengthStartingPointToEndingPoint;

    for (int iSeg = idxSegmentOfStartingPoint+1; iSeg < idxSegmentOfEndingPoint; iSeg++) {
        S_Segment segCurrent = getSegment(iSeg);
        routeLengthStartingPointToEndingPoint += segCurrent.length();
        qDebug() << __FUNCTION__ << " iSeg = " << iSeg << ": add segCurrent.length() (" << segCurrent.length() << ") to routeLengthStartingPointToEndingPoint: => " << routeLengthStartingPointToEndingPoint;
    }
    qDebug() << __FUNCTION__ << " after loop: routeLengthStartingPointToEndingPoint = " << routeLengthStartingPointToEndingPoint;

    S_Segment endingSeg_AtoEnd;
    endingSeg_AtoEnd._ptA = getSegment(idxSegmentOfEndingPoint)._ptA;
    endingSeg_AtoEnd._ptB = qpfEndingPoint;
    endingSeg_AtoEnd._bValid = true;
    endingSeg_AtoEnd_length = endingSeg_AtoEnd.length();
    routeLengthStartingPointToEndingPoint += endingSeg_AtoEnd_length;

    qDebug() << __FUNCTION__ << " endingSeg_AtoEnd._ptA: " << endingSeg_AtoEnd._ptA;
    qDebug() << __FUNCTION__ << " endingSeg_AtoEnd._ptB: " << endingSeg_AtoEnd._ptB;

    qDebug() << __FUNCTION__ << " endingSeg_AtoEnd_length: " << endingSeg_AtoEnd_length;
    qDebug() << __FUNCTION__ << " routeLengthStartingPointToEndingPoint += endingSeg_AtoEnd_length => " << routeLengthStartingPointToEndingPoint;

    //check if length from start to end is under the limit
    if (routeLengthStartingPointToEndingPoint <= minLengthLimit) {
        qDebug() << __FUNCTION__ << " if (routeLengthStartingPointToEndingPoint <= minLengthLimit) {";

        if (idxSegmentOfStartingPoint+1 != idxSegmentOfEndingPoint) { //lost last chance to find a middle
            qDebug() << __FUNCTION__ << " if (idxSegmentOfStartingPoint+1 != idxSegmentOfEndingPoint) {";
            return(false); //this case can only be about an error about the trace content; it should never happen.
        }

        //the start and end point are very close; one on a segment and the other on the following segment
        //Considers the end of the segment at start as the transition point
        qpfMiddlePoint = getSegment(idxSegmentOfStartingPoint)._ptB;
        idxSegmentOfMiddlePoint = idxSegmentOfStartingPoint;
        bDistanceFromMiddlePointToOthersIsUnderLimit = true;

        qDebug() << __FUNCTION__ << " final #2 qpfMiddlePoint:" << qpfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #2 idxSegmentOfMiddlePoint: " << idxSegmentOfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #2 bDistanceFromMiddlePointToOthersIsUnderLimit set to " << bDistanceFromMiddlePointToOthersIsUnderLimit;

        return(true);
    }

    qDebug() << __FUNCTION__ << " search the middle along the route";

    //search the middle along the route
    double middleLength = routeLengthStartingPointToEndingPoint / 2.0;

    qDebug() << __FUNCTION__ << " routeLengthStartingPointToEndingPoint =" << routeLengthStartingPointToEndingPoint;
    qDebug() << __FUNCTION__ << " middleLength =" << middleLength;

    double distanceFromStartExcludingSegmentContainingMiddle = .0;
    double distanceFromStart = .0;

    bool bSegmentContainingMiddleIsSegmentOfStartingPoint = false;
    bool bSegmentContainingMiddleIsSegmentOfEndingPoint = false;

    //bool bIdxSegmentContainingMiddle = false;
    int idxSegmentContainingMiddle = -1;

    for (int iSeg = idxSegmentOfStartingPoint; iSeg <= idxSegmentOfEndingPoint; iSeg++) {

        qDebug() << __FUNCTION__ << " iSeg = " << iSeg;

        if (iSeg == idxSegmentOfStartingPoint) {            
            distanceFromStart += startingSeg_startToB_length;

            qDebug() << __FUNCTION__ << " #c1 if (iSeg == idxSegmentOfStartingPoint) {";
            qDebug() << __FUNCTION__ << " #c1 distanceFromStart += " << startingSeg_startToB_length << " => " << distanceFromStart;

        } else {
            if (iSeg == idxSegmentOfEndingPoint) {
                distanceFromStart += endingSeg_AtoEnd_length;

                qDebug() << __FUNCTION__ << " #c2 if (iSeg == idxSegmentOfEndingPoint) {";
                qDebug() << __FUNCTION__ << " #c2 distanceFromStart += " << endingSeg_AtoEnd_length << " => " << endingSeg_AtoEnd_length;

            } else {

                S_Segment segCurrent = getSegment(iSeg);
                distanceFromStart += segCurrent.length();

                qDebug() << __FUNCTION__ << " #c3 iSeg is a segment inside (not at an extremity)";
                qDebug() << __FUNCTION__ << " #c3 distanceFromStart += " << segCurrent.length() << " => " << distanceFromStart;
            }
        }

        if (distanceFromStart > middleLength ) {
            idxSegmentContainingMiddle = iSeg;
            bSegmentContainingMiddleIsSegmentOfStartingPoint = (iSeg == idxSegmentOfStartingPoint);
            bSegmentContainingMiddleIsSegmentOfEndingPoint   = (iSeg == idxSegmentOfEndingPoint);
            qDebug() << __FUNCTION__ << " if (distanceFromStart > middleLength ) {";
            break;            
        }
        distanceFromStartExcludingSegmentContainingMiddle = distanceFromStart;
        qDebug() << __FUNCTION__ << "distanceFromStartExcludingSegmentContainingMiddle = " << distanceFromStartExcludingSegmentContainingMiddle;
    }


    //case bSegmentContainingMiddleIsSegmentOfStartingPoint at true and bSegmentContainingMiddleIsSegmentOfEndingPoint at true
    //should never happens as this case is handled in a specific case above.

    if (bSegmentContainingMiddleIsSegmentOfStartingPoint) {


        double distanceFromStartingPoint_inSegmentContainingMiddle_toReachMiddle = middleLength;

        S_Segment segmentABofSegmentContainingMiddle = getSegment(idxSegmentContainingMiddle);
        Vec2<double> vecABofSegmentContainingMiddle {
            segmentABofSegmentContainingMiddle._ptB.x() - segmentABofSegmentContainingMiddle._ptA.x(),
            segmentABofSegmentContainingMiddle._ptB.y() - segmentABofSegmentContainingMiddle._ptA.y(),
        };
        Vec2<double> vecABofSegmentContaingMiddleNormalized = vecABofSegmentContainingMiddle.normalized();

        Vec2<double> vec2_startingPoint {
            qpfStartingPoint.x(),
            qpfStartingPoint.y()
        };
        qDebug() << __FUNCTION__ << "vec2_startingPoint = " << vec2_startingPoint.x << ", " << vec2_startingPoint.y;

        Vec2<double> vec2add = distanceFromStartingPoint_inSegmentContainingMiddle_toReachMiddle * vecABofSegmentContaingMiddleNormalized;
        qDebug() << __FUNCTION__ << "vec2add = " << vec2add.x << ", " << vec2add.y;

        Vec2<double> vec2_middle = vec2_startingPoint + vec2add;

        qDebug() << __FUNCTION__ << "vec2_middle = " << vec2_middle.x << ", " << vec2_middle.y;

        qpfMiddlePoint = {vec2_middle.x, vec2_middle.y};
        idxSegmentOfMiddlePoint = idxSegmentContainingMiddle;

        qDebug() << __FUNCTION__ << "qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << __FUNCTION__ << "idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        //as minLengthLimit is very low, we just check the euclidian distance from one of the point, without any route segment consideration.
        S_Segment segmentMiddleToOthers;
        segmentMiddleToOthers._ptA = qpfMiddlePoint;
        segmentMiddleToOthers._ptB = qpfStartingPoint;
        segmentMiddleToOthers._bValid = true;

        bDistanceFromMiddlePointToOthersIsUnderLimit = (segmentMiddleToOthers.length() < minLengthLimit);

        qDebug() << __FUNCTION__ << " final #4 qpfMiddlePoint:" << qpfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #4 idxSegmentOfMiddlePoint: " << idxSegmentOfMiddlePoint;
        qDebug() << __FUNCTION__ << " final #4 ( segmentMiddleToOthers.length() = " << segmentMiddleToOthers.length() << " )";
        qDebug() << __FUNCTION__ << " final #4 bDistanceFromMiddlePointToOthersIsUnderLimit set to " << bDistanceFromMiddlePointToOthersIsUnderLimit;

        return(true);

    } else {

            double distanceFromPtA_inSegmentContainingMiddle_toReachMiddle = middleLength - distanceFromStartExcludingSegmentContainingMiddle;

            qDebug() << __FUNCTION__ << "distanceFromPtA_inSegmentContainingMiddle_toReachMiddle = " << distanceFromPtA_inSegmentContainingMiddle_toReachMiddle;

            S_Segment segmentABofSegmentContainingMiddle = getSegment(idxSegmentContainingMiddle);
            Vec2<double> vecABofSegmentContainingMiddle {
                segmentABofSegmentContainingMiddle._ptB.x() - segmentABofSegmentContainingMiddle._ptA.x(),
                segmentABofSegmentContainingMiddle._ptB.y() - segmentABofSegmentContainingMiddle._ptA.y(),
            };

            //qDebug() << __FUNCTION__ << "vecABofSegmentContaingMiddle: " << vecABofSegmentContaingMiddle.x << ", " << vecABofSegmentContaingMiddle.y;

            Vec2<double> vecABofSegmentContaingMiddleNormalized = vecABofSegmentContainingMiddle.normalized();
            qDebug() << __FUNCTION__ << "vecABofSegmentContaingMiddleNormalized = " << vecABofSegmentContaingMiddleNormalized.x << ", " << vecABofSegmentContaingMiddleNormalized.y;

            Vec2<double> vec2_ptAofSegmentContainingMiddle {
                segmentABofSegmentContainingMiddle._ptA.x(),
                segmentABofSegmentContainingMiddle._ptA.y()
            };
            qDebug() << __FUNCTION__ << "vec2_ptAofSegmentContainingMiddle = " << vec2_ptAofSegmentContainingMiddle.x << ", " << vec2_ptAofSegmentContainingMiddle.y;

            Vec2<double> vec2add = distanceFromPtA_inSegmentContainingMiddle_toReachMiddle * vecABofSegmentContaingMiddleNormalized;
            qDebug() << __FUNCTION__ << "vec2add = " << vec2add.x << ", " << vec2add.y;

            Vec2<double> vec2_middle = vec2_ptAofSegmentContainingMiddle + vec2add;

            qDebug() << __FUNCTION__ << "vec2_middle = " << vec2_middle.x << ", " << vec2_middle.y;

            qpfMiddlePoint = {vec2_middle.x, vec2_middle.y};
            idxSegmentOfMiddlePoint = idxSegmentContainingMiddle;

            qDebug() << __FUNCTION__ << "qpfMiddlePoint = " << qpfMiddlePoint;
            qDebug() << __FUNCTION__ << "idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

            //as minLengthLimit is very low, we just check the euclidian distance from one of the point, without any route segment consideration.
            S_Segment segmentMiddleToOthers;
            segmentMiddleToOthers._ptA = qpfMiddlePoint;
            segmentMiddleToOthers._ptB = qpfStartingPoint;
            segmentMiddleToOthers._bValid = true;

            bDistanceFromMiddlePointToOthersIsUnderLimit = (segmentMiddleToOthers.length() < minLengthLimit);

            qDebug() << __FUNCTION__ << " final #3 qpfMiddlePoint:" << qpfMiddlePoint;
            qDebug() << __FUNCTION__ << " final #3 idxSegmentOfMiddlePoint: " << idxSegmentOfMiddlePoint;
            qDebug() << __FUNCTION__ << " final #3 ( segmentMiddleToOthers.length() = " << segmentMiddleToOthers.length() << " )";
            qDebug() << __FUNCTION__ << " final #3 bDistanceFromMiddlePointToOthersIsUnderLimit set to " << bDistanceFromMiddlePointToOthersIsUnderLimit;

    }

    return(true);
}


//followed direction is from the route first point to the last point
bool Route::computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection(
    const QPointF& qpfRoutePoint, int idxSegmentOfRoutePoint,
    qreal &distanceFromStartingRoutePointToRoutePoint) const {

    distanceFromStartingRoutePointToRoutePoint = .0;

    int segCount = segmentCount();
    qDebug() << __FUNCTION__ << "segCount = " << segCount;

    if (!segCount) {
        qDebug() << __FUNCTION__ << "if (!segCount) {";
        return(false);
    }

    if (  (idxSegmentOfRoutePoint < 0)
        ||(idxSegmentOfRoutePoint >= segCount)) {
        qDebug() << __FUNCTION__ << "idxSegmentOfRoutePoint out of range";
        return(false);
    }


    //--------------------
    //As the center point is computed from the application, checking that the point is on the segment should not be useful
    //Add check as a shield

    QPointF qpfRelocatedPoint {999999,9999999};//999999 as invalid
    S_Segment segmentForRelocationtest = getSegment(idxSegmentOfRoutePoint);
    auto eResultRelocatedPointInSegment = relocatePointInSegment(qpfRoutePoint, segmentForRelocationtest, qpfRelocatedPoint);
    qDebug() << "  qpfRoutePoint = " << qpfRoutePoint;
    qDebug() << "  qpfRelocatedPoint = " << qpfRelocatedPoint;
    qDebug() << "  eResultRelocatedPointInSegment = " << eResultRelocatedPointInSegment;
    if (eResultRelocatedPointInSegment < e_RDORPIS_relocatedToA) {
        qDebug() << __FUNCTION__ << "if (eResultRelocatedPointInSegment < e_RDORPIS_relocatedToA) {";
        return(false);
    }
    //here if:
    //e_RDORPIS_relocatedToA
    //or e_RDORPIS_relocatedToB
    //or e_RDORPIS_relocatedInsideSegment

    //some points outside segment can be projected
    //consider the distance to detect them for rejection
    S_Segment segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint;
    segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._ptA = qpfRoutePoint;
    segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._ptB = qpfRelocatedPoint;
    segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint._bValid = true;

    double distance_relocatedCenterPoint_to_centerPoint = segmentForDistanceCheck_relocatedCenterPoint_to_centerPoint.length();
    //@#LP 0.1 is enough ?
    if (distance_relocatedCenterPoint_to_centerPoint > 0.1) {
        qDebug() << __FUNCTION__ << "distance_relocatedCenterPoint_to_centerPoint is too big: " << distance_relocatedCenterPoint_to_centerPoint;
        return(false);
    }
    //--------------------

    for (int iSeg = 0; iSeg < segCount; iSeg++) {

        S_Segment currentSegment = getSegment(iSeg);

        if (iSeg == idxSegmentOfRoutePoint) {
            currentSegment._ptB = qpfRoutePoint;
            distanceFromStartingRoutePointToRoutePoint += currentSegment.length();
            return(true);

        } else {
            distanceFromStartingRoutePointToRoutePoint += currentSegment.length();
        }
    }
    return(true);
}
