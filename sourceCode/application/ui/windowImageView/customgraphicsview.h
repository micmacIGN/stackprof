#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

#include "../logic/zoomLevelImage/zoomHandler.h"

#include "../wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h"


class CustomGraphicsScene;

class CustomGraphicsView : public QGraphicsView, WheelEventZoomInOutNoiseHandler {

  Q_OBJECT

public:
    CustomGraphicsView(QWidget *parent = nullptr);

    void set_enablePanOnImage(bool bEnable);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    bool _bPan_onImageArea;
    int _panStartX, _panStartY; //used with _bPan_onImageArea

signals:
    void signal_endOfPan_onImageArea();
    void signal_endOfPan_usingScrollBar();

    void signal_zoomRequest(int i_e_ZoomDir/*, QPointF mouseViewPosMappedToScene*/);

    void signal_mouseScenePositionFromView(QPointF mouseViewPosMappedToScene);

    void signal_visibleAreaChanged(QRectF visibleArea);//to inform small image nav

    void signal_showPixelGridIsPossible(bool bIsPossible);

public slots:

   void slot_sliderReleased();
   void slot_sliderActionTriggered(int action);

   void slot_adjustView(qreal scaleValue);
   void slot_setCenterOn(qreal xLoc, qreal yLoc);

   void slot_endOfResize();

   void slot_showPixelGrid(bool bState);

private:

   //float _fCurrentScale;

   bool _bJustCenteredOn;

   bool _bMousePressReleaseEventPossible;

   bool _bShowPixelGrid; //set through user action
   bool _bShowPixelGridIsPossible; //set through code using ratio pixel computation
};

#endif // CUSTOMGRAPHICSVIEW_H
