#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsRectItem>

#include <QDebug>

#include "smallimagenav_customgraphicsscene.h"

#include <QPainter>

SmallImageNav_CustomGraphicsScene::SmallImageNav_CustomGraphicsScene(QSize qsizeMax_smallImageNav ,QObject *parent):
    QGraphicsScene(parent),
    _qsizeMax_smallImageNav(qsizeMax_smallImageNav),
    _qrectfSceneImgSmallnav{.0,.0,.0,.0},
    _qGRectItemCurrentlyVisibleInImageView(nullptr),
    _dividerRatioFullImageToSmallNav(1.0),
    _currentLayerDisplayed {-1},
    _qsizeImageNoZoom {1,1}
{

    _stdvect_uptrVectorTypeSmallNavImageDataDisplay.resize(3);
    for (int i = 0; i < 3; i++) {
        _stdvect_uptrVectorTypeSmallNavImageDataDisplay[i].reset(nullptr); //@#LP TBC if mandatory
    }

    _p.setStyle(Qt::PenStyle::SolidLine);
    _p.setColor(Qt::cyan);
    //_p.setCosmetic(true);
    _p.setCosmetic(false);
    _p.setWidthF(1.0);

    _qGRectItemCurrentlyVisibleInImageView = new QGraphicsRectItem();
    _qGRectItemCurrentlyVisibleInImageView->setRect({10,10,20,30});
    _qGRectItemCurrentlyVisibleInImageView->setPen(_p);
    addItem(_qGRectItemCurrentlyVisibleInImageView); //_qGRectItemCurrentlyVisibleInImageView will be deleted by parent

    for (int i = 0; i < 4; i++) {
        _qGLineItemTargetingAreaVisibleInImageView[i] = new QGraphicsLineItem(.0,.0,.0,.0);
        _qGLineItemTargetingAreaVisibleInImageView[i]->setPen(_p);
        addItem(_qGLineItemTargetingAreaVisibleInImageView[i]); //_qGRectItemCurrentlyVisibleInImageView will be deleted by parent
    }

}

void SmallImageNav_CustomGraphicsScene::clear() {

    //do not alter _qsizeMax_smallImageNav;

    _qrectfSceneImgSmallnav = {.0,.0,.0,.0};

    if (_qGRectItemCurrentlyVisibleInImageView) {
        _qGRectItemCurrentlyVisibleInImageView->setRect({.0,.0,.0,.0});
        _qGRectItemCurrentlyVisibleInImageView->hide();
    }

    for (int i = 0; i < 4; i++) {
        _qGLineItemTargetingAreaVisibleInImageView[i]->setLine(.0,.0,.0,.0);
        _qGLineItemTargetingAreaVisibleInImageView[i]->hide();
    }

    _dividerRatioFullImageToSmallNav = 1.0;

    for (int i = 0; i < 3; i++) {
        _stdvect_uptrVectorTypeSmallNavImageDataDisplay[i].reset(nullptr);
    }
    _currentLayerDisplayed = -1;
}


QSize SmallImageNav_CustomGraphicsScene::getSizeForWindowSmallImageNav() {
    QSize qsz {
        static_cast<int>(_qrectfSceneImgSmallnav.width()),
        static_cast<int>(_qrectfSceneImgSmallnav.height())
    };
    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) : qsz = " << qsz;
    return(qsz);
}

void SmallImageNav_CustomGraphicsScene::switchToLayer(int ieLA) {
    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene)";
    if (ieLA == _currentLayerDisplayed) {
        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) not a switch (_currentLayerDisplayed = " << _currentLayerDisplayed;
        return;
    }

    _currentLayerDisplayed = ieLA;

    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) _currentLayerDisplayed <= ieLA = " << _currentLayerDisplayed;

    invalidate();
}

