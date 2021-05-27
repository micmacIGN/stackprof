#include <QDebug>

#include "../../core/routeContainer.h"

#include "GraphicsRouteItem.h"
#include "GraphicsItemsContainer.h"

#include "CustomGraphicsTinyTargetedPointItem.h"
#include "CustomGraphicsVectorDirectionAtPointItem.h"

#include "../customgraphicsscene_usingTPDO.h"

#include "CustomGraphicsBoxItem2.h"

#include <QVector>
#include <QHash>

#include "CustomGraphicsRectangularHighlightItem.h"

#include "CustomGraphicsTinyTargetPointItem_notDragrable2.h"

#include "GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.h"

#include "CustomGraphicsSpecificItemsOfInterest2.h"

#include "../../appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h"

GraphicsItemsContainer::GraphicsItemsContainer(): QObject(nullptr),

    _devicePixelRatioF(1.0),

    _qsizefBackgroundSize_noZLI(1.0,1.0), //default pretty invalid value
    _minimalElementSideSize_noZLI(_cstExpr_double_minimalElementSideSize_noZLI), //default value as close to 1 square diagonal length

    _currentZLI(-1),
    _ptrRouteContainer(nullptr),
    _ptrCustomGraphicsScene(nullptr),

    _ptrCGTinyTargetPtNotDragable_First_Last_Current {nullptr, nullptr, nullptr},

    _bHoverInOrOutState(false),
    _currentHoveredIdPoint(-1),
    _currentHoveredIdPoint_IsOwnedByIdRoute(-1),

    _mapBoxId_currentBoxHighlighted(-1),
    _bAddingBoxMode(false),
    _ptrGRouteItemWForbiddenSectionsFSAE(nullptr),

    _ptrCGSpecificItemsOfInterest(nullptr)
{

    //hardcoded
     _qvectSColorSet = {
                        {Qt::cyan,  //segment_notHighlighted
                         Qt::cyan,  //segment_highlighted
                         Qt::yellow},//point_notHighlighted

                        {QColor(0x3f, 0x67, 0xf2), //0x4a, 0x77, 0xfe), //sort of blue,    QColor(0x00,0xc3,0xc3)//darker cyan, //segment_notHighlighted
                         QColor(0x3f, 0x67, 0xf2), //0x4a, 0x77, 0xfe), //sort of blue,    QColor(0x00,0xc3,0xc3)//darker cyan, //segment_highlighted
                         QColor(0xff, 0x88, 0xfd)} //sort of pink, not too dark, not too light  //(0xfe, 0xc8, 0x11)} //sort of orange //QColor(0xc8, 0xc8, 0x00));//orange  //;/QColor(0xfb, 0xbc, 0x05)); orange} ////point_notHighlighted
    };

    _idxCurrentColorSet = 0;

    _sRouteShowAlone = {-1, false};

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    alloc_pointAlongRouteForPointSelection_current();

    setAddingBoxMode(_bAddingBoxMode);

    _currentScaleOfViewSideForCurrentZLI = 1.0;
}

void GraphicsItemsContainer::initMinimalElementSizeSide_fromBackgroundSizeAndPercentFactor(const QSizeF& qsfBackgroundSize, double percent) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (  (qsfBackgroundSize.width() < 1.0)
        ||(qsfBackgroundSize.height() < 1.0)) {
        qDebug() << __FUNCTION__ << " : rejection case #1";
        return;
    }
    if ((percent < 0.0025)||(percent>99.9)) {
        qDebug() << __FUNCTION__ << " : rejection case #2";
        return;
    }

    _qsizefBackgroundSize_noZLI = qsfBackgroundSize;
    double meanFromWH = (_qsizefBackgroundSize_noZLI.width() + _qsizefBackgroundSize_noZLI.height()) / 2.0;
    qDebug() << __FUNCTION__ << " : _qsizefBackgroundSize_noZLI w, h  = " << _qsizefBackgroundSize_noZLI.width() << ", " << _qsizefBackgroundSize_noZLI.height();
    qDebug() << __FUNCTION__ << " : meanFromWH = " << meanFromWH;

    _minimalElementSideSize_noZLI = meanFromWH * percent;

    if (_minimalElementSideSize_noZLI < _cstExpr_double_minimalElementSideSize_noZLI) {
        _minimalElementSideSize_noZLI = _cstExpr_double_minimalElementSideSize_noZLI;
    }

    qDebug() << __FUNCTION__ << " : final _minimalElementSideSize_noZLI = " << _minimalElementSideSize_noZLI;

}



GraphicsItemsContainer::~GraphicsItemsContainer() {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    remove_pointAlongRouteForPointSelection_current();

}

void GraphicsItemsContainer::connect_signalSlotToModel() {
    for(auto iter: _qmap_int_ptrGraphicsRouteItem) {
        connect_signalSlot_updateOrRemovePointOfRoute(iter);
     }
}


void GraphicsItemsContainer::connect_signalSlot_updateOrRemovePointOfRoute(GraphicsRouteItem* gRouteItem) {
    QObject::connect(gRouteItem, &GraphicsRouteItem::signal_updatePointOfRoute, _ptrRouteContainer, &RouteContainer::slot_updateModel_setPointOfRoute);

    //QObject::connect(iter, &GraphicsRouteItem::signal_removePointOfRoute, _ptrRouteContainer, &RouteContainer::slot_updateModel_removePointOfRoute);
    //QObject::connect(gRouteItem, &GraphicsRouteItem::signal_removePointOfRoute, this, &GraphicsItemsContainer::slot_removePointOfRoute);
    QObject::connect(gRouteItem, &GraphicsRouteItem::signal_pointHoverInOut_ownedByRoute, this, &GraphicsItemsContainer::slot_pointHoverInOut_ownedByRoute);

    //to restore initial location of a moved point who was rejected due to small distance to another point ( < 1. 0 )
    QObject::connect( _ptrRouteContainer, &RouteContainer::signal_setGraphicsLocationOfPointForRoute,
                      gRouteItem, &GraphicsRouteItem::slot_setGraphicsLocationOfPointForRoute);

}

//it's and update by replacing content from a fresh route equivalent
void GraphicsItemsContainer::resetGraphicsRouteItemFromRoute(int idRoute, bool bThisIsTheFirstPointForTheRoute) { //bThisIsTheFirstPointForTheRoute: extra check about find/remove error

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    //reset HoveredIdPoint:
    _bHoverInOrOutState = false;
    _currentHoveredIdPoint = -1;
    _currentHoveredIdPoint_IsOwnedByIdRoute = -1;

    //each change on a route implies a remove and insert of a new GraphicsRouteItem at the same idRoute
    bool bRouteRemoved = removeGraphicsRouteItem_noRouteIdReorder(idRoute);
    if (!bRouteRemoved) {
       if (!bThisIsTheFirstPointForTheRoute) {
           qDebug() << __FUNCTION__ << " error: can not remove GraphicsRouteItem in _qmap_int_ptrGraphicsRouteItem and it's not because it's the first point for the route ( _qmap_int_ptrGraphicsRouteItem: " << _qmap_int_ptrGraphicsRouteItem.size();
           return;
       }
    }

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)" << __LINE__;

    //replace content with fresh updated route content (ie 're-indexed' points and segments)
    //and add to scene
    GraphicsRouteItem* gRouteItem = new GraphicsRouteItem(nullptr);
    gRouteItem->initFromRoute(_ptrRouteContainer->getRouteRef(idRoute), _minimalElementSideSize_noZLI);


    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) after initFromRoute:";
    gRouteItem->debugShowStateAndLocation_aboutPoints();


    gRouteItem->setRouteHightlighted(true);
    connect_signalSlot_updateOrRemovePointOfRoute(gRouteItem);

    gRouteItem->adjustColors_segment_unselectedPoint(_qvectSColorSet[_idxCurrentColorSet]._qCol_segment_notHighlighted,
                                                     _qvectSColorSet[_idxCurrentColorSet]._qCol_point_notHighlighted);
    _qmap_int_ptrGraphicsRouteItem.insert(idRoute, gRouteItem);
    gRouteItem->adjustLocationAndSizeToZLI(_currentZLI);
    gRouteItem->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI);

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) after adjust*:";
    gRouteItem->debugShowStateAndLocation_aboutPoints();

    if (_ptrCustomGraphicsScene) {
        _ptrCustomGraphicsScene->addItem(gRouteItem);
    }
}


