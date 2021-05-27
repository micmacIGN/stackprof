#include <QDebug>

#include "dialog_appsettings.h"
#include "ui_dialog_appsettings.h"

#include "spinbox_withoutTextEdit.h"

#include "../../logic/OsPlatform/standardPathLocation.hpp"

#include "../../logic/toolbox/toolbox_pathAndFile.h"

#include "../../logic/model/appSettings/SAppSettings.h"

#include <QFileDialog>
#include <QMessageBox>

Dialog_AppSettings::Dialog_AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_AppSettings) {

    ui->setupUi(this);


    //connect(ui->widget_spinBoxWithoutTextEdit_imageView_RAMAmount, QOverload<const QString &>::of(&QSpinBox::valueChanged),
    //        this, &Dialog_AppSettings::slot_spinBoxWithoutTextEdit_imageView_RAMAmount_textChanged);
    connect(ui->widget_spinBoxWithoutTextEdit_imageView_RAMAmount, QOverload<int>            ::of(&QSpinBox::valueChanged),
            this, &Dialog_AppSettings::slot_spinBoxWithoutTextEdit_imageView_RAMAmount_valueChanged);

    connect(ui->pushButton_cacheStorageRootPath_selectDirectory, &QPushButton::clicked,
            this, &Dialog_AppSettings::slot_pushButton_cacheStorageRootPath_selectDirectory_clicked);

    connect(ui->lineEdit_cacheStorageRootPath, &QLineEdit::textEdited,
            this, &Dialog_AppSettings::slot_lineEdit_cacheStorageRootPath_textEdited);


    setToNoValues();
}


void Dialog_AppSettings::slot_spinBoxWithoutTextEdit_imageView_RAMAmount_valueChanged(int value) {
    qDebug() << __FUNCTION__ << "value = " << value;

    set_imagePyramid_OIIOCacheSizeMB(value);

    _bImagePyramid_OIIOCacheSizeMB_modified = true;
}

void Dialog_AppSettings::set_imagePyramid_OIIOCacheSizeMB(int value) {
    _sAppSettings._sas_imagePyramid.setWithRangeLimit(value);
    qDebug() << __FUNCTION__ << "_sAppSettings._sas_imagePyramid._OIIOCacheSizeMB now set to: " <<  _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB;
}

void Dialog_AppSettings::slot_pushButton_cacheStorageRootPath_selectDirectory_clicked() {

    //preselect the current cacheStorageRootPath if not empty and existing
    //else we preselect the home of the user
    QString strStartingDir = StandardPathLocation::strStartingDefaultDir();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;

    QString qstrDirectory = QFileDialog::getExistingDirectory(
                this, "Select directory for internal application data storage",
                strStartingDir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (qstrDirectory.isEmpty()) { //(QFileDialog directory selection canceled)
        return;
    }
    _sAppSettings._sas_cacheStorage.clear();
    bool bValid = validate_qstrDirectory_asCacheStorageRootPath_withErrorMessageBox(qstrDirectory);
    if (bValid) {
        _sAppSettings._sas_cacheStorage.set(qstrDirectory);
        ui->lineEdit_cacheStorageRootPath->setText(qstrDirectory);
        _qtrCacheStoragePathWaitingValidation.clear();
    }
    _bCacheStorageRootPath_modified = true;
}



void Dialog_AppSettings::slot_lineEdit_cacheStorageRootPath_textEdited(const QString& text) {
    qDebug() << __FUNCTION__ << " entering with text =" << text;
    _sAppSettings._sas_cacheStorage.clear();
    _qtrCacheStoragePathWaitingValidation = text;
    _bCacheStorageRootPath_modified = true;
}

void Dialog_AppSettings::setToNoValues() {

    _qtrCacheStoragePathWaitingValidation.clear();

    _bImagePyramid_OIIOCacheSizeMB_modified = false;
    _bCacheStorageRootPath_modified = false;

    _sAppSettings._sas_imagePyramid.setWithRangeLimit(S_AppSettings_ImagePyramid::_cst_defaultImagePyramid_OIIOCacheSizeMB);
    ui->widget_spinBoxWithoutTextEdit_imageView_RAMAmount->setValue(S_AppSettings_ImagePyramid::_cst_defaultImagePyramid_OIIOCacheSizeMB); //valid default

    _sAppSettings._sas_cacheStorage.clear();
    ui->lineEdit_cacheStorageRootPath->clear();

    ui->tabWidget->setCurrentIndex(e_T_DiskStoragePath);
}

void Dialog_AppSettings::setTab(e_Tab eT) {
    ui->tabWidget->setCurrentIndex(eT);
}

S_AppSettings Dialog_AppSettings::getAppSettings() {
    return(_sAppSettings);
}


bool Dialog_AppSettings::someSettingsModified() {

    qDebug() << __FUNCTION__ << " _bImagePyramid_OIIOCacheSizeMB_modified =" << _bImagePyramid_OIIOCacheSizeMB_modified;
    qDebug() << __FUNCTION__ << " _bCacheStorageRootPath_modified =" << _bCacheStorageRootPath_modified;

    return(_bImagePyramid_OIIOCacheSizeMB_modified || _bCacheStorageRootPath_modified);
}

void Dialog_AppSettings::feed(const S_AppSettings& sAppSettings) {

    setToNoValues();

    _sAppSettings = sAppSettings;

    QString strPathCacheStorage;
    if (_sAppSettings._sas_cacheStorage.get(strPathCacheStorage)) {
        ui->lineEdit_cacheStorageRootPath->setText(strPathCacheStorage);
        _qtrCacheStoragePathWaitingValidation = strPathCacheStorage;
    }

    if (_sAppSettings._sas_imagePyramid.isValid()) {
        ui->widget_spinBoxWithoutTextEdit_imageView_RAMAmount->setValue(_sAppSettings._sas_imagePyramid._OIIOCacheSizeMB);
    }

}

void Dialog_AppSettings::accept() {

   if (!_sAppSettings._sas_cacheStorage.isValid()) {
       bool bCacheStorageDirectoryValid = validate_qstrDirectory_asCacheStorageRootPath_withErrorMessageBox(_qtrCacheStoragePathWaitingValidation);
       if (!bCacheStorageDirectoryValid) {
           return;
       }
       _sAppSettings._sas_cacheStorage.set(_qtrCacheStoragePathWaitingValidation); //setting _bPathValid flag to true

    }
    if (!_sAppSettings._sas_imagePyramid.isValid()) {
        QMessageBox messageBox(QMessageBox::Information,
                               "Selected RAM for input image is invalid",
                               "(internal dev error)",
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        return;
    }
    _sAppSettings._sas_imagePyramid._OIIOCacheSizeMB = ui->widget_spinBoxWithoutTextEdit_imageView_RAMAmount->value();

    QDialog::accept();
}


bool Dialog_AppSettings::validate_qstrDirectory_asCacheStorageRootPath_withErrorMessageBox(const QString& qstrDirectory) {

    QString strErrorReason;

    bool bCacheStorageRootPathIsValid = _sAppSettings._sas_cacheStorage.checkValid(qstrDirectory, strErrorReason);
    if (!bCacheStorageRootPathIsValid) {
        QMessageBox messageBox(QMessageBox::Information,
                               "Selected directory doest not match with requirements",
                               "Error: " + strErrorReason,
                               QMessageBox::Ok, nullptr);
        messageBox.exec();
        return(false);
    }    
    return(true);
}

Dialog_AppSettings::~Dialog_AppSettings() {
    delete ui;
}
