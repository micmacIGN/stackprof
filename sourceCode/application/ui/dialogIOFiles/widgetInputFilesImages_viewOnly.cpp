#include <QDebug>

#include "ui_widget_inputFiles_images_viewOnly.h"

#include "widgetInputFilesImages_viewOnly.h"
#include "ui_enums.hpp"

WidgetInputFilesImages_viewOnly::WidgetInputFilesImages_viewOnly(QWidget *parent): QWidget(parent),
    _eUiSelectFileMode(e_uSFM_oneFile),
    _tqlineEditPtr_Alone_File1_File2 {nullptr, nullptr, nullptr}
   {

    qDebug() << __FUNCTION__;

    ui = new Ui::widget_inputFiles_images_viewOnly;
    if (!ui) {
        return;
    }

    ui->setupUi(this);

    _tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone] =  ui->lineEdit_fileAlone;
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File1] =  ui->lineEdit_file1;
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File2] =  ui->lineEdit_file2;

    setToNoValues();
    setSelectFilemode(_eUiSelectFileMode);

}

void WidgetInputFilesImages_viewOnly::setSelectFilemode(e_UiSelectFileMode eUiSelectFileMode) {
   _eUiSelectFileMode = eUiSelectFileMode;
    e_Page_wInputFImages pageStackedWidgetIndex = e_Page_wInputFImages::page_FileAlone;
    if (_eUiSelectFileMode == e_uSFM_twoFiles) {
        pageStackedWidgetIndex = e_Page_wInputFImages::page_File1_File2;
    }
    setCurrentIndex(pageStackedWidgetIndex);
}

void WidgetInputFilesImages_viewOnly::setFilesTitle(const QVector<QString> qStrFileTitle) {
    if (!ui) {
        return;
    }
    switch (_eUiSelectFileMode) {
        case e_uSFM_twoFiles:
            if (qStrFileTitle.size() < 2 ) {
                return;
            }
            ui->label_file1->setText(qStrFileTitle[0]);
            ui->label_file2->setText(qStrFileTitle[1]);
            break;
        case e_uSFM_oneFile:
            break;
    }
}

void WidgetInputFilesImages_viewOnly::setCurrentIndex(e_Page_wInputFImages ePage_wInputFImages) {
    if (!ui) {
        return;
    }
    ui->stackedWidget->setCurrentIndex(ePage_wInputFImages);
}

void WidgetInputFilesImages_viewOnly::set_file1_file2(const QString& strFile1, const QString& strFile2) {
    if (!ui) {
        return;
    }
    if (_eUiSelectFileMode != e_uSFM_twoFiles) {
        return;
    }
    _tqstrFilename_Alone_File1_File2[e_idxFS_File1] = strFile1;
    _tqstrFilename_Alone_File1_File2[e_idxFS_File2] = strFile2;

    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File1]->setText(strFile1);
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File2]->setText(strFile2);

}

void WidgetInputFilesImages_viewOnly::set_fileAlone(const QString& strFileAlone) {
    if (!ui) {
        return;
    }
    if (_eUiSelectFileMode != e_uSFM_oneFile) {
        return;
    }
    _tqstrFilename_Alone_File1_File2[e_idxFS_Alone] = strFileAlone;

    _tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone]->setText(strFileAlone);
}

void WidgetInputFilesImages_viewOnly::setToNoValues() {

    _tqstrFilename_Alone_File1_File2[e_idxFS_File1].clear();
    _tqstrFilename_Alone_File1_File2[e_idxFS_File2].clear();

    _tqstrFilename_Alone_File1_File2[e_idxFS_Alone].clear();

    setToNoValues_uiOnly();
}

void WidgetInputFilesImages_viewOnly::reFillUi_fileAlone() {
    if (!ui) {
        return;
    }
    ui->lineEdit_fileAlone->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_Alone]);
}

void WidgetInputFilesImages_viewOnly::reFillUi_file1_file2() {
    if (!ui) {
        return;
    }
    ui->lineEdit_file1->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_File1]);
    ui->lineEdit_file2->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_File2]);
}

void WidgetInputFilesImages_viewOnly::setToNoValues_uiOnly() {

    if (!ui) {
        return;
    }

    ui->lineEdit_file1->clear();
    ui->lineEdit_file2->clear();

    ui->lineEdit_fileAlone->clear();
}

WidgetInputFilesImages_viewOnly::~WidgetInputFilesImages_viewOnly() {
    delete ui;
}
