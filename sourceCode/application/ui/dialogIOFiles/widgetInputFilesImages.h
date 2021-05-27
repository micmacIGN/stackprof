#ifndef widgetInputFilesImages_H
#define widgetInputFilesImages_H


#include <QLineEdit>
#include <QPushButton>

#include "ui_enums.hpp"

namespace Ui {
    class widget_inputFiles_images;
}

class WidgetInputFilesImages : public QWidget {

  Q_OBJECT

public:

    WidgetInputFilesImages(QWidget *parent = nullptr);

    QString getStrStartingPathForFileSelection();

    void setSelectFilemode(e_UiSelectFileMode eUiSelectFileMode);
    void setFilesTitle(const QVector<QString> qStrFileTitle);

    void setToNoValues();
    void setToNoValues_uiOnly(); //used to clear the fiels at screen, but keep the entered field in private fields
                                 //this is used typically to have empty disabled widget which will be feed witht the previous entered field when re-enabled

    void reFillUi_fileAlone();
    void reFillUi_file1_file2();

    void set_file1_file2(const QString& strFile1, const QString& strFile2);
    void set_fileAlone(const QString& strFileAlone);

    bool inputFeed();

    QString get_fileAlone();

    void get_file1_file2(QString& strFile1, QString& strFile2);

    void set_editableState_warnState(e_idxFileSelection eIdxFileSelection, bool bEditable, bool bWarned);

    ~WidgetInputFilesImages() override;


signals:
    void signal_lineEdit_aboutInputFilename_emptyStateChanged();

    void signal_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

public slots:
    void slot_pushButton_selectFile_clicked();
    void slot_lineEditFilename_textEdited(const QString& text);

    void slot_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

private:

    void setStrStartingPathForFileSelection(const QString& strStartingPathForFileSelection);

    void setWarnedOff(int intAssociatedFileId);

    bool inputFilesfeed();

    enum e_Page_wInputFImages {
        page_File1_File2 = 0,
        page_FileAlone
    };

    void setCurrentIndex(e_Page_wInputFImages ePage_wInputFImages);

private:

    e_UiSelectFileMode _eUiSelectFileMode;

    Ui::widget_inputFiles_images *ui;

    QLineEdit* _tqlineEditPtr_Alone_File1_File2[3];
    QString _tqstrFilename_Alone_File1_File2[3];

    QPushButton *_tqpushButtonPtr_Alone_File1_File2[3];

    bool _tbWarned_Alone_File1_File2[3];

    QString _strStartingPathForFileSelection;

};


#endif // windowImageViewStatusBar_H
