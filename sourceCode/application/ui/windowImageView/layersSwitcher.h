#ifndef LayersSwitcher_H
#define LayersSwitcher_H

#include <QGraphicsView>

#include "ui_layersSwitcher.h"

#include "../logic/model/core/ComputationCore_structures.h"

namespace Ui {
    class LayersButtonsSwitcher;
}

class LayersSwitcher : public QWidget {

  Q_OBJECT

public:

    LayersSwitcher(QWidget *parent = nullptr);

    ~LayersSwitcher() override;

    void updateButtonsAvailability(const QVector<bool>& qvectsAvailableLayers);

    void setButtonsName(const QHash<int, QString>& qhash_int_qstr);
    bool setStateSetOn(int idxLayerOn);

    void set_enableLayerSwitching(bool bEnable);
    //void clearState();

public slots:

    void slot_tabWidget_index_currentChanged(int index);

signals:
    void signal_pushButtonClicked(int ieLA);

private:

    void switch_pushButton_forNewState(QWidget* pushButtonPtr_ofNewState);

private:
    Ui::LayersButtonsSwitcher *ui;

    QVector<bool> _qvectsAvailableLayers;

    QVector<QWidget*> _qvect_widgetsForTabWidgets;

    QHash<int, QString> _qhash_int_qstrTabTitle;

    int _ieLA_currentLayer;
    bool _bUpdatingButtonsAvailability;

    bool _bEmitSignalAboutLayerSwitch;
};


#endif // LayersSwitcher_H