void GraphicsItemsContainer::adjustLocationAndSizeToZLI_AllRouteItem() { //bThisIsTheFirstPointForTheRoute: extra check about find/remove error
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _currentZLI = " << _currentZLI;
    for (auto &gRouteItem_iter : _qmap_int_ptrGraphicsRouteItem) {
        gRouteItem_iter->adjustLocationAndSizeToZLI(_currentZLI);
    }   
}

bool GraphicsItemsContainer::removeGraphicsRouteItem_noRouteIdReorder(int idRoute) {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

/*
    bool bRouteRemoved = _qmap_int_ptrGraphicsRouteItem.remove(idRoute);
    if (!bRouteRemoved) {
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): error failed to remove route from ptrGraphicsRouteItem (idRoute:" << idRoute;
        return;
    }
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer):  OK, removed route from ptrGraphicsRouteItem (idRoute:" << idRoute;
*/

    //remove from scene and qmap of item
    GraphicsRouteItem* defaultGRIPtr = nullptr;
    GraphicsRouteItem* gRouteItemPtr = _qmap_int_ptrGraphicsRouteItem.value(idRoute, defaultGRIPtr);

    if (gRouteItemPtr != nullptr) {

        gRouteItemPtr->hide();

        qDebug() << __FUNCTION__ << __LINE__;

        //@#LP decide the way of using Item->scene() or _ptrCustomGraphicsScene; and stay on this way
        QGraphicsScene *scenePtrOfItem = gRouteItemPtr->scene();
        qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
        if (scenePtrOfItem) {
            qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
            scenePtrOfItem->removeItem(gRouteItemPtr);
        } else {
            qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
        }
        /*if (_ptrCustomGraphicsScene) {
            qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->removeItem(gRouteItemPtr)";
            _ptrCustomGraphicsScene->removeItem(gRouteItemPtr);
        }*/
        delete gRouteItemPtr;
        gRouteItemPtr = nullptr;
        qDebug() << __FUNCTION__ << "gRouteItemPtr deleted";

    } else {
        qDebug() << __FUNCTION__ << " info: can not find GraphicsRouteItem in_qmap_int_ptrGraphicsRouteItem, it should be be because we have not yet added the first point";
    }

    bool bRouteRemoved = _qmap_int_ptrGraphicsRouteItem.remove(idRoute);
    return(bRouteRemoved);
}



bool GraphicsItemsContainer::removeAllRouteItem() {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    int qmap_int_ptrGraphicsRouteItem_size = _qmap_int_ptrGraphicsRouteItem.size();

    qDebug() << __FUNCTION__ << "_qmap_int_ptrGraphicsRouteItem_size = "<< qmap_int_ptrGraphicsRouteItem_size;

    for(int i = 0; i < qmap_int_ptrGraphicsRouteItem_size; i++) {
        removeGraphicsRouteItem_noRouteIdReorder(i);
    }

    _sRouteShowAlone = { -1, false};

    return(true);
}


bool GraphicsItemsContainer::removeAllBoxesAndRelatedItems() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    //QVector<CustomGraphicsTinyTargetPointItem *> _qvect_ptrGraphicsTinyTargetPointItem;
    //QVector<CustomGraphicsVectorDirectionAtPointItem *> _qvect_ptrGraphicsVectorDirectionAtPointItem;
    //QVector<CustomGraphicsRectangularHighlightItem *> _qvect_ptrCustomGraphicsRectangularHighlightItem;

    //int qvect_ptrCustomGraphicsBoxItem_size = _qvect_ptrCustomGraphicsBoxItem.size();
    //int qmap_insertionBoxId_ptrCustomGraphicsBoxItem_size = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.size();
    //qDebug() << __FUNCTION__ << "qmap_insertionBoxId_ptrCustomGraphicsBoxItem_size = "<< qmap_insertionBoxId_ptrCustomGraphicsBoxItem_size;

    for (auto iter  = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.begin();
              iter != _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.end(); ++iter) {

        qDebug() << __FUNCTION__ << "iter on _qmap_insertionBoxId_ptrCustomGraphicsBoxItem: iter.key()= " << iter.key();

        //for (int i = 0; i < qmap_insertionBoxId_ptrCustomGraphicsBoxItem_size; i++) {

        //CustomGraphicsBoxItem *customGraphicsBoxItemPtr = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(i, nullptr); //_qvect_ptrCustomGraphicsBoxItem[i];
        CustomGraphicsBoxItem2 *customGraphicsBoxItemPtr = iter.value(); //_qvect_ptrCustomGraphicsBoxItem[i];

        qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr @= " << (void*)customGraphicsBoxItemPtr;

        if (customGraphicsBoxItemPtr != nullptr) {

            qDebug() << __FUNCTION__ << __LINE__;

            customGraphicsBoxItemPtr->hide();

            //@#LP decide the way of using Item->scene() or _ptrCustomGraphicsScene; and stay on this way
            QGraphicsScene *scenePtrOfItem = customGraphicsBoxItemPtr->scene();
            qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
            if (scenePtrOfItem) {
                qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
                scenePtrOfItem->removeItem(customGraphicsBoxItemPtr);
            } else {
                qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
            }
            /*if (_ptrCustomGraphicsScene) {
                qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->removeItem(gRouteItemPtr)";
                _ptrCustomGraphicsScene->removeItem(customGraphicsBoxItemPtr);
            }*/
            delete customGraphicsBoxItemPtr;
            customGraphicsBoxItemPtr = nullptr;
            qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

        } else {
            qDebug() << __FUNCTION__ << " info: can not find GraphicsRouteItem in_qmap_int_ptrGraphicsRouteItem, it should be be because we have not yet added the first point";
        }
    }

    _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.clear();
    //_qvect_ptrCustomGraphicsBoxItem.clear();

    _mapBoxId_currentBoxHighlighted = -1;
    return(true);
}




