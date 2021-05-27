#ifndef wheelEventZoomInOutNoiseHandler_H
#define wheelEventZoomInOutNoiseHandler_H

#include <QDebug>

#include <QWheelEvent>

#include "../logic/zoomLevelImage/zoomHandler.h"

class WheelEventZoomInOutNoiseHandler {

public:
    WheelEventZoomInOutNoiseHandler();

protected:
    bool wheelEventZoomInOut_noNoise(QWheelEvent *event, ZoomHandler::e_Zoom_Direction& zoomDirIfDoZoom);
    bool wheelEventZoomInOut_noNoise_noFilterSmallEvent(QWheelEvent *event, ZoomHandler::e_Zoom_Direction& zoomDirIfDoZoom);

protected:
    ZoomHandler::e_Zoom_Direction _previous_eZoomDirection;
    bool _bPrevious_eZoomDirection_set;
    int _y_cumul_steps_from_degrees;
    int _y_cumul_pixels;

};

#endif //wheelEventZoomInOutNoiseHandler_H

