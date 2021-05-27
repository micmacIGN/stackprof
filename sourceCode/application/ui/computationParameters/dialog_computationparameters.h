#ifndef DIALOG_COMPUTATIONPARAMETERS_H
#define DIALOG_COMPUTATIONPARAMETERS_H

#include <QDialog>
#include <QVector>

#include "../../logic/model/core/ComputationCore_structures.h"

class Widget_correlScoreMapParameters;

namespace Ui {
class Dialog_computationParameters;
}

class Dialog_computationParameters : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_computationParameters(QWidget *parent = nullptr);
    ~Dialog_computationParameters();

    S_ComputationParameters get_computationParameters_edited();

public slots:
    void slot_radioButton_interpolationMethod_toggled(bool bChecked);
    void slot_radioButton_baseComputationMethod_toggled(bool bChecked);

    void slot_setComputationParameters(S_ComputationParameters computationParameters,
                                       const U_CorrelationScoreMapFileUseFlags& uCorrelationScoreUsageFlags,
                                       QVector<QString> qvectCorrelationScoreMapFile_PX1_PX2_DeltaZ);

    //void slot_pushButtonOK_clicked();
    void slot_pushButton_runComputation_clicked();
    void slot_pushButton_cancel_clicked();

    void slot_rejectionValue_validityChanged();

private:
    void feedWithEmptyDefault();
    void feed(const S_ComputationParameters& computationParameters,
              const U_CorrelationScoreMapFileUseFlags& uCorrelationScoreUsageFlags,
              const QVector<QString>& qvectStr_CorrelationScoreMapFileUsed_PX1_PX2_DeltaZ);
    void setEnable_windowPushButtonOK(bool bNewState);
    bool evaluateIfParametersValid();

private:
    Ui::Dialog_computationParameters *ui;

    QVector<bool> _qvectb_layersToCompute;
    QVector<Widget_correlScoreMapParameters*> _qvectWidget_correlScoreMapUsage_PX1PX2_PX1PX2_deltaZ;

    S_ComputationParameters _computationParameters_received;
    S_ComputationParameters _computationParameters_inEdition;

};


#endif // DIALOG_COMPUTATIONPARAMETERS_H