bool GraphicsItemsContainer::removeRouteWithRouteIdReorder(int idRoute) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    bool bRouteRemoved = removeGraphicsRouteItem_noRouteIdReorder(idRoute);
    if (!bRouteRemoved) {
        qDebug() << __FUNCTION__ << " error: can not remove GraphicsRouteItem in _qmap_int_ptrGraphicsRouteItem" << _qmap_int_ptrGraphicsRouteItem.size();
        return(false);
    }

    //update routeIds to fill the gap

    //store the GraphicsRouteItem ptr from the _qmap_int_ptrGraphicsRouteItem to a vector
    //(qmap: value ordered by key)
    QVector<GraphicsRouteItem*> qvectGraphicsRouteItemPtr;
    for(auto iter: _qmap_int_ptrGraphicsRouteItem) {
        qvectGraphicsRouteItemPtr.push_back(iter);
    }
    _qmap_int_ptrGraphicsRouteItem.clear();

    //int _qmap_int_ptrGraphicsRouteItem_countAfterRemove = _qmap_int_ptrGraphicsRouteItem.size();
    int updading_routeId = 0;
    for(auto iter: qvectGraphicsRouteItemPtr) {        
        iter->resetId(updading_routeId);
        _qmap_int_ptrGraphicsRouteItem.insert(updading_routeId, iter);
        updading_routeId++;
    }
    return(true);
}

void GraphicsItemsContainer::removeAllDebugItems() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    //remove about init_devDebug_tinyTargetPoint_fromConstQVectQPointF
    {
        int qvect_ptrGraphicsTinyTargetPointItem_size = _qvect_ptrGraphicsTinyTargetPointItem.size();
        qDebug() << __FUNCTION__ << "qvect_ptrGraphicsTinyTargetPointItem_size = "<< qvect_ptrGraphicsTinyTargetPointItem_size;

        for(int i = 0; i < qvect_ptrGraphicsTinyTargetPointItem_size; i++) {
            CustomGraphicsTinyTargetPointItem *CustomGraphicsTinyTargetPointItemPtr = _qvect_ptrGraphicsTinyTargetPointItem[i];
            if (CustomGraphicsTinyTargetPointItemPtr != nullptr) {

                CustomGraphicsTinyTargetPointItemPtr->hide();

                qDebug() << __FUNCTION__ << __LINE__;
                //@#LP decide the way of using Item->scene() or _ptrCustomGraphicsScene; and stay on this way
                QGraphicsScene *scenePtrOfItem = CustomGraphicsTinyTargetPointItemPtr->scene();
                qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
                if (scenePtrOfItem) {
                    qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
                    scenePtrOfItem->removeItem(CustomGraphicsTinyTargetPointItemPtr);
                } else {
                    qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
                }
                /*if (_ptrCustomGraphicsScene) {
                    qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->removeItem(gRouteItemPtr)";
                    _ptrCustomGraphicsScene->removeItem(customGraphicsBoxItemPtr);
                }*/
                delete CustomGraphicsTinyTargetPointItemPtr;
                CustomGraphicsTinyTargetPointItemPtr = nullptr;
                qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

            } else {
                qDebug() << __FUNCTION__ << " info: can not find GraphicsRouteItem in_qmap_int_ptrGraphicsRouteItem, it should be be because we have not yet added the first point";
            }
        }
        _qvect_ptrGraphicsTinyTargetPointItem.clear();
    }

    //remove about init_devDebug_VectorDirectionAtPoint_fromConstQVect_point_and_unitVectorDirection
    {
        int qvect_ptrGraphicsVectorDirectionAtPointItem_size = _qvect_ptrGraphicsVectorDirectionAtPointItem.size();
        qDebug() << __FUNCTION__ << "_qvect_ptrGraphicsVectorDirectionAtPointItem_size = "<< qvect_ptrGraphicsVectorDirectionAtPointItem_size;

        for(int i = 0; i < qvect_ptrGraphicsVectorDirectionAtPointItem_size; i++) {
            CustomGraphicsVectorDirectionAtPointItem *CustomGraphicsVectorDirectionAtPointItemPtr = _qvect_ptrGraphicsVectorDirectionAtPointItem[i];
            if (CustomGraphicsVectorDirectionAtPointItemPtr != nullptr) {

                CustomGraphicsVectorDirectionAtPointItemPtr->hide();

                qDebug() << __FUNCTION__ << __LINE__;
                //@#LP decide the way of using Item->scene() or _ptrCustomGraphicsScene; and stay on this way
                QGraphicsScene *scenePtrOfItem = CustomGraphicsVectorDirectionAtPointItemPtr->scene();
                qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
                if (scenePtrOfItem) {
                    qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
                    scenePtrOfItem->removeItem(CustomGraphicsVectorDirectionAtPointItemPtr);
                } else {
                    qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
                }
                /*if (_ptrCustomGraphicsScene) {
                    qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->removeItem(gRouteItemPtr)";
                    _ptrCustomGraphicsScene->removeItem(customGraphicsBoxItemPtr);
                }*/
                delete CustomGraphicsVectorDirectionAtPointItemPtr;
                CustomGraphicsVectorDirectionAtPointItemPtr = nullptr;
                qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

            } else {
                qDebug() << __FUNCTION__ << " info: can not find GraphicsRouteItem in_qmap_int_ptrGraphicsRouteItem, it should be be because we have not yet added the first point";
            }
        }
        _qvect_ptrGraphicsVectorDirectionAtPointItem.clear();
    }

    //@LP added using 'init_devDebug_additional_temporaryDebugItem(...) can not be removed

    //remove about init_devDebug_CustomGraphicsRectangularHighlightItem_fromConstQVectPolygonF
    {
        int qvect_ptrCustomGraphicsRectangularHighlightItem_size = _qvect_ptrCustomGraphicsRectangularHighlightItem.size();
        qDebug() << __FUNCTION__ << "_qvect_ptrCustomGraphicsRectangularHighlightItem_size = "<< qvect_ptrCustomGraphicsRectangularHighlightItem_size;

        for(int i = 0; i < qvect_ptrCustomGraphicsRectangularHighlightItem_size; i++) {
            CustomGraphicsRectangularHighlightItem *CustomGraphicsRectangularHighlightItemPtr = _qvect_ptrCustomGraphicsRectangularHighlightItem[i];
            if (CustomGraphicsRectangularHighlightItemPtr != nullptr) {

                CustomGraphicsRectangularHighlightItemPtr->hide();

                qDebug() << __FUNCTION__ << __LINE__;
                //@#LP decide the way of using Item->scene() or _ptrCustomGraphicsScene; and stay on this way
                QGraphicsScene *scenePtrOfItem = CustomGraphicsRectangularHighlightItemPtr->scene();
                qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
                if (scenePtrOfItem) {
                    qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
                    scenePtrOfItem->removeItem(CustomGraphicsRectangularHighlightItemPtr);
                } else {
                    qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
                }
                /*if (_ptrCustomGraphicsScene) {
                    qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->removeItem(gRouteItemPtr)";
                    _ptrCustomGraphicsScene->removeItem(customGraphicsBoxItemPtr);
                }*/
                delete CustomGraphicsRectangularHighlightItemPtr;
                CustomGraphicsRectangularHighlightItemPtr = nullptr;
                qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

            } else {
                qDebug() << __FUNCTION__ << " info: can not find GraphicsRouteItem in_qmap_int_ptrGraphicsRouteItem, it should be be because we have not yet added the first point";
            }
        }
        _qvect_ptrCustomGraphicsRectangularHighlightItem.clear();
    }
}


