#include <string>
#include <vector>

#include <memory>

#include <QDebug>

#include <QFileInfo>
#include <QDir>

//---

#include <OpenImageIO/imagebufalgo.h>

#include <math.h> //@LP for NAN
//---

#include "../../vecttype.h"

#include <QRect>

using namespace std;

using namespace OIIO;

#include "tileprovider_withDisplayOutput.h"

#include <QElapsedTimer> //for basic time spent measurement

#include <QImage>

#include "../../toolbox/toolbox_math.h"

TileProvider_withDisplayOutput::TileProvider_withDisplayOutput(
        const string& strBaseInputImageFilename,
        const string& strRootOfImageZLIStorage,
        int oiio_cacheAttribute_maxMemoryMB): TileProvider3(strBaseInputImageFilename,
                                                            strRootOfImageZLIStorage,
                                                            oiio_cacheAttribute_maxMemoryMB) {
}

void TileProvider_withDisplayOutput::addZLIInLoadingQueue(int ZLI) {
    qDebug() << "TileProvider_withDisplayOutput:: " << __FUNCTION__ << "ZLI = " << ZLI;

}


QImage::Format TileProvider_withDisplayOutput::qDisplayImageFormat() {
    return(QImage::Format_Indexed8);

}


bool TileProvider_withDisplayOutput::prepareTilesSetEngine() {

    qDebug() << "TPWOD " << __FUNCTION__;

    bool bSuccess = TileProvider3::prepareTilesSetEngine();
    if (!bSuccess) {
        qDebug() << "TPWOD " << __FUNCTION__ << ": error in TileProvider3::prepareTilesSetEngine()";
        return(false);
    }
    bSuccess = initTilesContainerForDisplay();
    return(bSuccess);
}

bool TileProvider_withDisplayOutput::initTilesContainerForDisplay() {

    _aboutDisplayData._nbChannels = 1;

    _aboutDisplayData._typeDescBasetype = OIIO::TypeDesc::UINT8;

    qDebug() << "TPWOD " << __FUNCTION__;

    QHash<int, S_zoomLevelImgInfos>::const_iterator iter;
    for (iter = _qHashIntSzlii_inputImageInfosByZoomLevel.cbegin();
         iter!= _qHashIntSzlii_inputImageInfosByZoomLevel.cend();
         ++iter) {

        int nbTilesinH_fullImage = iter->_nbTilesinH_fullImage;
        int nbTilesinW_fullImage = iter->_nbTilesinW_fullImage;

        //qDebug() << "TileProvider_withDisplayOutput:" << __FUNCTION__ << "nbTilesinH_fullImage = " << nbTilesinH_fullImage;
        //qDebug() << "TileProvider_withDisplayOutput:" << __FUNCTION__ << "nbTilesinW_fullImage = " << nbTilesinW_fullImage;

        //--- @#LP refactorize this (same code as in tileprovider 3):
        //RefvvVectTypeOfCurrentzoomLevelImg
        vvVectType vvVectTypeOfCurrentzoomLevelImg;
        vvVectTypeOfCurrentzoomLevelImg.resize(nbTilesinH_fullImage); //<<--- y

        int iRow = 0;
        for (auto& rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            rowIter.resize(nbTilesinW_fullImage);//<<--- x

            /*int iTile = 0;
            for(auto& ptrTile: rowIter) {
                //qDebug() << "iRow#" << iRow << " before fill: iTile#" << iTile << ": &@" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                iTile++;
            }*/

            fill(rowIter.begin(), rowIter.end(), nullptr);

            /*iTile = 0;
            for(auto& ptrTile: rowIter) {
                qDebug() << "iRow#" << iRow << " after fill: iTile#" << iTile << ": &@" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                iTile++;
            }*/

            iRow++;
        }

        /*qDebug() << "TPWOD " << "after all fill:";
        int yt = 0;
        for (auto rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            int xt = 0;
            for(auto& ptrTile: rowIter) {
                qDebug() << "TPWOD " << "yt, xt :" << yt << "," << xt << ":" << "&@" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                xt++;
            }
            yt++;
        }*/
        //@#LP refactorize this (same code as in tileprovider 3) ---

        _qHashIntVvVectType_TilesForDisplayByzoomLevelImg.insert(iter.key(), vvVectTypeOfCurrentzoomLevelImg);

        /*
        //qDebug() << "TPWOD " << __FUNCTION__ << "inserted: _qHashIntVvVectType... key=" << iter.key();
        //qDebug() << "TPWOD " << __FUNCTION__ << " size y: " << _qHashIntVvVectType_TilesForDisplayByzoomLevelImg.value(iter.key()).size();
        int iLoop = 0;
        for (auto& rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            qDebug() << "TPWOD " << __FUNCTION__ << "  &@ " << (void*)&rowIter;
            qDebug() << "TPWOD " << __FUNCTION__ << "  size iLoop#: " << iLoop << " size: " << rowIter.size();
            iLoop++;
        }*/

    }
    return(true);
}


