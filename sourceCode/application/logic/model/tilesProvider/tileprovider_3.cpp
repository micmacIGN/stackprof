#include <string>
#include <vector>

#include <memory>

#include <QDebug>

#include <QDir>

//---
#include <OpenImageIO/imagecache.h>

#include <OpenImageIO/imagebufalgo.h>

#include <math.h> //@LP for NAN
//---

#include "../../vecttype.h"

#include <QRect>

using namespace std;

using namespace OIIO;

#include "tileprovider_3.h"

#include "../../io/InputImageFormatChecker.h"

#include "../../toolbox/toolbox_pathAndFile.h"

#include <QElapsedTimer> //for basic time spent measurement

void S_zoomLevelImgInfos::showContent() {
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_strFilename: " << _strFilename.c_str();
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_imageSpec.width  :" << _imageSpec.width;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_imageSpec.height :" << _imageSpec.height;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_imageSpec.nchannels :" << _imageSpec.nchannels;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_imageSpec.format.size():" << _imageSpec.format.size();
    OIIO::TypeDesc::BASETYPE typeDescBasetype = static_cast<OIIO::TypeDesc::BASETYPE>(_imageSpec.format.basetype);
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_imageSpec typeDescBasetype:" << typeDescBasetype;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos)";
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_nbTilesinW_fullImage  : " << _nbTilesinW_fullImage;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_nbTilesinH_fullImage  : " << _nbTilesinH_fullImage;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_widthOfLastTilesColumn: " << _widthOfLastTilesColumn;
    qDebug() << __FUNCTION__ << "(S_zoomLevelImgInfos) " << "_heightOfLastTilesRow  : " << _heightOfLastTilesRow;
}

bool TileProvider3::getImageSizeNoZoom(QSize& size) {
    size.setWidth (_qHashIntSzlii_inputImageInfosByZoomLevel.value(1)._imageSpec.width);
    size.setHeight(_qHashIntSzlii_inputImageInfosByZoomLevel.value(1)._imageSpec.height);
    qDebug() << __FUNCTION__ << size;
    return(true);
}


//return the width of a full standard tile
int TileProvider3::tileW() const {
    return(_tileWidth);
}

//return the height of a full standard tile
int TileProvider3::tileH() const {
    return(_tileHeight);
}

//return the width of any tile at xIdxTile
int TileProvider3::tileW(int xIdxTile) const {
    int nbTilesinW_fullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._nbTilesinW_fullImage;
    //qDebug() << __FUNCTION__ << "xIdxTile = " << xIdxTile << " ; nbTilesinW_fullImage = " << nbTilesinW_fullImage;
    if (xIdxTile == nbTilesinW_fullImage-1) {
        //qDebug() << __FUNCTION__ << "return:" << _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._widthOfLastTilesColumn;
        return(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._widthOfLastTilesColumn);
    }
    //qDebug() << __FUNCTION__ << "return std:" << _tileWidth;
    return(_tileWidth);
}

//return the height of any tile at yIdxTile
int TileProvider3::tileH(int yIdxTile) const {
    int nbTilesinH_fullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._nbTilesinH_fullImage;
    //qDebug() << __FUNCTION__ << "yIdxTile = " << yIdxTile << " ; nbTilesinH_fullImage = " << nbTilesinH_fullImage;
    if (yIdxTile == nbTilesinH_fullImage -1) {
         //qDebug() << __FUNCTION__ << "return:" << _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._heightOfLastTilesRow;
        return(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._heightOfLastTilesRow);
    }
    //qDebug() << __FUNCTION__ << "return std:" << _tileHeight;
    return(_tileHeight);
}

TileProvider3::TileProvider3(const string& strBaseInputImageFilename, const string& strRootOfImageZLIStorage, int oiio_cacheAttribute_maxMemoryMB):
    _strBaseInputImageFilename(strBaseInputImageFilename),
    _strRootOfImageZLIStorage(strRootOfImageZLIStorage),
    _oiio_cacheAttribute_maxMemoryMB(oiio_cacheAttribute_maxMemoryMB),
    _cache(nullptr) {

    _maximumZoomLevelImgInQHash = 0;
    _currentZoomLevelImg = 0;

    _tileWidth = 0;
    _tileHeight = 0;

    if (_cache != nullptr) {
        qDebug() << "#0";
        qDebug() << __FUNCTION__ << "#0 (_cache != nullptr ): _cache: @ &@" << (void*)_cache << ", " << (void*)&_cache;
    } else {
        qDebug() << "#00";
        qDebug() << __FUNCTION__ << "#00 (_cache is nullptr): _cache: @ &@" << (void*)_cache << ", " << (void*)&_cache;
    }

    _infosForTilesRelease._bZoomLevelImg_ChangeTriggered = false;

    _infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu = false;
}


S_zoomLevelImgInfos TileProvider3::get_SzLIInfosJustAboveWH(int w, int h) {

    ImageSpec ImageSpecNoCandidate;
    S_zoomLevelImgInfos SzLIInfosCandidate {"", ImageSpecNoCandidate, 0, 0, 0, 0 };

    qDebug() << __FUNCTION__ << "QHash ImageFilenamesByZoomLevel= ";

    int qhash_size = _qHashIntSzlii_inputImageInfosByZoomLevel.size();

    qDebug() << __FUNCTION__ << "qhash_size =" << qhash_size;

    if (!qhash_size) {
        qDebug() << __FUNCTION__ << "if (!_qHashIntSzlii_inputImageInfosByZoomLevel.size()) {";
        return(SzLIInfosCandidate);
    }

    qDebug() << __FUNCTION__ << "#_#1_";

    if (qhash_size == 1) {
        qDebug() << __FUNCTION__ << "if (_qHashIntSzlii_inputImageInfosByZoomLevel.size() == 1) {";
        SzLIInfosCandidate = _qHashIntSzlii_inputImageInfosByZoomLevel.cbegin().value();
        return(SzLIInfosCandidate);
    }

    qDebug() << __FUNCTION__ << "#_#2_";

    int ZLICandidate = 0;
    int wCandidate = 0;
    int hCandidate = 0;
    bool bCandidateInitialized = false;

    QHash<int, S_zoomLevelImgInfos>::const_iterator iter;
    for (iter = _qHashIntSzlii_inputImageInfosByZoomLevel.cbegin();
         iter!= _qHashIntSzlii_inputImageInfosByZoomLevel.cend();
         ++iter) {

        int wCurrent = iter.value()._imageSpec.width;
        int hCurrent = iter.value()._imageSpec.height;
        int ZLICurrent = iter.key();

        qDebug() << __FUNCTION__ << "wCurrent = " << wCurrent;
        qDebug() << __FUNCTION__ << "hCurrent = " << hCurrent;
        qDebug() << __FUNCTION__ << "ZLICurrent = " << ZLICurrent;

        //too small
        if ((wCurrent < w)&&(hCurrent < h)) {
            qDebug() << __FUNCTION__ << "too small";
            continue;
        }

        if (bCandidateInitialized) {
            //bigger than candidate
            if ((wCurrent > wCandidate)&&(hCurrent > hCandidate)) {
                qDebug() << __FUNCTION__ << "bigger than candidate";
                continue;
            }
            qDebug() << __FUNCTION__ << "set to candidate";
            wCandidate = wCurrent;
            hCandidate = hCurrent;
            ZLICandidate = ZLICurrent;

        } else {
            qDebug() << __FUNCTION__ << "set to candidate (first set)";
            wCandidate = wCurrent;
            hCandidate = hCurrent;
            ZLICandidate = ZLICurrent;
            bCandidateInitialized = true;
        }

        qDebug() << __FUNCTION__ << "#_#4_";

        qDebug() << __FUNCTION__
                 << iter.key() << " : " << iter.value()._strFilename.c_str()
                 << "; w, h:" << iter.value()._imageSpec.width<< " x " << iter.value()._imageSpec.height
                 << "; nchannels, format.basetype:" << iter.value()._imageSpec.nchannels << ", " << (int)iter.value()._imageSpec.format.basetype;

        qDebug() << __FUNCTION__ << "#_#5_";

    }

    qDebug() << __FUNCTION__ << "#_#6_";

    if (!bCandidateInitialized) { //no candidate; @#LP: occurs if no image w | h > w, h  and hash size > 1 (considered as a not encountered case)
        return(SzLIInfosCandidate);
    }

    qDebug() << __FUNCTION__ << "#_#7_";

    QHash<int, S_zoomLevelImgInfos>::const_iterator cIterFound = _qHashIntSzlii_inputImageInfosByZoomLevel.find(ZLICandidate);

    if (cIterFound != _qHashIntSzlii_inputImageInfosByZoomLevel.end()) {

        qDebug() << __FUNCTION__ << "#_#8_";

        SzLIInfosCandidate = cIterFound.value();

        qDebug() << __FUNCTION__ << "SzLIInfosCandidate: "
             << SzLIInfosCandidate._strFilename.c_str()
             << SzLIInfosCandidate._imageSpec.width<< " x " << SzLIInfosCandidate._imageSpec.height;
    }

    qDebug() << __FUNCTION__ << "#_#9_";


    return(SzLIInfosCandidate);
}


