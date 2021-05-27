#ifndef DIALOG_GEOREFINFOS_H
#define DIALOG_GEOREFINFOS_H

#include <QDialog>

namespace Ui {
    class Dialog_georefinfos;
}

enum e_geoRefInfos_uiEditMode {
    e_gRIUIEditMode_newTraceSetOrProject,
    e_gRIUIEditMode_changeEPSGOnOpenedValidTraceSet,
    e_gRIUIEditMode_viewOnlyOnOpenedValidProject
};

class Dialog_geoRefInfos : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog_geoRefInfos(/*const QString& strDialogTitle, */QWidget *parent = nullptr);
    //explicit Dialog_geoRefInfos(QWidget *parent = nullptr);

    ~Dialog_geoRefInfos();

    void setEditMode(e_geoRefInfos_uiEditMode eGeoRefInfos_uiEditMode);

    void clearUiContent_all_backToDefaultState();

    void setEPSGCodeAsEditedText_UserValidationRequiered(const QString& strEPSGCode);

    //debug method:
    //void showSelectedFiles();

public slots:
    void slot_setWorldFileDataFromStr(QVector<QString> qvectstr_worldFileParameters);
    void slot_setEPSGCodeFromStr(QString strEPSGCode);
    void slot_setImagesFilename(QVector<QString> qvectstr_ImageFilenames_threeMax);

    void slot_setEPSGCodeReceivedFromWidgetGeoRefInfos(QString strEPSGCode);

    void slot_setStrGeoRefImagesSetStatus(
      bool bWorldFileData_available, bool bEPSG_available,
      QString strMsgAboutTFW,  QString strMsgAboutTFW_errorDetails,
      QString strMsgAboutEPSG, QString strMsgAboutEPSG_errorDetails);
    void slot_disablePushButtonOk();

    virtual void closeEvent(QCloseEvent *event); //@LP about close window button typically
    virtual void reject(); //@LP about escape key typically

    void slot_request_EPSGCodeSelectionFromlist();

    void slot_associatedNameToEPSGCode(QString strAssociatedNameToEPSGCode);

signals:
    void signal_setEPSGCode(QString strEPSGCode);
    void signal_request_EPSGCodeSelectionFromlist();

    //void signal_EPSGCodeSelectedFromListQString strEPSGCode);

private:

    Ui::Dialog_georefinfos *ui;

    e_geoRefInfos_uiEditMode _eGeoRefInfos_uiEditMode;

    bool _bEnableState_pushButtonOK;

};

#endif // DIALOG_GEOREFINFOS_H
