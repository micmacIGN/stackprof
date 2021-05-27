#include <QGraphicsItem>

#include <QPainter>

#include <QDebug>

#include <QGraphicsScene>

#include "GraphicsRouteItem.h"

#include "CustomGraphicsTargetedPointItem2.h"

#include "CustomGraphicsSegmentItem2.h"

#include "CustomGraphicsItemTypes.h"

#include "../../core/route.h"

GraphicsRouteItem::GraphicsRouteItem():////QGraphicsObject//QObject
GraphicsRouteItem(nullptr) {

}

GraphicsRouteItem::GraphicsRouteItem(QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),
    _id(-1),
    _currentZLI(-1),
    _qrectf_boundingBoxNoZLI{.0,.0,.0,.0},
    _qrectf_boundingBox{.0,.0,.0,.0},

    _bShowAsHighlighted(false),
    _bShowBoundingRect(false) {
    _p.setStyle(Qt::PenStyle::SolidLine);
    _p.setColor(Qt::magenta);
    _p.setCosmetic(true);
}

void GraphicsRouteItem::setRouteHightlighted(bool bShowAsHighlighted) {

    _bShowAsHighlighted = bShowAsHighlighted;
    for (auto &iterPoint: _qvectCGTargetPointItem_addedToScene_ptr) {
        iterPoint->setHightlighted(_bShowAsHighlighted);
    }
    for (auto &iterSegment: _qvectCGSegmentItem_addedToScene_ptr) {
        iterSegment->setHightlighted(_bShowAsHighlighted);
    }
}

//this is requiered to update the GraphicsRouteItem id to be able to update the according route in the routecontainer
void GraphicsRouteItem::resetId(int newId) {
    _id = newId;
}

int GraphicsRouteItem::getId() {
    return(_id);
}

GraphicsRouteItem::~GraphicsRouteItem() {
    qDebug() << __FUNCTION__;
    removeItemsFromSceneAndDeleteThem();
}

