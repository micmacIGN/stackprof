#ifndef DIALOG_PROFILCURVESMEANINGS_H
#define DIALOG_PROFILCURVESMEANINGS_H

#include <QDialog>

namespace Ui {
class Dialog_profilCurvesMeanings;
}

class Dialog_profilCurvesMeanings : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_profilCurvesMeanings(QWidget *parent = nullptr);
    ~Dialog_profilCurvesMeanings();

    void setToNoValues();
    void setMeanings(const QVector<bool>& qvectBool_componentToCompute,
                     const QVector<QString>& qvectstr_curveMeaning,
                     const QVector<QString>& qvectstr_enveloppeAreaMeaning);
    void setUnits(const QVector<QString>& qvectstr_xyunits);

private:
    void setToNoValues_units();
    void setToNoValues_meaning();

private:
    Ui::Dialog_profilCurvesMeanings *ui;
};

#endif // DIALOG_PROFILCURVESMEANINGS_H