void GraphicsItemsContainer::setPtrCustomGraphicsScene(CustomGraphicsScene *ptrCustomGraphicsScene) {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene already set";
        return;
    }
    _ptrCustomGraphicsScene = ptrCustomGraphicsScene;
}

//don't add debug item
void GraphicsItemsContainer::addAllRouteItem_and_TinyTargetPtNotDragable_toScene() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene is nullptr";
        return;
    }
    foreach (GraphicsRouteItem* gRouteItem, _qmap_int_ptrGraphicsRouteItem) {
        _ptrCustomGraphicsScene->addItem(gRouteItem);
    }

    for (int ieAPt = e_APACFBI_first; ieAPt <= e_APACFBI_current; ieAPt++) {
        qDebug() << __FUNCTION__ << "GraphicsItemsContainer::addAllItemToScene(): _ptrCGTinyTargetPtNotDragable_First_Last_Current[" << ieAPt << "] @="
                 << (void*)_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt];

        if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]) {
            _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]->setSize(_minimalElementSideSize_noZLI);
            _ptrCustomGraphicsScene->addItem(_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]);
        } else {
            qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCGTinyTargetPtNotDragable_First_Last_Current[" << ieAPt << "] is nullptr: can not add to _ptrCustomGraphicsScene";
        }
    }
}

//before setting ptr stored here, be sure to delete the ptr stored here before;
//delete them by the scene or using scene->remove + delete the ptr after
void GraphicsItemsContainer::initFromAndConnectTo_RouteContainer(RouteContainer* rcPtr) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    _qmap_int_ptrGraphicsRouteItem.clear();

    if (!rcPtr) {
        qDebug() << __FUNCTION__ << ": warning: routeContainer is nullptr";
        return;
    }

    _ptrRouteContainer = rcPtr;
    for (auto iter: _ptrRouteContainer->qvectOfRouteId()) {

        qDebug() << __FUNCTION__ << "iter =" << iter;

        GraphicsRouteItem* gRouteItem = new GraphicsRouteItem(nullptr);
        gRouteItem->initFromRoute(_ptrRouteContainer->getRouteRef(iter), _minimalElementSideSize_noZLI);
        gRouteItem->setRouteHightlighted(false);

        gRouteItem->adjustColors_segment_unselectedPoint(_qvectSColorSet[_idxCurrentColorSet]._qCol_segment_notHighlighted,
                                                         _qvectSColorSet[_idxCurrentColorSet]._qCol_point_notHighlighted);

        _qmap_int_ptrGraphicsRouteItem.insert(iter, gRouteItem);
    }

    connect_signalSlotToModel();
}

void GraphicsItemsContainer::setDevicePixelRatioF(qreal devicePixelRatioF) {
    if (devicePixelRatioF < 1.0) { //avoid any futur division by zero
        _devicePixelRatioF = 1.0;
    } else {
        _devicePixelRatioF = devicePixelRatioF;
    }
}

void GraphicsItemsContainer::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) received ZLI = " << ZLI;

    _currentZLI = ZLI;
    foreach (GraphicsRouteItem* gRouteItem, _qmap_int_ptrGraphicsRouteItem) {
        gRouteItem->adjustLocationAndSizeToZLI(_currentZLI);
    }

    foreach (CustomGraphicsBoxItem2* ptrCGBoxItem, _qmap_insertionBoxId_ptrCustomGraphicsBoxItem) {
        ptrCGBoxItem->adjustLocationAndSizeToZLI(_currentZLI);
    }

    //debug items:
    foreach (CustomGraphicsTinyTargetPointItem* ptrCGTinyTargetPointItem, _qvect_ptrGraphicsTinyTargetPointItem) {
        ptrCGTinyTargetPointItem->adjustLocationAndSizeToZLI(_currentZLI);
    }
    foreach (CustomGraphicsVectorDirectionAtPointItem* ptrCGVectorDirectionAtPointItem, _qvect_ptrGraphicsVectorDirectionAtPointItem) {
        ptrCGVectorDirectionAtPointItem->adjustLocationAndSizeToZLI(_currentZLI);
    }

    if (_ptrGRouteItemWForbiddenSectionsFSAE) {
        _ptrGRouteItemWForbiddenSectionsFSAE->adjustLocationAndSizeToZLI(_currentZLI);
    }

    for (int ieAPt = e_APACFBI_first; ieAPt <= e_APACFBI_current; ieAPt++) {
        if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]) {
            qDebug() << __FUNCTION__ << "if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[" << ieAPt << "] not nullptr => adjustLocationAndSizeToZLI with _currentZLI = " << _currentZLI;
            _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]->adjustLocationAndSizeToZLI(_currentZLI);
        } else {
            qDebug() << __FUNCTION__ << "can not call adjustLocationAndSizeToZLI to_ptrCGTinyTargetPtNotDragable_First_Last_Current[" << ieAPt << "] because nullptr";
        }
    }

    adjustLocationAndSizeToCurrentZLI_aboutSpecificGraphicsItemAboutRouteStartAndEnd();

}


void GraphicsItemsContainer::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {
    _currentScaleOfViewSideForCurrentZLI = scaleOfViewSideForCurrentZLI;
    foreach (CustomGraphicsBoxItem2* ptrCGBoxItem, _qmap_insertionBoxId_ptrCustomGraphicsBoxItem) {
        ptrCGBoxItem->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    }

    foreach (GraphicsRouteItem* ptrCGRouteItem, _qmap_int_ptrGraphicsRouteItem) {
        ptrCGRouteItem->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    }

    if (_ptrGRouteItemWForbiddenSectionsFSAE) {
        _ptrGRouteItemWForbiddenSectionsFSAE->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
    }

    for (int ieAPt = e_APACFBI_first; ieAPt <= e_APACFBI_current; ieAPt++) {
        if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]) {
            _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]->adjustGraphicsItemWidth(scaleOfViewSideForCurrentZLI);
        }/* else {
            qDebug() << __FUNCTION__ << "can not call adjustLocationAndSizeToZLI to_ptrCGTinyTargetPtNotDragable_First_Last_Current[" << ieAPt << "] because nullptr";
        }*/
    }
    adjustGraphicsItemWidth_aboutSpecificGraphicsItemAboutRouteStartAndEnd();
}

void GraphicsItemsContainer::init_devDebug_tinyTargetPoint_fromConstQVectQPointF(int devDebugLayerID, const QVector<QPointF>& qvectqpf, qreal sideSizeNoZLI) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene is nullptr";
        return;
    }

    Qt::GlobalColor colorUnselected = Qt::green;
    Qt::GlobalColor colorSelected = Qt::red;

    switch (devDebugLayerID) {
        default:
            break;
        //this list probably totally unsync with Qt::color enum; this is an evolving dev-debug-tool method with none clean implementation in mind
        case 1: colorUnselected = Qt::green;   break;
        case 2: colorUnselected = Qt::magenta; break;
        case 3: colorUnselected = Qt::cyan;    break;
        case 4: colorUnselected = Qt::red;     break;
        case 5: colorUnselected = Qt::blue;    break;
        case 6: colorUnselected = Qt::yellow;  break;
        case 7: colorUnselected = Qt::white;   break;
        case 8: colorUnselected = Qt::darkRed; break;
        case 9: colorUnselected = Qt::darkCyan; break;

    }
    //no clear() call on ptr vect container

    int idxPoint = 0;
    int idPoint = 10000*devDebugLayerID; //idPoint should not be used or useful for dev-debug purpose but let's try to avoid id point value collision
    for(auto iter: qvectqpf) {
        CustomGraphicsTinyTargetPointItem *devdebug_tinyTargetPointItem =
                new CustomGraphicsTinyTargetPointItem(idxPoint, iter, sideSizeNoZLI/*0.75*//*0.125*/, colorUnselected, colorSelected);
        //qDebug() << __FUNCTION__ << "Adding devdebug_tinyTargetPointItem #" << idxPoint << " idPoint=" << idPoint;
        _ptrCustomGraphicsScene->addItem(devdebug_tinyTargetPointItem);
        _qvect_ptrGraphicsTinyTargetPointItem.push_back(devdebug_tinyTargetPointItem);
        idxPoint++;
        idPoint++;

    }
}

