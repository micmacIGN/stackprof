#ifndef DIALOG_SELECTONEOFLAYERS_H
#define DIALOG_SELECTONEOFLAYERS_H

#include <QDialog>

#include <QRadioButton>

namespace Ui {
class Dialog_selectOneOfLayers;
}

class Dialog_selectOneOfLayers : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_selectOneOfLayers(QWidget *parent = nullptr);
    ~Dialog_selectOneOfLayers();

    void setAvailableLayers(const QVector<bool>& qvectBAvailableLayers);
    void setLayersName(const QVector<QString> qVectStr);

    int getSelected();

public slots:
    void slot_radioButtonClicked();

private:
    Ui::Dialog_selectOneOfLayers *ui;

    QRadioButton *_tRadioButtonPtr[3];
    int _selected_iLayer;
};

#endif // DIALOG_SELECTONEOFLAYERS_H
