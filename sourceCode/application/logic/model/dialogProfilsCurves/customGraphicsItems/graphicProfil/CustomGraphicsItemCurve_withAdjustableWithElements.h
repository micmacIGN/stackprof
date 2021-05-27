#ifndef CustomGraphicsItemCurve_HPP
#define CustomGraphicsItemCurve_HPP

#include <QGraphicsItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include "../../../core/ComputationCore_structures.h"

#include "../../ConverterPixelToQsc.h"

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"

union U_MeanMedian_computedValuesWithValidityFlag;

class CustomGraphicsItemCurve:  public QGraphicsPathItem {

public:
    CustomGraphicsItemCurve(QGraphicsItem *parent=nullptr);

    //values at index #0 for qvect_yMajor and qvect_yMinorAroundYMajor matches with xmin
    //values at index #size()-1 for qvect_yMajor and qvect_yMinorAroundYMajor matches with xmax
    //xstep indicates how far are to successives values in x
    //@#LP values in qcs or ccs ?
    void setCurve(
        const QVector<U_MeanMedian_computedValuesWithValidityFlag>& computedValuesWithValidityFlag,
        qreal xMin, qreal xMax,
        qreal yMin, qreal yMax,
        qreal xStep);

    void setGraphicsParameters(const S_GraphicsParameters_ProfilCurveAndEnvelopArea &sGraphicsParameters_ProfilCurveAndEnvelopArea);

    void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);

    ~CustomGraphicsItemCurve() override ;

    int type() const override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

private:
    void computeBoundingBox();

    QPen _qpen_forMajor;
    QBrush _qbrush_forMajor;

    double _dThickness;
    bool _bUseAntialiasing;

    QVector<QVector<QPointF>> _qvectqvectPointF_majorByFragments_qcsDim; //contains the y values about the math/logic dimension (but with minus sign applyed)
    QVector<QVector<QPointF>> _qvectqvectPointF_majorByFragments_pixelDim; //contains the y values about the pixel rect scene

    QPainterPath _qppCurveMajor;

    QRectF _qrectfBBox_qcsDim;
    QRectF _qrectfBBox_pixelDim;

    ConverterPixelToQsc _converterPixToqsc;
};

#endif // CustomGraphicsItemCurve_HPP