void GraphicsRouteItem::computeBoundingBoxNoZLIFrom_ptrCGPointItem_addedToScene() {

    int qvectCGTargetPointItem_aTSp_size = _qvectCGTargetPointItem_addedToScene_ptr.size();

    if (qvectCGTargetPointItem_aTSp_size == 1) {

        qDebug() << __FUNCTION__ <<  " if (_qvectCGTargetPointItem_aTSp_size == 1)";

        _qrectf_boundingBoxNoZLI = _qvectCGTargetPointItem_addedToScene_ptr[0]->getBoundingRectNoZLI();

    } else {

        if (qvectCGTargetPointItem_aTSp_size) {

            QRectF qrectfUniting = _qvectCGTargetPointItem_addedToScene_ptr[0]->getBoundingRectNoZLI();
            for(auto& iter: _qvectCGTargetPointItem_addedToScene_ptr) {
                qrectfUniting = qrectfUniting.united(iter->getBoundingRectNoZLI());
            }
            _qrectf_boundingBoxNoZLI = qrectfUniting;

            /*
            int iDbg = 0;

            qreal qr_mostTop  = +100001;
            qreal qr_mostDown = -100001;
            qreal qr_mostLeft = +100001;
            qreal qr_mostRight= -100001;

            for(auto& iter: _qvectCGTargetPointItem_addedToScene_ptr) {
                QPointF qpf_locNoZLI = iter->get_qpointf_locationNoZLI();
                //qDebug() << __FUNCTION__ << "_qpf_locNoZLI = " << _qpf_locNoZLI;
                if (qpf_locNoZLI.x() < qr_mostLeft ) { qDebug() << "set mostLeft" ; qr_mostLeft  = qpf_locNoZLI.x();}
                if (qpf_locNoZLI.x() > qr_mostRight) { qDebug() << "set mostRight"; qr_mostRight = qpf_locNoZLI.x();}
                if (qpf_locNoZLI.y() < qr_mostTop  ) { qDebug() << "set mostTop"  ; qr_mostTop   = qpf_locNoZLI.y();}
                if (qpf_locNoZLI.y() > qr_mostDown ) { qDebug() << "set mostDown" ; qr_mostDown  = qpf_locNoZLI.y();}
                qDebug() << "iDbg=" << iDbg << "; iter.x;y = " << qpf_locNoZLI.x() << ", " << qpf_locNoZLI.y();
                iDbg++;
            }

            qDebug() << __FUNCTION__ << "qr_mostLeft  = " << qr_mostLeft;
            qDebug() << __FUNCTION__ << "qr_mostRight = " << qr_mostRight;
            qDebug() << __FUNCTION__ << "qr_mostTop  = " << qr_mostTop;
            qDebug() << __FUNCTION__ << "qr_mostDown = " << qr_mostDown;
            */

        } else {
            qDebug() << __FUNCTION__ <<  "_qvectCGTargetPointItem_addedToScene_ptr is empty";

            qreal qr_mostTop  = -0.5;
            qreal qr_mostDown =  qr_mostTop+1.0;
            qreal qr_mostLeft = -0.5;
            qreal qr_mostRight=qr_mostLeft + 1.0;

            _qrectf_boundingBoxNoZLI.setTopLeft(QPointF(qr_mostLeft, qr_mostTop));
            _qrectf_boundingBoxNoZLI.setBottomRight(QPointF(qr_mostRight, qr_mostDown)); //use of bottom and right is ok with QRectF (not QRect; see Qt doc)

            qDebug() << __FUNCTION__ << "qr_mostLeft  = " << qr_mostLeft;
            qDebug() << __FUNCTION__ << "qr_mostRight = " << qr_mostRight;
            qDebug() << __FUNCTION__ << "qr_mostTop  = " << qr_mostTop;
            qDebug() << __FUNCTION__ << "qr_mostDown = " << qr_mostDown;

        }        
    }

    qDebug() << __FUNCTION__ << "_boundingBoxNoZLI.topLeft     = " << _qrectf_boundingBoxNoZLI.topLeft();
    qDebug() << __FUNCTION__ << "_boundingBoxNoZLI.bottomRight = " << _qrectf_boundingBoxNoZLI.bottomRight();

}


void GraphicsRouteItem::removeItemsFromSceneAndDeleteThem() {
    qDebug() << __FUNCTION__ << "entering";

    for(auto& iter: _qvectCGTargetPointItem_addedToScene_ptr) {
        if (iter) {
            QGraphicsScene *scenePtrOfQGraphicsItem = iter->scene();
            if (scenePtrOfQGraphicsItem) {
                scenePtrOfQGraphicsItem->removeItem(iter);

            } else {
                qDebug() << __FUNCTION__ << "(targetPointItem) iter was not added to a scene()";

            }
            //iter->deleteLater(); //because the signal to delete the point was emit from the point itself
            delete iter;
            iter = nullptr;
        } else {
            qDebug() << __FUNCTION__ <<  "(targetPointItem) iter is nullptr => no delete";
        }
    }
    //lineItem
    for(auto& iter: _qvectCGSegmentItem_addedToScene_ptr) {
        if (iter) {
            QGraphicsScene *scenePtrOfQGraphicsItem = iter->scene();
            if (scenePtrOfQGraphicsItem) {
                scenePtrOfQGraphicsItem->removeItem(iter);
            } else {
                qDebug() << __FUNCTION__ <<  "(segmentItem) iter was not added to a scene()";
            }
            delete iter;
            iter = nullptr;
        } else {
            qDebug() << __FUNCTION__ << "(segmentItem) iter is nullptr => no delete";
        }
    }
}

