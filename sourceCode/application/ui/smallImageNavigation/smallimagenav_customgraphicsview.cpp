#include <QGraphicsView>
#include <QDebug>

#include "smallimagenav_customgraphicsview.h"

#include "../wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h"

SmallImageNav_customGraphicsView::SmallImageNav_customGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    qDebug() << __FUNCTION__ << " (constructor)";

    setMouseTracking(true); //to be able to have mouse event in the scene without the need to press a mouse button
    //setTransformationAnchor(QGraphicsView::NoAnchor);
    // //setTransformationAnchor(QGraphicsView::AnchorUnderMouse); //to zoom on the area under the mouse

     qDebug() << __FUNCTION__ << "(end of constructor)";
}


#define _def_code_using_WheelEventZoomInOutNoNoiseHandler
#ifdef _def_code_using_WheelEventZoomInOutNoNoiseHandler

void SmallImageNav_customGraphicsView::wheelEvent(QWheelEvent *event) {
    qDebug() << __FUNCTION__ << "-- start --------------";

    event->accept();

    ZoomHandler::e_Zoom_Direction zoomDirIfDoZoom = ZoomHandler::e_Zoom_Direction::e_ZoomDir_In; //just to be set on a value (can be invalid)
    bool bDoZoom = wheelEventZoomInOut_noNoise(event, zoomDirIfDoZoom);
    //bool bDoZoom = wheelEventZoomInOut_noNoise_noFilterSmallEvent(event, zoomDirIfDoZoom);
    if (bDoZoom) {
        emit signal_zoomRequest_fromSmallImaveNav(zoomDirIfDoZoom);
        _y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        _y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
    }
}

#endif
