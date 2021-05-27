#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsItem>
#include <QDebug>

#include <QKeyEvent>

#include <QList>
#include <QGraphicsView>

#include <QGraphicsRectItem>

#include "CustomGraphicsScene_profil.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h"

#include <QFontMetrics>

#include <QTransform>

#include <QWidget>

#include "landmarks.h"

#include <QRandomGenerator> //for dev test only

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.h"
#include "customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.h"

#include "ConverterPixelToQsc.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.h"

#include <algorithm> //for min_element() and max_element()

#include "../../mathComputation/linearRegression.h" //to compute locations of y for x0 left side and x0 right side

#include "../core/leftRight_sideAccess.hpp"

#include "../core/ComputationCore_structures.h"

#include "../../toolbox/toolbox_math.h"

#include "customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.h"

#include <qmath.h>

#include "customGraphicsItems/graphicProfil/CustomLineNoEvent.h"

CustomGraphicsScene_profil::CustomGraphicsScene_profil(QObject *parent) : QGraphicsScene(parent),
    _eCA_component(e_CA_Invalid),
    _cccs_xMin(0), _cccs_xMax(1),
    _cccs_yMin(0), _cccs_yMax(1),
    _x0(999999), //999999 as invalid value

    _fontSizeForTextLandmark(8),

    _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis(1),
    _wPixelSpaceBetweenTextLandmarkAndYAxis(1),
    _wPixelSpaceOutBorder(1),
    _hPixelSpaceOutBorder(1),

    _wPixelYAxisBigLandmark(7),
    _wPixelYAxisSmallLandmark(3),
    _hPixelXAxisBigLandmark(7),
    _hPixelXAxisSmallLandmark(3),

    _bDrawYLinesForBigLandmarkThroughArea(false),
    _bDrawXLinesForBigLandmarkThroughArea(false),

    _widgetParentPixelWidth(0),
    _widgetParentPixelHeight(0),
    _bWidgetParentPixelWHValid(false),

    _sceneRectF {.0,.0,1.0,1.0},//invalid value but not zero
    _sceneRectF_areaOfLayerWhenAlone_qcsDim {.0,.0,1.0,1.0},//invalid value but not zero
    _wSpaceAtLeft_whenAlone_qcsDim(0.1), //invalid value but not zero
    _wSpaceAtRight_whenAlone_qcsDim(0.1),//invalid value but not zero

    _sceneRectF_areaOfLayerWhenAlone_pixelDim {.0,.0,1.0,1.0},//invalid value but not zero
    _wSpaceAtLeft_whenAlone_pixelDim(0.1), //invalid value but not zero
    _wSpaceAtRight_whenAlone_pixelDim(0.1),//invalid value but not zero

    _eAlignementSceneMode { e_ASM_none_graphicAlone}
{

    _CGItemDblSidesXAdjWithCompLines_mainMeasure = nullptr;
    _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure = nullptr;

    _customGraphicsItemCurve = nullptr;
    _customGraphicsItemEnvelop = nullptr;

    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left ].clear();
    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right].clear();

    _xRangeForLinearRegressionComputation[e_LRsA_left  ] = {.0,.0};
    _xRangeForLinearRegressionComputation[e_LRsA_right ] = {.0,.0};

    //_qstrFontToUse = "Helvetica [Cronyx]";
    _qstrFontToUse = "Courier 10 Pitch";
    //_qstrFontToUse = "DejaVu Sans";
    //_qstrFontToUse = "DejaVu Sans Mono";

    _fnt = QFont(_qstrFontToUse, _fontSizeForTextLandmark);

    //use lines for landmarks
    _qbrush_for_landmarks.setStyle(Qt::NoBrush);

    _qpen_for_landmarks.setColor(QColor(0,0,0));

    _qpen_for_landmarks.setWidthF(1.0);

    _qpen_for_landmarks.setStyle(Qt::SolidLine);

    _qbrush_for_landmarks_throughArea.setStyle(/*Qt::SolidPattern*/Qt::NoBrush);

    _qpen_for_landmarks_throughArea.setColor(QColor(160,160,160));

//#define DEF_test_cosmeticFALSE_WIDTH2
#define DEF_test_cosmeticTRUE
#ifdef DEF_test_cosmeticTRUE
    _qpen_for_landmarks_throughArea.setCosmetic(true);
    _qpen_for_landmarks_throughArea.setWidthF(1.0);
#endif
#ifdef DEF_test_cosmeticFALSE_WIDTH2
    _qpen_for_landmarks_throughArea.setCosmetic(false);
    _qpen_for_landmarks_throughArea.setWidthF(2.0);
#endif
    _qpen_for_landmarks_throughArea.setStyle(Qt::DotLine);

    _qpen_for_landmarks_throughArea.setCapStyle(Qt::FlatCap);

    _stackedProfilWithMeasurementsPtr = nullptr;

    _eAlignementSceneMode = e_ASM_none_graphicAlone;

    _cccs_yMin_whenAlone = 0;
    _cccs_yMax_whenAlone = 1;

    /*
    Qt::SolidLine
    Qt::DashLine
    Qt::DotLine
    Qt::DashDotLine
    Qt::DashDotDotLine
    Qt::CustomDashLine
    */

    _eProfilAdjustMode = e_PAM_leftRight; //default
}

void CustomGraphicsScene_profil::setOffAnyAlignementMode() {

    _eAlignementSceneMode = e_ASM_none_graphicAlone;

    setXYRange(_cccs_xMin, _cccs_xMax,
               _cccs_yMin_whenAlone, _cccs_yMax_whenAlone);

    _sceneRectF = _sceneRectF_areaOfLayerWhenAlone_pixelDim;

    removeAdded_landmarksItem_texts();
    computeLandmarks_and_createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem();   
}

void CustomGraphicsScene_profil::updateItems() {

    qDebug() << __FUNCTION__ << "_eCA_component, = " << _eCA_component;
    if (_eCA_component >= e_CA_Invalid) {
        qDebug() << __FUNCTION__ << "_eLA_layer >= eLA_Invalid";
        return;
    }

    qDebug() << __FUNCTION__ << "_eAlignementSceneMode = " << _eAlignementSceneMode;

    //the sceneRect in use was computed from outside to match with other graphics (graphics alignement)
    if (_eAlignementSceneMode == e_ASM_alignSameWidth) {

        qDebug() << __FUNCTION__ << "e_ASM_alignSameWidth";

        //updating
        removeAdded_areaRectAndLinesItemsOnly_landMarksItemsFromScene();

        QRectF _sceneRectF_areaOfLayer_whenXAlignement_qcsDim;
        QRectF _sceneRectF_areaOfLayer_whenXAlignement_pixelDim;
        double _wSpaceAtLeft_whenXAlignement_pixelDim;
        double _wSpaceAtRight_whenXAlignement_pixelDim;

        computeSceneAreaSize_usingValuesFromOutsideForXAlignement(
                    _throughAllScene_left_pixelDim,
                    _throughAllScene_right_pixelDim,
                    _throughAllScene_wSceneSpaceAtLeft_pixelDim,
                    _throughAllScene_wSceneSpaceAtRight_pixelDim,

                    _sceneRectF_areaOfLayer_whenXAlignement_qcsDim,
                    _sceneRectF_areaOfLayer_whenXAlignement_pixelDim,
                    _forResizeComputation_sameWidth,
                    _wSpaceAtLeft_whenXAlignement_pixelDim,  //@#LP remove intput to output param args
                    _wSpaceAtRight_whenXAlignement_pixelDim);//@#LP remove intput to output param args

        _sceneRectF = _sceneRectF_areaOfLayer_whenXAlignement_pixelDim;

        //updating

        add_areaRectAndLinesItemsOnly_landMarksItemsToScene();
        setPosAndVisibility_textsItemsOnly_landMarksItemsOnScene();

        if (_CGItemDblSidesXAdjWithCompLines_mainMeasure) {

            qDebug() << __FUNCTION__  << " if (_CGItemDblSidesXAdjWithCompLines_mainMeasure != nullptr) {";

            _CGItemDblSidesXAdjWithCompLines_mainMeasure->adaptToPixelScene(_converterPixToqsc);
            //--------------------------------------------

            _CGItemDblSidesXAdjWithCompLines_mainMeasure->setAndUpdateParameters_pointCoupleForLeftRightSide(
                        _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left ],
                        _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right]);

            qDebug() << __FUNCTION__  << " #665";

            _CGItemDblSidesXAdjWithCompLines_mainMeasure->setAndUpdateParameters_xAdjustersPositionsForLeftRightSide(_xRangeForLinearRegressionComputation);

        }

        if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure) {

            qDebug() << __FUNCTION__  << " if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure != nullptr) {";

            _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->adaptToPixelScene(_converterPixToqsc);


            qDebug() << __FUNCTION__  << " #665";
        }

        setSceneRect(_sceneRectF);
        qDebug() << __FUNCTION__  << "setSceneRect with: " << _sceneRectF;

        qreal cccs_xMin_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMin);
        qreal cccs_yMin_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin));

        qreal cccs_xMax_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMax);
        qreal cccs_yMax_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax));

        QRectF qrectFCurveDisplayArea {
            QPointF(cccs_xMin_ToPixelSceneRect,
                    cccs_yMin_ToPixelSceneRect),

            QPointF(cccs_xMax_ToPixelSceneRect,
                    cccs_yMax_ToPixelSceneRect),
        };

        if (_customGraphicsItemCurve) {
            _customGraphicsItemCurve->adaptToPixelScene(_converterPixToqsc/*, qrectFCurveDisplayArea*/);
        }
        if (_customGraphicsItemEnvelop) {
            _customGraphicsItemEnvelop->adaptToPixelScene(_converterPixToqsc/*, qrectFCurveDisplayArea*/);
        }

        return;
    }

    //updating
    removeAdded_areaRectAndLinesItemsOnly_landMarksItemsFromScene();

    computeSceneAreaSize(
            _sceneRectF_areaOfLayerWhenAlone_qcsDim,
            _sceneRectF_areaOfLayerWhenAlone_pixelDim,
            _forResizeComputation_sameWidth,
            _wSpaceAtLeft_whenAlone_qcsDim, _wSpaceAtRight_whenAlone_qcsDim,
            _wSpaceAtLeft_whenAlone_pixelDim, _wSpaceAtRight_whenAlone_pixelDim);

    //qDebug() << __FUNCTION__ << "_sceneRectF_areaOfLayerWhenAlone : " << _sceneRectF_areaOfLayerWhenAlone;
    //qDebug() << __FUNCTION__ << "_wSpaceAtLeft_whenAlone  : " << _wSpaceAtLeft_whenAlone;
    //qDebug() << __FUNCTION__ << "_wSpaceAtRight_whenAlone : " << _wSpaceAtRight_whenAlone;

    _sceneRectF = _sceneRectF_areaOfLayerWhenAlone_pixelDim;

    //updating

    add_areaRectAndLinesItemsOnly_landMarksItemsToScene();
    setPosAndVisibility_textsItemsOnly_landMarksItemsOnScene();

    if (_CGItemDblSidesXAdjWithCompLines_mainMeasure) {

        qDebug() << __FUNCTION__  << " if (_CGItemDblSidesXAdjWithCompLines_mainMeasure != nullptr) {";

        _CGItemDblSidesXAdjWithCompLines_mainMeasure->adaptToPixelScene(_converterPixToqsc);

        _CGItemDblSidesXAdjWithCompLines_mainMeasure->setAndUpdateParameters_pointCoupleForLeftRightSide(
                    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_left ],
                    _pointCoupleForLineRegDisplay_Left_Right[e_LRsA_right]);

        qDebug() << __FUNCTION__  << " #665";

        _CGItemDblSidesXAdjWithCompLines_mainMeasure->setAndUpdateParameters_xAdjustersPositionsForLeftRightSide(_xRangeForLinearRegressionComputation);

    }

    if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure) {

        qDebug() << __FUNCTION__  << " if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure != nullptr) {";

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->adaptToPixelScene(_converterPixToqsc);

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setXPos(_x0);

        qDebug() << __FUNCTION__  << " #665";
    }

    setSceneRect(_sceneRectF);
    qDebug() << __FUNCTION__  << "setSceneRect with: " << _sceneRectF;

    qreal cccs_xMin_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMin);
    qreal cccs_yMin_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin));

    qreal cccs_xMax_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMax);
    qreal cccs_yMax_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax));

    QRectF qrectFCurveDisplayArea {
        QPointF(cccs_xMin_ToPixelSceneRect,
                cccs_yMin_ToPixelSceneRect),

        QPointF(cccs_xMax_ToPixelSceneRect,
                cccs_yMax_ToPixelSceneRect),
    };

    if (_customGraphicsItemCurve) {
        _customGraphicsItemCurve->adaptToPixelScene(_converterPixToqsc/*, qrectFCurveDisplayArea*/);
    }
    if (_customGraphicsItemEnvelop) {
        _customGraphicsItemEnvelop->adaptToPixelScene(_converterPixToqsc/*, qrectFCurveDisplayArea*/);
    }

}


