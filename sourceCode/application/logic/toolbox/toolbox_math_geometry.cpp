#include <QPointF>
#include <QSizeF>
#include <QRectF>

#include <QDebug>

#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "toolbox_math.h"

#include "toolbox_math_geometry.h"

#include <iostream>
#include <iomanip>

QPointF normalizeQPointFUnitVectorIfNeed(const QPointF& qpfUnitVectorDirection) {

    Vec2<double> vec2dblUnitVectorDirection {
        qpfUnitVectorDirection.x(),
        qpfUnitVectorDirection.y()
    };
    //using normalization change precision of the unit vector which can lead to small difference in futur computation.
    //which is not wanted as exported result from a new created project and the same result exported from this reloaded project can lead to
    //small difference (in decimal part of values)
    //Hence, we return the normalized vector only if it was not normalized (the use case is when the project was edited by a human, not by this software which write normalized vector)
    double vec2dblUnitVectorDirection_length = vec2dblUnitVectorDirection.length();
    //std::cout << __FUNCTION__ <<std::setprecision(14) << "vec2dblUnitVectorDirection_length = " << vec2dblUnitVectorDirection_length;

    //input qpfUnitVectorDirection considered as already normalized
    if (doubleValuesAreClose(vec2dblUnitVectorDirection_length, static_cast<double>(1.0), 0.001)) {
        return(qpfUnitVectorDirection);
    }
    vec2dblUnitVectorDirection.normalize();
    return(QPointF {vec2dblUnitVectorDirection.x,
                    vec2dblUnitVectorDirection.y});
}

bool checkThatThreePointsAreAligned(const QPointF& qpf_A, const QPointF& qpf_B, const QPointF& qpf_C) {

    Vec2<double> vect2dbl_AB { qpf_B.x() - qpf_A.x(),
                               qpf_B.y() - qpf_A.y() };

    Vec2<double> vec2dbl_AC { qpf_C.x() - qpf_A.x(),
                              qpf_C.y() - qpf_A.y()};

    double dbl_crossProduct_AB_AC = vect2dbl_AB.cross(vec2dbl_AC);

    /*qDebug() << __FUNCTION__
             << "vect2dbl_AB_axisDestVector: "
             <<  vect2dbl_AB_axisDestVector.x << ", "
             <<  vect2dbl_AB_axisDestVector.y;

    qDebug() << __FUNCTION__
             << "vec2dbl_C_onLine: "
             <<  vec2dbl_C_onLine.x << ", "
             <<  vec2dbl_C_onLine.y;
    */
    qDebug() << __FUNCTION__;

    show_QPointF_7decimals("(alignment test) qpf_A: ", qpf_A);
    show_QPointF_7decimals("(alignment test) qpf_B: ", qpf_B);
    show_QPointF_7decimals("(alignment test) qpf_C: ", qpf_C);

    qDebug() << __FUNCTION__ << "(alignment test) dbl_crossProduct_AB_AC: " << QString::number(dbl_crossProduct_AB_AC, 'f', 14);



    if (dbl_crossProduct_AB_AC > 0.001) { //not aligned enough
        return(false);
    }
    return(true);
}


