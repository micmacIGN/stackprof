#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>

#include <OpenImageIO/imagecache.h>

#include <OpenImageIO/imagebufalgo.h> //@LP for stats test

#include <math.h> //@LP for NAN

#include <string>
using namespace std;

using namespace OIIO;

#include "ZoomOutLevelImageBuilder.h"

#include <QDir>
#include <QString>

#include <QDebug>

#include <QElapsedTimer> //for basic time spent measurement

#include "iojsoninfos.h"

#include "../vecttype.h"

#include "InputImageFormatChecker.h"

#include "toolbox/toolbox_pathAndFile.h"

//@#LP can go to toolbox_math
int ZoomOutLevelImageBuilder::nearestPow2(int v) {
  if (v && !(v&(v-1))) {
    return(v);
  }
  double value = pow(2, round(log(v)/log(2)));
  return(static_cast<int>(value));
}

ZoomOutLevelImageBuilder::ZoomOutLevelImageBuilder(const QString& strRootImage, const QString& strDirRootZLIStorage, QObject *parent):
    QObject(parent),
    _strInputImageInfos_jsonFile("inputImageInfos.json"),
    _qstrRootImage(strRootImage),
    _qstrExtension(""),
    _qstrDirRootZLIStorage(strDirRootZLIStorage),
    _qStrFullPathDirnameForZLI(""),
    _infosAboutHigherZoomLevel {0,0,0} {

}

bool ZoomOutLevelImageBuilder::absoluteDirNameForRootImageAndID(const QString& qstrRootImage, qlonglong idForDir, QString& qStrFullPathDirnameForZLI) {

    qDebug() << __FUNCTION__;

    QString qStrPathAndFilenameRootImage;

    qStrPathAndFilenameRootImage = getPathAndFilenameFromPotentialSymlink(qstrRootImage);
    if (!fileExists(qStrPathAndFilenameRootImage)) {
        qDebug() << __FUNCTION__ << "fileExists() said false about: " << qStrPathAndFilenameRootImage;
        return(false);
    }

    QFileInfo fileInfoFinalTarget(qStrPathAndFilenameRootImage);
    QString qStrFinalTargetFilenameRootImage = fileInfoFinalTarget.fileName();

    //_LP: only one dot ('.') permitted in filename
    //QString qSstrBaseFilenameNoneExtension = fileInfoFinalTarget.baseName();  // example: "archive.tar.gz" => baseName = "archive" (".tar.gz" removed)
    _qstrExtension = fileInfoFinalTarget.completeSuffix();  // ext = "tar.gz"

    //_qstrDirRootZLIStorage = /tmp"
    //_qstrRootImage         = /xxx/yyy/zzz/filename.ext
    //=>
    //qStrFinalTargetFilenameRootImage = filename.ext
    //qStrDirnameForZLI                = filename.ext_ZLI
    //_qStrFullPathDirnameForZLI = /tmp/filename.ext_ZLI

    QString qStrDirnameForZLI = qStrFinalTargetFilenameRootImage + "_ZLI_id_" + QString::number(idForDir);
    qStrFullPathDirnameForZLI = _qstrDirRootZLIStorage + QDir::separator() + qStrDirnameForZLI; // + QDir::separator();

    qDebug() << __FUNCTION__ << "qStrFinalTargetFilenameRootImage=" << qStrFinalTargetFilenameRootImage;
    qDebug() << __FUNCTION__ << "qStrDirnameForZLI=" << qStrDirnameForZLI;
    qDebug() << __FUNCTION__ << "_qStrFullPathDirnameForZLI = " << _qStrFullPathDirnameForZLI;
    qDebug() << __FUNCTION__ << "qStrFullPathDirnameForZLI = " << qStrFullPathDirnameForZLI;
    qDebug() << __FUNCTION__ << "return(true);";

    return(true);
}




bool ZoomOutLevelImageBuilder::makeStorageDirectory(qlonglong idForDir) {

    qDebug() << __FUNCTION__ << "_qstrDirRootZLIStorage=" << _qstrDirRootZLIStorage;

    _qStrFullPathDirnameForZLI.clear();

    //base directory to create the directory dedicated to the ZLI of the image
    //has to exist
    QDir qdirRootZLIStorage(_qstrDirRootZLIStorage);
    if (!qdirRootZLIStorage.exists()) {
        qDebug() << __FUNCTION__ << "_qstrDirRootZLIStorage=" << _qstrDirRootZLIStorage;
        return(false);
    }        

    bool bReport = absoluteDirNameForRootImageAndID(_qstrRootImage, idForDir, _qStrFullPathDirnameForZLI);
    if (!bReport) {
        qDebug() << __FUNCTION__ << " failed in absoluteDirNameForRootImageAndID for _qstrRootImage = " << _qstrRootImage;
        return(false);
    }

    QDir qdirZLIStorage(_qStrFullPathDirnameForZLI);

    //create the ZLIStorage directory if it does not exists
    if (!qdirZLIStorage.exists()) {
        if (!qdirRootZLIStorage.mkdir(_qStrFullPathDirnameForZLI)) {
            qDebug() << __FUNCTION__ << "error: can not mkdir " << _qStrFullPathDirnameForZLI;
            return(false);
        }
    }
    return(true);
}

QString ZoomOutLevelImageBuilder::getZLIStoragePath() const {
    return(_qStrFullPathDirnameForZLI);
}

//check if the ZLI storage content seems to match with the input file
//This is a middle check but better to avoid to use a ZLI of an image considering only the input filename
//Instead, we check also the the width, height, depth, number of channels, unit size type and file size in bytes.

//this method consider _qStrFullPathDirnameForZLI directory exists
bool ZoomOutLevelImageBuilder::evaluateContentStateOfZLIStoragePath(bool& bNeedCleanDirectory) {

    QString qstrFullPathFilenameInputImageInfos = _qStrFullPathDirnameForZLI + QDir::separator() + _strInputImageInfos_jsonFile;

    bNeedCleanDirectory = false;
    if (!fileExists(qstrFullPathFilenameInputImageInfos)) {
        qDebug() << __FUNCTION__ << ": json file not found: " << qstrFullPathFilenameInputImageInfos;
        bNeedCleanDirectory =  true;
        return(true);
    }

    IOJsonInfos ioInputImageInfos;
    bool bRead = ioInputImageInfos.loadFromFile(qstrFullPathFilenameInputImageInfos);
    if (!bRead) {
        qDebug() << __FUNCTION__ << ": error: getting infos from file: " << qstrFullPathFilenameInputImageInfos;
        bNeedCleanDirectory = true; //considers to clean the directory
        return(true);
    }

    S_ImageFileAttributes_with_dateTimeLastModified sImageFileAttributes_fromJson;
    bool bJsonToSImgAttr = sImageFileAttributes_fromJson.fromQJsonObject(ioInputImageInfos.getQJsonObject());
    if (!bJsonToSImgAttr) {
        bNeedCleanDirectory = true; //consider to clean the directory
        return(true);
    }

    if (!fileExists(_qstrRootImage)) {
        qDebug() << __FUNCTION__ << " error: _qstrRootImage: " << _qstrRootImage << " does not exist";
        return(false);
    }

    S_ImageFileAttributes_with_dateTimeLastModified SimageFileAttributes_fromFile;

    bool bGotFromFile = SimageFileAttributes_fromFile.fromFile(_qstrRootImage);
    if (!bGotFromFile) {
        qDebug() << __FUNCTION__ << ": error: SimageFileAttributes.fromFile(...) on: " << _qstrRootImage;
        return(false);
    }
    bool bAllEqual = false;
    bAllEqual = SimageFileAttributes_fromFile.compareTo(sImageFileAttributes_fromJson);
    if (!bAllEqual) {
        qDebug() << __FUNCTION__ << " warning: some characteristic(s) not equal"  << _qstrRootImage;
        bNeedCleanDirectory = true; //consider: clean the directory
    }

    return(true);
}


bool ZoomOutLevelImageBuilder::cleanAndInitDirectory() {

    qDebug() << __FUNCTION__ << "clean and init directory with the json file about the input file";

    if (_qStrFullPathDirnameForZLI.isEmpty()) {
        qDebug() << __FUNCTION__ << "if (_qStrFullPathDirnameForZLI.isEmpty()) { => return(false)";
        return(false);
    }
    //clean the directory
    QDir qdirZLIStorage(_qStrFullPathDirnameForZLI);
    QStringList qstrListInputImageInfos = qdirZLIStorage.entryList(QDir::NoDotAndDotDot|QDir::Files); //let any sub-directories in peace
    qDebug() << __FUNCTION__ << qstrListInputImageInfos;

    for(auto iter: qstrListInputImageInfos) {
        QFile fileToRemove(iter);
        /*bool bRemoved = */fileToRemove.remove(); //@#LP consider that any fail to remove file is not critical error here.
    }

    //write json file

    //@#LP not check get info ok
    QFileInfo qfinfo_rootImage(_qstrRootImage);
    qint64 rooImageFileSize = qfinfo_rootImage.size();

    /*
    //json content example:
    {
        "filename": "hu.txt",
        "width": 1234,
        "height": 5678,
        "nchannels": 1,
        "filesize": 762104
    }
    */

    S_ImageFileAttributes_with_dateTimeLastModified SimageFileAttributes_fromFile;
    bool bGotFromFile = SimageFileAttributes_fromFile.fromFile(_qstrRootImage);
    if (!bGotFromFile) {
        qDebug() << __FUNCTION__ << ": error: SimageFileAttributes.fromFile(...) on: " << _qstrRootImage;
        return(false);
    }

    IOJsonInfos ioInputImageInfos;
    QJsonObject qJsonObject_IOInputImageInfos;
    SimageFileAttributes_fromFile.toQJsonObject(qJsonObject_IOInputImageInfos);
    ioInputImageInfos.setQJsonObject(qJsonObject_IOInputImageInfos);
    QString qstrFullPahInputImageInfos = _qStrFullPathDirnameForZLI + QDir::separator() + _strInputImageInfos_jsonFile;

    bool bWrite = ioInputImageInfos.saveToFile(qstrFullPahInputImageInfos);
    if (!bWrite) {
        qDebug() << __FUNCTION__ << ": error: writting infos to file: " << qstrFullPahInputImageInfos;
        return(false);
    }
    return(true);
}

