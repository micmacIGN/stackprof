#include <QCoreApplication>

#include "../../application/logic/model/core/route.h"
#include "../../application/logic/model/core/BoxOrienterAndDistributer.h"
#include "../../application/logic/model/core/ComputationCore_inheritQATableModel.h"

#include "../../application/logic/model/core/IDGenerator.h"

int test_insertBoxesSetInSameSegment_1();
int test_insertBoxesSetInMultipleSegments_1();
int test_moveOneBox_1();
int test_insertBoxes_1();
int test_insertBoxes_2();
int test_insertBoxes_beforeAfter_inSegmentsAroundASegmentWithOneBox();

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Dbg: %s\n", localMsg.constData());
        //fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "Inf: %s\n", localMsg.constData());
        //fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stdout, "warning: %s\n", localMsg.constData());
        //fprintf(stdout, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "critical: %s\n", localMsg.constData());
        //fprintf(stdout, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stdout, "fatal: %s\n", localMsg.constData());
        //fprintf(stdout, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[]) {

    //qInstallMessageHandler(myMessageOutput); // Install the handler


    //different uses cases:

    //test_insertBoxesSetInSameSegment_1();
    //test_insertBoxesSetInMultipleSegments_1();
    //test_moveOneBox_1();
    //test_insertBoxes_1();
    //test_insertBoxes_2();

    test_insertBoxes_beforeAfter_inSegmentsAroundASegmentWithOneBox();

    return(0);
}


int test_insertBoxesSetInSameSegment_1() {

    //=> insert 2 boxes created with centerPoints: (3, 0), (10.5, 0)
    //=> insert 4 boxes created with centerPoints: (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    Route r;

    //2 points, 1 segment
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14.0,.0}, bFirstPoint);

    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;
    BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = BoxOrienterAndDistributer::e_ERACBCAR_canNotFit;

    //add boxes with width = 7
    boad.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(&r, 7, 22, e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, false, true, e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);
    //=> 2 boxes created with centerPoints : (3, 0), (10.5, 0)
    //=> qmap  content (about center point): (3, 0), (10.5, 0)

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, { true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(false);
    }
    cc.setRoutePtr(&r);

    bool bCriticalErrorOccured = false;

    int vectIdxOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox  = -1;
    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
    int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
    bool bAddReport = true;
    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point):  (3, 0), (10.5, 0)


    //add boxes with width = 3
    boad.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(&r, 3, 11, e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, false, true, e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);
    //=> 4 boxes created with centerPoints:                    (1, 0), (4.5, 0), (8, 0), (11.5, 0)
    //=> qmap content (about center point): (3, 0), (10.5, 0), (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point): (1, 0), (3, 0), (4.5, 0), (8, 0), (10.5, 0), (11.5, 0)

    //QCoreApplication a(argc, argv);
    //return a.exec();
    return(bAddReport?0:1);
}



int test_insertBoxesSetInMultipleSegments_1() {

    //=> insert 2 boxes created with centerPoints: (3, 0), (10.5, 0)
    //=> insert 4 boxes created with centerPoints: (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    Route r;

    //4 points, 3 segments
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 2.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 9.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14.0,.0}, bFirstPoint);

    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;
    BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = BoxOrienterAndDistributer::e_ERACBCAR_canNotFit;

    //add boxes with width = 7
    boad.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(&r, 7, 22, e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, false, true, e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);
    //=> 2 boxes created with centerPoints : (3, 0), (10.5, 0)
    //=> qmap  content (about center point): (3, 0), (10.5, 0)

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, { true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(false);
    }
    cc.setRoutePtr(&r);

    bool bCriticalErrorOccured = false;

    int vectIdxOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox  = -1;
    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
    int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
    bool bAddReport = true;
    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point):  (3, 0), (10.5, 0)


    //add boxes with width = 3
    boad.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(&r, 3, 11, e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, false, true, e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);
    //=> 4 boxes created with centerPoints:                    (1, 0), (4.5, 0), (8, 0), (11.5, 0)
    //=> qmap content (about center point): (3, 0), (10.5, 0), (1, 0), (4.5, 0), (8, 0), (11.5, 0)
    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point): (1, 0), (3, 0), (4.5, 0), (8, 0), (10.5, 0), (11.5, 0)

    //QCoreApplication a(argc, argv);
    //return a.exec();
    return(bAddReport?0:1);
}



