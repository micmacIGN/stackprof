#ifndef S_GraphicsAppSettings_profil_h
#define S_GraphicsAppSettings_profil_h

#include <QColor>
#include <QMetaType>

#include <QVector>

#include <QDataStream>

#include "SGraphicsAppSettings_baseStruct.h"

struct S_GraphicsParameters_ProfilCurveAndEnvelopArea {
    S_Color_Thickness_Antialiasing _sCTA_profilCurve;
    S_Color_Antialiasing _sCA_enveloppArea;

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_GraphicsParameters_ProfilCurveAndEnvelopArea & object) {
         arch << object._sCA_enveloppArea;
         arch << object._sCTA_profilCurve;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_GraphicsParameters_ProfilCurveAndEnvelopArea & object) {
         arch >> object._sCA_enveloppArea;
         arch >> object._sCTA_profilCurve;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_GraphicsParameters_ProfilCurveAndEnvelopArea);

struct S_GraphicsParameters_LinearRegression {
    S_Color_Thickness _sCT_xAdjuster;
    S_Color_Thickness _sCT_xCentralAxis;
    S_Color_Thickness_Antialiasing _sCTA_computedLine;

    void clear();

    friend QDataStream & operator << (QDataStream &arch, const S_GraphicsParameters_LinearRegression & object) {
         arch << object._sCT_xAdjuster;
         arch << object._sCT_xCentralAxis;
         arch << object._sCTA_computedLine;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_GraphicsParameters_LinearRegression & object) {
         arch >> object._sCT_xAdjuster;
         arch >> object._sCT_xCentralAxis;
         arch >> object._sCTA_computedLine;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_GraphicsParameters_LinearRegression);


struct S_GraphicsParameters_Profil {
    S_GraphicsParameters_ProfilCurveAndEnvelopArea _sGP_PCurveAndEnvelop;
    S_GraphicsParameters_LinearRegression _sGP_LinearRegression;

    void clear();
    void debugShow_sgpp() const;

    friend QDataStream & operator << (QDataStream &arch, const S_GraphicsParameters_Profil & object) {
         arch << object._sGP_PCurveAndEnvelop;
         arch << object._sGP_LinearRegression;
         return arch;
     }

     friend QDataStream & operator >> (QDataStream &arch, S_GraphicsParameters_Profil & object) {
         arch >> object._sGP_PCurveAndEnvelop;
         arch >> object._sGP_LinearRegression;
         return arch;
     }
};
Q_DECLARE_METATYPE(S_GraphicsParameters_Profil);

#endif // S_GraphicsAppSettings_profil_h



