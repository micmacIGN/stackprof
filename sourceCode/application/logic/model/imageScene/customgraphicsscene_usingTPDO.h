#ifndef CUSTOMGRAPHICSSCENE_H
#define CUSTOMGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "zoomLevelImage/zoomHandler.h"

#include "customGraphicsItems/GraphicsItemsContainer.h"

//test
//#include "../../model/core/routeContainer.h"

//#include "../../model/imageScene/customGraphicsItems/CustomGraphicsTargetedPointItem.h"

#include "../../controller/AppState_enum.hpp"

#include <QFutureWatcher>

class QMouseEvent;

//class TileProvider2;
class TileProvider_withDisplayOutput;

class CustomGraphicsSquareItem;

class CustomGraphicsBoxItem;

struct S_zoomLevelImgInfos;//

class CustomGraphicsScene : public QGraphicsScene {

    Q_OBJECT

public:
    CustomGraphicsScene(QObject *parent = nullptr);

    void clearContent_letPtrToAssociatedInstances();

    //bool createAndInitTilesProvider();
    bool createAndInitTilesProvider(const QString& qstrRootImage,
                                    const QString& qstrRootForZLIStorage,
                                    int oiio_cacheAttribute_maxMemoryMB,
                                    int indexToUseForTileProvider);

    bool initSceneToFitImageInWindowView(QSize viewSize);

    int get_maximumZoomLevelImg();

    S_zoomLevelImgInfos get_SzLIInfosJustAboveWH(int w, int h, int idxLayer);
    bool getImageSizeNoZoom(QSize& size);

    void setZoomHandlerPtr(ZoomHandler *ptrZoomHandler);
    void setGraphicsItemsContainerPtr(GraphicsItemsContainer *ptrGraphicsItemsContainer);

    ~CustomGraphicsScene() override;

   void setState_editingRoute(int ieRouteEditionState, int routeId);

   void setAddingBoxMode(bool bAddingBoxMode);

   bool tileProviderForIndexLayerIsLoaded(int ieLA);
   void switchToLayer(int ieLA);

   bool setGlobalZoomFactorTo1();
   bool setZoomStepIncDec(bool bStepIsIncrease);

   void routeSet_route_showAlone(int routeId, bool bShowAlone);
   void routeSet_route_showAlone(int routeId);

protected:
    //about point and box:
    //for insertion: 'i' key

    void keyPressEvent(QKeyEvent *event)  override;

    void drawBackground(QPainter *painter, const QRectF &exposed) override;

    //for debug purpose
    //void drawForeground(QPainter *painter, const QRectF &rect) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void update_and_sendScaleFactorToView(); //code close to slot_zoomRequest but for the special case of init

    void rotateColorSetsForItems();

private:
    void add_debugItems();
    void setSceneRectForCurrentZoomLevel();
    void setSceneRectForZoomLevel(int ZLI);
    void centerOnMouseCursorLocation();
    void fitImageInCurrentWindowSize();

    void setSceneCenterNoZLI(QPointF qpfCenterLcationInSceneNoZLI, bool bMoveMouseCursorToSceneCenter) ;

public slots:
    void slot_updateTiles();

    void slot_zoomRequest(int i_e_ZoomDir);

    void slot_visibleAreaChanged(QRectF visibleArea);//to inform small image nav

    void slot_setSceneCenterNoZLI_fromOutside(QPointF qpfMouseLocationInSceneNoZLI); //from small image nav


    //signal from smallnav scene:
    void slot_requestFromOutside_setGlobalZoomFactorTo1();
    void slot_requestFromOutside_fitImageInCurrentWindowSize();
    void slot_requestFromOutside_setZoomStepIncDec(bool bsetOnStepInc);


signals:
    void signal_adjustView(qreal scale);
    void signal_setCenterOn(qreal xLoc, qreal yLoc);
    //void signal_adjustBarLocation(qreal hLoc, qreal vLoc);
    void signal_currentExposedRectFChanged(QRectF exposed, int currentZoomLevelImg);

    void signal_sceneMousePosF_with_pixelValue(QPointF sceneMousePosF_noZLI, float fPixelValue);
    void signal_sceneMousePosF_noPixelValue(QPointF sceneMousePosF_noZLI);
    void signal_sceneMousePosF_outOfImage();

    void signal_setEnable_setGlobalZoomFactorTo1(bool bState);

    void signal_currentZLIChanged(int ZLI);

    void signal_addPointToSelectedRoute(QPointF scenePosNoZLI);

    void signal_removeHoveredPointToSelectedRoute(int idPoint, int IdRoute);

    void signal_tryInsertPointToCurrentSelectedRoute(QPointF scenePosNoZLI, int currentZLI);

    void signal_mouseMove_currentLocation(QPointF scenePosNoZLI, int currentZLI);
    void signal_tryAddBox(QPointF scenePosNoZLI, int currentZLI);

    void signal_enablePossibleActionsOnImageView(bool bEnable);

    //void signal_enable_zoomStepIncDec(bool bEnableInc, bool bEnableDec);
    void signal_enable_zoomStepInc(bool bEnableInc);
    void signal_enable_zoomStepDec(bool bEnableDec);
    void signal_setFocusWidgetOnZoomStepIncOrDec(bool bsetOnStepInc);

    void signal_adjustGraphicsItemWidthF(qreal scale);

    void signal_trySelectCurrentHoveredBox();

    //void signal_setMouseCursorPos(QPoint qpXY);

    void signal_callSceneToFitImageInWindowView();

    void signal_switchShowGridState();

private:
    bool loadingContextPermitsOperationAndNoNullPtr();

private:
   bool _bRefresh;
   QRectF _currentExposedRectF;

   QRectF _currentExposedRectF_fullArea;
   QPointF _qpfCurrentCenter;

   int _nbTilesinW_fullImage = 0;
   int _nbTilesinH_fullImage = 0;

   QVector<TileProvider_withDisplayOutput *> _qvectPtrTileProvider_wDO; //@#LP stay on the same variable name for now
   int _currentIndexOfUsedTileProvider;

   ZoomHandler *_ptrZoomHandler;
   GraphicsItemsContainer *_ptrGraphicsItemsContainer;

   //when using thread load, we need to keep the current ZLI before the transition (already set in the ZoomHandler) to be able to extend the zoom
   int _current_zoomLevelImage_forSceneSide;
   bool _bTilesLoadingRequest_dueToZoom;
   bool _bTilesLoading_dueToZoom;
   int _ZLIofTilesLoading_dueToZoom;
   bool _bSwitchZLIAfterEndOfLoad;

   QVector<int> _qvect_ZLIToLoad;

   QPointF _qpfCenterOn_current_aboutCurrentZLI;

   struct S_sceneMousePosF {
       QPointF _qpf;
       bool _bValid;
   };
   S_sceneMousePosF _sSceneMousePosF_noZLI_current;
   //S_sceneMousePosF_noZLI _sSceneMousePosF_noZLI_previous;
   S_sceneMousePosF _sceneMousePosF_aboutCurrentZLI_current;

   e_routeEditionState _eRouteEditionState;

   bool _bAddingBoxMode;

   QFutureWatcher<bool> _watcherBool;
   void load_ended();

   bool _bOneActionInProgressIsWaitingDrawingUpdate;

};

#endif // CUSTOMGRAPHICSSCENE_H