int TileProvider3::get_maximumZoomLevelImg() const {
    qDebug() << __FUNCTION__ << "return _maximumZoomLevelImg = " << _maximumZoomLevelImgInQHash;
    return(_maximumZoomLevelImgInQHash);
}

void TileProvider3::set_currentZoomLevelImg(int zoomLevelImg) {

    if (zoomLevelImg == _currentZoomLevelImg) {
        qDebug() << __FUNCTION__ << __LINE__ << "if (zoomLevelImg == _currentZoomLevelImg) {";
        return;
    }

    _infosForTilesRelease._previousZoomLevelImg = _currentZoomLevelImg;
    _infosForTilesRelease._bZoomLevelImg_ChangeTriggered = true;

    if (zoomLevelImg > _maximumZoomLevelImgInQHash) {
        zoomLevelImg = _maximumZoomLevelImgInQHash;
    }
    if (zoomLevelImg < 1) {
        zoomLevelImg = 1;
    }
    _currentZoomLevelImg = zoomLevelImg;

    qDebug() << __FUNCTION__ << __LINE__ << "_previousZoomLevelImg set to :" <<  _infosForTilesRelease._previousZoomLevelImg;
    qDebug() << __FUNCTION__ << __LINE__ << "new _currentZoomLevelImg = " << _currentZoomLevelImg;

}

int TileProvider3::get_currentZoomLevelImg() const {
    return(_currentZoomLevelImg);
}

//@#LP warning method naming: feed a S_zoomLevelImgInfos struct but just strInputImageFilename and imageSpec
bool TileProvider3::initZLIInfos_filenameAndImageSpec_from_to(string strInputImageFilename, S_zoomLevelImgInfos& zoomLevelImgInfos) {
    zoomLevelImgInfos._strFilename = strInputImageFilename;
    return(InputImageFormatChecker::getImageSpec(strInputImageFilename, zoomLevelImgInfos._imageSpec));
}

bool TileProvider3::selectZoomLevelImgAndLoadFullImageToDisplayInSize(int zoomLevelImg) {

    set_currentZoomLevelImg(zoomLevelImg);

    qDebug() << __FUNCTION__ << "set _currentZoomLevelImg to : " << _currentZoomLevelImg;

    /*
    qDebug() << "final QHash ImageFilenamesByZoomLevel= ";
    QHash<uint, S_zoomLevelImgInfos>::cosnt_iterator iter;
    for (iter = _qHashIntSzlii_inputImageInfosByZoomLevel.begin();
         iter!= _qHashIntSzlii_inputImageInfosByZoomLevel.end();
         ++iter) {
        qDebug() << iter.key() << " : " << iter.value().strFilename.c_str() << ":" << iter.value()._imageSpec.width<< " x " << iter.value()._imageSpec.height;
    }
    */

    return(true);
}

bool TileProvider3::setHashOfZoomLevelInputImageInfos() {

    _qHashIntSzlii_inputImageInfosByZoomLevel.clear();
    _maximumZoomLevelImgInQHash = 0;

    if (_strBaseInputImageFilename.empty()) {
        qDebug() << __FUNCTION__ << "error: _strInputImageFilename is empty";
        return(false);
    }

    QString qStrPathAndFilename;

    qStrPathAndFilename = getPathAndFilenameFromPotentialSymlink(_strBaseInputImageFilename.c_str());

    S_zoomLevelImgInfos zliInfos;
    zliInfos._strFilename =  qStrPathAndFilename.toStdString();
    InputImageFormatChecker iifc;
    bool bCheckSuccess = iifc.check(zliInfos._strFilename, zliInfos._imageSpec);
    if (!bCheckSuccess) {
        qDebug() << __FUNCTION__ << "error: not supported fileformat file " << _strBaseInputImageFilename.c_str();
        return(false);
    }

    QFileInfo fileInfoFinalTarget(qStrPathAndFilename);
    QString qstrBasePath = QString(_strRootOfImageZLIStorage.c_str()); //fileInfoFinalTarget.absolutePath();

    //_LP: only one dot ('.') permitted in filename
    QString qSstrBaseFilenameNoneExtension = fileInfoFinalTarget.baseName();  // example: "archive.tar.gz" => baseName = "archive" (".tar.gz" removed)
    QString qStrExtension = fileInfoFinalTarget.completeSuffix();  // ext = "tar.gz"

    qDebug() << __FUNCTION__ << "qstrBasePath = " << qstrBasePath;
    qDebug() << __FUNCTION__ << "qSstrBaseFilenameNoneExtension = " << qSstrBaseFilenameNoneExtension;
    qDebug() << __FUNCTION__ << "qStrExtension = " << qStrExtension;


    _maximumZoomLevelImgInQHash = 1;
    _qHashIntSzlii_inputImageInfosByZoomLevel.insert(_maximumZoomLevelImgInQHash, zliInfos);

    bool bErrorOccured = false;

    //_LP: gap in zoom level file is not handled (missing file(s) for some zoom level(s) before the last existing zoom level is not permitted)
    bool bGap = false;
    //QString qStr_zoomLevelIDSuffixPart = "_Zoom";
    QString qStr_zoomLevelIDPart = "zoomOutLevel_";
    for (int zoomLevel = 2; zoomLevel < 128; zoomLevel*=2) {
        QString qStrFilenameForZoomLevel =
                qstrBasePath + QDir::separator()
                + qStr_zoomLevelIDPart + QString::number(zoomLevel)
                + "." + qStrExtension;

        qDebug() << "qStrFilenameForZoomLevel = " << qStrFilenameForZoomLevel;

        //_LP: there is none read permission check; considers read permitted
        if (fileExists(qStrFilenameForZoomLevel)) {
            if (bGap) {
                qDebug() << "error: file missing for zoom level #" << zoomLevel/2;

                bErrorOccured  = true;
                break;
            }
            //using QString::toStdString, assuming this:
            //' As of Qt 5.0, QString::toStdString() now uses QString::toUtf8() to perform the conversion, so the Unicode properties of the string will not be lost '
            //( https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring )

            if (!initZLIInfos_filenameAndImageSpec_from_to(qStrFilenameForZoomLevel.toStdString(), zliInfos)) {
                qDebug() << __FUNCTION__ << "error in get_zoomLevelImgInfosFor on file " << qStrFilenameForZoomLevel;
                bErrorOccured  = true;
                break;
            }

            _qHashIntSzlii_inputImageInfosByZoomLevel.insert(zoomLevel, zliInfos);
            _maximumZoomLevelImgInQHash = zoomLevel;
        } else {
            qDebug() << "possible gap at zoom level #" << zoomLevel;
            bGap = true;
        }
    }

    if (bErrorOccured) {
        _qHashIntSzlii_inputImageInfosByZoomLevel.clear();
        _maximumZoomLevelImgInQHash = 0;
        return(false);
    }

    int wPreviousLevel = 0;
    int hPreviousLevel = 0;
    ImageSpec imageSpecPreviousLevel = _qHashIntSzlii_inputImageInfosByZoomLevel.value(1)._imageSpec;

    bErrorOccured = false;

    for (int level = 1; level <= _maximumZoomLevelImgInQHash; level*=2) {

        zliInfos = _qHashIntSzlii_inputImageInfosByZoomLevel.value(level);

        int wCurrentLevel = zliInfos._imageSpec.width;
        int hCurrentLevel = zliInfos._imageSpec.height;
        qDebug() << __FUNCTION__ << "in: level#" << level << " w, h:" << wCurrentLevel << ", " << hCurrentLevel;

        if (level > 1) {

            if (zliInfos._imageSpec.nchannels != imageSpecPreviousLevel.nchannels) {
                qDebug() << __FUNCTION__ << "error: nb channels not equal";
                bErrorOccured = true;
                break;
            }
            if (zliInfos._imageSpec.format.basetype != imageSpecPreviousLevel.format.basetype) {
                qDebug() << __FUNCTION__ << "error: TypeDesc format.basetype not equal";
                bErrorOccured = true;
                break;
            }

            if (wPreviousLevel%2) { wPreviousLevel++; }
            if (hPreviousLevel%2) { hPreviousLevel++; }

            if ((wCurrentLevel != (wPreviousLevel/2)) ||
                (hCurrentLevel != (hPreviousLevel/2))) {
                qDebug() << __FUNCTION__ << "error: invalid w|h of level #" << level <<  " file: " << _qHashIntSzlii_inputImageInfosByZoomLevel.value(level)._strFilename.c_str();
                qDebug() << __FUNCTION__ << " details: w, h :" << wCurrentLevel << ", " << hCurrentLevel;
                qDebug() << __FUNCTION__ << "        : previous level (can be +1 adjusted): w, h:" << wPreviousLevel << ", " << hPreviousLevel;

                qDebug() << __FUNCTION__ << wCurrentLevel << " <w> " << wPreviousLevel;
                qDebug() << __FUNCTION__ << hCurrentLevel << " <h> " << hPreviousLevel;

                bErrorOccured = true;
                break;

            } else {
                qDebug() << __FUNCTION__ << "checked level#" << level << " w, h:" << wCurrentLevel << ", " << hCurrentLevel;
            }

            imageSpecPreviousLevel = zliInfos._imageSpec;

        }
        wPreviousLevel = wCurrentLevel;
        hPreviousLevel = hCurrentLevel;
    }

    if (bErrorOccured) {
        _qHashIntSzlii_inputImageInfosByZoomLevel.clear();
        _maximumZoomLevelImgInQHash = 0;
        return(false);
    }

    //all levels are sync about nchannels and TypeDesc, hence take the available infos from imageSpecPreviousLevel
    _aboutInputData._nbChannels = imageSpecPreviousLevel.nchannels;

    _aboutInputData._typeDescBasetype = static_cast<OIIO::TypeDesc::BASETYPE>(imageSpecPreviousLevel.format.basetype);

     qDebug() << "_aboutInputData._nbChannels   set to:" << _aboutInputData._nbChannels;
     qDebug() << "_aboutInputData._typeDescBasetype set to:" << _aboutInputData._typeDescBasetype;

    qDebug() << "final QHash ImageFilenamesByZoomLevel= ";
    QHash<int, S_zoomLevelImgInfos>::const_iterator iter;
    for (iter = _qHashIntSzlii_inputImageInfosByZoomLevel.cbegin();
         iter!= _qHashIntSzlii_inputImageInfosByZoomLevel.cend();
         ++iter) {
        qDebug() << iter.key() << " : " << iter.value()._strFilename.c_str()
                 << "; w, h:" << iter.value()._imageSpec.width<< " x " << iter.value()._imageSpec.height
                 << "; nchannels, format.basetype:" << iter.value()._imageSpec.nchannels << ", " << (int)iter.value()._imageSpec.format.basetype;

    }
    qDebug() << "_maximumZoomLevelImgInQHash = " << _maximumZoomLevelImgInQHash;

    return(true);
}

