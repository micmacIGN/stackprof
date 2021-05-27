#ifndef TILE_PROVIDER_3_H
#define TILE_PROVIDER_3_H

#include <vector>
using namespace std;

#include "OpenImageIO/imagebuf.h"

#include <OpenImageIO/imagecache.h>

using namespace OIIO;

#include <QHash>

#include <QRectF>
#include <QRect>
#include <QPoint>

#include <vector>
using namespace std;

class VectorType;

//moved from protected to public to be able to used it to initialiaze the smallImageNav
struct S_zoomLevelImgInfos {
    string _strFilename;
    ImageSpec _imageSpec;

    int _nbTilesinW_fullImage;
    int _nbTilesinH_fullImage;

    int _widthOfLastTilesColumn;
    int _heightOfLastTilesRow;

    void showContent();
};

class TileProvider3 {

public:

    TileProvider3(const string& strBaseInputImageFilename, const string& strRootOfImageZLIStorage, int oiio_cacheAttribute_maxMemoryMB);

    virtual ~TileProvider3();

    //@#LP: check the performance difference removing the usage of virtual (simply using different method names in the child and the base class)
    //@#LP: needs to change, separating interface and customization ( http://www.gotw.ca/publications/mill18.htm "Virtuality")
    virtual bool prepareTilesSetEngine();
    virtual bool releaseAndLoadTilesToMatchCurrentVisu(const QRectF &currentVisu);

    /*virtual*/ bool loadTilesRequestToMatchCurrentVisuAndZLI(
    const QRectF &currentExposedRectF_atZoomLevelImageToLoad, int zoomLevelImageToLoad);
    bool releaseAndSwitchAllocatedTrackedTiles();

    void releaseTileForZLI(int ZLIToRelease);

    //these methods return the base tile size w and h
    int tileW() const;
    int tileH() const;

    //these methods handle the last row and last column size
    int tileW(int xIdxTile) const;
    int tileH(int yIdxTile) const;

    quint16 fullImageW() const;
    quint16 fullImageH() const;

    void getNbTiles_fullImage(int& nbTilesinW_fullImage, int& nbTilesinH_fullImage) const;

    QRectF rectForTile(int x, int y) const ;

    virtual void *TileData(int tileX,int tileY);

    int get_maximumZoomLevelImg() const;

    void set_currentZoomLevelImg(int zoomLevelImg);
    int get_currentZoomLevelImg() const;

    bool selectZoomLevelImgAndLoadFullImageToDisplayInSize(int zoomLevelImg);

    bool getImageSizeNoZoom(QSize& size);

    //used to init smallImageNav
    S_zoomLevelImgInfos get_SzLIInfosJustAboveWH(int w, int h);

    bool findTileAndPosInTileForFullImageXYPos(const QPointF& qpfFullImageXYPos, QPoint& qpTileXYIndexes, QPoint& qpXYInTile);
    bool getPixelValue(const QPoint& qpTileXYIndexes, const QPoint& qpXYInTile, float& fPixelValue);

protected:

    struct S_ROIxywh {
        int _pixelX0, _pixelY0;
        int _w,_h;
    };

    bool createImageCache(); //this method call closeAndDestroy_cache() if needs

    bool initTilesEngine();
    void closeAndDestroy_cache();

    bool setHashOfZoomLevelInputImageInfos();

    //

    bool initZLIInfos_filenameAndImageSpec_from_to(string strInputImageFilename, S_zoomLevelImgInfos& zoomLevelImgInfos);

    bool loadTile(int tileX, int tileY);
    bool loadTile(int tileX, int tileY, int zoomLevelImageToLoad);

    bool loadTile_useVvv3u8(uint tileX, uint tileY); //will disappear at the end

    //use std::string instead of QString for quick translation to oiio needs:
    string _strBaseInputImageFilename; //no zoom (.ie zoom level = 1)
    string _strRootOfImageZLIStorage; //location of the zoom out level (ie zoom level >= 2)
    int _oiio_cacheAttribute_maxMemoryMB;

    QHash<int, S_zoomLevelImgInfos> _qHashIntSzlii_inputImageInfosByZoomLevel;
    int _maximumZoomLevelImgInQHash;

    ImageCache *_cache;

    //--- parameter which does not change after beeing initialized for an image ---
    int _tileWidth, _tileHeight;

    int _currentZoomLevelImg;

    using vvVectType = vector< //rows of tiles
        vector< //tiles for the row
            VectorType* //data pixel content of each tile
        >
    >;

    QHash<int, vvVectType> _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg;

    struct S_nbChannelsAndTypeDesc_forOIIOUsage {
        int _nbChannels;
        OIIO::TypeDesc::BASETYPE _typeDescBasetype;
    };

    S_nbChannelsAndTypeDesc_forOIIOUsage _aboutInputData;

    ImageSpec _inputImageSpec;

    struct { //S_relevantInfosToKnowWhichTilesReleaseWhenVisuChange

      bool _bZoomLevelImg_ChangeTriggered;
      int _previousZoomLevelImg;//can be 0 as invalid

      QRectF _qrectF_previousVisu;
      bool _bValid_TileIndexesMatchingPreviousVisu; //indicates if _qrect_TileIndexesMatchingPreviousVisu contains usable values
      QRect _qrect_TileIndexesMatchingPreviousVisu; //Tile index topLeft and tile index bottomRight


    } _infosForTilesRelease;

    bool releaseTile(int tileX, int tileY, int zoomLevelImg, const QString& strDebug);

    struct S_tilesIndexInZLI {
     QPoint _index;
     int _zli;
    };

    vector<S_tilesIndexInZLI> _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg;
    vector<S_tilesIndexInZLI> _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg;

    vector<S_tilesIndexInZLI> _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg;

};

#endif
