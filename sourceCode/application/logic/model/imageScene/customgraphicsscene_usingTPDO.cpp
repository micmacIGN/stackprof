#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsItem>

#include <QGraphicsTextItem>

#include <QDebug>

#include <QKeyEvent>

#include "customgraphicsscene_usingTPDO.h"

#include <QCursor>

#include <QPainter>

#include "../tilesProvider/tileprovider_withDisplayOutput.h"

#include "../../zoomLevelImage/zoomHandler.h"

#include "customGraphicsItems/GraphicsItemsContainer.h"

#include "../../model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetedPointItem.h"

//#include  "../../model/imageScene/customGraphicsItems/CustomGraphicsBoxItem.h"

#include "S_booleanKeyStatus.hpp"

#include "controller/AppState_enum.hpp"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>


CustomGraphicsScene::CustomGraphicsScene(QObject *parent) : QGraphicsScene(parent),
    _bRefresh(false), //avoid to build the first display
    _currentExposedRectF {.0,.0,1.0,1.0}, //value as never used field
    _currentExposedRectF_fullArea {.0,.0,1.0,1.0},//value as never used field
    _qpfCurrentCenter {.0,.0},
    _nbTilesinW_fullImage { 0 },
    _nbTilesinH_fullImage { 0 },

    _qvectPtrTileProvider_wDO {nullptr, nullptr, nullptr},
    _currentIndexOfUsedTileProvider {-1},
    _ptrZoomHandler {nullptr},
    _ptrGraphicsItemsContainer{nullptr},

    _current_zoomLevelImage_forSceneSide {0},
    _bTilesLoadingRequest_dueToZoom{false},
    _bTilesLoading_dueToZoom{false},
    _ZLIofTilesLoading_dueToZoom{0},
    _bSwitchZLIAfterEndOfLoad{false},

    _sSceneMousePosF_noZLI_current{{.0,.0},false},
    _sceneMousePosF_aboutCurrentZLI_current{{.0,.0},false},

    _eRouteEditionState{e_routeEditionState::e_rES_locked}, //used to insert a point 'in a segment'
    _bAddingBoxMode{false}
{

    _bOneActionInProgressIsWaitingDrawingUpdate = false;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- false";

    connect(&_watcherBool, &QFutureWatcher<bool>::finished, this, &CustomGraphicsScene::load_ended);



}

//@LP this method does not take account if there is a TilesLoading in progress
void CustomGraphicsScene::clearContent_letPtrToAssociatedInstances() {

    _bRefresh = false;
    _currentExposedRectF = {.0,.0,1.0,1.0}; //value as never used field
    _currentExposedRectF_fullArea = {.0,.0,1.0,1.0}; //value as never used field
    _qpfCurrentCenter = {.0,.0};

    _nbTilesinW_fullImage = { 0 };
    _nbTilesinH_fullImage = { 0 };

    qDeleteAll(_qvectPtrTileProvider_wDO);
    _qvectPtrTileProvider_wDO.clear();
    _qvectPtrTileProvider_wDO.fill(nullptr, 3);
    _currentIndexOfUsedTileProvider = -1;

    _current_zoomLevelImage_forSceneSide = 0;

    _bTilesLoadingRequest_dueToZoom = false;
    _bTilesLoading_dueToZoom = false;
    _ZLIofTilesLoading_dueToZoom = 0;
    _bSwitchZLIAfterEndOfLoad = false;

    _qvect_ZLIToLoad.clear();

    _sSceneMousePosF_noZLI_current  = {{.0,.0},false};
    _sceneMousePosF_aboutCurrentZLI_current = {{.0,.0},false};

    _eRouteEditionState = e_routeEditionState::e_rES_locked;

    _bOneActionInProgressIsWaitingDrawingUpdate = false;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- false";
}

void CustomGraphicsScene::setState_editingRoute(int ieRouteEditionState, int routeId) {
    qDebug() << __FUNCTION__;
    if (_eRouteEditionState != ieRouteEditionState) {
        _eRouteEditionState = static_cast<e_routeEditionState>(ieRouteEditionState);

        qDebug() << __FUNCTION__ << " new _eRouteEditionState <= " << _eRouteEditionState;

        if (_ptrGraphicsItemsContainer) {
            if (_eRouteEditionState == e_routeEditionState::e_rES_locked) {
                qDebug() << __FUNCTION__ << " setRoute_notHightlighted (only one)";
                _ptrGraphicsItemsContainer->setRoute_notHightlighted(routeId);
            } else {
                qDebug() << __FUNCTION__ << " setRoute_hightlighted (one + all other in notHightlighted)" << _eRouteEditionState;
                _ptrGraphicsItemsContainer->setRoute_hightlighted(routeId);
            }
        }
    }
}

void CustomGraphicsScene::setAddingBoxMode(bool bAddingBoxMode) {
    _bAddingBoxMode = bAddingBoxMode;
}


//track mouse location to display in view the according geo coordinates and pixel value
void CustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    //qDebug() << __FUNCTION__ << "event->pos()      =" << event->pos();

    qDebug() << __FUNCTION__ << "event->scenePos() =" << event->scenePos();

    //_sceneMousePosF = event->scenePos();
    //qDebug() << __FUNCTION__ << "_sceneMousePosF =" << _sceneMousePosF;

    if (_ptrZoomHandler) {
        QPointF sceneMousePosF = event->scenePos();
        QPointF sceneMousePosF_noZLI = { sceneMousePosF.x() * _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide(),
                                         sceneMousePosF.y() * _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide()};


        _sceneMousePosF_aboutCurrentZLI_current = { sceneMousePosF, true };
        _sSceneMousePosF_noZLI_current = { sceneMousePosF_noZLI, true };

        //_qpf_current_sceneMousePosF_noZLI_previous = _qpf_current_sceneMousePosF_noZLI;
        //_qpf_current_sceneMousePosF_noZLI = sceneMousePosF_noZLI;

        //get the pixel value from the _tileProvider3 at the ZLI

        if (_currentIndexOfUsedTileProvider == -1) {
            QGraphicsScene::mouseMoveEvent(event);//forward
            return;
        }
        if (_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
            QPoint qpTileXYIndexes;
            QPoint qpXYInTile;
            bool bReport_findTileAndPosInTile = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->findTileAndPosInTileForFullImageXYPos(sceneMousePosF, qpTileXYIndexes, qpXYInTile);
            if (bReport_findTileAndPosInTile) {

                if (  (_bTilesLoading_dueToZoom)
                    ||(_bSwitchZLIAfterEndOfLoad)
                    ||(_bTilesLoadingRequest_dueToZoom))  {

                   qDebug() << __FUNCTION__ << "if ((_bTilesLoading_dueToZoom)||(_bSwitchZLIAfterEndOfLoad)||(_bTilesLoadingRequest_dueToZoom)) {";
                   emit signal_sceneMousePosF_noPixelValue(sceneMousePosF_noZLI);
                } else {
                    float fPixelValue = .0;

                    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
                        qDebug() << __FUNCTION__ << "if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {";
                        emit signal_sceneMousePosF_noPixelValue(sceneMousePosF_noZLI);
                    } else {
                        bool bReportGet = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getPixelValue(qpTileXYIndexes, qpXYInTile, fPixelValue);
                        qDebug() << __FUNCTION__ << "bReportGet getPixelValue said: " << bReportGet;
                        if (bReportGet) {
                            emit signal_sceneMousePosF_with_pixelValue(sceneMousePosF_noZLI, fPixelValue);
                           //qDebug() << __FUNCTION__ << "sceneMousePosF_noZLI =" << sceneMousePosF_noZLI << " fPixelValue =" << fPixelValue;
                        } else {
                            emit signal_sceneMousePosF_noPixelValue(sceneMousePosF_noZLI);
                            //qDebug() << __FUNCTION__ << "signal_sceneMousePosF_outOfImage_withErrorStatus: error = true";
                        }
                    }
                }
            } else {
                emit signal_sceneMousePosF_outOfImage();
                // qDebug() << __FUNCTION__ << "signal_sceneMousePosF_outOfImage_withErrorStatus: error is false";
            }
        }
        if (_bAddingBoxMode) {
            //qDebug() << __FUNCTION__ << "(CustomGraphicsScene): _bAddingBoxMode activated";
            emit signal_mouseMove_currentLocation(_sSceneMousePosF_noZLI_current._qpf, _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
        }
    }

    QGraphicsScene::mouseMoveEvent(event);//forward
}



void CustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        event->accept();
        return;
    }
    /*
    if (  (_bTilesLoading_dueToZoom)
        ||(_bSwitchZLIAfterEndOfLoad)
        ||(_bTilesLoadingRequest_dueToZoom))  {
        event->accept();
        return;
    }*/

    bool bLeftButtonPressed = event->button() == Qt::LeftButton;
    S_booleanKeyStatus SbkeyStatus;

    if (_eRouteEditionState == e_rES_inEdition) {

        qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _eRouteEditionState is e_rES_inEdition";

        bool bAddPointToRoute = bLeftButtonPressed && SbkeyStatus._bShiftKeyAlone;
        if (bAddPointToRoute) {

            qDebug() << __FUNCTION__ << "(CustomGraphicsScene) bAddPointToRoute is true";

            QPointF scenePosNoZLI = event->scenePos() * _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
            emit signal_addPointToSelectedRoute(scenePosNoZLI);
            event->accept();

            return;
        }
   }

   QGraphicsScene::mousePressEvent(event);
}

