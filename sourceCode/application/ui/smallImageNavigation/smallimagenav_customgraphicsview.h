#ifndef smallImageNav_customGraphicsView_H
#define smallImageNav_customGraphicsView_H

#include <QGraphicsView>
#include <QMouseEvent>

#include "../wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h"

class SmallImageNav_customGraphicsView : public QGraphicsView, WheelEventZoomInOutNoiseHandler {

  Q_OBJECT

public:
    SmallImageNav_customGraphicsView(QWidget *parent = nullptr);

signals:
    void signal_zoomRequest_fromSmallImaveNav(int i_e_ZoomDir/*, QPointF mouseViewPosMappedToScene*/);

public slots:

protected:
    void wheelEvent(QWheelEvent *event) override;

private:

};

#endif // smallImageNav_customGraphicsView_H

