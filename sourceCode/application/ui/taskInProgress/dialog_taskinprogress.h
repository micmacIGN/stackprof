#ifndef DIALOG_TASKINPROGRESS_H
#define DIALOG_TASKINPROGRESS_H

#include <QDialog>

namespace Ui {
class Dialog_taskInProgress;
}

class Dialog_taskInProgress : public QDialog
{
    Q_OBJECT

public:
    enum e_Page_dialogTaskInProgress {
        e_PageDTIP_messagePleaseWait_noAbortButton = 0,
        e_PageDTIP_withAbortButton = 1
    };

    explicit Dialog_taskInProgress(QWidget *parent = nullptr);
    ~Dialog_taskInProgress() override;

    void reject() override; //to avoid that the user close the window with the close button dialog (this way should be cross-platform compliant)
    void accept() override;

    void setInitialState(e_Page_dialogTaskInProgress,
                         QString strBoxTitle, QString strProgressMessage,
                         int progressValueMin, int progressValueMax);
    void closeAndClear();

//public slots:
    //void slot_setProgressState(int progressValue, const QString& strMessage);
    void setProgressState(int progressValue);

protected:
    virtual void showEvent(QShowEvent *event) override;

private:
    void setDefaultState(e_Page_dialogTaskInProgress ePage_dialogTaskInProgress = e_PageDTIP_messagePleaseWait_noAbortButton);

private:
    Ui::Dialog_taskInProgress *ui;

    int _progressValueMin;
    int _progressValueMax;

    int _currentProgressValue;
};

#endif // DIALOG_TASKINPROGRESS_H
