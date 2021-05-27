#include <QGraphicsItem>

#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "CustomGraphicsItemType_v2.h"

#include "CustomGraphicsItemCurve_withAdjustableWithElements.h"

#include <QGraphicsPathItem>

#include "../../../core/ComputationCore_structures.h"

CustomGraphicsItemCurve::CustomGraphicsItemCurve(
    QGraphicsItem *parent):
    QGraphicsPathItem(parent),

    _qpen_forMajor(QColor(250,0,0)),
    _qbrush_forMajor(Qt::NoBrush),
    _dThickness{2.0},
    _bUseAntialiasing{false}   
{

    _qpen_forMajor.setCosmetic(false);
    _qpen_forMajor.setWidthF(_dThickness);
    _qpen_forMajor.setJoinStyle(Qt::RoundJoin);
    _qpen_forMajor.setCapStyle(Qt::FlatCap);

    _converterPixToqsc.setWidthRatioFrom(1.0,1.0);
    _converterPixToqsc.setHeightRatioFrom(1.0,1.0);

    _qrectfBBox_qcsDim = {.0,.0,.0, .0};
    computeBoundingBox();  
}

void CustomGraphicsItemCurve::computeBoundingBox() {

    QRectF qrectfBBox_pixelDim_qsc_to_pixel;

    qrectfBBox_pixelDim_qsc_to_pixel.setTopLeft(
        {_converterPixToqsc.qsc_y_to_pixel(_qrectfBBox_qcsDim.top()),
         _converterPixToqsc.qsc_x_to_pixel(_qrectfBBox_qcsDim.left())});

    qrectfBBox_pixelDim_qsc_to_pixel.setBottomRight(
        {_converterPixToqsc.qsc_y_to_pixel(_qrectfBBox_qcsDim.bottom()),
         _converterPixToqsc.qsc_x_to_pixel(_qrectfBBox_qcsDim.right())});

    double dThicknessDiv2 = _dThickness/2.0;

    _qrectfBBox_pixelDim.setTopLeft    ({qrectfBBox_pixelDim_qsc_to_pixel.top()   -dThicknessDiv2, qrectfBBox_pixelDim_qsc_to_pixel.left() -dThicknessDiv2});
    _qrectfBBox_pixelDim.setBottomRight({qrectfBBox_pixelDim_qsc_to_pixel.bottom()+dThicknessDiv2, qrectfBBox_pixelDim_qsc_to_pixel.right()+dThicknessDiv2});
}

void CustomGraphicsItemCurve::setCurve(
    const QVector<U_MeanMedian_computedValuesWithValidityFlag>& computedValuesWithValidityFlag,
    qreal xMin, qreal xMax,
    qreal yMin, qreal yMax,
    qreal xStep) {

    _qvectqvectPointF_majorByFragments_qcsDim.clear();
    _qvectqvectPointF_majorByFragments_pixelDim.clear();

    _qrectfBBox_qcsDim.setTopLeft({xMin, yMin});
    _qrectfBBox_qcsDim.setBottomRight({xMax, yMax});

    qDebug() << __FUNCTION__ << "_qrectfBBox.topLeft()     = " << _qrectfBBox_qcsDim.topLeft();
    qDebug() << __FUNCTION__ << "_qrectfBBox.bottomRight() = " << _qrectfBBox_qcsDim.bottomRight();

    if (computedValuesWithValidityFlag.isEmpty()) {
        qDebug() << __FUNCTION__ << "computedValuesWithValidityFlag.isEmpty() is true";
        return;
    }

    double x = .0;

    QVector<int> qvectIndexOfFirtPointForEachFragment; //used to feed _qvectqvectPointF_minorByFragments but using a separated loop, and without needs to iter computedValuesWithValidityFlag
    int currentIndexInComputedValuesWithValidityFlag = 0;

    //major
    x = xMin;
    QVector<QPointF> qvectPointF_EmptyFragment;

    int idxVectForPointInsertion = -1;
    bool bPreviousPointIsValid = false;
    for (auto computedValueWVF: computedValuesWithValidityFlag) {
        bool bCurrentPointIsValid = computedValueWVF._anonymMajorMinorWithValidityFlag._bValuesValid;
        if (bCurrentPointIsValid) {
            if (!bPreviousPointIsValid) {
                _qvectqvectPointF_majorByFragments_qcsDim.push_back(qvectPointF_EmptyFragment);
                idxVectForPointInsertion++;

                qvectIndexOfFirtPointForEachFragment.push_back(currentIndexInComputedValuesWithValidityFlag);
            }
            _qvectqvectPointF_majorByFragments_qcsDim[idxVectForPointInsertion].push_back({x, computedValueWVF._anonymMajorMinorWithValidityFlag._major_centralValue});
        }
        bPreviousPointIsValid = bCurrentPointIsValid;
        x += xStep;

        currentIndexInComputedValuesWithValidityFlag++;
    }
    //qDebug() << __FUNCTION__ << "_qvectqvectPointF_majorByFragments = " << _qvectqvectPointF_majorByFragments;


    int currentIndexOfFragment = 0;
    for (const auto& vect: _qvectqvectPointF_majorByFragments_qcsDim) {

        int qvectUpAndDownShapeBorder_size = vect.size()*2;
        QVector<QPointF> qvectPointF_upAndDownShapeBorders(qvectUpAndDownShapeBorder_size, {.0,.0});

        int currentIndexOfPointInFragment = 0;
        for (const auto& ptOfect: vect) {
            QPointF qpfInUpShapeBorder = { ptOfect.x(),
                                           ptOfect.y() + computedValuesWithValidityFlag[
                                           qvectIndexOfFirtPointForEachFragment[currentIndexOfFragment] + currentIndexOfPointInFragment ]
                                           ._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue };

            QPointF qpfInDownShapeBorder = { ptOfect.x(),
                                             ptOfect.y() - computedValuesWithValidityFlag[
                                             qvectIndexOfFirtPointForEachFragment[currentIndexOfFragment] + currentIndexOfPointInFragment ]
                                             ._anonymMajorMinorWithValidityFlag._minor_aroundCentralValue };

            int indexToSetUpPoint = currentIndexOfPointInFragment;
            int indexToSetDownPoint = (qvectUpAndDownShapeBorder_size - 1) - currentIndexOfPointInFragment;

            qvectPointF_upAndDownShapeBorders[indexToSetUpPoint  ] = qpfInUpShapeBorder;
            qvectPointF_upAndDownShapeBorders[indexToSetDownPoint] = qpfInDownShapeBorder;

            currentIndexOfPointInFragment++;
        }

        currentIndexOfFragment++;
    }
}