bool TileProvider_withDisplayOutput::releaseTile(int tileX, int tileY, int zoomLevelImg, const QString& strDebug) {

    qDebug() << __FUNCTION__ << " would release tileXY: " << tileX << tileY << "about zoomLevelImg=" << zoomLevelImg << " from call: " << strDebug;

    if (!_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(zoomLevelImg)) {
        return(false);
    }
    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[zoomLevelImg]; //.value(keyID) returns a const T
    //qDebug() << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    if (RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] == nullptr) {
        return(false);
    }

    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX]->clear();
    delete RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];
    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] = nullptr;
    return(true);
}

bool TileProvider_withDisplayOutput::releaseZLI(int ZLI) {

    if (_infosForTilesRelease._previousZoomLevelImg == ZLI) {
        return(false);
    }

    TileProvider3::releaseTileForZLI(ZLI);

    if (_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(ZLI)) {

        vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[ZLI]; //.value(keyID) returns a const T

        int yLoop = 0;
        for (auto& yIter : RefvvVectTypeOfCurrentzoomLevelImg) {
            int xLoop = 0;
            for (auto& xIter : yIter) {
                //qDebug() << __FUNCTION__ << "       x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                if (xIter != nullptr) {
                    qDebug() << __FUNCTION__ << "[done] x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                    delete xIter;
                    xIter = nullptr;
                }
                xLoop++;
            }
            yLoop++;
        }
    }

    return(true);
}


bool TileProvider_withDisplayOutput::releaseOldZLIIfNotEqualTo(int currentZLI) {

    if (_infosForTilesRelease._previousZoomLevelImg == currentZLI) {
        return(false);
    }

    TileProvider3::releaseTileForZLI(_infosForTilesRelease._previousZoomLevelImg);

    int ZLIToRelease = _infosForTilesRelease._previousZoomLevelImg;
    if (_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(ZLIToRelease)) {

        vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[ZLIToRelease]; //.value(keyID) returns a const T

        int yLoop = 0;
        for (auto& yIter : RefvvVectTypeOfCurrentzoomLevelImg) {
            int xLoop = 0;
            for (auto& xIter : yIter) {
                //qDebug() << __FUNCTION__ << "       x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                if (xIter != nullptr) {
                    qDebug() << __FUNCTION__ << "[done] x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                    delete xIter;
                    xIter = nullptr;
                }
                xLoop++;
            }
            yLoop++;
        }
    }

    return(true);
}


bool TileProvider_withDisplayOutput::releaseAndSwitchAllocatedTrackedTiles(int newZLI) {

    if (!TileProvider3::releaseAndSwitchAllocatedTrackedTiles()) {
        return(false);
    }

    //test which should be useless with the pre-filled hash
    if (_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(_currentZoomLevelImg)) {

        vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[_currentZoomLevelImg]; //.value(keyID) returns a const T

        int yLoop = 0;
        for (auto& yIter : RefvvVectTypeOfCurrentzoomLevelImg) {
            int xLoop = 0;
            for (auto& xIter : yIter) {
                //qDebug() << __FUNCTION__ << "       x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                if (xIter != nullptr) {
                    qDebug() << __FUNCTION__ << "[done] x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                    delete xIter;
                    xIter = nullptr;
                }
                xLoop++;
            }
            yLoop++;
        }
    }

    set_currentZoomLevelImg(newZLI);

    return(true);
}

