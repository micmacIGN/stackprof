#ifndef ComponentsSwitcher_H
#define ComponentsSwitcher_H

#include <QGraphicsView>

#include "ui_componentsSwitcher.h"

#include "e_hexaCodeLayerEditProfil.hpp"

namespace Ui {
    class ComponentsButtonsSwitcher;
}

class ComponentsSwitcher : public QWidget {

  Q_OBJECT

public:

    ComponentsSwitcher(QWidget *parent = nullptr);

    ~ComponentsSwitcher() override;

    void updateButtonsAvailability_setDefaultState(const QVector<bool>& qvectbLayersComputed, bool bSetToDefaultState);

    void setButtonsName(const QHash<e_hexaCodeComponentEditProfil, QString> qhash_e_hxCCEP_qstr);

    void clearState();

public slots:
    void slot_pushButtonClicked();

signals:
    void signal_pushButtonClicked(int iehxCLEP);

private:
    void setVisibleButtonForHashKey(int key, bool bVisible);

    void switch_pushButton_forNewState(QPushButton* pushButtonPtr_ofNewState);

private:
    Ui::ComponentsButtonsSwitcher *ui;

    QPushButton* _qpushButtonPtr_ofPreviousState;

    QVector<bool> _qvectbLayersComputed;

    QHash<int, QPushButton*> _qhash_int_PushButtonsPtr;

};


#endif // ComponentsSwitcher_H