void GraphicsRouteItem::slot_customGraphicsTargetPointItemEndOfMove(int idPoint/**/, QPointF newPosNoZLI/**/) {
    qDebug() << __FUNCTION__ ;
    QPointF qpfnewLocation = newPosNoZLI; //_qvectCGTargetPointItem_addedToScene_ptr.at(idPoint)->get_qpointf_locationNoZLI();
    emit signal_updatePointOfRoute(idPoint, qpfnewLocation, _id); //update model, to be sync

    prepareGeometryChange();
    computeBoundingBoxNoZLIFrom_ptrCGPointItem_addedToScene(); //update boundingBox using graphicsPointItem

    adjustLocationAndSizeToZLI(_currentZLI);

}

void GraphicsRouteItem::slot_setGraphicsLocationOfPointForRoute(int idPoint, QPointF newPosNoZLI, int idRoute) {

    qDebug() << __FUNCTION__  << "idPoint = " <<idPoint;
    qDebug() << __FUNCTION__  << "newPosNoZLI = " << newPosNoZLI;
    qDebug() << __FUNCTION__  << "idRoute = " << idRoute;

    if (_id != idRoute) {
        qDebug() << __FUNCTION__ << " not for me";
        return;
    }

    int nbSegment = _qvectCGSegmentItem_addedToScene_ptr.size();
    if (!nbSegment) {
        qDebug() << __FUNCTION__ << "no segment to update";
        return;
    }

    bool bInvalidPoint = false;
    if ( (idPoint < 0)
        ||(idPoint > nbSegment)) {
        bInvalidPoint = true;
    }
    if (bInvalidPoint) {
        qDebug() << __FUNCTION__ << " (#1) invalid idPoint (" << idPoint;
        return;
    }

    //should never happens:
    if (idPoint >= _qvectCGTargetPointItem_addedToScene_ptr.size()) {
        qDebug() << __FUNCTION__ << " (#2) invalid idPoint (" << idPoint;
        return;
    }

    //set CGTargetPointItem:
    _qvectCGTargetPointItem_addedToScene_ptr[idPoint]->set_qpointf_locationNoZLI(newPosNoZLI);

    //set associated segment:
    if (!idPoint) { //first point of all segments
        int segmentId = 0;
        qDebug() << __FUNCTION__ << __LINE__;
        _qvectCGSegmentItem_addedToScene_ptr.at(segmentId)->setPointAnoZLI_andUpdate(newPosNoZLI); //adjust only A on first segment
    } else {
        if (idPoint == nbSegment) { //last point of all segments
            int segmentId = nbSegment-1;
            qDebug() << __FUNCTION__ << __LINE__;
            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId)->setPointBnoZLI_andUpdate(newPosNoZLI);
        } else {
            int segmentId_endingAtPoint   = idPoint-1;
            int segmentId_startingAtPoint = idPoint;
            qDebug() << __FUNCTION__ << __LINE__;

            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId_endingAtPoint)  ->setPointBnoZLI_andUpdate(newPosNoZLI);
            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId_startingAtPoint)->setPointAnoZLI_andUpdate(newPosNoZLI);
        }
    }
}


void GraphicsRouteItem::slot_customGraphicsTargetPointItemMoved(int idPoint, QPointF newPos) {

    qDebug() << __FUNCTION__ << " received infos about point idPoint = " << idPoint;
    qDebug() << __FUNCTION__ << " received infos about point newPos = " << newPos;

    QPointF newPosNoZLI = newPos*static_cast<qreal>(_currentZLI);
    qDebug() << __FUNCTION__ << " newPosNoZLI = " << newPosNoZLI;

    int nbSegment = _qvectCGSegmentItem_addedToScene_ptr.size();
    if (!nbSegment) {
        qDebug() << __FUNCTION__ << "no segment to update";
        return;
    }

    bool bInvalidPoint = false;
    if ( (idPoint < 0)
        ||(idPoint > nbSegment)) {
        bInvalidPoint = true;
    }
    if (bInvalidPoint) {
        qDebug() << __FUNCTION__ << "invalid idPoint (" << idPoint;
        return;
    }

    if (!idPoint) { //first point of all segments
        int segmentId = 0;
        _qvectCGSegmentItem_addedToScene_ptr.at(segmentId)->setPointAnoZLI_andUpdate(newPosNoZLI); //adjust only A on first segment
    } else {
        if (idPoint == nbSegment) { //last point of all segments
            int segmentId = nbSegment-1;
            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId)->setPointBnoZLI_andUpdate(newPosNoZLI);
        } else {
            int segmentId_endingAtPoint   = idPoint-1;
            int segmentId_startingAtPoint = idPoint;
            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId_endingAtPoint)  ->setPointBnoZLI_andUpdate(newPosNoZLI);
            _qvectCGSegmentItem_addedToScene_ptr.at(segmentId_startingAtPoint)->setPointAnoZLI_andUpdate(newPosNoZLI);
        }
    }
}