//@##LP need release the tile if something failed after the allocation
bool TileProvider_withDisplayOutput::releaseAndLoadTilesToMatchCurrentVisu(/*double xScene, double yScene*/const QRectF &currentVisu) {

    qDebug() << "TPWOD " << __FUNCTION__ << currentVisu;

    bool bNonefailOnLoadTile = TileProvider3::releaseAndLoadTilesToMatchCurrentVisu(currentVisu);

    if (!bNonefailOnLoadTile) {
        qDebug() << "TPWOD " << __FUNCTION__ << "TileProvider_withDisplayOutput:: fail on TileProvider3::releaseAndLoadTilesToMatchCurrentVisu(currentVisu);";
        return(false);
    }

    for(auto iterTileIndexZLI: _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg) {
        releaseTile(iterTileIndexZLI._index.x(), iterTileIndexZLI._index.y(), iterTileIndexZLI._zli, "171");
    }
    for(auto iterTileIndexZLI: _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg) {
        bNonefailOnLoadTile &= allocateTile(iterTileIndexZLI._index.x(), iterTileIndexZLI._index.y());
    }

    QVector<S_TileXYIntersection> qvector_tileXYIntersection;

    if (_infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu) { // bVisuAndImageIntersects;
        int trueBottom =
            _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top()+
            _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.height();
        int trueRight =
            _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left()+
            _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.width();

        for(int yt = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top();
            yt < trueBottom;
            yt++) {
            for(int xt = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left();
                xt < trueRight;
                xt++) {
                QRectF rectTile = rectForTile(xt, yt);
                QRectF qrectFIntersection = rectTile.intersected(currentVisu);
                QRect qrectSubAreaOnTile(
                    static_cast<quint16>(qrectFIntersection.left())%_tileWidth,
                    static_cast<quint16>(qrectFIntersection.top()) %_tileHeight,
                    static_cast<quint16>(qrectFIntersection.width()),
                    static_cast<quint16>(qrectFIntersection.height())
                );
                qvector_tileXYIntersection.push_back({yt, xt, qrectSubAreaOnTile});
            }
        }
    }

    //at this point:
    //- all tiles for currentVisu are allocated and loaded from the input file image
    //- all tiles for currentVisu are allocated but no more
    //- qvector_tileXYIntersection contains rect

    //contrast remap here using the histograms  from the intersected rects
    //and apply to the areas of the tiles for the display

    //qDebug() << "TPWOD " << "nb Area = " <<  qvector_tileXYIntersection.size();

    //QVector<float> _stats_Min;
    //QVector<float> _stats_Max;
    //
    //@#LP: arbitrary huge but surely out values
    float fMinOfAllAreas =  900000.0; // set on arbitrary but surely out value
    float fMaxOfAllAreas = -900000.0; // set on arbitrary but surely out value

    for(auto& iter: qvector_tileXYIntersection) {

        int tileWidth  = tileW(iter._xt);
        int tileHeight = tileH(iter._yt);

        ImageSpec ImgSpecSrc(tileWidth, tileHeight, _aboutInputData._nbChannels, TypeDesc(_aboutInputData._typeDescBasetype));
        void *dataPtrSrc = TileProvider3::TileData(iter._xt,iter._yt);
        //qDebug() << "TPWOD " << __FUNCTION__ << "dataPtrSrc = " << (void*)dataPtrSrc;
        if (dataPtrSrc==nullptr) {
            qDebug() << "TPWOD " << __FUNCTION__ << "error: got dataPtrSrc = TileData(...) as nullptr";
            return(false);
        }
        ImageBuf IBufSrc(ImgSpecSrc, dataPtrSrc);

        /*qDebug() << "TPWOD " << "about src: (_aboutInputData) w, h, nchannels, typeDescBasetype:"
                 << tileWidth << ", "
                 << tileHeight << ", "
                 << _aboutInputData._nbChannels << ", "
                 << _aboutInputData._typeDescBasetype;*/

        //avoid usage of QRect bottom() and right() ( https://doc.qt.io/qt-5/qrect.html#details )

        ROI roiToConsiderInTile(
             iter._qrectIntersection.left(), /*iter._qrectIntersection.right()*/iter._qrectIntersection.left()+iter._qrectIntersection.width(),
             iter._qrectIntersection.top(), /*iter._qrectIntersection.bottom()*/iter._qrectIntersection.top()+iter._qrectIntersection.height(),
             0, 1, //z
             0, 1); ///@#LP fixed to first channel //@LP genereric code: _aboutDisplayData._nbChannels);


        /*qDebug() << __FUNCTION__ << "qvector_tileXYIntersection:";
        qDebug() << __FUNCTION__ << " xt:" << iter._xt;
        qDebug() << __FUNCTION__ << " yt:" << iter._yt;
        qDebug() << __FUNCTION__ << " x begin, end:" << roiToConsiderInTile.xbegin << ", " << roiToConsiderInTile.xend;
        qDebug() << __FUNCTION__ << " y begin, end:" << roiToConsiderInTile.ybegin << ", " << roiToConsiderInTile.yend;*/

        float fMin=.0, fMax=.0;
        bool bComputePixelStats_minMax = computePixelStats_minMax(IBufSrc, roiToConsiderInTile, fMin, fMax);
        if (!bComputePixelStats_minMax) {
            qDebug() << "TPWOD " << __FUNCTION__ << "error: ComputePixelStats_minMax";
            return(false);
        }
        qDebug() << __FUNCTION__<< " fMin = " << fMin << ", fMax = " << fMax;

        if (fMin < fMinOfAllAreas) { fMinOfAllAreas = fMin; }
        if (fMax > fMaxOfAllAreas) { fMaxOfAllAreas = fMax; }

        qDebug() << __FUNCTION__<< " fMinOfAllAreas = " << fMinOfAllAreas << ", fMaxOfAllAreas = " << fMaxOfAllAreas;

     }

    qDebug() << __FUNCTION__<< "min & max OfAllAreas: fMinOfAllAreas = " << fMinOfAllAreas << ", fMaxOfAllAreas = " << fMaxOfAllAreas;

    for(auto& iter: qvector_tileXYIntersection) {

        int tileWidth  = tileW(iter._xt);
        int tileHeight = tileH(iter._yt);

        ImageSpec ImgSpecSrc(tileWidth, tileHeight, _aboutInputData._nbChannels, TypeDesc(_aboutInputData._typeDescBasetype));
        void *dataPtrSrc = TileProvider3::TileData(iter._xt,iter._yt);
        //qDebug() << "TPWOD " << __FUNCTION__ << "dataPtrSrc = " << (void*)dataPtrSrc;
        if (dataPtrSrc==nullptr) {
            qDebug() << "TPWOD " << __FUNCTION__ << "error: got dataPtrSrc = TileData(...) as nullptr";
            return(false);
        }
        ImageBuf IBufSrc(ImgSpecSrc, dataPtrSrc);

        qDebug() << "TPWOD " << __FUNCTION__
                 << "about src: (_aboutInputData) w, h, nchannels, typeDescBasetype:"
                 << tileWidth << ", "
                 << tileHeight << ", "
                 << _aboutInputData._nbChannels << ", "
                 << _aboutInputData._typeDescBasetype;

        ROI roiToConsiderInTile(                    
            iter._qrectIntersection.left(), iter._qrectIntersection.left()+iter._qrectIntersection.width(),
            iter._qrectIntersection.top(), iter._qrectIntersection.top()+iter._qrectIntersection.height(),
             0, 1, //z
             0, 1); ///@#LP fixed to first channel //@LP genereric code: _aboutDisplayData._nbChannels);

        ImageSpec ImgSpecDest(tileWidth, tileHeight, _aboutDisplayData._nbChannels, TypeDesc(_aboutDisplayData._typeDescBasetype));
        void *dataPtrDest = TileData(iter._xt,iter._yt);
        //qDebug() << "TPWOD " << __FUNCTION__ << "dataPtrDest = " << (void*)dataPtrDest;
        if (dataPtrDest==nullptr) {
            qDebug() << "TPWOD " << __FUNCTION__ << "error: got dataPtrDest = TileData(...) as nullptr";
            return(false);
        }
        ImageBuf IBufDest(ImgSpecDest, dataPtrDest);
        /*qDebug() << "TPWOD " << __FUNCTION__ << "about dest (_aboutDisplayData): w, h, nchannels, typeDescBasetype:"
                 << tileWidth << ", "
                 << tileHeight << ", "
                << _aboutDisplayData._nbChannels << ", "
                << _aboutDisplayData._typeDescBasetype;*/

        float minOut = 0.0;
        float maxOut = 1.0;
        //color out adjustement if fMinOfAllAreas == fMaxOfAllAreas
        if (doubleValuesAreClose(static_cast<double>(fMinOfAllAreas), static_cast<double>(fMaxOfAllAreas), 0.00001)) {
            //@LP doubleValuesAreClose with threshold as 0.000001 or 0.0000001 seems to strong as threshold.
            //When using it, the contrast remap stay on 0 to 1 and 'some king of noise due to the resized image in the pyramide' is visible
            //(like regular grayed chessboard or regular black&white vertical lines)

            qDebug() << "TPWOD " << __FUNCTION__ << "fMinOfAllAreas and fMaxOfAllAreas very close: " << fMinOfAllAreas << ", " << fMaxOfAllAreas;

            minOut = fMinOfAllAreas;
            maxOut = fMinOfAllAreas;
        }

        qDebug() << "TPWOD " << __FUNCTION__ << "fMinOfAllAreas = " << fMinOfAllAreas;
        qDebug() << "TPWOD " << __FUNCTION__ << "fMaxOfAllAreas = " << fMaxOfAllAreas;
        qDebug() << "TPWOD " << __FUNCTION__ << "minOut = " << minOut;
        qDebug() << "TPWOD " << __FUNCTION__ << "maxOut = " << maxOut;

#define DEF_contrast_remap_linear
#ifdef DEF_contrast_remap_linear
        bool bContrastRemapSuccess = ImageBufAlgo::contrast_remap(
              IBufDest,//ImageBuf &dst,
              IBufSrc, //ImageBuf_contrastRemapped,//const ImageBuf &src,
              fMinOfAllAreas, fMaxOfAllAreas, //cspan<float> black=0.0f, cspan<float> white=1.0f,
              minOut, maxOut, //0, 1,//cspan<float> min=0.0f, cspan<float> max=1.0f,
              1.0f, 0.5f);

        if (!bContrastRemapSuccess) {
            qDebug() << "TPWOD " << __FUNCTION__ << "fail on contrast_remap(...); geterror() = [ " << geterror().c_str() << " ]" << endl;
            return(false);
        }
#endif

//#define DEF_contrast_remap_sigmoid_curve_no_hard_cutoffs
#ifdef DEF_contrast_remap_sigmoid_curve_no_hard_cutoffs

        // Use a sigmoid curve to add contrast but without any hard cutoffs.
        // Use a contrast parameter of 5.0.
        bool bContrastRemapSuccess = ImageBufAlgo::contrast_remap(
              IBufDest,
              IBufSrc,
              fMinOfAllAreas, fMaxOfAllAreas, //cspan<float> black=0.0f, cspan<float> white=1.0f,
              minOut, maxOut, //0, 1,//cspan<float> min=0.0f, cspan<float> max=1.0f,
              5.0f, 0.5f);

        if (!bContrastRemapSuccess) {
            qDebug() << "TPWOD " << __FUNCTION__ << "fail on contrast_remap(...); geterror() = [ " << geterror().c_str() << " ]" << endl;
            return(false);
        }

        bool bClampSuccess = ImageBufAlgo::clamp(IBufDest, IBufDest, minOut, maxOut);
        if (!bClampSuccess) {
            qDebug() << "TPWOD " << __FUNCTION__ << "fail on bClampSuccess(...); geterror() = [ " << geterror().c_str() << " ]" << endl;
            return(false);
        }

#endif

    }

    /*
    qDebug() << "_stats_Min = " << _stats_Min;
    qDebug() << "_stats_Max = " << _stats_Max;

    qDebug() << "minOfAllArea = " << minOfAllArea;
    qDebug() << "maxOfAllArea = " << maxOfAllArea;
    */

    return(true);

}