void CustomGraphicsScene::keyPressEvent(QKeyEvent *event) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    if (event->count() > 1) { event->accept(); return; }
    if (event->isAutoRepeat()) { event->accept(); return; }

    if (!_ptrGraphicsItemsContainer) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsScene): if (!_ptrGraphicsItemsContainer) {";
        event->accept();
        return;
    }

    if (!_ptrZoomHandler) {
        event->accept();
        return;
    }


    //Some other windows react on CtrlKeys+keys. Keys which is used here without CtrlKeys (S, D, G for example)
    //to avoid unwanted action, filter action using CtrlKeys status
    S_booleanKeyStatus SbkeyStatus;
    if (!SbkeyStatus._bNonePressed) {
        event->accept();
        return;
    }


    //permit zoom without waiting current load
    //--------------------
    bool bReport = false;
    int hoveredIdPoint = -1;
    int hoveredIdPoint_ownedBuyIdRoute = -1;

    bool bHandled = false;

    switch (event->key()) {

        //'H' and 'K' keyboard keys are around 'J'.
        case Qt::Key_H://zoom minus (go outside image). 'H' as 'Higher'
        //case Qt::Key_K://zoom minus (go outside image). added key to do not care which side is from zoom plus (the 'J' key)
            setZoomStepIncDec(false);
            bHandled = true;
            break;

        case Qt::Key_J: //zoom plus (go inside image). The 'J' keyboard key has a mark (like the 'F' keyboard key)
            setZoomStepIncDec(true);
            bHandled = true;
            break;

        default:
            break;
    }

    if (bHandled) {
        event->accept();
        return;
    }
    //-------------------


    if (  (_bTilesLoading_dueToZoom)
        ||(_bSwitchZLIAfterEndOfLoad)
        ||(_bTilesLoadingRequest_dueToZoom))  {        
        event->accept();
        return;
    }

    switch (event->key()) {

        case Qt::Key_I: //try to insert point to current selected route        
            if (_eRouteEditionState == e_rES_inEdition) {                
                qDebug() << __FUNCTION__ << "(CustomGraphicsScene): Key_I: " << event->key();
                emit signal_tryInsertPointToCurrentSelectedRoute(_sSceneMousePosF_noZLI_current._qpf, _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
                bHandled = true;
            }
            break;

        case Qt::Key_D: //try to remove hovered point to selected route
            if (_eRouteEditionState == e_rES_inEdition) {                
                qDebug() << __FUNCTION__ << "(CustomGraphicsScene): Key_D: " << event->key();
                bReport = _ptrGraphicsItemsContainer->getHoveredPointToSelectedRoute(hoveredIdPoint, hoveredIdPoint_ownedBuyIdRoute);
                if (bReport) {
                    qDebug() << __FUNCTION__ << "will now emit signal_removeHoveredPointToSelectedRoute("
                             << " hoveredIdPoint = " << hoveredIdPoint
                             << " hoveredIdPoint_ownedBuyIdRoute = " << hoveredIdPoint_ownedBuyIdRoute;
                    emit signal_removeHoveredPointToSelectedRoute(hoveredIdPoint, hoveredIdPoint_ownedBuyIdRoute);
                }
                bHandled = true;
            }
            break;

        case Qt::Key_A: //try to add box
            if (_bAddingBoxMode) {
                qDebug() << __FUNCTION__ << "(CustomGraphicsScene): Key_A: " << event->key();
                emit signal_tryAddBox(_sSceneMousePosF_noZLI_current._qpf, _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
            }
            bHandled = true;
            break;

        case Qt::Key_S: //try to select current hovered box
            if (!_bAddingBoxMode) {
                qDebug() << __FUNCTION__ << "(CustomGraphicsScene): Key_S: QWidget *QApplication::focusWidget() = ";
                emit signal_trySelectCurrentHoveredBox();
            }
            bHandled = true;
            break;

        case Qt::Key_R: //'rotate' color set
            rotateColorSetsForItems();
            bHandled = true;
            break;

        case Qt::Key_Space:
            setGlobalZoomFactorTo1();
            bHandled = true;
            break;

        /*case Qt::Key_C:
            centerOnMouseCursorLocation();
            bHandled = true;
            break;*/

        case Qt::Key_F:
            fitImageInCurrentWindowSize();
            bHandled = true;
            break;

        case Qt::Key_G: //show or hide grid
            emit signal_switchShowGridState();
            bHandled = true;
            break;

        default:
            break;
    }

    if (bHandled) {
        event->accept();

    } else {
        QGraphicsScene::keyPressEvent(event);
    }
}

void CustomGraphicsScene::rotateColorSetsForItems() {
    _ptrGraphicsItemsContainer->rotateColorSetsForItems();
}

void CustomGraphicsScene::setGraphicsItemsContainerPtr(GraphicsItemsContainer *ptrGraphicsItemsContainer) {
    if (_ptrGraphicsItemsContainer) {
        qDebug() << __FUNCTION__ << ": error: a GraphicsItemsContainer was already set";
        return;
    }
    _ptrGraphicsItemsContainer = ptrGraphicsItemsContainer;
}


void CustomGraphicsScene::setZoomHandlerPtr(ZoomHandler *ptrZoomHandler) {
    _ptrZoomHandler = ptrZoomHandler;
}


CustomGraphicsScene::~CustomGraphicsScene() {

    qDeleteAll(_qvectPtrTileProvider_wDO);
    _qvectPtrTileProvider_wDO.clear();
    _qvectPtrTileProvider_wDO.fill(nullptr, 3);
}

void CustomGraphicsScene::add_debugItems() {

    if (_currentIndexOfUsedTileProvider == -1) {
        return;
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        qDebug() << __FUNCTION__ << "can't: _tileProvider3 is nullptr";
        return;
    }

    QPen p;
    p.setStyle(Qt::PenStyle::SolidLine);
    p.setColor(Qt::black);
    p.setCosmetic(true);

    int tileW = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->tileW();
    int tileH = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->tileH();

    int nbTilesinH_fullImage=0, nbTilesinW_fullImage=0;
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getNbTiles_fullImage(nbTilesinH_fullImage, nbTilesinW_fullImage);

    for (int y = 0; y < nbTilesinH_fullImage+1; y++) {
       addLine(0, y*tileH, nbTilesinW_fullImage*tileW,  y*tileH, p);
    }
    for (int x = 0; x < nbTilesinW_fullImage+1; x++) {
       addLine(x*tileW, 0, x*tileW, nbTilesinH_fullImage*tileH, p);
    }

    for (int y = 0; y < nbTilesinH_fullImage; y++) {
       for (int x = 0; x < nbTilesinW_fullImage; x++) {
           QString strTileXY_pixXY;
           strTileXY_pixXY.append(QString::number(x));
           strTileXY_pixXY.append(", ");
           strTileXY_pixXY.append(QString::number(y));
           strTileXY_pixXY.append("\npix:");

           strTileXY_pixXY.append(QString::number(x*tileW));
           strTileXY_pixXY.append(", ");
           strTileXY_pixXY.append(QString::number(y*tileH));

           QGraphicsTextItem* QGTextItemAdded = addText(strTileXY_pixXY);
           if (QGTextItemAdded) {
               QGTextItemAdded->setPos(  x*tileW + tileW/10.0,  y*tileH + tileH/10.0);
           }
       }
    }
}



void CustomGraphicsScene::setSceneRectForCurrentZoomLevel() {
    qDebug() << "* CustomGraphicsScene::setSceneRectForCurrentZoomLevel()";
    qDebug() << "* CustomGraphicsScene::setSceneRectForCurrentZoomLevel() call setSceneRectForZoomLevel with _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = " << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
    setSceneRectForZoomLevel(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
}

void CustomGraphicsScene::setSceneRectForZoomLevel(int ZLI) {

    qDebug() << "CustomGraphicsScene::setSceneRectForZoomLevel with ZLI = " << ZLI;

    int borderAroundWidth = 1000;
    int borderAroundHeight = 1000;

    bool bUseEmptyScene = false;
    if (_currentIndexOfUsedTileProvider == -1) {
        bUseEmptyScene = true;
    }
    if (_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider] == nullptr) {
         bUseEmptyScene = true;
    }

    if (!bUseEmptyScene) {

        qDebug() << __FUNCTION__ << "if (!bUseEmptyScene) {";

        QRectF rectSceneF(
                    -borderAroundWidth,-borderAroundHeight, //xmin,ymin
                    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageW()+2*borderAroundWidth,
                    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageH()+2*borderAroundHeight);
        setSceneRect(rectSceneF);

        qDebug() << __FUNCTION__ << "******** setSceneRect("
                 << -borderAroundWidth  << ", "
                 << -borderAroundHeight << ", "
                 << _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageW()+2*borderAroundWidth  << ", "
                 << _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageH()+2*borderAroundHeight;

        _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getNbTiles_fullImage(_nbTilesinW_fullImage, _nbTilesinH_fullImage);
        qDebug() << __FUNCTION__ << "w, h :" << _nbTilesinW_fullImage << ", " << _nbTilesinH_fullImage;

    } else {

        qDebug() << __FUNCTION__ << ": error: tileProvider is nullptr";
        //set a default false valid scene rect
         QRect rectSceneF(   -borderAroundWidth,-borderAroundHeight, //xmin,ymin
                         1+2*borderAroundWidth, 1+2*borderAroundHeight);
         setSceneRect(rectSceneF);
    }

    //'old school' way to propagate the ZLI value for now (futur: replace by signal/slot ?)
    if (_ptrGraphicsItemsContainer) {
        _ptrGraphicsItemsContainer->adjustLocationAndSizeToZLI(ZLI);
    }

    qDebug() << __FUNCTION__ << "set now _current_zoomLevelImage_forSceneSide to ZLI = " << ZLI;

    _current_zoomLevelImage_forSceneSide = ZLI;

    emit signal_currentZLIChanged(_current_zoomLevelImage_forSceneSide);


}




int CustomGraphicsScene::get_maximumZoomLevelImg() {

    if (_currentIndexOfUsedTileProvider == -1) {
        return(0);
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) { //@LP considers that _qvectPtrTileProvider_wDO can be filled with nullptr but never empty
        qDebug() << __FUNCTION__ << ": error: _tileProvider3 is nullptr";
        return(0);
    }
    return(_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->get_maximumZoomLevelImg());
}

bool CustomGraphicsScene::createAndInitTilesProvider(const QString& qstrRootImage,
                                                     const QString& qstrRootForZLIStorage,
                                                     int oiio_cacheAttribute_maxMemoryMB,
                                                     int indexToUseForTileProvider) {

    if ((indexToUseForTileProvider < 0)||(indexToUseForTileProvider > 2 )) {
        return(false);
    }

    if (_qvectPtrTileProvider_wDO[indexToUseForTileProvider] == nullptr) {


        qDebug() << __FUNCTION__ << "oiio_cacheAttribute_maxMemoryMB = " << oiio_cacheAttribute_maxMemoryMB;

        _qvectPtrTileProvider_wDO[indexToUseForTileProvider] = new TileProvider_withDisplayOutput(qstrRootImage.toStdString(),
                                                                                                  qstrRootForZLIStorage.toStdString(),
                                                                                                  oiio_cacheAttribute_maxMemoryMB);

    }

    if (_qvectPtrTileProvider_wDO[indexToUseForTileProvider] == nullptr) {
        qDebug() << __FUNCTION__ << "error: _tileProvider is nullptr";
        return(false);
    }

    if (!_qvectPtrTileProvider_wDO[indexToUseForTileProvider]->prepareTilesSetEngine()) {
        qDebug() << __FUNCTION__ << ": error in _tileProvider3->prepareTilesSetEngine()";
        return(false);
    }

    //switch to the newly loaded if current was -1
    if (_currentIndexOfUsedTileProvider == -1) {
        _currentIndexOfUsedTileProvider = indexToUseForTileProvider;
    }

    qDebug() << __FUNCTION__ << "ending";

    return(true);
}

S_zoomLevelImgInfos CustomGraphicsScene::get_SzLIInfosJustAboveWH(int w, int h, int idxLayer) {

    if ((idxLayer < 0)||(idxLayer > 2)) {
        ImageSpec ImageSpecNoCandidate;
        S_zoomLevelImgInfos SzLIInfosCandidate {"", ImageSpecNoCandidate, 0, 0, 0, 0 };
        return(SzLIInfosCandidate);
    }
    if (!_qvectPtrTileProvider_wDO[idxLayer]) {

        ImageSpec ImageSpecNoCandidate;
        S_zoomLevelImgInfos SzLIInfosCandidate {"", ImageSpecNoCandidate, 0, 0, 0, 0 };
        return(SzLIInfosCandidate);
    }
    return(_qvectPtrTileProvider_wDO[idxLayer]->get_SzLIInfosJustAboveWH(w,h));
}


bool CustomGraphicsScene::getImageSizeNoZoom(QSize& size) {
    if (_currentIndexOfUsedTileProvider == -1) {
         return(false);
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
       return(false);
    }
    return(_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(size));
}



bool CustomGraphicsScene::initSceneToFitImageInWindowView(QSize viewSize) {

    qDebug() << __FUNCTION__ << "initSceneToFitImageInWindowView viewSize = " << viewSize;

    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        qDebug() << __FUNCTION__ << "if (!loadingContextPermitsOperationAndNoNullPtr()) {";
        return(false);
    }

    if (_currentIndexOfUsedTileProvider == -1) {
        qDebug() << __FUNCTION__ << "if (_currentIndexOfUsedTileProvider == -1) {";
         return(false);
    }
    if ((!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider])||(!_ptrZoomHandler)) {
        qDebug() << __FUNCTION__ << "if ((!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider])||(!_ptrZoomHandler)) {";
        return(false);
    }

    //---
    _bOneActionInProgressIsWaitingDrawingUpdate = true;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";
    //---

    QSize imageSizeNoZoom {0,0};
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(imageSizeNoZoom);

    qreal widthRatio  = static_cast<qreal>(viewSize.width())  / static_cast<qreal>(imageSizeNoZoom.width());
    qreal heightRatio = static_cast<qreal>(viewSize.height()) / static_cast<qreal>(imageSizeNoZoom.height());

    qreal ratioToUse = widthRatio < heightRatio ? widthRatio : heightRatio;
    ratioToUse = 1.0 / ratioToUse;

    qDebug() << __FUNCTION__ << " ratioToUse : " << ratioToUse;

    qDebug() << __FUNCTION__ << " imageSizeNoZoom * ratioTo use would result in size: "
             << imageSizeNoZoom.width()/ratioToUse
             << " x "
             << imageSizeNoZoom.height()/ratioToUse;

    int indexOfZoomValue = _ptrZoomHandler->findClosestZoomIndexOfZoomValueForGlobalZoomFactor(ratioToUse);

    _ptrZoomHandler->setToZoomFromIdx(indexOfZoomValue);

    //select here the zoom level and location of what we want to see on screen
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->selectZoomLevelImgAndLoadFullImageToDisplayInSize(
        _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

    qDebug() << __FUNCTION__ << "call now setSceneRectForCurrentZoomLevel @line "<< __LINE__;
    setSceneRectForCurrentZoomLevel();

    //add_debugItems();

    update_and_sendScaleFactorToView();

    QPointF currentCenter_noZLI = {
        _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageW()/2.0,
        _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->fullImageH()/2.0
    };

    qDebug() << __FUNCTION__ << "dbg signal_setCenterOn @line "<< __LINE__;
    _qpfCenterOn_current_aboutCurrentZLI = currentCenter_noZLI;
    emit signal_setCenterOn(currentCenter_noZLI.x(), currentCenter_noZLI.y());

    _bRefresh = true; //set to do the first display

    //_bOneActionInProgress = false;

    return(true);
}