int test_moveOneBox_1() {

    Route r;

    //4 points, 3 segments
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 2.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 9.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14.0,.0}, bFirstPoint);/*

    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 20.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 90.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({140.0,.0}, bFirstPoint);


    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 200.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 900.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({1400.0,.0}, bFirstPoint);


    bAdded &= r.tryAddPointAtRouteEnd({  .0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 2000.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 9000.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14000.0,.0}, bFirstPoint);*/


    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;
    BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = BoxOrienterAndDistributer::e_ERACBCAR_canNotFit;

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, {true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(false);
    }
    cc.setRoutePtr(&r);

    //add boxes with width = 3
    boad.computeBoxesAlongRoute_usingAutomaticDistribution_closeAsPossible_fromFirstPoint(&r, 3, 11, e_DBSBuAD_square2_compatibleOnlyWithNoneInterpolationToGetPixel, false, true, e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered);
    //=> 4 boxes created with centerPoints: (1, 0), (4.5, 0), (8, 0), (11.5, 0)
    //qmap content (about center point):    (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    bool bCriticalErrorOccured = false;

    int vectIdxOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox  = -1;
    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
    int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
    bool bAddReport = true;
    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point): (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    if (!bAddReport) {
        return(1);
    }

    qDebug() << "-1-------------------------------------------------------------------";
    qDebug() << "-1-------------------------------------------------------------------";
    //move the boxId 0
    int vectBoxId = 0;
    S_ProfilsBoxParameters sProfilsBoxParameters;
    bool bGot = cc.get_oneStackedProfilBoxParameters(vectBoxId, sProfilsBoxParameters);
    if (!bGot) {
        return(1);
    }
    qDebug() << "sProfilsBoxParameters got:";
    sProfilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();

    //simulate a move:
    sProfilsBoxParameters._qpfCenterPoint = {6.0, 0};
    sProfilsBoxParameters._idxSegmentOwnerOfCenterPoint = 1;
    //remove the link to automatic distribution:
    sProfilsBoxParameters._sUniqueBoxID._setID = IDGenerator::generateABoxesSetID();
    sProfilsBoxParameters._sUniqueBoxID._idxBoxInSet = 0;

    qDebug() << "moving to :" << sProfilsBoxParameters._qpfCenterPoint;
    qDebug() << "sProfilsBoxParameters with centerpoint and segment owner change:";
    sProfilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();

    int vectBoxId_afterReorderAlongRoute = -1;
    bool bReport =  cc.updateProfilsBoxParameters_withBoxMove(vectBoxId, sProfilsBoxParameters, vectBoxId_afterReorderAlongRoute);
    qDebug() << "vectBoxId_afterReorderAlongRoute = " << vectBoxId_afterReorderAlongRoute;

    //qmap  content (about center point): (6  , 0), (4.5, 0), (8, 0), (11.5, 0)
    //qvect content (about center point): (4.5, 0), (6   ,0), (8, 0), (11.5, 0)


    qDebug() << "-2-------------------------------------------------------------------";
    qDebug() << "-2-------------------------------------------------------------------";
    vectBoxId = vectBoxId_afterReorderAlongRoute;
    //simulate a second move; locate the box after all the others
    bGot = cc.get_oneStackedProfilBoxParameters(vectBoxId, sProfilsBoxParameters);
    if (!bGot) {
        return(1);
    }
    sProfilsBoxParameters._qpfCenterPoint = {12.0, 0};
    sProfilsBoxParameters._idxSegmentOwnerOfCenterPoint = 2;
    //remove the link to automatic distribution:
    sProfilsBoxParameters._sUniqueBoxID._setID = IDGenerator::generateABoxesSetID();
    sProfilsBoxParameters._sUniqueBoxID._idxBoxInSet = 0;

    qDebug() << "moving to :" << sProfilsBoxParameters._qpfCenterPoint;
    qDebug() << "sProfilsBoxParameters with centerpoint and segment owner change:";
    sProfilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();

    vectBoxId_afterReorderAlongRoute = -1;
    bReport =  cc.updateProfilsBoxParameters_withBoxMove(vectBoxId, sProfilsBoxParameters, vectBoxId_afterReorderAlongRoute);
    qDebug() << "vectBoxId_afterReorderAlongRoute = " << vectBoxId_afterReorderAlongRoute;

    //qmap  content (about center point): (12 , 0), (4.5, 0), (8   , 0), (11.5, 0)
    //qvect content (about center point): (4.5, 0), (8,   0), (11.5, 0), (12,   0)


    qDebug() << "-3-------------------------------------------------------------------";
    qDebug() << "-3-------------------------------------------------------------------";
    vectBoxId = vectBoxId_afterReorderAlongRoute;
    //simulate a third move; locate the box before all the others
    bGot = cc.get_oneStackedProfilBoxParameters(vectBoxId, sProfilsBoxParameters);
    if (!bGot) {
        return(1);
    }
    sProfilsBoxParameters._qpfCenterPoint = {0, 0};
    sProfilsBoxParameters._idxSegmentOwnerOfCenterPoint = 0;
    //remove the link to automatic distribution:
    sProfilsBoxParameters._sUniqueBoxID._setID = IDGenerator::generateABoxesSetID();
    sProfilsBoxParameters._sUniqueBoxID._idxBoxInSet = 0;

    qDebug() << "moving to :" << sProfilsBoxParameters._qpfCenterPoint;
    qDebug() << "sProfilsBoxParameters with centerpoint and segment owner change:";
    sProfilsBoxParameters.showContent_centerPointAndIdxSegmentOnly();

    vectBoxId_afterReorderAlongRoute = -1;
    bReport =  cc.updateProfilsBoxParameters_withBoxMove(vectBoxId, sProfilsBoxParameters, vectBoxId_afterReorderAlongRoute);
    qDebug() << "vectBoxId_afterReorderAlongRoute = " << vectBoxId_afterReorderAlongRoute;

    return(bReport?0:1);
}


