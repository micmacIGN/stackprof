#ifndef S_SEGMENT_H
#define S_SEGMENT_H

#include <QPointF>

struct S_Segment {
    QPointF _ptA;
    QPointF _ptB;
    bool _bValid;
    S_Segment();
    double length();
};

#endif // S_SEGMENT_H
