#include "math.h"

#include <QGraphicsView>
#include <QDebug>

#include <QMouseEvent>

#include <QScrollBar>

#include "customgraphicsview.h"

#include "../logic/model/imageScene/customgraphicsscene_usingTPDO.h"

#include "../logic/zoomLevelImage/zoomHandler.h"

#include "../wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent) : QGraphicsView(parent), _bMousePressReleaseEventPossible(true) {
    qDebug() << __FUNCTION__ << " (constructor)";

    setMouseTracking(true); //to be able to have mouse event in the scene without the need to press a mouse button

    //AnchorUnderMouse is not perfect:
    //setTransformationAnchor(QGraphicsView::AnchorUnderMouse); //to zoom on the area under the mouse

    //setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::NoAnchor);

    _bPan_onImageArea = false;

    QObject::connect(horizontalScrollBar(), &QScrollBar::sliderReleased,  this, &CustomGraphicsView::slot_sliderReleased);
    QObject::connect(horizontalScrollBar(), &QScrollBar::actionTriggered, this, &CustomGraphicsView::slot_sliderActionTriggered);

    QObject::connect(verticalScrollBar(), &QScrollBar::sliderReleased,  this, &CustomGraphicsView::slot_sliderReleased);
    QObject::connect(verticalScrollBar(), &QScrollBar::actionTriggered, this, &CustomGraphicsView::slot_sliderActionTriggered);

    qDebug() << __FUNCTION__ << "(end of constructor)";

    _bJustCenteredOn = false;


    _bShowPixelGrid = false;
    _bShowPixelGridIsPossible = false; //set through code using ratio pixel computation
}

//the scene is informed to update its side
void CustomGraphicsView::slot_adjustView(qreal scaleValue) {
    //scale on the view side
    qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_adjustView scaleValue = " << scaleValue;
    resetTransform();
    scale(scaleValue, scaleValue);
    qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_adjustView scaleValue emit now signal_visibleAreaChanged";

    emit signal_visibleAreaChanged(mapToScene(viewport()->geometry()).boundingRect());
}

void CustomGraphicsView::slot_sliderActionTriggered(int action) {
    switch (action) {
    case QAbstractSlider::SliderSingleStepAdd:
    case QAbstractSlider::SliderSingleStepSub:
    case QAbstractSlider::SliderPageStepAdd:
    case QAbstractSlider::SliderPageStepSub:
    case QAbstractSlider::SliderToMinimum:
    case QAbstractSlider::SliderToMaximum:
        qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_sliderActionTriggered emit now signal_endOfPan_usingScrollBar";
        emit signal_endOfPan_usingScrollBar();
        break;
    case QAbstractSlider::SliderMove:
    default:
        break;
    }
    qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_sliderActionTriggered emit now signal_visibleAreaChanged";
    emit signal_visibleAreaChanged(mapToScene(viewport()->geometry()).boundingRect());
}

void CustomGraphicsView::slot_sliderReleased() {
     qDebug() << __FUNCTION__;
     qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_sliderReleased emit now signal_endOfPan_usingScrollBar";
     emit signal_endOfPan_usingScrollBar();
}

//update the small nav rect just after a image view window resize using window border
void CustomGraphicsView::slot_endOfResize() {
     qDebug() << __FUNCTION__;
     emit signal_visibleAreaChanged(mapToScene(viewport()->geometry()).boundingRect());
}

void CustomGraphicsView::set_enablePanOnImage(bool bEnable) {

    _bMousePressReleaseEventPossible = bEnable;
    horizontalScrollBar()->setEnabled(bEnable);
    verticalScrollBar()->setEnabled(bEnable);
}

void CustomGraphicsView::slot_setCenterOn(qreal xLoc, qreal yLoc) {
    qDebug() << __FUNCTION__ << "dbg @line " << __LINE__;
    _bJustCenteredOn = true;
    qDebug() << __FUNCTION__ << "with xLoc =" << xLoc << "and yLoc=" << yLoc;
    centerOn({xLoc, yLoc});
    qDebug() << __FUNCTION__ << "CustomGraphicsView::slot_setCenterOn emit now signal_visibleAreaChanged";
    emit signal_visibleAreaChanged(mapToScene(viewport()->geometry()).boundingRect());
}