e_ResultDetailsOfRelocatedPointInSegment relocatePointInSegment(const QPointF& qpf, S_Segment segmentOwner,QPointF& qpfRelocatedPoint) {

    //qDebug() << __FUNCTION__ << "------";
    //qDebug() << __FUNCTION__ << "qpf: " << qpf;
    //qDebug() << __FUNCTION__ << "segmentOwner: " << segmentOwner._ptA << " => " << segmentOwner._ptB;
    //show_segment_7decimals(segmentOwner);

    //project the point (perpendicular projection) on the line defined by the segmentOwner
    //the resulting point will be on the line

    Vec2<double> vect2dbl_AB_axisDestVector { segmentOwner._ptB.x() - segmentOwner._ptA.x(),
                                              segmentOwner._ptB.y() - segmentOwner._ptA.y() };

    Vec2<double> vect2dbl_centerAxisForProjection { segmentOwner._ptA.x(), segmentOwner._ptA.y()};

    Vec2<double> vect2dbl_C_toProject { qpf.x(), qpf.y()};
    Vec2<double> vect2dbl_C_toProject_inCenterAxisCoordinates = vect2dbl_C_toProject - vect2dbl_centerAxisForProjection;

    Vec2<double> vect2dbl_AB_axisDestVectorNormalized = vect2dbl_AB_axisDestVector.normalized();
    Vec2<double> vect2dbl_AB_axisDestVectorNormalized_reorientedYToTop = reorientYToTop(vect2dbl_AB_axisDestVectorNormalized);

    Vec2<double> vect2dbl_C_toProject_shiftedToHaveAxisCenterAsA_reorientedYToTop = reorientYToTop(vect2dbl_C_toProject_inCenterAxisCoordinates);

    Vec2<double> vec2dbl_C_projectedToAxisDestVectorNormalized_AxisCenterAsA_withYToTop =
            transformPointFromCoordinateSystemToAnother_rotationOfAxes(
                vect2dbl_C_toProject_shiftedToHaveAxisCenterAsA_reorientedYToTop,
                vect2dbl_AB_axisDestVectorNormalized_reorientedYToTop);

    Vec2<double> vec2dbl_C_projectedToAxisDestVectorNormalized_AxisCenterAsA_reOrientedYToDown =
            reorientYToDown(vec2dbl_C_projectedToAxisDestVectorNormalized_AxisCenterAsA_withYToTop);

    Vec2<double> vec2dbl_C_onLine { segmentOwner._ptA.x() + vec2dbl_C_projectedToAxisDestVectorNormalized_AxisCenterAsA_reOrientedYToDown.x * vect2dbl_AB_axisDestVectorNormalized.x,
                                    segmentOwner._ptA.y() + vec2dbl_C_projectedToAxisDestVectorNormalized_AxisCenterAsA_reOrientedYToDown.x * vect2dbl_AB_axisDestVectorNormalized.y };


    //using this way: https://lucidar.me/en/mathematics/check-if-a-point-belongs-on-a-line-segment/


    // dev check A, B, C aligned

    Vec2<double> vec2dbl_AC { vec2dbl_C_onLine.x - segmentOwner._ptA.x(),
                              vec2dbl_C_onLine.y - segmentOwner._ptA.y()};

    double dbl_crossProduct_AB_AC = vect2dbl_AB_axisDestVector.cross(vec2dbl_AC);

    /*qDebug() << __FUNCTION__
             << "vect2dbl_AB_axisDestVector: "
             <<  vect2dbl_AB_axisDestVector.x << ", "
             <<  vect2dbl_AB_axisDestVector.y;

    qDebug() << __FUNCTION__
             << "vec2dbl_C_onLine: "
             <<  vec2dbl_C_onLine.x << ", "
             <<  vec2dbl_C_onLine.y;
    */
    //qDebug() << __FUNCTION__ << "(alignment test) dbl_crossProduct_AB_AC: " << QString::number(dbl_crossProduct_AB_AC, 'f', 14);



    if (dbl_crossProduct_AB_AC > 0.001) { //not aligned enough
        //very bad internal dev error
        qDebug() << __FUNCTION__ << "internal error : dbl_crossProduct_AB_AC = " << QString::number(dbl_crossProduct_AB_AC, 'f', 14) << " => A, B, C not aligned (!)";
        //qDebug() << __FUNCTION__ << "internal error : ( vec2dbl_C_onLine:" << vec2dbl_C_onLine.x << " , " << vec2dbl_C_onLine.y << " )";
        show_Vec2_dbl_7decimals("( vec2dbl_C_onLine:", vec2dbl_C_onLine);

        //should never happen
        return(e_RDORPIS_pointABCnotAligned);
    }

    //check that the projected point is located between the segment extremities

    double dbl_dotProduct_ABdotAC = vect2dbl_AB_axisDestVector.dot(vec2dbl_AC);

    qDebug() << __FUNCTION__ << "(inside segment test) dbl_dotProduct_ABdotAC: " << QString::number(dbl_dotProduct_ABdotAC, 'f', 14);

    if (dbl_dotProduct_ABdotAC < .0) {

        if (dbl_dotProduct_ABdotAC < -0.001) {
            //the point is not between A and B of the segment and not close enough to A
            qDebug() << __FUNCTION__ << "the point is not between A and B of the segment (case #1)";
            qDebug() << __FUNCTION__ << "not in ]A,B[ (case #1)";
            qDebug() << __FUNCTION__ << "return now e_RDORPIS_notRelocated_AisCloser";
            return(e_RDORPIS_notRelocated_AisCloser);
        }
        //we force to be A
        qDebug() << __FUNCTION__ << "force to A";
        qpfRelocatedPoint = segmentOwner._ptA;
        return(e_RDORPIS_relocatedToA);
    }

    double dbl_dotProduct_ABdotAB = vect2dbl_AB_axisDestVector.dot(vect2dbl_AB_axisDestVector);
    //qDebug() << __FUNCTION__ << "dbl_dotProduct_ABdotAB: " << QString::number(dbl_dotProduct_ABdotAB, 'f', 14);

    if (dbl_dotProduct_ABdotAC > dbl_dotProduct_ABdotAB) {

        if (dbl_dotProduct_ABdotAC > dbl_dotProduct_ABdotAB + 0.001) {
            //the point is not between A and B of the segment and not close enough to B
            qDebug() << __FUNCTION__ << "the point is not between A and B of the segment (case #2)";
            qDebug() << __FUNCTION__ << "not in ]A,B[ (case #2)";
            qDebug() << __FUNCTION__ << "return now e_RDORPIS_notRelocated_BisCloser";
            return(e_RDORPIS_notRelocated_BisCloser);
        }
        //we force to be B
        qDebug() << __FUNCTION__ << "force to B";
        qpfRelocatedPoint = segmentOwner._ptB;
        return(e_RDORPIS_relocatedToB);
    }


    qpfRelocatedPoint.setX(vec2dbl_C_onLine.x);
    qpfRelocatedPoint.setY(vec2dbl_C_onLine.y);

    qDebug() << __FUNCTION__ << "relocated point computed and valid (not forced to A or B)";
    //show delta in each direction between the input point and the according relocated point:
    /*show_Vec2_dbl_7decimals("delta x, y: ",
    { qAbs(qpfRelocatedPoint.x() - qpf.x()),
      qAbs(qpfRelocatedPoint.y() - qpf.y()) });*/
    return(e_RDORPIS_relocatedInsideSegment);
}

