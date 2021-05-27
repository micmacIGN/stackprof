#ifndef Widget_correlScoreMapParameters_h
#define Widget_correlScoreMapParameters_h

#include <QWidget>

#include "../logic/model/core/ComputationCore_structures.h"

namespace Ui {
class Widget_correlScoreMapParameters;
}

class Widget_correlScoreMapParameters : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_correlScoreMapParameters(QWidget *parent = nullptr);

    void feed(bool bAvailableForEdit,
              const QString& strCorrelScoreMapFileUsed,
              const S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters);

    bool get_correlScoreMapParameters(S_CorrelationScoreMapParameters& sCorrelationScoreMapParameters);

    ~Widget_correlScoreMapParameters();

    bool inputValid(bool& bRejectionValueValid);

public slots:
    void slot_checkBox_use(int iCheckState);
    void slot_checkBox_useRejectionThreshold_stateChanged(int iCheckState);
    void slot_checkBox_usedWeighting_stateChanged(int iCheckState);

    void slot_lineEdit_rejectionValue_rejectIfBelow_textEdited(const QString& text);

    void slot_spinBox_weighting_exponent_valueChanged(int value);

signals:
    void signal_rejectionValue_validityChanged();

private:
    void feedWithEmptyDefault();

    void setUiEnableAndChecked_option_rejection(bool bEnable, bool bChecked);
    void setUiEnableAndChecked_option_weightingAdjustement(bool bEnable, bool bChecked);

    void setUiEnable_option_rejection();
    void setUiEnable_option_weightingAdjustement();

    void setUiValueFromFloat_lineEdit_rejectionValue_rejectIfBelow(float fValue);
    void setUiValueFromFloat_spinBox_weighting_exponent(float fValue);

    bool updateFieldAndWidget(const QString& text, QString& qstr, bool &bValid);

    void update_StyleSheet_lineEdit_rejectionValue_rejectIfBelow(bool bChecked);

private:
    Ui::Widget_correlScoreMapParameters *ui;

    bool _bUiSetLogicFields;

    S_CorrelationScoreMapParameters _corrScoreMapParameters_inEdition;

    QString _qstrRejectionValue;
    bool _bqstrRejectionValueValid;

};

#endif // Widget_correlScoreMapParameters_h