bool TileProvider3::initTilesEngine() {

    _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg.clear();
    _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg.clear();

    _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg.clear();

    _tileWidth = 512;
    _tileHeight = 512;
    qDebug() << __FUNCTION__ << "tp3 _tileWidth, _tileHeight = " << _tileWidth << ", " << _tileHeight;

    QHash<int, S_zoomLevelImgInfos>::iterator iter;
    for (iter = _qHashIntSzlii_inputImageInfosByZoomLevel.begin();
         iter!= _qHashIntSzlii_inputImageInfosByZoomLevel.end();
         ++iter) {

        int fullImageW = iter->_imageSpec.width;
        int fullImageH = iter->_imageSpec.height;

        int nbTilesinW_fullImage = ceil( (double)fullImageW / (double)_tileWidth );
        int nbTilesinH_fullImage = ceil( (double)fullImageH / (double)_tileHeight);

        qDebug() << __FUNCTION__ << "tp3 _nbTilesinWFullImage, _nbTilesinHFullImage = " << nbTilesinW_fullImage<< ", " << nbTilesinH_fullImage;

        iter->_nbTilesinW_fullImage = nbTilesinW_fullImage;
        iter->_nbTilesinH_fullImage = nbTilesinH_fullImage;

        int widthOfLastTilesColumn = fullImageW - _tileWidth *(nbTilesinW_fullImage-1);
        int heightOfLastTilesRow   = fullImageH - _tileHeight*(nbTilesinH_fullImage-1);

        //adjust last Tiles sizes if the image size matches perfectly with tiles size:
        if (!widthOfLastTilesColumn) {
            widthOfLastTilesColumn = _tileWidth;
        }
        if (!heightOfLastTilesRow) {
            heightOfLastTilesRow = _tileHeight;
        }
        qDebug() << __FUNCTION__ << "tp3 widthOfLastTilesColumn, heightOfLastTilesRow = " << widthOfLastTilesColumn<< ", " << heightOfLastTilesRow;

        iter->_widthOfLastTilesColumn = widthOfLastTilesColumn;
        iter->_heightOfLastTilesRow   = heightOfLastTilesRow;

        //qDebug() << __FUNCTION__ << "inserted: _qHashIntVvv3u8... key=" << iter.key();

        //RefvvVectTypeOfCurrentzoomLevelImg
        vvVectType vvVectTypeOfCurrentzoomLevelImg;
        vvVectTypeOfCurrentzoomLevelImg.resize(nbTilesinH_fullImage); //<<--- y

        int iRow = 0;
        for (auto& rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            rowIter.resize(nbTilesinW_fullImage);//<<--- x

            int iTile = 0;
            for(auto& ptrTile: rowIter) {
                //qDebug() << "tp3 iRow#" << iRow << " before fill: iTile#" << iTile << ": @" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                iTile++;
            }

            fill(rowIter.begin(), rowIter.end(), nullptr);

            iTile = 0;
            for(auto& ptrTile: rowIter) {
                //qDebug() << "tp3  iRow#" << iRow << " after fill: iTile#" << iTile << ": @" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                iTile++;
            }

            iRow++;

        }

        qDebug() << "tp3  after all fill:";
        int yt = 0;
        for (auto rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            int xt = 0;
            for(auto& ptrTile: rowIter) {
                //qDebug() << "tp3 yt, xt :" << yt << "," << xt << ":" << "@" << (void*)&ptrTile << "= @" << (void*)ptrTile;
                xt++;
            }
            yt++;
        }

        _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.insert(iter.key(), vvVectTypeOfCurrentzoomLevelImg);

        /*
        //qDebug() << __FUNCTION__ << "tp3  inserted: _qHashIntVvVectType... key=" << iter.key();
        //qDebug() << __FUNCTION__ << "tp3  size y: " << _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.value(iter.key()).size();
        int xLoop = 0;
        for (auto& rowIter : vvVectTypeOfCurrentzoomLevelImg) {
            qDebug() << __FUNCTION__ << "tp3   @ " << (void*)&rowIter;
            qDebug() << __FUNCTION__ << "tp3   size x#: " << xLoop << " size: " << rowIter.size();
            xLoop++;
        }
        */

    }

    //qDebug() << "sizeof(_vvvf_tilesOfTheImageContent) = " << sizeof(_vvvf_tilesOfTheImageContent);
    //qDebug() << "sizeof(_vvv8_tilesOfTheImageContent) = " << sizeof(_vvv8_tilesOfTheImageContent);

    //qDebug() << "_visuWidth  =" << _visuWidth;
    //qDebug() << "_visuHeight =" << _visuHeight;

    //values computed; dividing the visualization area size (fixed for now) by the tile width, tile height
    //the image scale will be handled switching the image to use when needs
    //_nbColumnOfTilesToLoadForView = ceil((double)_visuWidth  / (double)_tileWidth ) +2; //+2: to have tiles for inbetween visu location
    //_nbRowOfTilesToLoadForView    = ceil((double)_visuHeight / (double)_tileHeight) +2; //+2: to have tiles for inbetween visu location

    //qDebug() << "_nbColumnOfTilesToLoad = " << _nbColumnOfTilesToLoadForView;
    //qDebug() << "_nbRowOfTilesToLoad    = " << _nbRowOfTilesToLoadForView;

    qDebug() << "end of " << __FUNCTION__;
    return(true);
}