void CustomGraphicsItemCurve::setGraphicsParameters(const S_GraphicsParameters_ProfilCurveAndEnvelopArea &sGraphicsParameters_ProfilCurveAndEnvelopArea) {

    _dThickness = sGraphicsParameters_ProfilCurveAndEnvelopArea._sCTA_profilCurve._dThickness;

    _qpen_forMajor.setColor(sGraphicsParameters_ProfilCurveAndEnvelopArea._sCTA_profilCurve._qColor);
    _qpen_forMajor.setCosmetic(false);
    _qpen_forMajor.setWidthF(_dThickness);
    _qpen_forMajor.setJoinStyle(Qt::RoundJoin);
    _qpen_forMajor.setCapStyle(Qt::FlatCap);
    _qbrush_forMajor.setStyle(Qt::NoBrush);

    _bUseAntialiasing = sGraphicsParameters_ProfilCurveAndEnvelopArea._sCTA_profilCurve._bAntialiasing;

    qDebug() << __FUNCTION__ << "_bUseAntialiasing  set now to : " << _bUseAntialiasing;

    prepareGeometryChange();
    computeBoundingBox();
}


void CustomGraphicsItemCurve::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc/*, const QRectF& qrectfBBox_pixelDim*/) {

    qDebug() << __FUNCTION__;

    prepareGeometryChange();
    _converterPixToqsc = converterPixToqsc;
    computeBoundingBox();//_qrectfBBox_pixelDim = qrectfBBox_pixelDim;

    _qvectqvectPointF_majorByFragments_pixelDim = _qvectqvectPointF_majorByFragments_qcsDim;

    /*for (auto& vect: _qvectqvectPointF_majorByFragments_pixelDim) {
        qDebug() << __FUNCTION__ << "---";
        for (auto& ptOfect: vect) {
            qDebug() << __FUNCTION__ << "before:" << ptOfect;
        }
    }*/

    for (auto& vect: _qvectqvectPointF_majorByFragments_pixelDim) {
        for (auto& ptOfect: vect) {
            ptOfect.setX(converterPixToqsc.qsc_x_to_pixel(ptOfect.x()));
            ptOfect.setY(converterPixToqsc.qsc_y_to_pixel(ptOfect.y()));
        }
    }

    /*for (auto& vect: _qvectqvectPointF_majorByFragments_pixelDim) {
        qDebug() << __FUNCTION__ << "---";
        for (auto& ptOfect: vect) {
            qDebug() << __FUNCTION__ << "after:" << ptOfect;
        }
    }*/

}

void CustomGraphicsItemCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemCurve)";

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, _bUseAntialiasing);

    //major curve

    painter->setPen(_qpen_forMajor);
    painter->setBrush(_qbrush_forMajor);

    for (auto vect: _qvectqvectPointF_majorByFragments_pixelDim) {
        painter->drawPolyline(static_cast<const QPointF*>(vect.constData()), vect.count());
        //qDebug() << __FUNCTION__ << "(CustomGraphicsItemCurve) vect = " << vect;
    }

    //@#LP debug:
    /*QBrush brush_debug(Qt::SolidPattern);
    QPen pen_debug(Qt::NoPen);
    brush_debug.setColor({128,0,64,128});
    painter->setBrush(brush_debug);
    painter->setPen(pen_debug);
    painter->drawRect(_qrectfBBox_pixelDim);*/

    painter->restore();
}

QRectF CustomGraphicsItemCurve::boundingRect() const {
      return(_qrectfBBox_pixelDim);
}

CustomGraphicsItemCurve::~CustomGraphicsItemCurve() {
    qDebug() << __FUNCTION__;
}

int CustomGraphicsItemCurve::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsItemCurve);
}
