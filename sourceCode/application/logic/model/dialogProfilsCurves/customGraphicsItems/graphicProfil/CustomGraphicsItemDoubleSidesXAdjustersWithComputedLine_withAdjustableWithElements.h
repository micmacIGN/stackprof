#ifndef CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines_HPP
#define CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines_HPP

#include <QGraphicsItem>
#include <QDebug>

class CustomGraphicsItemXAdjustersWithComputedLine;

#include "../../../core/ComputationCore_structures.h"

#include "../../ConverterPixelToQsc.h" //includes S_ParametersForPixelResizeInvariance

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"

class CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine: //public QObject, public QGraphicsItem {
                                                               public QGraphicsObject {

    Q_OBJECT

public:

    CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine(
            S_CoupleOfQPointF_withValidityFlag pointCoupleForLeftSide_forComputedLine,
            S_CoupleOfQPointF_withValidityFlag pointCoupleForRightSide_forComputedLine,

            S_XRangeForLinearRegressionComputation xAdjustersPositionsForLeftSide,
            S_XRangeForLinearRegressionComputation xAdjustersPositionsForRightSide,

            double xLimitMinLeftSide,
            double xLimitMaxRightSide,
            double xCenterAxis, //limits used as: xLimitMaxLeftSide andxLimitMinRightSide

            double yMin, double yMax,

            QSizeF qsizef_adjusterPixelGripSize,
            QGraphicsItem *parent=nullptr);

    ~CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine() override;

    void setGraphicsParameters(const S_GraphicsParameters_LinearRegression& sGP_LinearRegression);

    void setAndUpdateParameters_pointCoupleForLeftRightSide(
            const S_CoupleOfQPointF_withValidityFlag& pointCoupleForLeftSide,
            const S_CoupleOfQPointF_withValidityFlag& pointCoupleForRightSide);

    void setAndUpdateParameters_xAdjustersPositionsForLeftRightSide(const S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation[2]);

    //@#LP check if the other structure without validity flag is better instead of S_CoupleOfQPointF_withValidityFlag
    void set_pointCoupleForLeftSide(const S_CoupleOfQPointF_withValidityFlag& pointCoupleForLeftSide_forComputedLine);
    void set_pointCoupleForRightSide(const S_CoupleOfQPointF_withValidityFlag& pointCoupleForRightSide_forComputedLine);


    void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);
    //void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc, const QRectF& qrectfBBox_pixelDim);


    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //used in case of y range alignement between different stacked profil with different range

    void setState_movableAdjustersWithAccordGripsSquareVisibility(bool bState);

    int type() const override;

public slots:
    void slot_selectionStateChanged(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState);

    //void slot_xPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA);

    void slot_locationInAdjustement(bool bState);

signals:
    void signal_xPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA);

    //@#LP trying to pass the adjuster over the X0Profil
    void signal_selectionStateChanged(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState);

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

private:

    void createAndSet_CGItemXAdjustersWithComputedLine();

    S_CoupleOfQPointF_withValidityFlag _pointCoupleForLeftSide_forComputedLine;
    S_CoupleOfQPointF_withValidityFlag _pointCoupleForRightSide_forComputedLine;

    S_XRangeForLinearRegressionComputation _xAdjustersPositionsForLeftSide;
    S_XRangeForLinearRegressionComputation _xAdjustersPositionsForRightSide;

    double _xLimitMinLeftSide;
    double _xLimitMaxRightSide;
    double _xCenterAxis; //limits used as: xLimitMaxLeftSide andxLimitMinRightSide

    double _yMin, _yMax;

    QSizeF _qsizef_adjusterPixelGripSize;

    ConverterPixelToQsc _converterPixToqsc;

    CustomGraphicsItemXAdjustersWithComputedLine *_CGItemXAdjustersWithComputedLineLeftSide;
    CustomGraphicsItemXAdjustersWithComputedLine *_CGItemXAdjustersWithComputedLineRightSide;

    QRectF _qrectfbbox_qcsDim;
    QRectF _qrectfbbox_pixelDim;

};


#endif // CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines_HPP
