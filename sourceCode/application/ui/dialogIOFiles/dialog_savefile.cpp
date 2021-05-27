#include <QFileDialog>
#include <QString>

#include <QDebug>


#include "dialog_savefile.h"
#include "ui_dialog_savefile.h"

#include "../../logic/OsPlatform/standardPathLocation.hpp"

Dialog_saveFile::Dialog_saveFile(
        const QString& strWindowTitle,
        const QString& strAboutFilename,
        const QString& strQuestion,

        const QString& strDotExtension,
        bool bSaveButtonIsSaveAs,
        QWidget *parent):
    QDialog(parent),
    _bSaveButtonIsSaveAs(bSaveButtonIsSaveAs),

    _strDotExtension(strDotExtension),
    ui(new Ui::Dialog_saveFile)
{
    ui->setupUi(this);

    setWindowTitle(strWindowTitle);

    if (!_strDotExtension.isEmpty()) {
        _strFilter = "*" + strDotExtension;
    }

    _strStartingDir = StandardPathLocation::strStartingDefaultDir();

    ui->label_aboutFilename->setText(strAboutFilename);
    ui->label_question->setText(strQuestion);
    if (bSaveButtonIsSaveAs) {
        ui->pushButton_save->setText("Save as...");
    }

    connect(ui->pushButton_doNotSave, &QPushButton::clicked, this, &Dialog_saveFile::slot_doNotSave);
    connect(ui->pushButton_save     , &QPushButton::clicked, this, &Dialog_saveFile::slot_save);
    connect(ui->pushButton_cancel   , &QPushButton::clicked, this, &Dialog_saveFile::reject);
}

QString Dialog_saveFile::getStrSelectedfileName() {
    return(_strSelectedfileName);
}

void Dialog_saveFile::slot_save() {

    _strSelectedfileName.clear();

    if (!_bSaveButtonIsSaveAs) {
        accept();
        return;
    }

    //'save as...'

    qDebug() << __FUNCTION__ << "_strStartingDir: " << _strStartingDir;
    QString strfileName = QFileDialog::getSaveFileName(this, tr("Save as"), _strStartingDir, _strFilter);
    if (strfileName.isEmpty()) {
        return; //stay the window open, waiting a valid user action
    }
    _strSelectedfileName = strfileName;
    if (!_strDotExtension.isEmpty()) {
        if (!_strSelectedfileName.endsWith(_strDotExtension)) {
            _strSelectedfileName+=_strDotExtension;
        }
    }

    accept();
}

void Dialog_saveFile::setStartingDir(const QString& strStartingDir) {
    _strStartingDir = strStartingDir;
    if (_strStartingDir.isEmpty()) {
        _strStartingDir = StandardPathLocation::strStartingDefaultDir();
    }
}



void Dialog_saveFile::slot_doNotSave() {
    _strSelectedfileName.clear();
    done(e_DialogSaveFile_doNotSave);
}

Dialog_saveFile::~Dialog_saveFile() {
    delete ui;
}
