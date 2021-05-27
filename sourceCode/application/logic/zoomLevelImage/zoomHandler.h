#ifndef ZOOMHANDLER_H
#define ZOOMHANDLER_H

#include <QVector>
#include <QHash>

class ZoomHandler {

public:
    ZoomHandler();

    struct S_zoomParameters {
        double _globalZoomFactor;
        double _scale_forViewSide;
        int _zoomLevelImage_forSceneSide;
    };

    enum e_Zoom_Direction { e_ZoomDir_Out,
                            e_ZoomDir_In};

    enum e_zoomLevelImageTransition {
        e_zLIT_none,
        e_zLIT_zoomIn,
        e_zLIT_zoomOut
    };

    void clear();

    void initZoomRange_linearAdd();

    void initZoomRange_nonLinear_increasePreviousValueByXXXPercent(int maximumZoomLevelImageAvailable);
    void buildViewSideFullScalesForEachZLIWhenNoZLISwitch();
    qreal getViewScaleForExtendedScale(int ZLI);

    //org:
    //bool zoomIn();
    //bool zoomOut();
    bool zoomIn(bool& bMinInLevelReached);
    bool zoomOut(bool& bMaxOutLevelReached);

    void setToZoomLevelImage1_scale1();

    double getCurrent_globalZoomFactor();
    double getCurrent_scale_forViewSide();
    int    getCurrent_zoomLevelImage_forSceneSide();
    e_zoomLevelImageTransition getCurrent_zoomLevelImageTransition();

    int getIdxMax();
    bool setToZoomFromIdx(int idx);

    int findClosestZoomIndexOfZoomValueForGlobalZoomFactor(double globalZoomFactor);

private:

    //about current zoom
    int _idxZoom;
    int _idxForZoomLevelImage1Scale1;
    e_zoomLevelImageTransition _zoomLevelImageTransition;

    QVector<S_zoomParameters> _vectZoomParameters;

    //return true if limited
    bool limitToMinIfTooLow(bool& bMinInLevelReached);
    bool limitToMaxIfTooHigh(bool& bMaxOutLevelReached);

    bool update_zoomLevelImageTransition_withIdxZoom(int ZLI_beforeIdxZoomUpdate);

protected: //for test
    bool valueIsPowerOfTwo(double v);
    bool valueIsCloseToPowerOfTwo(float v, float deltaAround, float& adjustedV);

private:
    QHash<int, QVector<double>> _qhash_ZLI_extendedScalesForView;

};


#endif // ZOOMHANDLER_H