void GraphicsItemsContainer::setRoute_hightlighted(int idRoute) {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    QMap<int, GraphicsRouteItem*>::iterator qmap_int_GRouteItem_iterator = _qmap_int_ptrGraphicsRouteItem.begin();
    for(;qmap_int_GRouteItem_iterator !=  _qmap_int_ptrGraphicsRouteItem.end(); ++qmap_int_GRouteItem_iterator) {
        int intIter_idRoute = qmap_int_GRouteItem_iterator.key();
        qmap_int_GRouteItem_iterator.value()->setRouteHightlighted(intIter_idRoute == idRoute);
    }
}

void GraphicsItemsContainer::setRoute_notHightlighted(int idRoute) { //only set to notHightlighted the idRoute
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    GraphicsRouteItem* defaultGRIPtr = nullptr;
    GraphicsRouteItem* gRouteItemPtr = _qmap_int_ptrGraphicsRouteItem.value(idRoute, defaultGRIPtr);
    if (gRouteItemPtr != nullptr) {
        gRouteItemPtr->setRouteHightlighted(false);
    }
}

void GraphicsItemsContainer::init_devDebug_VectorDirectionAtPoint_fromConstQVect_point_and_unitVectorDirection(
        int devDebugLayerID,
        const QVector<S_qpf_point_and_unitVectorDirection> &_qvect_unitVectorForEachPointAlongRoute) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene is nullptr";
        return;
    }

    Qt::GlobalColor colorUnselected = Qt::yellow;
    Qt::GlobalColor colorSelected = Qt::red;

    //no clear() call on ptr vect container

    int idxPoint = 0;
    int idPoint = 100000*devDebugLayerID; //idPoint should not be used or useful for dev-debug purpose but let's try to avoid id point value collision
    for(const auto &iter: _qvect_unitVectorForEachPointAlongRoute) {
    CustomGraphicsVectorDirectionAtPointItem *devdebug_vectorDirectionAtPoint = new CustomGraphicsVectorDirectionAtPointItem(
                idxPoint, iter._point, iter._unitVectorDirection, colorUnselected, colorSelected, nullptr);
        qDebug() << __FUNCTION__ << "Adding CustomGraphicsVectorDirectionAtPointItem #" << idxPoint << " idPoint=" << idPoint;
        _ptrCustomGraphicsScene->addItem(devdebug_vectorDirectionAtPoint);
        _qvect_ptrGraphicsVectorDirectionAtPointItem.push_back(devdebug_vectorDirectionAtPoint);
        idxPoint++;
        idPoint++;
    }
}

void GraphicsItemsContainer::init_devDebug_CustomGraphicsRectangularHighlightItem_fromConstQVectPolygonF(const QVector<QPolygonF>& qvectPolygons) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCustomGraphicsScene) {
        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene is nullptr";
        return;
    }

     for(const auto &iter: qvectPolygons) {
         CustomGraphicsRectangularHighlightItem *devdebug_CGRectangularHighlightItem = new CustomGraphicsRectangularHighlightItem(iter);
         _ptrCustomGraphicsScene->addItem(devdebug_CGRectangularHighlightItem);
         _qvect_ptrCustomGraphicsRectangularHighlightItem.push_back(devdebug_CGRectangularHighlightItem);
    }
}

void GraphicsItemsContainer::init_devDebug_additional_temporaryDebugItem() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    int itemId = 5000;
    for (qreal yi=.5; yi < 5000.0; yi+=1.0) {
        CustomGraphicsTinyTargetPointItem *checkPositionning1 = new CustomGraphicsTinyTargetPointItem(itemId++, {yi,yi},1.0, nullptr);
        _ptrCustomGraphicsScene->addItem(checkPositionning1);
    }
}

void GraphicsItemsContainer::slot_pointHoverInOut_ownedByRoute(bool bHoverInOrOutstate, int idPoint, int idRoute) {
    _bHoverInOrOutState = bHoverInOrOutstate;
    if (_bHoverInOrOutState) {
        _currentHoveredIdPoint = idPoint;
        _currentHoveredIdPoint_IsOwnedByIdRoute = idRoute;
    } else {
        _currentHoveredIdPoint = -1;
        _currentHoveredIdPoint_IsOwnedByIdRoute = -1;
    }
}

bool GraphicsItemsContainer::getHoveredPointToSelectedRoute(int& idPoint, int& _ownedByIdRoute) {
    idPoint = _currentHoveredIdPoint;
    _ownedByIdRoute = _currentHoveredIdPoint_IsOwnedByIdRoute;
    return (_bHoverInOrOutState);
}

bool GraphicsItemsContainer::addBoxes_from_mapBoxAndStackedProfilWithMeasurements_fromKeyToKey(
        const QMap<int, S_BoxAndStackedProfilWithMeasurements>& qmapProfilsBoxParameters,
        int firstKey, int lastKey,
        bool& bCriticalErrorOccured) {

    bCriticalErrorOccured = false;

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    qDebug() << __FUNCTION__ << "firstKey:" << firstKey;
    qDebug() << __FUNCTION__ << "lastKey :" << lastKey;

    if (qmapProfilsBoxParameters.isEmpty()) {
        return(false);
    }

    if (lastKey < firstKey) {
        return(false);
    }

    int countToAdd = lastKey - firstKey + 1;
    qDebug() << __FUNCTION__ << "countToAdd :" << countToAdd;

    auto iterFirst = qmapProfilsBoxParameters.find(firstKey);
    auto iterLast = qmapProfilsBoxParameters.find(lastKey);

    if (  (iterFirst == qmapProfilsBoxParameters.end())
        ||(iterLast == qmapProfilsBoxParameters.end())) {
        return(false);
    }

    //int countAdded = 0;

    for (int iKey = firstKey; iKey <= lastKey; iKey++) {

        qDebug() << __FUNCTION__ << " _looping_ with iKey = " << iKey;

        auto iter = qmapProfilsBoxParameters.find(iKey);
        if (iter == qmapProfilsBoxParameters.end()) {
            //@#LP no cleanup ! critical error
            bCriticalErrorOccured = true;
            return(false);
        }

        //qDebug() << __FUNCTION__ << "adding with content:";
        iter.value().showContent_noMeasurements();

        CustomGraphicsBoxItem2 *CGBoxItem = new CustomGraphicsBoxItem2(iter.value()._profilsBoxParameters, iKey, _devicePixelRatioF);

        CGBoxItem->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI);

        _ptrCustomGraphicsScene->addItem(CGBoxItem);

        _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.insert(iKey, CGBoxItem);

        CGBoxItem->adjustLocationAndSizeToZLI(_currentZLI);

    }
    return(true);
}

