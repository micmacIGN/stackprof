#ifndef Widget_stackprofilbox_edit_H
#define Widget_stackprofilbox_edit_H

#include <QWidget>
#include <QLineEdit>

#include <QHash>
class QPushButton;

namespace Ui {
    class Widget_stackedprofilbox_edit;
}

class Widget_stackedprofilbox_edit : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_stackedprofilbox_edit(QWidget *parent = nullptr);
    ~Widget_stackedprofilbox_edit();

    void selectedBoxChanged(int boxId, int oddPixelWidth, int oddPixelLength);
    void boxCenterPossibleShiftsToView(const QHash<int, bool>& qhash_shift_bPossibleShift);

    enum e_stackedWidgetPage { e_sWP_adjustLocation = 0,
                               e_sWP_addBox };
    void setPage(e_stackedWidgetPage eStackedWidgetPage);

    void addBoxCanceled();

public slots:

    void slot_pushButton_plusMinus(bool checked);

    void slot_pushButton_cancelAdd_clicked(bool checked);
    void slot_editedWidthLengthValidity(bool bValid, int oddPixelWidth, int oddPixelLength);
    void slot_pushButton_remove_clicked(bool checked);

signals:
    void signal_addBoxCanceled();
    void signal_pushButton_setSize_clicked(int oddPixelWidth, int oddPixelLength);
    void signal_moveCenterBoxUsingShiftFromCurrentLocation(int boxId, int shift);
    void signal_editedWidthLengthValidity(bool bValid, int oddPixelWidth, int oddPixelLength);
    void signal_removeBoxFromVectId(int boxId_toRemove);

private:
    Ui::Widget_stackedprofilbox_edit *ui;

    int _boxId_currentlySelectedBox;
    int _oddPixelWidth_currentlySelectedBox;
    int _oddPixelLength_currentlySelectedBox;

    QHash<int, QPushButton*> _qhash_step_pushButtonptr;

    bool _bAddingBoxMode;
    int _oddPixelWidth_forAddingBoxMode;
    int _oddPixelLength_forAddingBoxMode;

};

#endif // Widget_stackprofilbox_edit_H
