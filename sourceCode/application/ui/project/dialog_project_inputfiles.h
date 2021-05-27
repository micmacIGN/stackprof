#ifndef DIALOG_PROJECT_INPUTFILES_H
#define DIALOG_PROJECT_INPUTFILES_H

#include <QDialog>
#include <QString>

#include "../../logic/model/core/ComputationCore_structures.h" //@#LP move S_InputFiles in a dedicated header file

class QLineEdit;

namespace Ui {
class Dialog_project_inputFiles;
}

class Dialog_project_inputFiles : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_project_inputFiles(QWidget *parent = nullptr);
    ~Dialog_project_inputFiles() override;

    void setStateAsEmptyProject();

    void setStrStartingPathForFileSelection(const QString& strStartingPathForFileSelection);
    QString getStrStartingPathForFileSelection();

    S_InputFiles get_inputFiles_imageAndCorrelationScoreMap();
    //bool get_qvectDequantizationStep_PX1_PX2_deltaZ(QVector<float>& qvectDequantizationStep_PX1_PX2_deltaZ);
    void setEditedProjectState_missingFiles_lockedRoute(const S_InputFiles& inputFiles,
                                                        const QVector<bool>& qvectb_layersToCompute,
                                                        const QVector<bool>& qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ,
                                                        const QVector<bool>& qvectb_missingFiles_inputFileCorrelationScore_PX1_PX2_DeltaZ,
                                                        const QString& strRouteName);

    void accept() override;
    void reject() override;

    //bool get_bUseGeoreferencing();

    void clearTraceSetAndTraceSelection_forRouteImport();
    void forceState_bUseGeoref(bool bUseGeoRefForProject);

public slots:
    void slot_pushButton_correlScoreMap_selectFile();
    void slot_lineEdit_correlScoreMap_textEdited(const QString& text);

    void slot_checkBox_createTraceLater_stateChanged(int state);

    void slot_pushButton_routesetFilename();
    void slot_lineEdit_routesetFilename_textEdited(const QString &text);

    void slot_pushButton_selectRoute();

    void slot_setIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport(
            int routeId, QString strRoutesetFilename,
            QString strRouteName, bool bValid);
    void slot_lineEdit_aboutInputImageFilename_emptyStateChanged();

    void slot_pushButtonOK_clicked();

    void slot_groupBox_PX1PX2_toggled(bool bChecked);
    void slot_groupBox_DeltaZorOther_toggled(bool bChecked);

    //connected to internal widgets
    void slot_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

    void slot_PX1PX2_correlScoreMap_checkBoxStateChanged(int iCheckState);
    void slot_deltaZorOther_correlScoreMap_checkBoxStateChanged(int iCheckState);

    void slot_checkBox_useGeoreferencing_stateChanged(bool bState);

signals:

    void signal_project_routeSelectionRequestForImport(QString strRoutesetFilenameToUseForImport);
    void signal_project_addADefaultRouteStateChanged(bool bActivated);

    void signal_pushButtonOK_clicked();
    void signal_canceled();

    //to connect to internal widgets
    void signal_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

    void signal_project_useGeoreferencing_stateChanged(bool bUseGeoreferencing);

    void signal_selected_filename_routeset_forRouteImport_hasChanged();
private:

    void setWidgets_checkedEnabledStatus_for_correlScoreMap_PX1PX2(bool bCheckedState, bool bEnabled);
    void refillUiState_about_correlScoreMap_PX1PX2_ifRequiered(bool bCheckedState);

    void setWidgets_checkedEnabledStatus_for_correlScoreMap_deltaZOrOther(bool bCheckedState, bool bEnabled);
    void refillUiState_about_correlScoreMap_deltaZAlone_ifRequiered(bool bCheckedState);

    void set_correlationScoreWarned_off(int intAssociatedLayerSetId);

    void setToNoValues();
    void setMode_fixingMissingFile(bool bFixingMissingFile);

    void updateWidgetsAndFieldsAboutSelectedRoute();

    bool evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization();


    void setEnable_windowPushButtonOK(bool bNewState);

    bool get_checkedState_GroupBox_DeltaZorOther();
    bool get_checkedState_GroupBox_PX1PX2();

    Ui::Dialog_project_inputFiles *ui;

    bool _bUiSetLogicFields;

    bool _bFixingMissingFile;
    bool _tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone[3];

    QVector<bool> _qvectb_layersToCompute;
    QVector<bool> _qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ;
    QVector<bool> _qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone;

    QVector<bool> _qvectb_correlScoreCheckBoxStateWhenGroupEnabled_PX1PX2Together_DeltaZAlone;

    //use 3 as size because using enum eLayerAccess as index to access it
    QLineEdit* _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[3];

    S_InputFiles _inputFiles;

    QString _strFilename_routeset_forRouteImport;

    bool _bAddADefaultTrace;

    bool _bOneRouteSelectedFromFile;
    int  _selectedRouteId;
    QString _strNameOfSelectedRoute;

    QString _strStartingPathForFileSelection;

    bool _bUseGeoreferencing;

    bool _about_bUseGeoreferencing__bForcedState_from_outside;


};

#endif // DIALOG_PROJECT_INPUTFILES_H