void CustomGraphicsScene::update_and_sendScaleFactorToView() {

    if (!_ptrZoomHandler) {
        qDebug() << __FUNCTION__ << " error: _ptrZoomHandler is nullptr";
        return;
    }

    if (_currentIndexOfUsedTileProvider == -1) {
        return;
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        qDebug() << __FUNCTION__ << " error: _tileProvider_wDO is nullptr";
        return;
    }

    QSize qsizeImageNoZoom {0, 0};
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(qsizeImageNoZoom);
    qDebug() << __FUNCTION__ << "qsizeImageNoZoom =" << qsizeImageNoZoom;


    QPointF sceneMousePosF_noZLI_beforeZoom {.0,.0};


        //qDebug() << __FUNCTION__ << "_sceneMousePosF: " << _sceneMousePosF;

        qreal xMulZLI = /*_sceneMousePosF.x()*/(qsizeImageNoZoom.width ()/2.0)*_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
        qreal yMulZLI = /*_sceneMousePosF.y()*/(qsizeImageNoZoom.height()/2.0)*_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();

        qDebug() << __FUNCTION__ << "xMulZLI =" << xMulZLI;
        qDebug() << __FUNCTION__ << "yMulZLI =" << yMulZLI;

        sceneMousePosF_noZLI_beforeZoom.setX(xMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
        sceneMousePosF_noZLI_beforeZoom.setY(yMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

        qDebug() << __FUNCTION__ << "static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() ="
                 << static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

        qDebug() << __FUNCTION__ << "sceneMousePosF_noZLI_beforeViewScaleAdjustement =" << sceneMousePosF_noZLI_beforeZoom;
    //}

    ZoomHandler::e_zoomLevelImageTransition zoomLevelImageTransition = _ptrZoomHandler->getCurrent_zoomLevelImageTransition();

    bool bSetBarLoc = false;

    if (zoomLevelImageTransition != ZoomHandler::e_zLIT_none) {

        qDebug() << __FUNCTION__ << "zoomLevelImageTransition != none: "
              << " globalZF = "   << _ptrZoomHandler->getCurrent_globalZoomFactor() << " , "
              << " ZLI = "        << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() << " , "
              << " Scale = "      << _ptrZoomHandler->getCurrent_scale_forViewSide() << " , "
              << " transition = " << _ptrZoomHandler->getCurrent_zoomLevelImageTransition();

        bSetBarLoc = true;

        int currentZoomLevelImg = _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
        _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->set_currentZoomLevelImg(currentZoomLevelImg);
        //@LP the others layers received none changes

        qDebug() << __FUNCTION__ << "call now setSceneRectForCurrentZoomLevel @line "<< __LINE__;
        setSceneRectForCurrentZoomLevel();

        _bRefresh = true;
        qDebug() << __FUNCTION__ << "call now update()";
        //update(); //@#LP update() works well but use invalidate instead;
        invalidate();

    } else {

        qDebug() << __FUNCTION__
              << " globalZF = "   << _ptrZoomHandler->getCurrent_globalZoomFactor() << " , "
              << " ZLI = "        << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() << " , "
              << " Scale = "      << _ptrZoomHandler->getCurrent_scale_forViewSide() << " , "
              << " noTransition";
        _bRefresh = true;
        //update(); //@#LP update() works well but use invalidate instead;
        invalidate();
    }



    qDebug() << __FUNCTION__ << "emit now signal_adjustView with " << _ptrZoomHandler->getCurrent_scale_forViewSide();
    emit signal_adjustView(_ptrZoomHandler->getCurrent_scale_forViewSide());

    //--*- adjust item width
    if (_ptrGraphicsItemsContainer) {
        _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(_ptrZoomHandler->getCurrent_scale_forViewSide());
    }//-*--

}


void CustomGraphicsScene::slot_zoomRequest(int i_e_ZoomDir/*, QPointF mouseViewPosMappedToScene*/) {

    if (!_ptrZoomHandler) {
        qDebug() << __FUNCTION__ << " error: _ptrZoomHandler is nullptr";
        return;
    }

    if (_currentIndexOfUsedTileProvider == -1) {
        return;
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        qDebug() << __FUNCTION__ << " error: _tileProvider_wDO is nullptr";
        return;
    }

    QSize qsizeImageNoZoom {0, 0};
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(qsizeImageNoZoom);
    qDebug() << __FUNCTION__ << "qsizeImageNoZoom =" << qsizeImageNoZoom;


    QPointF sceneCenterPosF_noZLI_beforeZoom_percent {.0,.0};
    QPointF currentCenter = _qpfCurrentCenter; //_currentExposedRectF_forCenterOn.center();

    //qDebug() << __FUNCTION__ << "_currentExposedRectF (_forCenterOn):" << _currentExposedRectF_forCenterOn;
    //qDebug() << __FUNCTION__ << "_currentExposedRectF (_forCenterOn) .topLeft:" << _currentExposedRectF_forCenterOn.topLeft();
    //qDebug() << __FUNCTION__ << "_currentExposedRectF (_forCenterOn) .bottomRight:" << _currentExposedRectF_forCenterOn.bottomRight();
    qDebug() << __FUNCTION__ << "get currentCenter <= _qpfCurrentCenter (_forCenterOn):" << currentCenter;


    //compute the current center location in ZLI(1) percent
    //to be able to set center on the same location, taking into account the current ZLI

    //example:
    //w for ZLI(1) = 1000
    //
    //_current_zoomLevelImage_forSceneSide = 2  (hence w for ZLI(2) = 500
    //current centerx = 250
    //centerxMulZLI = 250*2 = 500
    //sceneCenterPosF_noZLI_beforeZoom = 500 / 1000 = 0.5
    //
    //wFullImage = 1000 / 2 = 500
    //qpfCenterOn.setX(0.5 * 500) => 250

    qreal centerxMulZLI = .0;
    qreal centeryMulZLI = .0;

    qDebug() << __FUNCTION__ << "_bTilesLoading_dueToZoom = " << _bTilesLoading_dueToZoom;
    qDebug() << __FUNCTION__ << "_current_zoomLevelImage_forSceneSide = " <<_current_zoomLevelImage_forSceneSide;
    qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = " <<_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();


    //qDebug() << __FUNCTION__ << " if (_bTilesLoading_dueToZoom) { => use _current_zoomLevelImage_forSceneSide = " << _current_zoomLevelImage_forSceneSide;

    centerxMulZLI = currentCenter.x()*_current_zoomLevelImage_forSceneSide;
    centeryMulZLI = currentCenter.y()*_current_zoomLevelImage_forSceneSide;


    //qDebug() << __FUNCTION__ << "centerxMulZLI =" << centerxMulZLI;
    //qDebug() << __FUNCTION__ << "centeryMulZLI =" << centeryMulZLI;

    sceneCenterPosF_noZLI_beforeZoom_percent.setX(centerxMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
    sceneCenterPosF_noZLI_beforeZoom_percent.setY(centeryMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

    qDebug() << __FUNCTION__ << "sceneCenterPosF_noZLI_beforeZoom_percent =" << sceneCenterPosF_noZLI_beforeZoom_percent;


    bool bMaxOutLevelReached = false;
    bool bMinInLevelReached = false;
    //bool bLimitLevelReached = false;

    bool bZoomChanged = false;
    if (i_e_ZoomDir == ZoomHandler::e_ZoomDir_Out) {
        qDebug() << __FUNCTION__ << " call now _ptrZoomHandler->zoomOut()";        
        bZoomChanged  = _ptrZoomHandler->zoomOut(/*bLimitLevelReached*/bMaxOutLevelReached);
    } else { //e_ZoomDir_In
        qDebug() << __FUNCTION__ << " call now _ptrZoomHandler->zoomIn()";
        bZoomChanged  = _ptrZoomHandler->zoomIn(/*bLimitLevelReached*/bMinInLevelReached);
    }

    if (i_e_ZoomDir == ZoomHandler::e_ZoomDir_Out) {
        bool bEnableZoomStepDec = !/*bLimitLevelReached*/bMaxOutLevelReached && bZoomChanged;
        emit signal_enable_zoomStepDec(bEnableZoomStepDec);
        emit signal_enable_zoomStepInc(true);//considers that zooming Out implies that zooming In is a possible action

        if (bEnableZoomStepDec) {
            emit signal_setFocusWidgetOnZoomStepIncOrDec(false); //set focus on minus (zoom Out)
        } else {
            emit signal_setFocusWidgetOnZoomStepIncOrDec(true); //set focus on plus (zoom In)
        }

    } else {
        bool bEnableZoomStepInc = !/*bLimitLevelReached*/bMinInLevelReached && bZoomChanged;
        emit signal_enable_zoomStepInc(bEnableZoomStepInc);
        emit signal_enable_zoomStepDec(true);//considers that zooming In implies that zooming out is a possible action

        if (bEnableZoomStepInc) {
            emit signal_setFocusWidgetOnZoomStepIncOrDec(true); //set focus on plus (zoom In)
        } else {
            emit signal_setFocusWidgetOnZoomStepIncOrDec(false); //set focus on minus (zoom Out)
        }
    }

    if (!bZoomChanged) {
        qDebug() << __FUNCTION__ << " bZoomChanged is false";
        return;
    }

    //_bOneActionInProgressIsWaitingDrawingUpdate = true;
    //qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";

    ZoomHandler::e_zoomLevelImageTransition  zoomLevelImageTransition = _ptrZoomHandler->getCurrent_zoomLevelImageTransition();

    //the _ptrZoomHandler follows all zoom requests but when the zoom level requiered a tiles load,
    //the _current_zoomLevelImage_forSceneSide is not updated. And the requiered tiles load to match
    //with this zoom request is made in a parallele thread.

    if (zoomLevelImageTransition != ZoomHandler::e_zLIT_none) {

        qDebug() << __FUNCTION__ << "zoomLevelImageTransition != none: "
              << " globalZF = "   << _ptrZoomHandler->getCurrent_globalZoomFactor() << " , "
              << " ZLI = "        << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() << " , "
              << " Scale = "      << _ptrZoomHandler->getCurrent_scale_forViewSide() << " , "
              << " transition = " << _ptrZoomHandler->getCurrent_zoomLevelImageTransition();

        int zoomLevelImgToLoad = _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();

        //the transition matches in fact with the current ZLI (the user has made zoom in + zoom out for example)
        if (zoomLevelImgToLoad == _current_zoomLevelImage_forSceneSide) {

            qDebug() << __FUNCTION__ << "if (zoomLevelImgToLoad == _current_zoomLevelImage_forSceneSide) {";

            //no ZLI load requiered

            //
            //@#LP: release of the previous ZLI loaded if != _current_zoomLevelImage_forSceneSide can be made
            //

            //release and load tile about the current ZLI
            _bRefresh = true;



            qDebug() << __FUNCTION__ << "emit now signal_adjustView";

            //adjust the view scale
            emit signal_adjustView(_ptrZoomHandler->getCurrent_scale_forViewSide());

            //--*- adjust item width
            if (_ptrGraphicsItemsContainer) {
                _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(_ptrZoomHandler->getCurrent_scale_forViewSide());
            }//-*--

            //set on center, according to the current ZLI

            //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
            qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;
            qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;

            qDebug() << __FUNCTION__ << "computed new hwFullImage to compute qpfCenterOn= ";
            qDebug() << __FUNCTION__ << "wFullImageAtZLI = " << wFullImageAtZLI;
            qDebug() << __FUNCTION__ << "hFullImageAtZLI = " << hFullImageAtZLI;

            QPointF qpfCenterOn {.0,.0};

            qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
            qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

            qDebug() << __FUNCTION__ << "qpfCenterOn computed using sceneCenterPosF_noZLI_beforeZoom_percent: " << qpfCenterOn;

            qDebug() << __FUNCTION__ << "dbg signal_setCenterOn @line "<< __LINE__;

            qDebug() << __FUNCTION__ << "emit now signal_setCenterOn";

            _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;
            emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());

            bool bGlobalZoomFactorIs1 = (qAbs((_ptrZoomHandler->getCurrent_globalZoomFactor() - 1.0)) < 0.01);
            qDebug() << __FUNCTION__ << "bGlobalZoomFactorIs1 ="<< bGlobalZoomFactorIs1;
            emit signal_setEnable_setGlobalZoomFactorTo1(!bGlobalZoomFactorIs1);

            return;

        }  else { //ZLI of the required zoom does not match with the current ZLI

            qDebug() << __FUNCTION__ << "(zoomLevelImgToLoad != _current_zoomLevelImage_forSceneSide)";

            //push the ZLI in the 'LIFO'
            _qvect_ZLIToLoad.push_back(zoomLevelImgToLoad);

            if (_bTilesLoading_dueToZoom) { //a ZLI load already in progress

                qDebug() << __FUNCTION__ << "if (_bTilesLoading_dueToZoom) {";

                //adjust the view to simulate the zoom with the current Tiles of the current ZLI
                qreal viewScaleForExtendedScale = _ptrZoomHandler->getViewScaleForExtendedScale(_current_zoomLevelImage_forSceneSide);
                qDebug() << __FUNCTION__ << "emit now signal_adjustView";
                emit signal_adjustView(viewScaleForExtendedScale);

                //@LP commented to try to avoid little line width blinking due to coming soon width adjustement later
                //--*- adjust item width
                //if (_ptrGraphicsItemsContainer) {
                //    _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(/*viewScaleForExtendedScale*/_ptrZoomHandler->getCurrent_scale_forViewSide());
                //}//-*--


                //set on center, according to the current ZLI

                //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
                qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;
                qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;

                qDebug() << __FUNCTION__ << "computed new hwFullImage to compute qpfCenterOn= ";
                qDebug() << __FUNCTION__ << "wFullImageAtZLI = " << wFullImageAtZLI;
                qDebug() << __FUNCTION__ << "hFullImageAtZLI = " << hFullImageAtZLI;

                QPointF qpfCenterOn {.0,.0};

                qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
                qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

                qDebug() << __FUNCTION__ << "qpfCenterOn computed using sceneCenterPosF_noZLI_beforeZoom_percent: " << qpfCenterOn;

                qDebug() << __FUNCTION__ << "dbg signal_setCenterOn @line "<< __LINE__;
                _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;
                emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());


            } else { //no ZLI loading in progress

                //we can launch a load (this will take the last pushed ZLI value in the _qvect_ZLIToLoad ('LIFO'))
                _bTilesLoadingRequest_dueToZoom = true;

                //adjust the view to simulate the zoom with the current Tiles of the current ZLI
                qreal viewScaleForExtendedScale = _ptrZoomHandler->getViewScaleForExtendedScale(_current_zoomLevelImage_forSceneSide);
                qDebug() << __FUNCTION__ << "emit now signal_adjustView";
                emit signal_adjustView(viewScaleForExtendedScale);

                //@LP commented to try to avoid the width blinking
                //--*- adjust item width
                //if (_ptrGraphicsItemsContainer) {
                //    _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(/*viewScaleForExtendedScale*/_ptrZoomHandler->getCurrent_scale_forViewSide());
                //}//-*--

                //set on center, according to the current ZLI

                //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
                qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;
                qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;

                qDebug() << __FUNCTION__ << "computed new hwFullImage to compute qpfCenterOn= ";
                qDebug() << __FUNCTION__ << "wFullImageAtZLI = " << wFullImageAtZLI;
                qDebug() << __FUNCTION__ << "hFullImageAtZLI = " << hFullImageAtZLI;

                QPointF qpfCenterOn {.0,.0};

                qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
                qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

                qDebug() << __FUNCTION__ << "qpfCenterOn computed using sceneCenterPosF_noZLI_beforeZoom_percent: " << qpfCenterOn;

                qDebug() << __FUNCTION__ << "dbg signal_setCenterOn @line "<< __LINE__;
                _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;
                emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());

            }
        }
        return;
    }

    //no ZLI transition; we stay in the same ZLI

    //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
    if (_bTilesLoading_dueToZoom) { //a ZLI load already in progress

       qDebug() << __FUNCTION__ << "if (_bTilesLoading_dueToZoom) {";

       //adjust the view to simulate the zoom with the current Tiles of the current ZLI
       qreal viewScaleForExtendedScale = _ptrZoomHandler->getViewScaleForExtendedScale(_current_zoomLevelImage_forSceneSide);
       qDebug() << __FUNCTION__ << "emit now signal_adjustView";

       _bRefresh = false;

       emit signal_adjustView(viewScaleForExtendedScale);

       //@LP commented to try to avoid little line width blinking due to coming soon width adjustement later
       //--*- adjust item width
       //if (_ptrGraphicsItemsContainer) {
       //    _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(/*viewScaleForExtendedScale*/_ptrZoomHandler->getCurrent_scale_forViewSide());
       //}//-*--

    } else {

        qDebug() << __FUNCTION__ << "zoomLevelImageTransition is none: "
              << " globalZF = "   << _ptrZoomHandler->getCurrent_globalZoomFactor() << " , "
              << " ZLI = "        << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() << " , "
              << " Scale = "      << _ptrZoomHandler->getCurrent_scale_forViewSide() << " , "
              << " noTransition";

        _bRefresh = true;

        qDebug() << __FUNCTION__ << "emit now signal_adjustView";
        emit signal_adjustView(_ptrZoomHandler->getCurrent_scale_forViewSide());

        //--*- adjust item width
        if (_ptrGraphicsItemsContainer) {
            _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(_ptrZoomHandler->getCurrent_scale_forViewSide());
        }//-*--

        //----
        //retake current center which should be reset to center of the overwhall instead off a small part of graphic item

        currentCenter = _qpfCurrentCenter; //_currentExposedRectF_forCenterOn.center();

        centerxMulZLI = currentCenter.x()*_current_zoomLevelImage_forSceneSide;
        centeryMulZLI = currentCenter.y()*_current_zoomLevelImage_forSceneSide;

        sceneCenterPosF_noZLI_beforeZoom_percent.setX(centerxMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
        sceneCenterPosF_noZLI_beforeZoom_percent.setY(centeryMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

        qDebug() << __FUNCTION__ << "retaken currentCentern = " << currentCenter;

        //will be used below for setcenter
        //----
    }

    //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
    qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;
    qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;

    qDebug() << __FUNCTION__ << "computed new hwFullImage to compute qpfCenterOn= ";
    qDebug() << __FUNCTION__ << "wFullImageAtZLI = " << wFullImageAtZLI;
    qDebug() << __FUNCTION__ << "hFullImageAtZLI = " << hFullImageAtZLI;

    QPointF qpfCenterOn {.0,.0};

    qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
    qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

    qDebug() << __FUNCTION__ << "qpfCenterOn = " << qpfCenterOn;

    _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;
    qDebug() << __FUNCTION__ << "dbg signal_setCenterOn @line "<< __LINE__;
    emit signal_setCenterOn( qpfCenterOn.x(),qpfCenterOn.y());

    bool bGlobalZoomFactorIs1 = (qAbs((_ptrZoomHandler->getCurrent_globalZoomFactor() - 1.0)) < 0.01);
    qDebug() << __FUNCTION__ << "bGlobalZoomFactorIs1 ="<< bGlobalZoomFactorIs1;
    emit signal_setEnable_setGlobalZoomFactorTo1(!bGlobalZoomFactorIs1);

}

void CustomGraphicsScene::slot_visibleAreaChanged(QRectF visibleArea) { //to inform small image nav
    if (_currentIndexOfUsedTileProvider == -1) {
        return;
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        qDebug() << __FUNCTION__ << " error: _tileProvider_wDO is nullptr";
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) visibleArea = " << visibleArea;
    //qDebug() << __FUNCTION__ << "(void*)_tileProvider3 = " <<  (void*)_tileProvider3;
    emit signal_currentExposedRectFChanged(visibleArea, _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->get_currentZoomLevelImg());
}

void CustomGraphicsScene::drawBackground(QPainter *painter, const QRectF &exposed) {


    //---
    _bOneActionInProgressIsWaitingDrawingUpdate = false;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- false";
    //---

    qDebug() << __FUNCTION__ << "--\\/---------------------------------------";
    qDebug() << __FUNCTION__ << "exposed =                                          :" << exposed;
    qDebug() << __FUNCTION__ << "topLeft()     = " << exposed.topLeft();
    qDebug() << __FUNCTION__ << "bottomRight() = " << exposed.bottomRight();
    qDebug() << __FUNCTION__ << "--/\\---------------------------------------";


    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_current_zoomLevelImage_forSceneSide" << _current_zoomLevelImage_forSceneSide;
    qDebug() << __FUNCTION__ << "_bTilesLoadingRequest_dueToZoom"      << _bTilesLoadingRequest_dueToZoom;
    qDebug() << __FUNCTION__ << "_bTilesLoading_dueToZoom"  << _bTilesLoading_dueToZoom;
    qDebug() << __FUNCTION__ << "_bSwitchZLIAfterEndOfLoad" << _bSwitchZLIAfterEndOfLoad;
    qDebug() << __FUNCTION__ << "_bRefresh" << _bRefresh;
    qDebug() << __FUNCTION__ << "-----------------------------------------";

    _currentExposedRectF = exposed;


    if (_bRefresh) {
        _qpfCurrentCenter = _currentExposedRectF.center();
        qDebug() << __FUNCTION__ << "_qpfCurrentCenter set to: " << _qpfCurrentCenter;
    }

    //qDebug() << __FUNCTION__ << "(common) _currentExposedRectF <= " << _currentExposedRectF;
    //qDebug() << __FUNCTION__ << "(common) _currentExposedRectF_forCenterOn <= " << _currentExposedRectF_forCenterOn;

    if (_currentIndexOfUsedTileProvider == -1) {
        return;
    }
    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        return;
    }

    bool bReleaseAndLoadTileForTheCurrentZLI = true;

    if (_bSwitchZLIAfterEndOfLoad) { //a switch is ready

        _bSwitchZLIAfterEndOfLoad = false;

        if (!_ZLIofTilesLoading_dueToZoom) {

            qDebug() << __FUNCTION__ << "huho _ZLIofTilesLoading_dueToZoom is zero ?!";

        } else {

            _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->releaseAndSwitchAllocatedTrackedTiles(_ZLIofTilesLoading_dueToZoom);

            _ZLIofTilesLoading_dueToZoom = 0;

            //reuse releaseAndLoadTilesToMatchCurrentVisu to create the contrast remapped image to display
            _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->releaseAndLoadTilesToMatchCurrentVisu(_currentExposedRectF);

            _bRefresh = false;

            bReleaseAndLoadTileForTheCurrentZLI = false;

            //@LP the switch the tiles is made here, the view needs also to received the according scale and center
        }

    } else { //no switch ready

        if (_bTilesLoading_dueToZoom) { //a load is in progress

            //avoid new load ZLI request

        } else { //no load in progress

            if (_bTilesLoadingRequest_dueToZoom) { //a load request is pending

                qDebug() << __FUNCTION__ << "_bTilesLoadingRequest_dueToZoom";

                //launch a ZLI load
                _bTilesLoadingRequest_dueToZoom = false;
                _bTilesLoading_dueToZoom = true;
                _ZLIofTilesLoading_dueToZoom = _qvect_ZLIToLoad.last(); //take the more recent ZLI that the user want
                _qvect_ZLIToLoad.clear();

                //---
                //_bOneActionInProgressIsWaitingDrawingUpdate = true;
                //qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";
                //---

                emit signal_enablePossibleActionsOnImageView(false);
                emit signal_setEnable_setGlobalZoomFactorTo1(false);
                //emit signal_enable_zoomStepIncDec(false, false);

                //load the tiles for this ZLI and the current exposedRectF
                QFuture<bool> futureBool = QtConcurrent::run(_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider],
                                                         &TileProvider_withDisplayOutput::loadTilesRequestToMatchCurrentVisuAndZLI,
                                                         _currentExposedRectF,
                                                         _current_zoomLevelImage_forSceneSide,
                                                         _ZLIofTilesLoading_dueToZoom);
                _watcherBool.setFuture(futureBool); //be informed when loading terminated
            }
        }
    }

    if (bReleaseAndLoadTileForTheCurrentZLI) {

        if (_bRefresh)  {
            if (_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
                _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->releaseAndLoadTilesToMatchCurrentVisu(/*_currentExposedRectF.x(), _currentExposedRectF.y()*/_currentExposedRectF);
            }
            _bRefresh = false;
        } else {
            //do nothing here
        }
    }

    qDebug() << __FUNCTION__ << "_nbTilesinH_fullImage = " << _nbTilesinH_fullImage;
    qDebug() << __FUNCTION__ << "_nbTilesinW_fullImage = " << _nbTilesinW_fullImage;

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform,false);

    qDebug() << __FUNCTION__ << " _currentIndexOfUsedTileProvider = " << _currentIndexOfUsedTileProvider;

    // Draws all tiles that intersect the exposed area.
    for (int yt = 0; yt < _nbTilesinH_fullImage; ++yt) {
       for (int xt = 0; xt < _nbTilesinW_fullImage; ++xt) {
           QRectF rect = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->rectForTile(xt, yt/*, "called by drawBackground"*/);

           if (_currentExposedRectF.intersects(rect)) {

               qDebug() << __FUNCTION__ << "(inters) _currentExposedRectF = " << _currentExposedRectF << " tileRect = " << rect  <<  "[" << xt << "," << yt << "]";


               void *ptrTileData = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->TileData(xt,yt);


               qDebug() << " " << __FUNCTION__ << "ptrTileData = " << (void*)ptrTileData;

               if (ptrTileData == nullptr) {

                   qDebug() << " " << __FUNCTION__ << "tile not available !? (tile[" << xt << ", " << yt;

               } else {

                   //qDebug() << "tw(xt) = " << _tileProvider3->TileW(xt);
                   //qDebug() << "th(yt) = " << _tileProvider3->TileH(yt);

                   /*
                   qDebug() << " " << __FUNCTION__ << "build QImage for painter->drawPixmap(" << rect.topLeft()
                            << " using painter using data at @" << (void*)ptrTileData << " (tile[" << xt << ", " << yt;
                   */


                   qDebug() << " " << __FUNCTION__ << "build QImage for painter->drawPixmap(" << rect.topLeft()
                            << " using painter using data at @" <<(void*)ptrTileData
                            << " (tile[" << xt << ", " << yt;

                   int tileW_ofxyt = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->tileW(xt);
                   int tileH_ofxyt = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->tileH(yt);

                   QImage::Format qImageFormat = _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->qDisplayImageFormat();

                   //qDebug() << __FUNCTION__ << "tileW_ofxyt = " << tileW_ofxyt << ",  tileH_ofxyt = " << tileH_ofxyt;

                   //method #1

                   //method #1 with just one 'new', hence missing a delete ?
                   QImage *tileImg = new QImage(static_cast<const uchar*>(ptrTileData),
                                              tileW_ofxyt,
                                              tileH_ofxyt,

                                              tileW_ofxyt, //nb bytes per line; very important field to use to have correct display of last rows and last columns, not power of 8 or something like that
                                              qImageFormat);

                   painter->drawPixmap(rect.topLeft(), QPixmap::fromImage(*tileImg));

                   delete tileImg;
                   tileImg = nullptr;

                }
           } else {
               //qDebug() << __FUNCTION__ << "(notint) _currentExposedRectF = " << _currentExposedRectF << " tileRect = " << rect <<  "[" << xt << "," << yt << "]";
           }
       }
    }

    painter->restore();

}

