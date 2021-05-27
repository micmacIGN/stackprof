#include <QDebug>

#include "S_ForbiddenRouteSection.h"

S_QPointFWithIdxSegmentAndFeedStatus::S_QPointFWithIdxSegmentAndFeedStatus():
    _qpf {.0,.0},
    _idxSegmentOwner(-1),
    _bPointFeed(false) {
}

S_QPointFWithIdxSegmentAndFeedStatus::S_QPointFWithIdxSegmentAndFeedStatus(QPointF qpf, int idxSegmentOwner, bool bPointFeed) {
    _qpf = qpf;
    _idxSegmentOwner =idxSegmentOwner;
    _bPointFeed = bPointFeed;
}

void S_QPointFWithIdxSegmentAndFeedStatus::clear() {
    _qpf = {.0,.0};
    _idxSegmentOwner = -1;
    _bPointFeed = false;
}

void S_QPointFWithIdxSegmentAndFeedStatus::debug_show() {
    qDebug() << __FUNCTION__ << " (S_QPointFWithIdxSegmentAndFeedStatus)" << " _bPointFeed = " << _bPointFeed;
    qDebug() << __FUNCTION__ << " (S_QPointFWithIdxSegmentAndFeedStatus)" << " _idxSegmentOwner = " << _idxSegmentOwner;
    qDebug() << __FUNCTION__ << " (S_QPointFWithIdxSegmentAndFeedStatus)" << " _qpf = " << _qpf;
}

S_ForbiddenRouteSectionsFromStartAndEnd::S_ForbiddenRouteSectionsFromStartAndEnd(): _bFullRouteIsForbidden {true}, _bOnlyOnePointPossible {false} {

}

void S_ForbiddenRouteSectionsFromStartAndEnd::clear() {
    _sFrontierFromStart.clear();
    _sFrontierFromEnd.clear();
    _bFullRouteIsForbidden = false;
    _bOnlyOnePointPossible = false;
}

void S_ForbiddenRouteSectionsFromStartAndEnd::debug_show() {
    qDebug() << __FUNCTION__ << " ( S_ForbiddenRouteSectionsFromStartAndEnd)" << " _bFullRouteIsForbidden = " << _bFullRouteIsForbidden;
    qDebug() << __FUNCTION__ << " (S_ForbiddenRouteSectionsFromStartAndEnd)"  << " _bOnlyOnePointPossible = " << _bOnlyOnePointPossible;

    qDebug() << __FUNCTION__ << " (S_ForbiddenRouteSectionsFromStartAndEnd)"  << " _sFrontierFromStart = ";
    _sFrontierFromStart.debug_show();

    qDebug() << __FUNCTION__ << " (S_ForbiddenRouteSectionsFromStartAndEnd)"  << " _sFrontierFromEnd = ";
    _sFrontierFromEnd.debug_show();
}

