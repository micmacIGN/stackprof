#include <QGraphicsItem>

#include <QPainter>

#include <QDebug>

#include <QGraphicsScene>

#include "CustomGraphicsItemTypes.h"

#include "CustomGraphicsTargetedPointItem2.h"
#include "CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.h"

#include "CustomGraphicsSpecificItemsOfInterest2.h"

CustomGraphicsSpecificItemsOfInterest2::CustomGraphicsSpecificItemsOfInterest2(QGraphicsItem *parent): QGraphicsItem(parent),
    _currentZLI(-1),
    _qrectf_boundingBoxNoZLI{.0,.0,.0,.0},
    _qrectf_boundingBox{.0,.0,.0,.0},
    //_bShowBoundingRect(false),

    _cGTargetgetPoint_firstPointOfRoute {0, false, false, {.0,.0}, 1.0, Qt::green, Qt::green, this}, //no mouse move, no highlight
    _cGTargetPoint_lastPointOfRoute {1, {.0,.0}, 1.0, this},
    _bFeedWithValidLocations(false)
{
    _qpen.setStyle(Qt::PenStyle::SolidLine);
    _qpen.setColor(Qt::magenta);
    _qpen.setCosmetic(true);

    _cGTargetgetPoint_firstPointOfRoute.setDisplayState_doubleCircle_squareBox(true, false);

}

void CustomGraphicsSpecificItemsOfInterest2::clearLocations() {
    _cGTargetgetPoint_firstPointOfRoute.set_qpointf_locationNoZLI({.0,.0});
    _cGTargetPoint_lastPointOfRoute.set_qpointf_locationNoZLI({.0,.0});

    _bFeedWithValidLocations = false;
    computeBoundingBoxNoZLI();
}

void CustomGraphicsSpecificItemsOfInterest2::debugShowStateAndLocations() {
    qDebug() << __FUNCTION__ << "_bFeedWithValidLocations = " << _bFeedWithValidLocations;
    qDebug() << __FUNCTION__ << "_cGTargetgetPoint_firstPointOfRoute = ";
    _cGTargetgetPoint_firstPointOfRoute.debugShowStateAndLocation();

    qDebug() << __FUNCTION__ << "_cGTargetPoint_lastPointOfRoute = ";
    _cGTargetPoint_lastPointOfRoute.debugShowStateAndLocation();
}



CustomGraphicsSpecificItemsOfInterest2::~CustomGraphicsSpecificItemsOfInterest2() {
    qDebug() << __FUNCTION__;
}

//@LP considers here that routeForInit was fully checked as valid before calling initFromRoute()
void CustomGraphicsSpecificItemsOfInterest2::initFromRoute(const Route& routeForInit) {

    qDebug() << __FUNCTION__ <<"(CustomGraphicsSpecificItemsOfInterest)";

    if (!routeForInit.segmentCount()) {
        qDebug() << __FUNCTION__ <<"if (!routeForInit.segmentCount()) { ";
        return;
    }
    prepareGeometryChange();

    S_Segment shortcutSegmentFirstToLastPointofRoute;
    shortcutSegmentFirstToLastPointofRoute._ptA = routeForInit.getPoint(0);
    shortcutSegmentFirstToLastPointofRoute._ptB = routeForInit.getPoint(routeForInit.pointCount()-1);
    shortcutSegmentFirstToLastPointofRoute._bValid = true;
    double distanceOfShortcutSegmentFirstToLastPointofRoute = shortcutSegmentFirstToLastPointofRoute.length();

    if (distanceOfShortcutSegmentFirstToLastPointofRoute < 8.0) { //to have at least size of items below as 1.0 (which is already very small)
        distanceOfShortcutSegmentFirstToLastPointofRoute = 8.0;
    }

    double sideSizeNoZLI = distanceOfShortcutSegmentFirstToLastPointofRoute / 8.0;

    _cGTargetgetPoint_firstPointOfRoute.setSideSizeNoZLI(sideSizeNoZLI);
    _cGTargetPoint_lastPointOfRoute.setSideSizeNoZLI(sideSizeNoZLI);

    _cGTargetgetPoint_firstPointOfRoute.set_qpointf_locationNoZLI(routeForInit.getPoint(0));
    _cGTargetPoint_lastPointOfRoute.set_qpointf_locationNoZLI(routeForInit.getPoint(routeForInit.pointCount()-1));

    _bFeedWithValidLocations = true;

    computeBoundingBoxNoZLI();

    debugShowStateAndLocations();
}