bool SmallImageNav_CustomGraphicsScene::createImageBackgroundAndInit(
        S_zoomLevelImgInfos SZLIInfosJustAboveSmallImageNavMaxSize, QSize qsizeImageNoZoom, int idxLayer) {

    if ((idxLayer < 0)||(idxLayer > 2)) {
        return(false);
    }

    _qsizeImageNoZoom = qsizeImageNoZoom;

    qreal widthRatio  = static_cast<qreal>(_qsizeMax_smallImageNav.width())  / static_cast<qreal>(SZLIInfosJustAboveSmallImageNavMaxSize._imageSpec.width);
    qreal heightRatio = static_cast<qreal>(_qsizeMax_smallImageNav.height()) / static_cast<qreal>(SZLIInfosJustAboveSmallImageNavMaxSize._imageSpec.height);

    //the image used to build the small image nav is smaller than the small image nav size
    //In this case we consider that showing the small image nav is not useful.
    //But to avoid specific handling of action menu disable/re-enable of small image nav window about this very specific case,
    //we feed the  small image nav window with this small image
    if ((widthRatio >= 1.0) && (heightRatio >=1.0)) {
        widthRatio = 1.0;
        heightRatio = 1.0;
    }

    qreal ratioToUse = widthRatio < heightRatio ? widthRatio : heightRatio;
    ratioToUse = 1.0 / ratioToUse;

    //use int size for image blackground and scene
    QSize qsizeSceneImgSmallnav {
        static_cast<int>(SZLIInfosJustAboveSmallImageNavMaxSize._imageSpec.width/ratioToUse),
        static_cast<int>(SZLIInfosJustAboveSmallImageNavMaxSize._imageSpec.height/ratioToUse)
    };

    qDebug() << __FUNCTION__ << "set qsizeSceneImgSmallnav to :" << qsizeSceneImgSmallnav;
    qDebug() << __FUNCTION__ << " ratioToUse : " << ratioToUse;

    qDebug() << __FUNCTION__ << " _qsizeSceneImgSmallnav:"
             << qsizeSceneImgSmallnav.width() << " x " << qsizeSceneImgSmallnav.height();

    _qrectfSceneImgSmallnav.setTopLeft({.0,.0});
    _qrectfSceneImgSmallnav.setSize({static_cast<qreal>(qsizeSceneImgSmallnav.width()),
                                     static_cast<qreal>(qsizeSceneImgSmallnav.height())});

    qDebug() << __FUNCTION__ << "set _qrectfSceneImgSmallnav to :" << _qrectfSceneImgSmallnav;

    //@#LP add flag to reject trying to build additionnal small image for layers if _qrectfSceneImgSmallnav is invalid

    if ((!qsizeSceneImgSmallnav.width())||(!qsizeImageNoZoom.width())) {
        qDebug() << __FUNCTION__ << " error: width about qsizeSceneImgSmallnav or qsizeImageNoZoom is 0. set _dividerRatio to 1.0";
        _dividerRatioFullImageToSmallNav = 1.0;
    } else {
        _dividerRatioFullImageToSmallNav = static_cast<qreal>(qsizeImageNoZoom.width())/static_cast<qreal>(qsizeSceneImgSmallnav.width());
    }

    qDebug() << __FUNCTION__ << " _dividerRatioFullImageToSmallNav = " << _dividerRatioFullImageToSmallNav;

    setSceneRect(_qrectfSceneImgSmallnav);

    bool bImageBackgroundGenerated = generateImageBackground(SZLIInfosJustAboveSmallImageNavMaxSize._strFilename, idxLayer);

    if (bImageBackgroundGenerated) {
        if (_qGRectItemCurrentlyVisibleInImageView) {
            _qGRectItemCurrentlyVisibleInImageView->setRect({.0,.0,.0,.0});
            _qGRectItemCurrentlyVisibleInImageView->show();
        }        
        for(int i = 0; i < 4; i++) {
            if (_qGLineItemTargetingAreaVisibleInImageView[i]) {
                _qGLineItemTargetingAreaVisibleInImageView[i]->setLine(.0,.0,.0,.0);
                _qGLineItemTargetingAreaVisibleInImageView[i]->show();
            }
        }
        if (_currentLayerDisplayed == -1) {
            _currentLayerDisplayed = idxLayer;
        }
    }

    return(bImageBackgroundGenerated);

}

