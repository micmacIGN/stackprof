#include <QDebug>

#include <QString>

#include "SGraphicsAppSettings_baseStruct.h"
#include "SGraphicsAppSettings_profil.h"

void S_GraphicsParameters_Profil::debugShow_sgpp() const {
    qDebug() << __FUNCTION__<< "profilCurve._qColor        : " << _sGP_PCurveAndEnvelop._sCTA_profilCurve._qColor;
    qDebug() << __FUNCTION__<< "profilCurve._dThickness    : " << _sGP_PCurveAndEnvelop._sCTA_profilCurve._dThickness;
    qDebug() << __FUNCTION__<< "profilCurve._bAntialiasing : " << _sGP_PCurveAndEnvelop._sCTA_profilCurve._bAntialiasing;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "enveloppArea._qColor       : " <<_sGP_PCurveAndEnvelop._sCA_enveloppArea._qColor;
    qDebug() << __FUNCTION__<< "enveloppArea._bAntialiasing: " <<_sGP_PCurveAndEnvelop._sCA_enveloppArea._bAntialiasing;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "linearRegression.xAdjuster._qColor     : " << _sGP_LinearRegression._sCT_xAdjuster._qColor;
    qDebug() << __FUNCTION__<< "linearRegression.xAdjuster._dThickness : " << _sGP_LinearRegression._sCT_xAdjuster._dThickness;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "linearRegression.xCentralAxis._qColor     : " << _sGP_LinearRegression._sCT_xCentralAxis._qColor;
    qDebug() << __FUNCTION__<< "linearRegression.xCentralAxis._dThickness : " << _sGP_LinearRegression._sCT_xCentralAxis._dThickness;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__<< "linearRegression.computedLine._qColor :" << _sGP_LinearRegression._sCTA_computedLine._qColor;
    qDebug() << __FUNCTION__<< "linearRegression.computedLine._dThickness :" << _sGP_LinearRegression._sCTA_computedLine._dThickness;
    qDebug() << __FUNCTION__<< "linearRegression.computedLine._bAntialising :" << _sGP_LinearRegression._sCTA_computedLine._bAntialiasing;
}

void S_GraphicsParameters_ProfilCurveAndEnvelopArea::clear() {
    _sCTA_profilCurve.clear();
    _sCA_enveloppArea.clear();
}

void S_GraphicsParameters_LinearRegression::clear() {
    _sCT_xAdjuster.clear();
    _sCT_xCentralAxis.clear();
    _sCTA_computedLine.clear();
}

void S_GraphicsParameters_Profil::clear() {
    _sGP_PCurveAndEnvelop.clear();
    _sGP_LinearRegression.clear();
}