Vec2<double> reorientYToTop(Vec2<double> vecInClockWise) {
    //qDebug() << __FUNCTION__;
    return(Vec2<double>{vecInClockWise.x, -vecInClockWise.y});
}

Vec2<double> reorientYToDown(Vec2<double> vecInClockWise) {
    //qDebug() << __FUNCTION__;
    return(Vec2<double>{vecInClockWise.x, -vecInClockWise.y});
}

//@LP: unit tested:
//dstAxisUnitVector has to be normalized !
Vec2<double> transformPointFromCoordinateSystemToAnother_rotationOfAxes(
        Vec2<double> pointToTransform,
        //Vec2<float> srcAxisUnitVector,
        Vec2<double> dstAxisUnitVector) {

    Vec2<double> transformedPoint {.0, .0};

    transformedPoint.x = (pointToTransform.x * dstAxisUnitVector.x) + (pointToTransform.y * dstAxisUnitVector.y);
    transformedPoint.y = (pointToTransform.y * dstAxisUnitVector.x) - (pointToTransform.x * dstAxisUnitVector.y);

    //qDebug() << __FUNCTION__ << " pointToTransform  = " << pointToTransform.x << ", " << pointToTransform.y;
    //qDebug() << __FUNCTION__ << " dstAxisUnitVector = " << dstAxisUnitVector.x     << ", " << dstAxisUnitVector.y;
    //qDebug() << __FUNCTION__ << " transformedPoint = " << transformedPoint.x << ", " << transformedPoint.y;

    //@#LP fprintf(stdout, ...)
    //fprintf(stdout, "<< %s >> f f : transformedPoint= %f %f\n", __FUNCTION__, transformedPoint.x, transformedPoint.y);
    //fflush(stdout);

    return(transformedPoint);
}