void CustomGraphicsScene::slot_updateTiles() {
    qDebug() << __FUNCTION__ ;

    //---
    _bOneActionInProgressIsWaitingDrawingUpdate = true;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";
    //---

    _bRefresh = true;
    qDebug() << __FUNCTION__ << "call now update()";
    //update(); //@#LP update() works well but use invalidate instead
    invalidate();
}

bool CustomGraphicsScene::tileProviderForIndexLayerIsLoaded(int ieLA) {
    if ((ieLA < eLA_PX1)||(ieLA > eLA_deltaZ)) {
        return(false);
    }
    return(_qvectPtrTileProvider_wDO[ieLA] != nullptr);
}

void CustomGraphicsScene::switchToLayer(int ieLA) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) ieLA received: " << ieLA;

    if (ieLA == _currentIndexOfUsedTileProvider) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsScene) not a switch (_currentIndexOfUsedTileProvider = " << _currentIndexOfUsedTileProvider;
        return;
    }
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _currentIndexOfUsedTileProvider: " << _currentIndexOfUsedTileProvider;

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) @=" << _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider];
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _qvectPtrTileProvider_wDO[ieLA]) @=" << _qvectPtrTileProvider_wDO[ieLA];

    //when switching layer, the state of the layer to swith on can be, at this step, very unsync comparing to the the layer the user leave now
    //allocated tiles of the layer to swith on can be on a different ZLI or the same, at a totally different location, overlapping or matching

    int currentZoomLevelImg = _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide(); //_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->get_currentZoomLevelImg();
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) currentZoomLevelImg got from _currentIndexOfUsedTileProvider (="
             << _currentIndexOfUsedTileProvider
             << ") = " << currentZoomLevelImg;

    if (_qvectPtrTileProvider_wDO[ieLA]) {
         qDebug() << __FUNCTION__ << "(CustomGraphicsScene) if (_qvectPtrTileProvider_wDO[ieLA]) {" << " [ ieLA = " << ieLA;
        _qvectPtrTileProvider_wDO[ieLA]->set_currentZoomLevelImg(currentZoomLevelImg);
    } else {
        qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _qvectPtrTileProvider_wDO[ieLA] is nullptr" << " [ ieLA = " << ieLA;
    }

    _currentIndexOfUsedTileProvider = ieLA;

    slot_updateTiles();
}

void CustomGraphicsScene::load_ended() {

    if (!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]) {
        emit signal_enablePossibleActionsOnImageView(true);
        emit signal_setEnable_setGlobalZoomFactorTo1(true);
        _bOneActionInProgressIsWaitingDrawingUpdate = false;
        qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- false";
        return;
    }

    qDebug() << __FUNCTION__ << "ready to display ZLI " << _ZLIofTilesLoading_dueToZoom;
    qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() =" << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
    qDebug() << __FUNCTION__ << "_current_zoomLevelImage_forSceneSide =" << _current_zoomLevelImage_forSceneSide;

    qreal factor_newZLIDividedByCurrent = static_cast<qreal>(_ZLIofTilesLoading_dueToZoom)/static_cast<qreal>(_current_zoomLevelImage_forSceneSide);

    QPointF currentCenter = _qpfCurrentCenter; //_currentExposedRectF_forCenterOn.center(); //refers to _current_zoomLevelImage_forSceneSide
    qDebug() << __FUNCTION__ << "currentCenter:" << currentCenter;

    //use case example:
    // ZLI 2  centerx = 250   => centerxMulZLI = 250 * 2 = 500
    // wimageNoZoom = 1000
    // sceneCenterPosF_noZLI_beforeZoom.x = 500 / 1000 = 0.5

    qreal centerxMulZLI = currentCenter.x()*_current_zoomLevelImage_forSceneSide;
    qreal centeryMulZLI = currentCenter.y()*_current_zoomLevelImage_forSceneSide;

    qDebug() << __FUNCTION__ << "centerxMulZLI = " << centerxMulZLI;
    qDebug() << __FUNCTION__ << "centeryMulZLI = " << centeryMulZLI;

    QSize qsizeImageNoZoom {0, 0};
    _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(qsizeImageNoZoom);
    qDebug() << __FUNCTION__ << "qsizeImageNoZoom = " << qsizeImageNoZoom;

    QPointF sceneCenterPosF_noZLI_beforeZoom_percent {.0,.0};
    sceneCenterPosF_noZLI_beforeZoom_percent.setX(centerxMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
    sceneCenterPosF_noZLI_beforeZoom_percent.setY(centeryMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

    qDebug() << __FUNCTION__ << "sceneCenterPosF_noZLI_beforeZoom_percent = " << sceneCenterPosF_noZLI_beforeZoom_percent;

    _bTilesLoading_dueToZoom = false;

    //just loaded ZLI matches with the simulated zoom
    if (_ZLIofTilesLoading_dueToZoom == _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide()) {

        //and matches also with the current ZLI
        if (_ZLIofTilesLoading_dueToZoom == _current_zoomLevelImage_forSceneSide) {

            //no needs to switch
            _bSwitchZLIAfterEndOfLoad = false;

            _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->releaseOldZLIIfNotEqualTo(_ZLIofTilesLoading_dueToZoom);

            _bRefresh = true;

            emit signal_adjustView(_ptrZoomHandler->getCurrent_scale_forViewSide());

            //--*- adjust item width
            if (_ptrGraphicsItemsContainer) {
                _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(_ptrZoomHandler->getCurrent_scale_forViewSide());
            }//-*--

            //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
            //this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
            qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;//_tileProvider3->fullImageW();
            qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;//_tileProvider3->fullImageH();

            QPointF qpfCenterOn {.0,.0};
            qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
            qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

            qDebug() << __FUNCTION__ << "Z=P=C dbg signal_setCenterOn" << "qpfCenterOn = " << qpfCenterOn;
            qDebug() << __FUNCTION__ << "Z=P=C dbg _current_zoomLevelImage_forSceneSide " << _current_zoomLevelImage_forSceneSide;
            qDebug() << __FUNCTION__ << "Z=P=C dbg _ZLIofTilesLoading_dueToZoom " << _ZLIofTilesLoading_dueToZoom;
            qDebug() << __FUNCTION__ << "Z=P=C dbg wFullImageAtZLI " << wFullImageAtZLI;
            qDebug() << __FUNCTION__ << "Z=P=C dbg hFullImageAtZLI " << hFullImageAtZLI;

            _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;
            qDebug() << __FUNCTION__ << __LINE__ << " emit signal_setCenterOn() with qpfCenterOn = " << qpfCenterOn;

            emit signal_setCenterOn(qpfCenterOn.x(),qpfCenterOn.y());

        } else { //the just loaded ZLI matches with the wanted ZLI zoom (simulated by the scale using the _current_zoomLevelImage_forSceneSide, which is different)

            //take into account the newly loaded ZLI
            _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->set_currentZoomLevelImg(_ZLIofTilesLoading_dueToZoom);

            qDebug() << __FUNCTION__ << "before: _current_zoomLevelImage_forSceneSide=: " << _current_zoomLevelImage_forSceneSide;

            //change the scene rect to match with the newly loaded ZLI
            setSceneRectForCurrentZoomLevel(); //_current_zoomLevelImage_forSceneSide set in this method

            qDebug() << __FUNCTION__ << "after: _current_zoomLevelImage_forSceneSide=: " << _current_zoomLevelImage_forSceneSide;
            qDebug() << __FUNCTION__ << "should be equal to _ZLIofTilesLoading_dueToZoom : " << _ZLIofTilesLoading_dueToZoom;

            _bSwitchZLIAfterEndOfLoad = true;


            //@#LP udpate center to make happy the next zoomrequest. Because drawbackground could not be called before this next zoomrequest (hence _qpfCurrentCenter not updated by drawbackground)
            _qpfCurrentCenter = currentCenter / factor_newZLIDividedByCurrent;
            emit signal_adjustView(_ptrZoomHandler->getCurrent_scale_forViewSide());

            //--*- adjust item width
            if (_ptrGraphicsItemsContainer) {
                _ptrGraphicsItemsContainer->adjustGraphicsItemWidth(_ptrZoomHandler->getCurrent_scale_forViewSide());
            }//-*--

            //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
            //this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
            qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_ZLIofTilesLoading_dueToZoom;  //_current_zoomLevelImage_forSceneSide;
            qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_ZLIofTilesLoading_dueToZoom;  //_current_zoomLevelImage_forSceneSide;

            QPointF qpfCenterOn {.0,.0};
            qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*wFullImageAtZLI);
            qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*hFullImageAtZLI);

            qDebug() << __FUNCTION__ << "Z=P!=C dbg signal_setCenterOn" << "qpfCenterOn = " << qpfCenterOn;
            qDebug() << __FUNCTION__ << "Z=P!=C dbg _current_zoomLevelImage_forSceneSide " << _current_zoomLevelImage_forSceneSide;
            qDebug() << __FUNCTION__ << "Z=P!=C dbg _ZLIofTilesLoading_dueToZoom " << _ZLIofTilesLoading_dueToZoom;
            qDebug() << __FUNCTION__ << "Z=P!=C dbg wFullImageAtZLI " << wFullImageAtZLI;
            qDebug() << __FUNCTION__ << "Z=P!=C dbg hFullImageAtZLI " << hFullImageAtZLI;

            _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;

            qDebug() << __FUNCTION__ << __LINE__ << " emit signal_setCenterOn() with qpfCenterOn = " << qpfCenterOn;

            emit signal_setCenterOn(qpfCenterOn.x(),qpfCenterOn.y());

        }

    } else { //just loaded ZLI doest not match with the simulated zoom

        qDebug() << __FUNCTION__ << "Z!=P dbg";

        _bSwitchZLIAfterEndOfLoad = false;
        //release _ZLIofTilesLoading_dueToZoom, the user do not want it
        _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->releaseZLI(_ZLIofTilesLoading_dueToZoom);

        //loaded for nothing, probably that the user had made for example zoomIn/zoomOut pretty fast

        qDebug() << __FUNCTION__ << "-1-";

        //launch the load of the following ZLI which should be in the LIFO
        if (_qvect_ZLIToLoad.count()) {

            //we can launch a load (this will take the last pushed ZLI value in the _qvect_ZLIToLoad ('LIFO'))
            _bTilesLoadingRequest_dueToZoom = true;

            qDebug() << __FUNCTION__ << "- mandatory drawbackground needs to launch the new ZLI load pending ";

            _qpfCenterOn_current_aboutCurrentZLI = _qpfCurrentCenter;

            qDebug() << __FUNCTION__ << __LINE__ << " emit signal_setCenterOn() with _qpfCurrentCenter = " << _qpfCurrentCenter;

            emit signal_setCenterOn(_qpfCurrentCenter.x(), _qpfCurrentCenter.y()); //try force drawbackground

            qDebug() << __FUNCTION__ << "call now invalidate()";
            invalidate(); //to get the exposed rect and launch the loading for the ZLI we want

        } else {

            qDebug() << __FUNCTION__ << "huho, no ZLI load pending ?"; //@#LP

        }

        qDebug() << __FUNCTION__ << "-2-";

    }
    emit signal_enablePossibleActionsOnImageView(true);

    //emit signal_enable_zoomStepIncDec(true, true);

    qDebug() << __FUNCTION__ << "_current_zoomLevelImage_forSceneSide = " << _current_zoomLevelImage_forSceneSide;
    qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_scale_forViewSide() = " << _ptrZoomHandler->getCurrent_scale_forViewSide();
    qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_globalZoomFactor() = " << _ptrZoomHandler->getCurrent_globalZoomFactor();

    bool bGlobalZoomFactorIs1 = (qAbs((_ptrZoomHandler->getCurrent_globalZoomFactor() - 1.0)) < 0.01);
    qDebug() << __FUNCTION__ << "bGlobalZoomFactorIs1 ="<< bGlobalZoomFactorIs1;
    emit signal_setEnable_setGlobalZoomFactorTo1(!bGlobalZoomFactorIs1);

    _bOneActionInProgressIsWaitingDrawingUpdate = false;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- false";

}

