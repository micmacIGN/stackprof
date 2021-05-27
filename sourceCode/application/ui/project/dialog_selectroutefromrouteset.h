#ifndef DIALOG_SELECTROUTEFROMROUTESET_H
#define DIALOG_SELECTROUTEFROMROUTESET_H

#include <QDialog>
#include <QItemSelection>

namespace Ui {
class Dialog_selectRouteFromRouteset;
}

class RouteContainer;

class Dialog_selectRouteFromRouteset : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_selectRouteFromRouteset(QWidget *parent = nullptr);
    ~Dialog_selectRouteFromRouteset();
    void setModelPtrFortableView(RouteContainer *routeContainerPtr);
    void initConnectionModelViewSignalsSlots();

    void set_routesetFileSelected(const QString& strRoutesetFilename);

    void set_selectedRouteId(int selectedRouteId);

    //virtual void accept() override;

    int getSelectedRouteId();

public slots:

    void slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:
    void signal_selectedRouteFromRouteset(int selectedIdRoute);

private:

    void setWidgetsStates_noFileUsed_noRoute();

    //void tableview_unselectAll();

    Ui::Dialog_selectRouteFromRouteset *ui;
    RouteContainer* _routeContainerPtr;

    int _selectedRouteTableViewRow;

};

#endif // DIALOG_SELECTROUTEFROMROUTESET_H