void CustomGraphicsSpecificItemsOfInterest2::computeBoundingBoxNoZLI() {

    _qrectf_boundingBoxNoZLI = {.0,.0,.0,.0};
    if (!_bFeedWithValidLocations) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) " << "if (_bFeedWithValidLocations) {";
        return;
    }
    QRectF boundingRect_of_firstPointOfRouteNoZLI = _cGTargetgetPoint_firstPointOfRoute.getBoundingRectNoZLI();
    QRectF boundingRect_of_lastPointOfRouteNoZLI  = _cGTargetPoint_lastPointOfRoute.getBoundingRectNoZLI_withWidthBorder();
    qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) " << "boundingRect_of_firstPointOfRouteNoZLI = " << boundingRect_of_firstPointOfRouteNoZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) " << "boundingRect_of_lastPointOfRouteNoZLI  = " << boundingRect_of_lastPointOfRouteNoZLI;

    _qrectf_boundingBoxNoZLI = boundingRect_of_firstPointOfRouteNoZLI.united(boundingRect_of_lastPointOfRouteNoZLI);

    qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) _qrectf_boundingBoxNoZLI = " << _qrectf_boundingBoxNoZLI;

}

void CustomGraphicsSpecificItemsOfInterest2::adjustLocationAndSizeToZLI(int ZLI) {
    _currentZLI = ZLI;

    prepareGeometryChange();

    _cGTargetgetPoint_firstPointOfRoute.adjustLocationAndSizeToZLI(ZLI);
    _cGTargetPoint_lastPointOfRoute.adjustLocationAndSizeToZLI(ZLI);

    computeBoundingBoxNoZLI();

    _qrectf_boundingBox.setTopLeft    (_qrectf_boundingBoxNoZLI.topLeft()/static_cast<qreal>(_currentZLI));
    _qrectf_boundingBox.setBottomRight(_qrectf_boundingBoxNoZLI.bottomRight()/static_cast<qreal>(_currentZLI));

    qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) _qrectf_boundingBox = " << _qrectf_boundingBox;

}

QRectF CustomGraphicsSpecificItemsOfInterest2::boundingRect() const {
    return(_qrectf_boundingBox);
}

void CustomGraphicsSpecificItemsOfInterest2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    //qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) " << "_bShowBoundingRect =" << _bShowBoundingRect;
    qDebug() << __FUNCTION__ << "(CustomGraphicsSpecificItemsOfInterest) " << "_qrectf_boundingBox =" << _qrectf_boundingBox;

    /*
    bool bShowBoundingRect = true;
    //show the boudingRect for debug purpose
    if (bShowBoundingRect) {

        //---@#LP debug
        QBrush brush_debug(Qt::SolidPattern);
        //brush_debug.setColor({0,128,0,128});
        brush_debug.setColor({128,64,0,128});
        painter->setBrush(brush_debug);
        painter->setPen(Qt::PenStyle::NoPen);
        painter->drawRect(_qrectf_boundingBox);
        //---

        //painter->setPen(_qpen);
        //painter->drawRect(_qrectf_boundingBox);
    }
    */
}

int CustomGraphicsSpecificItemsOfInterest2::type() const {
    return(e_customGraphicsItemType::e_cgit_CustomGraphicsSpecificItemsOfInterest);
}


void CustomGraphicsSpecificItemsOfInterest2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

    if (!_bFeedWithValidLocations) {
        return;
    }

    _cGTargetgetPoint_firstPointOfRoute.adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    _cGTargetPoint_lastPointOfRoute.adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);

}




