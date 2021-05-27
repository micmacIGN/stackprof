#ifndef Dialog_graphicsSettings_H
#define Dialog_graphicsSettings_H

#include <QDialog>

#include <QColorDialog>

namespace Ui {
class Dialog_graphicsSettings;
}

//#include "../../logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h"
#include "../../logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"

class Dialog_graphicsSettings : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_graphicsSettings(QWidget *parent = nullptr);
    ~Dialog_graphicsSettings() override;

    void feed_profil(const S_GraphicsParameters_Profil& sGP_Profil_current);
    void feed_profilFromPreset_cancelable(const S_GraphicsParameters_Profil& sGP_Profil);
    //void feed_itemOnImageView(const S_GraphicsParameters_ItemOnImageView& sGP_itemOnImageView_current);

public slots:
    void slot_profil_curve_antialiasing_use(int iCheckState);
    void slot_envelop_area_antialising_use(int iCheckState);
    void slot_linearReg_computedLines_antialising_use(int iCheckState);

    void slot_spinBox_thickness_valueChanged(int intValue);

    void slot_pushButton_selectColor_clicked();

    void slot_qColorDialog_currentColorChanged(const QColor &color);

    void slot_graphicsSettings_profil_preset_clicked();

    /*
    void slot_itemOnImgView_box_antialiasing_use(int iCheckState);
    void slot_temOnImgView_segment_antialiasing_use(int iCheckState);
    void slot_itemOnImgView_pointInEdition_antialiasing_use(int iCheckState);
    void slot_itemOnImgView_firstAndLastPoint_antialiasing_use(int iCheckState);*/

    void accept() override;
    void reject() override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void signal_graphicsSettings_profil_changed(S_GraphicsParameters_Profil sGP_profil, bool bIsJustForPreview);
    void signal_graphicsSettings_profil_preset_changed(int presetId);

    //void signal_graphicsSettings_itemOnImgView_changed(S_GraphicsParameters_ItemOnImageView sGP_itemOnImgView, bool bIsJustForPreview);

private:

    void setUi_allEmptyAndDisabled();

    void initializePart_profil();
    //void initializePart_ItemOnImageView();

    void syncUiTo_profil(S_GraphicsParameters_Profil *ptr_sGraphicsParameters_Profil);
    //void syncUiTo_itemOnImageView(S_GraphicsParameters_ItemOnImageView *ptr_sGraphicsParameters_itemOnImageView);

    QString buildQString_backgroundStyleSheet(const QColor& qColor);

    enum e_id_pushButtonSelectColor { //@LP used as index to store custom color in QDialogCOlor. Hence starting at 0 is important for the first valid Id (0)
        e_id_pBSC_invalid = -1,
        e_id_pBSC_profil_curve = 0,
        e_id_pBSC_profil_envelop_area,
        e_id_pBSC_linearReg_xAdjusters,
        e_id_pBSC_linearReg_xCentralAxis,
        e_id_pBSC_linearReg_computedLines
    };

    enum e_id_spinbBoxSetThickness { //@LP used as index to store custom color in QDialogCOlor. Hence starting at 0 is important for the first valid Id (0)
        e_id_sBSC_invalid = -1,

        e_id_sBSC_profil_curve = 0,
        e_id_sBSC_linearReg_xAdjusters,
        e_id_sBSC_linearReg_xCentralAxis,
        e_id_sBSC_linearReg_computedLines,

        e_id_sBSC_itemOnImageView_box,
        e_id_sBSC_itemOnImageView_trace_segment,
        e_id_sBSC_itemOnImageView_trace_pointInEdition,
        e_id_sBSC_itemOnImageView_trace_firstAndLastPoint,
    };

private:
    Ui::Dialog_graphicsSettings *ui;

    bool _bFeed_sGPProfil_reference;
    S_GraphicsParameters_Profil _sGP_Profil_reference; //initial parameters received, and restored if the user press the cancel button of the dialog
    S_GraphicsParameters_Profil _sGP_Profil_workingOn; //parameters used if the user press the ok button of the dialog, _sGP_Profil_workingOn will be copyed to _sGP_Profil_reference and saved
    S_GraphicsParameters_Profil _sGP_Profil_colorInEdition; //for the color selection which can be also canceled or accepted by the user.
                                                       //if the user accept the color, _sGP_Profil_inEdition will be copyed to _sGP_Profil_workingOn

    //bool _bFeed_sGPitemOnImgView_reference;
    //S_GraphicsParameters_ItemOnImageView _sGP_itemOnImgView_reference;
    //S_GraphicsParameters_ItemOnImageView _sGP_itemOnImgView_workingOn; //parameters used if the user press the ok button of the dialog, _sGP_Profil_workingOn will be copyed to _sGP_Profil_reference and saved

    QColorDialog _qDialColor;
    e_id_pushButtonSelectColor _eIDpBSC_inColorSelection;
    QHash<int, QColor*> _qhash_int_qcolorPtr_initialForColorDialog;
    QHash<int, double*> _qhash_int_dThicknessPtr;

};

#endif // Dialog_graphicsSettings_H