//insert box one by one using centerpoints from a vector
int test_insertBoxes_1() {

    Route r;

    //4 points, 3 segments
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({  .05,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 2.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 9.05,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14.0,.0}, bFirstPoint);

    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, {true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(1);
    }
    cc.setRoutePtr(&r);


    struct S_QpointFWithIdxSegmentOwner_ForUnitTest {
        QPointF _qpf;
        int _idxSegmentOwner;
    };

    QVector<S_QpointFWithIdxSegmentOwner_ForUnitTest> qvect_sQpfIdxSegmentOwner {
        { /*{ 0.0, .0}, 0}, //(outside segment point) */
          { 0.3, .0}, 0},
        { { 1.0, .0}, 0},

        { { 1.05, .0}, 0},
        { { 2.0, .0}, 0},
        { { 2.0, .0}, 1},
        { { 4.0, .0}, 1},
        { { 9.5, .0}, 2},
        { {13.0, .0}, 2},

        { {13.5, .0}, 2},
        { {14.0, .0}, 2},
    };

    int vectBoxId = -1;
    for (auto sqpfIdxSegmentOwner: qvect_sQpfIdxSegmentOwner) {

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "--- LOOP ---------------------------------------------------");

        S_ProfilsBoxParameters sProfilsBoxParameters;
        bool bReport = boad.makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(&r,
                                                                                        sqpfIdxSegmentOwner._qpf,
                                                                                        sqpfIdxSegmentOwner._idxSegmentOwner,
                                                                                        3, 11,
                                                                                        sProfilsBoxParameters);
        if (!bReport) {
            qDebug() << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute report said false";
            qDebug() << "  tryed with sProfilsBoxParameters._qpfCenterPoint = " << sProfilsBoxParameters._qpfCenterPoint;
            qDebug() << "  tryed with sProfilsBoxParameters._oddPixelWidth  = " << sProfilsBoxParameters._oddPixelWidth;
            continue;//return(1);
        }

        qDebug() << __FUNCTION__ << "sProfilsBoxParameters made:";
        sProfilsBoxParameters.showContent();

        bool bCriticalErrorOccured = false;
        int vectIdxOfFirstInsertedBox = -1;
        int keyMapOfFirstInsertedBox  = -1;
        int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
        int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
        bool bAddReport = true;
        bCriticalErrorOccured = false;
        bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                    {sProfilsBoxParameters}, {true, true, true},
                    true,
                    vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                    bCriticalErrorOccured);
        if (!bAddReport) {
            qDebug() << "add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters report said false";
            return(1);
        }

        qDebug() << "sProfilsBoxParameters added with synced qvect";

        vectBoxId = aboutQVectOrderAlongRoute_indexOfFirstChange;
        sProfilsBoxParameters.showContent();
        qDebug() << "its vectBoxId is " << vectBoxId;

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "+++ current after add content:");


    }
    qDebug() << "";
    cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "*** final content :");

    return(0);
}


