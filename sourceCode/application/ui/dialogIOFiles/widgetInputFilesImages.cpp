#include <QFileDialog>
#include <QDebug>

#include <QRegularExpression>

#include "ui_widget_inputFiles_images.h"

#include "widgetInputFilesImages.h"
#include "ui_enums.hpp"

#include "../../logic/OsPlatform/standardPathLocation.hpp"

WidgetInputFilesImages::WidgetInputFilesImages(QWidget *parent): QWidget(parent),
    _eUiSelectFileMode(e_uSFM_oneFile),
    _tqlineEditPtr_Alone_File1_File2 {nullptr, nullptr, nullptr},
    _tbWarned_Alone_File1_File2{ false, false, false}
   {

    qDebug() << __FUNCTION__;

    ui = new Ui::widget_inputFiles_images;
    if (!ui) {
        return;
    }

    ui->setupUi(this);

    _tqpushButtonPtr_Alone_File1_File2[e_idxFS_Alone] = ui->pushButton_selectFileAlone;
    _tqpushButtonPtr_Alone_File1_File2[e_idxFS_File1] = ui->pushButton_selectFile1;
    _tqpushButtonPtr_Alone_File1_File2[e_idxFS_File2] = ui->pushButton_selectFile2;

    ui->pushButton_selectFileAlone->setProperty("associatedFileId", e_idxFS_Alone);
    ui->pushButton_selectFile1    ->setProperty("associatedFileId", e_idxFS_File1);
    ui->pushButton_selectFile2    ->setProperty("associatedFileId", e_idxFS_File2);


    _tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone] =  ui->lineEdit_fileAlone;
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File1] =  ui->lineEdit_file1;
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File2] =  ui->lineEdit_file2;

    _tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone]->setProperty("associatedFileId", e_idxFS_Alone);
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File1]->setProperty("associatedFileId", e_idxFS_File1);
    _tqlineEditPtr_Alone_File1_File2[e_idxFS_File2]->setProperty("associatedFileId", e_idxFS_File2);

    connect(ui->pushButton_selectFileAlone, &QPushButton::clicked,  this, &WidgetInputFilesImages::slot_pushButton_selectFile_clicked);
    connect(ui->pushButton_selectFile1, &QPushButton::clicked,      this, &WidgetInputFilesImages::slot_pushButton_selectFile_clicked);
    connect(ui->pushButton_selectFile2, &QPushButton::clicked,      this, &WidgetInputFilesImages::slot_pushButton_selectFile_clicked);

    connect(_tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone], &QLineEdit::textEdited, this, &WidgetInputFilesImages::slot_lineEditFilename_textEdited);
    connect(_tqlineEditPtr_Alone_File1_File2[e_idxFS_File1], &QLineEdit::textEdited, this, &WidgetInputFilesImages::slot_lineEditFilename_textEdited);
    connect(_tqlineEditPtr_Alone_File1_File2[e_idxFS_File2], &QLineEdit::textEdited, this, &WidgetInputFilesImages::slot_lineEditFilename_textEdited);

    setToNoValues();
    setSelectFilemode(_eUiSelectFileMode);

}


void WidgetInputFilesImages::slot_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection) {
    setStrStartingPathForFileSelection(strNewStartingPathForFileSelection);
}

void WidgetInputFilesImages::setStrStartingPathForFileSelection(const QString& strStartingPathForFileSelection) {
    _strStartingPathForFileSelection = strStartingPathForFileSelection;
}


QString WidgetInputFilesImages::getStrStartingPathForFileSelection() {
    return(_strStartingPathForFileSelection);
}

void WidgetInputFilesImages::setSelectFilemode(e_UiSelectFileMode eUiSelectFileMode) {
   _eUiSelectFileMode = eUiSelectFileMode;
    e_Page_wInputFImages pageStackedWidgetIndex = e_Page_wInputFImages::page_FileAlone;
    if (_eUiSelectFileMode == e_uSFM_twoFiles) {
        pageStackedWidgetIndex = e_Page_wInputFImages::page_File1_File2;
    }
    setCurrentIndex(pageStackedWidgetIndex);
}