void GraphicsItemsContainer::addOneBox_from_profilBoxParameters(int mapBoxId, const S_ProfilsBoxParameters& profilsBoxParameters) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";
    qDebug() << __FUNCTION__ << ": profilsBoxParameters = ";
    profilsBoxParameters.showContent();

    CustomGraphicsBoxItem2 *CGBoxItem = new CustomGraphicsBoxItem2(profilsBoxParameters, mapBoxId, _devicePixelRatioF);

    CGBoxItem->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI);

    _ptrCustomGraphicsScene->addItem(CGBoxItem);

    int startIdxForInserionBoxId = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.size(); //@LP: _qmap_insertionBoxId_ptrCustomGraphicsBoxItem has keys synced with the qmap in the logic side
    _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.insert(startIdxForInserionBoxId, CGBoxItem);

    CGBoxItem->adjustLocationAndSizeToZLI(_currentZLI);
}

void GraphicsItemsContainer::updateSelectedBox_givenMapBoxId(int mapBoxId) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (mapBoxId == _mapBoxId_currentBoxHighlighted) {
        qDebug() << __FUNCTION__ << " if (mapBoxId == _mapBoxId_currentBoxHighlighted) {";
        return;
    }

    //highlight off the previous if set:
    if (_mapBoxId_currentBoxHighlighted != -1) {
        CustomGraphicsBoxItem2 *customGraphicsBoxItemPtr = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(_mapBoxId_currentBoxHighlighted, nullptr);//_qvect_ptrCustomGraphicsBoxItem[_vectBoxId_currentBoxHighlighted];
        if (customGraphicsBoxItemPtr != nullptr) {            
            qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): (setState_highlighted false) @customGraphicsBoxItemPtr" << (void*)customGraphicsBoxItemPtr;
            customGraphicsBoxItemPtr->setState_highlighted(false);
            customGraphicsBoxItemPtr->setZValue(.0);
        }
    }

    auto cgBoxItemFound = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(mapBoxId, nullptr);
    if (!cgBoxItemFound) {
        _mapBoxId_currentBoxHighlighted = -1;
        return;
    }
    //highlight the new:
    _mapBoxId_currentBoxHighlighted = mapBoxId;
    CustomGraphicsBoxItem2 *customGraphicsBoxItemPtr = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(_mapBoxId_currentBoxHighlighted, nullptr); //_qvect_ptrCustomGraphicsBoxItem[_vectBoxId_currentBoxHighlighted];
    if (customGraphicsBoxItemPtr != nullptr) {
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): (setState_highlighted true) @customGraphicsBoxItemPtr" << (void*)customGraphicsBoxItemPtr;
        customGraphicsBoxItemPtr->setState_highlighted(true);
        customGraphicsBoxItemPtr->setZValue(1.0);
    }
}

void GraphicsItemsContainer::updateOneBox_givenMapBoxId_and_profilBoxParameters(int mapBoxId, const S_ProfilsBoxParameters& profilsBoxParameters) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): received mapBoxId = " << mapBoxId;
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): content as replacement: ";
    profilsBoxParameters.showContent();

    auto cgBoxItemFound = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(mapBoxId, nullptr);
    if (!cgBoxItemFound) {
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): cgBoxItemFound not found for mapBoxId = " << mapBoxId;
        return;
    }

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): @cgBoxItemFound" << (void*)cgBoxItemFound;

    bool bHighlighted = false;

    bHighlighted = cgBoxItemFound->getState_highlighted();

    QGraphicsScene *scenePtrOfItem = cgBoxItemFound->scene();
    qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
    if (scenePtrOfItem) {
        qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
        scenePtrOfItem->removeItem(cgBoxItemFound);
    } else {
        qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
    }

    delete cgBoxItemFound;
    cgBoxItemFound = nullptr;
    qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

    CustomGraphicsBoxItem2 *CGBoxItem = new CustomGraphicsBoxItem2(profilsBoxParameters, mapBoxId, _devicePixelRatioF);

    CGBoxItem->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI); //was missing ?

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): @CGBoxItem = new" << (void*)CGBoxItem;

    CGBoxItem->setState_highlighted(bHighlighted);
    _ptrCustomGraphicsScene->addItem(CGBoxItem);
    _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.insert(mapBoxId, CGBoxItem);
    CGBoxItem->adjustLocationAndSizeToZLI(_currentZLI);

}


void GraphicsItemsContainer::removeSelectionOfBoxes(/*int aboutQMap_boxIdOfFirstToRemove,
                                                  int aboutQMap_boxIdOflastToRemove*/const QVector<int> qvect_of_mapBoxIdToRemove) {

    int countRemoved = 0;

    for (auto mapBoxId : qvect_of_mapBoxIdToRemove) {

        CustomGraphicsBoxItem2 *customGraphicsBoxItemPtr = _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.value(mapBoxId, nullptr);
        if (customGraphicsBoxItemPtr != nullptr) {
            qDebug() << __FUNCTION__ << "(GraphicsItemsContainer): (setState_highlighted true) @customGraphicsBoxItemPtr" << (void*)customGraphicsBoxItemPtr;

            QGraphicsScene *scenePtrOfItem = customGraphicsBoxItemPtr->scene();
            qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
            if (scenePtrOfItem) {
                qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(gRouteItemPtr)";
                scenePtrOfItem->removeItem(customGraphicsBoxItemPtr);
            } else {
                qDebug() << __FUNCTION__ << "don't remove gRouteItemPtr from scenePtrOfItem";
            }

            delete customGraphicsBoxItemPtr;
            customGraphicsBoxItemPtr = nullptr;
            qDebug() << __FUNCTION__ << "customGraphicsBoxItemPtr deleted";

            countRemoved += _qmap_insertionBoxId_ptrCustomGraphicsBoxItem.remove(mapBoxId);
        }
    }

    qDebug() << __FUNCTION__ << "count to remove = " << qvect_of_mapBoxIdToRemove;
    qDebug() << __FUNCTION__ << "count removed   = " << countRemoved;
}

void GraphicsItemsContainer::alloc_pointAlongRouteForPointSelection_current() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]) {
        qDebug() << __FUNCTION__ << "error: e_APACFBI_current already allocated";
        return;
    }

    _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current] =
        new CustomGraphicsTinyTargetPointItem_notDragrable2(e_APACFBI_current, {.0,.0}, /*150.0*/_minimalElementSideSize_noZLI, Qt::yellow, Qt::yellow, false);
}


void GraphicsItemsContainer::alloc_CGSpecificItemsOfInterest() {
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";
    if (_ptrCGSpecificItemsOfInterest) {
       qDebug() << __FUNCTION__ << "error: _ptrCGSpecificItemsOfInterest already allocated";
       return;
    }
    _ptrCGSpecificItemsOfInterest = new CustomGraphicsSpecificItemsOfInterest2;
}

void GraphicsItemsContainer::addToScene_hidden_CGSpecificItemsOfInterest() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCGSpecificItemsOfInterest) {
        return;
    }
    _ptrCGSpecificItemsOfInterest->setVisible(false);
    _ptrCustomGraphicsScene->addItem(_ptrCGSpecificItemsOfInterest);
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCustomGraphicsScene->addItem(_ptrCGSpecificItemsOfInterest);";
}

