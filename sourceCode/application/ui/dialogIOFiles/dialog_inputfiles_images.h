#ifndef DIALOG_INPUTFILES_IMAGES_H
#define DIALOG_INPUTFILES_IMAGES_H

#include <QDialog>

#include "ui_enums.hpp"

namespace Ui {
class Dialog_inputFiles_images;
}

class Dialog_inputFiles_images : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog_inputFiles_images(const QString& strDialogTitle, e_UiSelectFileMode eUiSelectFileMode, QWidget *parent = nullptr);
    ~Dialog_inputFiles_images();

    void setFilesTitle(const QVector<QString> qStrFileTitle);

    //debug method:
    void showSelectedFiles();

    virtual void accept() override;

    QString get_fileAlone(); 
    void get_file1_file2(QString& strFile1, QString& strFile2);

    float get_stepDequantizationStep();

private:
    e_UiSelectFileMode _eUiSelectFileMode;

    Ui::Dialog_inputFiles_images *ui;

    QString _tqstrFilename_Alone_File1_File2[3];

    float _fDequantizationStep;

    QString _tqstrFileTitle_Alone_File1_File2[3];

};

#endif // DIALOG_INPUTFILES_IMAGES_H
