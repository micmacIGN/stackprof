#ifndef S_GraphicsAppSettings_baseStruct_h
#define S_GraphicsAppSettings_baseStruct_h

#include <QColor>
#include <QMetaType>

#include <QVector>

#include <QDataStream>

struct S_Color_Thickness {
    QColor _qColor;
    double _dThickness;

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_Color_Thickness & object) {
         arch << object._qColor;
         arch << object._dThickness;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_Color_Thickness & object) {
         arch >> object._qColor;
         arch >> object._dThickness;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_Color_Thickness);


struct S_Color_Thickness_Antialiasing {
    QColor _qColor;
    double _dThickness;
    bool _bAntialiasing;

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_Color_Thickness_Antialiasing & object) {
         arch << object._qColor;
         arch << object._dThickness;
         arch << object._bAntialiasing;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_Color_Thickness_Antialiasing & object) {
         arch >> object._qColor;
         arch >> object._dThickness;
         arch >> object._bAntialiasing;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_Color_Thickness_Antialiasing);

struct S_Color_Antialiasing {
    QColor _qColor;
    bool _bAntialiasing;

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_Color_Antialiasing & object) {
         arch << object._qColor;
         arch << object._bAntialiasing;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_Color_Antialiasing & object) {
         arch >> object._qColor;
         arch >> object._bAntialiasing;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_Color_Antialiasing);


struct S_Thickness_Antialiasing {
    double _dThickness;
    bool _bAntialiasing;  

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_Thickness_Antialiasing & object) {
         arch << object._dThickness;
         arch << object._bAntialiasing;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_Thickness_Antialiasing & object) {
         arch >> object._dThickness;
         arch >> object._bAntialiasing;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_Thickness_Antialiasing);

#endif // S_GraphicsAppSettings_baseStruct_h



