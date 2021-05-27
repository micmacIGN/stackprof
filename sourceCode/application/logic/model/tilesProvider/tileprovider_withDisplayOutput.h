#ifndef TILE_PROVIDER_WITH_DISPLAYOUTPUT_H
#define TILE_PROVIDER_WITH_DISPLAYOUTPUT_H

#include <QHash>

#include <QImage>

#include <vector>
using namespace std;

#include "tileprovider_3.h"

class TileProvider_withDisplayOutput : public TileProvider3 {

public:
    TileProvider_withDisplayOutput(const string& strBaseInputImageFilename,
                                   const string& strRootOfImageZLIStorage,
                                   int oiio_cacheAttribute_maxMemoryMB);

    ~TileProvider_withDisplayOutput() override;

    bool prepareTilesSetEngine() override;
    bool releaseAndLoadTilesToMatchCurrentVisu(const QRectF &currentVisu) override;

    bool loadTilesRequestToMatchCurrentVisuAndZLI(const QRectF &currentExposedRectF,
                                                  int current_zoomLevelImage_OfCurrentExposedRectF,
                                                  int zoomLevelImageToLoad) /*override*/;

    void *TileData(int tileX,int tileY) override;

    QImage::Format qDisplayImageFormat();

    void addZLIInLoadingQueue(int ZLI);

    bool releaseAndSwitchAllocatedTrackedTiles(int newZLI);
    bool releaseOldZLIIfNotEqualTo(int currentZLI);
    bool releaseZLI(int ZLI);
private:

    bool allocateTile(uint tileX, uint tileY); //use _currentZoomLevelImg (inherited from TileProvider3)
    bool allocateTile(uint tileX, uint tileY, int zoomLevelImg);

    bool releaseTile(int tileX, int tileY, int zoomLevelImg, const QString& strDebug);

    bool initTilesContainerForDisplay();

    QHash<uint, vvVectType> _qHashIntVvVectType_TilesForDisplayByzoomLevelImg;
    S_nbChannelsAndTypeDesc_forOIIOUsage _aboutDisplayData;

    struct S_TileXYIntersection {
        int _yt;
        int _xt;
        QRect _qrectIntersection;
    };

    bool computePixelStats_minMax(const ImageBuf& IBuf, const ROI& roiToConsider, float& min, float& max);

};

#endif
