#ifndef widgetInputFilesImages_viewOnly_H
#define widgetInputFilesImages_viewOnly_H


#include <QLineEdit>

#include "ui_enums.hpp"

namespace Ui {
    class widget_inputFiles_images_viewOnly;
}

class WidgetInputFilesImages_viewOnly : public QWidget {

  Q_OBJECT

public:

    WidgetInputFilesImages_viewOnly(QWidget *parent = nullptr);

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

    QString get_fileAlone();

    void get_file1_file2(QString& strFile1, QString& strFile2);

    ~WidgetInputFilesImages_viewOnly() override;

signals:

public slots:

private:

    void setStrStartingPathForFileSelection(const QString& strStartingPathForFileSelection);

    enum e_Page_wInputFImages {
        page_File1_File2 = 0,
        page_FileAlone
    };

    void setCurrentIndex(e_Page_wInputFImages ePage_wInputFImages);

private:

    e_UiSelectFileMode _eUiSelectFileMode;

    Ui::widget_inputFiles_images_viewOnly *ui;

    QLineEdit* _tqlineEditPtr_Alone_File1_File2[3];
    QString _tqstrFilename_Alone_File1_File2[3];

};

#endif // widgetInputFilesImages_viewOnly_H
