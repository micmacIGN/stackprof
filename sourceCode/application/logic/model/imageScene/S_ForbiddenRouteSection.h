#ifndef S_FORBIDDENROUTESECTION_H
#define S_FORBIDDENROUTESECTION_H

#include <QPointF>

struct S_QPointFWithIdxSegmentAndFeedStatus {
    QPointF _qpf;
    int _idxSegmentOwner;
    bool _bPointFeed;
    S_QPointFWithIdxSegmentAndFeedStatus();
    S_QPointFWithIdxSegmentAndFeedStatus(QPointF qpf, int idxSegmentOwner, bool bPointFeed);
    void clear();
    void debug_show();
};
struct S_ForbiddenRouteSectionsFromStartAndEnd {
    bool _bFullRouteIsForbidden; //when _bFullRouteIsForbidden is true, content of _sForbiddenRSFromStart and _sForbiddenRSFromEnd below considered unusable
    bool _bOnlyOnePointPossible; //_sForbiddenRSFromStart and _sForbiddenRSFromEnd contains the same values
    S_QPointFWithIdxSegmentAndFeedStatus _sFrontierFromStart;
    S_QPointFWithIdxSegmentAndFeedStatus _sFrontierFromEnd;
    S_ForbiddenRouteSectionsFromStartAndEnd();
    void clear();
    void debug_show();
};

#endif // S_FORBIDDENROUTESECTION_H
