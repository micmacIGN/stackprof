#ifndef DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_DISTANCEBETWEENCENTER_H
#define DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_DISTANCEBETWEENCENTER_H

#include <QDialog>

#include "../logic/model/core/ComputationCore_structures.h"

namespace Ui {
    class Dialog_addBoxes_automaticDistribution_distanceBetweenCenter;
}

class Dialog_addBoxes_automaticDistribution_distanceBetweenCenter : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_addBoxes_automaticDistribution_distanceBetweenCenter(QWidget *parent = nullptr);
    ~Dialog_addBoxes_automaticDistribution_distanceBetweenCenter();

    void setWidgetStates_startingPoint(e_StartingPointForAutomaticDistribution eSPFAD_value);

public slots:
    void slot_pushButtonCreateBoxes_clicked();
    void slot_editedWidthLengthValidity(bool bValid);

    void slot_distanceBetweenCenters_edited(const QString &qstrText);

    void slot_setStartingPoint_firstPoint();
    void slot_setStartingPoint_lastPoint();

signals: //@#LP replace by get parameters to createBoxes to avoid signal slot in this case
    void signal_createBoxes_withDistanceBetweenCenter(int boxWidth, int boxlength, int ieStartingPointPFAD_selected, int pixelDistanceBetweenCenter);

private:

    void distanceBetweenCenters_edited();
    void updateEnableState_of_setPushButton(bool bEnable);

    Ui::Dialog_addBoxes_automaticDistribution_distanceBetweenCenter *ui;

    int _pixelDistanceBetweenCenter;
    bool _bPixelDistanceBetweenCenterValid;

    bool _bWidthLengthValidForComputation;

    e_StartingPointForAutomaticDistribution _eSPFAD_selected;

};

#endif // DIALOG_ADDBOXES_AUTOMATICDISTRIBUTION_DISTANCEBETWEENCENTER_H
