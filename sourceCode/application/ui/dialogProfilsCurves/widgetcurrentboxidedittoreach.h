#ifndef WIDGETCURRENTBOXIDEDITTOREACH_H
#define WIDGETCURRENTBOXIDEDITTOREACH_H

#include <QWidget>

namespace Ui {
class widgetCurrentBoxIdEditToReach;
}

class widgetCurrentBoxIdEditToReach : public QWidget
{
    Q_OBJECT

public:
    explicit widgetCurrentBoxIdEditToReach(QWidget *parent = nullptr);
    ~widgetCurrentBoxIdEditToReach();

    void setCurrentAndBoxCount(int currentBoxId, int boxCount);

public slots:
    void slot_pushbutton_current_slash_max_clicked();
    void slot_pushbutton_pushbutton_flat_slash_max();
    void slot_lineEdit_boxIDToReach_textEdited(const QString &text);
    void slot_lineEdit_boxIDToReach_returnPressed();

signals:
    void signal_boxIdToReachFromEditedId(int boxId);

private:
    Ui::widgetCurrentBoxIdEditToReach *ui;

    int _currentPage;

    int _currentBoxId;
    int _boxCount;
    int _boxIdInEditionWaitingUserValidation;

};

#endif // WIDGETCURRENTBOXIDEDITTOREACH_H
