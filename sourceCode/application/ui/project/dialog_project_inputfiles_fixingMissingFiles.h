#ifndef DIALOG_PROJECT_INPUTFILES_MISSINGFILES_H
#define DIALOG_PROJECT_INPUTFILES_MISSINGFILES_H

#include <QDialog>
#include <QString>

#include "../../logic/model/core/ComputationCore_structures.h" //@#LP move S_InputFiles in a dedicated header file

class QLineEdit;

namespace Ui {
class Dialog_project_inputFiles_fixingMissingFiles;
}

class Dialog_project_inputFiles_fixingMissingFiles : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_project_inputFiles_fixingMissingFiles(QWidget *parent = nullptr);
    ~Dialog_project_inputFiles_fixingMissingFiles() override;

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

    void setToChangePossible();
    void setToNoneChangePossible_viewProjectInputOnly();

public slots:
    void slot_pushButton_correlScoreMap_selectFile();
    void slot_lineEdit_correlScoreMap_textEdited(const QString& text);

    void slot_setIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport(
            int routeId, QString strRoutesetFilename,
            QString strRouteName, bool bValid);
    void slot_lineEdit_aboutInputImageFilename_emptyStateChanged();

    void slot_pushButtonOK_clicked();

    //connected to internal widgets
    void slot_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

signals:

    void signal_project_routeSelectionRequestForImport(QString strRoutesetFilenameToUseForImport);
    void signal_pushButtonOK_clicked();
    void signal_canceled();

    //to connect to internal widgets
    void signal_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection);

private:

    void set_correlationScoreWarned_off(int intAssociatedLayerSetId);

    void setToNoValues();
    void setMode_fixingMissingFile(bool bFixingMissingFile);

    void updateWidgetsAndFieldsAboutSelectedRoute();

    bool evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization();

    void setEnable_windowPushButtonOK(bool bNewState);

    bool get_checkedState_GroupBox_DeltaZorOther();
    bool get_checkedState_GroupBox_PX1PX2();

    Ui::Dialog_project_inputFiles_fixingMissingFiles *ui;

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

    bool _bIsMissing_deltaZorOther_correlScoreMap;
    bool _bIsMissing_PX1PX2_correlScoreMap;

};

#endif // DIALOG_PROJECT_INPUTFILES_MISSINGFILES_H