bool TileProvider_withDisplayOutput::allocateTile(uint tileX, uint tileY, int zoomLevelImg) {

    qDebug() << "TPWDO: " << __FUNCTION__ << "tileX, tileY: " << tileX << ", " << tileY << " ; zoomLevelImg = " << zoomLevelImg;

    if(!_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(zoomLevelImg)) {
        //qDebug() << "TPWDO: " << __FUNCTION__ << "_qHashIntVvv3u8_tiles.. does not contains value for _currentZoomLevelImg= " << _currentZoomLevelImg;
        return(false);
    }

    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[zoomLevelImg]; //.value(keyID) returns a const T

    //qDebug() << "TPWDO: " << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    VectorType *ptrVectorType = RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];

    //qDebug() << "TPWDO: " << __FUNCTION__ << "(void*)ptrVectorType = " << (void*)ptrVectorType;


    if (ptrVectorType == nullptr) {
        //qDebug() << "TPWDO: " << __FUNCTION__ <<  " we will allocate now";
    } else {
        //qDebug() << "TPWDO: " << __FUNCTION__ << "#2001 tileX " << tileX << "," << tileY << "not empty so already allocated => return now";
        return(true);
    }

    //width and height to allocate:
    int tileWidth  = tileW(tileX);
    int tileHeight = tileH(tileY);

    //qDebug() << "TPWDO: " << __FUNCTION__ << " tileWidth = "<< tileWidth << " ; tileHeight = " << tileHeight;

    //src

    uint32_t sizeVect= tileWidth * tileHeight * _aboutDisplayData._nbChannels;

    //qDebug() << "TPWDO: " << __FUNCTION__ << "sizeVect = " << sizeVect << "( = " << tileWidth << " * " << tileHeight << " * " << _aboutInputData._nbChannels;

    //int sizeofType = sizeofTypeForOIIOTypeDesc(_aboutInputData._typeDesc);

    ptrVectorType = new VectorUint8;

    //qDebug() << "TPWDO: " << __FUNCTION__ << "after allocation: (void*)ptrVectorType = " << (void*)ptrVectorType;

    ptrVectorType->resize(sizeVect);
    ptrVectorType->fillZero();

    qDebug() << "TPWDO: " <<__FUNCTION__ << "(void*)ptrVectorType->data() =" << (void*)(ptrVectorType->data());

    //qDebug() << "TPWDO: " << __FUNCTION__ << "#2003 resized = " << ptrVectorType->size();

    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] = ptrVectorType;

    //qDebug() << "TPWDO: " << __FUNCTION__ << "RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] @" << &(RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX]) << " = @" << RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];

    return(true);

}

