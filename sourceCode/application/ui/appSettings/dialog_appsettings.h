#ifndef DIALOG_APPSETTINGS_H
#define DIALOG_APPSETTINGS_H

#include <QDialog>

#include "../logic/model/appSettings/appSettings.h"

namespace Ui {
class Dialog_AppSettings;
}

class Dialog_AppSettings : public QDialog
{
    Q_OBJECT

public:
    enum e_Tab { //@LP must be sync with tabWidget
        e_T_DiskStoragePath = 0,
        e_T_ImageView_RAMOneImage = 1
    };

    void setToNoValues();
    void setTab(e_Tab eT);

    void feed(const S_AppSettings& sAppSettings);
    S_AppSettings getAppSettings();
    bool someSettingsModified();

    explicit Dialog_AppSettings(QWidget *parent = nullptr);

    virtual void accept() override;

    ~Dialog_AppSettings() override;

public slots:
    void slot_spinBoxWithoutTextEdit_imageView_RAMAmount_valueChanged(int value);

    void slot_pushButton_cacheStorageRootPath_selectDirectory_clicked();
    void slot_lineEdit_cacheStorageRootPath_textEdited(const QString& text);

private:
    bool validate_qstrDirectory_asCacheStorageRootPath_withErrorMessageBox(const QString& qstrDirectory);

    void set_imagePyramid_OIIOCacheSizeMB(int value);

    Ui::Dialog_AppSettings *ui;

    S_AppSettings _sAppSettings;

    bool _bImagePyramid_OIIOCacheSizeMB_modified;
    bool _bCacheStorageRootPath_modified;

    QString _qtrCacheStoragePathWaitingValidation;

};

#endif // DIALOG_APPSETTINGS_H