void CustomGraphicsScene_profil::slot_xAdjusterPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil)";
    emit signal_xAdjusterPosition_changed(static_cast<uint>(_eCA_component), i_eLRsA_locationRelativeToX0CentralAxis, xpos, i_eLRsA);

}

void CustomGraphicsScene_profil::slot_centerX0AdjusterPosition_changed(double xpos) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil)";
    emit signal_centerX0AdjusterPosition_changed(static_cast<uint>(_eCA_component), xpos);
}

void CustomGraphicsScene_profil::setParentPixelSizeForInit(int widgetParentPixelWidth, int widgetParentPixelHeight) {
    _widgetParentPixelWidth  = widgetParentPixelWidth;
    _widgetParentPixelHeight = widgetParentPixelHeight;

    _bWidgetParentPixelWHValid = (_widgetParentPixelWidth > 0) && (_widgetParentPixelHeight > 0);

    qDebug() << __FUNCTION__  << "(CustomGraphicsScene_profil) _bWidgetParentPixelWHValid: " << _bWidgetParentPixelWHValid;
}

double CustomGraphicsScene_profil::cccsY_to_qscY(double y) {
    double ccs_y_range = qAbs(_cccs_yMax - _cccs_yMin);
    return(ccs_y_range - (y - _cccs_yMin));
}

double CustomGraphicsScene_profil::qscY_to_cccsY(double y) {
    double ccs_y_range = qAbs(_cccs_yMax - _cccs_yMin);
    return(ccs_y_range - (y -_cccs_yMin));
}

void CustomGraphicsScene_profil::setXYRange(
                double cccs_xMin, double cccs_xMax,
                double cccs_yMin, double cccs_yMax) {

    _cccs_xMin = cccs_xMin;
    _cccs_xMax = cccs_xMax;
    _cccs_yMin = cccs_yMin;
    _cccs_yMax = cccs_yMax;

    qDebug() << __FUNCTION__;
}

bool CustomGraphicsScene_profil::getYRangeAsSoleLayer(double& cccs_yMin, double &cccs_yMax) {
    if (_eCA_component == e_CA_Invalid) {
        return(false);
    }
    cccs_yMin = _cccs_yMin_whenAlone;
    cccs_yMax = _cccs_yMax_whenAlone;
    return(true);
}

int CustomGraphicsScene_profil::splitOnExponentRemovingHtmlTags(const QString strIn,
                                     QString& str_partWithoutExponent,
                                     QString& str_partWithExponentOnly) {

    int indexOfHtmlSup = strIn.indexOf("<sup>");
    int countExponentChar = 0;
    if (indexOfHtmlSup != -1) {
        str_partWithoutExponent = strIn.left(indexOfHtmlSup);

        #define def_lengthOf_supTag 5 //<sup>
        #define def_lengthOf_slashsupTag 6 //</sup>

         str_partWithExponentOnly = strIn.mid(indexOfHtmlSup+def_lengthOf_supTag);

        qDebug() << __FUNCTION__ << "step1: strExponentOnly =" << str_partWithExponentOnly;
        str_partWithExponentOnly.chop(def_lengthOf_slashsupTag);
        qDebug() << __FUNCTION__ << "step2: strExponentOnly =" << str_partWithExponentOnly;

        countExponentChar = str_partWithExponentOnly.count();
        qDebug() << __FUNCTION__ << "strText_removedHtmlTagsAndExponent = " << str_partWithoutExponent;
        qDebug() << __FUNCTION__ << "strExponentOnly = " << str_partWithExponentOnly;
    }
    return(countExponentChar);
}


void CustomGraphicsScene_profil::setHtmlAndStateForBoundingRectComputation(QGraphicsTextItem *&qGTextItem, const QString& strHtml) {
    qGTextItem->setFont(_fnt);
    qGTextItem->setHtml(strHtml); // /*setPlainText*/
    qGTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    //just to be sure there will be no extra space:
    qGTextItem->document()->setDocumentMargin(0);
    qGTextItem->document()->setIndentWidth(0);
}

QSizeF CustomGraphicsScene_profil::createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem(
    QVector<QString> qvectStrAxisLandMarks, const QFontMetricsF& fntmetrF, const QRectF& qrectFBTight_smallExampleCharExponent,
    QVector<QGraphicsTextItem*>& qvectGraphicsTextItem_Axis, QVector<QSizeF>& qvectAxisPixelSizeFLandMarks) {

    qDebug() << __FUNCTION__ << "&qvectGraphicsTextItem_Axis = " << (void*)&qvectGraphicsTextItem_Axis;
    qDebug() << __FUNCTION__ << "&qvectAxisPixelSizeFLandMarks = " << (void*)&qvectAxisPixelSizeFLandMarks;

    QSizeF maxSizeFTextAxisLandMark {.0,.0};

    for(auto strLm: qvectStrAxisLandMarks) {

        QString strText = strLm;
        qDebug() << __FUNCTION__ << "strText = " << strText;

        //evaluate size of tight boudingbox


        QString strText_partWithoutExponent;
        QString strText_partWithExponentOnly;
        int countExponentChar = 0;

        countExponentChar = splitOnExponentRemovingHtmlTags(strText,
                                                        strText_partWithoutExponent,
                                                        strText_partWithExponentOnly);

        QRectF qrectFBTight = fntmetrF.tightBoundingRect(strText_partWithoutExponent);
        qDebug() << __FUNCTION__ << "step 1: strText_partWithoutExponent = " << qrectFBTight;

        if (countExponentChar) {
            //qDebug() << __FUNCTION__ << " fntmetrF.averageCharWidth() = " << fntmetrF.averageCharWidth();
            //qrectFBTight.setWidth( qrectFBTight.width() + fntmetrF.averageCharWidth() ); //add just one
            //qDebug() << __FUNCTION__ << "step 2: qrectFBTight = " << qrectFBTight;

            qrectFBTight.setHeight( qrectFBTight_smallExampleCharExponent.height());
            qrectFBTight.setWidth( qrectFBTight.width() + qrectFBTight_smallExampleCharExponent.width() ); //add just one

            qDebug() << __FUNCTION__ << "step 3: qrectFBTight = " << qrectFBTight;
        }

        qDebug() << __FUNCTION__ << "final qrectFBTight = " << qrectFBTight;

        QGraphicsTextItem *justAddedTextY = new QGraphicsTextItem("");
        justAddedTextY->setDefaultTextColor({0,0,0});

        setHtmlAndStateForBoundingRectComputation(justAddedTextY, strText);

        addItem(justAddedTextY);

        QRectF bboxGraphicsTextItem = justAddedTextY->boundingRect();
        qDebug() << __FUNCTION__ << "bboxGraphicsTextItem = " << bboxGraphicsTextItem;

        qvectGraphicsTextItem_Axis << justAddedTextY;
        qvectAxisPixelSizeFLandMarks <<  bboxGraphicsTextItem.size();

        if (bboxGraphicsTextItem.width() > maxSizeFTextAxisLandMark.width()) {
            maxSizeFTextAxisLandMark.setWidth(bboxGraphicsTextItem.width());
        }
        if (bboxGraphicsTextItem.height() > maxSizeFTextAxisLandMark.height()) {
            maxSizeFTextAxisLandMark.setHeight(bboxGraphicsTextItem.height());
        }
    }
    return(maxSizeFTextAxisLandMark);
}



bool CustomGraphicsScene_profil::computeLandmarks_and_createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem() {

    QFontMetricsF fntmetrF(_fnt);

    QChar smallSquareTwoChar(253);
    QRectF qrectFBTight_smallExampleCharExponent = fntmetrF.tightBoundingRect(smallSquareTwoChar);
    qDebug() << __FUNCTION__ << "qrectFBTight_smallExampleCharExponent = " << qrectFBTight_smallExampleCharExponent;

    //y
    qDebug() << __FUNCTION__ << "_yLM.set( " << _cccs_yMin << ", " <<  _cccs_yMax << ")";

    //compute y landmarks, and text for the big landmarks
    //compute texts width and height for the big landmarks in pixel size,
    //and compute the biggest text landmark size to reserve enough space for it
    _yLM.set(_cccs_yMin, _cccs_yMax);

    if (!_yLM.compute_fixedSteps_bigDiv_2_100__smallDiv_5()) {
        qDebug() << " error computing y LandmarkInRange";
        return(false);
    }

    _qvectYBigLandMarks = _yLM.getBigLandMarksRef();//@#LP avoid copy constructor


    qDebug() << __FUNCTION__ << "_qvectYBigLandMarks = " << _qvectYBigLandMarks;

    _qvectYPixelSizeFBigLandMarks.clear();
    _qvectStrYBigLandMarks.clear();
    _maxSizeFTextYLandMark = {0,0};

    //compute how the y big landmarks text has to be displayed (with exponent or not, dot and decimal or not, etc)
    if (!_qvectYBigLandMarks.isEmpty()) {

        _qvectStrYBigLandMarks = valuesToPowerString_withUniformPower(_qvectYBigLandMarks);

        qDebug() << __FUNCTION__ << "if (!_qvectYBigLandMarks.isEmpty()) {";
        qDebug() << __FUNCTION__ << "  &_qvectGraphicsTextItem_yAxis_landmarks = " << (void*)&_qvectGraphicsTextItem_yAxis_landmarks;
        qDebug() << __FUNCTION__ << "  &_qvectYPixelSizeFBigLandMarks = " << (void*)&_qvectYPixelSizeFBigLandMarks;

        _maxSizeFTextYLandMark = createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem(
            _qvectStrYBigLandMarks, fntmetrF, qrectFBTight_smallExampleCharExponent,
            _qvectGraphicsTextItem_yAxis_landmarks, _qvectYPixelSizeFBigLandMarks);


        qDebug() << __FUNCTION__ << "  _qvectStrYBigLandMarks = " << _qvectStrYBigLandMarks;
        qDebug() << __FUNCTION__ << "  _qvectStrYBigLandMarks.count() = " << _qvectStrYBigLandMarks.count();

        qDebug() << __FUNCTION__ << "  _qvectGraphicsTextItem_yAxis_landmarks.count() = " << _qvectGraphicsTextItem_yAxis_landmarks.count();


        for(auto& iterGTextItem_yAxis: _qvectGraphicsTextItem_yAxis_landmarks)  {
            iterGTextItem_yAxis->setVisible(false);
        }
    }

    //x

    //compute x landmarks, and text for the big landmarks
    //compute texts width and height for the big landmarks in pixel size, and the biggest text landmark to reserve enough space for it

    _xLM.set(_cccs_xMin, _cccs_xMax);

    if (!_xLM.compute_fixedSteps_min0max_with_intBigDiv_2_3_4_5__intSmallDiv_v2()) {
        qDebug() << " error computing x LandmarkInRange";
        return(false);
    }

    _qvectXBigLandMarks   = _xLM.getBigLandMarksRef();


    _qvectXPixelSizeFBigLandMarks.clear();
    _qvectStrXBigLandMarks.clear();

    _maxSizeFTextXLandMark = {0,0};

    //compute how the x big landmarks text has to be displayed (with exponent or not, dot and decimal or not, etc)
    if (!_qvectXBigLandMarks.isEmpty()) {

        _qvectStrXBigLandMarks = valuesToPowerString_withUniformPower(_qvectXBigLandMarks);

        if (!_qvectStrXBigLandMarks.isEmpty()) {

            qDebug() << __FUNCTION__ << "if (!_qvectStrXBigLandMarks.isEmpty()) {";
            qDebug() << __FUNCTION__ << "&_qvectGraphicsTextItem_xAxis_landmarks = " << (void*)&_qvectGraphicsTextItem_xAxis_landmarks;
            qDebug() << __FUNCTION__ << "&_qvectXPixelSizeFBigLandMarks = " << (void*)&_qvectXPixelSizeFBigLandMarks;

            _maxSizeFTextXLandMark = createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem(
                _qvectStrXBigLandMarks, fntmetrF, qrectFBTight_smallExampleCharExponent,
                _qvectGraphicsTextItem_xAxis_landmarks, _qvectXPixelSizeFBigLandMarks);

            for(auto& iterGTextItem_XAxis: _qvectGraphicsTextItem_xAxis_landmarks)  {
                iterGTextItem_XAxis->setVisible(false);
            }
        }
    }

    return(true);
}