bool  SmallImageNav_CustomGraphicsScene::createAdditionnalImageAndInit(/*S_zoomLevelImgInfos SZLIInfosJustAboveSmallImageNavMaxSize, QSize qsizeImageNoZoom*/const string& strImageFilename, int idxLayer) {

    if ((idxLayer < 0)||(idxLayer > 2)) {
        return(false);
    }

    qDebug() << __FUNCTION__ << " _qrectfSceneImgSmallnav:" << _qrectfSceneImgSmallnav;

    qDebug() << __FUNCTION__ << " _dividerRatioFullImageToSmallNav = " << _dividerRatioFullImageToSmallNav;

    bool bImageBackgroundGenerated = generateImageBackground(strImageFilename, idxLayer);

    return(bImageBackgroundGenerated);
}


#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include <OpenImageIO/imagebufalgo.h>

#include "vecttype.h"

bool SmallImageNav_CustomGraphicsScene::generateImageBackground(const string& strFilename, int idxLayer) {

    auto in = ImageInput::open(strFilename);
    if (!in) {
        qDebug() << __FUNCTION__ << " error: one or more cache->attribute set failed";
        qDebug() << __FUNCTION__ << " oiio error msg:" << geterror().c_str(); //@#LP check if available after an ImageInput::open
        return(false);
    }

    const ImageSpec &inputImgSpec = in->spec();
    int xres = inputImgSpec.width;
    int yres = inputImgSpec.height;
    int channels = inputImgSpec.nchannels;

    unique_ptr<VectorType> uptrVectorTypeInputImageData;
    unique_ptr<VectorType> uptrVectorTypeResizedImageData;

    //@#LP duplicated code here and there:
    //-----------
    OIIO::TypeDesc::BASETYPE typeDescBasetype = static_cast<OIIO::TypeDesc::BASETYPE>(inputImgSpec.format.basetype);

    bool bStopDueToError = false;
    switch (typeDescBasetype) {

            case OIIO::TypeDesc::BASETYPE::UINT8:
                uptrVectorTypeInputImageData = make_unique<VectorUint8>();
                uptrVectorTypeResizedImageData = make_unique<VectorUint8>();
                qDebug() << __FUNCTION__ << "new VectorUint8";
               break;

            case OIIO::TypeDesc::BASETYPE::INT16: //signed 16 bits integer
                uptrVectorTypeInputImageData = make_unique<VectorSignedInt16>();
                uptrVectorTypeResizedImageData = make_unique<VectorSignedInt16>();
                qDebug() << __FUNCTION__ << __LINE__ << "new VectorSignedInt16";
               break;

            case OIIO::TypeDesc::BASETYPE::UINT16: //unsigned 16 bits integer
                uptrVectorTypeInputImageData = make_unique<VectorUnsignedInt16>();
                uptrVectorTypeResizedImageData = make_unique<VectorUnsignedInt16>();
                qDebug() << __FUNCTION__ << __LINE__ << "new VectorUnsignedInt16";
               break;

            case OIIO::TypeDesc::BASETYPE::FLOAT:
                uptrVectorTypeInputImageData = make_unique<VectorFloat>();
                uptrVectorTypeResizedImageData = make_unique<VectorFloat>();
                qDebug() << __FUNCTION__ << "new VectorFloat";
                break;

            default:
                qDebug() << __FUNCTION__ << "error: invalid typeDescBasetype:" << typeDescBasetype;
                bStopDueToError = true;
                break;
    }
    if  (bStopDueToError) {
        qDebug() << __FUNCTION__ << " error: unknown typeDescBasetypeone: "<< typeDescBasetype;
        in->close ();
        return(false);
    }

    //-------------

    uint32_t sizeInputImageData = xres * yres * channels;
    uptrVectorTypeInputImageData->resize(sizeInputImageData);
    qDebug() << "uptrVectorTypeInputImageData size: " << uptrVectorTypeInputImageData->size();
    qDebug() << __FUNCTION__ << "uptrVectorTypeInputImageData @ = " << (void*)uptrVectorTypeInputImageData->data();

    int subimage = 0;
    int miplevel = 0;
    int chbegin = 0;
    int chend = channels-1;

    bool bReadImageReport = in->read_image(
                subimage, miplevel,
                chbegin, chend,
                typeDescBasetype,
                (void *)uptrVectorTypeInputImageData->data(),
                AutoStride, AutoStride, AutoStride,
                /*ProgressCallback progress callback=*/NULL,
                /*void *progress callback data=*/NULL);

    if  (!bReadImageReport) {
        qDebug() << __FUNCTION__ << " error: reading image " << strFilename.c_str();
        in->close ();
        return(bReadImageReport);
    }

    in->close ();

    //#define debugSaveOnDiskLoaded_0
    #ifdef debugSaveOnDiskLoaded_0
    char cOutFilename0[512];
    sprintf(cOutFilename0, "/home/laurent/log/loaded.tiff");
    std::unique_ptr<ImageOutput> out = ImageOutput::create (cOutFilename0);
    if (out) {
        ImageSpec ImageSpecOut (xres, yres, channels, TypeDesc(typeDescBasetype));

        ImageSpecOut.attribute("compression", "none");
        ImageSpecOut.attribute("oiio::BitsPerSample", (int)(8*inputImgSpec.format.size())); //(8*sizeof(float)));  // *channels ?? <=> @####LP RGB 3 channels ?
        qDebug() << __FUNCTION__ << "oiio::BitsPerSample as :" << (int)(8*inputImgSpec.format.size());
        ImageSpecOut.attribute("tiff:write exif", int(1));
        //TBC values:
        ImageSpecOut.attribute("Orientation", (int)1);
        ImageSpecOut.attribute("XResolution", (float)1.f);
        ImageSpecOut.attribute("YResolution", (float)1.f);
        //<attrib name="tiff:PhotometricInterpretation" type="int">1</attrib>
        ImageSpecOut.attribute("tiff:PlanarConfiguration", (int)1);
        //<attrib name="planarconfig" type="string">contig</attrib>
        ImageSpecOut.attribute("tiff:Compression", (int)1);
        //<attrib name="compression" type="string">none</attrib>
        ImageSpecOut.attribute("tiff:RowsPerStrip", (int)1);
        ImageSpecOut.attribute("PixelAspectRatio", (float)1.f);
        ImageSpecOut.attribute("tiff:subfiletype", (int)0);

        bool bOpened = out->open(cOutFilename0, ImageSpecOut);
        qDebug() << " " << __FUNCTION__ << "bOpened" << bOpened;
        if (bOpened) {
            bool bWrote = out->write_image (typeDescBasetype, uptrVectorTypeInputImageData->data());
            qDebug() << " " << __FUNCTION__ << "bWrote" << bWrote;
        }
        out->close ();
    } else {
        qDebug() << " " << __FUNCTION__ << "ImageOutput::create failed (" << cOutFilename0;
    }
    #endif

    uint32_t sizeResizedImage = _qrectfSceneImgSmallnav.width() * _qrectfSceneImgSmallnav.height() * channels;
    uptrVectorTypeResizedImageData->resize(sizeResizedImage);

    //
    //resize to smallImageNav size, staying on the same typeDescBasetype
    //
    int nbThreadsToUse = 1;
    bool bResizeReport = false;


    ImageSpec ImageSpec_warpedBlockData     (xres , yres , channels, TypeDesc(typeDescBasetype));
    ImageSpec ImageSpec_warpedResampledBlock(_qrectfSceneImgSmallnav.width(), _qrectfSceneImgSmallnav.height(), channels, TypeDesc(typeDescBasetype));

    //warp vector to ImageBuf
    ImageBuf ImageBufBlock   (ImageSpec_warpedBlockData, (void *)uptrVectorTypeInputImageData->data());

    //resample to destBlock
    ImageBuf ImageBufResampledBlock(ImageSpec_warpedResampledBlock, (void*)uptrVectorTypeResizedImageData->data());

    //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => gaussian 3.0 ===> 6m30,940s (SSD)
    //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => default ========>           (SSD)
    //string_view oiioStrView_resizeFilter( "cubic");

    ROI roiResampled(
      0, _qrectfSceneImgSmallnav.width(),
      0, _qrectfSceneImgSmallnav.height(),
      0, 1, //z
      0, channels);
    //bResizeReport = ImageBufAlgo::resize(ImageBufResampledBlock, ImageBufBlock, oiioStrView_resizeFilter, 0, roiResampled);
    bResizeReport = ImageBufAlgo::resample(ImageBufResampledBlock, ImageBufBlock, true, roiResampled, nbThreadsToUse);


    qDebug() << __FUNCTION__ <<  "bResizeReport: " << bResizeReport;

    if (!bResizeReport) {
        qDebug() << "error: resize failed";
        qDebug() << "global geterror() = " << geterror().c_str();
        return(bResizeReport);
    }

    //free input data
    uptrVectorTypeInputImageData.reset();


//#define debugSaveOnDiskLoaded_1
#ifdef debugSaveOnDiskLoaded_1
    char cOutFilename1[512];
    sprintf(cOutFilename1, "/home/laurent/log/resized.tiff");
    /*
    bool bSuccessWrite1 = ImageBufResampledBlock.write(cOutFilename1, typeDescBasetype);
    qDebug() << " " << __FUNCTION__ << "write success report of about "<< cOutFilename1 << " :" << bSuccessWrite1;
    if (!bSuccessWrite1) {
        qDebug() << " " << __FUNCTION__ <<"global geterror() = [ " << geterror().c_str() << " ]";
    }*/

    std::unique_ptr<ImageOutput> out1 = ImageOutput::create(cOutFilename1);
    if (out1) {
        ImageSpec ImageSpecOut1 (_qrectfSceneImgSmallnav.width(), _qrectfSceneImgSmallnav.height(), channels, TypeDesc(typeDescBasetype));

        ImageSpecOut1.attribute("compression", "none");
        ImageSpecOut1.attribute("oiio::BitsPerSample", (int)(8*ImageSpec_warpedResampledBlock.format.size())); //(8*sizeof(float)));  // *channels ?? <=> @###LP RGB 3 channels ?
        qDebug() << __FUNCTION__ << "oiio::BitsPerSample as :" << (int)(8*ImageSpec_warpedResampledBlock.format.size());
        ImageSpecOut1.attribute("tiff:write exif", int(1));
        //TBC values:
        ImageSpecOut1.attribute("Orientation", (int)1);
        ImageSpecOut1.attribute("XResolution", (float)1.f);
        ImageSpecOut1.attribute("YResolution", (float)1.f);
        //<attrib name="tiff:PhotometricInterpretation" type="int">1</attrib>
        ImageSpecOut1.attribute("tiff:PlanarConfiguration", (int)1);
        //<attrib name="planarconfig" type="string">contig</attrib>
        ImageSpecOut1.attribute("tiff:Compression", (int)1);
        //<attrib name="compression" type="string">none</attrib>
        ImageSpecOut1.attribute("tiff:RowsPerStrip", (int)1);
        ImageSpecOut1.attribute("PixelAspectRatio", (float)1.f);
        ImageSpecOut1.attribute("tiff:subfiletype", (int)0);

        bool bOpened1 = out1->open(cOutFilename1, ImageSpecOut1);
        qDebug() << " " << __FUNCTION__ << "bOpened" << bOpened1;
        if (bOpened1) {
            bool bWrote1 = out1->write_image (typeDescBasetype, uptrVectorTypeResizedImageData->data());
            qDebug() << " " << __FUNCTION__ << "bWrote1" << bWrote1;
        }
        /*
        bool bSuccessWrite1 = ImageBufResampledBlock.write(out1.get());
        if (!bSuccessWrite1) {
            qDebug() << " " << __FUNCTION__ <<"write1 global geterror() = [ " << geterror().c_str() << " ]";
        }*/
        out1->close();

    } else {
        qDebug() << " " << __FUNCTION__ << "out1 create failed (" << cOutFilename1;
    }
#endif


    //
    //contrast remap
    //
    struct S_nbChannelsAndTypeDesc_forOIIOUsage {
        int _nbChannels;
        OIIO::TypeDesc::BASETYPE _typeDescBasetype;
    };

    S_nbChannelsAndTypeDesc_forOIIOUsage aboutDisplayData;
    aboutDisplayData._nbChannels = 1; //3;
    aboutDisplayData._typeDescBasetype = OIIO::TypeDesc::UINT8;

    ImageSpec ImgSpecDest(_qrectfSceneImgSmallnav.width(),
                          _qrectfSceneImgSmallnav.height(),
                          aboutDisplayData._nbChannels,
                          TypeDesc(aboutDisplayData._typeDescBasetype));

    qDebug() << "_aboutDisplayData._typeDescBasetype = " << aboutDisplayData._typeDescBasetype;
    qDebug() << "TypeDesc(_aboutDisplayData._typeDescBasetype) = " << TypeDesc(aboutDisplayData._typeDescBasetype);

    _stdvect_uptrVectorTypeSmallNavImageDataDisplay[idxLayer] = make_unique<VectorUint8>();


    uint32_t sizeSmallNavImageDataDisplay = _qrectfSceneImgSmallnav.width() * _qrectfSceneImgSmallnav.height() * aboutDisplayData._nbChannels; //@#LP: _typeDescBasetype not used

    _stdvect_uptrVectorTypeSmallNavImageDataDisplay[idxLayer]->resize(sizeSmallNavImageDataDisplay);


    void *dataPtrDest = _stdvect_uptrVectorTypeSmallNavImageDataDisplay[idxLayer]->data();

    qDebug() << __FUNCTION__ << "dataPtrDest = " << (void*)dataPtrDest;
    if (dataPtrDest == nullptr) {
        qDebug() << "TPWOD " << __FUNCTION__ << "error: got dataPtrDest = TileData(...) as nullptr";
        return(false);
    }

    ImageBuf IBufDest(ImgSpecDest, dataPtrDest);
    /*qDebug() << __FUNCTION__ << "about dest (_aboutDisplayData): w, h, nchannels, typeDescBasetype:"
             << tileWidth << ", "
             << tileHeight << ", "
             << _aboutDisplayData._nbChannels << ", "
             << _aboutDisplayData._typeDescBasetype;*/

    float fMin =  900000.0; // set on arbitrary but surely out value
    float fMax = -900000.0; // set on arbitrary but surely out value

    ImageBufAlgo::PixelStats pixStats;
    bool bStatsComputeSuccess = ImageBufAlgo::computePixelStats (pixStats, ImageBufResampledBlock/*, roiToConsider*/);
    if (!bStatsComputeSuccess) {
        qDebug() << __FUNCTION__ << "error: computePixelStats failed";
        //@#LP add OIIO's get errors functions
        qDebug() << "TPWOD " << "global geterror() = [ " << geterror().c_str() << " ]";
        return(false);
    }

    for (int chan = 0; chan < ImageBufResampledBlock.nchannels(); ++chan) {
        qDebug() << __FUNCTION__ << "channel #" << chan;
        qDebug() << __FUNCTION__ << "  min = " << pixStats.min[chan];
        qDebug() << __FUNCTION__ << "  max = " << pixStats.max[chan];
        qDebug() << __FUNCTION__ << "nancount = " << pixStats.nancount[chan];
        qDebug() << __FUNCTION__ << "infcount = " << pixStats.infcount[chan];
    }

    //@LP get the min max of the first channel
    fMin = pixStats.min[0];
    fMax = pixStats.max[0];

    qDebug() << __FUNCTION__ << "min & max about channel #0: " << fMin << ", " <<  fMax;

    float minOut = 0.0;
    float maxOut = 1.0;

    bool bContrastRemapSuccess = ImageBufAlgo::contrast_remap(
          IBufDest,//ImageBuf &dst,
          ImageBufResampledBlock, //ImageBuf_contrastRemapped,//const ImageBuf &src,
          fMin, fMax, //cspan<float> black=0.0f, cspan<float> white=1.0f,
          minOut, maxOut, //0, 1,//cspan<float> min=0.0f, cspan<float> max=1.0f,
          1.0f, 0.5f);

    if (!bContrastRemapSuccess) {
        qDebug() << __FUNCTION__ << "fail on contrast_remap(...); geterror() = [ " << geterror().c_str() << " ]" << endl;
        return(false);
    }

    qDebug() << __FUNCTION__ << "contrast_remap done";

//#define debugSaveOnDiskLoaded_2
    #ifdef debugSaveOnDiskLoaded_2
    char cOutFilename2[512];
    sprintf(cOutFilename2, "/home/laurent/log/ctrastremap_layer%d.png", idxLayer);
    bool bSuccessWrite2 = IBufDest.write(cOutFilename2, /*TypeDesc::UINT8*/TypeUnknown, string_view("png"));
    qDebug() << " " << __FUNCTION__ << "2 write success report of about "<< cOutFilename2 << " :" << bSuccessWrite2;
    if (!bSuccessWrite2) {
        qDebug() << " " << __FUNCTION__ <<"2 global geterror() = [ " << geterror().c_str() << " ]";
    }
#endif

    return(true);
}