bool TileProvider3::createImageCache() {

    qDebug() <<__FUNCTION__;

    if (_cache != nullptr) {
        qDebug() <<__FUNCTION__ << "error: _cache is not nullptr ";
        qDebug() << __FUNCTION__ << "#1 Not OK (_cache != nullptr ): _cache: @ &@" << (void*)_cache << ", " << (void*)&_cache;
        return(false);
    }

     // Create an image cache and set some options
    _cache = ImageCache::create(false);
    if (_cache == nullptr) {
         qDebug() <<__FUNCTION__ << "error: ImageCache::create failed";
         qDebug() << __FUNCTION__ << "#2 (_cache == nullptr)"; // _cache: %p %p\n", _cache, &_cache);
         return(false);
    }

    qDebug() << __FUNCTION__ << "#3 ok (_cache != nullptr ): _cache: @ &@" << (void*)_cache << ", " << (void*)&_cache;

    qDebug() << __FUNCTION__ << "_oiio_cacheAttribute_maxMemoryMB = " << _oiio_cacheAttribute_maxMemoryMB;

    if (_oiio_cacheAttribute_maxMemoryMB < 2)  { //avoid invalid cache size (zero typically)
        closeAndDestroy_cache();
        return(false);
    }

    bool bSuccessAttributeSet = true;

    bSuccessAttributeSet &= _cache->attribute("max_memory_MB", _oiio_cacheAttribute_maxMemoryMB);
    bSuccessAttributeSet &= _cache->attribute("autotile", 64); //TBC value
    bSuccessAttributeSet &= _cache->attribute("autoscanline", 0); //square tile (autotile != 0)

    bSuccessAttributeSet &= _cache->attribute("forcefloat", 1);

    bSuccessAttributeSet &= _cache->attribute("unassociatedalpha", 1);

    if (!bSuccessAttributeSet) {
      qDebug() <<__FUNCTION__ << "error: one or more cache->attribute set failed";
      closeAndDestroy_cache();
      return(false);
    }

    return(true);
}




bool TileProvider3::prepareTilesSetEngine() {

    qDebug() << "TileProvider3::" << __FUNCTION__;

    bool bSuccess = false;
    for (int iStep = 0; iStep < 3; iStep++) {
        qDebug() << "step #" << iStep;
        switch(iStep) {
            case 0: bSuccess = createImageCache(); break;
            case 1: bSuccess = setHashOfZoomLevelInputImageInfos(); break;
            case 2: bSuccess = initTilesEngine(); break;
            default: break;
        }
        if (!bSuccess) {
            qDebug() << __FUNCTION__ << "error in step " << iStep;
            return(false);
        }
    }
    return(bSuccess);
}


void TileProvider3::getNbTiles_fullImage(int& nbTilesinW_fullImage, int& nbTilesinH_fullImage) const {
    //@#LP check the value if key not found (should never happens)
    qDebug() << __FUNCTION__ << "_currentZoomLevelImg =" << _currentZoomLevelImg;
    nbTilesinW_fullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._nbTilesinW_fullImage;
    nbTilesinH_fullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._nbTilesinH_fullImage;
}

//@#LP int range limitation
QRectF TileProvider3::rectForTile(int x, int y/*, const string& strDebugCaller*/) const {
    //Return the rectangle for the tile at position (x, y).
    //qDebug() << __FUNCTION__ <<  ": " <<  QRectF(x * _tileWidth, y * _tileHeight, TileW(x), TileH(y));
    //qDebug() << __FUNCTION__ <<  ": " <<  strDebugCaller.c_str() <<  QRectF(x * _tileWidth, y * _tileHeight, TileW(x), TileH(y));
    return QRectF(x * _tileWidth, y * _tileHeight, tileW(x), tileH(y));
}

