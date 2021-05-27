#ifndef widgetImageViewUpBar_H
#define widgetImageViewUpBar_H

#include <QGraphicsView>

#include "ui_widgetImageviewUpBar.h"

namespace Ui {
    class widgetImageViewUpBar;
}

class WidgetImageViewUpBar : public QWidget {

  Q_OBJECT

public:

    WidgetImageViewUpBar(QWidget *parent = nullptr);

    void setToNoValues();

    void setVisible_layersSwitcher(bool bVisible);

    void setLayersNamesForLayersSwitcher(const QVector<QString>& qvectStrLayersName);
    void setAvailablelayersForLayersSwitcher(const QVector<bool>& qvectb_layersToUseForCompute);

    void setStateSetOn(int idxLayerOn);

    ~WidgetImageViewUpBar() override;

    void set_enableLayerSwitching(bool bEnable);

    //set_enablePossibleActionsOnImage is about fit and show grid (excludes LayerSwitching by design)
    //the goal is the same that for set_enableLayerSwitching: enable or not depending on, typically, the loading state in logic side (scene)
    void set_enablePossibleActionsOnImage(bool bEnable);

    void switchShowGridState();

signals:  
    void signal_layersSwitcher_pushButtonClicked(int ieLA);

    void signal_fitImageInView();
    void signal_showPixelGrid(bool bState);

public slots:
    void slot_layersSwitcher_pushButtonClicked(int ieLA); //from layerSwitcher widget

    void slot_pushButton_fitImageInView_clicked();
    void slot_checkBox_showPixelGrid_stateChanged(int iCheckState);

    void slot_showPixelGridIsPossible(bool bIsPossible);

private:
    //this method is to enable or not the checkbox depending on the current zoom
    //(showing or not the grid is not possible when the pixel size is too small and, hence, the grid is off in this case)
    void set_enableShowPixelGrid(bool bEnable);

private:
    Ui::widgetImageViewUpBar *ui;

    bool _bEnabled_checkBox_showPixelGrid;
};


#endif // widgetImageViewUpBar_H
