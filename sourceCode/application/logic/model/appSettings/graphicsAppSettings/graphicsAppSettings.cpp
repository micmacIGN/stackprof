#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDir>

#include <QColor>

#include <QDebug>

#include "graphicsAppSettings.h"



#include <QMetaType>

GraphicsAppSettings::GraphicsAppSettings() {

    feed_qvectSGP_profilPreset();

    _sGP_Profil_current.clear();
    _sGP_ItemOnImageView_current.clear();
}

//from ui
void GraphicsAppSettings::setFrom_graphicsParameters_Profil(const S_GraphicsParameters_Profil& sGP_profil) {
    _sGP_Profil_current = sGP_profil;
    _sGP_Profil_current.debugShow_sgpp();
}

S_GraphicsParameters_Profil GraphicsAppSettings::get_graphicsParametersProfil() {
    return(_sGP_Profil_current);
}

void GraphicsAppSettings::setFrom_graphicsParameters_ItemOnImageView(const S_GraphicsParameters_ItemOnImageView& sGP_ItemOnImageView) {
    qDebug() << __FUNCTION__;
    sGP_ItemOnImageView.debugShow_sgpp();
    _sGP_ItemOnImageView_current = sGP_ItemOnImageView;
    _sGP_ItemOnImageView_current.debugShow_sgpp();
}

S_GraphicsParameters_ItemOnImageView GraphicsAppSettings::get_graphicsParametersItemOnImageView() {
    _sGP_ItemOnImageView_current.debugShow_sgpp();
    return(_sGP_ItemOnImageView_current);
}

S_GraphicsParameters_Profil GraphicsAppSettings::get_graphicsParametersProfil_preset(int presetID) {

    if (  (presetID < 0)
        ||(presetID >= _qvectSGP_profilPreset.count())) {

        //thin with antialiasing
        S_GraphicsParameters_Profil sGP_profil_Thickness1 {
            {
                { Qt::red, 1.0 , true}, //_sCTA_profilCurve
                { QColor(210,210,210), true} //_sCA_enveloppArea
            },
            {
                {Qt::darkGreen, 1.0},//_sCT_xAdjuster
                {QColor(  0,102,204), 1.0},//_sCT_xCentralAxis
                {QColor(  0, 81,255), 1.0, true} //_sCTA_computedLine
            }
        };
        return(sGP_profil_Thickness1);
    }
    return(_qvectSGP_profilPreset[presetID]);
}

void GraphicsAppSettings::feed_qvectSGP_profilPreset() {

    _qvectSGP_profilPreset.clear();














    //thin without antialiasing
    S_GraphicsParameters_Profil sGP_profil_Thin {
        {
            { Qt::red, 1.0 , false}, //_sCTA_profilCurve
            { QColor(210,210,210), false} //_sCA_enveloppArea
        },
        {
            {Qt::darkGreen, 1.0},//_sCT_xAdjuster
            {QColor(  0,102,204), 1.0},//_sCT_xCentralAxis
            {QColor(  0, 81,255), 1.0, false} //_sCTA_computedLine
        }
    };

    //thin with antialiasing
    S_GraphicsParameters_Profil sGP_profil_Thickness1 {
        {
            { Qt::red, 1.0 , true}, //_sCTA_profilCurve
            { QColor(210,210,210), true} //_sCA_enveloppArea
        },
        {
            {Qt::darkGreen, 1.0},//_sCT_xAdjuster
            {QColor(  0,102,204), 1.0},//_sCT_xCentralAxis
            {QColor(  0, 81,255), 1.0, true} //_sCTA_computedLine
        }
    };

    //thicknes at 2.0 with antialiasing
    S_GraphicsParameters_Profil sGP_profil_Thickness2 {
        {
            { Qt::red, 2.0 , true}, //_sCTA_profilCurve
            { QColor(210,210,210), true} //_sCA_enveloppArea
        },
        {
            {Qt::darkGreen, 2.0},//_sCT_xAdjuster
            {QColor(  0,102,204), 2.0},//_sCT_xCentralAxis
            {QColor(  0, 81,255), 2.0, true} //_sCTA_computedLine
        }
    };

    _qvectSGP_profilPreset.push_back(sGP_profil_Thin);
    _qvectSGP_profilPreset.push_back(sGP_profil_Thickness1);
    _qvectSGP_profilPreset.push_back(sGP_profil_Thickness2);
}

S_GraphicsParameters_ItemOnImageView GraphicsAppSettings::get_sGP_ItemOnImageView_default() {
    return(S_GraphicsParameters_ItemOnImageView{
        { 1.0, false },
        { 1.0, false },
        { 1.0, false },
        { 1.0, false },
    });
}