void SmallImageNav_CustomGraphicsScene::slot_currentExposedRectFChanged(QRectF exposed, int currentZoomLevelImg) {

    if (!_qGRectItemCurrentlyVisibleInImageView) {
        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) _qGRectItemCurrentlyVisibleInImageView is nullptr";
        return;
    }

    //qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) exposed:" << exposed;
    //qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) currentZoomLevelImg:" << currentZoomLevelImg;

    QRectF qrectLocationVisibleArea_fullImage;
    qrectLocationVisibleArea_fullImage.setTopLeft({
        exposed.topLeft().x()*currentZoomLevelImg,
        exposed.topLeft().y()*currentZoomLevelImg});

    qrectLocationVisibleArea_fullImage.setSize({
        exposed.width() *currentZoomLevelImg,
        exposed.height()*currentZoomLevelImg});

    //qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) qrectLocationVisibleArea_fullImage:" << qrectLocationVisibleArea_fullImage;

    //qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) _dividerRatioFullImageToSmallNav = " << _dividerRatioFullImageToSmallNav;


    QRectF qrectLocationVisibleAreaInImgSmallnav;
    qrectLocationVisibleAreaInImgSmallnav.setTopLeft({qrectLocationVisibleArea_fullImage.topLeft().x()/_dividerRatioFullImageToSmallNav,
                                                      qrectLocationVisibleArea_fullImage.topLeft().y()/_dividerRatioFullImageToSmallNav});

    qrectLocationVisibleAreaInImgSmallnav.setSize({ qrectLocationVisibleArea_fullImage.width() /_dividerRatioFullImageToSmallNav,
                                                    qrectLocationVisibleArea_fullImage.height()/_dividerRatioFullImageToSmallNav});

    //qDebug() << __FUNCTION__ << "qrectLocationVisibleAreaInImgSmallnav:" << qrectLocationVisibleAreaInImgSmallnav;


    _qGRectItemCurrentlyVisibleInImageView->setRect(qrectLocationVisibleAreaInImgSmallnav);

    //top line
    if (_qGLineItemTargetingAreaVisibleInImageView[0]) {
        _qGLineItemTargetingAreaVisibleInImageView[0]->setLine(qrectLocationVisibleAreaInImgSmallnav.center().x(), -1,
                                                               qrectLocationVisibleAreaInImgSmallnav.center().x(), qrectLocationVisibleAreaInImgSmallnav.top());

    }

    //bottom line
    if (_qGLineItemTargetingAreaVisibleInImageView[1]) {               
        _qGLineItemTargetingAreaVisibleInImageView[1]->setLine(qrectLocationVisibleAreaInImgSmallnav.center().x(), qrectLocationVisibleAreaInImgSmallnav.bottom(),
                                                               qrectLocationVisibleAreaInImgSmallnav.center().x(), _qrectfSceneImgSmallnav.bottom());
    }

    //left line
    if (_qGLineItemTargetingAreaVisibleInImageView[2]) {
        _qGLineItemTargetingAreaVisibleInImageView[2]->setLine(-1, qrectLocationVisibleAreaInImgSmallnav.center().y(),
                                                               qrectLocationVisibleAreaInImgSmallnav.left(), qrectLocationVisibleAreaInImgSmallnav.center().y());
    }

    //right line
    if (_qGLineItemTargetingAreaVisibleInImageView[3]) {
        _qGLineItemTargetingAreaVisibleInImageView[3]->setLine(qrectLocationVisibleAreaInImgSmallnav.right(), qrectLocationVisibleAreaInImgSmallnav.center().y(),
                                                               _qrectfSceneImgSmallnav.right(), qrectLocationVisibleAreaInImgSmallnav.center().y());
    }
}