bool CustomGraphicsScene_profil::get_sceneRectF_areaOfLayerWhenAlone_pixelDim(QRectF& sceneRectF) {

    if (_eCA_component == e_CA_Invalid) {
        return(false);
    }
    sceneRectF = _sceneRectF_areaOfLayerWhenAlone_pixelDim;

    return(true);
}


bool CustomGraphicsScene_profil::get_sceneRectF_areaOfLayerWhenAlone_pixelDim(QRectF& sceneRectF, double& wSpacePixelNeedsAtLeft, double& wSpacePixelNeedsAtRight) {

    if (_eCA_component == e_CA_Invalid) {
        return(false);
    }
    sceneRectF = _sceneRectF_areaOfLayerWhenAlone_pixelDim;

    wSpacePixelNeedsAtLeft = _wSumPixelAtLeft_whenAlone;
    wSpacePixelNeedsAtRight = _wSumPixelAtRight_whenAlone;

    return(true);
}

//This method now provide the sceneAre and spaces in qcs and pixel units
// The first implementation of the scene management was all in qcs. But due to constraint about impossible width of graphics item != 1 (ignore aspect ratio and setcosmetic(true) on qpens,
// the scene is now handled as pixel scene. The CustomGraphicsScene_profil is still feed with math-logic coordinates and for graphics alignements between different graphics,
// the CustomGraphicsScene_profil will received pixel area size insteaf of math-logic coordinates.
// To avoid amibiguity and flexibility, the values feed by this method are provided in the two units (math-logic(qcsDim), and pixelDim)
bool CustomGraphicsScene_profil::computeSceneAreaSize(
        QRectF& resultSceneArea_qcsDim,
        QRectF& resultSceneArea_pixelDim,

        S_ForResizeComputation_noTextLandMarksChange& forResizeComputation,

        double& wSpaceAtLeft_qcsDim, double& wSpaceAtRight_qcsDim,
        double& wSpaceAtLeft_pixelDim, double& wSpaceAtRight_pixelDim) {

    //Compute the size of the sceneRect from an initial display of the destination view.
    //Because text Landmarks and some elements like the Landmark and some spaces are fixed in pixel and will not scale
    //and because these elements take space in the scene, the whole scene is bigger than the graphic area with the curve

    //@#LP fix qsc cccs correct denomination for pixel_y_to_qsc(init) and other unsyc variables names with the same unsync


    //qsc: qt scene coordinates
    //scene width with space around graphics area


    //qDebug() << __FUNCTION__ << "_wPixelSpaceOutBorder: " << _wPixelSpaceOutBorder;
    //qDebug() << __FUNCTION__ <<  "pixelSizeMaxTextYLandmark.width():" << _maxSizeFTextYLandMark.width();
    //qDebug() << __FUNCTION__ <<  "_wPixelSpaceBetweenTextLandmarkAndYAxis:" << _wPixelSpaceBetweenTextLandmarkAndYAxis;
    //qDebug() << __FUNCTION__ <<  "_wPixelSpaceOutBorder:" << _wPixelSpaceOutBorder;

    double qsc_widthGraphicArea = qAbs(_cccs_xMax - _cccs_xMin);
    forResizeComputation._qsc_widthGraphicArea = qsc_widthGraphicArea;


    //here we compute pixel widths parts of first and last x text landmarks going over the curve area
    //to be able to let enough space for them if requiered

/*
   V loc in range 0, range = first x landmark  - _cccs_xMin
   loc in pixel in widget = (V loc in range 0, range * widget width ) / range
   left border = loc in pixel in widget - _qvectXPixelSizeFBigLandMarks.first().width()/2.0);
   if (left border < 0 ) => add (0 - left border)

   V loc in range 0, range = last x landmark  - _cccs_xMin
   loc in pixel in widget = (V loc in range 0, range * widget width ) / range
   right border = loc in pixel in widget + _qvectXPixelSizeFBigLandMarks.last().width()/2.0);
   if (left border >  widget width ) => add (left border - widget width)
*/
    double pixelPartOfFirstXLandmarkGoingOverAtLeftBorder = .0;
    bool bFirstXTextLandmarkGoOverCurveArea = false;

    double pixelPartOfLastXLandmarkGoingOverAtRightBorder = .0;
    bool bLastXTextLandmarkGoOverCurveArea = false;

    qDebug() << __FUNCTION__ << __LINE__ << "_qvectXPixelSizeFBigLandMarks = " << _qvectXPixelSizeFBigLandMarks;


    int qvectXPixelSizeFBigLandMarks_size = _qvectXPixelSizeFBigLandMarks.size();
    if (qvectXPixelSizeFBigLandMarks_size) {

        double locInWidgetAreaIfNoneOutBorder = ((_xLM.getBigLandMarksRef().first() - _cccs_xMin) * _widgetParentPixelWidth ) / qsc_widthGraphicArea;
        double locLeftBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder - (_qvectXPixelSizeFBigLandMarks.first().width()/2.0);

        qDebug() << __FUNCTION__ << _xLM.getBigLandMarksRef().first() << "<=> " << " _qvectXPixelSizeFBigLandMarks.first().width()/2.0 = " << _qvectXPixelSizeFBigLandMarks.first().width()/2.0;
        qDebug() << __FUNCTION__ << "locLeftBorderInWidgetAreaIfNoneOutBorder = " << locLeftBorderInWidgetAreaIfNoneOutBorder;

        if (locLeftBorderInWidgetAreaIfNoneOutBorder < .0) {
            bFirstXTextLandmarkGoOverCurveArea = true;
            pixelPartOfFirstXLandmarkGoingOverAtLeftBorder = (.0 - locLeftBorderInWidgetAreaIfNoneOutBorder);
        }

        if (qvectXPixelSizeFBigLandMarks_size > 1) {
            locInWidgetAreaIfNoneOutBorder = ((_xLM.getBigLandMarksRef().last() - _cccs_xMin) * _widgetParentPixelWidth ) / qsc_widthGraphicArea;
            double locRightBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder + (_qvectXPixelSizeFBigLandMarks.last().width()/2.0);

            qDebug() << __FUNCTION__ << _xLM.getBigLandMarksRef().last() << "<=>" << "_qvectXPixelSizeFBigLandMarks.last().width()/2.0 = " << _qvectXPixelSizeFBigLandMarks.last().width()/2.0;
            qDebug() << __FUNCTION__ << "locRightBorderInWidgetAreaIfNoneOutBorder = " << locRightBorderInWidgetAreaIfNoneOutBorder;
            qDebug() << __FUNCTION__ << "_widgetParentPixelWidth= " << _widgetParentPixelWidth;

            if (locRightBorderInWidgetAreaIfNoneOutBorder > _widgetParentPixelWidth) {
                bLastXTextLandmarkGoOverCurveArea = true;
                pixelPartOfLastXLandmarkGoingOverAtRightBorder = (locRightBorderInWidgetAreaIfNoneOutBorder - _widgetParentPixelWidth);
            }
        }
    }

    qDebug() << __FUNCTION__ << "pixelPartOfLastXLandmarkGoingOverAtRightBorder= " << pixelPartOfLastXLandmarkGoingOverAtRightBorder;



    //used for scene rect expansion:
    double wSumPixelAtLeft_usingYlandmark =
              static_cast<double>(_wPixelSpaceOutBorder)
            + static_cast<double>(_maxSizeFTextYLandMark.width())
            + static_cast<double>(_wPixelSpaceBetweenTextLandmarkAndYAxis);

    double wSumPixelAtLeft_usingXlandmarkGoindOverCurveArea =
              static_cast<double>(_wPixelSpaceOutBorder)
            + static_cast<double>(pixelPartOfFirstXLandmarkGoingOverAtLeftBorder);

    double wSumPixelAtLeft = wSumPixelAtLeft_usingYlandmark;
    if (wSumPixelAtLeft_usingXlandmarkGoindOverCurveArea > wSumPixelAtLeft) {
        wSumPixelAtLeft = wSumPixelAtLeft_usingXlandmarkGoindOverCurveArea;
    }
    _wSumPixelAtLeft_whenAlone = wSumPixelAtLeft;


    qDebug() << __FUNCTION__ << "wSumPixelAtLeft_usingYlandmark = " << wSumPixelAtLeft_usingYlandmark;
    qDebug() << __FUNCTION__ << "wSumPixelAtLeft_usingXlandmarkGoindOverCurveArea = " << wSumPixelAtLeft_usingXlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "wSumPixelAtLeft = " << wSumPixelAtLeft;



    //used for scene rect expansion:
    double wSumPixelAtRight_usingYlandmark =
              static_cast<double>(_wPixelSpaceOutBorder);

    double wSumPixelAtRight_usingXlandmarkGoindOverCurveArea =
              static_cast<double>(_wPixelSpaceOutBorder)
            + static_cast<double>(pixelPartOfLastXLandmarkGoingOverAtRightBorder);

    double wSumPixelAtRight = wSumPixelAtRight_usingYlandmark;
    if (wSumPixelAtRight_usingXlandmarkGoindOverCurveArea > wSumPixelAtRight) {
        wSumPixelAtRight = wSumPixelAtRight_usingXlandmarkGoindOverCurveArea;
    }
    _wSumPixelAtRight_whenAlone = wSumPixelAtRight;


    double wSumPixelAroundGraphicsArea = wSumPixelAtLeft + wSumPixelAtRight;



    qDebug() << __FUNCTION__ << "wSumPixelAtRight_usingYlandmark = " << wSumPixelAtRight_usingYlandmark;
    qDebug() << __FUNCTION__ << "wSumPixelAtRight_usingXlandmarkGoindOverCurveArea = " << wSumPixelAtRight_usingXlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "wSumPixelAtRight = " << wSumPixelAtRight;
    qDebug() << __FUNCTION__ << "wSumPixelAroundGraphicsArea = " << wSumPixelAroundGraphicsArea;


    //
    //  widthWidget-wSumPixelAroundGraphicsArea  <-|->  qsc_widthGraphicArea
    //  - - - - - - - - - - - - - - - - - - - - - -|- - - - - - - - - - - - -
    //        wSumPixelAroundGraphicsArea        <-|->  qsc value to compute
    //

    qDebug() << "wSumPixelAroundGraphicsArea  = " << wSumPixelAroundGraphicsArea;
    qDebug() << "qsc_widthGraphicArea (double) = " << qsc_widthGraphicArea;
    qDebug() << "_widgetParentPixelWidth (int) = " << _widgetParentPixelWidth;

    if (static_cast<double>(_widgetParentPixelWidth) - wSumPixelAroundGraphicsArea <= 0) { //@LP divider checked not 0
        qDebug() << __FUNCTION__ << "Width too small";
        return(false);
    }
    double wSpaceAroundGraphicsArea_qsc = (wSumPixelAroundGraphicsArea * qsc_widthGraphicArea) / (_widgetParentPixelWidth - wSumPixelAroundGraphicsArea);
    double widthSceneWithSpaceAroundGraphicsArea_qsc = qsc_widthGraphicArea + wSpaceAroundGraphicsArea_qsc;

    qDebug() << __FUNCTION__ <<  "wSpaceAroundGraphicsArea_qsc = " << wSpaceAroundGraphicsArea_qsc;
    qDebug() << __FUNCTION__ <<  "widthSceneWithSpaceAroundGraphicsArea_qsc = " << widthSceneWithSpaceAroundGraphicsArea_qsc;

    _converterPixToqsc.setWidthRatioFrom(widthSceneWithSpaceAroundGraphicsArea_qsc, _widgetParentPixelWidth);

    double wSpaceAtLeft_qsc = _converterPixToqsc.pixel_x_to_qsc(wSumPixelAtLeft);

    qDebug() << __FUNCTION__ <<  "wSumPixelAtLeft:" << wSumPixelAtLeft;
    qDebug() << __FUNCTION__ <<  "wSpaceAtLeft_qsc:" << wSpaceAtLeft_qsc;

    //for x alignement, new way
    wSpaceAtLeft_qcsDim  = wSpaceAtLeft_qsc;
    wSpaceAtRight_qcsDim = _converterPixToqsc.pixel_x_to_qsc(wSumPixelAtRight);

    wSpaceAtLeft_pixelDim = wSumPixelAtLeft;
    wSpaceAtRight_pixelDim = wSumPixelAtRight;




    qDebug() << __FUNCTION__ << "_qstrFontToUse = " << _qstrFontToUse;

    QRectF qr_notused_t, qr_notused_nt;
    QSizeF pixelSizeFMaxTextXLandmark =
            pixelWidthHeight_ofStringForFontWithPixelFontSize(
                QString::number(_cccs_xMin),
                _qstrFontToUse, _fontSizeForTextLandmark, qr_notused_t, qr_notused_nt);

    //scene height with space around graphics area
    //same principle than above
    double qsc_heightGraphicArea = _cccs_yMax - _cccs_yMin;
    forResizeComputation._qsc_heightGraphicArea = qsc_heightGraphicArea;

    double pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = .0;
    bool bFirstYTextLandmarkGoOverCurveArea = false;

    double pixelPartOfLastYLandmarkGoingOverAtTopBorder = .0;
    bool bLastYTextLandmarkGoOverCurveArea = false;

    int qvectYPixelSizeFBigLandMarks_size = _qvectYPixelSizeFBigLandMarks.size();
    if (qvectYPixelSizeFBigLandMarks_size) {

        double locInWidgetAreaIfNoneOutBorder = ((_yLM.getBigLandMarksRef().first() - _cccs_yMin) * _widgetParentPixelHeight ) / qsc_heightGraphicArea;
        double locBottomBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder - (_qvectYPixelSizeFBigLandMarks.first().height()/2.0);

        qDebug() << __FUNCTION__ << "pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = " << pixelPartOfFirstYLandmarkGoingOverAtBottomBorder;

        if (pixelPartOfFirstYLandmarkGoingOverAtBottomBorder < .0) {
            bFirstYTextLandmarkGoOverCurveArea = true;
            pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = (.0 - locBottomBorderInWidgetAreaIfNoneOutBorder);
        }

        if (qvectYPixelSizeFBigLandMarks_size > 1) {
            locInWidgetAreaIfNoneOutBorder = ((_yLM.getBigLandMarksRef().last() - _cccs_yMin) * _widgetParentPixelHeight ) / qsc_heightGraphicArea;
            double locTopBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder + (_qvectYPixelSizeFBigLandMarks.last().height()/2.0);

            qDebug() << __FUNCTION__ << "locTopBorderInWidgetAreaIfNoneOutBorder = " << locTopBorderInWidgetAreaIfNoneOutBorder;
            qDebug() << __FUNCTION__ << "_widgetParentPixelHeight= " << _widgetParentPixelHeight;

            if (locTopBorderInWidgetAreaIfNoneOutBorder > _widgetParentPixelHeight) {
                bLastYTextLandmarkGoOverCurveArea = true;
                pixelPartOfLastYLandmarkGoingOverAtTopBorder = (locTopBorderInWidgetAreaIfNoneOutBorder - _widgetParentPixelHeight);
            }
        }
    }


    //used for scene rect expansion:
    double hSumPixelAtBottom_usingXlandmark =
              static_cast<double>(_hPixelSpaceOutBorder)
            + _pixel_gripSquareSideHeight
            + pixelSizeFMaxTextXLandmark.height()
            + _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis;

    double hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea =
              static_cast<double>(_hPixelSpaceOutBorder)
            + pixelPartOfFirstYLandmarkGoingOverAtBottomBorder;

    double hSumPixelAtBottom = hSumPixelAtBottom_usingXlandmark;
    if (hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea > hSumPixelAtBottom) {
        hSumPixelAtBottom = hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea;
    }

    qDebug() << __FUNCTION__ << "hSumPixelAtBottom_usingXlandmark = " << hSumPixelAtBottom_usingXlandmark;
    qDebug() << __FUNCTION__ << "hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea = " << hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "hSumPixelAtBottom = " << hSumPixelAtBottom;


    //used for scene rect expansion:
    double hSumPixelAtTop_usingXlandmark =
              static_cast<double>(_hPixelSpaceOutBorder);

    double hSumPixelAtTop_usingYlandmarkGoindOverCurveArea =
              static_cast<double>(_hPixelSpaceOutBorder)
            + static_cast<double>(pixelPartOfLastYLandmarkGoingOverAtTopBorder);

    double hSumPixelAtTop = hSumPixelAtTop_usingXlandmark;
    if (hSumPixelAtTop_usingYlandmarkGoindOverCurveArea > hSumPixelAtTop) {
        hSumPixelAtTop = hSumPixelAtTop_usingYlandmarkGoindOverCurveArea;
    }
    forResizeComputation._hSumPixelAtTop = hSumPixelAtTop;
    forResizeComputation._hSumPixelAtBottom = hSumPixelAtBottom;

    double hSumPixelAroundGraphicsArea = hSumPixelAtBottom + hSumPixelAtTop;

    qDebug() << __FUNCTION__ << "hSumPixelAtTop_usingXlandmark = " << hSumPixelAtTop_usingXlandmark;
    qDebug() << __FUNCTION__ << "hSumPixelAtTop_usingYlandmarkGoindOverCurveArea = " << hSumPixelAtTop_usingYlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "hSumPixelAtTop = " << hSumPixelAtTop;

    if (static_cast<double>(_widgetParentPixelHeight) - hSumPixelAroundGraphicsArea <= 0) { //@LP divider checked not 0
        qDebug() << __FUNCTION__ << "height too small";
        return(false);
    }
    double ySpaceAroundGraphicsArea_qsc = (hSumPixelAroundGraphicsArea * qsc_heightGraphicArea) / (_widgetParentPixelHeight - hSumPixelAroundGraphicsArea);
    double heightSceneWithSpaceAroundGraphicsArea_qsc = qsc_heightGraphicArea + ySpaceAroundGraphicsArea_qsc;

    qDebug() << __FUNCTION__ << "_hSumPixelAroundGraphicsArea = " << hSumPixelAroundGraphicsArea;
    qDebug() << __FUNCTION__ << "qsc_heightGraphicArea = " << qsc_heightGraphicArea;
    qDebug() << __FUNCTION__ << "heightSceneWithSpaceAroundGraphicsArea_qsc  = " << heightSceneWithSpaceAroundGraphicsArea_qsc;
    qDebug() << __FUNCTION__ << "ySpaceAroundGraphicsArea_qsc = " << ySpaceAroundGraphicsArea_qsc;

    _converterPixToqsc.setHeightRatioFrom(heightSceneWithSpaceAroundGraphicsArea_qsc, _widgetParentPixelHeight);

    //used for scene rect expension:

    double ySpaceAtTop_qsc = (hSumPixelAtTop * qsc_heightGraphicArea) / (_widgetParentPixelHeight - hSumPixelAroundGraphicsArea);


    qDebug() << __FUNCTION__ <<"hSumPixelAtTop  = " << hSumPixelAtTop;
    qDebug() << __FUNCTION__ <<"ySpaceAtTop_qsc = " << ySpaceAtTop_qsc;

#define DEF_forDevDebugPurposeOnly
#ifdef DEF_forDevDebugPurposeOnly
    double ySpaceAtBottom_qsc = (hSumPixelAtBottom * qsc_heightGraphicArea) / (_widgetParentPixelHeight - hSumPixelAroundGraphicsArea);
    qDebug() << __FUNCTION__ << "DEF_forDevDebugPurposeOnly" << " hSumPixelAtBottom = " << hSumPixelAtBottom;
    qDebug() << __FUNCTION__ << "DEF_forDevDebugPurposeOnly" << " ySpaceAtBottom_qsc = " << ySpaceAtBottom_qsc;
#endif

    //for convenience:
    //x follow the cccs. Hence we extend left and right to let scene space for text and over stuff around the graphic area
    //y axis (value y=0) is set to be the upper screen value of the graphic area (graphic area = the area where the curve can be shown)

    resultSceneArea_qcsDim.setLeft(_cccs_xMin - wSpaceAtLeft_qsc);
    resultSceneArea_qcsDim.setTop( - ySpaceAtTop_qsc);

    resultSceneArea_qcsDim.setSize({widthSceneWithSpaceAroundGraphicsArea_qsc, heightSceneWithSpaceAroundGraphicsArea_qsc});

    qDebug() << __FUNCTION__ << "resultSceneArea_qcsDim left, top : " << resultSceneArea_qcsDim.left()  << ", " << resultSceneArea_qcsDim.top();
    qDebug() << __FUNCTION__ << "resultSceneArea_qcsDim    size   : " << resultSceneArea_qcsDim.width() << ", " << resultSceneArea_qcsDim.height();

    qDebug() << __FUNCTION__ << "_cccs_xMin ,_cccs_xMax = " << _cccs_xMin << ", " << _cccs_xMax;
    qDebug() << __FUNCTION__ << "_cccs_yMin ,_cccs_yMax = " << _cccs_yMin << ", " << _cccs_yMax;


    qDebug() << __FUNCTION__ << " resultSceneArea_qcsDim = " << resultSceneArea_qcsDim;

    //@LP: use now a computed scene rect as a pixel scene rect (instead of the logic math space)
    //This is because width at 1 for somes items is very too thin. + on Mac OS X, the pixel width is thinner (divided by two)
    //It seems that the independant device pixel on MacOS X doesn't work as expected (no difference seens unsing flag in Info.plist) (?))
    //And in any case, the possibility to have some items with its width > 1 is a good feature to have for the profil linear model edition.
    //
    //to be able to remove setCosmetic(true) on qpen + avoid using ignore aspect ratio + permits to setWidth (in pixel) on qpen
    //this change is about to permit at the end: constant width for lines typically
    //
    //Previoulsy: used setCosmetic(true) on qpen + ignore aspect ratio => works good as any item  width was 1
    //removing setCosmetic(true) but letting ignore aspect ratio produces different "scales" factor for x and y axis
    //the result is that line width depends on its direction. This is really bad for the profile curve, and for the lines about the linear regression model
    //

    resultSceneArea_pixelDim.setLeft(_converterPixToqsc.qsc_x_to_pixel(resultSceneArea_qcsDim.left()));
    resultSceneArea_pixelDim.setWidth(_widgetParentPixelWidth);
    resultSceneArea_pixelDim.setTop(_converterPixToqsc.qsc_y_to_pixel(resultSceneArea_qcsDim.top()));
    resultSceneArea_pixelDim.setHeight(_widgetParentPixelHeight);

    return(true);
}