//@#LP refactor, merging multiple same code about item remove
void GraphicsItemsContainer::removeItemFromSceneAndDelete_CGSpecificItemsOfInterest() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

     if (!_ptrCGSpecificItemsOfInterest) {
        qDebug() << __FUNCTION__ << "error: _ptrCGSpecificItemsOfInterest is nullptr";
        return;
     }
     _ptrCGSpecificItemsOfInterest->hide();

     QGraphicsScene *scenePtrOfItem = _ptrCGSpecificItemsOfInterest->scene();
     qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
     if (scenePtrOfItem) {
         qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(_ptrCGSpecificItemsOfInterest)";
         scenePtrOfItem->removeItem(_ptrCGSpecificItemsOfInterest);
     } else {
         qDebug() << __FUNCTION__ << "don't remove _ptrCGSpecificItemsOfInterest from scenePtrOfItem";
     }
     delete _ptrCGSpecificItemsOfInterest;
     _ptrCGSpecificItemsOfInterest = nullptr;
     qDebug() << __FUNCTION__ << "_ptrCGSpecificItemsOfInterest deleted";
}

void GraphicsItemsContainer::remove_pointAlongRouteForPointSelection_current() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    for (int ieAPt = e_APACFBI_first; ieAPt <= e_APACFBI_current; ieAPt++) {

        if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt] != nullptr) {

            _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]->hide();

            QGraphicsScene *scenePtrOfItem = _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]->scene();
            qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
            if (scenePtrOfItem) {
                qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(_ptrCGTinyTargetPtNotDragable_First_Last_Current[...])";
                scenePtrOfItem->removeItem(_ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt]);
            } else {
                qDebug() << __FUNCTION__ << "don't remove _ptrCGTinyTargetPtNotDragable_First_Last_Current[...] from scenePtrOfItem";
            }
            delete _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt];
            _ptrCGTinyTargetPtNotDragable_First_Last_Current[ieAPt] = nullptr;
            qDebug() << __FUNCTION__ << "_ptrCGTinyTargetPtNotDragable_First_Last_Current[...] deleted";
        }
    }
}

void GraphicsItemsContainer::remove_gRouteItemWForbiddenSectionsFSAE() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrGRouteItemWForbiddenSectionsFSAE) {        
        qDebug() << __FUNCTION__ << "if (!_ptrGRouteItemWForbiddenSectionsFSAE) {";
        return;
    }

    _ptrGRouteItemWForbiddenSectionsFSAE->hide();//@LP hiding before remove seems to solve the crash about incorrect bbox about this item.

    QGraphicsScene *scenePtrOfItem = _ptrGRouteItemWForbiddenSectionsFSAE->scene();
    qDebug() << __FUNCTION__ << "dbg: " << (void*)_ptrCustomGraphicsScene << " cmp to " << (void*)scenePtrOfItem;
    if (scenePtrOfItem) {
        qDebug() << __FUNCTION__ << "scenePtrOfItem->removeItem(_gRouteItemWForbiddenSectionsFSAE)";
        scenePtrOfItem->removeItem(_ptrGRouteItemWForbiddenSectionsFSAE);
    } else {
        qDebug() << __FUNCTION__ << "don't remove _gRouteItemWForbiddenSectionsFSAE from scenePtrOfItem";
    }
    delete _ptrGRouteItemWForbiddenSectionsFSAE;
    _ptrGRouteItemWForbiddenSectionsFSAE = nullptr;
    qDebug() << __FUNCTION__ << "_gRouteItemWForbiddenSectionsFSAE deleted";

}

bool GraphicsItemsContainer::setAndShowForbiddenSectionAlongRouteForPointSelection(const S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRSFromStartAndEnd) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    hide_targetCursorLocationOfPossibleCenterForAddBox();

    if (_ptrGRouteItemWForbiddenSectionsFSAE) {
        _ptrGRouteItemWForbiddenSectionsFSAE->hide();
    }

    _sForbiddenSectionsFromStartAndEnd = sForbiddenRSFromStartAndEnd;

    qDebug() << __FUNCTION__ << " sForbiddenRSFromStartAndEnd._bFullRouteIsForbidden = "       << sForbiddenRSFromStartAndEnd._bFullRouteIsForbidden;
    qDebug() << __FUNCTION__ << "_sForbiddenSectionsFromStartAndEnd._bFullRouteIsForbidden = " << _sForbiddenSectionsFromStartAndEnd._bFullRouteIsForbidden;

    qDebug() << __FUNCTION__ << "..._sFrontierFromStart.qpf = "              << sForbiddenRSFromStartAndEnd._sFrontierFromStart._qpf;
    qDebug() << __FUNCTION__ << "..._sFrontierFromStart._idxSegmentOwner = " << sForbiddenRSFromStartAndEnd._sFrontierFromStart._idxSegmentOwner;
    qDebug() << __FUNCTION__ << "..._sFrontierFromStart._bPointFeed = "      << sForbiddenRSFromStartAndEnd._sFrontierFromStart._bPointFeed;

    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd.qpf = "              << sForbiddenRSFromStartAndEnd._sFrontierFromEnd._qpf;
    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd._idxSegmentOwner = " << sForbiddenRSFromStartAndEnd._sFrontierFromEnd._idxSegmentOwner;
    qDebug() << __FUNCTION__ << "..._sFrontierFromEnd._bPointFeed = "      << sForbiddenRSFromStartAndEnd._sFrontierFromEnd._bPointFeed;

    if (!_ptrRouteContainer) {
        return(false);
    }

    Route& routeRef = _ptrRouteContainer->getRouteRef(0);
    if (routeRef.getId() == -1) {
        return(false);
    }

    remove_gRouteItemWForbiddenSectionsFSAE();
    qDebug() << __FUNCTION__ << "_ptrGRouteItemWForbiddenSectionsFSAE = new GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd();";
    _ptrGRouteItemWForbiddenSectionsFSAE = new GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd();
    _ptrGRouteItemWForbiddenSectionsFSAE->initFromRouteAndFirstAndLastPointFromStartAndEnd(routeRef, _sForbiddenSectionsFromStartAndEnd/*, _minimalElementSideSize_noZLI*/);
    _ptrGRouteItemWForbiddenSectionsFSAE->adjustLocationAndSizeToZLI(_currentZLI);
    _ptrGRouteItemWForbiddenSectionsFSAE->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI);
    if (_ptrCustomGraphicsScene) {
        _ptrCustomGraphicsScene->addItem(_ptrGRouteItemWForbiddenSectionsFSAE);
    }
    _ptrGRouteItemWForbiddenSectionsFSAE->show();

    qDebug() << __FUNCTION__ << "_ptrGRouteItemWForbiddenSectionsFSAE->show();";

    return(true);
}

void GraphicsItemsContainer::setAndShow_targetCursorLocationOfPossibleCenterForAddBox(QPointF qpfPoint) {
    if (!_ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]) {
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current] is nullptr";
        return;
    }
    _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]->setLocation(qpfPoint);
    _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]->setVisible(true);
}

void GraphicsItemsContainer::hide_targetCursorLocationOfPossibleCenterForAddBox() {
    if (!_ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]) {
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current] is nullptr";
        return;
    }
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) if (_ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current])";
    _ptrCGTinyTargetPtNotDragable_First_Last_Current[e_APACFBI_current]->setVisible(false);
}