double convert_qcsVectorDirectionToAzimuthtFromTopVertical(QPointF unitVectorDirection) {

    /* azimuth from top vertical to box vector direction

       top
     vertical
    (0 degree)
        |
        |
        |.            .
        |                 .
        |                    .
        |                      .
        |                       .
        |                        .
        |
        + box center point        .
         \
          \                       .
           \     box
            \   vector           .
             \ direction        .
              \               .
               \           .
                \ |     .
                 \| .     azimuth
                  \----
                   \
                    \
    */


    //qcs to geometry x,y (y increasing to top)
    double cos_alpha = unitVectorDirection.x();
    double sin_alpha = -unitVectorDirection.y();

    double radAngleFromCos = acos(cos_alpha);
    double radAngleFromSin = asin(sin_alpha);
    qDebug() << "#1 radAngleFromCos: " << radAngleFromCos;
    qDebug() << "#1 radAngleFromSin: " << radAngleFromSin;

    if (sin_alpha < .0 ) { //adjust if sinus < 0.0
        radAngleFromCos = -radAngleFromCos;
    }

    qDebug() << "#2 radAngleFromCos: " << radAngleFromCos;

    //inverse direction to be in clockwise direction
    radAngleFromCos = -radAngleFromCos;

    qDebug() << "#2 radAngleFromCos: " << radAngleFromCos;


    //convert to have zero at top vertical instead of right horizontal
    radAngleFromCos = (M_PI/2.0) + radAngleFromCos;

    qDebug() << "#3 radAngleFromCos: " << radAngleFromCos;

    double degAngleFromCos = (radAngleFromCos*180.0)/M_PI;

    qDebug() << "#4 degAngleFromCos: " << degAngleFromCos;

    //very small value close to zero become zero
    if (qAbs(degAngleFromCos) < 0.0000001) {
        degAngleFromCos = .0;
    }

    return(degAngleFromCos);
}


bool routeBoundingBoxIntersectWithImage_pixelImageUnit(const QPointF &qpfTopLeft,
                                                       const QSizeF &qSizeFImage,
                                                       const QVector<QPointF>& qvectpointf_ofRoute) {

    QRectF qrect_qcs_Image;
    qrect_qcs_Image.setTopLeft(qpfTopLeft);
    qrect_qcs_Image.setSize(qSizeFImage);

    double qcs_x_dst_min = 999999;
    double qcs_y_dst_min = 999999;

    double qcs_x_dst_max = -999999;
    double qcs_y_dst_max = -999999;

    for (const auto& qcs_dst: qvectpointf_ofRoute) {

        qDebug() << __FUNCTION__;
        qDebug() << __FUNCTION__  << "loop with qcs_dst : " << qcs_dst;

        if (qcs_dst.x() < qcs_x_dst_min) { qcs_x_dst_min = qcs_dst.x(); }
        if (qcs_dst.x() > qcs_x_dst_max) { qcs_x_dst_max = qcs_dst.x(); }
        if (qcs_dst.y() < qcs_y_dst_min) { qcs_y_dst_min = qcs_dst.y(); }
        if (qcs_dst.y() > qcs_y_dst_max) { qcs_y_dst_max = qcs_dst.y(); }

        qDebug() << __FUNCTION__  << "loop qcs_x_dst_min : " << qcs_x_dst_min;
        qDebug() << __FUNCTION__  << "loop qcs_y_dst_min : " << qcs_y_dst_min;
        qDebug() << __FUNCTION__  << "loop qcs_x_dst_max : " << qcs_x_dst_max;
        qDebug() << __FUNCTION__  << "loop qcs_y_dst_max : " << qcs_y_dst_max;
    }

    qDebug() << __FUNCTION__  << "final after loop qcs_x_dst_min : " << qcs_x_dst_min;
    qDebug() << __FUNCTION__  << "final after loop qcs_y_dst_min : " << qcs_y_dst_min;
    qDebug() << __FUNCTION__  << "final after loop qcs_x_dst_max : " << qcs_x_dst_max;
    qDebug() << __FUNCTION__  << "final after loop qcs_y_dst_max : " << qcs_y_dst_max;

    //intersection test with width or height at .0 leads to no intersection.
    //Hence, increase enough for intersection test if requiered
    if (doubleValuesAreClose(qcs_x_dst_min, qcs_x_dst_max, 1.1)) {//vertical route
        qcs_x_dst_min -= 1.0;
        qcs_x_dst_max += 1.0;
    }
    if (doubleValuesAreClose(qcs_y_dst_min, qcs_y_dst_max, 1.1)) { //horizontal route
        qcs_y_dst_min -= 1.0;
        qcs_y_dst_max += 1.0;
    }

    QRectF qrect_routeBBox;
    qrect_routeBBox.setTopLeft    ({qcs_x_dst_min, qcs_y_dst_min});
    qrect_routeBBox.setBottomRight({qcs_x_dst_max, qcs_y_dst_max});

    qDebug() << __FUNCTION__  << "qrect_qcs_Image = " << qrect_qcs_Image;
    qDebug() << __FUNCTION__  << "qrect_routeBBox = " << qrect_routeBBox;

    return(qrect_qcs_Image.intersects(qrect_routeBBox));
}