//before setting ptr stored here, be sure to delete the ptr stored here before; delete them by the scene or using scene->remove + delete the ptr after
void GraphicsRouteItem::initFromRoute(const Route& r, qreal minimalElementSideSize) {

    qDebug() << __FUNCTION__ << "r.getId() = " << r.getId();
    qDebug() << __FUNCTION__ << "r.pointCount() = " << r.pointCount();

    _id = r.getId(); //id sync

    //clear the qvector with no delete (done before)
    _qvectCGTargetPointItem_addedToScene_ptr.clear();
    _qvectCGSegmentItem_addedToScene_ptr.clear();

    qreal elementSideSize = minimalElementSideSize;
    qreal minDistanceBetweenClosestSuccessivePoints = r.distanceBetweenClosestSuccessivePoints();
    if (minDistanceBetweenClosestSuccessivePoints < .0) {
        //do nothing: elementSideSize already set above
    } else {
        elementSideSize = 0.33*minDistanceBetweenClosestSuccessivePoints;
    }

    int idPoint = 0;
    //add the points
    for(const auto& iter: r.getCstRefVectPoint()) {

        qDebug() << __FUNCTION__ << "iter on getCstRefVectPoint #idPoint: " << idPoint;

        CustomGraphicsTargetPointItem2 *cgPointItem = new CustomGraphicsTargetPointItem2(idPoint, true, true, iter, elementSideSize);

        QObject::connect(cgPointItem, &CustomGraphicsTargetPointItem2::signal_customGraphicsTargetPointItemMoved,
                                this, &GraphicsRouteItem::slot_customGraphicsTargetPointItemMoved);

        QObject::connect(cgPointItem, &CustomGraphicsTargetPointItem2::signal_customGraphicsTargetPointItemEndOfMove,
                                this, &GraphicsRouteItem::slot_customGraphicsTargetPointItemEndOfMove);

        QObject::connect(cgPointItem, &CustomGraphicsTargetPointItem2::signal_pointHoverInOut,
                                this, &GraphicsRouteItem::slot_pointHoverInOut);

        if (!idPoint) {
            cgPointItem->setDisplayState_doubleCircle_squareBox(true, true);
        }

        cgPointItem->setParentItem(this);
        cgPointItem->setPos(0,0);
        _qvectCGTargetPointItem_addedToScene_ptr.push_back(cgPointItem);
        qDebug() << "insert idPoint as "<< idPoint << "; iter.x;y = " <<iter.x() << ", " << iter.y();
        idPoint++;
    }

    prepareGeometryChange();
    computeBoundingBoxNoZLIFrom_ptrCGPointItem_addedToScene();

    //add the segment line
    int nbSegment = r.segmentCount();
    for(int idxSegment=0; idxSegment< nbSegment; idxSegment++) {
        S_Segment segment = r.getSegment(idxSegment);
        CustomGraphicsSegmentItem2 *lineSegment = new CustomGraphicsSegmentItem2(idxSegment, segment._ptA, segment._ptB, Qt::cyan, Qt::cyan/*Qt::darkCyan*/, this);
        _qvectCGSegmentItem_addedToScene_ptr.push_back(lineSegment);
    }

}