//@#LP ambiguity about unit naming
bool CustomGraphicsScene_profil::computeSceneAreaSize_usingValuesFromOutsideForXAlignement(
        double throughAllScene_left_pixelDim, double throughAllScene_right_pixelDim,
        double throughAllScene_wSceneSpaceAtLeft_pixelDim, double throughAllScene_wSceneSpaceAtRight_pixelDim,

        QRectF& resultSceneArea_qcsDim,
        QRectF& resultSceneArea_pixelDim,

        S_ForResizeComputation_noTextLandMarksChange& forResizeComputation,
        double& wSpaceAtLeft_pixelDim, double& wSpaceAtRight_pixelDim) {

    //Compute the size of the sceneRect from values from outside.
    //Because text Landmarks and some elements like the Landmark and some spaces are fixed in pixel and will not scale
    //and because these elements take space in the scene, the whole scene is bigger than the graphic area with the curve

    //@#LP fix qsc cccs correct denomination for pixel_y_to_qsc(init) and other unsyc variables names with the same unsync

    //qsc: qt scene coordinates
    //scene width with space around graphics area


    //qDebug() << __FUNCTION__ << "_wPixelSpaceOutBorder: " << _wPixelSpaceOutBorder;
    //qDebug() << __FUNCTION__ <<  "pixelSizeMaxTextYLandmark.width():" << _maxSizeFTextYLandMark.width();
    //qDebug() << __FUNCTION__ <<  "_wPixelSpaceBetweenTextLandmarkAndYAxis:" << _wPixelSpaceBetweenTextLandmarkAndYAxis;
    //qDebug() << __FUNCTION__ <<  "_wPixelSpaceOutBorder:" << _wPixelSpaceOutBorder;

    qDebug() << __FUNCTION__ <<  "received: throughAllScene_left_pixelDim:" << throughAllScene_left_pixelDim;
    qDebug() << __FUNCTION__ <<  "received: throughAllScene_right_pixelDim:" << throughAllScene_right_pixelDim;
    qDebug() << __FUNCTION__ <<  "received: throughAllScene_wSceneSpaceAtLeft_pixelDim:" << throughAllScene_wSceneSpaceAtLeft_pixelDim;
    qDebug() << __FUNCTION__ <<  "received: throughAllScene_wSceneSpaceAtRight_pixelDim:" << throughAllScene_wSceneSpaceAtRight_pixelDim;

    double qsc_widthGraphicArea = qAbs(_cccs_xMax - _cccs_xMin);
    forResizeComputation._qsc_widthGraphicArea = qsc_widthGraphicArea;


    //the values from outside let enough space for any cases as we got here the maximum spaces through all layers

    //here we compute pixel widths parts of first and last x text landmarks going over the curve area
    //to be able to let enough space for them if requiered

/*
   V loc in range 0, range = first x landmark  - _cccs_xMin
   loc in pixel in widget = (V loc in range 0, range * widget width ) / range
   left border = loc in pixel in widget - _qvectXPixelSizeFBigLandMarks.first().width()/2.0);
   if (left border < 0 ) => add (0 - left border)

   V loc in range 0, range = last x landmark  - _cccs_xMin
   loc in pixel in widget = (V loc in range 0, range * widget width ) / range
   right border = loc in pixel in widget + _qvectXPixelSizeFBigLandMarks.last().width()/2.0);
   if (left border >  widget width ) => add (left border - widget width)
*/

    double widthSceneWithSpaceAroundGraphicsArea_pixelDim = (throughAllScene_right_pixelDim - throughAllScene_left_pixelDim);


    qDebug() << __FUNCTION__ << "widthSceneWithSpaceAroundGraphicsArea_pixelDim = " << widthSceneWithSpaceAroundGraphicsArea_pixelDim;

    //
    //  widthWidget  <-|->  wLeftToRightScene + wSumwSceneSpaceAroundGraphicsArea
    //  - - - - - - - -|- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //               <-|->
    //

    //qDebug() << "wSumPixelAroundGraphicsArea  = " << wSumPixelAroundGraphicsArea;
    //qDebug() << "qsc_widthGraphicArea (double) = " << qsc_widthGraphicArea;
    //qDebug() << "_widgetParentPixelWidth (int) = " << _widgetParentPixelWidth;

    double wSumPixelAroundGraphicsArea = throughAllScene_wSceneSpaceAtLeft_pixelDim + throughAllScene_wSceneSpaceAtRight_pixelDim;

    if (static_cast<double>(_widgetParentPixelWidth) - wSumPixelAroundGraphicsArea <= 0) { //@LP divider checked not 0
        qDebug() << __FUNCTION__ << "Width too small";
        return(false);
    }
    double wSpaceAroundGraphicsArea_qsc = (wSumPixelAroundGraphicsArea * qsc_widthGraphicArea) / (_widgetParentPixelWidth - wSumPixelAroundGraphicsArea);
    double widthSceneWithSpaceAroundGraphicsArea_qsc = qsc_widthGraphicArea + wSpaceAroundGraphicsArea_qsc;

    qDebug() << __FUNCTION__ <<  "wSumPixelAroundGraphicsArea = " << wSumPixelAroundGraphicsArea;
    qDebug() << __FUNCTION__ <<  "wSpaceAroundGraphicsArea_qsc = " << wSpaceAroundGraphicsArea_qsc;
    qDebug() << __FUNCTION__ <<  "widthSceneWithSpaceAroundGraphicsArea_qsc = " << widthSceneWithSpaceAroundGraphicsArea_qsc;

    _converterPixToqsc.setWidthRatioFrom(widthSceneWithSpaceAroundGraphicsArea_qsc, _widgetParentPixelWidth);

    double wSpaceAtLeft_qsc = _converterPixToqsc.pixel_x_to_qsc(throughAllScene_wSceneSpaceAtLeft_pixelDim);

    qDebug() << __FUNCTION__ << "wSpaceAtLeft_qsc = " << wSpaceAtLeft_qsc;



    //qDebug() << __FUNCTION__ <<  "wSumPixelAtLeft:" << wSumPixelAtLeft;
    //qDebug() << __FUNCTION__ <<  "wSpaceAtLeft_qsc:" << wSpaceAtLeft_qsc;

    //for x alignement, new way

    wSpaceAtRight_pixelDim = throughAllScene_wSceneSpaceAtRight_pixelDim;
    wSpaceAtLeft_pixelDim = throughAllScene_wSceneSpaceAtLeft_pixelDim;


    qDebug() << __FUNCTION__ << "_qstrFontToUse = " << _qstrFontToUse;

    QRectF qr_notused_t, qr_notused_nt;
    QSizeF pixelSizeFMaxTextXLandmark =
            pixelWidthHeight_ofStringForFontWithPixelFontSize(
                QString::number(_cccs_xMin),
                _qstrFontToUse, _fontSizeForTextLandmark, qr_notused_t, qr_notused_nt);


    //scene height with space around graphics area

    double qsc_heightGraphicArea = _cccs_yMax - _cccs_yMin;
    forResizeComputation._qsc_heightGraphicArea = qsc_heightGraphicArea;

    double pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = .0;
    bool bFirstYTextLandmarkGoOverCurveArea = false;

    double pixelPartOfLastYLandmarkGoingOverAtTopBorder = .0;
    bool bLastYTextLandmarkGoOverCurveArea = false;

    int qvectYPixelSizeFBigLandMarks_size = _qvectYPixelSizeFBigLandMarks.size();
    if (qvectYPixelSizeFBigLandMarks_size) {

        double locInWidgetAreaIfNoneOutBorder = ((_yLM.getBigLandMarksRef().first() - _cccs_yMin) * _widgetParentPixelHeight ) / qsc_heightGraphicArea;
        double locBottomBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder - (_qvectYPixelSizeFBigLandMarks.first().height()/2.0);

        qDebug() << __FUNCTION__ << "pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = " << pixelPartOfFirstYLandmarkGoingOverAtBottomBorder;

        if (pixelPartOfFirstYLandmarkGoingOverAtBottomBorder < .0) {
            bFirstYTextLandmarkGoOverCurveArea = true;
            pixelPartOfFirstYLandmarkGoingOverAtBottomBorder = (.0 - locBottomBorderInWidgetAreaIfNoneOutBorder);
        }

        if (qvectYPixelSizeFBigLandMarks_size > 1) {
            locInWidgetAreaIfNoneOutBorder = ((_yLM.getBigLandMarksRef().last() - _cccs_yMin) * _widgetParentPixelHeight ) / qsc_heightGraphicArea;
            double locTopBorderInWidgetAreaIfNoneOutBorder = locInWidgetAreaIfNoneOutBorder + (_qvectYPixelSizeFBigLandMarks.last().height()/2.0);

            qDebug() << __FUNCTION__ << "locTopBorderInWidgetAreaIfNoneOutBorder = " << locTopBorderInWidgetAreaIfNoneOutBorder;
            qDebug() << __FUNCTION__ << "_widgetParentPixelHeight= " << _widgetParentPixelHeight;

            if (locTopBorderInWidgetAreaIfNoneOutBorder > _widgetParentPixelHeight) {
                bLastYTextLandmarkGoOverCurveArea = true;
                pixelPartOfLastYLandmarkGoingOverAtTopBorder = (locTopBorderInWidgetAreaIfNoneOutBorder - _widgetParentPixelHeight);
            }
        }
    }


    //used for scene rect expansion:
    double hSumPixelAtBottom_usingXlandmark =
              static_cast<double>(_hPixelSpaceOutBorder)
            + _pixel_gripSquareSideHeight
            + pixelSizeFMaxTextXLandmark.height()
            + _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis;

    double hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea =
              static_cast<double>(_hPixelSpaceOutBorder)
            + pixelPartOfFirstYLandmarkGoingOverAtBottomBorder;

    double hSumPixelAtBottom = hSumPixelAtBottom_usingXlandmark;
    if (hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea > hSumPixelAtBottom) {
        hSumPixelAtBottom = hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea;
    }

    qDebug() << __FUNCTION__ << "hSumPixelAtBottom_usingXlandmark = " << hSumPixelAtBottom_usingXlandmark;
    qDebug() << __FUNCTION__ << "hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea = " << hSumPixelAtBottom_usingYlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "hSumPixelAtBottom = " << hSumPixelAtBottom;


    //used for scene rect expansion:
    double hSumPixelAtTop_usingXlandmark =
              static_cast<double>(_hPixelSpaceOutBorder);

    double hSumPixelAtTop_usingYlandmarkGoindOverCurveArea =
              static_cast<double>(_hPixelSpaceOutBorder)
            + static_cast<double>(pixelPartOfLastYLandmarkGoingOverAtTopBorder);

    double hSumPixelAtTop = hSumPixelAtTop_usingXlandmark;
    if (hSumPixelAtTop_usingYlandmarkGoindOverCurveArea > hSumPixelAtTop) {
        hSumPixelAtTop = hSumPixelAtTop_usingYlandmarkGoindOverCurveArea;
    }
    forResizeComputation._hSumPixelAtTop = hSumPixelAtTop;
    forResizeComputation._hSumPixelAtBottom = hSumPixelAtBottom;

    double hSumPixelAroundGraphicsArea = hSumPixelAtBottom + hSumPixelAtTop;

    qDebug() << __FUNCTION__ << "hSumPixelAtTop_usingXlandmark = " << hSumPixelAtTop_usingXlandmark;
    qDebug() << __FUNCTION__ << "hSumPixelAtTop_usingYlandmarkGoindOverCurveArea = " << hSumPixelAtTop_usingYlandmarkGoindOverCurveArea;
    qDebug() << __FUNCTION__ << "hSumPixelAtTop = " << hSumPixelAtTop;

    if (static_cast<double>(_widgetParentPixelHeight) - hSumPixelAroundGraphicsArea <= 0) { //@LP divider checked not 0
        qDebug() << __FUNCTION__ << "height too small";
        return(false);
    }
    double ySpaceAroundGraphicsArea_qsc = (hSumPixelAroundGraphicsArea * qsc_heightGraphicArea) / (_widgetParentPixelHeight - hSumPixelAroundGraphicsArea);
    double heightSceneWithSpaceAroundGraphicsArea_qsc = qsc_heightGraphicArea + ySpaceAroundGraphicsArea_qsc;

    qDebug() << __FUNCTION__ << "_hSumPixelAroundGraphicsArea = " << hSumPixelAroundGraphicsArea;
    qDebug() << __FUNCTION__ << "qsc_heightGraphicArea = " << qsc_heightGraphicArea;
    qDebug() << __FUNCTION__ << "heightSceneWithSpaceAroundGraphicsArea_qsc  = " << heightSceneWithSpaceAroundGraphicsArea_qsc;
    qDebug() << __FUNCTION__ << "ySpaceAroundGraphicsArea_qsc = " << ySpaceAroundGraphicsArea_qsc;

    _converterPixToqsc.setHeightRatioFrom(heightSceneWithSpaceAroundGraphicsArea_qsc, _widgetParentPixelHeight);

    //used for scene rect expension:

    double ySpaceAtTop_qsc = (hSumPixelAtTop * qsc_heightGraphicArea) / (_widgetParentPixelHeight - hSumPixelAroundGraphicsArea);


    qDebug() << __FUNCTION__ <<"hSumPixelAtTop  = " << hSumPixelAtTop;
    qDebug() << __FUNCTION__ <<"ySpaceAtTop_qsc = " << ySpaceAtTop_qsc;

    //for convenience:
    //x follow the cccs. Hence we extend left and right to let scene space for text and over stuff around the graphic area
    //y axis (value y=0) is set to be the upper screen value of the graphic area (graphic area = the area where the curve can be shown)

    resultSceneArea_qcsDim.setLeft(_cccs_xMin - wSpaceAtLeft_qsc);
    resultSceneArea_qcsDim.setTop( - ySpaceAtTop_qsc);

    resultSceneArea_qcsDim.setSize({_converterPixToqsc.pixel_x_to_qsc(widthSceneWithSpaceAroundGraphicsArea_pixelDim),
                                                                        heightSceneWithSpaceAroundGraphicsArea_qsc});

    qDebug() << __FUNCTION__ << "resultSceneArea_qcsDim left, top : " << resultSceneArea_qcsDim.left()  << ", " << resultSceneArea_qcsDim.top();
    qDebug() << __FUNCTION__ << "resultSceneArea_qcsDim    size   : " << resultSceneArea_qcsDim.width() << ", " << resultSceneArea_qcsDim.height();

    qDebug() << __FUNCTION__ << "_cccs_xMin ,_cccs_xMax = " << _cccs_xMin << ", " << _cccs_xMax;
    qDebug() << __FUNCTION__ << "_cccs_yMin ,_cccs_yMax = " << _cccs_yMin << ", " << _cccs_yMax;


    qDebug() << __FUNCTION__ << " resultSceneArea_qcsDim = " << resultSceneArea_qcsDim;


    resultSceneArea_pixelDim.setLeft(_converterPixToqsc.qsc_x_to_pixel(resultSceneArea_qcsDim.left()));
    resultSceneArea_pixelDim.setWidth(_widgetParentPixelWidth);
    resultSceneArea_pixelDim.setTop(_converterPixToqsc.qsc_y_to_pixel(resultSceneArea_qcsDim.top()));
    resultSceneArea_pixelDim.setHeight(_widgetParentPixelHeight);

    qDebug() << __FUNCTION__ << "resultSceneArea_pixelDim= " << resultSceneArea_pixelDim;

    return(true);
}