void CustomGraphicsView::drawForeground(QPainter *painter, const QRectF &rect) {

    QColor fgc(255,128,128);

    QPen penFgrnd;
    penFgrnd.setStyle(Qt::PenStyle::SolidLine);
    penFgrnd.setColor(fgc);

    penFgrnd.setWidthF(.025);

    painter->setPen(penFgrnd);

    if (!scene()) {
        return;
    }

    QSize qsize_screenPixel = viewport()->size();
    if (  (qsize_screenPixel.width()  < 8)
        ||(qsize_screenPixel.height() < 8)) {
        _bShowPixelGridIsPossible = false;
        emit signal_showPixelGridIsPossible(_bShowPixelGridIsPossible);
        return;
    }

    QRectF currentExposedRectF = mapToScene(viewport()->geometry()).boundingRect();
    qreal pixelRatio_screenDivScene_width = static_cast<qreal>(qsize_screenPixel.width()) / currentExposedRectF.width();
    //qreal pixelRatio_screenDivScene_height = static_cast<qreal>(qsize_screenPixel.height() / currentExposedRectF.height();

    qDebug() << __FUNCTION__ << "qsize_screenPixel  = " << qsize_screenPixel;
    qDebug() << __FUNCTION__ << "currentExposedRectF  = " << currentExposedRectF;
    qDebug() << __FUNCTION__ << "pixelRatio_screenDivScene_width  = " << pixelRatio_screenDivScene_width;
    //qDebug() << __FUNCTION__ << "pixelRatio_sceneDivScreen_height = " << pixelRatio_sceneDivScreen_height;

    if (pixelRatio_screenDivScene_width < 16) {
        _bShowPixelGridIsPossible = false;
        emit signal_showPixelGridIsPossible(_bShowPixelGridIsPossible);
        return;
    }

    if (!_bShowPixelGridIsPossible) {
        _bShowPixelGridIsPossible = true;
        emit signal_showPixelGridIsPossible(_bShowPixelGridIsPossible);
    }

    if (!_bShowPixelGrid) {
        return;
    }

    //qreal yMiddle = ( (currentExposedRectF.bottom()+1) + (currentExposedRectF.top() -1) ) /2.0;

    for (int ygrid = static_cast<int>(currentExposedRectF.top())-1;
             ygrid < static_cast<int>(currentExposedRectF.bottom())+1;
             ygrid++) {

        QPointF qpf_start {currentExposedRectF.left(), static_cast<qreal>(ygrid)};
        QPointF qpf_end   {currentExposedRectF.right(), static_cast<qreal>(ygrid)};

        painter->drawLine(qpf_start, qpf_end);
    }

    //qreal xMiddle = ( (currentExposedRectF.right() +1) + (currentExposedRectF.left()-1) ) /2.0;

    for (int xgrid = static_cast<int>(currentExposedRectF.left())-1;
             xgrid < static_cast<int>(currentExposedRectF.right())+1;
             xgrid++) {
        QPointF qpf_start {static_cast<qreal>(xgrid), currentExposedRectF.top()};
        QPointF qpf_end   {static_cast<qreal>(xgrid), currentExposedRectF.bottom()};

        painter->drawLine(qpf_start, qpf_end);
    }

    if (!_bShowPixelGridIsPossible) {
        _bShowPixelGridIsPossible = true;
        emit signal_showPixelGridIsPossible(_bShowPixelGridIsPossible);
        return;
    }

}


void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsView)";

    if (_bPan_onImageArea)  {

            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
            _panStartX = event->x();
            _panStartY = event->y();
            event->accept();            
            qDebug() << __FUNCTION__ << "CustomGraphicsView::mouseMoveEvent emit now signal_visibleAreaChanged";
            emit signal_visibleAreaChanged(mapToScene(viewport()->geometry()).boundingRect());
            return;

    }

    //let the others behaviors working
    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {

    if (!_bMousePressReleaseEventPossible) {
        event->accept();
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsView)";

    if (event->button() == Qt::RightButton) {
        _bPan_onImageArea = true;
        _panStartX = event->x();
        _panStartY = event->y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    //let the others behaviors working
    QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent * event) {

    if (!_bMousePressReleaseEventPossible) {
        event->accept();
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsView)";

    /*if (event->button() == Qt::LeftButton) {
        qDebug() << __FUNCTION__ << "**** mouse loc mapped FromGlobal:" << mapFromGlobal(QCursor::pos());
        qDebug() << __FUNCTION__ << "**** mouse loc mapped to Scene  :" << mapToScene(mapFromGlobal(QCursor::pos()));
    }*/

    if (event->button() == Qt::RightButton) {
        _bPan_onImageArea = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        emit signal_endOfPan_onImageArea();
        return;
    }

    //let the others behaviors working
    QGraphicsView::mouseReleaseEvent(event);
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event) {
    qDebug() << __FUNCTION__ << "-- start --------------";

    event->accept();

    ZoomHandler::e_Zoom_Direction zoomDirIfDoZoom = ZoomHandler::e_Zoom_Direction::e_ZoomDir_In; //just to be set on a value (can be invalid)
    bool bDoZoom = wheelEventZoomInOut_noNoise(event, zoomDirIfDoZoom);
    //bool bDoZoom = wheelEventZoomInOut_noNoise_noFilterSmallEvent(event, zoomDirIfDoZoom);
    if (bDoZoom) {
        emit signal_zoomRequest(zoomDirIfDoZoom);
        _y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        _y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
    }
}

void CustomGraphicsView::slot_showPixelGrid(bool bState) {
    if (!scene()) {
        return;
    }
    _bShowPixelGrid = bState;
    scene()->invalidate(scene()->sceneRect()); //will generate a call to WindowImageView->drawForeground
}
