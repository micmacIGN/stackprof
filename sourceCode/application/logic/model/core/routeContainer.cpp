#include <QDebug>

#include "route.h"
#include "routeContainer.h"

using namespace std;

RouteContainer::RouteContainer(QObject *parent): QAbstractTableModel(parent), _indexBox(-1), _nextRouteIdAvailable(0) {

}

bool RouteContainer::allRoutesHaveSegment(QString& strDetailsAboutFail) const {

    int nbRoute = routeCount();
    if (!nbRoute) {
        strDetailsAboutFail = "None trace";
        return(false);
    }
    bool bSomeRouteHaveNoSegment = false;
    for (int i = 0; i < nbRoute; i++) {
        if (_qvectRoutePtr_syncWithqlinkedlist[i]->segmentCount() < 1) {
            qDebug() << __FUNCTION__
                     << "Error: segment count < 1 for route [ " << i << " ]"
                     << " trace #" << _qvectRoutePtr_syncWithqlinkedlist[i]->getId()
                     << " ( " << _qvectRoutePtr_syncWithqlinkedlist[i]->getName() << " )";
            bSomeRouteHaveNoSegment = true;
        }
    }
    if (bSomeRouteHaveNoSegment) {
        strDetailsAboutFail = "some trace(s) have less than two points. Remove these traces or add points to them";
        return(false);
    }
    return(true);
}

bool RouteContainer::toQJsonArray(QJsonArray& qjsonArray_routeset, QString& strDetailsAboutFail) const {

    qDebug() << __FUNCTION__ ;

    int nbRoute = routeCount();

    if (!nbRoute) {
        qDebug() << __FUNCTION__ << "Error: no trace";
        strDetailsAboutFail = "None trace";
        return(false);
    }

    if (!allRoutesHaveSegment(strDetailsAboutFail)) {
        return(false);
    }

    for (int i = 0; i < nbRoute; i++) {
        QJsonObject qjsonOb_oneRoute;
        bool bReport = _qvectRoutePtr_syncWithqlinkedlist[i]->toQJsonObject(qjsonOb_oneRoute);
        if (!bReport) {
            strDetailsAboutFail = "route point vector toQJsonObject failed formatting double"; //@LP be sure to update this detail if code change about it
            return(false);
        }
        qjsonArray_routeset.insert(i, qjsonOb_oneRoute);
    }
    qDebug() << __FUNCTION__ << "qjsonArray_routeset = " << qjsonArray_routeset;
    return(true);
}

bool RouteContainer::fromQJsonArray(const QJsonArray& qjsonArray_routeset) {

    qDebug() << __FUNCTION__ ;

    clear();

    bool bLoadingError = false;

    emit layoutAboutToBeChanged();

    int idx = 0;
    for(const auto iter: qjsonArray_routeset) {
        QJsonObject qjsonRoute = iter.toObject();
        Route routeFromQjson;
        bool bReport = routeFromQjson.fromQJsonObject(qjsonRoute);
        if (!bReport) {

            bLoadingError = true;
        }
        if (routeFromQjson.getId() != idx) {
            qDebug() << __FUNCTION__ << " erro getting trace #" << idx << " got routeId: " << routeFromQjson.getId();

            bLoadingError = true;
        }
        if (bLoadingError) {
            break;
        }

        _qlinkedlist_route.push_back(routeFromQjson);
        idx++;
    }

    if (bLoadingError) {
        qDebug() << __FUNCTION__  << "error: bLoadingError";
        emit layoutChanged();
        clear();
        return(false);
    }

    _nextRouteIdAvailable = idx;
    update_qvectRoutePtr_orderedByKey();
    _indexBox = -1;

    int routeCountAfterFeed = routeCount();
    QModelIndex qmodelIndexForInsertAll = index(0, 0);

    if (!routeCountAfterFeed) {
        qDebug() << __FUNCTION__  << " error: routeCountAfterFeed is 0";
        emit layoutChanged();
        clear();
        return(false);
    }

    qDebug() << __FUNCTION__ << "qmodelIndexForRemoveAll= " << qmodelIndexForInsertAll;
    qDebug() << __FUNCTION__ << "qlinkedlist_route_size= " << routeCountAfterFeed;
    qDebug() << __FUNCTION__ << "beginInsertRows(..., = " << 0 << ", " << routeCountAfterFeed-1;

    int lastIndex = routeCountAfterFeed-1;

    beginInsertRows(qmodelIndexForInsertAll, 0, lastIndex);

    endInsertRows();

    emit layoutChanged();

    emit signal_routeCountAdded(routeCountAfterFeed);

    qDebug() << __FUNCTION__ << "ending";

    return(true);
}