//same as test_insertBoxes_1 but from the last to the first centerpoint from the vector
int test_insertBoxes_2() {

    Route r;

    //4 points, 3 segments
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({  .05,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 2.0,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({ 9.05,.0}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({14.0,.0}, bFirstPoint);

    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, {true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(1);
    }
    cc.setRoutePtr(&r);


    struct S_QpointFWithIdxSegmentOwner_ForUnitTest {
        QPointF _qpf;
        int _idxSegmentOwner;
    };

    QVector<S_QpointFWithIdxSegmentOwner_ForUnitTest> qvect_sQpfIdxSegmentOwner {
        { /*{ 0.0, .0}, 0}, //(outside segment point) */
          { 0.3, .0}, 0},
        { { 1.0, .0}, 0},

        { { 1.05, .0}, 0},
        { { 2.0, .0}, 0},
        { { 2.0, .0}, 1},
        { { 4.0, .0}, 1},
        { { 9.5, .0}, 2},
        { {13.0, .0}, 2},

        { {13.5, .0}, 2},
        { {14.0, .0}, 2},
    };

    int vectBoxId = -1;
    for (auto revIter = qvect_sQpfIdxSegmentOwner.rbegin();
         revIter != qvect_sQpfIdxSegmentOwner.rend();
         ++revIter) {

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "--- LOOP ---------------------------------------------------");

        S_ProfilsBoxParameters sProfilsBoxParameters;
        bool bReport = boad.makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(&r,
                                                                                        revIter->_qpf,
                                                                                        revIter->_idxSegmentOwner,
                                                                                        3, 11,
                                                                                        sProfilsBoxParameters);
        if (!bReport) {
            qDebug() << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute report said false";
            qDebug() << "  tryed with sProfilsBoxParameters._qpfCenterPoint = " << sProfilsBoxParameters._qpfCenterPoint;
            qDebug() << "  tryed with sProfilsBoxParameters._oddPixelWidth  = " << sProfilsBoxParameters._oddPixelWidth;
            continue;//return(1);
        }

        qDebug() << __FUNCTION__ << "sProfilsBoxParameters made:";
        sProfilsBoxParameters.showContent();

        bool bCriticalErrorOccured = false;

        int vectIdxOfFirstInsertedBox = -1;
        int keyMapOfFirstInsertedBox  = -1;
        int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
        int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
        bool bAddReport = true;
        bCriticalErrorOccured = false;
        bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                    {sProfilsBoxParameters}, {true, true, true},
                    true,
                    vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                    bCriticalErrorOccured);
        if (!bAddReport) {
            qDebug() << "add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters report said false";
            return(1);
        }

        qDebug() << "sProfilsBoxParameters added with synced qvect";

        vectBoxId = aboutQVectOrderAlongRoute_indexOfFirstChange;
        sProfilsBoxParameters.showContent();
        qDebug() << "its vectBoxId is " << vectBoxId;

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "+++ current after add content:");


    }
    qDebug() << "";
    cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "*** final content :");

    return(0);
}

