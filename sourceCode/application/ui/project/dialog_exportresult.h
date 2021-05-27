#ifndef DIALOG_EXPORTRESULT_H
#define DIALOG_EXPORTRESULT_H

#include <QDialog>

#include "../../logic/model/core/exportResult.h"

namespace Ui {
class Dialog_exportResult;
}

class Dialog_exportResult : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_exportResult(QWidget *parent = nullptr);

    //void setExportSettings(const S_ExportSettings& sExportSettings);

    void setCurrentBoxId(int currentBoxId);
    void setSomeFlaggedProfilExist(bool bSomeFlaggedProfilExist_anyBoxes,
                                   bool bSomeFlaggedProfilExistForCurrentBoxId);

    //void setCurrentBoxId(int currentBoxId);
    //S_ExportSettings getExportSettings();

    ~Dialog_exportResult();

public slots:
    void slot_radioButton_json_toggled(bool bChecked);
    void slot_radioButton_ascii_toggled(bool bChecked);

    void slot_radioButton_allBoxes_toggled(bool bChecked);
    void slot_radioButton_onlyCurrentBox_toggled(bool bChecked);

    void slot_checkBox_includeProfilesCurvesData_stateChanged(int iCheckState);

    void slot_selectOutputFileAndExport();

    void slot_checkBox_setLinRegResultAsEmpty_stateChanged(int iCheckState);
    void slot_checkBox_setProfileCurveDataAsEmpty_stateChanged(int iCheckState);

signals:
    void signal_project_exportResult(S_ExportSettings sExportSettings);

private:
    void setExportSettings(const S_ExportSettings& sExportSettings);
    void updateEnableWidgetState_aboutSomeFlaggedProfilExistStates();

private:
    Ui::Dialog_exportResult *ui;

    S_ExportSettings _sExportSettings;

    bool _bSomeFlaggedProfilExist_anyBoxes;
    bool _bSomeFlaggedProfilExistForCurrentBoxId;

    //bool _bUiSetLogicFields_forsetProfileCurveDataAsEmpty;
    bool _bUiSetLogicFields_forsetAsEmpty;

};

#endif // DIALOG_EXPORTRESULT_H