CustomGraphicsScene_profil::~CustomGraphicsScene_profil() {
    qDebug() << __FUNCTION__;
    removeAllAddedItems();
}

void CustomGraphicsScene_profil::add_areaRectAndLinesItemsOnly_landMarksItemsToScene() {

    qDebug() << __FUNCTION__;

    qreal cccs_xMin_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMin);
    qreal cccs_yMin_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin));

    qreal cccs_xMax_ToPixelSceneRect = _converterPixToqsc.qsc_x_to_pixel(_cccs_xMax);
    qreal cccs_yMax_ToPixelSceneRect = _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax));

    qDebug() << __FUNCTION__ << "cccs_xMin_ToPixelSceneRect = " << cccs_xMin_ToPixelSceneRect;
    qDebug() << __FUNCTION__ << "cccs_yMin_ToPixelSceneRect = " << cccs_yMin_ToPixelSceneRect;
    qDebug() << __FUNCTION__ << "cccs_xMax_ToPixelSceneRect = " << cccs_xMax_ToPixelSceneRect;
    qDebug() << __FUNCTION__ << "cccs_yMax_ToPixelSceneRect = " << cccs_yMax_ToPixelSceneRect;

    //landmarks

    //lines landmarks
    //------

    /*  y axis (left and right side)
       .............
      -|-         -|-
      -|-         -|-
      -|-         -|-
       .............
    */
    for(int idx=0; idx < _qvectYBigLandMarks.size(); idx++) {

       //left side
       QGraphicsLineItem *justAddedLineY = addLine(cccs_xMin_ToPixelSceneRect,
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),

                                                   cccs_xMin_ToPixelSceneRect +
                                                        _wPixelYAxisBigLandmark,
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),

                                                   _qpen_for_landmarks);

       _qvectGraphicsLineItem_yAxis_landmarks << justAddedLineY;


       //right side
       justAddedLineY = addLine(cccs_xMax_ToPixelSceneRect,
                                _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),

                                cccs_xMax_ToPixelSceneRect -

                                    _wPixelYAxisBigLandmark,
                                _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),

                                _qpen_for_landmarks);

       _qvectGraphicsLineItem_yAxis_landmarks << justAddedLineY;


       // |-...-|
       // yAxis_Biglandmarks_throughArea (horizontally)
       justAddedLineY = addLine(
                   cccs_xMin_ToPixelSceneRect +

                    _wPixelYAxisBigLandmark,
                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),
                   cccs_xMax_ToPixelSceneRect -

                   _wPixelYAxisBigLandmark,
                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYBigLandMarks[idx])),
                   _qpen_for_landmarks_throughArea);

       _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea << justAddedLineY;
    }


    //@LP area border and y axis above; code updated to use pixel size area for scene rect instead of adapted qcs


    /* x axis (top and bottom side)
       |-|-|-|-|-|
       .         .
       .         .
       |-|-|-|-|-|
    */

    for(int idx=0; idx < _qvectXBigLandMarks.size(); idx++) {
        //bottom side
        QGraphicsLineItem *justAddedLineX = addLine(_converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]),
                                                    _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin)),

                                                   _converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]),
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin)) - _hPixelXAxisBigLandmark,

                                                   _qpen_for_landmarks);
        _qvectGraphicsLineItem_xAxis_landmarks << justAddedLineX;


        //top side
        justAddedLineX = addLine(_converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]),
                                 _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax)),

                                 _converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]),
                                 _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax)) + _hPixelXAxisBigLandmark,

                                _qpen_for_landmarks);

        _qvectGraphicsLineItem_xAxis_landmarks << justAddedLineX;

        //xAxis_Biglandmarks_throughArea (vertically)
        justAddedLineX = addLine(
                    _converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]), _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin)) - _hPixelXAxisBigLandmark,
                    _converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]), _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax)) + _hPixelXAxisBigLandmark,
                    _qpen_for_landmarks_throughArea);

        _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea << justAddedLineX;
    }

    _qvectYSmallLandMarks = _yLM.getSmallLandMarksRef();//@#LP avoid copy constructor
    qDebug() << __FUNCTION__ << "_qvectYSmallLandMarks.size() = " << _qvectYSmallLandMarks.size();
    qDebug() << __FUNCTION__ << "_qvectYSmallLandMarks = " << _qvectYSmallLandMarks ;


    //
    //y axis small landmarks (no text)
    //
    for(int idx=0; idx < _qvectYSmallLandMarks.size(); idx++) {

       //left side
       QGraphicsLineItem *justAddedLineY = addLine(_converterPixToqsc.qsc_x_to_pixel(_cccs_xMin),
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYSmallLandMarks[idx])),

                                                   _converterPixToqsc.qsc_x_to_pixel(_cccs_xMin) + _wPixelYAxisSmallLandmark,
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYSmallLandMarks[idx])),

                                                   _qpen_for_landmarks);
       _qvectGraphicsLineItem_yAxis_landmarks << justAddedLineY;


       //right side
       justAddedLineY = addLine(_converterPixToqsc.qsc_x_to_pixel(_cccs_xMax),
                                _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYSmallLandMarks[idx])),

                                _converterPixToqsc.qsc_x_to_pixel(_cccs_xMax) - _wPixelYAxisSmallLandmark,
                                _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_qvectYSmallLandMarks[idx])),

                                _qpen_for_landmarks);
       _qvectGraphicsLineItem_yAxis_landmarks << justAddedLineY;

    }

    _qvectXSmallLandMarks = _xLM.getSmallLandMarksRef();
    qDebug() << __FUNCTION__ << "_qvectXSmallLandMarks.size() = " << _qvectXSmallLandMarks.size();
    qDebug() << __FUNCTION__ << "_qvectXSmallLandMarks = " << _qvectXSmallLandMarks ;


    //
    //x axis small landmarks (no text)
    //
    for(int idx=0; idx < _qvectXSmallLandMarks.size(); idx++) {

        //qDebug() << "_cccs_yMin + _converterPixToqcs.pixel_y_to_qsc(_hPixelXAxisSmallLandmark)) =" <<
        //    _cccs_yMin + _converterPixToqcs.pixel_y_to_qsc(_hPixelXAxisSmallLandmark);

        //qDebug() << "cccsY_to_qcsY(_cccs_yMin + _converterPixToqcs.pixel_y_to_qsc(_hPixelXAxisSmallLandmark)) =" <<
        //    cccsY_to_qcsY(_cccs_yMin + _converterPixToqcs.pixel_y_to_qsc(_hPixelXAxisSmallLandmark));


        //bottom side
        QGraphicsLineItem *justAddedLineX = addLine(_converterPixToqsc.qsc_x_to_pixel(_qvectXSmallLandMarks[idx]),
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin)),

                                                   _converterPixToqsc.qsc_x_to_pixel(_qvectXSmallLandMarks[idx]),
                                                   _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin)) - _hPixelXAxisSmallLandmark,

                                                   _qpen_for_landmarks);
        _qvectGraphicsLineItem_xAxis_landmarks << justAddedLineX;

        //top side
        justAddedLineX = addLine(_converterPixToqsc.qsc_x_to_pixel(_qvectXSmallLandMarks[idx]),
                               _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMax)),

                              _converterPixToqsc.qsc_x_to_pixel(_qvectXSmallLandMarks[idx]),
                              _converterPixToqsc.qsc_x_to_pixel(cccsY_to_qscY(_cccs_yMax)) + _hPixelXAxisSmallLandmark,

                              _qpen_for_landmarks);

        _qvectGraphicsLineItem_xAxis_landmarks << justAddedLineX;

    }

    QGraphicsLineItem *vLeft = addLine(cccs_xMin_ToPixelSceneRect, cccs_yMin_ToPixelSceneRect,
                                       cccs_xMin_ToPixelSceneRect, cccs_yMax_ToPixelSceneRect,
                                       _qpen_for_landmarks);
    _qvectGraphicsLineItem_rectCurveArea << vLeft;

    QGraphicsLineItem *vRight = addLine(cccs_xMax_ToPixelSceneRect, cccs_yMin_ToPixelSceneRect,
                                        cccs_xMax_ToPixelSceneRect, cccs_yMax_ToPixelSceneRect,
                                        _qpen_for_landmarks);
    _qvectGraphicsLineItem_rectCurveArea << vRight;

    QGraphicsLineItem *hTop = addLine(cccs_xMin_ToPixelSceneRect, cccs_yMin_ToPixelSceneRect,
                                      cccs_xMax_ToPixelSceneRect, cccs_yMin_ToPixelSceneRect,
                                       _qpen_for_landmarks);
    _qvectGraphicsLineItem_rectCurveArea << hTop;

    QGraphicsLineItem *hBottom = addLine(cccs_xMin_ToPixelSceneRect, cccs_yMax_ToPixelSceneRect,
                                         cccs_xMax_ToPixelSceneRect, cccs_yMax_ToPixelSceneRect,
                                       _qpen_for_landmarks);
    _qvectGraphicsLineItem_rectCurveArea << hBottom;

}

