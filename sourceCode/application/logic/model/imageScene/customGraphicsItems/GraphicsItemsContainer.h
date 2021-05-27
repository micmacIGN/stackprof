#ifndef GRAPHICSITEMCONTAINER_HPP
#define GRAPHICSITEMCONTAINER_HPP //GRAPHICSITEMCONTAINER_HPP

#include <QObject>
#include <QMap>

#include <QSizeF>

class GraphicsRouteItem;
class RouteContainer;
class CustomGraphicsScene;

struct S_qpf_point_and_unitVectorDirection;

class CustomGraphicsTinyTargetPointItem;
class CustomGraphicsVectorDirectionAtPointItem;

//class CustomGraphicsBoxItem;
class CustomGraphicsBoxItem2;

class CustomGraphicsRectangularHighlightItem;

//class CustomGraphicsTinyTargetPointItem_notDragrable;
class CustomGraphicsTinyTargetPointItem_notDragrable2;

class GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd;

class CustomGraphicsSpecificItemsOfInterest2;

#include "../../core/ComputationCore_structures.h"

#include "../S_ForbiddenRouteSection.h"

#include "../../appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h"

class GraphicsItemsContainer : public QObject {

    Q_OBJECT
public:
    GraphicsItemsContainer();
    ~GraphicsItemsContainer();

    void initFromAndConnectTo_RouteContainer(RouteContainer* rcPtr);

    void initMinimalElementSizeSide_fromBackgroundSizeAndPercentFactor(const QSizeF& qsfBackgroundSize, double percent);

    void setDevicePixelRatioF(qreal devicePixelRatioF);

    void adjustLocationAndSizeToZLI(int ZLI);
    void setPtrCustomGraphicsScene(CustomGraphicsScene *ptrCustomGraphicsScene);
    void addAllRouteItem_and_TinyTargetPtNotDragable_toScene();

    void addPointToSelectedRoute(QPointF qpfScenePos);
    bool removeRouteWithRouteIdReorder(int idRoute);

    bool removeAllRouteItem();
    bool removeAllBoxesAndRelatedItems();

    void removeAllDebugItems();

    void remove_gRouteItemWForbiddenSectionsFSAE();

    void allocInitFromRouteAndAddHiddenToScene_locationOfSpecificGraphicsItemAboutRouteStartAndEnd();

    void setVisible_startAndEndOfRoute(bool state);//void setState_routeStartAndEndPointshighlighted(bool bState);

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

    void adjustLocationAndSizeToCurrentZLI_aboutSpecificGraphicsItemAboutRouteStartAndEnd();

    void adjustGraphicsItemWidth_aboutSpecificGraphicsItemAboutRouteStartAndEnd();

    void setGraphicsSettings(const S_GraphicsParameters_ItemOnImageView& sGP_itemOnImageView);

    bool oneBoxIsCurrentlyHoveredAndNotSelected(int &mapBoxId_ofHoveredAndNotSelected);

    void rotateColorSetsForItems();

    void routeSet_route_showAlone(int routeId, bool bShowAlone);
    void routeSet_route_showAlone(int routeId);

    private:
    S_GraphicsParameters_ItemOnImageView _sGP_itemOnImageView;
    void applyGraphicsSettings();

private:
    void alloc_pointAlongRouteForPointSelection_current();
    void remove_pointAlongRouteForPointSelection_current();

    void alloc_CGSpecificItemsOfInterest();
    void addToScene_hidden_CGSpecificItemsOfInterest();

public:

    void removeItemFromSceneAndDelete_CGSpecificItemsOfInterest();

    //void init_devDebug_tinyTargetPoint_fromConstQVectQPointF(const QVector<QPointF>& qvectqpf);
    void init_devDebug_tinyTargetPoint_fromConstQVectQPointF(int devDebugLayerID, const QVector<QPointF>& qvectqpf, qreal sideSizeNoZLI = 3.0);
    /*void init_devDebug_VectorDirectionAtPoint_fromConstQVect_point_and_unitVectorDirection(int devDebugLayerID,
                                                                    const QVector<ComputationCore::S_qpf_point_and_unitVectorDirection> &_qvect_unitVectorForEachPointAlongRoute);
    */
    void init_devDebug_VectorDirectionAtPoint_fromConstQVect_point_and_unitVectorDirection(
            int devDebugLayerID,
            const QVector<S_qpf_point_and_unitVectorDirection> &_qvect_unitVectorForEachPointAlongRoute);

    void init_devDebug_additional_temporaryDebugItem();

    void init_devDebug_CustomGraphicsRectangularHighlightItem_fromConstQVectPolygonF(const QVector<QPolygonF>& qvectPolygons);

    bool addBoxes_from_mapBoxAndStackedProfilWithMeasurements_fromKeyToKey(const QMap<int, S_BoxAndStackedProfilWithMeasurements>& qmapProfilsBoxParameters,
                                                                           int firstKey, int lastKey,
                                                                           bool& bCriticalErrorOccured);