bool CustomGraphicsScene::setZoomStepIncDec(bool bStepIsIncrease) {

     if (_currentIndexOfUsedTileProvider == -1) {
          return(false);
     }
     if ((!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider])||(!_ptrZoomHandler)) {
         return(false);
     }

     if (bStepIsIncrease) {
         slot_zoomRequest(ZoomHandler::e_ZoomDir_In);
     } else {
         slot_zoomRequest(ZoomHandler::e_ZoomDir_Out);
     }
     return(true);
}

bool CustomGraphicsScene::setGlobalZoomFactorTo1() {

    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        return(false);
    }

    qDebug() << __FUNCTION__ << "(qsizeImageNoZoom)";

    if (_currentIndexOfUsedTileProvider == -1) {
          return(false);
     }
     if ((!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider])||(!_ptrZoomHandler)) {
         return(false);
     }

     if (!_current_zoomLevelImage_forSceneSide) {
         return(false);
     }


    //---
    _bOneActionInProgressIsWaitingDrawingUpdate = true;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";
    //---


     QSize qsizeImageNoZoom {0, 0};
     _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->getImageSizeNoZoom(qsizeImageNoZoom);
     qDebug() << __FUNCTION__ << "qsizeImageNoZoom =" << qsizeImageNoZoom;

     //@LP be aware that this value can be bigger than the real image no ZLI due to div2/4/8/... accuracy loss
     qreal wFullImageAtZLI = qsizeImageNoZoom.width()/_current_zoomLevelImage_forSceneSide;
     qreal hFullImageAtZLI = qsizeImageNoZoom.height()/_current_zoomLevelImage_forSceneSide;


     qDebug() << __FUNCTION__ << "computed new hwFullImage to compute qpfCenterOn= ";
     qDebug() << __FUNCTION__ << "wFullImageAtZLI = " << wFullImageAtZLI;
     qDebug() << __FUNCTION__ << "hFullImageAtZLI = " << hFullImageAtZLI;


     QPointF qpfCenterOn {.0,.0};

     QPointF sceneCenterPosF_noZLI_beforeZoom_percent {.0,.0};
     QPointF currentCenter = _qpfCurrentCenter; //_currentExposedRectF_forCenterOn.center();

     qreal centerxMulZLI = .0;
     qreal centeryMulZLI = .0;

     centerxMulZLI = currentCenter.x()*_current_zoomLevelImage_forSceneSide;
     centeryMulZLI = currentCenter.y()*_current_zoomLevelImage_forSceneSide;

     qDebug() << __FUNCTION__ << "centerxMulZLI =" << centerxMulZLI;
     qDebug() << __FUNCTION__ << "centeryMulZLI =" << centeryMulZLI;

     sceneCenterPosF_noZLI_beforeZoom_percent.setX(centerxMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
     sceneCenterPosF_noZLI_beforeZoom_percent.setY(centeryMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

     int indexOfZoomValue = _ptrZoomHandler->findClosestZoomIndexOfZoomValueForGlobalZoomFactor(1.0);

     _ptrZoomHandler->setToZoomFromIdx(indexOfZoomValue);

     qDebug() << __FUNCTION__ << "indexOfZoomValue = " << indexOfZoomValue;
     qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = " << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();

     //select here the zoom level and location of what we want to see on screen
     _qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider]->selectZoomLevelImgAndLoadFullImageToDisplayInSize(
                 /*zoomParameters._zoomLevelImage_forSceneSide*/
                 _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

     qDebug() << __FUNCTION__ << "call now setSceneRectForCurrentZoomLevel @line "<< __LINE__;
     setSceneRectForCurrentZoomLevel();

     //add_debugItems();

     update_and_sendScaleFactorToView(); // slot_zoomChanged(); //to update and send to the view the scaleFactor

     qDebug() << __FUNCTION__ << "sceneCenterPosF_noZLI_beforeZoom_percent =" << sceneCenterPosF_noZLI_beforeZoom_percent;

     qpfCenterOn.setX(sceneCenterPosF_noZLI_beforeZoom_percent.x()*qsizeImageNoZoom.width());
     qpfCenterOn.setY(sceneCenterPosF_noZLI_beforeZoom_percent.y()*qsizeImageNoZoom.height());

     qDebug() << __FUNCTION__ << "qpfCenterOn computed using sceneCenterPosF_noZLI_beforeZoom_percent: " << qpfCenterOn;

     _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;

     _bRefresh = true; //mandatory ?

     qDebug() << __FUNCTION__ << "emit now signal_setCenterOn @line "<< __LINE__;

     emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());

     emit signal_setEnable_setGlobalZoomFactorTo1(false);

     return(true);
}