void RouteContainer::aboutAbstractTableModel_addNewRoute(QString strRouteName) {

    Route routeToAdd;
    if (strRouteName.isEmpty()) {
        strRouteName = Route::defaultStrRouteName();
    }
    routeToAdd.setName(strRouteName);

    int routeCountBeforeInsertion = routeCount();
    QModelIndex qmodelIndexForInsert = index(routeCountBeforeInsertion, 0);

    emit layoutAboutToBeChanged();

    beginInsertRows(qmodelIndexForInsert, routeCountBeforeInsertion, routeCountBeforeInsertion);
    int addRouteId = addRoute(routeToAdd);
    routeToAdd.showContent();

    endInsertRows();

    emit layoutChanged();

    showContent();

    if (addRouteId == -1) {
        qDebug() << __FUNCTION__ << "Error: failed to add Route";
        //@#LP
        emit signal_errorFromModel("Error: failed to add Route");
    } else {
        //from logic to logic
        emit signal_routeAddedSuccessfully(addRouteId);
    }
}

void RouteContainer::aboutAbstractTableModel_removeRoute(int idRoute) {

    qDebug() << __FUNCTION__ << "idRoute = " << idRoute;

    int qlinkedlist_route_size = _qlinkedlist_route.size();
    if (!qlinkedlist_route_size) {
        qDebug() << __FUNCTION__ << " error: RouteContainer has no route";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
        return;
    }

    bool bHasIndex =  hasIndex(idRoute,0);
    if (!bHasIndex) {
        qDebug() << __FUNCTION__ << " error: hasIndex(idRoute,0) said false for idRoute:" << idRoute;
        return;
    }

    QModelIndex qmodelIndexForRemove =  index(idRoute, 0);

    emit layoutAboutToBeChanged();

    beginRemoveRows(qmodelIndexForRemove, idRoute, idRoute);

    removeRoute(idRoute);

    endRemoveRows();

    emit layoutChanged();

    emit signal_routeRemovedSuccessfully(routeCount(), idRoute);

    showContent();

}


int RouteContainer::rowCount(const QModelIndex &parent) const {
    qDebug() << __FUNCTION__ << "RouteContainer::rowCount =>" << routeCount();
    return(routeCount());
}

int RouteContainer::columnCount(const QModelIndex &parent) const  {
    return(_modelColumnCount); //row value, nbpoints in route, and title defined by the user
}

QVariant RouteContainer::data(const QModelIndex &index, int role) const {
    int indexRow = index.row();
    //'Note: If you do not have a value to return, return an invalid QVariant instead of returning 0.'
    QVariant qvariantEmpty;

   qDebug() << __FUNCTION__ << "RouteContainer::data indexRow = " << indexRow;

    if (role == Qt::DisplayRole) {
        if (indexRow < routeCount()) {
            switch(index.column()) {
            case 0:
                qDebug() << __FUNCTION__ << "RouteContainer::data column 0";

                qDebug() << __FUNCTION__ << "RouteContainer::data return now QString::number(indexRow) :" << QString::number(indexRow);
                return(QString::number(indexRow));
                break;

            case 1:
                qDebug() << __FUNCTION__ << "RouteContainer::data _qvectRoutePtr_orderedByKey[indexRow]->getTitle() :" << _qvectRoutePtr_syncWithqlinkedlist[indexRow]->getName();
                return(_qvectRoutePtr_syncWithqlinkedlist[indexRow]->pointCount());
                break;

            case 2:
                qDebug() << __FUNCTION__ << "RouteContainer::data _qvectRoutePtr_orderedByKey[indexRow]->getTitle() :" << _qvectRoutePtr_syncWithqlinkedlist[indexRow]->getName();
                return(_qvectRoutePtr_syncWithqlinkedlist[indexRow]->getName());
                break;
            default:;
            }
        }
    }
    return(qvariantEmpty);
}