bool TileProvider_withDisplayOutput::allocateTile(uint tileX, uint tileY) {
    return(allocateTile(tileX, tileY, _currentZoomLevelImg));
}

void *TileProvider_withDisplayOutput::TileData(int tileX,int tileY) {

    qDebug() << "TPWDO " << "_currentZoomLevelImg =" << _currentZoomLevelImg;

    if (!_qHashIntVvVectType_TilesForDisplayByzoomLevelImg.contains(_currentZoomLevelImg)) {
        return(nullptr);
    }

    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg[_currentZoomLevelImg]; //.value(keyID) returns a const T

    qDebug() << "TPWDO " << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    VectorType *ptrVectorType = RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];
    //qDebug() << "TPWDO " << __FUNCTION__ << "(void*)ptrVectorType = " << (void*)ptrVectorType;

    if (ptrVectorType != nullptr) {
        qDebug() << "TPWDO " << __FUNCTION__ << "ptr @ =" << (void*)ptrVectorType;
        qDebug() << "TPWDO " << __FUNCTION__ << "ptr->data() @ =" << (void*)ptrVectorType->data();
    } else {
        qDebug() << "TPWDO " << __FUNCTION__ << "RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] is nullptr [ " << tileY << " ][ " << tileX << " ]";
        return(nullptr); //@###LP need ignition of an exception, or need to set an error flag to differentiate nullptr about data() and about the vectype* which is very differente error
    }
    return(ptrVectorType->data()); //could be nullptr
}

