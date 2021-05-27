#ifndef CUSTOMGRAPHICSSCENE_PROFIL_H
#define CUSTOMGRAPHICSSCENE_PROFIL_H

#include <QGraphicsScene>
#include <QFontMetricsF>

class CustomGraphicsVerticalAdjuster;
class CustomGraphicsItemCurve;
class CustomGraphicsItemEnvelop;

#include "ConverterPixelToQsc.h"

class CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine;
class CustomGraphicsItemVerticalAdjusterWithCentereredGrip;

struct S_XAdjustersPosAndComputedLineParameters;

class CustomLineNoEvent;

#include "landmarks.h"

#include <QPointF>

#include "../StackedProfilInEdition.h"

#include "../appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"

#include "enum_profiladjustmode.h"

QSizeF pixelWidthHeight_ofStringForFontWithPixelFontSize(
    const QString& strText,
    const QString& strFontName,
    int pixelFontSize,
    QRectF& bbox,
    QRectF& bboxGraphicsTextItem);

struct S_ForResizeComputation_noTextLandMarksChange {
    //set from here, this will not change from outside like it can happens with  the fiels below
    double _qsc_widthGraphicArea;
    double _qsc_heightGraphicArea;

    double _hSumPixelAtTop;
    double _hSumPixelAtBottom;

    //from outside:
    QRectF _sceneRectF; //includes extension left and right for x alignement;  includes also extension top, bottom for x+y alignement
    double _wPixelSpaceAtLeft;
    double _wPixelSpaceAtRight;

    //for y range for x+y alignement:
    double _wPixelSpaceAtTop;
    double _wPixelSpaceAtBottom;

};

class CustomGraphicsScene_profil : public QGraphicsScene {

    Q_OBJECT

public:
    CustomGraphicsScene_profil(QObject *parent = nullptr);
    ~CustomGraphicsScene_profil();

    void setParentPixelSizeForInit(int widgetParentPixelWidth, int widgetParentPixelHeight);
    void updateItems();

    //will set mode at true:
    void setOnModeXGraphAreaAlignement(qreal throughAllScene_left, qreal throughAllScene_right,
                                      double wPixelSpaceAtLeft, double wPixelSpaceAtRight);
    void setOffAnyAlignementMode();

    bool feedScene(
        e_ComponentsAccess eCA_component,
        const S_StackedProfilInEdition& stackedProfilInEdition,
        qreal xGraphMin,
        qreal xGraphMax,
        e_MeanMedianIndexAccess meanMedianIndexAccess,
        const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode);

    //due to update miss, make 3 feedscene method depending on what alignement we want
    //The previous way to manage was to just adjust item in updateItems, but some paint event on some items were not called
    //Hence, the new way is to refeed the scene with specific, instead of just recompute the scenerect and adjust items location (text landmark for example)
    //Note that for example, with the first way, vertical adjuster needs to be set on the new height and the curves had a paint event not called

    bool feedScene_withExtendedYRange(
        e_ComponentsAccess eCA_component,
        const S_StackedProfilInEdition& stackedProfilInEdition,

        qreal throughAllScene_yMin, qreal throughAllScene_yMax,

        qreal xGraphMin, qreal xGraphMax,
        e_MeanMedianIndexAccess meanMedianIndexAccess,
        const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode);

    bool feedScene_withExtendedXRange(
        e_ComponentsAccess eCA_component,
        const S_StackedProfilInEdition& stackedProfilInEdition,

        qreal throughAllScene_left_pixelDim, qreal throughAllScene_right_pixelDim,
        qreal wPixelSpaceAtLeft, qreal wPixelSpaceAtRight,

        qreal xGraphMin, qreal xGraphMax,
        e_MeanMedianIndexAccess meanMedianIndexAccess,
        const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode);

    bool feedScene_withExtendedXRange_sceneSpaceValuesInsteadOfPixel(
        e_ComponentsAccess eCA_component,
        const S_StackedProfilInEdition& stackedProfilInEdition,

        qreal throughAllScene_left_pixelDim, qreal throughAllScene_right_pixelDim, //scene rect smallest left through all layer, scene rect biggest right through all layer,
        qreal throughAllScene_wSceneSpaceAtLeft_pixelDim, qreal throughAllScene_wSceneSpaceAtRight_pixelDim,//scene rect biggest wSceneSpaceAtLeft and wSceneSpaceAtRight through all layer

        qreal xGraphMin, qreal xGraphMax,
        e_MeanMedianIndexAccess meanMedianIndexAccess,
        const S_GraphicsParameters_Profil& sGraphicParameters,
        e_ProfilAdjustMode eProfilAdjustMode);

