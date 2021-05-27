#ifndef DIALOG_REPLACEIMAGEFILE_H
#define DIALOG_REPLACEIMAGEFILE_H

#include <QDialog>

namespace Ui {
class Dialog_replaceFile;
}

class Dialog_replaceImageFile : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog_replaceImageFile(const QString& strWindowTitle,
                             const QString& strAboutFilename,
                             const QString& strQuestion,
                             const QString& strPathAsStartingDirectoryForSelection,
                             QWidget *parent = nullptr);

    QString getStrSelectedfileName();

    ~Dialog_replaceImageFile();

public slots:
    void slot_replace();

private:

    Ui::Dialog_replaceFile *ui;

    QString _strSelectedfileName;

    QString _strPathAsStartingDirectoryForSelection;

};

#endif // DIALOG_REPLACEIMAGEFILE_H
