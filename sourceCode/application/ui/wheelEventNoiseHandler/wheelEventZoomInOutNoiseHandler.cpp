#include "wheelEventZoomInOutNoiseHandler.h"

#include <QDebug>

#include <QDateTime>

#include <QWheelEvent>

#include "../logic/zoomLevelImage/zoomHandler.h"

WheelEventZoomInOutNoiseHandler::WheelEventZoomInOutNoiseHandler():
    _previous_eZoomDirection {ZoomHandler::e_ZoomDir_In},
    _bPrevious_eZoomDirection_set { false },
    _y_cumul_steps_from_degrees { 0 },
    _y_cumul_pixels { 0 } {

    qDebug() << __FUNCTION__ << "-- constructor --";
}

//#TagToFindAboutReleaseDeliveryOption
//#define DONT_USE_QDBG_USE_COUT_INSTEAD_WHEELLEVENT
#ifndef DONT_USE_QDBG_USE_COUT_INSTEAD_WHEELLEVENT
#define useQDebug_we
#endif
#ifdef useQDebug_we
    #include <QDebug>
    #define debugOut_wE qDebug()
    #define debugEndl_wE ""
#else
#include <iostream>
using namespace std;
    #define debugOut_wE cout
    #define debugEndl_wE endl
#endif


//no accept(), no signal emited from this class
//just handle the noise
bool WheelEventZoomInOutNoiseHandler::wheelEventZoomInOut_noNoise(QWheelEvent *event, ZoomHandler::e_Zoom_Direction& zoomDirIfDoZoom) {
  
    qDebug() << __FUNCTION__ << "-- start --------------";

    //@#LP move to dedicated not static structure/field
    static qint64 msecPrevious = 0;
    //static bool bMsPreviousSet = false;
    qint64 msecFrom1970_toCurrent = QDateTime::currentMSecsSinceEpoch();
    qint64 msDeltatFromPrevious = msecFrom1970_toCurrent - msecPrevious;
    //if (bMsPreviousSet) {
        debugOut_wE << __FUNCTION__ << "time since last call: " << msDeltatFromPrevious << debugEndl_wE;
    //}
    msecPrevious = msecFrom1970_toCurrent;
    //bMsPreviousSet = true;

    bool bCumulReachedFor_zoomIn = false;
    bool bCumulReachedFor_zoomOut = false;

    ZoomHandler::e_Zoom_Direction eZoomDirection {ZoomHandler::e_ZoomDir_In}; //zoom_In by default

    {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        if (!numPixels.isNull()) {

            debugOut_wE << __FUNCTION__ << "scrollWithPixels numPixels.y() = " << numPixels.y() << debugEndl_wE;

            if (numPixels.y() < 0) {
                eZoomDirection = ZoomHandler::e_ZoomDir_Out;
            }

            if (!_bPrevious_eZoomDirection_set) {
                _y_cumul_pixels = numPixels.y();
                _previous_eZoomDirection = eZoomDirection;
                _bPrevious_eZoomDirection_set = true;
        } else {
                if (eZoomDirection == _previous_eZoomDirection) {
                    _y_cumul_pixels += numPixels.y();
                } else {
                    if (msDeltatFromPrevious < 150) {
                        debugOut_wE << __FUNCTION__ << "(numPixels) msDeltatFromPrevious = " << msDeltatFromPrevious << " => reject event" << debugEndl_wE;
                        return(false);
                    }
                    _y_cumul_pixels = numPixels.y();
                    _previous_eZoomDirection = eZoomDirection;
                }
            }

        } else {

            if (!numDegrees.isNull()) {

                _y_cumul_pixels = 0;

                //debugOut_wE << __FUNCTION__ << "numDegrees.y() = " << numDegrees.y()  << debugEndl_wE;

                QPoint numSteps = numDegrees / 15;

                debugOut_wE << __FUNCTION__ << "scrollWithDegrees numSteps.y() = " << numSteps.y() << debugEndl_wE;

                if (numSteps.y() < 0) {
                    eZoomDirection = ZoomHandler::e_ZoomDir_Out;
                }

                if (!_bPrevious_eZoomDirection_set) {
                    _y_cumul_steps_from_degrees = numSteps.y();
                    _previous_eZoomDirection = eZoomDirection;
                    _bPrevious_eZoomDirection_set = true;
                } else {
                    if (eZoomDirection == _previous_eZoomDirection) {
                        _y_cumul_steps_from_degrees += numSteps.y();
                    } else {
                        if (msDeltatFromPrevious < 150) {
                            debugOut_wE << __FUNCTION__ << "(numDegrees) msDeltatFromPrevious = " << msDeltatFromPrevious << " => reject event" << debugEndl_wE;
                            return(false);
                        }
                        _y_cumul_steps_from_degrees = numSteps.y();
                        _previous_eZoomDirection = eZoomDirection;
                    }
                }

            } else {
                _y_cumul_steps_from_degrees = 0;
            }
        }

    }

    debugOut_wE << __FUNCTION__ << "y_cumul_pixels = " << _y_cumul_pixels << debugEndl_wE;
    debugOut_wE << __FUNCTION__ << "y_cumul_steps_from_degrees = " << _y_cumul_steps_from_degrees << debugEndl_wE;

    if (_y_cumul_pixels > 32) { //@##LP totally arbitrary value, not tested
        bCumulReachedFor_zoomIn = true;
        _y_cumul_pixels = 0;
    }
    if (_y_cumul_pixels < -32) { //@##LP totally arbitrary value, not tested
        bCumulReachedFor_zoomOut = true;
        _y_cumul_pixels = 0;
    }

    if (  (!bCumulReachedFor_zoomIn)
        &&(!bCumulReachedFor_zoomOut)) {

        if (_y_cumul_steps_from_degrees >= 1) {
            bCumulReachedFor_zoomIn = true;
            _y_cumul_steps_from_degrees = 0;
        }
        if (_y_cumul_steps_from_degrees <= -1) {
            bCumulReachedFor_zoomOut = true;
            _y_cumul_steps_from_degrees = 0;
        }
    }

    if (  (!bCumulReachedFor_zoomIn)
        &&(!bCumulReachedFor_zoomOut)) {
        return(false);
    }

    bool bDoZoom = false;

    if (bCumulReachedFor_zoomOut) {
        bDoZoom = true;
        zoomDirIfDoZoom = ZoomHandler::e_ZoomDir_Out;
        debugOut_wE << __FUNCTION__ << "e_ZoomDir_Out" << debugEndl_wE;
        //emit signal_zoomRequest(ZoomHandler::e_ZoomDir_Out/*, mouseViewPosMappedToScene*/);
        //_y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        //_y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
    } else {
        if (bCumulReachedFor_zoomIn) {
            bDoZoom = true;
            zoomDirIfDoZoom = ZoomHandler::e_ZoomDir_In;
            debugOut_wE << __FUNCTION__ << "e_ZoomDir_In" << debugEndl_wE;
            //emit signal_zoomRequest(ZoomHandler::e_ZoomDir_In/*, mouseViewPosMappedToScene*/);
            //_y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
            //_y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        }
    }

    debugOut_wE << __FUNCTION__ << "-- end with bDoZoom:" << bDoZoom << debugEndl_wE;
    return(bDoZoom);
}



//no accept(), no signal emited from this class
//just handle the noise
bool WheelEventZoomInOutNoiseHandler::wheelEventZoomInOut_noNoise_noFilterSmallEvent(QWheelEvent *event, ZoomHandler::e_Zoom_Direction& zoomDirIfDoZoom) {

    qDebug() << __FUNCTION__ << "-- start --------------";
/*
    //@#LP move to dedicated not static structure/field
    static qint64 msecPrevious = 0;
    //static bool bMsPreviousSet = false;
    qint64 msecFrom1970_toCurrent = QDateTime::currentMSecsSinceEpoch();
    qint64 msDeltatFromPrevious = msecFrom1970_toCurrent - msecPrevious;
    //if (bMsPreviousSet) {
        debugOut_wE << __FUNCTION__ << "time since last call: " << msDeltatFromPrevious << debugEndl_wE;
    //}
    msecPrevious = msecFrom1970_toCurrent;
    //bMsPreviousSet = true;
*/


    bool bCumulReachedFor_zoomIn = false;
    bool bCumulReachedFor_zoomOut = false;

    ZoomHandler::e_Zoom_Direction eZoomDirection {ZoomHandler::e_ZoomDir_In}; //zoom_In by default

    {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        if (!numPixels.isNull()) {

            debugOut_wE << __FUNCTION__ << "scrollWithPixels numPixels.y() = " << numPixels.y() << debugEndl_wE;

            if (numPixels.y() < 0) {
                eZoomDirection = ZoomHandler::e_ZoomDir_Out;
            }

            if (!_bPrevious_eZoomDirection_set) {
                _y_cumul_pixels = numPixels.y();
                _previous_eZoomDirection = eZoomDirection;
                _bPrevious_eZoomDirection_set = true;
            } else {
                if (eZoomDirection == _previous_eZoomDirection) {
                    _y_cumul_pixels += numPixels.y();
                } else {
                    /*if (msDeltatFromPrevious < 150) {
                        debugOut_wE << __FUNCTION__ << "(numPixels) msDeltatFromPrevious = " << msDeltatFromPrevious << " => reject event" << debugEndl_wE;
                        return(false);
                    }*/
                    _y_cumul_pixels = numPixels.y();
                    _previous_eZoomDirection = eZoomDirection;
                }
            }

        } else {

            if (!numDegrees.isNull()) {

                _y_cumul_pixels = 0;

                //debugOut_wE << __FUNCTION__ << "numDegrees.y() = " << numDegrees.y()  << debugEndl_wE;

                QPoint numSteps = numDegrees / 15;

                debugOut_wE << __FUNCTION__ << "scrollWithDegrees numSteps.y() = " << numSteps.y() << debugEndl_wE;

                if (numSteps.y() < 0) {
                    eZoomDirection = ZoomHandler::e_ZoomDir_Out;
                }

                if (!_bPrevious_eZoomDirection_set) {
                    _y_cumul_steps_from_degrees = numSteps.y();
                    _previous_eZoomDirection = eZoomDirection;
                    _bPrevious_eZoomDirection_set = true;
                } else {
                    if (eZoomDirection == _previous_eZoomDirection) {
                        _y_cumul_steps_from_degrees += numSteps.y();
                    } else {
                        /*if (msDeltatFromPrevious < 150) {
                            debugOut_wE << __FUNCTION__ << "(numDegrees) msDeltatFromPrevious = " << msDeltatFromPrevious << " => reject event" << debugEndl_wE;
                            return(false);
                        }*/
                        _y_cumul_steps_from_degrees = numSteps.y();
                        _previous_eZoomDirection = eZoomDirection;
                    }
                }

            } else {
                _y_cumul_steps_from_degrees = 0;
            }
        }

    }

    debugOut_wE << __FUNCTION__ << "y_cumul_pixels = " << _y_cumul_pixels << debugEndl_wE;
    debugOut_wE << __FUNCTION__ << "y_cumul_steps_from_degrees = " << _y_cumul_steps_from_degrees << debugEndl_wE;

    if (_y_cumul_pixels > 32) { //@##LP totally arbitrary value, not tested
        bCumulReachedFor_zoomIn = true;
        _y_cumul_pixels = 0;
    }
    if (_y_cumul_pixels < -32) { //@##LP totally arbitrary value, not tested
        bCumulReachedFor_zoomOut = true;
        _y_cumul_pixels = 0;
    }

    if (  (!bCumulReachedFor_zoomIn)
        &&(!bCumulReachedFor_zoomOut)) {

        if (_y_cumul_steps_from_degrees >= 1) {
            bCumulReachedFor_zoomIn = true;
            _y_cumul_steps_from_degrees = 0;
        }
        if (_y_cumul_steps_from_degrees <= -1) {
            bCumulReachedFor_zoomOut = true;
            _y_cumul_steps_from_degrees = 0;
        }
    }

    if (  (!bCumulReachedFor_zoomIn)
        &&(!bCumulReachedFor_zoomOut)) {
        return(false);
    }

    bool bDoZoom = false;

    if (bCumulReachedFor_zoomOut) {
        bDoZoom = true;
        zoomDirIfDoZoom = ZoomHandler::e_ZoomDir_Out;
        debugOut_wE << __FUNCTION__ << "e_ZoomDir_Out" << debugEndl_wE;
        //emit signal_zoomRequest(ZoomHandler::e_ZoomDir_Out/*, mouseViewPosMappedToScene*/);
        //_y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        //_y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
    } else {
        if (bCumulReachedFor_zoomIn) {
            bDoZoom = true;
            zoomDirIfDoZoom = ZoomHandler::e_ZoomDir_In;
            debugOut_wE << __FUNCTION__ << "e_ZoomDir_In" << debugEndl_wE;
            //emit signal_zoomRequest(ZoomHandler::e_ZoomDir_In/*, mouseViewPosMappedToScene*/);
            //_y_cumul_pixels = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
            //_y_cumul_steps_from_degrees = 0; //@LP trying to avoid some more eratic mouse zoomin/zoomout
        }
    }

    debugOut_wE << __FUNCTION__ << "-- end with bDoZoom:" << bDoZoom << debugEndl_wE;
    return(bDoZoom);
}