    //to have many layers profils aligned (ie same ranges for profil but also for qrectf area as landmarks alter the sceneRectF)

    bool getYRangeAsSoleLayer(double& cccs_yMin, double &cccs_yMax);

    //to have many layers profils aligned (ie same ranges for profil but also for qrectf area as landmarks alter the sceneRectF)

    bool computeSceneAreaSize(
            QRectF& resultSceneArea_qcsDim,
            QRectF& resultSceneArea_pixelDim,

            S_ForResizeComputation_noTextLandMarksChange& forResizeComputation,

            double& wSpaceAtLeft_qcsDim, double& wSpaceAtRight_qcsDim,
            double& wSpaceAtLeft_pixelDim, double& wSpaceAtRight_pixelDim);

    bool computeSceneAreaSize_usingValuesFromOutsideForXAlignement(
        double throughAllScene_left_pixelDim, double throughAllScene_right_pixelDim,
        double throughAllScene_wSceneSpaceAtLeft_pixelDim, double throughAllScene_wSceneSpaceAtRight_pixelDim,

        QRectF& resultSceneArea_qcsDim,
        QRectF& resultSceneArea_pixelDim,
        S_ForResizeComputation_noTextLandMarksChange& forResizeComputation,
        double& wSpaceAtLeft_pixelDim, double& wSpaceAtRight_pixelDim);

    bool get_sceneRectF_areaOfLayerWhenAlone_pixelDim(QRectF& sceneRectF);
    bool get_sceneRectF_areaOfLayerWhenAlone_pixelDim(QRectF& sceneRectF, double& wSpacePixelNeedsAtLeft, double& wSpacePixelNeedsAtRight);

    void showLinesForBigLandmarkThroughArea(
            bool bDrawYLinesForBigLandmarkThroughArea,
            bool bDrawXLinesForBigLandmarkThroughArea);

    void clearContent();

    void setProfilAdjustMode(e_ProfilAdjustMode eProfilAdjustMode);

private:

    bool initPixelSizeElements_andFontSize(
            int fontSizeForTextLandmark,
            int hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis, //int hPixelSpaceBetweenTextLandmarkAndXAxis,
            int wPixelSpaceBetweenTextLandmarkAndYAxis,
            int wPixelSpaceOutBorder,
            int hPixelSpaceOutBorder,
            int wPixelYAxisBigLandmark, int wPixelYAxisSmallLandmark,
            int hPixelXAxisBigLandmark, int hPixelXAxisSmallLandmark);

    void setXYRange(double cccs_xMin, double cccs_xMax,
                    double cccs_yMin, double cccs_yMax);

    double cccsY_to_qscY(double y);
    double qscY_to_cccsY(double y);

    void removeAllAddedItems();

    void add_areaRectAndLinesItemsOnly_landMarksItemsToScene();
    void setPosAndVisibility_textsItemsOnly_landMarksItemsOnScene();

    bool findMinMaxOfMajor_MMcomputedValuesWithValidityFlag(
            const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_aboutMeanMedian,
            double& min, double& max);

    bool findMinMaxOfMajorWithAddSubMinor_MMcomputedValuesWithValidityFlag(
        const QVector<U_MeanMedian_computedValuesWithValidityFlag>& qvect_aboutMeanMedian, double multiplierOfMinorForAddSubOnMajor,
        double& min, double& max);

    bool compute_pointsCoupleForLineRegDisplay_left_right_sides(
        double localToMethod_cccs_yMin,
        double localToMethod_cccs_yMax,
        const S_LinearRegressionParameters linearRegrParam_left_right_side[2],
        double x0, //x0 to compute according ys, theses points needsto be visible on screen in any case
        const double xLimitForSecondPoint_clipping_left_right_side[2],
        S_CoupleOfQPointF_withValidityFlag coupleOfQPointF_withValidityFlag_left_right_side[2]);

    bool compute_pointForLineRegDisplay_YForX0(
        double localToMethod_cccs_yMin,
        double localToMethod_cccs_yMax,
        const S_LinearRegressionParameters& linearRegrParam,
        double x0, //x0 to compute according y, this point needs to be visible on screen in any case
        S_QPointF_withValidityFlag& qPointF_withValidityFlag);  

private:

    void removeAdded_areaRectAndLinesItemsOnly_landMarksItemsFromScene();
    void removeAdded_landmarksItem_texts();

    void removeAdded_CGItemCurveAndEnvelop();
    void removeAdded_CGItemDblSidesXAdjWithCompLines();
    void removeAdded_CGItemVerticalX0AdjWithCentereredGrip();

    int splitOnExponentRemovingHtmlTags(const QString strIn,
                                         QString& str_partWithoutExponent,
                                         QString& str_partWithExponentOnly);
    void setHtmlAndStateForBoundingRectComputation(QGraphicsTextItem *&qGTextItem, const QString& strHtml) ;

    bool computeLandmarks_and_createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem();

    /*bool computeLandmarks_and_createTextLandmarkGraphicsItemsComputingRequieredSpace_adustingYLandmarksLocationUsingReceivedSpaceFromOutside(
        qreal throughAllScene_left, qreal throughAllScene_right, //scene rect smallest left through all layer, scene rect biggest right through all layer,
        qreal throughAllScene_wSceneSpaceAtLeft, qreal throughAllScene_wSceneSpaceAtRight);*/

    QSizeF createTextLandmarkGraphicsItemsComputingRequieredSpaceForThem(
        QVector<QString> qvectStrAxisLandMarks, const QFontMetricsF& fntmetrF, const QRectF& qrectFBTight_smallExampleCharExponent,
        QVector<QGraphicsTextItem*>& qvectGraphicsTextItem_Axis, QVector<QSizeF>& qvectAxisPixelSizeFLandMarks);

public slots:

    void slot_xAdjusterPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA);

    void slot_centerX0AdjusterPosition_changed(double xpos);

    void slot_selectionStateChanged_aboutXAdjusterOfDoubleSidesXAdjusters(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState);

    void slot_locationInAdjustement(bool bState);

signals:
    void signal_xAdjusterPosition_changed(uint ieComponents,
                                          //bool bOnMainMeasure,
                                          //int secondaryMeasureIdx,
                                          uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA);
    void signal_fitInView();


    void signal_centerX0AdjusterPosition_changed(uint ieComponents,
                                          //bool bOnMainMeasure,
                                          //int secondaryMeasureIdx,
                                          double xpos);

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

private:

    S_ForResizeComputation_noTextLandMarksChange _forResizeComputation_sameWidth;
    S_ForResizeComputation_noTextLandMarksChange _forResizeComputation_sameHeightAndWidth;

    bool forAlignSameWidth_updateSceneRectAndRatioConverterFromFixedSpaceAroundGraphicAreaAndFixedCurveArea(const S_ForResizeComputation_noTextLandMarksChange& forResizeComputation);
    bool forAlignSameHeightAndWidth_updateSceneRectAndRatioConverterFromFixedSpaceAroundGraphicAreaAndFixedCurveArea(const S_ForResizeComputation_noTextLandMarksChange& forResizeComputation);

