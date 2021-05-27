#ifndef S_GraphicsAppSettings_ItemOnImageView_h
#define S_GraphicsAppSettings_ItemOnImageView_h

#include <QColor>
#include <QMetaType>

#include <QVector>

#include <QDataStream>

#include "SGraphicsAppSettings_baseStruct.h"

struct S_GraphicsParameters_ItemOnImageView {

    S_Thickness_Antialiasing _sTA_box;
    S_Thickness_Antialiasing _sTA_trace_segment;
    S_Thickness_Antialiasing _sTA_trace_point;
    S_Thickness_Antialiasing _sTA_trace_firstAndLastPoint;

    void clear();
    void debugShow_sgpp() const;

    friend QDataStream & operator << (QDataStream &arch, const S_GraphicsParameters_ItemOnImageView & object) {
         arch << object._sTA_box;
         arch << object._sTA_trace_segment;
         arch << object._sTA_trace_point;
         arch << object._sTA_trace_firstAndLastPoint;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_GraphicsParameters_ItemOnImageView & object) {
         arch >> object._sTA_box;
         arch >> object._sTA_trace_segment;
         arch >> object._sTA_trace_point;
         arch >> object._sTA_trace_firstAndLastPoint;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_GraphicsParameters_ItemOnImageView);


#endif // S_GraphicsAppSettings_ItemOnImageView_h