QVariant RouteContainer::headerData(int section, Qt::Orientation orientation, int role) const {
    //Returns the data for the given role and section in the header with the specified orientation.
    //For horizontal headers, the section number corresponds to the column number. Similarly, for vertical headers, the section number corresponds to the row number.
    QVariant qvariantEmpty;
    if (role != Qt::DisplayRole)    { return(qvariantEmpty); }
    if (orientation == Qt::Vertical) { return(qvariantEmpty); }
    //if (section!= 0)                 { return(qvariantEmpty); }
    qDebug() << __FUNCTION__ << "section:" << section
             << "orientation:" << orientation
             << "role:" << role;

    switch(section) {
        case 0: return("Index"); //break;
        case 1: return("Nb points"); //break;
        case 2: return("Title"); //break;
        default:
            break;
    }

    return(qvariantEmpty);
}

void RouteContainer::slot_selectionChangedInTableView(int idRoute) {
    _indexBox = idRoute;
    emit signal_routeSelectionChanged(_indexBox);
}

/*
void RouteContainer::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    qDebug() << __FUNCTION__  << "selected  :" << selected;
    qDebug() << __FUNCTION__  << "deselected:" << deselected;
    _indexBox = selected.indexes().first().row();

    emit signal_routeSelectionChanged(_indexBox);
}
*/

void RouteContainer::update_qvectRoutePtr_orderedByKey() {

    _qvectRoutePtr_syncWithqlinkedlist.clear();
    int i = 0;
    for (auto iter = _qlinkedlist_route.begin();
         iter != _qlinkedlist_route.end();
         ++iter) {
        //qDebug() << "i = " << i << " =>";
        //cstIter->showContent();
        _qvectRoutePtr_syncWithqlinkedlist.push_back(&(*iter));
        qDebug() << __FUNCTION__ << "i = " << i << " =>@" << (void*)(&(*iter));
        i++;
    }
}

int RouteContainer::addRoute() {
    return(addRoute(Route()));
}

int RouteContainer::getNextAvailableRouteId() {
    return(_nextRouteIdAvailable);
}

int RouteContainer::routeCount() const {
    return(_qlinkedlist_route.size());
}


QVector<int> RouteContainer::qvectOfRouteId() const {
    QVector<int> qvectOfRouteId;

    for (auto iter: _qvectRoutePtr_syncWithqlinkedlist) {
        qvectOfRouteId << iter->getId();
    }

    qDebug() << __FUNCTION__ << " qvectOfRouteId =" << qvectOfRouteId;

    return(qvectOfRouteId);
}


int RouteContainer::addRoute(Route r) {
    if (checkRouteIdExists(_nextRouteIdAvailable)) {
        return(-1);
    }
    int idForAddedRoute = _nextRouteIdAvailable;
    qDebug() << __FUNCTION__ << " will insert with idForAddedRoute = " << idForAddedRoute;
    r.setId(idForAddedRoute);

    _qlinkedlist_route.push_back(r);
    qDebug() << __FUNCTION__ << " inserted";
    _nextRouteIdAvailable++;
    /*for (auto cstIter = _qmap_route.cbegin();
         cstIter != _qmap_route.cend();
         ++cstIter) {
        qDebug() << __FUNCTION__ << " >@" << (void*)(&(cstIter.value()));
    }*/
    for (auto iter = _qlinkedlist_route.begin();
         iter != _qlinkedlist_route.end();
         ++iter) {
        qDebug() << __FUNCTION__ << " =>@" << (void*)(&(*iter));
    }

    update_qvectRoutePtr_orderedByKey();

    showContent();

    return(idForAddedRoute);
}

