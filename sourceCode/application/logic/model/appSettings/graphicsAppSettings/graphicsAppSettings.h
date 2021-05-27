#ifndef GraphicsAppSettingsAppSettings_h
#define GraphicsAppSettingsAppSettings_h

#include <QString>

#include <QSettings>

#include "SGraphicsAppSettings_profil.h"
#include "SGraphicsAppSettings_ItemOnImageView.h"

#include <QColor>

class GraphicsAppSettings {

public:

    GraphicsAppSettings();

    //return false in case of fatal error
    bool load_setMissingToDefaultPreset(bool &bLoadedFromDefault);

    bool save();

    void setFrom_graphicsParameters_Profil(const S_GraphicsParameters_Profil& sGP_Profis);
    void setFrom_graphicsParameters_ItemOnImageView(const S_GraphicsParameters_ItemOnImageView& sGP_ItemOnImageView);

    S_GraphicsParameters_ItemOnImageView get_graphicsParametersItemOnImageView();

    S_GraphicsParameters_Profil get_graphicsParametersProfil();
    S_GraphicsParameters_Profil get_graphicsParametersProfil_preset(int presetID);

private:
    S_GraphicsParameters_Profil _sGP_Profil_current;
    QVector<S_GraphicsParameters_Profil> _qvectSGP_profilPreset;

    S_GraphicsParameters_ItemOnImageView _sGP_ItemOnImageView_current;

private:
    //bool readAppSettingsToQVariant(const QSettings &settings, QString strKey, QVariant& qvariantValueRead, bool bShouldBeQColor);

private:

    //for storage:
    //
    static constexpr const char* _strCompanyName = "ipgp_fr";
    static constexpr const char* _strAppName     = "stackp_settings";
    static constexpr const char* _strSectionName = "AppGraphicsParameters";

    void feed_qvectSGP_profilPreset();
    S_GraphicsParameters_ItemOnImageView get_sGP_ItemOnImageView_default();

    static const int _defaultIdxPreset = 1;

    static constexpr const char* _strKey_profil = "graphicsParameters_profil";//('profil' is about the items in the profil curve graphic in edition)

    static constexpr const char* _strKey_itemOnImageView = "graphicsParameters_itemOnImageView";

};

#endif //