void CustomGraphicsScene_profil::setPosAndVisibility_textsItemsOnly_landMarksItemsOnScene() {

    //text landmark: setPos and SetVisible only
    qDebug() << __FUNCTION__ << "_qvectYBigLandMarks.count() = "           << _qvectYBigLandMarks.count();
    qDebug() << __FUNCTION__ << "_qvectYPixelSizeFBigLandMarks.count() = " << _qvectYPixelSizeFBigLandMarks.count();
    qDebug() << __FUNCTION__ << "_qvectGraphicsTextItem_yAxis.count() = "  << _qvectGraphicsTextItem_yAxis_landmarks.count();

    bool bDiffCaseError = false;
    if (_qvectYBigLandMarks.count() != _qvectYPixelSizeFBigLandMarks.count()) {
        bDiffCaseError = true;
        qDebug() << __FUNCTION__ << "diff case 1: " << _qvectYBigLandMarks.count() << " != " << _qvectYPixelSizeFBigLandMarks.count();
    }
    if (_qvectYBigLandMarks.count() != _qvectGraphicsTextItem_yAxis_landmarks.count()) {
        bDiffCaseError = true;
        qDebug() << __FUNCTION__ << "diff case 2: " << _qvectYBigLandMarks.count() << " != " << _qvectGraphicsTextItem_yAxis_landmarks.count();
    }

    if (bDiffCaseError) {
        return;
    }

    //y text landmarks
    for(int idx = 0; idx < _qvectYBigLandMarks.size(); idx++) {

        qreal qcs_x_top_left_corner =
                  _converterPixToqsc.qsc_x_to_pixel(_cccs_xMin)
                - ( _qvectYPixelSizeFBigLandMarks[idx].width() + _wPixelSpaceBetweenTextLandmarkAndYAxis);

        qreal qcs_y_top_left_corner =
                _converterPixToqsc.qsc_y_to_pixel((cccsY_to_qscY(_qvectYBigLandMarks[idx])))
                - (
                _qvectYPixelSizeFBigLandMarks[idx].height() / 2.0 );

        _qvectGraphicsTextItem_yAxis_landmarks[idx]->setPos(qcs_x_top_left_corner, qcs_y_top_left_corner);

        _qvectGraphicsTextItem_yAxis_landmarks[idx]->setVisible(true);

    }

    //x text landmarks
    qDebug() << "_qvectXBigLandMarks.count() = "           << _qvectXBigLandMarks.count();
    qDebug() << "_qvectXPixelSizeFBigLandMarks.count() = " <<  _qvectXPixelSizeFBigLandMarks.count();
    qDebug() << "_qvectGraphicsTextItem_xAxis.count() = "  << _qvectGraphicsTextItem_xAxis_landmarks.count();

    for(int idx = 0; idx < _qvectXBigLandMarks.size(); idx++) {

        qreal qcs_x_top_left_corner =
               _converterPixToqsc.qsc_x_to_pixel(_qvectXBigLandMarks[idx]) - (_qvectXPixelSizeFBigLandMarks[idx].width()/2.0);

        qreal qcs_y_top_left_corner =
               _converterPixToqsc.qsc_y_to_pixel(cccsY_to_qscY(_cccs_yMin))
               +(_pixel_gripSquareSideHeight + _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis);

        _qvectGraphicsTextItem_xAxis_landmarks[idx]->setPos(qcs_x_top_left_corner, qcs_y_top_left_corner);
        _qvectGraphicsTextItem_xAxis_landmarks[idx]->setVisible(true);

    }

}