int test_insertBoxes_beforeAfter_inSegmentsAroundASegmentWithOneBox() {

    Route r;

    //4 points, 3 segments
    bool bAdded = true;
    bool bFirstPoint = false;
    bAdded &= r.tryAddPointAtRouteEnd({11615.2531875, 23974.5955781}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({11718.8867812, 24165.2813906}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({11769.9720469, 24360.7221562}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({11772.77625, 24590.179125}, bFirstPoint);
    bAdded &= r.tryAddPointAtRouteEnd({11799.7209844, 24712.4667656}, bFirstPoint);

    if (!bAdded) {
        qDebug() << "error adding point to route";
        return(1);
    }

    BoxOrienterAndDistributer boad;

    ComputationCore_inheritQATableModel cc;
    if (!cc.__forUnitTestOnly__set_layers_components_end_mainComputationMode({true, true, true}, {true, true, true}, e_mCM_Typical_PX1PX2Together_DeltazAlone)) {
        return(1);
    }
    cc.setRoutePtr(&r);

    struct S_QpointFWithIdxSegmentOwner_ForUnitTest {
        QPointF _qpf;
        int _idxSegmentOwner;
    };

    QVector<S_QpointFWithIdxSegmentOwner_ForUnitTest> qvect_sQpfIdxSegmentOwner {
        { {  11761.412, 24327.973}, 1},
    };

    //
    //{  11674.99 , 24084.512}, 0},
    //{  11770.657, 24416.784}, 2},

    int vectBoxId = -1;
    for (auto revIter = qvect_sQpfIdxSegmentOwner.rbegin();
         revIter != qvect_sQpfIdxSegmentOwner.rend();
         ++revIter) {

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "--- LOOP ---------------------------------------------------");

        S_ProfilsBoxParameters sProfilsBoxParameters;
        bool bReport = boad.makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute(&r,
                                                                                        revIter->_qpf,
                                                                                        revIter->_idxSegmentOwner,
                                                                                        721, 131,
                                                                                        sProfilsBoxParameters);
        if (!bReport) {
            qDebug() << "makeABoxFitInRouteChecked_fromPointOfIdxSegmentOwnerofRoute report said false";
            qDebug() << "  tryed with sProfilsBoxParameters._qpfCenterPoint = " << sProfilsBoxParameters._qpfCenterPoint;
            qDebug() << "  tryed with sProfilsBoxParameters._oddPixelWidth  = " << sProfilsBoxParameters._oddPixelWidth;
            continue;//return(1);
        }

        qDebug() << __FUNCTION__ << "sProfilsBoxParameters made:";
        sProfilsBoxParameters.showContent();

        bool bCriticalErrorOccured = false;

        int vectIdxOfFirstInsertedBox = -1;
        int keyMapOfFirstInsertedBox  = -1;
        int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
        int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
        bool bAddReport = true;
        bCriticalErrorOccured = false;
        bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                    {sProfilsBoxParameters}, {true, true, true},
                    true,
                    vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                    bCriticalErrorOccured);
        if (!bAddReport) {
            qDebug() << "add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters report said false";
            return(1);
        }

        qDebug() << "sProfilsBoxParameters added with synced qvect";

        vectBoxId = aboutQVectOrderAlongRoute_indexOfFirstChange;
        sProfilsBoxParameters.showContent();
        qDebug() << "its vectBoxId is " << vectBoxId;

        cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "+++ current after add content:");
    }

    cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "*** after initial box add :");


    BoxOrienterAndDistributer::e_EvaluationResultAboutCenterBoxCandidateAlongRoute e_ERACBCAR_firstBoxStatusWhenNotAdjustementRequiered = BoxOrienterAndDistributer::e_ERACBCAR_canNotFit;

    boad.computeBoxesAlongRoute_usingAutomaticDistribution_distanceBetweenCenter_fromFirstPoint(&r, 251, 251, 350);
    //=> 2 boxes created with centerPoints:
    //qmap content (about center point):

    qDebug() << "-1------------------";
    qDebug() << "boad.getConstRef_qvectProfBoxParameters_automatically_distributed(): ";
    qDebug() << "-2------------------";
    qDebug() << "size:" << boad.getConstRef_qvectProfBoxParameters_automatically_distributed().size();
    for (auto& pB: boad.getConstRef_qvectProfBoxParameters_automatically_distributed()) {
        qDebug() << "-3------------------";
        pB.showContent_centerPointAndIdxSegmentOnly();
    }
    qDebug() << "-4------------------";

    bool bCriticalErrorOccured = false;

    int vectIdxOfFirstInsertedBox = -1;
    int keyMapOfFirstInsertedBox  = -1;
    int aboutQVectOrderAlongRoute_indexOfFirstChange = 0;
    int aboutQVectOrderAlongRoute_indexOfLastChange  = 0;
    bool bAddReport = true;
    bCriticalErrorOccured = false;
    bAddReport &= cc.add_boxAndStackedProfilWithMeasurements_from_qvectProfilsBoxParameters(
                boad.getConstRef_qvectProfBoxParameters_automatically_distributed(), {true, true, true},
                true,
                vectIdxOfFirstInsertedBox, keyMapOfFirstInsertedBox,
                bCriticalErrorOccured);
    //qvect content (about center point): (1, 0), (4.5, 0), (8, 0), (11.5, 0)

    if (!bAddReport) {
        return(1);
    }

    cc.showContent_qmap_qvectSyncWithQMap(QString(__FUNCTION__) + "*** after added boxes around:");

    return(0);
}