void GraphicsRouteItem::slot_pointHoverInOut(bool bHoverInOrOutstate, int idPoint) {
    emit signal_pointHoverInOut_ownedByRoute(bHoverInOrOutstate, idPoint, _id);
}

void GraphicsRouteItem::adjustLocationAndSizeToZLI(int ZLI) {
    _currentZLI = ZLI;

    prepareGeometryChange();

    foreach(CustomGraphicsSegmentItem2 *PtrSegmentLine, _qvectCGSegmentItem_addedToScene_ptr) {
        PtrSegmentLine->adjustLocationAndSizeToZLI(_currentZLI);
    }
    foreach(CustomGraphicsTargetPointItem2 *PtrPointItem, _qvectCGTargetPointItem_addedToScene_ptr) {
        PtrPointItem->adjustLocationAndSizeToZLI(_currentZLI);
    }

    _qrectf_boundingBox.setTopLeft    (_qrectf_boundingBoxNoZLI.topLeft()/static_cast<qreal>(_currentZLI));
    _qrectf_boundingBox.setBottomRight(_qrectf_boundingBoxNoZLI.bottomRight()/static_cast<qreal>(_currentZLI));

    qDebug() << __FUNCTION__ << "GraphicsRouteItem: _boudingBoxNoZLI.TopLeft    = " << _qrectf_boundingBox.topLeft();
    qDebug() << __FUNCTION__ << "GraphicsRouteItem: _boudingBoxNoZLI.BottomRight = " << _qrectf_boundingBox.bottomRight();
}

QRectF GraphicsRouteItem::boundingRect() const {
    return(_qrectf_boundingBox);
}

void GraphicsRouteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //qDebug() << __FUNCTION__ << " GraphicsRouteItem";

    //qDebug() << __FUNCTION__ << "GraphicsRouteItem " << "_qrectf_boundingBox = " << _qrectf_boundingBox;

    //show the boudingRect for debug purpose

    /*_bShowBoundingRect = true;
    if (_bShowBoundingRect) {
        painter->setPen(_p);
        painter->drawRect(_qrectf_boundingBox);
    }*/

    //qDebug() << __FUNCTION__ << "GraphicsRouteItem " << "scenePos()       = " << scenePos();
    //qDebug() << __FUNCTION__ << "GraphicsRouteItem " << "sceneTransform() = " << sceneTransform();

}

int GraphicsRouteItem::type() const {
    return(e_customGraphicsItemType::e_cgit_GraphicsRouteItem);
}

void GraphicsRouteItem::adjustColors_segment_unselectedPoint(const QColor& qCol_segmentColor, const QColor& qCol_sunselectPoint) {
    for (auto &iterPoint: _qvectCGTargetPointItem_addedToScene_ptr) {
        iterPoint->setColor_unselectedPoint(qCol_sunselectPoint);
    }
    for (auto &iterSegment: _qvectCGSegmentItem_addedToScene_ptr) {
        qDebug() << __FUNCTION__ << "(GraphicsRouteItem) loop for (auto &iterSegment: _qvectCGSegmentItem_addedToScene_ptr) { ...";
        iterSegment->setColorNotHighlighted_Highlighted(qCol_segmentColor, qCol_segmentColor);
    }
    setRouteHightlighted(_bShowAsHighlighted);
}

void GraphicsRouteItem::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {
    //_currentScaleOfViewSideForCurrentZLI = scaleOfViewSideForCurrentZLI;
    for (auto &iterSegment: _qvectCGSegmentItem_addedToScene_ptr) {
        iterSegment->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    }    
    for (auto &iterPoint: _qvectCGTargetPointItem_addedToScene_ptr) {
        iterPoint->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    }
}

void GraphicsRouteItem::debugShowStateAndLocation_aboutPoints() {
    for (auto &iterPoint: _qvectCGTargetPointItem_addedToScene_ptr) {
        iterPoint->debugShowStateAndLocation();
    }
}