void CustomGraphicsScene_profil::clearContent() {
    _eCA_component = e_CA_Invalid;
    removeAllAddedItems();
    invalidate();
}


void CustomGraphicsScene_profil::removeAllAddedItems() {
    qDebug() << __FUNCTION__;

    removeAdded_areaRectAndLinesItemsOnly_landMarksItemsFromScene();
    removeAdded_landmarksItem_texts();

    removeAdded_CGItemCurveAndEnvelop();
    removeAdded_CGItemDblSidesXAdjWithCompLines();
    removeAdded_CGItemVerticalX0AdjWithCentereredGrip();
}

void CustomGraphicsScene_profil::removeAdded_CGItemDblSidesXAdjWithCompLines() {

    if (_CGItemDblSidesXAdjWithCompLines_mainMeasure) {
        removeItem(_CGItemDblSidesXAdjWithCompLines_mainMeasure);
        _CGItemDblSidesXAdjWithCompLines_mainMeasure->deleteLater();
        _CGItemDblSidesXAdjWithCompLines_mainMeasure = nullptr;
    }
}

void CustomGraphicsScene_profil::removeAdded_CGItemVerticalX0AdjWithCentereredGrip() {

    if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure) {
        removeItem(_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure);

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->deleteLater();
        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure = nullptr;
    }

}


void CustomGraphicsScene_profil::removeAdded_CGItemCurveAndEnvelop() {

    if (_customGraphicsItemCurve) {
        removeItem(_customGraphicsItemCurve);
        delete _customGraphicsItemCurve;
        _customGraphicsItemCurve = nullptr;
    }
    if (_customGraphicsItemEnvelop) {
        removeItem(_customGraphicsItemEnvelop);
        delete _customGraphicsItemEnvelop;
        _customGraphicsItemEnvelop = nullptr;
    }
}

void CustomGraphicsScene_profil::removeAdded_areaRectAndLinesItemsOnly_landMarksItemsFromScene() {

    if (                _qvectGraphicsLineItem_rectCurveArea.size()) {
        for(auto& iter: _qvectGraphicsLineItem_rectCurveArea) {
            if (iter!= nullptr) { qDebug() << __FUNCTION__ << "remove _qvectGraphicsLineItem_rectCurveArea"; removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsLineItem_rectCurveArea);
                   _qvectGraphicsLineItem_rectCurveArea.clear();
    }

    if (                _qvectGraphicsLineItem_yAxis_landmarks.size()) {
        for(auto& iter: _qvectGraphicsLineItem_yAxis_landmarks) {
            if (iter!= nullptr) { qDebug() << __FUNCTION__ << "remove _qvectGraphicsLineItem_yAxis_landmarks"; removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsLineItem_yAxis_landmarks);
                   _qvectGraphicsLineItem_yAxis_landmarks.clear();
    }

    if (                _qvectGraphicsLineItem_xAxis_landmarks.size()) {
        for(auto& iter: _qvectGraphicsLineItem_xAxis_landmarks) {
            if (iter!= nullptr) { qDebug() << __FUNCTION__ << "remove _qvectGraphicsLineItem_xAxis_landmarks"; removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsLineItem_xAxis_landmarks);
                   _qvectGraphicsLineItem_xAxis_landmarks.clear();
    }

    if (                _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea.size()) {
        for(auto& iter: _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea) {
            if (iter!= nullptr) { qDebug() << __FUNCTION__ << "remove _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea"; removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea);
                   _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea.clear();
    }

    if (                _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea.size()) {
        for(auto& iter: _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea) {
            if (iter!= nullptr) { qDebug() << __FUNCTION__ << "remove _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea"; removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea);
                   _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea.clear();
    }
}


void CustomGraphicsScene_profil::removeAdded_landmarksItem_texts() {

    if (                _qvectGraphicsTextItem_yAxis_landmarks.size()) {
        for(auto& iter: _qvectGraphicsTextItem_yAxis_landmarks) {
            if (iter!= nullptr) { removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsTextItem_yAxis_landmarks);
                   _qvectGraphicsTextItem_yAxis_landmarks.clear();
    }

    if (                _qvectGraphicsTextItem_xAxis_landmarks.size()) {
        for(auto& iter: _qvectGraphicsTextItem_xAxis_landmarks) {
            if (iter!= nullptr) { removeItem(iter); }
        }
        qDeleteAll(_qvectGraphicsTextItem_xAxis_landmarks);
                   _qvectGraphicsTextItem_xAxis_landmarks.clear();
    }
}

bool CustomGraphicsScene_profil::initPixelSizeElements_andFontSize(
        int fontSizeForTextLandmark,
        int hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis,
        int wPixelSpaceBetweenTextLandmarkAndYAxis,
        int wPixelSpaceOutBorder,
        int hPixelSpaceOutBorder,
        int wPixelYAxisBigLandmark, int wPixelYAxisSmallLandmark,
        int hPixelXAxisBigLandmark, int hPixelXAxisSmallLandmark) {

    _fontSizeForTextLandmark = fontSizeForTextLandmark;

    _fnt = QFont(_qstrFontToUse, _fontSizeForTextLandmark);

    qDebug() << __FUNCTION__ << "_fontSizeForTextLandmark set to : " << fontSizeForTextLandmark;


    _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis = hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis;
    _wPixelSpaceBetweenTextLandmarkAndYAxis = wPixelSpaceBetweenTextLandmarkAndYAxis;
    _wPixelSpaceOutBorder = wPixelSpaceOutBorder;

    _hPixelSpaceOutBorder = hPixelSpaceOutBorder;
    _wPixelYAxisBigLandmark = wPixelYAxisBigLandmark;

    _wPixelYAxisSmallLandmark = wPixelYAxisSmallLandmark;
    _hPixelXAxisBigLandmark = hPixelXAxisBigLandmark;
    _hPixelXAxisSmallLandmark = hPixelXAxisSmallLandmark;

     return(true);

}

void CustomGraphicsScene_profil::showLinesForBigLandmarkThroughArea(
        bool bDrawYLinesForBigLandmarkThroughArea,
        bool bDrawXLinesForBigLandmarkThroughArea) {

    _bDrawYLinesForBigLandmarkThroughArea = bDrawYLinesForBigLandmarkThroughArea;
    _bDrawXLinesForBigLandmarkThroughArea = bDrawXLinesForBigLandmarkThroughArea;

}


bool CustomGraphicsScene_profil::compute_pointForLineRegDisplay_YForX0(
            double localToMethod_cccs_yMin,
            double localToMethod_cccs_yMax,
            const S_LinearRegressionParameters& linearRegrParam,
            double x0, //x0 to compute according y, this point needs to be visible on screen in any case
            S_QPointF_withValidityFlag& qPointF_withValidityFlag) {

    qDebug() << "___ linearRegrParam <=> y = "
             << linearRegrParam._mathComp_LinearRegressionParameters._GSLCompLinRegParameters._c1_a_slope << " * x + "
             << linearRegrParam._mathComp_LinearRegressionParameters._GSLCompLinRegParameters._c0_b_intercept;

    qPointF_withValidityFlag._bValid = false;


    double y = .0;
    bool bPoint1Computed = computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
        linearRegrParam._mathComp_LinearRegressionParameters._GSLCompLinRegParameters, x0, y);
    if (!bPoint1Computed) {
        qDebug() << __FUNCTION__  << " error: can not compute y for x0 = " << x0;
        return(false);
    }

    qDebug() << "___ Point1Computed : " << x0 << ", " << y;

    //If computed y for x0 is smaller than the current _cccs_yMin or bigger than the current _cccs_yMax,
    //this y will be the new min (or max) for the y range as we want that the point on (x0,y) visible on the graphic

    qDebug() << __FUNCTION__  << " localToMethod_cccs_yMin: " << localToMethod_cccs_yMin;
    qDebug() << __FUNCTION__  << " localToMethod_cccs_yMax: " << localToMethod_cccs_yMax;
    qDebug() << __FUNCTION__  << "          y: " << y;

    qPointF_withValidityFlag._pointF = {x0, y};
    qPointF_withValidityFlag._bValid = true;

    return(true);
}


bool CustomGraphicsScene_profil::compute_pointsCoupleForLineRegDisplay_left_right_sides(
    double localToMethod_cccs_yMin,
    double localToMethod_cccs_yMax,
    const S_LinearRegressionParameters linearRegrParam_left_right_side[2],
    double x0, //x0 to compute according ys, theses points needsto be visible on screen in any case
    const double xLimitForSecondPoint_clipping_left_right_side[2],
    S_CoupleOfQPointF_withValidityFlag coupleOfQPointF_withValidityFlag_left_right_side[2]) {

    coupleOfQPointF_withValidityFlag_left_right_side[e_LRsA_left ]._bValid = false;
    coupleOfQPointF_withValidityFlag_left_right_side[e_LRsA_right]._bValid = false;

    //each sides takes into account the computed ys for x0 on each side
    S_QPointF_withValidityFlag qPointF_yForX0_withValidityFlag_left_right_side[2] {
        {{.0, .0}, false},
        {{.0, .0}, false}
    };

    bool bReport_computedYForX0_left_right_sides[2] {false, false};

    for (int ie_LRsA = e_LRsA_left; ie_LRsA <= e_LRsA_right; ie_LRsA++) {
        qDebug() << __FUNCTION__  << "loop: ie_LRsA = " << ie_LRsA << " call now compute_pointForLineRegDisplay_YForX0()";
        bReport_computedYForX0_left_right_sides[ie_LRsA] = compute_pointForLineRegDisplay_YForX0(
                localToMethod_cccs_yMin, localToMethod_cccs_yMax,
                linearRegrParam_left_right_side[ie_LRsA], x0,
                qPointF_yForX0_withValidityFlag_left_right_side[ie_LRsA]);
    }

    if (   (!bReport_computedYForX0_left_right_sides[e_LRsA_left])
         ||(!bReport_computedYForX0_left_right_sides[e_LRsA_right])) {
        qDebug() << __FUNCTION__  << " some error(s): can not compute y for x0 = " << x0;
        return(false);
    }

    coupleOfQPointF_withValidityFlag_left_right_side[e_LRsA_left]._pointWithBiggestX =
     qPointF_yForX0_withValidityFlag_left_right_side[e_LRsA_left]._pointF;

    coupleOfQPointF_withValidityFlag_left_right_side[e_LRsA_right]._pointWithSmallestX =
     qPointF_yForX0_withValidityFlag_left_right_side[e_LRsA_right]._pointF;


    QVector<double> qvect_yMinMax_candidates;
    qvect_yMinMax_candidates << localToMethod_cccs_yMin;
    qvect_yMinMax_candidates << localToMethod_cccs_yMax;
    qvect_yMinMax_candidates << qPointF_yForX0_withValidityFlag_left_right_side[e_LRsA_left ]._pointF.y();
    qvect_yMinMax_candidates << qPointF_yForX0_withValidityFlag_left_right_side[e_LRsA_right]._pointF.y();

    double yGraphMin = *std::min_element(qvect_yMinMax_candidates.constBegin(), qvect_yMinMax_candidates.constEnd());
    double yGraphMax = *std::max_element(qvect_yMinMax_candidates.constBegin(), qvect_yMinMax_candidates.constEnd());

    qDebug() << __FUNCTION__  << " qvect_yMinMax_candidates = " << qvect_yMinMax_candidates;
    qDebug() << __FUNCTION__  << " yGraphMin = " << yGraphMin;
    qDebug() << __FUNCTION__  << " yGraphMax = " << yGraphMax;


    //here we have the yGraphMin and yGraphMax wich permits to have on screen:
    //- the curve (with minor if yMin and yMax passed to this method take it into account)
    //- the linear regression points crossing x0


    //now compute the other points to be able to draw the linear regression lines

    /*
      xLimitForSecondPoint_clipping can be located:

         |             |             |
         |             |...          |
         |             |   ...       |
         |            '|      ...    |
         |         ''' |         ..  |
         |      '''    |           ..|
         |   '''       |             |
         |'''          |             |
                       x0          xLimit
                                 ForSecondPoint
                                  _clipping


         |             |             |
         |             |...          |
         |             |   ...       |
         |            '|      ...    |
         |         ''' |         ..  |
         |      '''    |           ..|
         |   '''       |             |
         |'''          |             |
       xLimit          x0
    ForSecondPoint
      _clipping

    */

    //compute now point#2 y candidate #1 for point at xLimitForSecondPoint_clipping

    for (int ie_LRsA = e_LRsA_left; ie_LRsA <= e_LRsA_right; ie_LRsA++) {

        qDebug() << __FUNCTION__  << "computing Point2c1 for ie_LRsA : " << ie_LRsA;

        double y2c1 = .0; //candidate#1
        bool bPoint2c1Computed = computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
            linearRegrParam_left_right_side[ie_LRsA]._mathComp_LinearRegressionParameters._GSLCompLinRegParameters,
            xLimitForSecondPoint_clipping_left_right_side[ie_LRsA],
            y2c1);

        if (!bPoint2c1Computed) {
            qDebug() << __FUNCTION__  << "error: can not compute y2c1 for ie_LRsA = " << ie_LRsA;
            return(false);
        }
        qDebug() << __FUNCTION__  << "bPoint2c1Computed computed";

        if ((y2c1 >= yGraphMin)&&(y2c1 <= yGraphMax)) {
            qDebug() << __FUNCTION__  <<  "___ Point2c1Computed : Inside ]yGraphMin, yGraphMax[";

            if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] > x0) {                
                qDebug() << __FUNCTION__  <<  "if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] > x0) {";
                coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithBiggestX = { xLimitForSecondPoint_clipping_left_right_side[ie_LRsA], y2c1};

                qDebug() << __FUNCTION__  <<  "___ coupleOfQPointF_withValidityFlag_left_right_side[ " << ie_LRsA << "] = "
                                                << coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithBiggestX;

            } else {
                qDebug() << __FUNCTION__  <<  "if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] <= x0) {";
                coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithSmallestX = { xLimitForSecondPoint_clipping_left_right_side[ie_LRsA], y2c1};

                qDebug() << __FUNCTION__  <<  "___ coupleOfQPointF_withValidityFlag_left_right_side[ " << ie_LRsA << "] = "
                                                << coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithSmallestX;

            }
            coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._bValid = true;

        } else {
            qDebug() << __FUNCTION__  <<  "___ Point2c1Computed : Outside ]yGraphMin, yGraphMax[";

            //point#2 y candidate #1 is over the range  ]yGraphMin, yGraphMax[
            //we have to compute the point at the range limit

            double givenY2_candidates2 = .0;

            if (y2c1 > yGraphMax) {
                //search x for ymax;
                qDebug() << __FUNCTION__  <<  "search x for ymax";
                givenY2_candidates2 = yGraphMax;                
            } else {
                //search x for ymin;
                qDebug() << __FUNCTION__  <<  "search x for ymin";
                givenY2_candidates2 = yGraphMin;
            }

            //compute x values for point at givenY2_candidates2
            double x2_candidates2 = .0;
            bool bPoint2c2Computed = computeXForGivenY_usingComputedLinearRegressionParameters_slopeInterceptMethod(
                linearRegrParam_left_right_side[ie_LRsA]._mathComp_LinearRegressionParameters._GSLCompLinRegParameters, givenY2_candidates2, x2_candidates2);
            if (!bPoint2c2Computed) {
                qDebug() << __FUNCTION__  << "error: can not compute x2_candidates2 for ie_LRsA = " << ie_LRsA;
                return(false);
            }

            qDebug() << __FUNCTION__  << "bPoint2c2Computed computed";
            qDebug() << __FUNCTION__  << "___ Point2c2Computed (for given givenY2_candidates2): " << x2_candidates2 << ", " << givenY2_candidates2;

            if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] > x0) {
                qDebug() << __FUNCTION__  <<  "if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] > x0) {";
                coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithBiggestX  = { x2_candidates2, givenY2_candidates2};
            } else {
                qDebug() << __FUNCTION__  <<  "if (xLimitForSecondPoint_clipping_left_right_side[ie_LRsA] <= x0) {";
                coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._pointWithSmallestX = { x2_candidates2, givenY2_candidates2};
            }
            coupleOfQPointF_withValidityFlag_left_right_side[ie_LRsA]._bValid = true;
        }
    }

    return(true);
}

