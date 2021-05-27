#ifndef CustomGraphicsItemXAdjustersWithComputedLine_HPP
#define CustomGraphicsItemXAdjustersWithComputedLine_HPP

#include <QGraphicsItem>
#include <QDebug>

#include <QPen>
#include <QBrush>

class CustomGraphicsItemVerticalAdjuster;

class CustomGraphicsLineItem;

#include "../../ConverterPixelToQsc.h"

#include "../../../core/ComputationCore_structures.h"

#include "../../../core/leftRight_sideAccess.hpp"

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

class CustomGraphicsItemXAdjustersWithComputedLine: //public QObject, public QGraphicsItem {
                                                    public QGraphicsObject {

    Q_OBJECT

public:

     CustomGraphicsItemXAdjustersWithComputedLine(
            e_LRsideAccess e_LRsA_locationAboutX0CentralAxis,
            S_CoupleOfQPointF_withValidityFlag coupleOfQPointFOnComputedLine,
            S_XRangeForLinearRegressionComputation xAdjustersPosition,
            double xMin, double xMax,
            double yMin, double yMax,
            QSizeF qsizef_adjusterPixelGripSize,
            QGraphicsItem *parent=nullptr);

     void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);

     void setAndUpdateParameters_coupleOfQPointFOnComputedLine(
             const S_CoupleOfQPointF_withValidityFlag& coupleOfQPointFOnComputedLine);

     void setAndUpdateParameters_xAdjustersPositions(S_XRangeForLinearRegressionComputation xRangeForLinearRegressionComputation);

    ~CustomGraphicsItemXAdjustersWithComputedLine() override;

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override;

    void setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster,
                               const S_Color_Thickness_Antialiasing& sCTA_computedLine);

    void setState_movableAdjustersWithAccordGripsSquareVisibility(bool bState);

    void slot_locationInAdjustement(bool bState);

signals:
    void signal_selectionStateChanged(uint i_eLRsA_locationRelativeToX0CentralAxis, bool bNewState);

    //send the xpos of the vertical adjuster i_eLRsA (of the eLRsA_locationAboutX0CentralAxis verticalxadjusters couple)
    void signal_xPosition_changed(uint i_eLRsA_locationRelativeToX0CentralAxis, double xpos, int i_eLRsA_adjuster);

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

public slots:
    void slot_selectionStateChanged(bool bNewState);

    //received here the new x position from the left or the right adjuster
    void slot_xPosition_changed(double xpos, int i_eLRsA);

private:
    void setStyleOfQPen_for_computedLine();
    void setGraphicsParameters_for_computedLine(const S_Color_Thickness_Antialiasing& sCTA_computedLine);

private:

    e_LRsideAccess _e_LRsA_locationAboutX0CentralAxis;

    S_CoupleOfQPointF_withValidityFlag _coupleOfQPointFOnComputedLine;

    S_XRangeForLinearRegressionComputation _xAdjustersPositions;

    double _xMin, _xMax;
    double _yMin, _yMax;

    ConverterPixelToQsc _converterPixToqsc;

    QSizeF _qsizef_adjusterPixelGripSize;

    QRectF _qrectfbbox_qcsDim;
    QRectF _qrectfbbox_pixelDim;

    CustomGraphicsItemVerticalAdjuster *_CGItemVerticalItemAdjusterLeft;
    CustomGraphicsItemVerticalAdjuster *_CGItemVerticalItemAdjusterRight;

    CustomGraphicsLineItem *_addedLinePtr;

};


#endif // CustomGraphicsItemXAdjustersWithComputedLine_HPP