bool TileProvider3::releaseAndSwitchAllocatedTrackedTiles() {

    if (_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(_currentZoomLevelImg)) {

        vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[_currentZoomLevelImg]; //.value(keyID) returns a const T

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

    _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg.clear();
    _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg = _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg;

    return(true);
}

void TileProvider3::releaseTileForZLI(int ZLIToRelease) {

    if (_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(ZLIToRelease)) {        
        vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[ZLIToRelease]; //.value(keyID) returns a const T

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
}


bool TileProvider3::releaseAndLoadTilesToMatchCurrentVisu(/*double xScene, double yScene*/const QRectF &currentVisu) {

    //qDebug() << "TileProvider3::" << __FUNCTION__;

    QElapsedTimer timer;
    timer.start();

    _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg.clear();
    _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg.clear();

    //release tiles part
    // when switching ZLI, release all the tiles of the previous ZLI
    if (_infosForTilesRelease._bZoomLevelImg_ChangeTriggered) {
        //qDebug() << __FUNCTION__ <<  "if (_infosForTilesRelease._bZoomLevelImg_ChangeTriggered) {";

        //qDebug() << "TileProvider3::" << __FUNCTION__ << " ZoomLevelImg switch!";

        if (_infosForTilesRelease._previousZoomLevelImg == 0) {
            //qDebug() << __FUNCTION__ <<  "if (_infosForTilesRelease._previousZoomLevelImg == 0) {";
        } else {
            //qDebug() << "TileProvider3::" << __FUNCTION__ << " and _previousZoomLevelImg is !=0";

            if (_infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu) {

                //qDebug() << "TileProvider3::" << __FUNCTION__ << " release previous Visu for previous ZLI";
                //qDebug() << "TileProvider3::" << __FUNCTION__ << " _infosForTilesRelease._previousZoomLevelImg = " <<  _infosForTilesRelease._previousZoomLevelImg;

                //avoid usage of QRect bottom() and right() ( https://doc.qt.io/qt-5/qrect.html#details )
                int trueBottom =  _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.height();
                for(int yt     =  _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top(); yt < trueBottom; yt++) {
                    int trueRight = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.width();
                    //qDebug() << " " << __FUNCTION__ << "  left() = " << _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left();
                    //qDebug() << " " << __FUNCTION__ << " width() = " << _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.width();
                    //qDebug() << " " << __FUNCTION__ << " trueRight = " << trueRight;
                    for(int xt  = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left(); xt < trueRight; xt++) {
                        releaseTile(xt,yt, _infosForTilesRelease._previousZoomLevelImg, "920");
                    }
                }

            } else {
                //qDebug() << "TileProvider3::" << __FUNCTION__ << "..._bValid_TileIndexesMatchingPreviousVisu is false" ;
                //do nothing
            }
        }
    } else {
        //do nothing
    }

    //find the tiles (no tiles load) which needs to be loaded for the current visu:
    QRectF qrectf_ImageAreaOfcurrentZLI(QPointF {.0, .0},
                                QPointF { static_cast<double>(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.width),
                                          static_cast<double>(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.height)});

    int idxX_firstTile = 0;
    int idxY_firstTile = 0;
    int idxX_lastTile = 0;
    int idxY_lastTile = 0;

    bool bVisuAndImageIntersects = true;

    bool bInside  = qrectf_ImageAreaOfcurrentZLI.contains(currentVisu);
    if (bInside) {
        idxX_firstTile = currentVisu.left() /_tileWidth;
        idxY_firstTile = currentVisu.top()  /_tileHeight;
        idxX_lastTile  = (currentVisu.left() + currentVisu.width()) / _tileWidth;
        idxY_lastTile  = (currentVisu.top() + currentVisu.height())/_tileHeight;
    } else {

        int wFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.width;
        int hFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.height;

        QRectF qrectfFullImage(0, 0, wFullImage, hFullImage);
        QRectF qrectIntersection = currentVisu.intersected(qrectfFullImage);
        if (qrectIntersection.isEmpty()) {
            qDebug() << __FUNCTION__ << "fullImage and currentVisu do not intersects";
            bVisuAndImageIntersects = false;

        } else {
            idxX_firstTile = qrectIntersection.left()  /_tileWidth;
            idxY_firstTile = qrectIntersection.top()   /_tileHeight;
            idxX_lastTile  = (qrectIntersection.left()+qrectIntersection.width()) /_tileWidth;
            idxY_lastTile  = (qrectIntersection.top()+qrectIntersection.height()) /_tileHeight;
        }
    }

     //qDebug() << __FUNCTION__ <<  "xy firstTile; xy lastTile: " << idxX_firstTile << ", " << idxY_firstTile << "; " << idxX_lastTile << ", " << idxY_lastTile;

    //the ZLI has not changed; release the tiles of the previous visu which don't intersect with the current visu
    if (!_infosForTilesRelease._bZoomLevelImg_ChangeTriggered) {
        //qDebug() << __FUNCTION__ <<  "if (!_infosForTilesRelease._bZoomLevelImg_ChangeTriggered)";

        if (_infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu) {
            //qDebug() << __FUNCTION__ <<  "if (_infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu)";

            if (bVisuAndImageIntersects) {
                //qDebug() << __FUNCTION__ <<  "if (bVisuAndImageIntersects) {";

                 QRect _qrect_TileIndexesMatchingCurrentVisu; //avoiding constructor with bottom right
                 _qrect_TileIndexesMatchingCurrentVisu.setCoords(idxX_firstTile, idxY_firstTile, idxX_lastTile, idxY_lastTile);

                 //qDebug() << __FUNCTION__ <<  "_qrect_TileIndexesMatchingCurrentVisu =" << _qrect_TileIndexesMatchingCurrentVisu;

                 int trueBottom = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.height();//qDebug() << __FUNCTION__ << "trueBottom =" << trueBottom;
                 for(int yt     = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top(); yt < trueBottom; yt++) {
                    int trueRight = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.width();//qDebug() << __FUNCTION__ << "trueRight =" << trueRight;
                    for(int xt    = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left(); xt < trueRight; xt++) {
                        //qDebug() << __FUNCTION__ <<  "test intersect: tile[xt, yt]:" << xt << ", " << yt;
                        QRect qrectTileFromPrevious(QPoint(xt, yt), QSize(1, 1));
                        if (!qrectTileFromPrevious.intersects(_qrect_TileIndexesMatchingCurrentVisu)) {
                            //qDebug() << __FUNCTION__ <<  qrectTileFromPrevious << "release it (does not intersect with: " << _qrect_TileIndexesMatchingCurrentVisu;
                            releaseTile(xt,yt, _currentZoomLevelImg,"1001");
                        } else {
                            //qDebug() << __FUNCTION__ <<  qrectTileFromPrevious << "don't release it (intersect with: " << _qrect_TileIndexesMatchingCurrentVisu;
                        }

                    }
                }
            } else {
                //qDebug() << __FUNCTION__ <<  "bVisuAndImageIntersects is false";
                //release all tiles
                //qDebug() << __FUNCTION__ <<  "release all tiles";
                int trueBottom = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.height();
                for(int yt     = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.top(); yt < trueBottom; yt++) {
                   int trueRight = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left() + _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.width();
                   for(int xt    = _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.left(); xt < trueRight; xt++) {
                       //qDebug() << __FUNCTION__ <<  "xt, yt:" << xt << ", " << yt;
                       releaseTile(xt,yt, _currentZoomLevelImg, "1017");
                   }
                }
            }
        }
    }

    _infosForTilesRelease._bZoomLevelImg_ChangeTriggered = false;

    //load tiles for visu and set fields to be able to releases the tiles that will become out of visu at the next call
    _infosForTilesRelease._qrectF_previousVisu = currentVisu;
    _infosForTilesRelease._bValid_TileIndexesMatchingPreviousVisu = bVisuAndImageIntersects;

    qDebug() << __FUNCTION__ <<  "bVisuAndImageIntersects = " << bVisuAndImageIntersects;

    bool bNonefailOnLoadTile = true;

    if (bVisuAndImageIntersects) {
        qDebug() << __FUNCTION__ <<  ".setCoords => ";
        _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.setCoords(idxX_firstTile, idxY_firstTile, idxX_lastTile, idxY_lastTile);
        for(int yt = idxY_firstTile; yt <= idxY_lastTile; yt++) {
            for(int xt = idxX_firstTile; xt <= idxX_lastTile; xt++) {
                bNonefailOnLoadTile &= loadTile(xt, yt);
            }
        }
    }

    qint64 elapsedTime = timer.elapsed();
    qDebug() << __FUNCTION__ << "took :" << elapsedTime << " ms (bInside was " << bInside << ")";
    return(bNonefailOnLoadTile);
}

void *TileProvider3::TileData(int tileX,int tileY) {

    if (!_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(_currentZoomLevelImg)) {
        return(nullptr);
    }

    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[_currentZoomLevelImg]; //.value(keyID) returns a const T

    //qDebug() << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    VectorType *ptrVectorType = RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];
    //qDebug() << __FUNCTION__ << "(void*)ptrVectorType = " << (void*)ptrVectorType;

    if (ptrVectorType != nullptr) {
        //qDebug() << __FUNCTION__ << "ptr @ =" << (void*)ptrVectorType;
        //qDebug() << __FUNCTION__ << "ptr->data() @ =" << (void*)ptrVectorType->data();
    } else {
        //qDebug() << __FUNCTION__ << "RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] is nullptr [ " << tileY << " ][ " << tileX << " ]";
        return(nullptr); //@###LP need ignition of and exception, or need to set an error flag to differentiate nullptr about data() and about the vectype* which is very differente error
    }
    return(ptrVectorType->data()); //could be nullptr
}

quint16 TileProvider3::fullImageW() const{
    qDebug() << __FUNCTION__ << "_currentZoomLevelImg =" << _currentZoomLevelImg;
    return(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.width);
}

quint16 TileProvider3::fullImageH() const {
    qDebug() << __FUNCTION__ << "_currentZoomLevelImg =" << _currentZoomLevelImg;
    return(_qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.height);
}


bool TileProvider3::releaseTile(int tileX, int tileY, int zoomLevelImg, const QString& strDebug) {
    qDebug() << __FUNCTION__ << " would release tileXY: " << tileX << tileY << "about zoomLevelImg=" << zoomLevelImg << " from call: " << strDebug;

    //test which should be useless with the pre-filled hash
    if (!_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(zoomLevelImg)) {
        return(false);
    }

    //---------
    QHash<int, S_zoomLevelImgInfos>::const_iterator pair_found_ZLI_zoomLevelImgInfos = _qHashIntSzlii_inputImageInfosByZoomLevel.find(zoomLevelImg);
    if (pair_found_ZLI_zoomLevelImgInfos == _qHashIntSzlii_inputImageInfosByZoomLevel.end()) {
        qDebug() << __FUNCTION__ << "pair_found_ZLI_zoomLevelImgInfos: none";
        return(false);
    }
    bool bInvalidTileXorY = false;
    if (tileX >= pair_found_ZLI_zoomLevelImgInfos.value()._nbTilesinW_fullImage) {
        bInvalidTileXorY = true;
        qDebug() << __FUNCTION__ << "about found: nbTilesinW_fullImage = " << pair_found_ZLI_zoomLevelImgInfos.value()._nbTilesinW_fullImage;
        qDebug() << __FUNCTION__ << " tileX >= ";
    }

    if (tileY >= pair_found_ZLI_zoomLevelImgInfos.value()._nbTilesinH_fullImage) {
        bInvalidTileXorY = true;
        qDebug() << __FUNCTION__ << "about found: nbTilesinH_fullImage = " << pair_found_ZLI_zoomLevelImgInfos.value()._nbTilesinH_fullImage;
        qDebug() << __FUNCTION__ << " tileY >= ";
    }

    if (bInvalidTileXorY) {
        return(false);
    }

    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[zoomLevelImg]; //.value(keyID) returns a const T
    qDebug() << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    if (!RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX]) {
        return(false);
    }
    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX]->clear();
    delete RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];
    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] = nullptr;

    if (zoomLevelImg == _currentZoomLevelImg) {
        _vectTilesIndexZLINewlyReleased_ofCurrentZoomLevelImg.push_back({{tileX, tileY}, zoomLevelImg});
    }

    return(true);
}

