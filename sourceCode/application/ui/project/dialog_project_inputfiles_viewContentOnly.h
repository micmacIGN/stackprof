#ifndef DIALOG_PROJECT_INPUTFILES_viewContentOnly_H
#define DIALOG_PROJECT_INPUTFILES_viewContentOnly_H

#include <QDialog>
#include <QString>

#include "../../logic/model/core/ComputationCore_structures.h" //@#LP move S_InputFiles in a dedicated header file

class QLineEdit;

namespace Ui {
class Dialog_project_inputFiles_viewContentOnly;
}

class Dialog_project_inputFiles_viewContentOnly : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_project_inputFiles_viewContentOnly(QWidget *parent = nullptr);
    ~Dialog_project_inputFiles_viewContentOnly() override;

    void setStateAsEmptyProject();

    //bool get_qvectDequantizationStep_PX1_PX2_deltaZ(QVector<float>& qvectDequantizationStep_PX1_PX2_deltaZ);
    void setEditedProject(const S_InputFiles& inputFiles,
                          const QVector<bool>& qvectb_layersToCompute,
                          const QString& strRouteName);
    void setEditedProject_routeName(const QString& strRouteName);

public slots:

signals:

private:

    void setWidgets_enabledStatus_for_correlScoreMap_PX1PX2( bool bEnabled);

    void setWidgets_enabledStatus_for_correlScoreMap_deltaZOrOther(bool bEnabled);

    void setToNoValues();

    Ui::Dialog_project_inputFiles_viewContentOnly *ui;

    QVector<bool> _qvectb_layersToCompute;  
    QVector<bool> _qvectb_correlScoreCheckBoxStateWhenGroupEnabled_PX1PX2Together_DeltaZAlone;

    //use 3 as size because using enum eLayerAccess as index to access it
    QLineEdit* _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[3];

    S_InputFiles _inputFiles;
};

#endif // DIALOG_PROJECT_INPUTFILES_viewContentOnly_H