void GraphicsItemsContainer::setAddingBoxMode(bool bState) {
    _bAddingBoxMode = bState;
    if (_bAddingBoxMode) {
        setCGBoxItemSelectable(false);
    } else {
        hide_targetCursorLocationOfPossibleCenterForAddBox();

        setCGBoxItemSelectable(true);
    }
}

void GraphicsItemsContainer::setCGBoxItemSelectable(bool bState) {
    for (auto& iter: _qmap_insertionBoxId_ptrCustomGraphicsBoxItem) {
        iter->setSelectable(bState);
        iter->setState_highlighted(false);
    }
    _mapBoxId_currentBoxHighlighted = -1;
}

void GraphicsItemsContainer::adjustLocationAndSizeToCurrentZLI_aboutSpecificGraphicsItemAboutRouteStartAndEnd() {
  if (!_ptrCGSpecificItemsOfInterest) {
      return;
  }
  _ptrCGSpecificItemsOfInterest->adjustLocationAndSizeToZLI(_currentZLI);
}

void GraphicsItemsContainer::adjustGraphicsItemWidth_aboutSpecificGraphicsItemAboutRouteStartAndEnd() {
  if (!_ptrCGSpecificItemsOfInterest) {
      return;
  }
  _ptrCGSpecificItemsOfInterest->adjustGraphicsItemWidth(_currentScaleOfViewSideForCurrentZLI);
}


void GraphicsItemsContainer::allocInitFromRouteAndAddHiddenToScene_locationOfSpecificGraphicsItemAboutRouteStartAndEnd() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!bCheckRouteContainerPermitsUseOfSpecificGraphicsItemAboutRouteStartAndEnd()) {
        qDebug() << __FUNCTION__ << __LINE__ << "return now";
        return;
    }

    if (_ptrCGSpecificItemsOfInterest) {
        qDebug() << __FUNCTION__ << __LINE__ << "_ptrCGSpecificItemsOfInterest already allocated!";
        return;
    }

    alloc_CGSpecificItemsOfInterest();
    _ptrCGSpecificItemsOfInterest->initFromRoute(_ptrRouteContainer->getRouteRef(0));
    addToScene_hidden_CGSpecificItemsOfInterest();

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCGSpecificItemsOfInterest->initFromRoute called";
}

void GraphicsItemsContainer::setVisible_startAndEndOfRoute(bool bState) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrCGSpecificItemsOfInterest) {        
        qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)  if (!_ptrCGSpecificItemsOfInterest) {";
        return;
    }
    _ptrCGSpecificItemsOfInterest->setVisible(bState);
    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _ptrCGSpecificItemsOfInterest->setVisible( " << bState << " ) called";
}

bool GraphicsItemsContainer::bCheckRouteContainerPermitsUseOfSpecificGraphicsItemAboutRouteStartAndEnd() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";

    if (!_ptrRouteContainer) {
        qDebug() << __FUNCTION__ << "if (!_ptrRouteContainer) {";
        return(false);
    }

    if (_ptrRouteContainer->routeCount() != 1) {
        qDebug() << __FUNCTION__ << "if (_ptrRouteContainer->routeCount() != 1) {";
        return(false);
    }

    Route& refUsedRoute = _ptrRouteContainer->getRouteRef(0);

    if (refUsedRoute.getId() == -1) {
        qDebug() << __FUNCTION__ << "if (refUsedRoute.getId() == -1) {";
        return(false);
    }

    if (!refUsedRoute.segmentCount()) {
       qDebug() << __FUNCTION__ << "if (!refUsedRoute.segmentCount()) {";
       return(false);
    }

    qDebug() << __FUNCTION__ << ": ok!";
    return(true);
}

bool GraphicsItemsContainer::oneBoxIsCurrentlyHoveredAndNotSelected(int &mapBoxId_ofHoveredAndNotSelected) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) entering with mapBoxId_ofHoveredAndNotSelected = " << mapBoxId_ofHoveredAndNotSelected;

    //@##LP: looping O(n)
    //improve like that: the current hovered mapBoxId can be stored in GraphicsItemsContainer
    //and set to mapboxid when entering and det to -1 when leaving (informed by signal coming from mouse event in boxItem)
    mapBoxId_ofHoveredAndNotSelected = -1;
    foreach (CustomGraphicsBoxItem2* ptrCGBoxItem, _qmap_insertionBoxId_ptrCustomGraphicsBoxItem) {
        if (ptrCGBoxItem->getState_hover()) {
            mapBoxId_ofHoveredAndNotSelected = ptrCGBoxItem->getBoxID();
            break;
        }
    }
    if (mapBoxId_ofHoveredAndNotSelected != -1) {
        if (mapBoxId_ofHoveredAndNotSelected != _mapBoxId_currentBoxHighlighted) {
            return(true);
        }
    }
    return(false);
}

void GraphicsItemsContainer::rotateColorSetsForItems() {

    _idxCurrentColorSet++;
    if (_idxCurrentColorSet >= _qvectSColorSet.size()) {
        _idxCurrentColorSet = 0;
    }
    for (auto &gRouteItem_iter : _qmap_int_ptrGraphicsRouteItem) {
             gRouteItem_iter->adjustColors_segment_unselectedPoint(_qvectSColorSet[_idxCurrentColorSet]._qCol_segment_notHighlighted,
                                                                   _qvectSColorSet[_idxCurrentColorSet]._qCol_point_notHighlighted);
     }
}

void GraphicsItemsContainer::setGraphicsSettings(const S_GraphicsParameters_ItemOnImageView& sGP_itemOnImageView) {
    _sGP_itemOnImageView = sGP_itemOnImageView;
    applyGraphicsSettings();
}

void GraphicsItemsContainer::applyGraphicsSettings() {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer)";
}

void GraphicsItemsContainer::routeSet_route_showAlone(int routeId) {
    routeSet_route_showAlone(routeId, _sRouteShowAlone._bShowAlone);
}

void GraphicsItemsContainer::routeSet_route_showAlone(int routeId, bool bShowAlone) {

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) routeId = " << routeId << ", bShowAlone = " << bShowAlone;

    _sRouteShowAlone._routeId = routeId;
    _sRouteShowAlone._bShowAlone = bShowAlone;

    qDebug() << __FUNCTION__ << "(GraphicsItemsContainer) _qmap_int_ptrGraphicsRouteItem.count() = " << _qmap_int_ptrGraphicsRouteItem.count();

    if ((!bShowAlone)||(routeId == -1)) { //if no route selected, we show all
        for (auto &gRouteItem_iter : _qmap_int_ptrGraphicsRouteItem) {
            gRouteItem_iter->setVisible(true);
        }
        return;
    }

    //bShowAlone  
    QMap<int, GraphicsRouteItem*>::const_iterator gRouteItem_cst_iter = _qmap_int_ptrGraphicsRouteItem.cbegin();
    for (; gRouteItem_cst_iter != _qmap_int_ptrGraphicsRouteItem.cend(); ++gRouteItem_cst_iter) {
        if (gRouteItem_cst_iter.key() == routeId) {
            gRouteItem_cst_iter.value()->setVisible(true);
        } else {
            gRouteItem_cst_iter.value()->setVisible(false);
        }
    }
}