bool TileProvider3::loadTile(int tileX, int tileY) {
    return(loadTile(tileX, tileY, _currentZoomLevelImg));
}

bool TileProvider3::loadTile(int tileX, int tileY, int zoomLevelImageToLoad) {

    if (!_cache) {
        qDebug() << __FUNCTION__ << "_cache is nullptr";
        return(false);
    }
    qDebug() << __FUNCTION__ << "_cache @: " << (void*)_cache;

    qDebug() <<  "TileProvider3::" << __FUNCTION__ << "#2000 tile x,y requested: " << tileX << ", " << tileY << "with zoomLevelImageToLoad =" << zoomLevelImageToLoad;

    //qDebug() << " " << __FUNCTION__ << "_vvv8_tilesOfTheImageContent[tileY][tileX].empty()= " << _vvv8_tilesOfTheImageContent[tileY][tileX].empty();
    //qDebug() << " " << __FUNCTION__ << "_vvv8_tilesOfTheImageContent[tileY][tileX].data() = @" << (void*)(_vvv8_tilesOfTheImageContent[tileY][tileX].data());

    //test which should be useless with the pre-filled hash
    if (!_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(zoomLevelImageToLoad)) {
        //qDebug() << " " << __FUNCTION__ << "_qHashIntVvv3u8_tiles.. does not contains value for zoomLevelImageToLoad= " << zoomLevelImageToLoad;
        return(false);
    }

    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[zoomLevelImageToLoad]; //.value(keyID) returns a const T

    qDebug() << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    VectorType *ptrVectorType = RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];

    //qDebug() << __FUNCTION__ << "(void*)ptrVectorType = " << (void*)ptrVectorType;


    if (ptrVectorType == nullptr) {
        //qDebug() << __FUNCTION__ <<  " we will allocate now";
    } else {
        /*if ((RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX])->empty()) {
            qDebug() << __FUNCTION__ <<  "ptr not null but vector empty; this should never happens !";
            return(false);
        } else {*/
            qDebug() << __FUNCTION__ << "#2001 tileX " << tileX << "," << tileY << "not empty so already loaded => return now";
            return(true);
        //}
    }

    //width and height to load:
    int tileWidth  = tileW(tileX);
    int tileHeight = tileH(tileY);

    S_ROIxywh ROIxywh {0,0,0,0};
    ROIxywh._pixelX0 = tileX * _tileWidth;
    ROIxywh._pixelY0 = tileY * _tileHeight;
    ROIxywh._w = tileWidth;
    ROIxywh._h = tileHeight;

    //qDebug() << __FUNCTION__ << "tileWidth = "<< tileWidth << " ; tileHeight = " << tileHeight;

    //@LP Src is dest in this test

    //src
    uint32_t sizeVectSrc = tileWidth * tileHeight * _aboutInputData._nbChannels; //* sizeofTypeForOIIOTypeDesc(_aboutInputData._typeDesc)*/; //consider sizeofTypeForOIIOTypeDesc never return zero

    qDebug() << " " << __FUNCTION__ << "sizeVectSrc = " << sizeVectSrc << "( = " << tileWidth << " * " << tileHeight << " * " << _aboutInputData._nbChannels;

    switch (_aboutInputData._typeDescBasetype) {

        case OIIO::TypeDesc::BASETYPE::FLOAT:
            ptrVectorType = new VectorFloat;
            qDebug() << __FUNCTION__ << "new VectorFloat";
            break;

        case OIIO::TypeDesc::BASETYPE::UINT8:
            ptrVectorType = new VectorUint8;
            qDebug() << __FUNCTION__ << "new VectorUint8";
            break;

        case OIIO::TypeDesc::BASETYPE::INT16:
            ptrVectorType = new VectorSignedInt16;
            qDebug() << __FUNCTION__ << "new VectorSignedInt16";
            break;

        case OIIO::TypeDesc::BASETYPE::UINT16:
            ptrVectorType = new VectorUnsignedInt16;
            qDebug() << __FUNCTION__ << "new VectorUnsignedInt16";
            break;

        default:
            //qDebug() << __FUNCTION__ << "error: invalid sizeofType (" << _aboutInputData._elementSize;
            return(false);
    }


    //qDebug() << __FUNCTION__ << "after allocation: (void*)ptrVectorType = " << (void*)ptrVectorType;

    ptrVectorType->resize(sizeVectSrc);

    //qDebug() << __FUNCTION__ << "(void*)ptrVectorType->data() =" << (void*)(ptrVectorType->data());

    //qDebug() << " " << __FUNCTION__ << "#2003 resized = " << ptrVectorType->size();

    RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] = ptrVectorType;

    qDebug() << " " << __FUNCTION__ << "RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] @" << &(RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX]) << " = @" << RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];

    //qDebug() << " " << __FUNCTION__ << "#2003 resized size = " << sizeVectSrc;

    ImageSpec ImgSpecSrc(tileWidth, tileHeight, _aboutInputData._nbChannels, TypeDesc(_aboutInputData._typeDescBasetype));
    //Ibuf src used for histogram , contrast remap and so on

    //qDebug() << "warp ImageBuf to @" << (void*)RefVvv3u8OfCurrentzoomLevelImg[tileY][tileX].data();

    //qDebug() << "warp ImageBuf to @" << ptrVectorType->data();

    //qDebug() << " " << __FUNCTION__ << "#2004 now get _pixels:";

    int x1 = ROIxywh._pixelX0 + ROIxywh._w;
    int y1 = ROIxywh._pixelY0 + ROIxywh._h;

    //qDebug() << " " << __FUNCTION__ << "get_pixels with roi x0x1 -> y0,y1 :" << ROIxywh._pixelX0 << "," << x1 << "->" << ROIxywh._pixelY0 << "," << y1;
    //qDebug() << " " << __FUNCTION__ << "will put pixels at [x=][y=] .data()= @: [" << tileX << "][" << tileY << (void*)_vvv8_tilesOfTheImageContent[tileY][tileX].data();
    //qDebug() << " " << __FUNCTION__ << "will put pixels at [x=][y=] .data()= @: [" << tileX << "][" << tileY << RefVvv3u8OfCurrentzoomLevelImg[tileY][tileX].data();

    qDebug() << " " << __FUNCTION__ << "_aboutInputData._typeDescBasetype = " << _aboutInputData._typeDescBasetype;

    //get src into vector
    bool bCacheGotPixel = _cache->get_pixels (
        ustring(_qHashIntSzlii_inputImageInfosByZoomLevel.value(zoomLevelImageToLoad)._strFilename), //ustring filename,
        0, 0, //int subimage, int miplevel

        //here is: int xbegin, int xend, int ybegin, int yend,
        // 0, wInputImg, 0, hInputImg,
        //xlocstart, xlocstart+wout_oneQuarter, ylocstart, ylocstart+hout_oneQuarter,
        ROIxywh._pixelX0, x1,
        ROIxywh._pixelY0, y1,

        0, 1,              //int zbegin, int zend,
        0, _aboutInputData._nbChannels,   //int chbegin, int chend,
        _aboutInputData._typeDescBasetype,

        (void*)ptrVectorType->data(),
        AutoStride, AutoStride //stride_t xstride=AutoStride, stride_t ystride=AutoStride
    ); //cache_chbegin, cache_chend = -1

    if (!bCacheGotPixel) {
        qDebug() << "global geterror() = [ " << geterror().c_str() << " ]" << endl;
        qDebug() << "cache-> geterror() = " << _cache->geterror().c_str() << endl;

        delete RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX];
        RefvvVectTypeOfCurrentzoomLevelImg[tileY][tileX] = nullptr;

        //qDebug() << " " << __FUNCTION__ << "#2005" << "bCacheGotPixel failed";
        return(false);
    }

   //qDebug() << " " << __FUNCTION__ << "#2006" << "bCacheGotPixel succeed";