bool TileProvider_withDisplayOutput::computePixelStats_minMax(const ImageBuf& IBuf, const ROI& roiToConsider, float& min, float& max) {

    /*qDebug() << "TPWOD " << __FUNCTION__ << " roiToConsiderInTile.xbegin, xend, ybegin, yend :";

    qDebug() << "TPWOD " << __FUNCTION__ << "xb:" << roiToConsider.xbegin;
    qDebug() << "TPWOD " << __FUNCTION__ << "xe:" << roiToConsider.xend;
    qDebug() << "TPWOD " << __FUNCTION__ << "yb:" << roiToConsider.ybegin;
    qDebug() << "TPWOD " << __FUNCTION__ << "ye:" << roiToConsider.yend;*/

    ImageBufAlgo::PixelStats pixStats;
    bool bStatsComputeSuccess = ImageBufAlgo::computePixelStats (pixStats, IBuf, roiToConsider);
    if (!bStatsComputeSuccess) {
        qDebug() << "TPWOD " << __FUNCTION__ << "error: computePixelStats failed";
        //@#LP add OIIO's get errors functions
        qDebug() << "TPWOD " << "global geterror() = [ " << geterror().c_str() << " ]";
        return(false);
    }
    /*for (int chan = 0; chan < IBuf.nchannels(); ++chan) {
        qDebug() << "TPWOD " << __FUNCTION__ << "channel #" << chan;
        qDebug() << "TPWOD " << __FUNCTION__ << "  min = " << pixStats.min[chan];
        qDebug() << "TPWOD " << __FUNCTION__ << "  max = " << pixStats.max[chan];
        qDebug() << "TPWOD " << __FUNCTION__ << "nancount = " << pixStats.nancount[chan];
        qDebug() << "TPWOD " << __FUNCTION__ << "infcount = " << pixStats.infcount[chan];
    }*/

    //@#LP get the min max of the first channel
    min = pixStats.min[0];
    max = pixStats.max[0];

    //qDebug() << "TPWOD " << __FUNCTION__ << "return min & max about channel #0: " << min << ", " <<  max;
    return(true);
}


