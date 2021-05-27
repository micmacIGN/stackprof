#ifndef SmallImageNav_customGraphicsScene_H
#define SmallImageNav_customGraphicsScene_H

#include <QGraphicsScene>

#include <vector>

#include "model/tilesProvider/tileprovider_3.h" //@#LP for S_zoomLevelImgInfos definition, move S_zoomLevelImgInfos to another file

class QMouseEvent;

class CustomGraphicsSquareItem;

class CustomGraphicsBoxItem;

class SmallImageNav_CustomGraphicsScene : public QGraphicsScene {

    Q_OBJECT

public:
    SmallImageNav_CustomGraphicsScene(QSize qsizeMax_smallImageNav, QObject *parent = nullptr);

    bool createImageBackgroundAndInit(S_zoomLevelImgInfos SZLIInfosJustAboveSmallImageNavMaxSize, QSize qsizeImageNoZoom, int idxLayer);

    bool createAdditionnalImageAndInit(const string& strImageFilename, int idxLayer);

    QSize getSizeForWindowSmallImageNav();

    void clear();

    void switchToLayer(int ieLA);

    ~SmallImageNav_CustomGraphicsScene() override;

protected:
    void drawBackground(QPainter *painter, const QRectF &exposed) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    //void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool generateImageBackground(const string& strFilename, int idxLayer);

public slots:
    void slot_currentExposedRectFChanged(QRectF exposed, int currentZoomLevelImg);

signals:
    void signal_setSceneCenterNoZLI(QPointF qpfMouseLocationInSceneNoZLI);

    //destination is the image scene (customgraphiscene_usingTPDO)
    void signal_setGlobalZoomFactorTo1();
    void signal_fitImageInCurrentWindowSize();
    void signal_setZoomStepIncDec(bool bStepIsIncrease);

private:

    QSize _qsizeMax_smallImageNav;
    QRectF _qrectfSceneImgSmallnav;

    /*
                       |
                       | line top
                       |
                    --------
    line left  -----| rect |------ line right
                    --------
                       |
                       |
                       | line bottom
    */

    QGraphicsRectItem *_qGRectItemCurrentlyVisibleInImageView;

    QGraphicsLineItem *_qGLineItemTargetingAreaVisibleInImageView[4]; //top/down/left/right

    QPen _p;

    qreal _dividerRatioFullImageToSmallNav;

    std::vector<unique_ptr<VectorType>> _stdvect_uptrVectorTypeSmallNavImageDataDisplay;
    int _currentLayerDisplayed;

    QSize _qsizeImageNoZoom;
};

#endif // SmallImageNav_customGraphicsScene_H

