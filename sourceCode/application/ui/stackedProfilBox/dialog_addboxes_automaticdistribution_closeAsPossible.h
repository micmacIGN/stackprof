#ifndef DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_CLOSEASPOSSIBLE_H
#define DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_CLOSEASPOSSIBLE_H

#include <QDialog>

#include "../logic/model/core/ComputationCore_structures.h"

namespace Ui {
class Dialog_addBoxes_automaticDistribution_closeAsPossible;
}

class Dialog_addBoxes_automaticDistribution_closeAsPossible : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_addBoxes_automaticDistribution_closeAsPossible(QWidget *parent = nullptr);
    ~Dialog_addBoxes_automaticDistribution_closeAsPossible();

    void setWidgetStates_distanceBetweenSuccessivesBoxes(e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution e_DBSBuAD_value);
    void setWidgetStates_startingPoint(e_StartingPointForAutomaticDistribution eSPFAD_value);

    //void setFocusToFirstEditableWidget();

    //void showEvent(QShowEvent *event) override;

public slots:
    void slot_pushButtonCreateBoxes_clicked();
    void slot_editedWidthLengthValidity(bool bValid);

    void slot_setDistanceBetweenSuccessivesBoxes_square2();
    void slot_setDistanceBetweenSuccessivesBoxes_square8();

    void slot_setStartingPoint_firstPoint();
    void slot_setStartingPoint_lastPoint();

signals: //@#LP replace by get parameters to createBoxes to avoid signal slot in this case
    void signal_createBoxes_closeAsPossible(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int ieDistanceBSBuAD_selected);

private:
    Ui::Dialog_addBoxes_automaticDistribution_closeAsPossible *ui;

    e_DistanceBetweenSuccessivesBoxes_usingAutomaticDistribution _eDBSBuAD_selected;
    e_StartingPointForAutomaticDistribution _eSPFAD_selected;


};

#endif // DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_CLOSEASPOSSIBLE_H