void WidgetInputFilesImages::setFilesTitle(const QVector<QString> qStrFileTitle) {
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

QString WidgetInputFilesImages::get_fileAlone() {
    return(_tqstrFilename_Alone_File1_File2[e_idxFS_Alone]);
}

void WidgetInputFilesImages::get_file1_file2(QString& strFile1, QString& strFile2) {
    strFile1 = _tqstrFilename_Alone_File1_File2[e_idxFS_File1];
    strFile2 = _tqstrFilename_Alone_File1_File2[e_idxFS_File2];
}

void WidgetInputFilesImages::set_editableState_warnState(e_idxFileSelection eIdxFileSelection, bool bEditable, bool bWarned) {

    _tqpushButtonPtr_Alone_File1_File2[eIdxFileSelection]->setEnabled(bEditable);
    _tqlineEditPtr_Alone_File1_File2[eIdxFileSelection]->setEnabled(bEditable);    
    _tbWarned_Alone_File1_File2[eIdxFileSelection] = bWarned;
    if (_tbWarned_Alone_File1_File2[eIdxFileSelection]) {
        _tqlineEditPtr_Alone_File1_File2[eIdxFileSelection]->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
    } else {

        _tqlineEditPtr_Alone_File1_File2[eIdxFileSelection]->setStyleSheet("");
    }
}

void WidgetInputFilesImages::slot_lineEditFilename_textEdited(const QString &text) {

    qDebug() << __FUNCTION__ << "text = " << text;

    QObject *ptrSender = sender();
    QVariant qvariant_associatedFile = ptrSender->property("associatedFileId");
    bool bOk = false;
    int intAssociatedFileId  = qvariant_associatedFile.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associatedFile.toInt()";
        return;
    }

    if (  (intAssociatedFileId < e_idxFS_Alone)
        ||(intAssociatedFileId > e_idxFS_File2)) {
        qDebug() << __FUNCTION__ << " error: invalid intAssociatedFileId ( " << intAssociatedFileId << " )";
        return;
    }

    qDebug() << __FUNCTION__ << "intAssociatedFileId:" << intAssociatedFileId;

    bool bFieldWasEmpty = _tqstrFilename_Alone_File1_File2[intAssociatedFileId].isEmpty();

    _tqstrFilename_Alone_File1_File2[intAssociatedFileId] = text;

    setWarnedOff(intAssociatedFileId);

    bool bFieldNowEmpty  = text.isEmpty();

    if (bFieldNowEmpty) {
        emit signal_lineEdit_aboutInputFilename_emptyStateChanged(/*true*/);
    } else {
        if (bFieldWasEmpty && (!bFieldNowEmpty)) { //avoid to emit at each character added
            emit signal_lineEdit_aboutInputFilename_emptyStateChanged(/*false*/);
        }
    }
}

void WidgetInputFilesImages::setWarnedOff(int intAssociatedFileId) {
    if (_tbWarned_Alone_File1_File2[intAssociatedFileId]) {
        _tbWarned_Alone_File1_File2[intAssociatedFileId] = false;

        _tqlineEditPtr_Alone_File1_File2[intAssociatedFileId]->setStyleSheet("");
    }
}

#include "toolbox/toolbox_pathAndFile.h"

void WidgetInputFilesImages::slot_pushButton_selectFile_clicked() {

    qDebug() << __FUNCTION__;

    QObject *ptrSender = sender();
    QVariant qvariant_associatedFile = ptrSender->property("associatedFileId");
    bool bOk = false;
    int intAssociatedFileId  = qvariant_associatedFile.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associatedFile.toInt()";
        return;
    }

    if (  (intAssociatedFileId < e_idxFS_Alone)
        ||(intAssociatedFileId > e_idxFS_File2)) {
        qDebug() << __FUNCTION__ << " error: invalid intAssociatedFileId ( " << intAssociatedFileId << " )";
        return;
    }

    qDebug() << __FUNCTION__ << "intAssociatedFileId:" << intAssociatedFileId;


    QString strStartingDir = _strStartingPathForFileSelection; //StandardPathLocation::strStartingDefaultDir();
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;

    QString strfileName = QFileDialog::getOpenFileName(this, tr("Open image"),
                                                    strStartingDir,
                                                    tr("*.tif *.tiff *.TIF *.TIFF"));

    if (strfileName.isEmpty()) {
        return;
    }

    _strStartingPathForFileSelection = getStrPathFromPathFile(strfileName);
    emit signal_startingPathForFileSelectionChanged(_strStartingPathForFileSelection);

    _tqstrFilename_Alone_File1_File2[intAssociatedFileId] = strfileName;
    if (_tqlineEditPtr_Alone_File1_File2[intAssociatedFileId]) {
        _tqlineEditPtr_Alone_File1_File2[intAssociatedFileId]->setText(strfileName);
    }

    setWarnedOff(intAssociatedFileId);

    emit signal_lineEdit_aboutInputFilename_emptyStateChanged();

}