bool RouteContainer::renameRoute(int routeId, const QString& strRouteName) {

    if (!checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << " checkRouteIdExists said false: routeId = " << routeId;
        return(false);
    }
    qDebug() << __FUNCTION__ << "routeId = " << routeId << "new strRouteName: " << strRouteName;
    _qvectRoutePtr_syncWithqlinkedlist[routeId]->setName(strRouteName);
    return(true);
}

//this method removeRoute(int routeId) updates the routeIds.
//Hence, used this method only about case like "remove one specfici route from the container",
//and Never about cases like "remove all routes except one with a specific routeId"
bool RouteContainer::removeRoute(int routeId) { //return success report

    if (!checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << " checkRouteIdExists said false: routeId = " << routeId;
        return(false);
    }
    qDebug() << __FUNCTION__ << "routeId = " << routeId;

    Route *routePtr = _qvectRoutePtr_syncWithqlinkedlist[routeId];
    _qvectRoutePtr_syncWithqlinkedlist.remove(routeId);//@LP (update_qvectRoutePtr_orderedByKey done below)

    _qlinkedlist_route.removeOne(*routePtr);

    //update routeIds to fill the gap
    int updading_routeId = 0;
    for(auto& iter: _qlinkedlist_route) {
        iter.setId(updading_routeId);
        updading_routeId++;
    }
    _nextRouteIdAvailable--; //we removed one

    qDebug() << __FUNCTION__ << updading_routeId << " should be equal to " << _nextRouteIdAvailable;

    showContent();

    update_qvectRoutePtr_orderedByKey();
    return(true);
}

//this method is used for route importation in a (new) project
bool RouteContainer::removeAllRouteExceptOne(int routeIdToKeep) { //return success report

    if (!checkRouteIdExists(routeIdToKeep)) {
        qDebug() << __FUNCTION__ << " checkRouteIdExists said false: routeIdToKeep = " << routeIdToKeep;
        return(false);
    }
    qDebug() << __FUNCTION__ << "routeIdToKeep = " << routeIdToKeep;

    //Route *routeToKeepPtr =  _qvectRoutePtr_syncWithqlinkedlist[routeIdToKeep];

    int rCount = routeCount();
    for(int rId = 0; rId < rCount; rId++) {
        if (rId != routeIdToKeep) {
            Route *routePtrToRemove = _qvectRoutePtr_syncWithqlinkedlist[rId];
            _qlinkedlist_route.removeOne(*routePtrToRemove);
        }
    }

    rCount = routeCount();
    //critical case which never happens
    if (rCount != 1) {
        _qlinkedlist_route.clear();
        update_qvectRoutePtr_orderedByKey();
        _nextRouteIdAvailable = 0;
        return(false);
    }

    //update routeId
    _qlinkedlist_route.first().setId(0);
    _nextRouteIdAvailable = 1;
    update_qvectRoutePtr_orderedByKey();

    showContent();

    return(true);
}


