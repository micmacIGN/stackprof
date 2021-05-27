#ifndef ROUTECONTAINER_H
#define ROUTECONTAINER_H

#include <QObject>
#include <QLinkedList>

#include <QAbstractTableModel>
#include <QItemSelection>

#include "route.h"

#include <QJsonArray>

class RouteContainer : public QAbstractTableModel {

    Q_OBJECT

public:
    RouteContainer(QObject *parent = nullptr);    

    bool allRoutesHaveSegment(QString& strDetailsAboutFail) const;

    bool toQJsonArray(QJsonArray& qjsonArray_routeset, QString& strDetailsAboutWriteFail) const;
    bool fromQJsonArray(const QJsonArray& qjsonArray_routeset);


    //void getModelIndex_TL_BR(QModelIndex& qmiTopLeft, QModelIndex& qmiBottomRight);

    //---------------------------------
    //QAbstractTableModel side -------- //@#LP split using inheritance ? (like class RouteContainerWithTableModel : public RouteContainer, public QAbstractTableModel ?)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void aboutAbstractTableModel_removeRoute(int idRoute);
    void aboutAbstractTableModel_addNewRoute(QString strRouteName);

private:
    int _indexBox; //@#LP seems not really used

signals:
    void signal_errorFromModel(QString strError);
    void signal_routeAddedSuccessfully(int idRouteAdded);
    void signal_routeRemovedSuccessfully(int routeCountAfterRemove, int idRouteRemoved);

    void signal_pointOfRouteMoved();
    void signal_pointOfRouteRemoved();

    void signal_routeAdded(int addedRouteId);
    void signal_routeSelectionChanged(int routeId);

    void signal_routeCountAdded(int routeCount);


private:
    //id refers to index in qvector

    int addRoute(Route r);

    int  getNextAvailableRouteId();
    void update_qvectRoutePtr_orderedByKey();

public:

    void clear();
    int addRoute(); //#LP in public for test purpose; use inheritence class for test instead

    bool tryAddPointAtRouteEnd(int routeId, QPointF qpf, bool& bThisIsTheFirstPointForTheRoute);
    bool tryInsertPointInRoute(int routeId, QPointF qpfNoZLI, int currentZLI, bool& bThisIsTheFirstPointForTheRoute);

    bool checkRouteIdExists(int routeId) const;

    bool removeRoute(int routeId); //return success report
    bool renameRoute(int routeId, const QString& strRouteName);

    bool removeAllRouteExceptOne(int routeIdToKeep);

    bool removePointOfRoute(int idPoint, int routeId, int& nbRemainingPointInRoute);

    Route& getRouteRef(int routeId); //return a ref on _invalidRoute if routeId is invalid

    QVector<int> qvectOfRouteId() const;

    int routeCount() const;

    void showContent() const;    

    //for debug purpose:
    void getAllPolygonsAreasAroundSegments_forDebugDisplay(int idRoute,
                                                           QVector<QPolygonF>& qvectPolygonsAreasAroundSegments);

signals:
    //because we have to avoid to have distance between two successive point < 1.0, the ui needs to restore the initial location of a moved point rejected by the model
    void signal_setGraphicsLocationOfPointForRoute(int idPoint, QPointF pos, int idRoute);

public slots:
    void slot_updateModel_setPointOfRoute(int idPoint, QPointF qpfnewLocation, int idRoute);
    void slot_updateModel_removePointOfRoute(int idPoint, int idRoute);

    void slot_selectionChangedInTableView(int idRoute);

private:

    Route _invalidRoute;
    int _nextRouteIdAvailable;

    //@LP using QMap to have key ordered, to be able to automatically have nice order in view
    //QMap<int, Route> _qmap_route;
    QLinkedList<Route> _qlinkedlist_route;
    QVector<Route*> _qvectRoutePtr_syncWithqlinkedlist; //store ptr on route in the _qlinkedlist_route for quick access to update tableview

    int _modelColumnCount = 3;
};

#endif // ROUTECONTAINER_H