TileProvider_withDisplayOutput::~TileProvider_withDisplayOutput() {

    qDebug() << "TPWOD " << __FUNCTION__;

    quint32 countDeleted = 0;
    int zliCount = 0;
    for (QHash<uint, vvVectType>::iterator iterZLI = _qHashIntVvVectType_TilesForDisplayByzoomLevelImg.begin();
         iterZLI != _qHashIntVvVectType_TilesForDisplayByzoomLevelImg.end();
         ++iterZLI) {
        //qDebug() << "TPWOD " << __FUNCTION__ << "zli key:" << iterZLI.key();
        int yLoop = 0;
        for (auto& yIter : iterZLI.value()) {
            int xLoop = 0;
            for (auto& xIter : yIter) {
                //qDebug() << "TPWOD " << __FUNCTION__ << "       x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                if (xIter != nullptr) {
                    delete xIter;
                    xIter = nullptr;
                    //qDebug() << "TPWOD " << __FUNCTION__ << "[done] x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                    countDeleted++;
                }
                xLoop++;
            }
            yLoop++;
        }
        zliCount++;
    }
    //qDebug() << "TPWOD " << __FUNCTION__ << "countDeleted: " << countDeleted;
    //qDebug() << "TPWOD " << __FUNCTION__ << "( zliCount: " << zliCount << " )";
}

