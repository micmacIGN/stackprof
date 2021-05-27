#ifndef DIALOG_TRACEPARAMETERS_H
#define DIALOG_TRACEPARAMETERS_H

#include <QDialog>

namespace Ui {
class Dialog_traceParameters;
}

class Dialog_traceParameters : public QDialog
{
    Q_OBJECT

public:
    //idRoute here is not mandatory the real idRoute but the idRoute displayed on ui side like row number
    //This is because the routeContainer has a specific idRoute handling which can be disturbing to display to the user
    //When saving on disk, the idRoutes handlede by the routeContainer will be reordered.
    //Hence, this is invisible to the user
    explicit Dialog_traceParameters(int idRoute, QWidget *parent = nullptr);
    ~Dialog_traceParameters();

    void setRouteName(const QString& strRouteName);
    QString getEditedName();

public slots:
    void slot_lineEdit_routeName_textEdited(const QString &text);

private:
    void checkStringValidAndUpdateButtonOK(const QString &text, bool bUsePreviousWarningState);

private:

    Ui::Dialog_traceParameters *ui;

    bool _bWarned;
};

#endif // DIALOG_TRACEPARAMETERS_H
