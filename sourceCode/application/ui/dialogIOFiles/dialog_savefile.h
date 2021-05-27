#ifndef DIALOG_SAVEFILE_H
#define DIALOG_SAVEFILE_H

#include <QDialog>

namespace Ui {
class Dialog_saveFile;
}

class Dialog_saveFile : public QDialog
{
    Q_OBJECT

public:

    enum e_DialogSaveFile_doneValue {
           e_DialogSaveFile_cancel     = QDialog::Rejected,

           e_DialogSaveFile_save       = QDialog::Accepted,
           e_DialogSaveFile_saveAs     = e_DialogSaveFile_save,

           e_DialogSaveFile_doNotSave  = e_DialogSaveFile_saveAs + 5
    };

    explicit Dialog_saveFile(const QString& strWindowTitle,
                             const QString& strAboutFilename,
                             const QString& strQuestion,

                             const QString& strDotExtension,
                             bool bSaveButtonIsSaveAs,
                             QWidget *parent = nullptr);

    QString getStrSelectedfileName();

    void setStartingDir(const QString& strStartingDir);

    ~Dialog_saveFile();

public slots:
    void slot_doNotSave();
    void slot_save();

private:
    bool _bSaveButtonIsSaveAs;
    QString _strDotExtension;

    QString _strStartingDir;

    Ui::Dialog_saveFile *ui;

    QString _strFilter;

    QString _strSelectedfileName;




};

#endif // DIALOG_SAVEFILE_H