bool TileProvider_withDisplayOutput::loadTilesRequestToMatchCurrentVisuAndZLI(
    const QRectF &currentExposedRectF,
    int current_zoomLevelImage_OfCurrentExposedRectF,
    int zoomLevelImageToLoad) {

    qDebug() << __FUNCTION__ << "currentExposedRectF =" << currentExposedRectF;
    qDebug() << __FUNCTION__ << "current_zoomLevelImage_OfCurrentExposedRectF =" << current_zoomLevelImage_OfCurrentExposedRectF;
    qDebug() << __FUNCTION__ << "zoomLevelImageToLoad =" << zoomLevelImageToLoad;


    //from the currentExposedRectF which match with current_zoomLevelImage_OfCurrentExposedRectF,
    //we compute the ExposedRectF which match with zoomLevelImageToLoad, to be able to know which tiles we need to load

    QSize qsizeImageNoZoom {0, 0};
    getImageSizeNoZoom(qsizeImageNoZoom);

    //qreal xMulZLI = currentCenter.x()*_current_zoomLevelImage_forSceneSide;
    //qreal yMulZLI = currentCenter.y()*_current_zoomLevelImage_forSceneSide;

    //qDebug() << __FUNCTION__ << "xMulZLI =" << xMulZLI;
    //qDebug() << __FUNCTION__ << "yMulZLI =" << yMulZLI;

    //sceneCenterPosF_noZLI_beforeZoom.setX(xMulZLI/static_cast<qreal>(qsizeImageNoZoom.width()));
    //sceneCenterPosF_noZLI_beforeZoom.setY(yMulZLI/static_cast<qreal>(qsizeImageNoZoom.height()));

    //qDebug() << __FUNCTION__ << "sceneCenterPosF_noZLI_beforeZoom =" << sceneCenterPosF_noZLI_beforeZoom;

    QRectF currentExposedRectF_forZoomLevelImageToLoad {.0, .0, .0, .0};

    qreal fDivider = static_cast<qreal>(current_zoomLevelImage_OfCurrentExposedRectF) / static_cast<qreal>(zoomLevelImageToLoad);

    currentExposedRectF_forZoomLevelImageToLoad.setTop   ( currentExposedRectF.top()   / fDivider);
    currentExposedRectF_forZoomLevelImageToLoad.setLeft  ( currentExposedRectF.left()  / fDivider);
    currentExposedRectF_forZoomLevelImageToLoad.setBottom( currentExposedRectF.bottom()/ fDivider);
    currentExposedRectF_forZoomLevelImageToLoad.setRight ( currentExposedRectF.right() / fDivider);

    //qDebug() << "TPWOD " << __FUNCTION__ << currentVisu;

    bool bNonefailOnLoadTile = TileProvider3::loadTilesRequestToMatchCurrentVisuAndZLI(currentExposedRectF_forZoomLevelImageToLoad, zoomLevelImageToLoad);

    if (!bNonefailOnLoadTile) {
        qDebug() << "TPWOD " << __FUNCTION__ << "TileProvider_withDisplayOutput:: fail on TileProvider3::loadTilesRequestToMatchCurrentVisuAndZLI(...);";
        return(false);
    }

    for(auto iterTileIndexZLI: _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg) {
        bNonefailOnLoadTile &= allocateTile(iterTileIndexZLI._index.x(), iterTileIndexZLI._index.y(), zoomLevelImageToLoad);
    }

    return(bNonefailOnLoadTile);
}