//#define debugSaveOnDiskLoaded
#ifdef debugSaveOnDiskLoaded

    bool bOutputTiff = true;

    if (bOutputTiff) {

        ImgSpecSrc.attribute("compression", "none");
        ImgSpecSrc.attribute("oiio::BitsPerSample", (int)(8*sizeof(float)));  //*channels ?  ##LP

        ImgSpecSrc.attribute("tiff:write exif", int(1));
        //TBC values:
        ImgSpecSrc.attribute("Orientation", (int)1);
        ImgSpecSrc.attribute("XResolution", (float)1.f);
        ImgSpecSrc.attribute("YResolution", (float)1.f);
        //<attrib name="tiff:PhotometricInterpretation" type="int">1</attrib>
        ImgSpecSrc.attribute("tiff:PlanarConfiguration", (int)1);
        //<attrib name="planarconfig" type="string">contig</attrib>
        ImgSpecSrc.attribute("tiff:Compression", (int)1);
        //<attrib name="compression" type="string">none</attrib>
        ImgSpecSrc.attribute("tiff:RowsPerStrip", (int)1);
        ImgSpecSrc.attribute("PixelAspectRatio", (float)1.f);
        ImgSpecSrc.attribute("tiff:subfiletype", (int)0);

        char cOutFilename_IBuf_roi[512];
        sprintf(cOutFilename_IBuf_roi, "ZL%d_x0y0_%d_%d_tox1y1_%d_%d.tif", _currentZoomLevelImg, ROIxywh._pixelX0, ROIxywh._pixelY0, x1, y1);
        bool bSuccessWrite_IBuf_roi = IBufSrc.write(cOutFilename_IBuf_roi, TypeDesc::/*UINT8*/FLOAT);
        qDebug() << " " << __FUNCTION__ << "write success report of about "<< cOutFilename_IBuf_roi << " :" << bSuccessWrite_IBuf_roi;
        if (!bSuccessWrite_IBuf_roi) {
            qDebug() << " " << __FUNCTION__ <<"global geterror() = [ " << geterror().c_str() << " ]";
        }
    } else {
        char cOutFilename_IBuf_roi[512];
        sprintf(cOutFilename_IBuf_roi, "ZL%d_x0y0_%d_%d_tox1y1_%d_%d.bmp", _currentZoomLevelImg, ROIxywh._pixelX0, ROIxywh._pixelY0, x1, y1);
        bool bSuccessWrite_IBuf_roi = IBufSrc.write(cOutFilename_IBuf_roi, TypeDesc::UINT8);
        qDebug() << " " << __FUNCTION__ << "write success report of about "<< cOutFilename_IBuf_roi << " :" << bSuccessWrite_IBuf_roi;
        if (!bSuccessWrite_IBuf_roi) {
            qDebug() << " " << __FUNCTION__ <<"global geterror() = [ " << geterror().c_str() << " ]";
        }
    }
    #endif

    if ( zoomLevelImageToLoad == _currentZoomLevelImg) {
        _vectTilesIndexZLINewlyAllocated_ofCurrentZoomLevelImg.push_back({{tileX, tileY}, zoomLevelImageToLoad});
    } else {
        _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg.push_back({{tileX, tileY}, zoomLevelImageToLoad});
    }

    return(true);
}

TileProvider3::~TileProvider3() {

    qDebug() << "TileProvider3::" << __FUNCTION__;

    quint32 countDeleted = 0;
    int zliCount = 0;
    for (QHash<int, vvVectType>::iterator iterZLI = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.begin();
         iterZLI != _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.end();
         ++iterZLI) {
        qDebug() << __FUNCTION__ << "zli key:" << iterZLI.key();
        int yLoop = 0;
        for (auto& yIter : iterZLI.value()) {
            int xLoop = 0;
            for (auto& xIter : yIter) {
                //qDebug() << __FUNCTION__ << "       x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                if (xIter != nullptr) {
                    qDebug() << __FUNCTION__ << "[done] x, y = " << xLoop << ", " << yLoop << ": &@" << &xIter << " :@" << xIter;
                    delete xIter;
                    xIter = nullptr;
                    countDeleted++;
                }
                xLoop++;
            }
            yLoop++;
        }
        zliCount++;
    }
    qDebug() << __FUNCTION__ << "countDeleted: " << countDeleted;
    qDebug() << __FUNCTION__ << "( zliCount: " << zliCount << " )";

    closeAndDestroy_cache();
}

void TileProvider3::closeAndDestroy_cache() {
    qDebug() <<__FUNCTION__ ;
    if (_cache) {
        _cache->close_all();
        ImageCache::destroy(_cache);
        _cache = nullptr;
    }
}



bool TileProvider3::findTileAndPosInTileForFullImageXYPos(const QPointF& qpfFullImageXYPos, QPoint& qpTileXYIndexes, QPoint& qpXYInTile) {

    if (  (qpfFullImageXYPos.x() < .0 )
        ||(qpfFullImageXYPos.y() < .0 )) {
        qDebug() <<__FUNCTION__  << "qpfFullImageXYPos under 0";
        return(false);
    }

    int wFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.width;
    int hFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(_currentZoomLevelImg)._imageSpec.height;
    if (  (qpfFullImageXYPos.x() >= wFullImage )
        ||(qpfFullImageXYPos.y() >= hFullImage )) {
        qDebug() <<__FUNCTION__  << "qpfFullImageXYPos over full Image w or h";
        return(false);
    }

    qpTileXYIndexes.setX(static_cast<int>( qpfFullImageXYPos.x()/static_cast<double>(_tileWidth ) ));
    qpTileXYIndexes.setY(static_cast<int>( qpfFullImageXYPos.y()/static_cast<double>(_tileHeight) ));

    qpXYInTile.setX(static_cast<int>(qpfFullImageXYPos.x()) - qpTileXYIndexes.x()*_tileWidth);
    qpXYInTile.setY(static_cast<int>(qpfFullImageXYPos.y()) - qpTileXYIndexes.y()*_tileWidth);

    return(true);
}