bool GraphicsAppSettings::load_setMissingToDefaultPreset(bool &bLoadedFromDefault) {

    bLoadedFromDefault = false;

    //prevent dev-error
    if (_qvectSGP_profilPreset.count() < (_defaultIdxPreset+1)) {
        qDebug() << __FUNCTION__ << "internal dev-error: hard coded defaults values not loaded for GraphicsAppSettings ?";
        return(false);
    }

    QSettings settings(_strCompanyName, _strAppName);

    settings.beginGroup(_strSectionName);

    qRegisterMetaTypeStreamOperators<S_GraphicsParameters_Profil>("S_GraphicsParameters_Profil");
    qRegisterMetaTypeStreamOperators<S_GraphicsParameters_ItemOnImageView>("S_GraphicsParameters_ItemOnImageView");

    //profil
    {
        bool bNoneError = true;

        qRegisterMetaTypeStreamOperators<S_GraphicsParameters_Profil>("S_GraphicsParameters_Profil");

        QVariant qvariantValueRead = settings.value(_strKey_profil);

        qDebug() << __FUNCTION__ << __LINE__;

        qDebug() << __FUNCTION__ << qvariantValueRead;

        if (!qvariantValueRead.isValid()) {
            qDebug() << __FUNCTION__ << "if (!qvariantValueRead.isValid()) {";
            bNoneError = false;
        } else {
            if (qvariantValueRead.isNull()) {
                qDebug() << __FUNCTION__ << "if (qvariantValueRead.isNull()) {";
                bNoneError = false;
            } else {
                qDebug() << __FUNCTION__ << __LINE__;
               _sGP_Profil_current = qvariantValueRead.value<S_GraphicsParameters_Profil>();
               _sGP_Profil_current.debugShow_sgpp();
            }
        }

        if (!bNoneError) {
            qDebug() << __FUNCTION__ << "=> uses default preset #" << _defaultIdxPreset;
            _sGP_Profil_current = _qvectSGP_profilPreset.at(_defaultIdxPreset);
            bLoadedFromDefault = true;
        }

        _sGP_Profil_current.debugShow_sgpp();
    }


    //ItemOnImageView
    {
        bool bNoneError = true;

        QVariant qvariantValueRead_itemOnImageView = settings.value(_strKey_itemOnImageView);

        qDebug() << __FUNCTION__ << qvariantValueRead_itemOnImageView;

        if (!qvariantValueRead_itemOnImageView.isValid()) {
            qDebug() << __FUNCTION__ << "if (!qvariantValueRead_itemOnImageView.isValid()) {";
            bNoneError = false;
        } else {
            if (qvariantValueRead_itemOnImageView.isNull()) {
                qDebug() << __FUNCTION__ << "if (qvariantValueRead_itemOnImageView.isNull()) {";
                bNoneError = false;
            } else {
                qDebug() << __FUNCTION__ << __LINE__;
               _sGP_ItemOnImageView_current = qvariantValueRead_itemOnImageView.value<S_GraphicsParameters_ItemOnImageView>();
               _sGP_ItemOnImageView_current.debugShow_sgpp();
            }
        }

        if (!bNoneError) {
            qDebug() << __FUNCTION__ << "=> uses default preset";
            _sGP_ItemOnImageView_current = get_sGP_ItemOnImageView_default();
            _sGP_ItemOnImageView_current.debugShow_sgpp();
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << " (end) ";

    return(true);
}


bool GraphicsAppSettings::save() {

    qDebug() << __FUNCTION__ << "(GraphicsAppSettings)";

    QSettings settings(_strCompanyName, _strAppName);

    settings.beginGroup(_strSectionName);


    {//-------------
        _sGP_Profil_current.debugShow_sgpp();

        qRegisterMetaTypeStreamOperators<S_GraphicsParameters_Profil>("S_GraphicsParameters_Profil");

        QVariant qVariantToStore;
        qVariantToStore.setValue(_sGP_Profil_current);

        //qDebug() << __FUNCTION__ << "(GraphicsAppSettings) _sGP_Profil_current => qVariantToStore = " << qVariantToStore;

        settings.setValue(_strKey_profil, qVariantToStore);
    }


    {//-------------
        _sGP_ItemOnImageView_current.debugShow_sgpp();

        qRegisterMetaTypeStreamOperators<S_GraphicsParameters_ItemOnImageView>("S_GraphicsParameters_ItemOnImageView");

        QVariant qVariantToStore;
        qVariantToStore.setValue(_sGP_ItemOnImageView_current);

        //qDebug() << __FUNCTION__ << "(GraphicsAppSettings) _sGP_ItemOnImageView_current => qVariantToStore = " << qVariantToStore;

        settings.setValue(_strKey_itemOnImageView, qVariantToStore);
    }

    settings.endGroup();

    settings.sync();
    return(true);
}

