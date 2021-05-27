#ifndef MATH_TOOLBOX_GEOMETRY_H
#define MATH_TOOLBOX_GEOMETRY_H

#include <QPointF>
#include <QSizeF>

#include "ImathVec.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "../model/core/S_Segment.h" //@#LP move this file ?

enum e_ResultDetailsOfRelocatedPointInSegment {
    e_RDORPIS_notSet,

    e_RDORPIS_pointABCnotAligned, //should never happen

    e_RDORPIS_notRelocated_AisCloser,
    e_RDORPIS_notRelocated_BisCloser,

    e_RDORPIS_relocatedToA,
    e_RDORPIS_relocatedToB,
    e_RDORPIS_relocatedInsideSegment,

};

e_ResultDetailsOfRelocatedPointInSegment relocatePointInSegment(const QPointF& qpf, S_Segment segmentOwner,
                                                                QPointF& qpfRelocatedPoint);

double convert_qcsVectorDirectionToAzimuthtFromTopVertical(QPointF unitVectorDirection);

QPointF normalizeQPointFUnitVectorIfNeed(const QPointF& qpfUnitVectorDirection);

bool checkThatThreePointsAreAligned(const QPointF& qpf_A, const QPointF& qpf_B, const QPointF& qpf_C);

/*
Vec2<float> reorientYToTop(Vec2<float> vecInClockWise);
Vec2<float> reorientYToDown(Vec2<float> vecInClockWise);
*/
Vec2<double> reorientYToTop(Vec2<double> vecInClockWise);
Vec2<double> reorientYToDown(Vec2<double> vecInClockWise);

Vec2<double> transformPointFromCoordinateSystemToAnother_rotationOfAxes(
        Vec2<double> pointToTransform,
        //Vec2<float> srcAxisUnitVector,
        Vec2<double> dstAxisUnitVector);

bool routeBoundingBoxIntersectWithImage_pixelImageUnit(const QPointF &qpfTopLeft,
                                                       const QSizeF &qSizeFImage,
                                                       const QVector<QPointF>& qvectpointf_ofRoute);

#endif