#include <QCursor>
#include <QWidget>
#include <QGraphicsView>

void CustomGraphicsScene::centerOnMouseCursorLocation() {

    if (!_ptrZoomHandler) {
        return;
    }

    if (_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() == 1) {

        qDebug() << __FUNCTION__ << "" << "_ptrZoomHandler->getCurrent_scale_forViewSide() = " << _ptrZoomHandler->getCurrent_scale_forViewSide();

        QPointF qpfCenterOnCursorZLI_fromNoZLIDivZLI = _sSceneMousePosF_noZLI_current._qpf / static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
        qDebug() << __FUNCTION__ << "" << "qpfCenterOnCursorZLI_fromNoZLIDivZLI = " << qpfCenterOnCursorZLI_fromNoZLIDivZLI;

        qreal xAbsDelta_center_to_cursor_fNZLIDivZLI = qAbs(_qpfCenterOn_current_aboutCurrentZLI.x() - qpfCenterOnCursorZLI_fromNoZLIDivZLI.x());
        qreal yAbsDelta_center_to_cursor_fNZLIDivZLI = qAbs(_qpfCenterOn_current_aboutCurrentZLI.y() - qpfCenterOnCursorZLI_fromNoZLIDivZLI.y());

        qDebug() << __FUNCTION__ << "" << "xAbsDelta_center_to_cursor_fNZLIDivZLI = " << xAbsDelta_center_to_cursor_fNZLIDivZLI;
        qDebug() << __FUNCTION__ << "" << "yAbsDelta_center_to_cursor_fNZLIDivZLI = " << yAbsDelta_center_to_cursor_fNZLIDivZLI;

        qreal oneDivCurrent_scale_forViewSide = 1.0/_ptrZoomHandler->getCurrent_scale_forViewSide();
        qDebug() << __FUNCTION__ << "" << "oneDivCurrent_scale_forViewSide = " << oneDivCurrent_scale_forViewSide;

        qreal frontier = oneDivCurrent_scale_forViewSide + (oneDivCurrent_scale_forViewSide/_ptrZoomHandler->getCurrent_scale_forViewSide()); //add a little bit more than just oneDivCurrent_scale_forViewSide
        qDebug() << __FUNCTION__ << "" << "frontier = " << frontier;
        if (xAbsDelta_center_to_cursor_fNZLIDivZLI <= frontier) {
            if (yAbsDelta_center_to_cursor_fNZLIDivZLI <= frontier) {
                qDebug() << __FUNCTION__ << "" << "mouse loc too close to current center";
                return;
            }
        }

    } else {

        QPointF qpfCenterOnCursorZLI_fromNoZLIDivZLI = _sSceneMousePosF_noZLI_current._qpf / static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());
        QPointF qpfCenterOnCursorZLI = _sceneMousePosF_aboutCurrentZLI_current._qpf;

        qDebug() << __FUNCTION__ << "" << "qpfCenterOnCursorZLI_fromNoZLIDivZLI = " << qpfCenterOnCursorZLI_fromNoZLIDivZLI;
        qDebug() << __FUNCTION__ << "" << "qpfCenterOnCursorZLI = " << qpfCenterOnCursorZLI;
        qDebug() << __FUNCTION__ << "" << "_qpfCenterOn_current_aboutCurrentZLI = " << _qpfCenterOn_current_aboutCurrentZLI;
        qDebug() << __FUNCTION__ << "" << "_ptrZoomHandler->getCurrent_scale_forViewSide() = " << _ptrZoomHandler->getCurrent_scale_forViewSide();

        ////QPointF pointF_center_to_cursor = _qpfCenterOn_current_aboutCurrentZLI - qpfCenterOnCursor;
        //{
            qreal xAbsDelta_center_to_cursor_fNZLIDivZLI = qAbs(_qpfCenterOn_current_aboutCurrentZLI.x() - qpfCenterOnCursorZLI_fromNoZLIDivZLI.x());
            qreal yAbsDelta_center_to_cursor_fNZLIDivZLI = qAbs(_qpfCenterOn_current_aboutCurrentZLI.y() - qpfCenterOnCursorZLI_fromNoZLIDivZLI.y());

            qreal oneDivCurrent_scale_forViewSide = 1.0/_ptrZoomHandler->getCurrent_scale_forViewSide();
            qDebug() << __FUNCTION__ << "" << "oneDivCurrent_scale_forViewSide = " << oneDivCurrent_scale_forViewSide;

            qDebug() << __FUNCTION__ << "" << "xAbsDelta_center_to_cursor_fNZLIDivZLI = " << xAbsDelta_center_to_cursor_fNZLIDivZLI;
            qDebug() << __FUNCTION__ << "" << "yAbsDelta_center_to_cursor_fNZLIDivZLI = " << yAbsDelta_center_to_cursor_fNZLIDivZLI;

            qreal frontier = oneDivCurrent_scale_forViewSide + 1.0/static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide()); //add a little bit more than just oneDivCurrent_scale_forViewSide
            qDebug() << __FUNCTION__ << "" << "frontier = " << frontier;
            if (xAbsDelta_center_to_cursor_fNZLIDivZLI <= frontier) {
                if (yAbsDelta_center_to_cursor_fNZLIDivZLI <= frontier) {
                    qDebug() << __FUNCTION__ << "" << "mouse loc too close to current center";
                    return;
                }
            }
        //}
    }

    //qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = " << _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
    //qDebug() << __FUNCTION__ << "_ptrZoomHandler->getCurrent_scale_forViewSide() = " << _ptrZoomHandler->getCurrent_scale_forViewSide();

    setSceneCenterNoZLI(_sSceneMousePosF_noZLI_current._qpf, true);