SmallImageNav_CustomGraphicsScene::~SmallImageNav_CustomGraphicsScene() {

}

void SmallImageNav_CustomGraphicsScene::drawBackground(QPainter *painter, const QRectF &exposed) {

    if (_currentLayerDisplayed == -1) {
        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) if (_currentLayerDisplayed == -1) {";
        return;
    }

    if (!_stdvect_uptrVectorTypeSmallNavImageDataDisplay[_currentLayerDisplayed]) {
        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) if ( !_uptrVectorTypeSmallNavImageDataDisplay)";
        return;
    }

    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) _currentLayerDisplayed <= ieLA = " << _currentLayerDisplayed;

    QImage::Format qImageFormat = QImage::Format_Indexed8;

    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) _stdvect_uptrVectorTypeSmallNavImageDataDisplay[_currentLayerDisplayed]->data() = @"
             << _stdvect_uptrVectorTypeSmallNavImageDataDisplay[_currentLayerDisplayed]->data();

    QImage *tileImg = new QImage(static_cast<const uchar*>(
                                                                              _stdvect_uptrVectorTypeSmallNavImageDataDisplay[_currentLayerDisplayed]->data()),
                           _qrectfSceneImgSmallnav.width(),
                           _qrectfSceneImgSmallnav.height(),

                            _qrectfSceneImgSmallnav.width(), //nb bytes per line; very important field to use to have correct display of last rows and last columns, not power of 8 or something like that
                           qImageFormat);

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform,false);

    painter->drawPixmap(QPoint{0,0}, QPixmap::fromImage(*tileImg));

    painter->restore();

    delete tileImg;
    tileImg = nullptr;
}

