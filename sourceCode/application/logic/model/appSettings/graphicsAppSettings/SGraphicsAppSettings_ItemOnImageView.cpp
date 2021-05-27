#include <QDebug>

#include <QString>

#include "SGraphicsAppSettings_baseStruct.h"
#include "SGraphicsAppSettings_ItemOnImageView.h"

void S_GraphicsParameters_ItemOnImageView::debugShow_sgpp() const {

    qDebug() << __FUNCTION__<< "_sTA_box._dThickness   : " << _sTA_box._dThickness;
    qDebug() << __FUNCTION__<< "_sTA_box._bAntialiasing: " << _sTA_box._bAntialiasing;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "_sTA_trace_point._dThickness   : " << _sTA_trace_point._dThickness;
    qDebug() << __FUNCTION__<< "_sTA_trace_point._bAntialiasing: " << _sTA_trace_point._bAntialiasing;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "_sTA_trace_segment._dThickness   : " << _sTA_trace_segment._dThickness;
    qDebug() << __FUNCTION__<< "_sTA_trace_segment._bAntialiasing: " << _sTA_trace_segment._bAntialiasing;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "_sTA_trace_firstAndLastPoint._dThickness   : " << _sTA_trace_firstAndLastPoint._dThickness;
    qDebug() << __FUNCTION__<< "_sTA_trace_firstAndLastPoint._bAntialiasing: " << _sTA_trace_firstAndLastPoint._bAntialiasing;
}

void S_GraphicsParameters_ItemOnImageView::clear() {
    _sTA_box.clear();
    _sTA_trace_segment.clear();
    _sTA_trace_point.clear();
    _sTA_trace_firstAndLastPoint.clear();
}