#ifdef false  //merged code

    qDebug() << __FUNCTION__ << "static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = "
             << static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

    QPointF qpfCenterOn = _sSceneMousePosF_noZLI_current._qpf / static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

    _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;

    qDebug() << __FUNCTION__ << "qpfCenterOn = " << qpfCenterOn;

    _bRefresh = true;

    emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());

    QList<QGraphicsView *> qlistGView = QGraphicsScene::views();
    if (!qlistGView.count()) {
        return;
    }

    QPoint qp_sceneToView = qlistGView[0]->mapFromScene(qpfCenterOn);
    QPoint qp_sceneToGlobal = qlistGView[0]->mapToGlobal(qp_sceneToView);

    qDebug() << __FUNCTION__ << "qp_sceneToView   = " << qp_sceneToView;
    qDebug() << __FUNCTION__ << "qp_sceneToGlobal = " << qp_sceneToGlobal;

    QCursor QCursorTocenterMouse;
    QCursorTocenterMouse.setPos(qp_sceneToGlobal);

    qDebug() << __FUNCTION__ << "call qlistGView[0]->setCursor(centerMouseQCursor)";
    qlistGView[0]->setCursor(QCursorTocenterMouse);
    qDebug() << __FUNCTION__ << "qlistGView[0]->setCursor(centerMouseQCursor) called";

    slot_updateTiles();