bool TileProvider3::getPixelValue(const QPoint& qpTileXYIndexes, const QPoint& qpXYInTile, float& fPixelValue) {

    //TileXYIndexes check
    if (  (qpTileXYIndexes.x() < 0)
        ||(qpTileXYIndexes.y() < 0)) {
        qDebug() << __FUNCTION__ << "qpTileXYIndexes error (<00)";
        return(false);
    }

    int nbTilesinW_fullImage = 0;
    int nbTilesinH_fullImage = 0;
    getNbTiles_fullImage(nbTilesinW_fullImage, nbTilesinH_fullImage);

    qDebug() << __FUNCTION__ << "nbTilesinW_fullImage = "<< nbTilesinW_fullImage << " ; nbTilesinH_fullImage = " << nbTilesinH_fullImage;

    if (  (qpTileXYIndexes.x() >= nbTilesinW_fullImage)
        ||(qpTileXYIndexes.y() >= nbTilesinH_fullImage)) {
        qDebug() << __FUNCTION__ << "qpTileXYIndexes error (>=wh)";
        return(false);
    }

    //XYInTile check
    if (  (qpXYInTile.x() < 0)
        ||(qpXYInTile.y() < 0)) {
        qDebug() << __FUNCTION__ << "qpXYInTile error (<00)";
        return(false);
    }

    int tileWidth  = tileW(qpTileXYIndexes.x());
    int tileHeight = tileH(qpTileXYIndexes.y());
    //qDebug() << __FUNCTION__ << "tileWidth = "<< tileWidth << " ; tileHeight = " << tileHeight;
    if (  (qpXYInTile.x() >= tileWidth)
        ||(qpXYInTile.y() >= tileHeight)) {
        qDebug() << __FUNCTION__ << "qpXYInTile error (>=wh)";
        return(false);
    }

    //test which should be useless with the pre-filled hash
    if (!_qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg.contains(_currentZoomLevelImg)) {
        //qDebug() << " " << __FUNCTION__ << "_qHashIntVvv3u8_tiles.. does not contains value for _currentZoomLevelImg= " << _currentZoomLevelImg;
        qDebug() << __FUNCTION__ << "contains _currentZoomLevelImg error";
        return(false);
    }
    vvVectType &RefvvVectTypeOfCurrentzoomLevelImg = _qHashIntVvVectType_TilesOfTheInputImageContentByzoomLevelImg[_currentZoomLevelImg]; //.value(keyID) returns a const T

    //qDebug() << __FUNCTION__ << "&RefvvVectTypeOfCurrentzoomLevelImg = " << (void*)(&RefvvVectTypeOfCurrentzoomLevelImg);

    VectorType *ptrVectorType = RefvvVectTypeOfCurrentzoomLevelImg[qpTileXYIndexes.y()][qpTileXYIndexes.x()];

    //qDebug() << __FUNCTION__ << "(void*)ptrVectorType = " << (void*)ptrVectorType;

    if (ptrVectorType == nullptr) {
        /*qDebug() << __FUNCTION__ << "ptrVectorType == nullptr ('error')";
        qDebug() << __FUNCTION__ << "details: _currentZoomLevelImg = " << _currentZoomLevelImg;
        qDebug() << __FUNCTION__ << "details: qpTileXYIndexes = " << qpTileXYIndexes;
        qDebug() << __FUNCTION__ << "---";*/
        return(false);
    }

    uint32_t offsetInVect_toGetPixelValue = ((qpXYInTile.y() * tileWidth) + qpXYInTile.x()) * _aboutInputData._nbChannels; //* sizeofTypeForOIIOTypeDesc(_aboutInputData._typeDesc)*/; //consider sizeofTypeForOIIOTypeDesc never return zero
    //qDebug() << " " << __FUNCTION__ << "offsetInVect_toGetPixelValue = " << offsetInVect_toGetPixelValue;

    /*qDebug() << __FUNCTION__ << "---";
    qDebug() << __FUNCTION__ << "details: qpTileXYIndexes = " << qpTileXYIndexes;
    qDebug() << __FUNCTION__ << "details: qpXYInTile      = " << qpXYInTile;
    qDebug() << __FUNCTION__ << "-_-";
    qDebug() << __FUNCTION__ << "details: nbTilesinW_fullImage = " << nbTilesinW_fullImage;
    qDebug() << __FUNCTION__ << "details: nbTilesinH_fullImage = " << nbTilesinH_fullImage;
    qDebug() << __FUNCTION__ << "details: tileWidth  = " << tileWidth;
    qDebug() << __FUNCTION__ << "details: tileHeight = " << tileHeight;
    qDebug() << __FUNCTION__ << "details:_currentZoomLevelImg = " << _currentZoomLevelImg;
    qDebug() << __FUNCTION__ << "-_-";
    qDebug() << __FUNCTION__ << "details: offsetInVect_toGetPixelValue = " << offsetInVect_toGetPixelValue;
    qDebug() << __FUNCTION__ << "*( recalculated x,y pixel img: "
             << qpTileXYIndexes.x()*tileWidth + qpXYInTile.x()
             << ", "
             << qpTileXYIndexes.y()*tileHeight + qpXYInTile.y();*/

    //int sizeofType = sizeofTypeForOIIOTypeDesc(_aboutInputData._typeDesc);

    switch (_aboutInputData._typeDescBasetype) {

        case OIIO::TypeDesc::BASETYPE::FLOAT: //  /// ///case sizeof(float):
            fPixelValue = static_cast<float>( *(static_cast<float*>(ptrVectorType->data())+offsetInVect_toGetPixelValue));
            qDebug() << __FUNCTION__ << "FLOAT";
            break;

        case OIIO::TypeDesc::BASETYPE::UINT8: //  /// ///case sizeof(char):
            fPixelValue = static_cast<float>( *(static_cast<unsigned char*>(ptrVectorType->data())+offsetInVect_toGetPixelValue));
            qDebug() << __FUNCTION__ << "Uint8";
            break;

        case OIIO::TypeDesc::BASETYPE::INT16: //
            fPixelValue = static_cast<float>( *(static_cast<signed short int*>(ptrVectorType->data())+offsetInVect_toGetPixelValue));
            qDebug() << __FUNCTION__ << "SignedInt16";
            break;

        case OIIO::TypeDesc::BASETYPE::UINT16: //
            fPixelValue = static_cast<float>( *(static_cast<unsigned short int*>(ptrVectorType->data())+offsetInVect_toGetPixelValue));
            qDebug() << __FUNCTION__ << "UnsignedInt16";
            break;

        default:
            qDebug() << __FUNCTION__ << "invalid sizeofType error (" << _aboutInputData._typeDescBasetype;
            return(false);
    }

    return(true);
}

//This methos is used to load Tiles for the ZLI to display when ready, which is different than the _currentZLI
bool TileProvider3::loadTilesRequestToMatchCurrentVisuAndZLI(const QRectF &currentExposedRectF_atZoomLevelImageToLoad, int zoomLevelImageToLoad) {

    qDebug() << "TileProvider3::" << __FUNCTION__ << "zoomLevelImageToLoad = " << zoomLevelImageToLoad;

    QElapsedTimer timer;
    timer.start();

    _vectTilesIndexZLINewlyAllocated_ofFuturZoomLevelImg.clear();

    //find the tiles (no tiles load) which needs to be loaded for the current visu:
    QRectF qrectf_ImageAreaOfcurrentZLI(
                QPointF {.0, .0},
                QPointF { static_cast<double>(_qHashIntSzlii_inputImageInfosByZoomLevel.value(zoomLevelImageToLoad)._imageSpec.width),
                          static_cast<double>(_qHashIntSzlii_inputImageInfosByZoomLevel.value(zoomLevelImageToLoad)._imageSpec.height)});

    int idxX_firstTile = 0;
    int idxY_firstTile = 0;
    int idxX_lastTile = 0;
    int idxY_lastTile = 0;

    bool bVisuAndImageIntersects = true;

    bool bInside  = qrectf_ImageAreaOfcurrentZLI.contains(currentExposedRectF_atZoomLevelImageToLoad);
    if (bInside) {
        idxX_firstTile = currentExposedRectF_atZoomLevelImageToLoad.left() /_tileWidth;
        idxY_firstTile = currentExposedRectF_atZoomLevelImageToLoad.top()  /_tileHeight;
        idxX_lastTile  = (currentExposedRectF_atZoomLevelImageToLoad.left() + currentExposedRectF_atZoomLevelImageToLoad.width()) / _tileWidth;
        idxY_lastTile  = (currentExposedRectF_atZoomLevelImageToLoad.top() + currentExposedRectF_atZoomLevelImageToLoad.height())/_tileHeight;
    } else {

        int wFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(zoomLevelImageToLoad)._imageSpec.width;
        int hFullImage = _qHashIntSzlii_inputImageInfosByZoomLevel.value(zoomLevelImageToLoad)._imageSpec.height;

        QRectF qrectfFullImage(0, 0, wFullImage, hFullImage);
        QRectF qrectIntersection = currentExposedRectF_atZoomLevelImageToLoad.intersected(qrectfFullImage);
        if (qrectIntersection.isEmpty()) {
            qDebug() << __FUNCTION__ << "fullImage and currentVisu do not intersects";
            bVisuAndImageIntersects = false;

        } else {
            idxX_firstTile = qrectIntersection.left()  /_tileWidth;
            idxY_firstTile = qrectIntersection.top()   /_tileHeight;
            idxX_lastTile  = (qrectIntersection.left()+qrectIntersection.width()) /_tileWidth;
            idxY_lastTile  = (qrectIntersection.top()+qrectIntersection.height()) /_tileHeight;
        }
    }

    qDebug() << __FUNCTION__ << "bVisuAndImageIntersects = " << bVisuAndImageIntersects;

    //qDebug() << __FUNCTION__ <<  "xy firstTile; xy lastTile: " << idxX_firstTile << ", " << idxY_firstTile << "; " << idxX_lastTile << ", " << idxY_lastTile;

    bool bNonefailOnLoadTile = true;

    if (bVisuAndImageIntersects) {
        qDebug() << __FUNCTION__ << "if (bVisuAndImageIntersects) { .setCoords =>";
        _infosForTilesRelease._qrect_TileIndexesMatchingPreviousVisu.setCoords(idxX_firstTile, idxY_firstTile, idxX_lastTile, idxY_lastTile);
        for(int yt = idxY_firstTile; yt <= idxY_lastTile; yt++) {
            for(int xt = idxX_firstTile; xt <= idxX_lastTile; xt++) {                
                bNonefailOnLoadTile &= loadTile(xt, yt, zoomLevelImageToLoad);
            }
        }
    }

    qint64 elapsedTime = timer.elapsed();
    qDebug() << __FUNCTION__ << "took :" << elapsedTime << " ms (bInside was " << bInside << ")";
    return(bNonefailOnLoadTile);

}