bool WidgetInputFilesImages::inputFeed() {
    return(inputFilesfeed());
}

bool WidgetInputFilesImages::inputFilesfeed() {
    switch (_eUiSelectFileMode) {
        case e_uSFM_twoFiles:
            qDebug() << "_tqstrFilename_Alone_File1_File2[e_idxFS_File1] = [ " << _tqstrFilename_Alone_File1_File2[e_idxFS_File1] << "]";
            qDebug() << "_tqstrFilename_Alone_File1_File2[e_idxFS_File2] = [ " << _tqstrFilename_Alone_File1_File2[e_idxFS_File2] << "]";
            if (  (!_tqstrFilename_Alone_File1_File2[e_idxFS_File1].isEmpty())
                &&(!_tqstrFilename_Alone_File1_File2[e_idxFS_File2].isEmpty())) {
                    return(true);
            }
            break;
        case e_uSFM_oneFile:        
            qDebug() << "_tqstrFilename_Alone_File1_File2[e_idxFS_Alone] = [ " << _tqstrFilename_Alone_File1_File2[e_idxFS_Alone] << "]";
            return(!_tqstrFilename_Alone_File1_File2[e_idxFS_Alone].isEmpty());
    }
    return(false);
}

void WidgetInputFilesImages::setCurrentIndex(e_Page_wInputFImages ePage_wInputFImages) {
    if (!ui) {
        return;
    }
    ui->stackedWidget->setCurrentIndex(ePage_wInputFImages);
}

void WidgetInputFilesImages::set_file1_file2(const QString& strFile1, const QString& strFile2) {
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

void WidgetInputFilesImages::set_fileAlone(const QString& strFileAlone) {
    if (!ui) {
        return;
    }
    if (_eUiSelectFileMode != e_uSFM_oneFile) {
        return;
    }
    _tqstrFilename_Alone_File1_File2[e_idxFS_Alone] = strFileAlone;

    _tqlineEditPtr_Alone_File1_File2[e_idxFS_Alone]->setText(strFileAlone);
}

void WidgetInputFilesImages::setToNoValues() {

    _tqstrFilename_Alone_File1_File2[e_idxFS_File1].clear();
    _tqstrFilename_Alone_File1_File2[e_idxFS_File2].clear();

    _tqstrFilename_Alone_File1_File2[e_idxFS_Alone].clear();

    setToNoValues_uiOnly();
}

void WidgetInputFilesImages::reFillUi_fileAlone() {
    if (!ui) {
        return;
    }
    ui->lineEdit_fileAlone->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_Alone]);
}

void WidgetInputFilesImages::reFillUi_file1_file2() {
    if (!ui) {
        return;
    }
    ui->lineEdit_file1->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_File1]);
    ui->lineEdit_file2->setText(_tqstrFilename_Alone_File1_File2[e_idxFS_File2]);
}

void WidgetInputFilesImages::setToNoValues_uiOnly() {

    if (!ui) {
        return;
    }

    ui->lineEdit_file1->clear();
    ui->lineEdit_file2->clear();

    ui->lineEdit_fileAlone->clear();

    //separated lineEdit_dequantStep to be more nice on screen

    set_editableState_warnState(e_idxFS_File1, true, false);
    set_editableState_warnState(e_idxFS_File2, true, false);
    set_editableState_warnState(e_idxFS_Alone, true, false);

}

WidgetInputFilesImages::~WidgetInputFilesImages() {
    delete ui;
}
