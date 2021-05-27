#include <QGraphicsItem>

#include <QPainter>

#include <QDebug>

#include <QGraphicsScene>

#include "GraphicsRouteItem.h"

#include "GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.h"

//#include "CustomGraphicsTargetedPointItem.h"

#include "CustomGraphicsSegmentItem2.h"

#include "CustomGraphicsItemTypes.h"

#include "../../core/route.h"

#include "../S_ForbiddenRouteSection.h"

GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd::GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd(QGraphicsItem *parent):

    GraphicsRouteItem(parent) {
}

GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd::~GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd() {
    qDebug() << __FUNCTION__ ;
}

//before setting ptr stored here, be sure to delete the ptr stored here bevore; delete them by the scene or using scene->remove + delete the ptr after
bool GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd::initFromRouteAndFirstAndLastPointFromStartAndEnd(
        const Route& r, const S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRSFromStartAndEnd/*, qreal minimalElementSideSize*/) {

    qDebug() << __FUNCTION__ << "r.getId() = " << r.getId();
    qDebug() << __FUNCTION__ << "r.pointCount() = " << r.pointCount();

    _id = r.getId(); //id sync

    if (_qvectCGTargetPointItem_addedToScene_ptr.size()) {
        qDebug() << __FUNCTION__ << " rejected, _qvectCGTargetPointItem_addedToScene_ptr not empty";
        return(false);
    }
    if (_qvectCGSegmentItem_addedToScene_ptr.size()) {
        qDebug() << __FUNCTION__ << " rejected, _qvectCGSegmentItem_addedToScene_ptr not empty";
        return(false);
    }

    prepareGeometryChange();
    computeBoundingBoxNoZLIFrom_ptrCGPointItem_addedToScene();

    Qt::GlobalColor colorForbiddenSections    = Qt::red;
    Qt::GlobalColor colorForPermittedSections = Qt::green;

    if (sForbiddenRSFromStartAndEnd._bFullRouteIsForbidden) {
        qDebug() << __FUNCTION__ << "_bFullRouteIsForbidden";
        Qt::GlobalColor colorToUse = colorForbiddenSections;
        int nbSegment = r.segmentCount();
        for(int idxSegment=0; idxSegment< nbSegment; idxSegment++) {
            S_Segment segment = r.getSegment(idxSegment);
            insertCustomGraphicsSegmentItem_fromSegment(idxSegment, segment, colorToUse, colorToUse, this);
        }
        return(true);
    }

    //only one point considered as very close to impossible.
    if (sForbiddenRSFromStartAndEnd._bOnlyOnePointPossible) {
        qDebug() << __FUNCTION__ << "_bOnlyOnePointPossible";
        Qt::GlobalColor colorToUse = Qt::magenta;
        int nbSegment = r.segmentCount();
        for(int idxSegment=0; idxSegment< nbSegment; idxSegment++) {
            S_Segment segment = r.getSegment(idxSegment);
            insertCustomGraphicsSegmentItem_fromSegment(idxSegment, segment, colorToUse, colorToUse, this);
        }
        return(true);
    }

    qDebug() << __FUNCTION__ << "it's about frontier to frontier";

    bool bAForbiddenSectionFromStartExists = sForbiddenRSFromStartAndEnd._sFrontierFromStart._bPointFeed;
    bool bAForbiddenSectionFromEndExists   = sForbiddenRSFromStartAndEnd._sFrontierFromEnd._bPointFeed;

    bool bForbiddenSectionStartAndEndOnTheSameSegment = false;
    if (   bAForbiddenSectionFromStartExists
        && bAForbiddenSectionFromEndExists) {
        if (   sForbiddenRSFromStartAndEnd._sFrontierFromStart._idxSegmentOwner
            == sForbiddenRSFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner) {
            bForbiddenSectionStartAndEndOnTheSameSegment = true;
        }
    }

    //route with some forbidden section(s)

    //add the segments taking into account the forbidden sections:
    int nbSegment = r.segmentCount();

//#define DEF_debug_using_colored_segments
#ifdef DEF_debug_using_colored_segments
    Qt::GlobalColor colorForbiddenSections_beforeStartLimit_fullSegment = Qt::white;
    Qt::GlobalColor colorForbiddenSections_beforeStartLimit_partOfSegment = Qt::gray;

    Qt::GlobalColor colorForbiddenSections_afterEndLimit_fullSegment = Qt::yellow;
    Qt::GlobalColor colorForbiddenSections_afterEndLimit_partOfSegment = Qt::darkYellow;

    Qt::GlobalColor colorForPermittedSections_partOfSegment = Qt::blue;

#else
    Qt::GlobalColor colorForbiddenSections_beforeStartLimit_fullSegment = colorForbiddenSections;
    Qt::GlobalColor colorForbiddenSections_beforeStartLimit_partOfSegment = colorForbiddenSections;

    Qt::GlobalColor colorForbiddenSections_afterEndLimit_partOfSegment = colorForbiddenSections;
    Qt::GlobalColor colorForbiddenSections_afterEndLimit_fullSegment = colorForbiddenSections;

    Qt::GlobalColor colorForPermittedSections_partOfSegment = colorForPermittedSections;

#endif

    int idxSegmentInserted = 0;
    for(int idxSegment = 0; idxSegment < nbSegment; idxSegment++) {

        qDebug() << __FUNCTION__ << "loop with idxSegment = " << idxSegment;

        bool bDoTheTestaboutEnd = false;

        // F: Forbidden section
        // P: Permitted section
        // +: segment extremity
        // -: limit between F and P inside a segment
        //
        //typical examples:
        // F + F-P + P + P-F + F :: bForbiddenSectionStartAndEndOnTheSameSegment at false
        // F + F-P-F + F         :: bForbiddenSectionStartAndEndOnTheSameSegment at true
        // P + P-F + F :: no forbidden section from start,    forbidden section from end
        // F + F-P + P ::    forbidden section from start, no forbidden section from end

        //case with only one segment:
        // F-P-F :: forbidden section from start, forbidden section from end

        if (bAForbiddenSectionFromStartExists) {

            qDebug() << __FUNCTION__ << "#s1";

            //F
            //any full segment before the limit from the start
            if (idxSegment < sForbiddenRSFromStartAndEnd._sFrontierFromStart._idxSegmentOwner) {

                qDebug() << __FUNCTION__ << "#s2";

                S_Segment segment = r.getSegment(idxSegment);
                insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segment,
                                                            colorForbiddenSections_beforeStartLimit_fullSegment,
                                                            colorForbiddenSections_beforeStartLimit_fullSegment,
                                                            this);
                idxSegmentInserted++;

            } else {

                qDebug() << __FUNCTION__ << "#s3";

                //on the segment which define the limit from the start
                if (idxSegment == sForbiddenRSFromStartAndEnd._sFrontierFromStart._idxSegmentOwner) {

                    qDebug() << __FUNCTION__ << "#s4";

                    //F-
                    //insert forbidden section:
                    S_Segment segmentForbidden = r.getSegment(idxSegment);
                    segmentForbidden._ptB = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                    insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentForbidden,
                                                                colorForbiddenSections_beforeStartLimit_partOfSegment,
                                                                colorForbiddenSections_beforeStartLimit_partOfSegment,
                                                                this);
                    idxSegmentInserted++;

                    //handle all remaining parts of the same segment

                    if (!bAForbiddenSectionFromEndExists) {

                        qDebug() << __FUNCTION__ << "#s5";

                        //-P
                        //insert permitted about the end of the segment
                        S_Segment segmentPermitted = r.getSegment(idxSegment);
                        segmentPermitted._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                        insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                    colorForPermittedSections_partOfSegment,
                                                                    colorForPermittedSections_partOfSegment,
                                                                    this);
                        idxSegmentInserted++;

                    } else { //forbidden section from end

                        qDebug() << __FUNCTION__ << "#s6";

                        if (bForbiddenSectionStartAndEndOnTheSameSegment) {

                            qDebug() << __FUNCTION__ << "#s7";

                            //-P
                            //insert first permitted until the forbidden section on the same segment
                            S_Segment segmentPermitted;
                            segmentPermitted._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                            segmentPermitted._ptB = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                            insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                        colorForPermittedSections_partOfSegment,
                                                                        colorForPermittedSections_partOfSegment, this);
                            idxSegmentInserted++;

                            //-F
                            //insert first section of the forbidden section from the end:
                            S_Segment segmentForbiddenSS = r.getSegment(idxSegment);
                            segmentForbiddenSS._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromEnd._qpf;
                            insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentForbiddenSS,
                                                                        colorForbiddenSections_afterEndLimit_partOfSegment,
                                                                        colorForbiddenSections_afterEndLimit_partOfSegment,
                                                                        this);
                            idxSegmentInserted++;

                        } else {

                            qDebug() << __FUNCTION__ << "#s8";

                            //-P
                            //insert permitted about the end of the same segment
                            S_Segment segmentPermitted = r.getSegment(idxSegment);
                            segmentPermitted._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                            insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                        colorForPermittedSections_partOfSegment,
                                                                        colorForPermittedSections_partOfSegment,
                                                                        this);
                            idxSegmentInserted++;
                        }
                    }
                } else { // > forbidden from the start limit

                    qDebug() << __FUNCTION__ << "#s9";

                    bDoTheTestaboutEnd = true;
                }
            }

        } else { //no forbidden section from start

            qDebug() << __FUNCTION__ << "#s10";

            bDoTheTestaboutEnd = true;
        }

        qDebug() << __FUNCTION__ << "#s10-5 idxSegment = " << idxSegment << "bDoTheTestaboutEnd = " << bDoTheTestaboutEnd;

        if (bDoTheTestaboutEnd) {

            qDebug() << __FUNCTION__ << "#s11";

            if (bAForbiddenSectionFromEndExists) {

                qDebug() << __FUNCTION__ << "#s12";

                //any full permitted segments
                if (idxSegment < sForbiddenRSFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner) {

                    qDebug() << __FUNCTION__ << "#s13";

                    S_Segment segmentPermitted = r.getSegment(idxSegment);
                    insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                colorForPermittedSections,
                                                                colorForPermittedSections,
                                                                this);
                    idxSegmentInserted++;

                } else {

                    qDebug() << __FUNCTION__ << "#s14";

                    //any full segment after the limit from the end
                    if (idxSegment > sForbiddenRSFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner) {

                        qDebug() << __FUNCTION__ << "#s15";

                        S_Segment segmentForbidden = r.getSegment(idxSegment);
                        insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentForbidden,
                                                                    colorForbiddenSections_afterEndLimit_fullSegment,
                                                                    colorForbiddenSections_afterEndLimit_fullSegment,
                                                                    this);
                        idxSegmentInserted++;

                    } else {

                        qDebug() << __FUNCTION__ << "#s16";

                        //segment has the limit point between permitted/forbidden from the end
                        if (idxSegment == sForbiddenRSFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner) {

                            qDebug() << __FUNCTION__ << "#s17";

                            //-P
                            //insert first permitted until the forbidden section on the same segment
                            S_Segment segmentPermitted= r.getSegment(idxSegment);
                            segmentPermitted._ptB = sForbiddenRSFromStartAndEnd._sFrontierFromEnd._qpf;
                            insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                        colorForPermittedSections_partOfSegment,
                                                                        colorForPermittedSections_partOfSegment,
                                                                        this);
                            idxSegmentInserted++;

                            //-F
                            //insert first section of the forbidden section from the end:
                            S_Segment segmentForbiddenSS = r.getSegment(idxSegment);
                            segmentForbiddenSS._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromEnd._qpf;
                            insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentForbiddenSS,
                                                                        colorForbiddenSections_afterEndLimit_partOfSegment,
                                                                        colorForbiddenSections_afterEndLimit_partOfSegment,
                                                                        this);
                            idxSegmentInserted++;
                        }
                    }
                }
            } else { //no forbidden section from end

                qDebug() << __FUNCTION__ << "#s18";

                S_Segment segmentPermitted = r.getSegment(idxSegment);
                insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                            colorForPermittedSections,
                                                            colorForPermittedSections,
                                                            this);
                idxSegmentInserted++;
            }
        } else {

            if (nbSegment == 1) {

                if (bAForbiddenSectionFromEndExists) {

                    qDebug() << __FUNCTION__ << "#s18-5";

                    S_Segment segmentPermitted = r.getSegment(idxSegment);
                    segmentPermitted._ptA = sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
                    segmentPermitted._ptB = sForbiddenRSFromStartAndEnd._sFrontierFromEnd._qpf;
                    insertCustomGraphicsSegmentItem_fromSegment(idxSegmentInserted, segmentPermitted,
                                                                colorForPermittedSections,
                                                                colorForPermittedSections,
                                                                this);
                    idxSegmentInserted++;
                }
            }
        }
    }
    return(true);
}


void GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd::insertCustomGraphicsSegmentItem_fromSegment(int idxSegmentInserted,  S_Segment segment,
                                                                    Qt::GlobalColor colorHightlighted, Qt::GlobalColor colorNotHightlighted,
                                                                    QGraphicsItem *parent) {
    CustomGraphicsSegmentItem2 *lineSegment_permitted = new CustomGraphicsSegmentItem2(idxSegmentInserted,
                                                                                   segment._ptA, segment._ptB,
                                                                                   colorHightlighted, colorNotHightlighted,
                                                                                   parent);
    _qvectCGSegmentItem_addedToScene_ptr.push_back(lineSegment_permitted);
}


int GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd::type() const {
    return(e_customGraphicsItemType::e_cgit_GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd);
}
