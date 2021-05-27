#include <QDebug>

#include "../application/logic/model/core/route.h"
#include "../application/logic/toolbox/toolbox_math.h"

bool test_check_find_middle();
bool test_computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection();

int main(int argc, char*argv[]) {

    test_check_find_middle();

    test_computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection();
}

bool test_check_find_middle() {

	Route r1;
	bool bFirstPoint = false;
	r1.tryAddPointAtRouteEnd({ .0, .0}, bFirstPoint);
	r1.tryAddPointAtRouteEnd({1.0, .0}, bFirstPoint);
	QPointF qpfMiddlePoint;
	int idxSegmentOfMiddlePoint = -1;
	bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
	bool bReport = r1.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
	    {  .0, .0}, 0,
	    { 1.0, .0}, 0,
	    qpfMiddlePoint, idxSegmentOfMiddlePoint,
	    bDistanceFromMiddlePointToOthersIsUnderLimit);

	qDebug() << "Result = ";
	qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
	qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

	{
	    QPointF qpfMustBeMiddlePoint_forward;
	    int idxSegmentOfMustBeMiddlePoint_forward = -1;
	    bReport = r1.findPointAtADistanceFromAnotherPoint_forwardDirection(
		{  .0, .0}, 0, 0.5,
		qpfMustBeMiddlePoint_forward, idxSegmentOfMustBeMiddlePoint_forward);

	    qDebug() <<" forwardDirection:";
	    qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint_forward;
	    qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint_forward;
	}

    {
        QPointF qpfMustBeMiddlePoint_backward;
        int idxSegmentOfMustBeMiddlePoint_backward = -1;
        bReport = r1.findPointAtADistanceFromAnotherPoint_backwardDirection(
            { 1.0, .0}, 0, 0.5,
            qpfMustBeMiddlePoint_backward, idxSegmentOfMustBeMiddlePoint_backward);

        qDebug() <<" backwardDirection:";
        qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint_backward;
        qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint_backward;
    }



    {
        QPointF qpfMustFail_forward;
        int idxSegmentOfMustFail_forward = -1;
        bReport = r1.findPointAtADistanceFromAnotherPoint_forwardDirection(
            {  .0, .0}, 0, 1.1,
            qpfMustFail_forward, idxSegmentOfMustFail_forward);

        qDebug() <<" forwardDirection:";
        qDebug() << "  qpfMustFail_forward = " << qpfMustFail_forward;
        qDebug() << "  idxSegmentOfMustFail_forward = " << idxSegmentOfMustFail_forward;
    }

    {
        QPointF qpfMustFail_backward;
        int idxSegmentOfMustFail_backward = -1;
        bReport = r1.findPointAtADistanceFromAnotherPoint_backwardDirection(
            { 1.0, .0}, 0, 1.1,
            qpfMustFail_backward, idxSegmentOfMustFail_backward);

        qDebug() <<" backwardDirection:";
        qDebug() << "  qpfMustFail_backward = " << qpfMustFail_backward;
        qDebug() << "  idxSegmentOfMustFail_backward = " << idxSegmentOfMustFail_backward;
    }

    //check find middle
    {
        Route r2;
        bool bFirstPoint = false;
        r2.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r2.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            {  .0,   .0}, 0,
            {10.0, 10.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        QPointF qpfMustBeMiddlePoint;
        int idxSegmentOfMustBeMiddlePoint = -1;
        bReport = r2.findPointAtADistanceFromAnotherPoint_forwardDirection(
            {  .0, .0}, 0, 10.0,
            qpfMustBeMiddlePoint, idxSegmentOfMustBeMiddlePoint);

        qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint;
        qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint;


        {
            QPointF qpfMustFail_forward;
            int idxSegmentOfMustFail_forward = -1;
            bReport = r2.findPointAtADistanceFromAnotherPoint_forwardDirection(
                {  .0, .0}, 0, 20.1,
                qpfMustFail_forward, idxSegmentOfMustFail_forward);

            qDebug() <<" forwardDirection:";
            qDebug() << "  qpfMustFail_forward = " << qpfMustFail_forward;
            qDebug() << "  idxSegmentOfMustFail_forward = " << idxSegmentOfMustFail_forward;
        }

        {
            QPointF qpfMustFail_backward;
            int idxSegmentOfMustFail_backward = -1;
            bReport = r2.findPointAtADistanceFromAnotherPoint_backwardDirection(
                { 10.0, 10.0}, 1, 20.1,
                qpfMustFail_backward, idxSegmentOfMustFail_backward);

            qDebug() <<" backwardDirection:";
            qDebug() << "  qpfMustFail_backward = " << qpfMustFail_backward;
            qDebug() << "  idxSegmentOfMustFail_backward = " << idxSegmentOfMustFail_backward;
        }

    }

    {
        Route r3;
        bool bFirstPoint = false;
        r3.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r3.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { 5.0,  .0}, 0,
            {10.0, 5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

        QPointF qpfMustBeMiddlePoint;
        int idxSegmentOfMustBeMiddlePoint = -1;
        bReport = r3.findPointAtADistanceFromAnotherPoint_backwardDirection(
            {10.0, 10.0}, 1, 10.0,
            qpfMustBeMiddlePoint, idxSegmentOfMustBeMiddlePoint);

        qDebug() << "  qpfMustBeMiddlePoint = " << qpfMustBeMiddlePoint;
        qDebug() << "  idxSegmentOfMustBeMiddlePoint = " << idxSegmentOfMustBeMiddlePoint;
    }

    {
        Route r4;
        bool bFirstPoint = false;
        r4.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r4.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r4.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r4.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { 5.0,   .0}, 0,
            {10.0, 10.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;

    }

    {
        Route r5;
        bool bFirstPoint = false;
        r5.tryAddPointAtRouteEnd({  .0,   .0}, bFirstPoint);
        r5.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r5.tryAddPointAtRouteEnd({10.0, 10.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r5.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            {  .0, 0.0}, 0,
            {10.0, 5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }


    {
        Route r6;
        bool bFirstPoint = false;
        r6.tryAddPointAtRouteEnd({ -5.0, -5.0}, bFirstPoint);
        r6.tryAddPointAtRouteEnd({  0.0,   .0}, bFirstPoint);
        r6.tryAddPointAtRouteEnd({  5.0,  5.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r6.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { -5.0, -5.0}, 0,
            {  5.0,  5.0}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }

    {
        Route r7;
        bool bFirstPoint = false;
        r7.tryAddPointAtRouteEnd({ -5.0, -5.0}, bFirstPoint);
        r7.tryAddPointAtRouteEnd({  0.0,   .0}, bFirstPoint);
        r7.tryAddPointAtRouteEnd({  5.0,  5.0}, bFirstPoint);

        QPointF qpfMiddlePoint;
        int idxSegmentOfMiddlePoint = -1;
        bool bDistanceFromMiddlePointToOthersIsUnderLimit = false;
        bool bReport = r7.findMiddlePointOfRoute_betweenProvidedPoints_forwardDirection(
            { -0.0001, -0.0001}, 0,
            {  0.0001,  0.0001}, 1,
            qpfMiddlePoint, idxSegmentOfMiddlePoint,
            bDistanceFromMiddlePointToOthersIsUnderLimit);

        qDebug() << "Result = ";
        qDebug() << "  qpfMiddlePoint = " << qpfMiddlePoint;
        qDebug() << "  idxSegmentOfMiddlePoint = " << idxSegmentOfMiddlePoint;
    }

    return(true);

}


struct S_Test_qpf_bTestResultToConsiderAsTestSuccess {
    QPointF _qpf;
    int idxSegmentOfRoutePoint;
    bool _bTestResultToConsiderAsTestSuccess;
    qreal _distanceWished;
};

void testDistance(const Route &r,
                  const QVector<S_Test_qpf_bTestResultToConsiderAsTestSuccess>& qvectQpf_S_Test_distanceTests,
                  int& countTestPass, int &countTestFailed);

bool test_computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection() {

    qDebug() << __FUNCTION__;

    int countTestPass = 0;
    int countTestFailed = 0;

    //these three tests must pass:
/*
    //----------------------------------------------------------------------------------------------------
    //test with one segment
    //----------------------------------------------------------------------------------------------------
    {
        Route r1;
        bool bFirstPoint = false;
        r1.tryAddPointAtRouteEnd({0.0, .0}, bFirstPoint);
        r1.tryAddPointAtRouteEnd({1.0, .0}, bFirstPoint);

        QVector<S_Test_qpf_bTestResultToConsiderAsTestSuccess> qvectQpf_S_Test_distanceTests {

            {{0.00, 0.00}, 0, true, 0.00},
            {{0.33, 0.00}, 0, true, 0.33},
            {{0.50, 0.00}, 0, true, 0.50},
            {{1.00, 0.00}, 0, true, 1.00},

            //rejection tests:
            //invalid segment index
            {{0.00, 0.00}, 1, false, -999.999},
            {{0.33, 0.00}, 1, false, -999.999},
            {{0.50, 0.00}, 1, false, -999.999},
            {{1.00, 0.00}, 1, false, -999.999},

            //invalid segment index
            {{0.00, 0.00}, -1, false, -999.999},
            {{0.33, 0.00}, -1, false, -999.999},
            {{0.50, 0.00}, -1, false, -999.999},
            {{1.00, 0.00}, -1, false, -999.999},

            //point not on segment
            {{-20.00,  0.00}, 0, false, -999.999},
            {{ 20.00,  0.00}, 0, false, -999.999},
            {{  0.00, 20.00}, 0, false, -999.999},
            {{  0.00,-20.00}, 0, false, -999.999},
            {{  1.00,  1.00}, 0, false, -999.999},
            {{  0.50,  0.50}, 0, false, -999.999}
        };

        testDistance(r1, qvectQpf_S_Test_distanceTests, countTestPass, countTestFailed);
    }

    //----------------------------------------------------------------------------------------------------
    //test with two segments
    //----------------------------------------------------------------------------------------------------
    {
        Route r2;
        bool bFirstPoint = false;
        r2.tryAddPointAtRouteEnd({-1.0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0,   .0}, bFirstPoint);
        r2.tryAddPointAtRouteEnd({10.0, -1.0}, bFirstPoint);

        QVector<S_Test_qpf_bTestResultToConsiderAsTestSuccess> qvectQpf_S_Test_distanceTests {

            {{-1.00,  0.00}, 0, true, 0.00},
            {{ 0.00,  0.00}, 0, true, 1.00},
            {{ 0.33,  0.00}, 0, true, 1.33},
            {{ 0.50,  0.00}, 0, true, 1.50},
            {{ 5.00,  0.00}, 0, true, 6.00},

            {{ 10.00, 0.00}, 0, true, 11.0},
            {{ 10.00, 0.00}, 1, true, 11.0},

            {{ 10.00, -0.5}, 1, true, 11.5},
            {{ 10.00, -1.0}, 1, true, 12.0},

            //rejection tests:
            //invalid segment index
            {{-1.00,  0.00}, 1, false, -999.999},
            {{ 0.00,  0.00}, 1, false, -999.999},
            {{ 0.33,  0.00}, 1, false, -999.999},
            {{ 0.50,  0.00}, 1, false, -999.999},
            {{ 5.00,  0.00}, 1, false, -999.999},

            {{ 10.00, -0.5}, 0, false, -999.999},
            {{ 10.00, -1.0}, 0, false, -999.999},

            {{ 10.00, 0.00}, 2, false, -999.999},
            {{ 10.00, -0.5}, 2, false, -999.999},
            {{ 10.00, -1.0}, 2, false, -999.999},

            //point not on segment
            {{ -2.00,  0.00}, 0, false, -999.999},
            {{ 12.00,  0.00}, 0, false, -999.999},
            {{  0.00, 20.00}, 0, false, -999.999},
            {{  0.00,-20.00}, 0, false, -999.999},
            {{  1.00,  1.00}, 0, false, -999.999},
            {{  0.50,  0.50}, 0, false, -999.999},
            {{ 10.00, -1.20}, 0, false, -999.999},

            {{ -2.00,  0.00}, 1, false, -999.999},
            {{ 12.00,  0.00}, 1, false, -999.999},
            {{  0.00, 20.00}, 1, false, -999.999},
            {{  0.00,-20.00}, 1, false, -999.999},
            {{  1.00,  1.00}, 1, false, -999.999},
            {{  0.50,  0.50}, 1, false, -999.999},
            {{ 10.00, -1.20}, 1, false, -999.999}

        };

        testDistance(r2, qvectQpf_S_Test_distanceTests, countTestPass, countTestFailed);
    }
*/

    //----------------------------------------------------------------------------------------------------
    //test with three segments
    //----------------------------------------------------------------------------------------------------
    {
        Route r3;
        bool bFirstPoint = false;
        r3.tryAddPointAtRouteEnd({-1.0,  1.0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({ 1.0, -1.0}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({ 3.0, -1.0}, bFirstPoint);
        //r3.tryAddPointAtRouteEnd({ 3.5, -0.5}, bFirstPoint);
        r3.tryAddPointAtRouteEnd({ 5.0, 1.0}, bFirstPoint);

        QVector<S_Test_qpf_bTestResultToConsiderAsTestSuccess> qvectQpf_S_Test_distanceTests {

            {{-1.00,  1.00}, 0, true, 0.00},
            {{ 0.00,  0.00}, 0, true, sqrt(2.0)},

            {{ 1.00, -1.00}, 0, true, 2.0*sqrt(2.0)},
            {{ 1.00, -1.00}, 1, true, 2.0*sqrt(2.0)},

            {{ 2.00, -1.00}, 1, true, 2.0*sqrt(2.0) + 1.00},

            {{ 3.00, -1.00}, 1, true, 2.0*sqrt(2.0) + 2.00},
            {{ 3.00, -1.00}, 2, true, 2.0*sqrt(2.0) + 2.00},

            {{ 3.50, -0.50}, 2, true, 2.0*sqrt(2.0) + 2.00 + sqrt((0.5*0.5)+(0.5*0.5))},

            {{ 5.00,  1.00}, 2, true, 2.0*sqrt(2.0) + 2.00 + 2*sqrt(2.0)},


            //rejection tests:
            //invalid segment index
            {{-1.00,  1.00}, 1, false, -999.999},
            {{ 0.00,  0.00}, 1, false, -999.999},

            {{ 1.00, -1.00}, 2, false, -999.999},
            {{ 1.00, -1.00}, 2, false, -999.999},

            {{ 2.00, -1.00}, 0, false, -999.999},
            {{ 2.00, -1.00}, 2, false, -999.999},

            {{ 3.00, -1.00}, 0, false, -999.999},
            {{ 3.50, -0.50}, 1, false, -999.999},

            //point not on segment
            {{ -2.00, 2.00}, 0, false, -999.999},
            {{  2.00, 2.00}, 0, false, -999.999},

            {{  0.00,-1.00}, 0, false, -999.999},
            {{  0.00,-1.00}, 1, false, -999.999},

            {{  4.00,-1.00}, 0, false, -999.999},
            {{  4.00,-1.00}, 1, false, -999.999},
            {{  4.00,-1.00}, 3, false, -999.999},

            {{  2.00,-2.00}, 0, false, -999.999},
            {{  2.00,-2.00}, 1, false, -999.999},
            {{  2.00,-2.00}, 2, false, -999.999},

            {{  5.00, 0.00}, 0, false, -999.999},
            {{  5.00, 0.00}, 1, false, -999.999},
            {{  5.00, 0.00}, 2, false, -999.999},

            {{  5.00, 1.50}, 0, false, -999.999},
            {{  5.00, 1.50}, 1, false, -999.999},
            {{  5.00, 1.50}, 2, false, -999.999},

        };

        testDistance(r3, qvectQpf_S_Test_distanceTests, countTestPass, countTestFailed);
    }


    qDebug() << __FUNCTION__ ;
    qDebug() << "countTestPas   : " << countTestPass;
    qDebug() << "countTestFailed: " << countTestFailed;

    return(true);
}


void testDistance(const Route &r,
                  const QVector<S_Test_qpf_bTestResultToConsiderAsTestSuccess>& qvectQpf_S_Test_distanceTests,
                  int& countTestPass, int &countTestFailed) {

   for (auto sTestDist_i: qvectQpf_S_Test_distanceTests) {

       qDebug() << "test with point " << sTestDist_i._qpf << "with idxSegmentOfRoutePoint as: " << sTestDist_i.idxSegmentOfRoutePoint;

       qreal distanceFromStartingRoutePoint_to_point = 9999999.99; //9999999.99 as invalid
       bool bGot = r.computeDistance_alongRoute_fromStartingRoutePointToARoutePoint_forwardDirection(sTestDist_i._qpf,
                                                                                                      sTestDist_i.idxSegmentOfRoutePoint,
                                                                                                      distanceFromStartingRoutePoint_to_point);
       if (bGot == sTestDist_i._bTestResultToConsiderAsTestSuccess) {
           if (sTestDist_i._bTestResultToConsiderAsTestSuccess) {
               qDebug() << "  test OK :distance to point is :" << distanceFromStartingRoutePoint_to_point;
               if (doubleValuesAreClose(distanceFromStartingRoutePoint_to_point, sTestDist_i._distanceWished, 0.05)) {
                   countTestPass++;
                   qDebug() << "  test OK : _distanceWished is :" << sTestDist_i._distanceWished;
               } else {
                   countTestFailed++;
                   qDebug() << "!!! test FAILED, distance is not distanceWished, it should be:" << sTestDist_i._distanceWished;
               }
           } else {
               countTestPass++;
               qDebug() << "  test OK :distance can not be computed";
           }
       } else {
           countTestFailed++;
           qDebug() << "!!! test FAILED, report is: " << bGot << " shoudl be: " << sTestDist_i._bTestResultToConsiderAsTestSuccess;
       }
       qDebug() << " ";
   }
}