//in case of none value in vector, this method returns false
bool CustomGraphicsScene_profil::findMinMaxOfMajor_MMcomputedValuesWithValidityFlag(
    const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_aboutMeanMedian,
    double& min, double& max) {

    min =  999999999;
    max = -999999999;

    bool bSomeValid = false;
    for(const auto& iterValue: qvect_aboutMeanMedian) {
        if (iterValue._anonymMajorMinorWithValidityFlag._bValuesValid) {
            bSomeValid = true;
            if (      iterValue._anonymMajorMinorWithValidityFlag._major_centralValue < min) {
                min = iterValue._anonymMajorMinorWithValidityFlag._major_centralValue;
            }
            if (      iterValue._anonymMajorMinorWithValidityFlag._major_centralValue > max) {
                max = iterValue._anonymMajorMinorWithValidityFlag._major_centralValue;
            }
        }
    }
    return(bSomeValid);
}

bool CustomGraphicsScene_profil::findMinMaxOfMajorWithAddSubMinor_MMcomputedValuesWithValidityFlag(
    const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_aboutMeanMedian, double multiplierOfMinorForAddSubOnMajor,
    double& min, double& max) {

    min =  999999999;
    max = -999999999;

    bool bSomeValid = false;
    for(const auto& iterValue: qvect_aboutMeanMedian) {
        if (iterValue._anonymMajorMinorWithValidityFlag._bValuesValid) {
            bSomeValid = true;

            double major_withAddedMultiplierOfMinor =
                    iterValue._anonymMajorMinorWithValidityFlag._major_centralValue
                    + multiplierOfMinorForAddSubOnMajor *  iterValue._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;

            double major_withSubtractedMultiplierOfMinor =
                    iterValue._anonymMajorMinorWithValidityFlag._major_centralValue
                    - multiplierOfMinorForAddSubOnMajor *  iterValue._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue;

            if (major_withAddedMultiplierOfMinor < min) { min = major_withAddedMultiplierOfMinor; }
            if (major_withAddedMultiplierOfMinor > max) { max = major_withAddedMultiplierOfMinor; }

            if (major_withSubtractedMultiplierOfMinor < min) { min = major_withSubtractedMultiplierOfMinor; }
            if (major_withSubtractedMultiplierOfMinor > max) { max = major_withSubtractedMultiplierOfMinor; }
        }
    }
    return(bSomeValid);
}


void CustomGraphicsScene_profil::setProfilAdjustMode(e_ProfilAdjustMode eProfilAdjustMode) {

    if (eProfilAdjustMode == e_PAM_notSet) {
        return;
    }

    _eProfilAdjustMode = eProfilAdjustMode;

    bool bSetProfilCenter_movable_andGripSquareVisible = (_eProfilAdjustMode == e_PAM_X0Center);

    qDebug() << __FUNCTION__ << "bSetProfilCenter_movable_andGripSquareVisible = " << bSetProfilCenter_movable_andGripSquareVisible;

    if (_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure) {

        _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setState_movableWithAccordGripsSquareVisibility(bSetProfilCenter_movable_andGripSquareVisible);

        //we hide the _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure to avoid take event, to let another VerticalLineMouseOver take it
        //and we show a 'fake' line about the main mesur at the same location, for display
        switch (_eProfilAdjustMode) {
            case e_PAM_leftRight:
                _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setZValue(2);
                break;

            case e_PAM_X0Center:
                _CGItemVerticalAdjusterWithCentereredGrip_mainMeasure->setZValue(6);
                break;

            default:;
        }
    }

    if (_CGItemDblSidesXAdjWithCompLines_mainMeasure) {
        _CGItemDblSidesXAdjWithCompLines_mainMeasure->setState_movableAdjustersWithAccordGripsSquareVisibility(!bSetProfilCenter_movable_andGripSquareVisible);
    }
}

QSizeF pixelWidthHeight_ofStringForFontWithPixelFontSize(
        const QString& strText,
        const QString& strFontName,
        int pixelFontSize,
        QRectF& bboxTightF,

        QRectF& bboxGraphicsTextItem) {

    QFont fnt(strFontName, pixelFontSize);

    qDebug() << __FUNCTION__ << "strFontName   = " << strFontName;
    qDebug() << __FUNCTION__ << "pixelFontSize = " << pixelFontSize;
    qDebug() << __FUNCTION__ << "---";


    qDebug() << __FUNCTION__ << "fnt.exactMatch() = " << fnt.exactMatch();
    QFontInfo qfntInfo(fnt);
    qDebug() << __FUNCTION__ << "qfntInfo.family() = " << qfntInfo.family();
    QFontDatabase qfntDB;
    qDebug() << __FUNCTION__ << "qfntDB.families() = " << qfntDB.families();


    QFontMetricsF fntmetrF(fnt);


    qDebug() << __FUNCTION__ << "strText = " << strText;

    QString strText_removedHtmlTags = strText;
    strText_removedHtmlTags.remove("<sup>").remove("</sup>"); //exponent

    qDebug() << __FUNCTION__ << "strText_removedHtmlTags = " << strText_removedHtmlTags;

    bboxTightF = fntmetrF.tightBoundingRect(strText_removedHtmlTags);
    qDebug() << __FUNCTION__ << "bboxTight of strText_removedHtmlTags = " << bboxTightF;

    qDebug() << __FUNCTION__ << "fntmetrF.horizontalAdvance(strText[0]) = " <<fntmetrF.horizontalAdvance(strText[0]);

    qDebug() << __FUNCTION__ << "fntmetr.ascent() = " << fntmetrF.ascent();

    QGraphicsTextItem *testTextSize = new QGraphicsTextItem("");
    testTextSize->setFont(fnt);
    testTextSize->document()->setDocumentMargin(0);
    testTextSize->document()->setIndentWidth(0);

    testTextSize->setHtml(strText);

    if (!testTextSize->document()->blockCount()) {
        delete testTextSize;
        qDebug() << __FUNCTION__ << "error!blockCount() is zero";
        return(QSizeF {1.0,1.0});
    }

    bboxGraphicsTextItem = testTextSize->boundingRect();
    qDebug() << __FUNCTION__ << "bboxGraphicsTextItem = " << bboxGraphicsTextItem;

    delete testTextSize;

    return(bboxTightF.size());
}

void CustomGraphicsScene_profil::slot_locationInAdjustement(bool bState) { //just redirect
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil) bState = " << bState;
    emit signal_locationInAdjustement(bState);
}

void CustomGraphicsScene_profil::slot_selectionStateChanged_aboutXAdjusterOfDoubleSidesXAdjusters(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene_profil)";
}