void RouteContainer::clear() {

    //@LP
    qDebug() << __FUNCTION__ << "Be sure to call clear on RouteContainer with any matching graphics item synced";

    int qlinkedlist_route_size = _qlinkedlist_route.size();
    if (!qlinkedlist_route_size) {
        qDebug() << __FUNCTION__ << "RouteContainer already cleared";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
        return;
    }

    emit layoutAboutToBeChanged();

    QModelIndex qmodelIndexForRemoveAll =  index(0, 0);

    qDebug() << __FUNCTION__ << "qmodelIndexForRemoveAll= " << qmodelIndexForRemoveAll;
    qDebug() << __FUNCTION__ << "qlinkedlist_route_size= " << qlinkedlist_route_size;
    qDebug() << __FUNCTION__ << "beginRemoveRows(..., = " << 0 << ", " << qlinkedlist_route_size-1;

    int lastIndex = qlinkedlist_route_size-1;

    beginRemoveRows(qmodelIndexForRemoveAll, 0, lastIndex);

    _indexBox = -1;
    _nextRouteIdAvailable = 0;

    _qlinkedlist_route.clear();

    _qvectRoutePtr_syncWithqlinkedlist.clear();

    endRemoveRows();

    emit layoutChanged();

    emit signal_routeRemovedSuccessfully(0, 0);

    showContent();
}