private:

    e_ComponentsAccess _eCA_component;

    //range for the curve area:
    double _cccs_xMin, _cccs_xMax;
    double _cccs_yMin, _cccs_yMax;

    double _x0; //axis for the measure (central or senconday)

    //pixel size values of some elements and spaces:
    int _fontSizeForTextLandmark;

    int _hPixelSpaceBetweenGripSquareAndTextLandmarkAndXAxis;
    int _wPixelSpaceBetweenTextLandmarkAndYAxis;
    int _wPixelSpaceOutBorder;
    int _hPixelSpaceOutBorder;
    int _wPixelYAxisBigLandmark;
    int _wPixelYAxisSmallLandmark;
    int _hPixelXAxisBigLandmark;
    int _hPixelXAxisSmallLandmark;

    bool _bDrawYLinesForBigLandmarkThroughArea;
    bool _bDrawXLinesForBigLandmarkThroughArea;

    int _widgetParentPixelWidth;
    int _widgetParentPixelHeight;
    bool _bWidgetParentPixelWHValid;

    QFont _fnt;

    //the curve area is extented to includes spaces and text around:

    QRectF _sceneRectF; //in use (in pixel)

    QRectF _sceneRectF_areaOfLayerWhenAlone_qcsDim;
    double _wSpaceAtLeft_whenAlone_qcsDim;
    double _wSpaceAtRight_whenAlone_qcsDim;

    QRectF _sceneRectF_areaOfLayerWhenAlone_pixelDim;
    double _wSpaceAtLeft_whenAlone_pixelDim;
    double _wSpaceAtRight_whenAlone_pixelDim;

    double _cccs_yMin_whenAlone, _cccs_yMax_whenAlone;

    enum e_AlignementSceneMode {
        e_ASM_none_graphicAlone,
        e_ASM_alignSameWidth,
        e_ASM_alignSameRangeWidthAndHeight
    };

    e_AlignementSceneMode _eAlignementSceneMode;
    double _throughAllScene_left_pixelDim;
    double _throughAllScene_right_pixelDim;
    double _throughAllScene_wSceneSpaceAtLeft_pixelDim;
    double _throughAllScene_wSceneSpaceAtRight_pixelDim;

    //for axis, landmarks and area limit

    QPen     _qpen_for_landmarks;
    QBrush _qbrush_for_landmarks;

    QPen     _qpen_for_landmarks_throughArea;
    QBrush _qbrush_for_landmarks_throughArea;

    //borders of the curve rect area

    QVector<QGraphicsLineItem *> _qvectGraphicsLineItem_rectCurveArea;

    //the curve:

    CustomGraphicsItemCurve *_customGraphicsItemCurve;
    CustomGraphicsItemEnvelop *_customGraphicsItemEnvelop;

    //

    CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine *_CGItemDblSidesXAdjWithCompLines_mainMeasure;
    CustomGraphicsItemVerticalAdjusterWithCentereredGrip *_CGItemVerticalAdjusterWithCentereredGrip_mainMeasure; //for X0

    ConverterPixelToQsc _converterPixToqsc;

    //values, sizes and texts of landmarks:
    LandmarkInRange _xLM;
    LandmarkInRange _yLM;

    QSizeF _maxSizeFTextXLandMark;
    QSizeF _maxSizeFTextYLandMark;

    QVector<QSizeF> _qvectXPixelSizeFBigLandMarks;
    QVector<QString> _qvectStrXBigLandMarks;

    QVector<double> _qvectXBigLandMarks;
    QVector<double> _qvectXSmallLandMarks;

    QVector<QSizeF> _qvectYPixelSizeFBigLandMarks;
    QVector<QString> _qvectStrYBigLandMarks;

    QVector<double> _qvectYBigLandMarks;
    QVector<double> _qvectYSmallLandMarks;

    //graphics item for landmarks:

    //use html for exponent display
    QVector<QGraphicsTextItem*> _qvectGraphicsTextItem_xAxis_landmarks;
    QVector<QGraphicsTextItem*> _qvectGraphicsTextItem_yAxis_landmarks;

    QVector<QGraphicsLineItem *> _qvectGraphicsLineItem_xAxis_landmarks;
    QVector<QGraphicsLineItem *> _qvectGraphicsLineItem_yAxis_landmarks;

    QVector<QGraphicsLineItem *> _qvectGraphicsLineItem_xAxis_bigLandmarks_throughArea;
    QVector<QGraphicsLineItem *> _qvectGraphicsLineItem_yAxis_bigLandmarks_throughArea;

    const double _pixel_gripSquareSideWidth = 7.0;
    const double _pixel_gripSquareSideHeight = 7.0;

    //the content of _computedValuesWithValidityFlag will be adjust to match with the used qt scene area
    //which is different in y axis than the value from the computation
    QVector<U_MeanMedian_computedValuesWithValidityFlag> _computedValuesWithValidityFlag;

    //what we need to draw the lines equations:
    S_CoupleOfQPointF_withValidityFlag _pointCoupleForLineRegDisplay_Left_Right[2];
    //what we need to set the x range adjusters locations:
    S_XRangeForLinearRegressionComputation _xRangeForLinearRegressionComputation[2];

    QString _qstrFontToUse;
    QString _qstrTextDebugFontDisplay;

   // bool _bJustFeed;

    S_StackedProfilWithMeasurements *_stackedProfilWithMeasurementsPtr;

    //try to provides to model for correct resize operation letting space for font with alignement
    //computed when alone:
    double _wSumPixelAtLeft_whenAlone;
    double _wSumPixelAtRight_whenAlone;    

    e_ProfilAdjustMode _eProfilAdjustMode;
};

#endif // CUSTOMGRAPHICSSCENE_PROFIL_H