//void SmallImageNav_CustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
void SmallImageNav_CustomGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {

    qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene)";

    bool bLeftButtonPressed = event->button() == Qt::LeftButton;
    if (bLeftButtonPressed) {

        QPointF event_scenePos = event->scenePos();
        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) event_scenePos =" << event_scenePos;

        QPointF mouseLocationInSceneNoZLI = event_scenePos*_dividerRatioFullImageToSmallNav;

        qDebug() << __FUNCTION__ << "(SmallImageNav_CustomGraphicsScene) mouseLocationInSceneNoZLI =" << mouseLocationInSceneNoZLI;

        event->accept();

        emit signal_setSceneCenterNoZLI(mouseLocationInSceneNoZLI);
        return;
    }

   QGraphicsScene::mousePressEvent(event);
}

#include <QKeyEvent>
#include "../imageScene/S_booleanKeyStatus.hpp"

void SmallImageNav_CustomGraphicsScene::keyPressEvent(QKeyEvent *event) {

    if (event->count() > 1) { event->accept(); return; }
    if (event->isAutoRepeat()) { event->accept(); return; }

    //Some other windows react on CtrlKeys+keys. Keys which is used here without CtrlKeys (S, D, G for example)
    //to avoid unwanted action, filter action using CtrlKeys status
    S_booleanKeyStatus SbkeyStatus;
    if (!SbkeyStatus._bNonePressed) {
        event->accept();
        return;
    }

    bool bHandled = false;

    switch (event->key()) {

    case Qt::Key_Space:
        emit signal_setGlobalZoomFactorTo1();
        bHandled = true;
        break;

    /*case Qt::Key_C: //not availble from small image nav
        emit signal_centerOnMouseCursorLocation();
        bHandled = true;
        break;
    */

    case Qt::Key_F:
        emit signal_fitImageInCurrentWindowSize();
        bHandled = true;
        break;

    //'H' and 'K' keyboard keys are around 'J'.
    case Qt::Key_H://zoom minus (go outside image). 'H' as 'Higher'
    //case Qt::Key_K://zoom minus (go outside image). added key to do not care which side is from zoom plus (the 'J' key)
        emit signal_setZoomStepIncDec(false);
        bHandled = true;
        break;

    case Qt::Key_J: //zoom plus (go inside image). The 'J' keyboard key has a mark (like the 'F' keyboard key)
        emit signal_setZoomStepIncDec(true);
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

