#include <QDebug>

#include "S_Segment.h"

#include "ImathVec.h"
#include "ImathVecAlgo.h"
#include "ImathFun.h"
#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

S_Segment::S_Segment():
    _ptA(.0,.0),
    _ptB(.0,.0),
    _bValid(false) {
}

double S_Segment::length() {
    if (!_bValid) {
        return(.0);
    }
    Vec2<double> vec2dblAB {
        _ptB.x() - _ptA.x(),
        _ptB.y() - _ptA.y()
    };
    //qDebug() << __FUNCTION__ << "(S_Segment) vec2dblAB.length() = " << vec2dblAB.length();
    return(vec2dblAB.length());
}
