#ifndef DIALOG_SETPROFILORIENTATION_H
#define DIALOG_SETPROFILORIENTATION_H

#include <QDialog>

#include "../logic/model/core/ComputationCore_structures.h"

namespace Ui {
class Dialog_setProfilOrientation;
}

class Dialog_setProfilOrientation : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_setProfilOrientation(QWidget *parent = nullptr);

    void setProfilOrientation(e_ProfilOrientation eProfilOrientation);

    e_ProfilOrientation getProfilOrientation();

    ~Dialog_setProfilOrientation();

public slots:
    void slot_radioButton_firstToLast_clicked();
    void slot_radioButton_lastToFirst_clicked();

private:
    void setRadioButtonsCheckedStatesFromProfilOrientation(e_ProfilOrientation eProfilOrientation);

    Ui::Dialog_setProfilOrientation *ui;

    e_ProfilOrientation _eProfilOrientation;
};

#endif // DIALOG_SETPROFILORIENTATION_H
