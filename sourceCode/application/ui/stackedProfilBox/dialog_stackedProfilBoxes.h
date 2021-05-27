#ifndef DIALOG2_H
#define DIALOG2_H

#include <QDialog>

class ComputationCore_inheritQATableModel;

namespace Ui {
    class Dialog_stackedProfilBoxes;
}

class Dialog_stackedProfilBoxes : public QDialog {
    Q_OBJECT

public:
    explicit Dialog_stackedProfilBoxes(QWidget *parent = nullptr);
    ~Dialog_stackedProfilBoxes();

    void setModelsPtr_stackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr);
    void initConnectionModelViewSignalsSlots();

    void backToState_noneBoxSelected();

    void setAddingBoxModePossible(bool bState);

    void set_addingBoxModeCanceled();

public slots:
    void slot_pushButton_setSize_clicked(int oddPixelWidth, int oddPixelLength);
    void slot_setBoxId_and_boxSize(int boxId, int oddPixelWidth, int oddPixelLength);

    void slot_boxCenterPossibleShiftsToView(QHash<int, bool> qhash_shift_bPossibleShift);
    void slot_moveCenterBoxUsingShiftFromCurrentLocation(int boxId, int shift);

    void slot_pushButton_addBox_clicked();
    void slot_editedWidthLengthValidity(bool bValid, int oddPixelWidth, int oddPixelLength);
    void slot_addingBoxModeCanceled();

    void slot_removeBoxFromVectId(int boxId_toRemove);
    void slot_removeAllBoxes_clicked();
    void slot_oneOrMoreBoxesExist(bool bExist);

    virtual void closeEvent(QCloseEvent *event); //@LP about close window button typically
    virtual void reject(); //@LP about escape key typically

signals:

    void signal_userRequest_onCurrentBox_sizeChange(int boxId, int oddPixelWidth, int oddPixelLength);
    void signal_moveCenterOfCurrentBoxUsingShiftFromCurrentLocation(int boxId, int shift);

    void signal_enteringAddingBoxMode();
    void signal_addingBoxModeActivated(bool bBoxWidthValid, int oddPixelWidth, int oddPixelLength);
    void signal_addingBoxModeCanceled();    

    void signal_removeBoxFromVectId(int boxId_toRemove);
    void signal_removeAllBoxes();

private:

    enum e_spb_stackedWidgetPage { e_spb_sWP_withoutBoxesList = 0,
                                   e_spb_sWP_withBoxesList
    };

    void setPage(e_spb_stackedWidgetPage espb_stackedWidgetPage);
    void setEnable_buttonRemoveAllBoxes(bool bEnable);

    Ui::Dialog_stackedProfilBoxes *ui;

    ComputationCore_inheritQATableModel *_computationCore_iqaTableModel_ptr;

    bool _bOneOrMoreBoxExist;
    bool _bAddingBoxMode;
    int _selected_vectBoxId;

    bool _bAddingBoxModePossible;

};

#endif // DIALOG2_H