/* build_untilWidthOrHeightGreaterThan(...) method uses
   the previous image in the image pyramid to produce the next image in the pyramid level (big to small).
   It's faster than build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(...)
   but Nan pixels are more than propagated. This behavior can create Nan area from a single Nan.
   build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel method produces better Nan handling but slower.
*/
bool ZoomOutLevelImageBuilder::build_untilWidthOrHeightGreaterThan(int32_t whsize, int stepAmount, bool &bNanDetectedInZoomLevelFour) {

    QElapsedTimer timer;
    timer.start();

    bNanDetectedInZoomLevelFour = false;

    ImageSpec imgSpec;
    bool bImageSpecGot = InputImageFormatChecker::getImageSpec(_qstrRootImage.toStdString(), imgSpec);
    if (!bImageSpecGot) {
        qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << _qstrRootImage;
        return(false);
    }

    //string_view oiioStrView_resizeFilter( "cubic");
    int nbThreadsToUse = 4; //@#LP fixed to 4 , as not too small abd not too big
    //@#LP maybe nbThreadsToUse could go in app settings in case of trouble with more than one thread

    unique_ptr<VectorType> uptrVectorTypeROIdataFullBlock;
    unique_ptr<VectorType> uptrVectorTypeROIdataFullBlockResampled;

    unique_ptr<VectorType> uptrVectorTypeROIdataRemainingBlock;
    unique_ptr<VectorType> uptrvectorTypeROIdataRemainingResampledBlock;

    //Create an image cache and set some options
    ImageCache *cache = ImageCache::create(false);
    bool bSuccessAttributeSet = true;
    bSuccessAttributeSet &= cache->attribute("max_memory_MB", 256.0);
    bSuccessAttributeSet &= cache->attribute("autotile", 64);
    bSuccessAttributeSet &= cache->attribute("autoscanline", 0); //square tile (autotile != 0)
    bSuccessAttributeSet &= cache->attribute("unassociatedalpha",1);

    float stepFractionFor0to100Range = 100.0/static_cast<float>(stepAmount);

    int currentStep = 0;

    //consider the first channel for the basetype
    //set the cache attribute to float if basetype is float
    unsigned char typeDescBaseType = imgSpec.channelformat(0).basetype;
    switch (typeDescBaseType) {
        case TypeDesc::UINT8:
            bSuccessAttributeSet &= cache->attribute("forcefloat", 0); //default is float
            qDebug() << "set forcefloat to 0";
            break;
        case TypeDesc::INT16://signed 16 bits integer //@#LP testing INT16 with forcefloat 1
        case TypeDesc::UINT16://unsigned 16 bits integer //@#LP testing UINT16 with forcefloat 1
        case TypeDesc::FLOAT:
            bSuccessAttributeSet &= cache->attribute("forcefloat", 1);
            qDebug() << "set forcefloat to 1";
            break;
        default:
            bSuccessAttributeSet = false;
            qDebug() << __FUNCTION__ << ": error: unsupported basetype:" << typeDescBaseType;
            break;
    }

    if (!bSuccessAttributeSet) {
        qDebug() << "error: one or more cache->attribute set failed";
        qDebug() << geterror().c_str();

        cache->close_all();
        ImageCache::destroy(cache);

        return(false);
    }

    string strInputFile = _qstrRootImage.toStdString();

    qDebug() << "strInputFile = " << strInputFile.c_str();

    bool bNoMoreZoomOut = false;
    {
        int wInputImg = imgSpec.width;
        int hInputImg = imgSpec.height;

        qDebug() << "#1 wInputImg: " << wInputImg;
        qDebug() << "#1 hInputImg: " << hInputImg;

        qDebug() << "#1 whsize: " << whsize;

        if ((wInputImg <= whsize) && (hInputImg <= whsize)) {
            qDebug() << "if ((wInputImg <= whsize) && (hInputImg <= whsize)) {";
            bNoMoreZoomOut = true;
        }
    }

    int zoomLevel = 2; //for computation
    int zoomLevel_forFilenaming = 2;

    bool bNoError = true;

    while(1) {

        if (bNoMoreZoomOut) {
            qDebug() << "while(1) { if  (bNoMoreZoomOut) { ";
            break;
        }

        ImageSpec inputImgSpec;

        bool bImageSpecGot = InputImageFormatChecker::getImageSpec(strInputFile, inputImgSpec);
        if (!bImageSpecGot) {
            qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << strInputFile.c_str();
            bNoError = false;
            break;
        }

        int wInputImg = inputImgSpec.width;
        int hInputImg = inputImgSpec.height;
        int channels = inputImgSpec.nchannels;

        for (int c = 0; c < inputImgSpec.nchannels; ++c) {
            qDebug() << " _ inputImgSpec.channelformat: " << inputImgSpec.channelformat(c).basetype;
        }

        qDebug() << "InputImg(w,h,c) " << wInputImg << " x " << hInputImg << "ch: " << channels;

        qInfo() << __FUNCTION__ << "building Zoom Level Image from " << strInputFile.c_str();

        //compute output size, adjusting input size to one pixel upper if odd
        int wOutputImg = (wInputImg+wInputImg%2)/zoomLevel;
        int hOutputImg = (hInputImg+hInputImg%2)/zoomLevel;

        if ((wOutputImg <= whsize) && (hOutputImg <= whsize)) {
            qDebug() << __FUNCTION__ << "whsize = " << whsize;
            qDebug() << __FUNCTION__ << "if ((wOutputImg <= whsize) && (hOutputImg <= whsize)) {";
            bNoMoreZoomOut = true;
        }

        QString qstrZL = QString::number(zoomLevel_forFilenaming);

        QString qstrZLIOutputFilename = "zoomOutLevel_" + qstrZL + '.' + _qstrExtension; //+ ".tif";

        QString qstrDirSeparator = QDir::separator();
        string strOutPathAndFilename = _qStrFullPathDirnameForZLI.toStdString() + qstrDirSeparator.toStdString() + qstrZLIOutputFilename.toStdString();
        //@#LP: add a test about already exist to avoid time consuming overwrite ?

        qDebug() << " will write resized_finalpic in: " << strOutPathAndFilename.c_str();

        std::unique_ptr<ImageOutput> ImgOut = ImageOutput::create(strOutPathAndFilename.c_str());
        if (ImgOut == nullptr) {
            qDebug() << "error in ImageOutput::create";
            bNoError = false;
            break;
        }

        qDebug() << "--- #1: ImgOut->format_name() = " << ImgOut->format_name();
        qDebug() << "(while1) OutputImg(w,h)      : " << wOutputImg << ", " << hOutputImg;
        qDebug() << "(while1) SrcImgForResize(w,h): " << wInputImg  << ", " << hInputImg;

        int readDataBlockMaxSizeInBytes =  64*1024*1024;
        qDebug() << "readDataBlockMaxSizeInBytes : " << readDataBlockMaxSizeInBytes;

        int sizeOfAPixelMultiplyByNbChannels = static_cast<int>(inputImgSpec.format.size()) * inputImgSpec.nchannels;
        qDebug() << "sizeOfUnitMultiplyChannels : " << sizeOfAPixelMultiplyByNbChannels;

        int nbLineToReadByBlock = ((readDataBlockMaxSizeInBytes / (wInputImg * sizeOfAPixelMultiplyByNbChannels))/2)*2; //avoid odd nb lines value
        qDebug() << "step#1: nbLineToReadByBlock : " << nbLineToReadByBlock;

        int nearestPow2_of_nbLineToReadByBlock = nearestPow2(nbLineToReadByBlock);
        //cout << "step#2: nearestPow2_of_nbLineToReadByBlock : " << nearestPow2_of_nbLineToReadByBlock;cout << endl;
        if (nearestPow2_of_nbLineToReadByBlock > nbLineToReadByBlock) {
            nearestPow2_of_nbLineToReadByBlock /= 2;
        }
        nbLineToReadByBlock = nearestPow2_of_nbLineToReadByBlock;

        int nbLineToWriteByBlock = nbLineToReadByBlock / zoomLevel;
        qDebug() << "step#2: nbLineToReadByBlock  : " << nbLineToReadByBlock;

        //set the limit if the image is small enough to be read in one block:
        if ((int)nbLineToReadByBlock > hInputImg) {
            qDebug() << "step#2-3: the whole image fits in one block";

            nbLineToReadByBlock = hInputImg;
            nbLineToWriteByBlock = hOutputImg;
        }

        int nbLineRemainingForLastBlock = hInputImg % nbLineToReadByBlock;

        qDebug() << "step#3: nbLineToReadByBlock = " << nbLineToReadByBlock;
        qDebug() << "step#3: nbLineToWriteByBlock = " << nbLineToWriteByBlock;
        qDebug() << "step#3: nbLineRemainingForLastBlock = " << nbLineRemainingForLastBlock;

        //- computation of nbLineToReadByBlock take into account the ammoutn of buffer in bytes
        //- any size of block belows already implicitely handles the pixel size type due to template type usage vector<pixel size type>

        //-----------
        OIIO::TypeDesc::BASETYPE typeDescBasetype = static_cast<OIIO::TypeDesc::BASETYPE>(inputImgSpec.format.basetype);

        switch (typeDescBasetype) {

                case OIIO::TypeDesc::BASETYPE::UINT8:
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorUint8>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorUint8>();
                    qDebug() << __FUNCTION__ << "new VectorUint8";
                    break;

                case OIIO::TypeDesc::BASETYPE::INT16: //signed 16 bits integer
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorSignedInt16>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorSignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorSignedInt16";
                    break;

                case OIIO::TypeDesc::BASETYPE::UINT16: //unsigned 16 bits integer
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorUnsignedInt16>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorUnsignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorUnsignedInt16";
                    break;

                case OIIO::TypeDesc::BASETYPE::FLOAT: //  /// ///case sizeof(float):
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorFloat>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorFloat>();
                    qDebug() << __FUNCTION__ << "new VectorFloat";
                    break;

                default:
                    qDebug() << __FUNCTION__ << "error: invalid typeDescBasetype:" << typeDescBasetype;
                    bNoError = false;
                    break;
        }
        if  (!bNoError) {
            break;
        }

        qDebug() << __FUNCTION__ << "ptrVectorTypeROIdataFullBlock @ = "          << (void*)uptrVectorTypeROIdataFullBlock->data();
        qDebug() << __FUNCTION__ << "ptrVectorTypeROIdataFullBlockResampled @ = " << (void*)uptrVectorTypeROIdataFullBlockResampled->data();

        //-------------
        uint32_t sizeFullBlock = wInputImg * nbLineToReadByBlock * channels;

        uptrVectorTypeROIdataFullBlock->resize(sizeFullBlock);
        qDebug() << "_ptrVectorTypeROIdataFullBlock size: " << uptrVectorTypeROIdataFullBlock->size();

        uint32_t sizeFullBlockResampled = wOutputImg  * nbLineToWriteByBlock * channels;

        uptrVectorTypeROIdataFullBlockResampled->resize(sizeFullBlockResampled);
        qDebug() << "_ptrVectorTypeROIdataFullBlockResampled size: " << uptrVectorTypeROIdataFullBlockResampled->size();

        {
            ImageBuf ImageBufInput_(ustring(strInputFile), cache);

            //string xmlstring_ImgInputSpec_withAllSetAttribute  = ImageBufSrc.spec().to_xml();

            //cout << "ImageBufInput.spec() xml: " << xmlstring_ImgSrcSpec_withAllSetAttribute << endl;

            qDebug() << __FUNCTION__ << "ImageBufInput.spec().format = " << ImageBufInput_.spec().format.c_str();
            qDebug() << __FUNCTION__ << "typeDescBasetype = " << typeDescBasetype;

            ImageSpec ImageSpecOut(wOutputImg, hOutputImg, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

            //cout << "--- #2: ImgOut->format_name() = " << ImgOut->format_name() << endl;

            //outSpec.from_xml(xmlstring_ImgSrcSpec_withAllSetAttribute.c_str());

            if (   (_qstrExtension.toLower() == "tif")
                 ||(_qstrExtension.toLower() == "tiff") ) {

                ImageSpecOut.attribute("compression", "none");
                ImageSpecOut.attribute("oiio::BitsPerSample", (int)(8*inputImgSpec.format.size())); //@##LP RGB 3 => * channels ?
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
            }

            bool bOPenSuccess = ImgOut->open(strOutPathAndFilename.c_str(), ImageSpecOut/*, ImageOutput::OpenMode::Create*/);
            qDebug() << "bOPenSuccess = " << bOPenSuccess;
            if (!bOPenSuccess) {
                qDebug() << "global geterror() = " << geterror().c_str();
                qDebug() << "ImgOut-> geterror() = " << ImgOut-> geterror().c_str();

                bNoError = false;
                break;
            }

            qDebug() << __FUNCTION__ <<  "outSpec.to_xml() xml: " << ImageSpecOut.to_xml().c_str();

            qDebug() << __FUNCTION__ <<  " ImageBufInput.spec().format.size() = " << ImageBufInput_.spec().format.size();
            qDebug() << __FUNCTION__ <<  " ImageBufInput.spec().format.c_str()= " << ImageBufInput_.spec().format.c_str();

            //cout << " ImageBufSrc.spec().get_int_attribute(\"oiio:BitsPerSample\") = " << ImageBufSrc.spec().get_int_attribute("oiio:BitsPerSample") << endl;

            qDebug() << __FUNCTION__ <<  " ImageSpecOut.spec().size()  = " << ImageSpecOut.format.size();
            qDebug() << __FUNCTION__ <<  " ImageSpecOut.spec().c_str() = " << ImageSpecOut.format.c_str();

            //cout << " outSpec.get_int_attribute(\"oiio:BitsPerSample\") = " << outSpec.get_int_attribute("oiio:BitsPerSample") << endl;
            //cout << " TypeDesc::HALF =" << TypeDesc::HALF << endl;

            //qDebug() << __FUNCTION__ <<  " ImageSpec_warpedBlockData.format        = " << ImageSpec_warpedBlockData.format;
            //cout << " ImgSpec_warpedBlockData.get_int_attribute(\"oiio:BitsPerSample\") = " << ImgSpec_warpedBlockData.get_int_attribute("oiio:BitsPerSample") << endl;

            //cout << "ImgSpec_warpedBlockData.scanline_bytes()        = " << ImgSpec_warpedBlockData.scanline_bytes() << endl;
            //cout << "ImageSpec_warpedResampledBlock.scanline_bytes() = " << ImgSpec_warpedResampledBlock.scanline_bytes() << endl;

            ROI roiResampled(
              0, wOutputImg,
              0, nbLineToWriteByBlock,
              0, 1, //z
              0, channels);

            if (!roiResampled.defined()) {
                qDebug() << "*** error: if (!roiResampled.defined())" ;

                bNoError = false;
                break;
            }
            qDebug() << "roiResampled(w,h): "
                     << roiResampled.width() << "," << roiResampled.height()
                     << " ( depth: " << roiResampled.depth()
                     << " nchannels: " << roiResampled.nchannels() << endl;

            //read, resample and write all full blocks

            int nbFullBlockToProcess = hInputImg / nbLineToReadByBlock;
            int yWriteScanLine = 0;

            qDebug() << "nbFullBlockToProcess = " << nbFullBlockToProcess;

            for (int iFullBlock = 0; iFullBlock < nbFullBlockToProcess; iFullBlock++) {

                qDebug() << "loop: iFullBlock = " << iFullBlock;

                //get pixel from source:
                int x0 = 0;
                int x1 = wInputImg;
                int y0 = iFullBlock*nbLineToReadByBlock;
                int y1 = y0 + nbLineToReadByBlock;

                qDebug() << "cache->get_pixels : " << x0 << ", " << x1 << ", " << y0 << ", " <<  y1;

                //get data into vector
                bool bCacheGotPixel = cache->get_pixels (
                    ustring(strInputFile), //ustring filename,
                    0, 0,                  //int subimage, int miplevel
                    x0, x1, y0, y1,        //int xbegin, int xend,int ybegin, int yend,
                    0, 1,                  //int zbegin, int zend,
                    0, channels,           //int chbegin, int chend,
                    typeDescBasetype, //TypeDesc::FLOAT,        //TypeDesc format,
                    (void*)uptrVectorTypeROIdataFullBlock->data(), //void *result,
                     AutoStride, AutoStride        //stride_t xstride=AutoStride, stride_t ystride=AutoStride
                ); //cache_chbegin, cache_chend = -1

                if (!bCacheGotPixel) {
                    qDebug() << "global geterror() = " << geterror().c_str();
                    qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                    bNoError = false;
                    break;
                }

                qDebug() << __FUNCTION__ <<  "****** get_pixels done!";

                //cout << "total bytes read by cache =" << cache->bytes_read() << endl;

                emit signal_stepProgressionForOutside(static_cast<int>( static_cast<float>(currentStep) * stepFractionFor0to100Range));

                //cout << "cache->get_pixels: done" << endl;

                bool bResizeSuccess = false;

                {
                    ImageSpec ImageSpec_warpedBlockData     (wInputImg , nbLineToReadByBlock , channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));
                    ImageSpec ImageSpec_warpedResampledBlock(wOutputImg, nbLineToWriteByBlock, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

                    //warp vector to ImageBuf
                    ImageBuf ImageBufBlock   (ImageSpec_warpedBlockData, (void*)/*vectROIdataFullBlock.*/uptrVectorTypeROIdataFullBlock->data());

                    //resample to destBlock
                    ImageBuf ImageBufResampledBlock(ImageSpec_warpedResampledBlock, (void*)/*vectROIdataFullBlockResampled.*/uptrVectorTypeROIdataFullBlockResampled->data());

                    //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => gaussian 3.0 ===> 6m30,940s (SSD)
                    //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => default ========>           (SSD)
                    //bResizeSuccess = ImageBufAlgo::resize(ImageBufResampledBlock, ImageBufBlock, oiioStrView_resizeFilter, .0f, roiResampled, nbThreadsToUse);

                    //test resample instead of resize to see if it crashes with float and thread:
                    //resample instead of resize (resize on float type and thread => crash on MacOSX)
                    bResizeSuccess = ImageBufAlgo::resample(ImageBufResampledBlock, ImageBufBlock, true, roiResampled, nbThreadsToUse);


                    qDebug() << __FUNCTION__ <<  "bResizeSuccess: " << bResizeSuccess;

                    if (!bResizeSuccess) {

                        qDebug() << "error: resize failed on full block #" << iFullBlock;

                        qDebug() << "global geterror() = " << geterror().c_str();
                        qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                        bNoError = false;
                        break;
                    }

                    //------------------------------------------------------
                    //ZLI for level #2 build is the same in the two ways to handle Nan
                    //Hence, Nan detection is done on level #4 to limit the time spent for the overall build (with or without the switch to the other way to handle Nan)
                    if (zoomLevel_forFilenaming == 4) {
                        bool bContainsNan = false;
                        bNoError = pixStats_NanCount(ImageBufResampledBlock, bContainsNan);
                        if (!bNoError) {
                            break;
                        }
                        if (bContainsNan) {
                            bNanDetectedInZoomLevelFour = true;
                            break;
                        }
                    }
                    //------------------------------------------------------

                }

                qDebug() << __FUNCTION__ <<  "bUseMany write_scanlines( y->y : " << yWriteScanLine << "-> " << yWriteScanLine+nbLineToWriteByBlock;

                bool bSuccessWrite = ImgOut->write_scanlines(
                    yWriteScanLine,
                    yWriteScanLine+nbLineToWriteByBlock,
                    0,
                    typeDescBasetype, //TypeDesc::FLOAT,//TypeDesc::UNKNOWN,

                    uptrVectorTypeROIdataFullBlockResampled->data(), AutoStride, AutoStride);

                if (!bSuccessWrite) {
                    qDebug() << "error in write_scanlines";
                    qDebug() << "global geterror() = " << geterror().c_str();
                    qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                    bNoError = false;
                    break;
                } else {
                    qDebug() << __FUNCTION__ <<  "write_scanlines ok! (many)";
                }

                yWriteScanLine+=nbLineToWriteByBlock;

                qDebug() << __FUNCTION__ << __LINE__;

                currentStep++;
                emit signal_stepProgressionForOutside(static_cast<int>( static_cast<float>(currentStep) * stepFractionFor0to100Range));

            }

            if (!bNoError) {
                break;
            }
            if (bNanDetectedInZoomLevelFour) {
                break;
            }



            qDebug() << __FUNCTION__ << __LINE__;

            qDebug() << __FUNCTION__ <<  "ptrVectorTypeROIdataFullBlock @ = "          << (void*)uptrVectorTypeROIdataFullBlock->data();
            qDebug() << __FUNCTION__ <<  "ptrVectorTypeROIdataFullBlockResampled @ = " << (void*)uptrVectorTypeROIdataFullBlockResampled->data();

            qDebug() << __FUNCTION__ << __LINE__;

            //remaining last not full block
            if (nbLineRemainingForLastBlock) {

                switch (typeDescBasetype) {

                    case OIIO::TypeDesc::BASETYPE::FLOAT:
                        uptrVectorTypeROIdataRemainingBlock = make_unique<VectorFloat>();
                        uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorFloat>();
                        qDebug() << __FUNCTION__ << "new VectorFloat";
                        break;

                    case OIIO::TypeDesc::BASETYPE::UINT8:
                        uptrVectorTypeROIdataRemainingBlock = make_unique<VectorUint8>();
                        uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorUint8>();
                        qDebug() << __FUNCTION__ << "new VectorUint8";
                        break;

                    case OIIO::TypeDesc::BASETYPE::INT16: //signed 16 bits integer
                        uptrVectorTypeROIdataRemainingBlock = make_unique<VectorSignedInt16>();
                        uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorSignedInt16>();
                        qDebug() << __FUNCTION__ << __LINE__ << "new VectorSignedInt16";
                        break;

                    case OIIO::TypeDesc::BASETYPE::UINT16: //unsigned 16 bits integer
                        uptrVectorTypeROIdataRemainingBlock = make_unique<VectorUnsignedInt16>();
                        uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorUnsignedInt16>();
                        qDebug() << __FUNCTION__ << __LINE__ << "new VectorUnsignedInt16";
                        break;

                    default:
                        qDebug() << __FUNCTION__ << "error: invalid typeDescBasetype:" << typeDescBasetype;
                        return(false);
                }

                uptrVectorTypeROIdataRemainingBlock->resize(wInputImg * nbLineRemainingForLastBlock * channels);
                qDebug() << "vectROIdataRemainingBlock size: " << uptrVectorTypeROIdataRemainingBlock->size();

                int nbLineToWriteForLastBlock = (nbLineRemainingForLastBlock + nbLineRemainingForLastBlock%2) / zoomLevel; //valid because zoomLevel stay at 2

                uptrvectorTypeROIdataRemainingResampledBlock->resize(wOutputImg * nbLineToWriteForLastBlock * channels);
                qDebug() << "vectROIdataRemainingBlockResampled size: " << uptrvectorTypeROIdataRemainingResampledBlock->size();

                //@#LP opt: possible to avoid to allocate ImgBlock each time ?

                int gpx0 = 0;
                int gpx1 = wInputImg;
                int gpy0 = hInputImg-nbLineRemainingForLastBlock;
                int gpy1 = hInputImg;

                qDebug() << "remaning cache->get_pixels: " << gpx0 << ", "<< gpx1 << ", "<< gpy0 << ", "<< gpy1;
                //get data into vector
                bool bCacheGotPixel = cache->get_pixels (
                    ustring(strInputFile), //ustring filename,
                    0, 0,                  //int subimage, int miplevel
                    gpx0, gpx1, gpy0, gpy1,//int xbegin, int xend,int ybegin, int yend,
                    0, 1,                  //int zbegin, int zend,
                    0, channels,           //int chbegin, int chend,
                    typeDescBasetype, //TypeDesc::FLOAT,         //TypeDesc format,
                    (void*)uptrVectorTypeROIdataRemainingBlock->data(), //void *result,
                    AutoStride, AutoStride        //stride_t xstride=AutoStride, stride_t ystride=AutoStride
                ); //int cache_chbegin = 0, int cache_chend = -1

                if (!bCacheGotPixel) {
                    qDebug() << "global geterror() = " << geterror().c_str();
                    qDebug() << "cache-> geterror() = " << cache->geterror().c_str();

                    bNoError = false;
                    break;
                }

                //cout << "cache->get_pixels: done" << endl;

                ROI roiResampled_remainingBlock(
                0, wOutputImg,
                0, nbLineToWriteForLastBlock,
                0, 1, //z
                0, channels);

                //qDebug() << "roiResampled_remainingBlock: " << x0_remaining << ", " <<  x1_remaining << " ; " <<  y0_remaining << ", " <<  y1_remaining;

                qDebug() << "roiResampled_remainingBlock: xbegin, xend; ybegin, yend: "
                       << roiResampled_remainingBlock.xbegin << ", " <<  roiResampled_remainingBlock.xend << " ; "
                       << roiResampled_remainingBlock.ybegin << ", " <<  roiResampled_remainingBlock.yend;

                bool bResizeSuccess = false;

                {
                    ImageSpec ImgSpec_warpedRemainingBlockData     ( wInputImg, nbLineRemainingForLastBlock, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));
                    ImageSpec ImgSpec_warpedRemainingResampledBlock(wOutputImg, nbLineToWriteForLastBlock  , channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

                    //warp vector to ImageBuf
                    ImageBuf ImageBuf_remainingBlock(ImgSpec_warpedRemainingBlockData, (void*)/*vectROIdataRemainingBlock.*/uptrVectorTypeROIdataRemainingBlock->data());

                    //@#LP opt: possible to avoid to allocate ImgBlock each time ?
                    //resample to destBlock
                    ImageBuf ImageBuf_remainingResampledBlock(ImgSpec_warpedRemainingResampledBlock, (void*)/*vectROIdataRemainingResampledBlock.*/uptrvectorTypeROIdataRemainingResampledBlock->data());

                    //bResizeSuccess = ImageBufAlgo::resize(ImageBuf_remainingResampledBlock, ImageBuf_remainingBlock, oiioStrView_resizeFilter, 0, roiResampled_remainingBlock, nbThreadsToUse);
                    //test resample instead of resize to see if it crashes with float and thread:
                    bResizeSuccess = ImageBufAlgo::resample(ImageBuf_remainingResampledBlock, ImageBuf_remainingBlock, true, roiResampled_remainingBlock, nbThreadsToUse);


                    qDebug() << "bResizeSuccess: " << bResizeSuccess;
                    if (!bResizeSuccess) {

                        qDebug() << "error: resize failed on roiResampled_remainingBlock";

                        qDebug() << "global geterror() = " << geterror().c_str();
                        qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                        bNoError = false;
                        break;
                    }

                    //------------------------------------------------------
                    //ZLI for level #2 build is the same in the two ways to handle Nan
                    //Hence, Nan detection is done on level #4 to limit the time spent for the overall build (with or without the switch to the other way to handle Nan)
                    if (zoomLevel_forFilenaming == 4) {
                        bool bContainsNan = false;
                        bNoError = pixStats_NanCount(ImageBuf_remainingResampledBlock, bContainsNan);
                        if (!bNoError) {
                            break;
                        }
                        if (bContainsNan) {
                            bNanDetectedInZoomLevelFour = true;
                            break;
                        }
                    }
                    //------------------------------------------------------
                }

                //yWriteScanLine = nbLineToWriteByBlock * nbFullBlockToProcess;
                qDebug() << "many write_scanlines( y->y : " << yWriteScanLine << "-> " << yWriteScanLine+nbLineToWriteForLastBlock;

                bool bSuccessWrite = ImgOut->write_scanlines(
                yWriteScanLine,
                yWriteScanLine+nbLineToWriteForLastBlock,
                0,
                typeDescBasetype, //TypeDesc::FLOAT,//TypeDesc::UNKNOWN,

                uptrvectorTypeROIdataRemainingResampledBlock->data(), AutoStride, AutoStride);

                if (!bSuccessWrite) {
                    qDebug() << "error in write_scanlines";
                } else {
                    qDebug() << "write_scanlines ok (many)!";
                }

                currentStep++;
                emit signal_stepProgressionForOutside(static_cast<int>( static_cast<float>(currentStep) * stepFractionFor0to100Range));

            }

            qDebug() << __FUNCTION__ << __LINE__;

            bool bimgOutCloseOK = ImgOut->close();
            qDebug() << __FUNCTION__  << __LINE__ << "bimgOutCloseOK = " << bimgOutCloseOK;

            cache->close(ustring(strInputFile));

            qDebug() << __FUNCTION__ << __LINE__;

            uptrVectorTypeROIdataFullBlock.reset(nullptr);

            qDebug() << __FUNCTION__ << __LINE__;

            uptrVectorTypeROIdataFullBlockResampled.reset(nullptr);

            qDebug() << __FUNCTION__ << __LINE__;

            uptrVectorTypeROIdataRemainingBlock.reset(nullptr);

            qDebug() << __FUNCTION__ << __LINE__;

            uptrvectorTypeROIdataRemainingResampledBlock.reset(nullptr);

            qDebug() << __FUNCTION__ << __LINE__;

        }

        if (!bNoError) {
            break;
        }
        if (bNanDetectedInZoomLevelFour) {
            break;
        }

        bool bimgOutCloseOK = ImgOut->close();
        qDebug() << __FUNCTION__ << __LINE__ << "bimgOutCloseOK = " << bimgOutCloseOK;

        zoomLevel_forFilenaming*=2;

        cache->close(ustring(strInputFile));

        qDebug() << __FUNCTION__ << __LINE__;

        qDebug() << __FUNCTION__ << __LINE__;

        uptrVectorTypeROIdataFullBlock.reset(nullptr);
        uptrVectorTypeROIdataFullBlockResampled.reset(nullptr);

        uptrVectorTypeROIdataRemainingBlock.reset(nullptr);
        uptrvectorTypeROIdataRemainingResampledBlock.reset(nullptr);


        //next input is the just generated zoom level picture
        strInputFile = strOutPathAndFilename;
    }

    qDebug() << __FUNCTION__ << __LINE__;

    cache->close_all();
    ImageCache::destroy(cache, true); //@#LP try with teardown true

    uptrVectorTypeROIdataFullBlock.reset(nullptr);
    uptrVectorTypeROIdataFullBlockResampled.reset(nullptr);

    uptrVectorTypeROIdataRemainingBlock.reset(nullptr);
    uptrvectorTypeROIdataRemainingResampledBlock.reset(nullptr);


    qint64 elapsedTime = timer.elapsed();
    qDebug() << __FUNCTION__ << "took :" << elapsedTime << " ms ";

    return(bNoError);
}



bool ZoomOutLevelImageBuilder::computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan(int32_t whsize, int& stepAmount) {

    stepAmount = 0;

    ImageSpec imgSpec;
    bool bImageSpecGot = InputImageFormatChecker::getImageSpec(_qstrRootImage.toStdString(), imgSpec);
    if (!bImageSpecGot) {
        qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << _qstrRootImage;
        return(false);
    }

    int sizeOfAPixelMultiplyByNbChannels_ofInputImage = static_cast<int>(imgSpec.format.size()) * imgSpec.nchannels;
    qDebug() << "sizeOfAPixelMultiplyByNbChannels_ofInputImage : " << sizeOfAPixelMultiplyByNbChannels_ofInputImage;
    int wInputImg = imgSpec.width;
    int hInputImg = imgSpec.height;
    int channels  = imgSpec.nchannels;
    qDebug() << "InputImg(w,h,c) " << wInputImg << " x " << hInputImg << "ch: " << channels;

    string strInputFile = _qstrRootImage.toStdString();

    qDebug() << "strInputFile = " << strInputFile.c_str();

    bool bNoMoreZoomOut = false;
    int zoomLevel = 2;

    while(1) {

        if  (bNoMoreZoomOut) {
            break;
        }

        qInfo() << __FUNCTION__ << "Zoom Level Image from " << strInputFile.c_str();

        //compute output size, adjusting input size to one pixel upper if odd
        int wOutputImg = (wInputImg+wInputImg%2)/zoomLevel;
        int hOutputImg = (hInputImg+hInputImg%2)/zoomLevel;

        if ((wOutputImg <= whsize) && (hOutputImg <= whsize)) {
            qDebug() << __FUNCTION__ << "if ((wOutputImg <= 1024) && (hOutputImg <= 1024)) {";
            bNoMoreZoomOut = true;
        }

        //qDebug() << "OutputImg(w,h)      : " << wOutputImg << ", " << hOutputImg;
        //qDebug() << "SrcImgForResize(w,h): " << wInputImg  << ", " <<  hInputImg;

        int readDataBlockMaxSizeInBytes =  64*1024*1024;
        qDebug() << "readDataBlockMaxSizeInBytes : " << readDataBlockMaxSizeInBytes;


        int nbLineToReadByBlock = ((readDataBlockMaxSizeInBytes / (wInputImg * sizeOfAPixelMultiplyByNbChannels_ofInputImage))/2)*2; //avoid odd nb lines value
        qDebug() << "step#1: nbLineToReadByBlock : " << nbLineToReadByBlock;

        int nearestPow2_of_nbLineToReadByBlock = nearestPow2(nbLineToReadByBlock);
        //cout << "step#2: nearestPow2_of_nbLineToReadByBlock : " << nearestPow2_of_nbLineToReadByBlock;cout << endl;
        if (nearestPow2_of_nbLineToReadByBlock > nbLineToReadByBlock) {
            nearestPow2_of_nbLineToReadByBlock /= 2;
        }
        nbLineToReadByBlock = nearestPow2_of_nbLineToReadByBlock;

        int nbLineToWriteByBlock = nbLineToReadByBlock / zoomLevel;
        qDebug() << "step#2: nbLineToReadByBlock  : " << nbLineToReadByBlock;

        //set the limit if the image is small enough to be read in one block:
        if ((int)nbLineToReadByBlock > hInputImg) {
            qDebug() << "step#2-3: the whole image fits in one block";

            nbLineToReadByBlock = hInputImg;
            nbLineToWriteByBlock = hOutputImg;
        }

        int nbLineRemainingForLastBlock = hInputImg % nbLineToReadByBlock;

        qDebug() << "step#3: nbLineToReadByBlock = " << nbLineToReadByBlock;
        qDebug() << "step#3: nbLineToWriteByBlock = " << nbLineToWriteByBlock;
        qDebug() << "step#3: nbLineRemainingForLastBlock = " << nbLineRemainingForLastBlock;

        //- computation of nbLineToReadByBlock take into account the ammoutn of buffer in bytes
        //- any size of block belows already implicitely handles the pixel size type due to template type usage vector<pixel size type>

        //-------------

        {
            int nbFullBlockToProcess = hInputImg / nbLineToReadByBlock;

            stepAmount += nbFullBlockToProcess;

            //remaining last not full block
            if (nbLineRemainingForLastBlock) {
                stepAmount++; //@LP considers any remaining last block as a step (does not considers its size)
            }
        }

        wInputImg = wOutputImg;
        hInputImg = hOutputImg;

    }

    return(true);
}



#include <QHash>
#include <QSize>
//
/* build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(...) method uses
   the root image for each level. It produces better Nan value handling than build_untilWidthOrHeightGreaterThan(...) */
bool ZoomOutLevelImageBuilder::build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_fromLevel(int firstZLIToBuild, int32_t whsize, int stepAmount) {
    
    if (firstZLIToBuild < 2) {
        return(false);
    }

    /*
    QElapsedTimer timer;
    timer.start();
    */

    //string_view oiioStrView_resizeFilter( "cubic");
    
    ImageSpec inputImgSpec;
    bool bImageSpecGot = InputImageFormatChecker::getImageSpec(_qstrRootImage.toStdString(), inputImgSpec);
    if (!bImageSpecGot) {
        qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << _qstrRootImage;
        return(false);
    }

    int wInputImg = inputImgSpec.width;
    int hInputImg = inputImgSpec.height;

    QHash<int, QSize> qhash_ZLI_QsizeWHForEachZLI;

    int maxZoomLevel = 1;

    //find the maximum level which will need tp be built
    {
        int wOutputImg = wInputImg;
        int hOutputImg = hInputImg;

        bool bNoMoreZoomOut = false;

        while(1) {

            qDebug() << "whsize = " << whsize;

            if ((wOutputImg <= whsize) && (hOutputImg <= whsize)) {
                qDebug() << "if ((wOutputImg <= 1024) && (hOutputImg <= 1024)) {";
                bNoMoreZoomOut = true;
                break;
            }

            wOutputImg = wInputImg;
            hOutputImg = hInputImg;

            maxZoomLevel *= 2;

            //compute output size, adjusting input size to one pixel upper if odd
            for (int zLvl = 2; zLvl <= maxZoomLevel; zLvl*=2) {
                wOutputImg = (wOutputImg+wOutputImg%2)/2;
                hOutputImg = (hOutputImg+hOutputImg%2)/2;
            }

            qhash_ZLI_QsizeWHForEachZLI.insert(maxZoomLevel, {wOutputImg, hOutputImg});

            qDebug() << "wOutputImg = " << wOutputImg << ", hOutputImg = " << hOutputImg;
        }

        qDebug() << "maxZoomLevel = " << maxZoomLevel;

        if (maxZoomLevel == 1) {
            return(true);
        }

    }

    bool bNoError = true;

    int currentStep = 0;




    //Create an image cache and set some options
    ImageCache *cache = ImageCache::create(false);
    bool bSuccessAttributeSet = true;
    bSuccessAttributeSet &= cache->attribute("max_memory_MB", 256.0);
    bSuccessAttributeSet &= cache->attribute("autotile", 64);
    bSuccessAttributeSet &= cache->attribute("autoscanline", 0); //square tile (autotile != 0)
    bSuccessAttributeSet &= cache->attribute("unassociatedalpha",1);

    float stepFractionFor0to100Range = 100.0/static_cast<float>(stepAmount);

    //consider the first channel for the basetype
    //set the cache attribute to float if basetype is float
    unsigned char typeDescBaseType = inputImgSpec.channelformat(0).basetype;
    switch (typeDescBaseType) {
        case TypeDesc::UINT8:
            bSuccessAttributeSet &= cache->attribute("forcefloat", 0); //default is float
            qDebug() << "set forcefloat to 0";
            break;
        case TypeDesc::INT16://signed 16 bits integer
        case TypeDesc::UINT16://unsigned 16 bits integer
        case TypeDesc::FLOAT:
            bSuccessAttributeSet &= cache->attribute("forcefloat", 1);
            qDebug() << "set forcefloat to 1";
            break;
        default:
            bSuccessAttributeSet = false;
            qDebug() << __FUNCTION__ << ": error: unsupported basetype:" << typeDescBaseType;
            break;
    }

    if (!bSuccessAttributeSet) {
        qDebug() << "error: one or more cache->attribute set failed";
        qDebug() << geterror().c_str();

        cache->close_all();
        ImageCache::destroy(cache);

        return(false);
    }

    for (int zLvl = firstZLIToBuild; zLvl <= maxZoomLevel; zLvl*=2) {

        qDebug() << "zLvl = " << zLvl;

        //string_view oiioStrView_resizeFilter( "cubic");
        int nbThreadsToUse = 4; //@#LP fixed to 4 , as not too small abd not too big //@#LP maybe nbThreadsToUse could go in app settings in case of trouble with more than one thread

        unique_ptr<VectorType> uptrVectorTypeROIdataFullBlock;
        unique_ptr<VectorType> uptrVectorTypeROIdataFullBlockResampled;

        unique_ptr<VectorType> uptrVectorTypeROIdataRemainingBlock;
        unique_ptr<VectorType> uptrvectorTypeROIdataRemainingResampledBlock;

        string strInputFile = _qstrRootImage.toStdString();

        qDebug() << "strInputFile = " << strInputFile.c_str();

        int channels = inputImgSpec.nchannels;
        for (int c = 0; c < inputImgSpec.nchannels; ++c) {
            qDebug() << " _ inputImgSpec.channelformat: " << inputImgSpec.channelformat(c).basetype;
        }
        qDebug() << "InputImg(w,h,c) " << wInputImg << " x " << hInputImg << "ch: " << channels;

        qInfo() << __FUNCTION__ << "building Zoom Level Image from " << strInputFile.c_str();

        //compute output size, adjusting input size to one pixel upper if odd
        int wOutputImg = qhash_ZLI_QsizeWHForEachZLI.value(zLvl).width();
        int hOutputImg = qhash_ZLI_QsizeWHForEachZLI.value(zLvl).height();

        qDebug() << __FUNCTION__ << "wOutputImg : " <<  wOutputImg;
        qDebug() << __FUNCTION__ << "hOutputImg : " <<  hOutputImg;

        QString qstrZL = QString::number(zLvl);
        QString qstrZLIOutputFilename = "zoomOutLevel_" + qstrZL + /*"_fromInputImage."*/ '.' + _qstrExtension; //+ ".tif";

        QString qstrDirSeparator = QDir::separator();
        string strOutPathAndFilename = _qStrFullPathDirnameForZLI.toStdString() + qstrDirSeparator.toStdString() + qstrZLIOutputFilename.toStdString();
        //@#LP: add a test about already exist to avoid time consuming overwrite ?

        qDebug() << " will write resized_finalpic in: " << strOutPathAndFilename.c_str();

        std::unique_ptr<ImageOutput> ImgOut = ImageOutput::create(strOutPathAndFilename.c_str());
        if (ImgOut == nullptr) {
            qDebug() << "error in ImageOutput::create";

            bNoError = false;
            break;
        }

        qDebug() << "--- #1: ImgOut->format_name() = " << ImgOut->format_name();
        qDebug() << "(while1) OutputImg(w,h)      : " << wOutputImg << ", " << hOutputImg;
        qDebug() << "(while1) SrcImgForResize(w,h): " << wInputImg  << ", " << hInputImg;

        int readDataBlockMaxSizeInBytes =  64*1024*1024;
        qDebug() << "readDataBlockMaxSizeInBytes : " << readDataBlockMaxSizeInBytes;

        int sizeOfAPixelMultiplyByNbChannels = static_cast<int>(inputImgSpec.format.size()) * inputImgSpec.nchannels;
        qDebug() << "sizeOfUnitMultiplyChannels : " << sizeOfAPixelMultiplyByNbChannels;

        int nbLineToReadByBlock = ((readDataBlockMaxSizeInBytes / (wInputImg * sizeOfAPixelMultiplyByNbChannels))/2)*2; //avoid odd nb lines value
        qDebug() << "step#1: nbLineToReadByBlock : " << nbLineToReadByBlock;

        int nearestPow2_of_nbLineToReadByBlock = nearestPow2(nbLineToReadByBlock);
        //cout << "step#2: nearestPow2_of_nbLineToReadByBlock : " << nearestPow2_of_nbLineToReadByBlock;cout << endl;
        if (nearestPow2_of_nbLineToReadByBlock > nbLineToReadByBlock) {
            nearestPow2_of_nbLineToReadByBlock /= 2;
        }
        nbLineToReadByBlock = nearestPow2_of_nbLineToReadByBlock;

        int nbLineToWriteByBlock = nbLineToReadByBlock / zLvl;
        qDebug() << "step#2: nbLineToReadByBlock  : " << nbLineToReadByBlock;

        //set the limit if the image is small enough to be read in one block:
        if ((int)nbLineToReadByBlock > hInputImg) {
            qDebug() << "step#2-3: the whole image fits in one block";

            nbLineToReadByBlock = hInputImg;
            nbLineToWriteByBlock = hOutputImg;
        }

        int nbLineRemainingForLastBlock = hInputImg % nbLineToReadByBlock;

        qDebug() << "step#3: nbLineToReadByBlock = " << nbLineToReadByBlock;
        qDebug() << "step#3: nbLineToWriteByBlock = " << nbLineToWriteByBlock;
        qDebug() << "step#3: nbLineRemainingForLastBlock = " << nbLineRemainingForLastBlock;

        //- computation of nbLineToReadByBlock take into account the ammoutn of buffer in bytes
        //- any size of block belows already implicitely handles the pixel size type due to template type usage vector<pixel size type>

        //-----------
        OIIO::TypeDesc::BASETYPE typeDescBasetype = static_cast<OIIO::TypeDesc::BASETYPE>(inputImgSpec.format.basetype);

        switch (typeDescBasetype) {

                case OIIO::TypeDesc::BASETYPE::UINT8:
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorUint8>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorUint8>();
                    qDebug() << __FUNCTION__ << "new VectorUint8";
                    break;

                case OIIO::TypeDesc::BASETYPE::INT16: //signed 16 bits integer
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorSignedInt16>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorSignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorSignedInt16";
                    break;

                case OIIO::TypeDesc::BASETYPE::UINT16: //unsigned 16 bits integer
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorUnsignedInt16>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorUnsignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorUnsignedInt16";
                    break;

                case OIIO::TypeDesc::BASETYPE::FLOAT: //  /// ///case sizeof(float):
                    uptrVectorTypeROIdataFullBlock = make_unique<VectorFloat>();
                    uptrVectorTypeROIdataFullBlockResampled = make_unique<VectorFloat>();
                    qDebug() << __FUNCTION__ << "new VectorFloat";
                    break;

                default:
                    qDebug() << __FUNCTION__ << "error: invalid typeDescBasetype:" << typeDescBasetype;
                    bNoError = false;
                    break;
        }
        if  (!bNoError) {

            break;
        }

        qDebug() << __FUNCTION__ << "ptrVectorTypeROIdataFullBlock @ = "          << (void*)uptrVectorTypeROIdataFullBlock->data();
        qDebug() << __FUNCTION__ << "ptrVectorTypeROIdataFullBlockResampled @ = " << (void*)uptrVectorTypeROIdataFullBlockResampled->data();

        //-------------
        uint32_t sizeFullBlock = wInputImg * nbLineToReadByBlock * channels;

        uptrVectorTypeROIdataFullBlock->resize(sizeFullBlock);
        qDebug() << "_ptrVectorTypeROIdataFullBlock size: " << uptrVectorTypeROIdataFullBlock->size();

        uint32_t sizeFullBlockResampled = wOutputImg  * nbLineToWriteByBlock * channels;

        uptrVectorTypeROIdataFullBlockResampled->resize(sizeFullBlockResampled);
        qDebug() << "_ptrVectorTypeROIdataFullBlockResampled size: " << uptrVectorTypeROIdataFullBlockResampled->size();




        ImageBuf ImageBufInput_(ustring(strInputFile), cache);

        //string xmlstring_ImgInputSpec_withAllSetAttribute  = ImageBufSrc.spec().to_xml();

        //cout << "ImageBufInput.spec() xml: " << xmlstring_ImgSrcSpec_withAllSetAttribute << endl;

        qDebug() << __FUNCTION__ << "ImageBufInput.spec().format = " << ImageBufInput_.spec().format.c_str();
        qDebug() << __FUNCTION__ << "typeDescBasetype = " << typeDescBasetype;

        ImageSpec ImageSpecOut(wOutputImg, hOutputImg, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

        //cout << "--- #2: ImgOut->format_name() = " << ImgOut->format_name() << endl;

        //outSpec.from_xml(xmlstring_ImgSrcSpec_withAllSetAttribute.c_str());

        if (   (_qstrExtension.toLower() == "tif")
             ||(_qstrExtension.toLower() == "tiff") ) {

            ImageSpecOut.attribute("compression", "none");
            ImageSpecOut.attribute("oiio::BitsPerSample", (int)(8*inputImgSpec.format.size())); //@##LP RGB 3 channels => * channels ?
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
        }

        bool bOPenSuccess = ImgOut->open(strOutPathAndFilename.c_str(), ImageSpecOut/*, ImageOutput::OpenMode::Create*/);
        qDebug() << "bOPenSuccess = " << bOPenSuccess;
        if (!bOPenSuccess) {
            qDebug() << "global geterror() = " << geterror().c_str();
            qDebug() << "ImgOut-> geterror() = " << ImgOut-> geterror().c_str();

            bNoError = false;
            break;
        }

        qDebug() << __FUNCTION__ <<  "outSpec.to_xml() xml: " << ImageSpecOut.to_xml().c_str();

        qDebug() << __FUNCTION__ <<  " ImageBufInput.spec().format.size() = " << ImageBufInput_.spec().format.size();
        qDebug() << __FUNCTION__ <<  " ImageBufInput.spec().format.c_str()= " << ImageBufInput_.spec().format.c_str();

        //cout << " ImageBufSrc.spec().get_int_attribute(\"oiio:BitsPerSample\") = " << ImageBufSrc.spec().get_int_attribute("oiio:BitsPerSample") << endl;

        qDebug() << __FUNCTION__ <<  " ImageSpecOut.spec().size()  = " << ImageSpecOut.format.size();
        qDebug() << __FUNCTION__ <<  " ImageSpecOut.spec().c_str() = " << ImageSpecOut.format.c_str();

        //cout << " outSpec.get_int_attribute(\"oiio:BitsPerSample\") = " << outSpec.get_int_attribute("oiio:BitsPerSample") << endl;
        //cout << " TypeDesc::HALF =" << TypeDesc::HALF << endl;

        //qDebug() << __FUNCTION__ <<  " ImageSpec_warpedBlockData.format        = " << ImageSpec_warpedBlockData.format;
        //cout << " ImgSpec_warpedBlockData.get_int_attribute(\"oiio:BitsPerSample\") = " << ImgSpec_warpedBlockData.get_int_attribute("oiio:BitsPerSample") << endl;

        //cout << "ImgSpec_warpedBlockData.scanline_bytes()        = " << ImgSpec_warpedBlockData.scanline_bytes() << endl;
        //cout << "ImageSpec_warpedResampledBlock.scanline_bytes() = " << ImgSpec_warpedResampledBlock.scanline_bytes() << endl;

        ROI roiResampled(
          0, wOutputImg,
          0, nbLineToWriteByBlock,
          0, 1, //z
          0, channels);

        if (!roiResampled.defined()) {
            qDebug() << "*** error: if (!roiResampled.defined())" ;

            bNoError = false;
            break;
        }
        qDebug() << "roiResampled(w,h): "
                 << roiResampled.width() << "," << roiResampled.height()
                 << " ( depth: " << roiResampled.depth()
                 << " nchannels: " << roiResampled.nchannels() << endl;

        //read, resample and write all full blocks

        int nbFullBlockToProcess = hInputImg / nbLineToReadByBlock;
        int yWriteScanLine = 0;

        qDebug() << "nbFullBlockToProcess = " << nbFullBlockToProcess;

        for (int iFullBlock = 0; iFullBlock < nbFullBlockToProcess; iFullBlock++) {

            qDebug() << "loop: iFullBlock = " << iFullBlock;

            //get pixel from source:
            int x0 = 0;
            int x1 = wInputImg;
            int y0 = iFullBlock*nbLineToReadByBlock;
            int y1 = y0 + nbLineToReadByBlock;

            qDebug() << "cache->get_pixels : " << x0 << ", " << x1 << ", " << y0 << ", " <<  y1;

            //get data into vector
            bool bCacheGotPixel = cache->get_pixels (
                ustring(strInputFile), //ustring filename,
                0, 0,                  //int subimage, int miplevel
                x0, x1, y0, y1,        //int xbegin, int xend,int ybegin, int yend,
                0, 1,                  //int zbegin, int zend,
                0, channels,           //int chbegin, int chend,
                typeDescBasetype, //TypeDesc::FLOAT,        //TypeDesc format,
                (void*)uptrVectorTypeROIdataFullBlock->data(), //void *result,
                 AutoStride, AutoStride        //stride_t xstride=AutoStride, stride_t ystride=AutoStride
            ); //cache_chbegin, cache_chend = -1

            if (!bCacheGotPixel) {
                qDebug() << "global geterror() = " << geterror().c_str();
                qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                bNoError = false;
                break;
            }

            qDebug() << __FUNCTION__ <<  "****** get_pixels done!";

            //cout << "total bytes read by cache =" << cache->bytes_read() << endl;

            //cout << "cache->get_pixels: done" << endl;


            bool bResizeSuccess = false;
            {
                ImageSpec ImageSpec_warpedBlockData     (wInputImg , nbLineToReadByBlock , channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));
                ImageSpec ImageSpec_warpedResampledBlock(wOutputImg, nbLineToWriteByBlock, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

                //warp vector to ImageBuf
                ImageBuf ImageBufBlock   (ImageSpec_warpedBlockData, (void*)/*vectROIdataFullBlock.*/uptrVectorTypeROIdataFullBlock->data());

                //resample to destBlock
                ImageBuf ImageBufResampledBlock(ImageSpec_warpedResampledBlock, (void*)/*vectROIdataFullBlockResampled.*/uptrVectorTypeROIdataFullBlockResampled->data());

                //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => gaussian 3.0 ===> 6m30,940s (SSD)
                //12GB ZLI 2 -> 32   w/ hROIRead = 1024 => default ========>           (SSD)
                //bResizeSuccess = ImageBufAlgo::resize(ImageBufResampledBlock, ImageBufBlock, oiioStrView_resizeFilter, .0f, roiResampled, nbThreadsToUse);

                //test resample instead of resize to see if it crashes with float and thread:
                //resample instead of resize (resize on float type and thread => crash on MacOSX)
                bResizeSuccess = ImageBufAlgo::resample(ImageBufResampledBlock, ImageBufBlock, true, roiResampled, nbThreadsToUse);
            }

            qDebug() << __FUNCTION__ <<  "bResizeSuccess: " << bResizeSuccess;

            if (!bResizeSuccess) {

                qDebug() << "error: resize failed on full block #" << iFullBlock;

                qDebug() << "global geterror() = " << geterror().c_str();
                qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                bNoError = false;
                break;
            }

            qDebug() << __FUNCTION__ <<  "bUseMany write_scanlines( y->y : " << yWriteScanLine << "-> " << yWriteScanLine+nbLineToWriteByBlock;

            bool bSuccessWrite = ImgOut->write_scanlines(
                yWriteScanLine,
                yWriteScanLine+nbLineToWriteByBlock,
                0,
                typeDescBasetype, //TypeDesc::FLOAT,//TypeDesc::UNKNOWN,

                uptrVectorTypeROIdataFullBlockResampled->data(), AutoStride, AutoStride);

            if (!bSuccessWrite) {
                qDebug() << "error in write_scanlines";
                qDebug() << "global geterror() = " << geterror().c_str();
                qDebug() << "cache-> geterror() = " << cache-> geterror().c_str();

                bNoError = false;
                break;
            } else {
                qDebug() << __FUNCTION__ <<  "write_scanlines ok! (many)";
            }

            yWriteScanLine+=nbLineToWriteByBlock;

            qDebug() << __FUNCTION__ << __LINE__;

            currentStep++;
            emit signal_stepProgressionForOutside(static_cast<int>( static_cast<float>(currentStep) * stepFractionFor0to100Range));

        }
        if (!bNoError) {

            break;
        }

        qDebug() << __FUNCTION__ <<  "ptrVectorTypeROIdataFullBlock @ = "          << (void*)uptrVectorTypeROIdataFullBlock->data();
        qDebug() << __FUNCTION__ <<  "ptrVectorTypeROIdataFullBlockResampled @ = " << (void*)uptrVectorTypeROIdataFullBlockResampled->data();

        qDebug() << __FUNCTION__ <<  "nbLineRemainingForLastBlock = " << nbLineRemainingForLastBlock;

        //remaining last not full block

        if (nbLineRemainingForLastBlock) {

            switch (typeDescBasetype) {

                case OIIO::TypeDesc::BASETYPE::FLOAT:
                    uptrVectorTypeROIdataRemainingBlock = make_unique<VectorFloat>();
                    uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorFloat>();
                    qDebug() << __FUNCTION__ << "new VectorFloat";
                    break;

                case OIIO::TypeDesc::BASETYPE::UINT8:
                    uptrVectorTypeROIdataRemainingBlock = make_unique<VectorUint8>();
                    uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorUint8>();
                    qDebug() << __FUNCTION__ << "new VectorUint8";
                    break;

                case OIIO::TypeDesc::BASETYPE::INT16: //signed 16 bits integer
                    uptrVectorTypeROIdataRemainingBlock = make_unique<VectorSignedInt16>();
                    uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorSignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorSignedInt16";
                    break;

                case OIIO::TypeDesc::BASETYPE::UINT16: //unsigned 16 bits integer
                    uptrVectorTypeROIdataRemainingBlock = make_unique<VectorUnsignedInt16>();
                    uptrvectorTypeROIdataRemainingResampledBlock = make_unique<VectorUnsignedInt16>();
                    qDebug() << __FUNCTION__ << __LINE__ << "new VectorUnsignedInt16";
                    break;

                default:
                    qDebug() << __FUNCTION__ << "error: invalid typeDescBasetype:" << typeDescBasetype;
                    bNoError = false;
            }

            if (!bNoError) {

                break;
            }

            uptrVectorTypeROIdataRemainingBlock->resize(wInputImg * nbLineRemainingForLastBlock * channels);
            qDebug() << __FUNCTION__ << "vectROIdataRemainingBlock size: " << uptrVectorTypeROIdataRemainingBlock->size();

            int nbLineToWriteForLastBlock = hOutputImg - (nbFullBlockToProcess * nbLineToWriteByBlock);
            qDebug() << __FUNCTION__ << "nbLineToWriteForLastBlock = " << nbLineToWriteForLastBlock;

            uptrvectorTypeROIdataRemainingResampledBlock->resize(wOutputImg * nbLineToWriteForLastBlock * channels);
            qDebug() << __FUNCTION__ << "vectROIdataRemainingBlockResampled size: " << uptrvectorTypeROIdataRemainingResampledBlock->size();

            //@#LP opt: possible to avoid to allocate ImgBlock each time ?

            int gpx0 = 0;
            int gpx1 = wInputImg;
            int gpy0 = hInputImg-nbLineRemainingForLastBlock;
            int gpy1 = hInputImg;

            qDebug() << __FUNCTION__ << "remaning cache->get_pixels: " << gpx0 << ", "<< gpx1 << ", "<< gpy0 << ", "<< gpy1;
            //get data into vector
            bool bCacheGotPixel = cache->get_pixels (
                ustring(strInputFile), //ustring filename,
                0, 0,                  //int subimage, int miplevel
                gpx0, gpx1, gpy0, gpy1,//int xbegin, int xend,int ybegin, int yend,
                0, 1,                  //int zbegin, int zend,
                0, channels,           //int chbegin, int chend,
                typeDescBasetype, //TypeDesc::FLOAT,         //TypeDesc format,
                (void*)uptrVectorTypeROIdataRemainingBlock->data(), //void *result,
                AutoStride, AutoStride        //stride_t xstride=AutoStride, stride_t ystride=AutoStride
            ); //int cache_chbegin = 0, int cache_chend = -1

            if (!bCacheGotPixel) {
                qDebug() << __FUNCTION__ << "global geterror() = " << geterror().c_str();
                qDebug() << __FUNCTION__ << "cache-> geterror() = " << cache->geterror().c_str();

                bNoError = false;
                break;
            }

            //cout << "cache->get_pixels: done" << endl;

            ROI roiResampled_remainingBlock(
            0, wOutputImg,
            0, nbLineToWriteForLastBlock,
            0, 1, //z
            0, channels);

            //qDebug() << "roiResampled_remainingBlock: " << x0_remaining << ", " <<  x1_remaining << " ; " <<  y0_remaining << ", " <<  y1_remaining;

            qDebug() << __FUNCTION__ << "roiResampled_remainingBlock: xbegin, xend; ybegin, yend: "
                   << roiResampled_remainingBlock.xbegin << ", " <<  roiResampled_remainingBlock.xend << " ; "
                   << roiResampled_remainingBlock.ybegin << ", " <<  roiResampled_remainingBlock.yend;

            bool bResizeSuccess = false;
            {
                ImageSpec ImgSpec_warpedRemainingBlockData     ( wInputImg, nbLineRemainingForLastBlock, channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));
                ImageSpec ImgSpec_warpedRemainingResampledBlock(wOutputImg, nbLineToWriteForLastBlock  , channels, /*ImageBufInput.spec().format*/TypeDesc(typeDescBasetype));

                //warp vector to ImageBuf
                ImageBuf ImageBuf_remainingBlock(ImgSpec_warpedRemainingBlockData, (void*)/*vectROIdataRemainingBlock.*/uptrVectorTypeROIdataRemainingBlock->data());

                //@#LP opt: possible to avoid to allocate ImgBlock each time ?
                //resample to destBlock
                ImageBuf ImageBuf_remainingResampledBlock(ImgSpec_warpedRemainingResampledBlock, (void*)/*vectROIdataRemainingResampledBlock.*/uptrvectorTypeROIdataRemainingResampledBlock->data());

                //bResizeSuccess = ImageBufAlgo::resize(ImageBuf_remainingResampledBlock, ImageBuf_remainingBlock, oiioStrView_resizeFilter, 0, roiResampled_remainingBlock, nbThreadsToUse);
                //test resample instead of resize to see if it crashes with float and thread:
                bResizeSuccess = ImageBufAlgo::resample(ImageBuf_remainingResampledBlock, ImageBuf_remainingBlock, true, roiResampled_remainingBlock, nbThreadsToUse);
            }

            qDebug() << __FUNCTION__ << "bResizeSuccess: " << bResizeSuccess;
            if (!bResizeSuccess) {

                qDebug() << __FUNCTION__ << "error: resize failed on roiResampled_remainingBlock";

                qDebug() << __FUNCTION__ << "global geterror() = " << geterror().c_str();
                qDebug() << __FUNCTION__ << "cache-> geterror() = " << cache-> geterror().c_str();

                bNoError = false;
                break;
            }

            //yWriteScanLine = nbLineToWriteByBlock * nbFullBlockToProcess;
            qDebug() << __FUNCTION__ << "many write_scanlines( y->y : " << yWriteScanLine << "-> " << yWriteScanLine+nbLineToWriteForLastBlock;

            bool bSuccessWrite = ImgOut->write_scanlines(
            yWriteScanLine,
            yWriteScanLine+nbLineToWriteForLastBlock,
            0,
            typeDescBasetype, //TypeDesc::FLOAT,//TypeDesc::UNKNOWN,

            uptrvectorTypeROIdataRemainingResampledBlock->data(), AutoStride, AutoStride);

            if (!bSuccessWrite) {
                qDebug() << __FUNCTION__ << "error in write_scanlines";
            } else {
                qDebug() << __FUNCTION__ << "write_scanlines ok (many)!";
            }

            currentStep++;
            emit signal_stepProgressionForOutside(static_cast<int>( static_cast<float>(currentStep) * stepFractionFor0to100Range));

        }

        qDebug() << __FUNCTION__ << __LINE__;

        bool bimgOutCloseOK = ImgOut->close();
        qDebug() << __FUNCTION__  << __LINE__ << "bimgOutCloseOK = " << bimgOutCloseOK;

        cache->close(ustring(strInputFile));

        uptrVectorTypeROIdataFullBlock.reset(nullptr);

        uptrVectorTypeROIdataFullBlockResampled.reset(nullptr);

        uptrVectorTypeROIdataRemainingBlock.reset(nullptr);

        uptrvectorTypeROIdataRemainingResampledBlock.reset(nullptr);

    }

    cache->close_all();
    ImageCache::destroy(cache, true); //@#LP try with teardown true

    //qint64 elapsedTime = timer.elapsed();
    //qDebug() << __FUNCTION__ << "took :" << elapsedTime << " ms ";

    return(bNoError);
}




bool ZoomOutLevelImageBuilder::computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(int32_t whsize,
                                                                                                                        int& stepAmount,
                                                                                                                        int& stepAmount_startAtZLI4) {

    stepAmount = 0;
    stepAmount_startAtZLI4 = 0;

    ImageSpec inputImgSpec;
    bool bImageSpecGot = InputImageFormatChecker::getImageSpec(_qstrRootImage.toStdString(), inputImgSpec);
    if (!bImageSpecGot) {
        qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << _qstrRootImage;
        return(false);
    }

    int wInputImg = inputImgSpec.width;
    int hInputImg = inputImgSpec.height;

    QHash<int, QSize> qhash_ZLI_QsizeWHForEachZLI;

    int maxZoomLevel = 1;

    //find the maximum level which will need tp be built
    {
        int wOutputImg = wInputImg;
        int hOutputImg = hInputImg;

        bool bNoMoreZoomOut = false;

        while(1) {

            qDebug() << "whsize = " << whsize;

            if ((wOutputImg <= whsize) && (hOutputImg <= whsize)) {
                qDebug() << "if ((wOutputImg <= 1024) && (hOutputImg <= 1024)) {";
                bNoMoreZoomOut = true;
                break;
            }

            wOutputImg = wInputImg;
            hOutputImg = hInputImg;

            maxZoomLevel *= 2;

            //compute output size, adjusting input size to one pixel upper if odd
            for (int zLvl = 2; zLvl <= maxZoomLevel; zLvl*=2) {
                wOutputImg = (wOutputImg+wOutputImg%2)/2;
                hOutputImg = (hOutputImg+hOutputImg%2)/2;
            }

            qhash_ZLI_QsizeWHForEachZLI.insert(maxZoomLevel, {wOutputImg, hOutputImg});

            qDebug() << "wOutputImg = " << wOutputImg << ", hOutputImg = " << hOutputImg;
        }

        qDebug() << "maxZoomLevel = " << maxZoomLevel;

        if (maxZoomLevel == 1) {
            return(true);
        }

    }

    bool bNoError = true;

    for (int zLvl = 2; zLvl <= maxZoomLevel; zLvl*=2) {

        qDebug() << "zLvl = " << zLvl;

        string strInputFile = _qstrRootImage.toStdString();

        qDebug() << "strInputFile = " << strInputFile.c_str();

        int channels = inputImgSpec.nchannels;
        for (int c = 0; c < inputImgSpec.nchannels; ++c) {
            qDebug() << " _ inputImgSpec.channelformat: " << inputImgSpec.channelformat(c).basetype;
        }
        qDebug() << "InputImg(w,h,c) " << wInputImg << " x " << hInputImg << "ch: " << channels;

        qInfo() << __FUNCTION__ << "building Zoom Level Image from " << strInputFile.c_str();

        int wOutputImg = qhash_ZLI_QsizeWHForEachZLI.value(zLvl).width();
        int hOutputImg = qhash_ZLI_QsizeWHForEachZLI.value(zLvl).height();

        qDebug() << __FUNCTION__ << "wOutputImg : " <<  wOutputImg;
        qDebug() << __FUNCTION__ << "hOutputImg : " <<  hOutputImg;

        qDebug() << "(while1) OutputImg(w,h)      : " << wOutputImg << ", " << hOutputImg;
        qDebug() << "(while1) SrcImgForResize(w,h): " << wInputImg  << ", " << hInputImg;

        int readDataBlockMaxSizeInBytes =  64*1024*1024;
        qDebug() << "readDataBlockMaxSizeInBytes : " << readDataBlockMaxSizeInBytes;

        int sizeOfAPixelMultiplyByNbChannels = static_cast<int>(inputImgSpec.format.size()) * inputImgSpec.nchannels;
        qDebug() << "sizeOfUnitMultiplyChannels : " << sizeOfAPixelMultiplyByNbChannels;

        int nbLineToReadByBlock = ((readDataBlockMaxSizeInBytes / (wInputImg * sizeOfAPixelMultiplyByNbChannels))/2)*2; //avoid odd nb lines value
        qDebug() << "step#1: nbLineToReadByBlock : " << nbLineToReadByBlock;

        int nearestPow2_of_nbLineToReadByBlock = nearestPow2(nbLineToReadByBlock);
        //cout << "step#2: nearestPow2_of_nbLineToReadByBlock : " << nearestPow2_of_nbLineToReadByBlock;cout << endl;
        if (nearestPow2_of_nbLineToReadByBlock > nbLineToReadByBlock) {
            nearestPow2_of_nbLineToReadByBlock /= 2;
        }
        nbLineToReadByBlock = nearestPow2_of_nbLineToReadByBlock;

        int nbLineToWriteByBlock = nbLineToReadByBlock / zLvl;
        qDebug() << "step#2: nbLineToReadByBlock  : " << nbLineToReadByBlock;

        //set the limit if the image is small enough to be read in one block:
        if ((int)nbLineToReadByBlock > hInputImg) {
            qDebug() << "step#2-3: the whole image fits in one block";

            nbLineToReadByBlock = hInputImg;
            nbLineToWriteByBlock = hOutputImg;
        }

        int nbLineRemainingForLastBlock = hInputImg % nbLineToReadByBlock;

        qDebug() << "step#3: nbLineToReadByBlock = " << nbLineToReadByBlock;
        qDebug() << "step#3: nbLineToWriteByBlock = " << nbLineToWriteByBlock;
        qDebug() << "step#3: nbLineRemainingForLastBlock = " << nbLineRemainingForLastBlock;

        //- computation of nbLineToReadByBlock take into account the ammoutn of buffer in bytes
        //- any size of block belows already implicitely handles the pixel size type due to template type usage vector<pixel size type>

        //-------------
        uint32_t sizeFullBlock = wInputImg * nbLineToReadByBlock * channels;

        int nbFullBlockToProcess = hInputImg / nbLineToReadByBlock;
        qDebug() << "nbFullBlockToProcess = " << nbFullBlockToProcess;

        stepAmount += nbFullBlockToProcess;
        if (nbLineRemainingForLastBlock) {
            stepAmount++;
        }

        if (zLvl >= 4) {
            stepAmount_startAtZLI4 += nbFullBlockToProcess;
            if (nbLineRemainingForLastBlock) {
                stepAmount_startAtZLI4++;
            }
        }
    }

    return(true);
}


bool ZoomOutLevelImageBuilder::pixStats_NanCount(const ImageBuf ImageBufForNanCount, bool& bContainsNan) {

    bContainsNan = false;

    ImageBufAlgo::PixelStats pixStats;
    bool bStatsComputeSuccess = ImageBufAlgo::computePixelStats (pixStats, ImageBufForNanCount/*, roiToConsider*/);
    if (!bStatsComputeSuccess) {
        qDebug() << __FUNCTION__ << "error: computePixelStats failed";
        //@#LP add OIIO's get errors functions
        qDebug() << "TPWOD " << "global geterror() = [ " << geterror().c_str() << " ]";
        return(false);
    }

    size_t channelForNanCount = 0;
    qDebug() << __FUNCTION__ << "nancount = " << pixStats.nancount[channelForNanCount];
    bContainsNan = (pixStats.nancount[channelForNanCount] != 0);

    return(true);
}