    void addOneBox_from_profilBoxParameters(int mapBoxId, const S_ProfilsBoxParameters& profilsBoxParameters);
    void updateOneBox_givenMapBoxId_and_profilBoxParameters(int mapBoxId, const S_ProfilsBoxParameters& profilsBoxParameters);
    void updateSelectedBox_givenMapBoxId(int mapBoxId);

    void removeSelectionOfBoxes(const QVector<int> qvect_of_mapBoxIdToRemove);

    void resetGraphicsRouteItemFromRoute(int idRoute, bool bThisIsTheFirstAddedRoute); //bThisIsTheFirstAddedRoute: extra check about find/remove error

    bool getHoveredPointToSelectedRoute(int& idPoint, int& _ownedByIdRoute);

    void setRoute_hightlighted(int idRoute); //set also other routes as NotHightlighted
    void setRoute_notHightlighted(int idRoute); //only set to notHightlighted the idRoute

    void adjustLocationAndSizeToZLI_AllRouteItem();

    void setAddingBoxMode(bool bState);

    bool setAndShowForbiddenSectionAlongRouteForPointSelection(const S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRSFromStartAndEnd);

    void hide_targetCursorLocationOfPossibleCenterForAddBox();
    void setAndShow_targetCursorLocationOfPossibleCenterForAddBox(QPointF qpfPoint);
    void hideAndClear_specificItemsOfInterest();

public slots:
    void slot_pointHoverInOut_ownedByRoute(bool bHoverInOrOutstate, int idPoint, int idRoute);

private:

     void allocMinMaxPointAlongRouteForPointSelection();

     void connect_signalSlotToModel();
     void connect_signalSlot_updateOrRemovePointOfRoute(GraphicsRouteItem* gRouteItem);

     bool removeGraphicsRouteItem_noRouteIdReorder(int idRoute);//used for replacement of a fresh route equivalent

     void setCGBoxItemSelectable(bool bState);

     bool bCheckRouteContainerPermitsUseOfSpecificGraphicsItemAboutRouteStartAndEnd();

private:

    qreal _devicePixelRatioF;

    QSizeF _qsizefBackgroundSize_noZLI;
    double _minimalElementSideSize_noZLI;

    static constexpr double _cstExpr_double_minimalElementSideSize_noZLI = 1.415; //default value as close to 1 square diagonal length

    int _currentZLI;
    RouteContainer* _ptrRouteContainer;
    CustomGraphicsScene* _ptrCustomGraphicsScene;

    QMap<int, GraphicsRouteItem*> _qmap_int_ptrGraphicsRouteItem;

    //QVector<CustomGraphicsBoxItem *> _qvect_ptrCustomGraphicsBoxItem;
    QMap<int, CustomGraphicsBoxItem2 *> _qmap_insertionBoxId_ptrCustomGraphicsBoxItem;

    //debug, store ptr to be able to call adjustLocationAndSizeToZLI on it from GraphicsItemsContainer
    QVector<CustomGraphicsTinyTargetPointItem *> _qvect_ptrGraphicsTinyTargetPointItem;
    QVector<CustomGraphicsVectorDirectionAtPointItem *> _qvect_ptrGraphicsVectorDirectionAtPointItem;
    QVector<CustomGraphicsRectangularHighlightItem *> _qvect_ptrCustomGraphicsRectangularHighlightItem;

    enum e_AccessPointAsCenterForBoxInsertion {
        e_APACFBI_first = 0,
        e_APACFBI_last = 1,
        e_APACFBI_current = 2
    };
    CustomGraphicsTinyTargetPointItem_notDragrable2* _ptrCGTinyTargetPtNotDragable_First_Last_Current[3];

    S_ForbiddenRouteSectionsFromStartAndEnd _sForbiddenSectionsFromStartAndEnd;

    //about a route:
    bool _bHoverInOrOutState;
    int _currentHoveredIdPoint;
    int _currentHoveredIdPoint_IsOwnedByIdRoute;

    int _mapBoxId_currentBoxHighlighted;

    bool _bAddingBoxMode;

    //when the "adding box mode" is activated, the route from the qmap is hidden. And _gRouteItemWithForbiddenSection will be visible.
    //It will be built based on the route from the qmap and the point (with according segment owner) received, which defines the forbidden sections
    //(set in a different color)
    GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd *_ptrGRouteItemWForbiddenSectionsFSAE;

    //the GraphicsItem to highlight the first and last point of the route (for profil orientation or box distribution) is handled outside the route
    CustomGraphicsSpecificItemsOfInterest2 *_ptrCGSpecificItemsOfInterest;

    qreal _currentScaleOfViewSideForCurrentZLI;

    struct S_colorSet {
        QColor _qCol_segment_notHighlighted;
        QColor _qCol_segment_highlighted;
        QColor _qCol_point_notHighlighted;
    };
    QVector<S_colorSet> _qvectSColorSet;
    int _idxCurrentColorSet;

    struct S_routeShowAlone {
        int _routeId;
        bool _bShowAlone;
    };
    S_routeShowAlone _sRouteShowAlone;

};

#endif // GRAPHICSITEMCONTAINER_HPP