bool RouteContainer::removePointOfRoute(int idPoint, int routeId, int& nbRemainingPointInRoute) {

    qDebug() << __FUNCTION__;

    Route& routeRef = getRouteRef(routeId);
    if (routeRef.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: invalid idRoute (" << routeId;
        return(false);
    }

    if (!routeRef.removePoint(idPoint))  {
        qDebug() << __FUNCTION__ << "error: invalid idPoint (" << idPoint;
        return(false);
    }

    nbRemainingPointInRoute = routeRef.pointCount();
    return(true);
}

bool RouteContainer::tryAddPointAtRouteEnd(int routeId, QPointF qpf, bool& bThisIsTheFirstPointForTheRoute) {

    qDebug() << __FUNCTION__ << " qpf: " << qpf;

    if (!checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << " error: idRoute does not exist ( " << routeId  << " )";
        return(false);
    }

    Route& routeRefAddPointAtRouteEnd = getRouteRef(routeId);

    bool bAdded = routeRefAddPointAtRouteEnd.tryAddPointAtRouteEnd(qpf, bThisIsTheFirstPointForTheRoute);
    return(bAdded);
}


bool RouteContainer::tryInsertPointInRoute(int routeId, QPointF qpfNoZLI, int currentZLI, bool& bThisIsTheFirstPointForTheRoute) {
    if (!checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << "here";
        return(false);
    }
    Route& routeRef = getRouteRef(routeId);
    return(routeRef.tryInsertPointInRoute(qpfNoZLI, currentZLI, bThisIsTheFirstPointForTheRoute));
}


Route& RouteContainer::getRouteRef(int routeId) {

    qDebug() << __FUNCTION__ << "routeId = " << routeId;

    if (!checkRouteIdExists(routeId)) {
        qDebug() << __FUNCTION__ << "here";
        return(_invalidRoute);
    }
    return(*(_qvectRoutePtr_syncWithqlinkedlist[routeId]));
}

bool RouteContainer::checkRouteIdExists(int routeId) const {
    qDebug() << __FUNCTION__ << "routeId = " << routeId;
    qDebug() << __FUNCTION__ << "_qlinkedlist_route.size() = " << _qlinkedlist_route.size();
    if ((routeId < 0)||(routeId >= _qlinkedlist_route.size())) {
        return(false);
    }
    return(true);
}

void RouteContainer::showContent() const {

    qDebug() << __FUNCTION__ ;

    int i = 0;
    for (auto cstIter  = _qlinkedlist_route.cbegin();
         cstIter != _qlinkedlist_route.cend();
         ++cstIter) {
        qDebug() << __FUNCTION__ << "i = " << i << "showContent:";
        cstIter->showContent();
        i++;
    }

    qDebug() << __FUNCTION__  << " _qvectRoutePtr_syncWithqlinkedlist :";
    i = 0;
    for (auto iter : _qvectRoutePtr_syncWithqlinkedlist) {
        qDebug() << __FUNCTION__ << "_qvectRoutePtr_syncWithqlinkedlist[ "
                 << i << " ] <=> "
                 << " getId: " << iter->getId() << " ;"
                 << " pointCount: " << iter->pointCount() << " ;"
                 << " getTitle: " << iter->getName();
        i++;
    }
}

//these two operations are connected directly from the graphicItemTargetPoint
//Hence, its have to inform that something has changed in the route (use by AppFile._bModifiedSinceLastSavedToDisk typically)
void RouteContainer::slot_updateModel_setPointOfRoute(int idPoint, QPointF qpfnewLocation, int idRoute) {
    qDebug() << __FUNCTION__;
    Route& routeRef = getRouteRef(idRoute);
    if (routeRef.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: invalid idRoute (" << idRoute;
        return;
    }
    bool bEmitSignalAboutRejectedOperation = false;
    bool bReport = routeRef.setPointAt(idPoint,qpfnewLocation, bEmitSignalAboutRejectedOperation);
    if (!bReport) { //major error
        return;
    }
    if (bEmitSignalAboutRejectedOperation) {
        qDebug() << __FUNCTION__ << "error: setPointAt rejected " << idRoute;
        emit signal_setGraphicsLocationOfPointForRoute(idPoint, routeRef.getPoint(idPoint), idRoute);
    } else {
        emit signal_pointOfRouteMoved();
    }

}

void RouteContainer::slot_updateModel_removePointOfRoute(int idPoint, int idRoute) {
    qDebug() << __FUNCTION__;
    Route& routeRef = getRouteRef(idRoute);
    if (routeRef.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: invalid idRoute (" << idRoute;
        return;
    }
    if (!routeRef.removePoint(idPoint)) {
        qDebug() << __FUNCTION__ << "error: invalid idRoute (" << idRoute;
        return;
    }
    emit signal_pointOfRouteRemoved();

    qDebug() << __FUNCTION__ << " idPoint " << idPoint << "was removed from idRoute " << idRoute;
}

void RouteContainer::getAllPolygonsAreasAroundSegments_forDebugDisplay(int idRoute,
                                                                       QVector<QPolygonF>& qvectPolygonsAreasAroundSegments) {
    qDebug() << __FUNCTION__;
    Route& routeRef = getRouteRef(idRoute);
    if (routeRef.getId() == -1) {
        qDebug() << __FUNCTION__ << "error: invalid idRoute (" << idRoute;
        return;
    }

    int segmentCount = routeRef.segmentCount();
    for (int i = 0; i < segmentCount; i++) {

        S_Segment segment = routeRef.getSegment(i);
        QPointF qpointFToTest {.0,.0};

        QPolygonF qpolygfOfConsideredAreaAroundSegment;

        routeRef.rectangularShapeFromSegment_containsPoint(
                segment,
                1.0, //@LP width
                qpointFToTest,
                true,
                qpolygfOfConsideredAreaAroundSegment);

        if (qpolygfOfConsideredAreaAroundSegment.count()) {
            qvectPolygonsAreasAroundSegments << qpolygfOfConsideredAreaAroundSegment;
        }
    }
}

/*
void RouteContainer::getModelIndex_TL_BR(QModelIndex& qmiTopLeft, QModelIndex& qmiBottomRight) {

    qmiTopLeft = index(0, 0, QModelIndex());

    int size = routeCount();

    if (!size) {
        qDebug() << __FUNCTION__ << "size is zero";
        qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);

        qmiBottomRight = index(0, 0, QModelIndex());
        return;
    }

    int lastIndex = size-1;
    int lastColumn = _modelColumnCount-1;

    qDebug() << __FUNCTION__ << "size != zero";
    qDebug() << __FUNCTION__ << "hasIndex(0,0) = " << hasIndex(0,0);
    qDebug() << __FUNCTION__ << "hasIndex(lastIndex,lastColumn) = " << hasIndex(lastIndex, lastColumn);
    qDebug() << __FUNCTION__ << "lastIndex = " << lastIndex;

    qmiBottomRight = index(lastIndex, lastColumn);

}
*/
