#include <QFileDialog>
#include <QString>

#include <QDebug>

#include "dialog_replaceImageFile.h"
#include "ui_dialog_replacefile.h"

#include "../../logic/OsPlatform/standardPathLocation.hpp"


#include "../../logic/toolbox/toolbox_pathAndFile.h"

Dialog_replaceImageFile::Dialog_replaceImageFile(
        const QString& strWindowTitle,
        const QString& strAboutFilename,
        const QString& strQuestion,
        const QString& strPathAsStartingDirectoryForSelection,
        QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog_replaceFile)
{
    ui->setupUi(this);

    setWindowTitle(strWindowTitle);


    ui->lineEdit_filename->setText(strAboutFilename);
    ui->label_question->setText(strQuestion);

    _strPathAsStartingDirectoryForSelection = strPathAsStartingDirectoryForSelection;

    connect(ui->pushButton_replace  , &QPushButton::clicked, this, &Dialog_replaceImageFile::slot_replace);
    connect(ui->pushButton_cancel   , &QPushButton::clicked, this, &Dialog_replaceImageFile::reject);
}

QString Dialog_replaceImageFile::getStrSelectedfileName() {
    return(_strSelectedfileName);
}

void Dialog_replaceImageFile::slot_replace() {

    _strSelectedfileName.clear();   

    _strSelectedfileName = QFileDialog::getOpenFileName(nullptr,
                                                       tr("Replace image"),
                                                       _strPathAsStartingDirectoryForSelection,
                                                       tr("*.tif *.tiff *.TIF *.TIFF"));

    if (_strSelectedfileName.isEmpty()) {
        qDebug() << __FUNCTION__ << " canceled by user";
        reject();
        return;
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_strSelectedfileName = " << _strSelectedfileName;

    _strSelectedfileName = getPathAndFilenameFromPotentialSymlink(_strSelectedfileName);

    qDebug() << __FUNCTION__ << __LINE__ << "_strSelectedfileName = " << _strSelectedfileName;


    accept();
}

Dialog_replaceImageFile::~Dialog_replaceImageFile() {
    delete ui;
}
