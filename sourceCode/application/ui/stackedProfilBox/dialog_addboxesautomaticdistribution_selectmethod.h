#ifndef DIALOG_ADDBOXESAUTOMATICDISTRIBUTION_SELECTMETHOD_H
#define DIALOG_ADDBOXESAUTOMATICDISTRIBUTION_SELECTMETHOD_H

#include <QDialog>

namespace Ui {
class Dialog_addBoxesAutomaticDistribution_selectMethod;
}

#include "../../logic/model/core/enum_automaticDistributionMethod.h"

class Dialog_addBoxesAutomaticDistribution_selectMethod : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_addBoxesAutomaticDistribution_selectMethod(QWidget *parent = nullptr);
    ~Dialog_addBoxesAutomaticDistribution_selectMethod();

    e_AutomaticDistributionMethod get_selectedMethod();

public slots:
    void slot_closeAsPossible_clicked();
    void slot_selectDistanceBetweenBoxesCenter_clicked();

private:
    Ui::Dialog_addBoxesAutomaticDistribution_selectMethod *ui;

    e_AutomaticDistributionMethod _eAutomaticDistributionMethod;
};

#endif // DIALOG_ADDBOXESAUTOMATICDISTRIBUTION_SELECTMETHOD_H
