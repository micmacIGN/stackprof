#ifndef dialogVLayoutStackProfilEdit_H
#define dialogVLayoutStackProfilEdit_H

#include <QDialog>

#include "../../logic/model/dialogProfilsCurves/enum_profiladjustmode.h"

namespace Ui {
    class dialogVLayoutStackProfilEdit;
}

class CustomGraphicsScene_profil;
class StackProfilEdit_textWidgetPartModel;
class stackprofiledit;

class DialogVLayoutStackProfilEdit : public QDialog
{
    Q_OBJECT

public:
    DialogVLayoutStackProfilEdit(QWidget *parent = nullptr);
    ~DialogVLayoutStackProfilEdit() override;

   void resizeEvent(QResizeEvent *event) override;

   void setCGScenePtr(const QVector<CustomGraphicsScene_profil*> &qvectPtrSmallImageNav_CustomGraphicsScene);
   void setTextWidgetPartModelPtr(const QVector<StackProfilEdit_textWidgetPartModel*> &qvectPtrStackProfilEdit_textWidgetPartModel);

   void clearUiContent_all();
   void clearUiContent_profilCurvesGraphics_and_xlimitsAndLinearRegrModelResults();

   void feedTextWidgets();

   bool initConnectionModelViewSignalsSlots();

   void updateOnDemand_onQvectStackprofiledit();

   void setLabelAndButtonsStateCurrentBoxId(int boxId, int boxCount);

   void keyPressEvent(QKeyEvent *event) override;

public slots:

   void slot_setComponentsPossiblyVisible(QVector<bool> qvectComputedComponents);

   //internal: layersSwitcher::signal to DialogVLayoutStackProfilEdit::slot
   void slot_componentsSwitcherPushButtonClicked(int iehxCLEP);

   void slot_pushButton_nextBox_clicked();
   void slot_pushButton_previousBox_clicked();
   void slot_checkBox_syncYRange_stateChanged(int iCheckState);

   //connected to logic AppMmodel:
   void slot_setComponentsVisible_fromQvectBoolComponents(QVector<bool> qvectBoolLayers);

   void slot_pushButton_meaning_clicked();

   void slot_pushButton_switchAdjustMode_clicked();

   void slot_boxIdToReachFromEditedId(int boxId);

   void slot_locationInAdjustement(bool bState);

signals:
    //@LP there is no reason to send bShowWindowLayoutStackProfilEditOnTopAndGiveFocusToIt at false from DialogVLayoutStackProfilEdit
    void signal_switchToBox(int boxId, bool bShowWindowLayoutStackProfilEditOnTopAndGiveFocusToIt=true);

    void signal_componentsToShowChanged(QVector<bool> qvectBoolLayersToShow);
    void signal_syncYRange_stateChanged(bool bChecked);

    void signal_profilAdjustMode_changed(int ieProfilAdjustMode);//inform model

private:
   void setComponentsPossiblyVisibleAndUpdateComponentsSwitcher_fromQvectComponentsComputed(const QVector<bool>& qvectComputedComponents);

   bool setComponentsVisible_fromQvectBoolComponents(const QVector<bool>& qvectBoolComponents);

   void switchAdjustMode();
   void set_switchAdjustMode(e_ProfilAdjustMode eProfilAdjustMode);

private:

    Ui::dialogVLayoutStackProfilEdit *ui;

    e_ProfilAdjustMode _eProfilAdjustMode;
    bool _bSwitchAdjustMode_actionIsPossible;

    QVector<CustomGraphicsScene_profil*> _qvectPtrCustomGScene_Perp_Parall_deltaZ;
    QVector<StackProfilEdit_textWidgetPartModel*> _qvectPtrStackProfilEdit_textWidgetPartModel_Perp_Parall_deltaZ;

    QVector<stackprofiledit*> _qvectStackprofileditPtr;

    QVector<bool> _qvectBoolComponentsPossiblyVisible_Perp_Parall_deltaZ;
    QVector<bool> _qvectBoolComponentsVisible_Perp_Parall_deltaZ;
    bool _bQvectBoolCompVisible_PPdZ_previouslySetOnAValidCombination;

    QString _qstrUnits;

    int _boxCount;
    int _boxId;

};

#endif // dialogVLayoutStackProfilEdit_H
