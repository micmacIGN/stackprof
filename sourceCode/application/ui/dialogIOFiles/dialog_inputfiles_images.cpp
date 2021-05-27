#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

#include "dialog_inputfiles_images.h"
#include "ui_dialog_inputfiles_images.h"

#include "widgetInputFilesImages.h"
#include "ui_enums.hpp"

#include "../../logic/toolbox/toolbox_pathAndFile.h"

Dialog_inputFiles_images::Dialog_inputFiles_images(const QString& strDialogTitle, e_UiSelectFileMode eUiSelectFileMode, QWidget *parent) :
    QDialog(parent),
    _eUiSelectFileMode(eUiSelectFileMode),
    ui(new Ui::Dialog_inputFiles_images),
    _fDequantizationStep(1.0f){

    if (!ui) {
        return;
    }

    ui->setupUi(this);
    ui->wdgt_inputFilesImages->setSelectFilemode(_eUiSelectFileMode);

    setWindowTitle(strDialogTitle);
}

QString Dialog_inputFiles_images::get_fileAlone() {
    return(_tqstrFilename_Alone_File1_File2[e_idxFS_Alone]);
}

void Dialog_inputFiles_images::get_file1_file2(QString& strFile1, QString& strFile2) {
    strFile1 = _tqstrFilename_Alone_File1_File2[e_idxFS_File1];
    strFile2 = _tqstrFilename_Alone_File1_File2[e_idxFS_File2];
}

float Dialog_inputFiles_images::get_stepDequantizationStep() {
    return(_fDequantizationStep);
}

void Dialog_inputFiles_images::accept() {

    qDebug() << __FUNCTION__ ;

    bool bFilenameFeed = false;

    bFilenameFeed = ui->wdgt_inputFilesImages->inputFeed();

    if (bFilenameFeed) {
        bool bFileExist_Alone_File1_File2[3] = { false, false, false};
        bool bSelectedFileExist = true;

        QString strTitleFormessageBox;
        QString strTextFormessageBox;

        switch (_eUiSelectFileMode) {
            case e_uSFM_twoFiles:
                ui->wdgt_inputFilesImages->get_file1_file2(
                   _tqstrFilename_Alone_File1_File2[e_idxFS_File1],
                   _tqstrFilename_Alone_File1_File2[e_idxFS_File2]);

                bFileExist_Alone_File1_File2[e_idxFS_File1] = fileExists(_tqstrFilename_Alone_File1_File2[e_idxFS_File1]);
                bFileExist_Alone_File1_File2[e_idxFS_File2] = fileExists(_tqstrFilename_Alone_File1_File2[e_idxFS_File2]);

                if (!bFileExist_Alone_File1_File2[e_idxFS_File1]) {
                    bSelectedFileExist = false;
                    strTitleFormessageBox = "Selected file doesn't exist";
                    strTextFormessageBox = "choose an existing file for " + _tqstrFileTitle_Alone_File1_File2[e_idxFS_File1];
                }
                if (!bFileExist_Alone_File1_File2[e_idxFS_File2]) {
                    bSelectedFileExist = false;
                    strTitleFormessageBox = "Selected file doesn't exist";
                    strTextFormessageBox = "choose an existing file for " + _tqstrFileTitle_Alone_File1_File2[e_idxFS_File2];
                }
                if (  (!bFileExist_Alone_File1_File2[e_idxFS_File1])
                    &&(!bFileExist_Alone_File1_File2[e_idxFS_File2])) {
                    bSelectedFileExist = false;
                    strTitleFormessageBox = "selected files don't exist";
                    strTextFormessageBox = "choose existing files for ";
                    strTextFormessageBox += _tqstrFileTitle_Alone_File1_File2[e_idxFS_File1];
                    strTextFormessageBox +  " and ";
                    strTextFormessageBox += _tqstrFileTitle_Alone_File1_File2[e_idxFS_File2];
                }

                break;

            case e_uSFM_oneFile:
                _tqstrFilename_Alone_File1_File2[e_idxFS_Alone] = ui->wdgt_inputFilesImages->get_fileAlone();
                bFileExist_Alone_File1_File2[e_idxFS_Alone] = fileExists(_tqstrFilename_Alone_File1_File2[e_idxFS_Alone]);
                if (!bFileExist_Alone_File1_File2[e_idxFS_Alone]) {
                    bSelectedFileExist = false;
                    strTitleFormessageBox = "selected files don't exist";
                        strTextFormessageBox = "choose an existing file";
                }
                break;
        }

        if (!bSelectedFileExist) {
            QMessageBox messageBoxIfInputFileImagesDontExist(QMessageBox::Information,
                                                             strTitleFormessageBox,
                                                             strTextFormessageBox,
                                                             QMessageBox::Ok, nullptr);
            messageBoxIfInputFileImagesDontExist.exec();
        } else {

                //@LP empty _qstrStepDequantizationStep here should never happens
                QDialog::accept();

        }
    } else {
        QString strTitleFormessageBox;
        QString strTextFormessageBox;

        strTitleFormessageBox ="input filename empty";
        strTextFormessageBox = "choose an existing file";

        if (_eUiSelectFileMode == e_uSFM_twoFiles) {
            strTitleFormessageBox = "some input filename(s) empty";
            strTextFormessageBox  = "choose existing files";
        }

        QMessageBox messageBoxIfInputFileImagesDontExist(QMessageBox::Information,
                                                         strTitleFormessageBox,
                                                         strTextFormessageBox,
                                                         QMessageBox::Ok, nullptr);
        messageBoxIfInputFileImagesDontExist.exec();
    }
}


void Dialog_inputFiles_images::setFilesTitle(const QVector<QString> qStrFileTitle) {
    if (!ui) {
        return;
    }
    if (qStrFileTitle.size() < 2) {
        qDebug() << __FUNCTION__ << "error: qStrFileTitle.size() < 2";
        return;
    }
    ui->wdgt_inputFilesImages->setFilesTitle(qStrFileTitle);

    _tqstrFileTitle_Alone_File1_File2[e_idxFS_File1] = qStrFileTitle[0];
    _tqstrFileTitle_Alone_File1_File2[e_idxFS_File2] = qStrFileTitle[1];
}

Dialog_inputFiles_images::~Dialog_inputFiles_images()
{
    delete ui;
}