#endif

}

void CustomGraphicsScene::fitImageInCurrentWindowSize() {

    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    if (_currentIndexOfUsedTileProvider == -1) {
         return;
    }
    if ((!_qvectPtrTileProvider_wDO[_currentIndexOfUsedTileProvider])||(!_ptrZoomHandler)) {
        return;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) will emit";
    emit signal_callSceneToFitImageInWindowView();

    bool bGlobalZoomFactorIs1 = (qAbs((_ptrZoomHandler->getCurrent_globalZoomFactor() - 1.0)) < 0.01);
    qDebug() << __FUNCTION__ << "bGlobalZoomFactorIs1 ="<< bGlobalZoomFactorIs1;
    emit signal_setEnable_setGlobalZoomFactorTo1(!bGlobalZoomFactorIs1);
}

void CustomGraphicsScene::slot_setSceneCenterNoZLI_fromOutside(QPointF qpfMouseLocationInSceneNoZLI) { //from small image nav

    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        return;
    }
    /*//reject conditions
    if (  (_bTilesLoading_dueToZoom)
        ||(_bSwitchZLIAfterEndOfLoad)
        ||(_bTilesLoadingRequest_dueToZoom))  {
        return;
    }*/

    setSceneCenterNoZLI(qpfMouseLocationInSceneNoZLI, false);
}

void CustomGraphicsScene::setSceneCenterNoZLI(QPointF qpfCenterLcationInSceneNoZLI, bool bMoveMouseCursorToSceneCenter) {

    qDebug() << __FUNCTION__ << "static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide() = "
             << static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

    QPointF qpfCenterOn = qpfCenterLcationInSceneNoZLI / static_cast<qreal>(_ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide());

    _qpfCenterOn_current_aboutCurrentZLI = qpfCenterOn;

    qDebug() << __FUNCTION__ << "qpfCenterOn = " << qpfCenterOn;

    _bRefresh = true;

    //---
    _bOneActionInProgressIsWaitingDrawingUpdate = true;
    qDebug() << __FUNCTION__ << "_bOneActionInProgressIsWaitingDrawingUpdate <- true";
    //---

    emit signal_setCenterOn(qpfCenterOn.x(), qpfCenterOn.y());

    QList<QGraphicsView *> qlistGView = QGraphicsScene::views();
    if (!qlistGView.count()) {
        return;
    }

    QPoint qp_sceneToView = qlistGView[0]->mapFromScene(qpfCenterOn);
    QPoint qp_sceneToGlobal = qlistGView[0]->mapToGlobal(qp_sceneToView);

    qDebug() << __FUNCTION__ << "qp_sceneToView   = " << qp_sceneToView;
    qDebug() << __FUNCTION__ << "qp_sceneToGlobal = " << qp_sceneToGlobal;

    if (bMoveMouseCursorToSceneCenter) {
        QCursor QCursorTocenterMouse;
        QCursorTocenterMouse.setPos(qp_sceneToGlobal);

        qDebug() << __FUNCTION__ << "call qlistGView[0]->setCursor(centerMouseQCursor)";
        qlistGView[0]->setCursor(QCursorTocenterMouse);
        qDebug() << __FUNCTION__ << "qlistGView[0]->setCursor(centerMouseQCursor) called";
    }

    slot_updateTiles();

}

//void SmallImageNav_CustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
void CustomGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";
    if (!loadingContextPermitsOperationAndNoNullPtr()) {
        event->accept();
        return;
    }

    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene)";

    bool bLeftButtonPressed = event->button() == Qt::LeftButton;
    if (bLeftButtonPressed) {

        //double click left mouse button

        //avoid center if trying to add a point (adding point to quick leading to a double click)
        if (_eRouteEditionState == e_rES_inEdition) {
            qDebug() << __FUNCTION__ << "(CustomGraphicsScene) _eRouteEditionState is e_rES_inEdition";
            S_booleanKeyStatus SbkeyStatus;
            bool bAddPointToRoute = bLeftButtonPressed && SbkeyStatus._bShiftKeyAlone;
            if (bAddPointToRoute) { //but in adding point mode

                /*qDebug() << __FUNCTION__ << "(CustomGraphicsScene) bAddPointToRoute is true";

                QPointF scenePosNoZLI = event->scenePos() * _ptrZoomHandler->getCurrent_zoomLevelImage_forSceneSide();
                emit signal_addPointToSelectedRoute(scenePosNoZLI);*/
                event->accept();
                return;
            }
        }

        //double click and not in adding point mode
        centerOnMouseCursorLocation();
        event->accept();

    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

//signal from smallnav scene:
void CustomGraphicsScene::slot_requestFromOutside_setGlobalZoomFactorTo1() {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    setGlobalZoomFactorTo1();
}

void CustomGraphicsScene::slot_requestFromOutside_fitImageInCurrentWindowSize() {    
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene)";

    fitImageInCurrentWindowSize();    
}

void CustomGraphicsScene::slot_requestFromOutside_setZoomStepIncDec(bool bsetOnStepInc) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsScene) bsetOnStepInc = " << bsetOnStepInc;

    /*if (!loadingContextPermitsOperationAndNoNullPtr()) { //
        return;
    }*/
    setZoomStepIncDec(bsetOnStepInc);
}

bool CustomGraphicsScene::loadingContextPermitsOperationAndNoNullPtr() {

    if (_bOneActionInProgressIsWaitingDrawingUpdate) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsScene): if (_bOneActionInProgressIsWaitingDrawingUpdate) {";
                return(false);
    }

    if (  (_bTilesLoading_dueToZoom)
        ||(_bSwitchZLIAfterEndOfLoad)
        ||(_bTilesLoadingRequest_dueToZoom))  {
        return(false);
    }

    if (!_ptrGraphicsItemsContainer) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsScene): if (!_ptrGraphicsItemsContainer) {";
        return(false);
    }

    if (!_ptrZoomHandler) {
        return(false);
    }

    return(true);
}

void CustomGraphicsScene::routeSet_route_showAlone(int routeId, bool bShowAlone) {

    if (!_ptrGraphicsItemsContainer) {
        return;
    }

    _ptrGraphicsItemsContainer->routeSet_route_showAlone(routeId, bShowAlone);

}


void CustomGraphicsScene::routeSet_route_showAlone(int routeId) {

    if (!_ptrGraphicsItemsContainer) {
        return;
    }
    _ptrGraphicsItemsContainer->routeSet_route_showAlone(routeId);
}

