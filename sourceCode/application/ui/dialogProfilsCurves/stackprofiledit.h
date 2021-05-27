#ifndef stackprofiledit_H
#define stackprofiledit_H

#include <QGraphicsView>

#include "ui_stackprofiledit.h"

#include "logic/model/dialogProfilsCurves/CustomGraphicsScene_profil.h"
#include "logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp"

#include "../../logic/model/dialogProfilsCurves/enum_profiladjustmode.h"

namespace Ui {
    class Form;
}

class stackprofiledit : public QWidget {

  Q_OBJECT

public:

    stackprofiledit(QWidget *parent = nullptr);

    void setSceneForView(QGraphicsScene *sceneForView);
    void setTextWidgetsPartModel_andConnectModelViewSignalsSlots(StackProfilEdit_textWidgetPartModel* stackProfilEdit_textWidgetPartModelPtr);
    void feedTextWidgetsFromModel();

    void updateOnDemand();

    void clearUiContent();

    //void resizeEvent(QResizeEvent *event) override;
    //void showEvent(QShowEvent* event) override;

    ~stackprofiledit() override;

    void call_invalidateScene();

    void set_switchAdjustMode(e_ProfilAdjustMode eProfilAdjustMode);

signals:
    void signal_switchAdjustModeChanged(int ieProfilAdjustMode);
    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

public slots:
    void slot_validateEditedAdjustersSetPositionValues_evaluatingConsistency();
    void slot_feed_stackProfilEdit_textWidgets();

    void slot_checkBox_flag_toggled_fromWidget(bool checked);
    void slot_setWarnFlag(uint ieComponent, bool bWarnFlag);

private:
    void clearUi_offsetAtX0_sigmaSum();
    void clearUi_with_invalid_value();

    void validateEditedX0CenterSetPositionValue_evaluatingConsistency();
    void validateEditedAdjustersSetPositionValues_evaluatingConsistency();

    void setWarnFlag_optionnalForceWidgetCheckedState_noUpdateModel(bool bWarnFlag, bool bForceWidgetCheckedState);

    void setEnabled_aLR_aX0_flag(bool bState);

    void setCheckedWithColor_aLR_aX0_flag(bool bState);

    void setCheckedWithColor_checkBox(QCheckBox *qCheckBoxPtr, bool bState);

private:
    Ui::Form *ui;

    e_ProfilAdjustMode _eProfilAdjustMode;

    StackProfilEdit_textWidgetPartModel *_stackProfilEdit_textWidgetPartModelPtr;

    QLineEdit *_a2x2_QLineEditptr_LRCoupleOfLRxPosValues[2][2];
    double _a2x2_doubleValueOfQLineEditTextContent_LRCoupleOfLRxPosValues[2][2];
    bool _a2x2_evaluatedConsistencyFlags_LRCoupleOfLRxPosValues[2][2];

    double _doubleValueOfQLineEditTextContent_X0PosValue;
    bool _evaluatedConsistencyFlag_X0PosValue;

    bool _bUpdateModel_aboutChekBoxFlag;
};


#endif // stackprofiledit_H
