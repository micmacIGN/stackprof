#ifndef DIALOG_MICMACSTEPVALUEANDSPATIALRESOLUTION_H
#define DIALOG_MICMACSTEPVALUEANDSPATIALRESOLUTION_H

#include <QDialog>

class Widget_MicMacStepValueAndSpatialResolution;

namespace Ui {
class Dialog_MicMacStepValueAndSpatialResolution;
}

class Dialog_MicMacStepValueAndSpatialResolution : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_MicMacStepValueAndSpatialResolution(QWidget *parent = nullptr);
    ~Dialog_MicMacStepValueAndSpatialResolution() override;

    bool initLayersFilenameAndSetVisibility(
                const QVector<QString>& qvectStr_inputFile_PX1_PX2_DeltaZ,
                const QVector<bool> &qvectb_layersToCompute);

    bool initLayersmicMacStepValueAndSpatialResolution(
            const QVector<QString>& qvectStr_micMacStepValue_PX1_PX2_DeltaZ,
            const QVector<QString>& qvectStr_spatialResolution_PX1_PX2_DeltaZ,
            const QVector<bool>& qvectb_layersToCompute);

    bool get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ(
                            QVector<float>& qvect_micmacStep_PX1_PX2_deltaZ,
                            QVector<float>& qvect_spatialResolution_PX1_PX2_deltaZ) const;

    bool get_micmacStepAndSpatialResolution_PX1_PX2_deltaZ_asString(
            QVector<QString>& qvectStr_micmacStep_PX1_PX2_deltaZ,
            QVector<QString>& qvectStr_spatialResolution_PX1_PX2_deltaZ) const;


public slots:
    void slot_aboutLineEdit_validityChanged();

private:
    void setEnable_windowPushButtonOK(bool bNewState);
    bool evaluateIfParametersValid();

private:
    Ui::Dialog_MicMacStepValueAndSpatialResolution *ui;

    Widget_MicMacStepValueAndSpatialResolution *_widgetPtr_PX1_PX2_deltaZ[3];
    QVector<bool> _qvectb_layersToCompute;
};

#endif // DIALOG_MICMACSTEPVALUEANDSPATIALRESOLUTION_H
