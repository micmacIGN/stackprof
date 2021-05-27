#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagecache.h>
#include <string>
using namespace std;

using namespace OIIO;

#include <QDebug>

#include "ImathVec.h"

#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "../vecttype.h"

#include "InputImageFormatChecker.h"

#include "PixelPicker.h"

float PixelPicker::percentFactorForTargetedPixelValue_v2(float frac) {
    float percentFactorForTargetedPixelV = .0;
    // 0.0... ...0.5... ...1.0  frac
    //-0.5... ...0.0... ...0.5  frac-0.5 = #1
    // ....1+#1... ....1-#1...  pct
    // 0.5... ...1.0... ...0.5  pct
    frac-=0.5f;
    if (frac < .0f) {
        percentFactorForTargetedPixelV = 1+frac;
    } else {
        percentFactorForTargetedPixelV = 1-frac;
    }
    return(percentFactorForTargetedPixelV);
}

bool PixelPicker::typedDataValueToFloat(void *data,float& pixelValue) {
    unsigned char* ptrCharData  = nullptr;
    signed short int* ptrSignedInt16Data= nullptr;
    float* ptrFloatData = nullptr;
    unsigned short int* ptrUnsignedInt16Data= nullptr;

    //static int first10 =0;

    switch (_imgSpec.channelformat(0).basetype) {
        case TypeDesc::UINT8:
            ptrCharData = reinterpret_cast<unsigned char*>(data);
            pixelValue = static_cast<float>(*ptrCharData);
            break;

        case TypeDesc::INT16://signed 16 bits integer
            ptrSignedInt16Data = reinterpret_cast<signed short int*>(data);
            pixelValue = static_cast<float>(*ptrSignedInt16Data);
            break;

        case TypeDesc::UINT16://unsigned 16 bits integer
            ptrUnsignedInt16Data = reinterpret_cast<unsigned short int*>(data);
            pixelValue = static_cast<float>(*ptrUnsignedInt16Data);
            break;

        case TypeDesc::FLOAT:
            ptrFloatData = reinterpret_cast<float*>(data);
            pixelValue = (*ptrFloatData);
            break;

        default:
            //qDebug() << __FUNCTION__ << ": error: unsupported basetype";;
            return(false);
            //break;
    }
    return(true);
}


bool PixelPicker::typedDataValueBlock2x2ToFloat(void *data, float* pixelValueBlock2x2) {
    unsigned char* ptrCharData  = nullptr;
    signed short int* ptrSignedInt16Data= nullptr;
    float* ptrFloatData = nullptr;    
    unsigned short int* ptrUnsignedInt16Data= nullptr;

    int iLoop = 0;

    switch (_imgSpec.channelformat(0).basetype) {
        case TypeDesc::UINT8:
            ptrCharData = reinterpret_cast<unsigned char*>(data);
            for (iLoop = 0; iLoop < 4; iLoop++) {
                *(pixelValueBlock2x2+iLoop) = static_cast<float>(*(ptrCharData+iLoop));
            }
            break;

        case TypeDesc::INT16://signed 16 bits integer
            ptrSignedInt16Data = reinterpret_cast<signed short int*>(data);

            for (iLoop = 0; iLoop < 4; iLoop++) {
                *(pixelValueBlock2x2+iLoop) = static_cast<float>(*(ptrSignedInt16Data+iLoop));
            }
            break;

        case TypeDesc::UINT16://unsigned 16 bits integer
           ptrUnsignedInt16Data = reinterpret_cast<unsigned short int*>(data);

           for (iLoop = 0; iLoop < 4; iLoop++) {
               *(pixelValueBlock2x2+iLoop) = static_cast<float>(*(ptrUnsignedInt16Data+iLoop));
           }
           break;

        case TypeDesc::FLOAT:
            ptrFloatData = reinterpret_cast<float*>(data);
            for (iLoop = 0; iLoop < 4; iLoop++) {
                *(pixelValueBlock2x2+iLoop) = (*(ptrFloatData+iLoop));
            }
            break;

        default:
            //qDebug() << __FUNCTION__ << ": error: unsupported basetype";
            return(false);
            //break;
    }
    return(true);
}


bool PixelPicker::imageContainsContinousPixelXY(Vec2<double> continousPixelXY, bool& bOnBorder) {
    int xtexel = 0, ytexel = 0;
    floorfrac(static_cast<float>(continousPixelXY.x), &xtexel);
    floorfrac(static_cast<float>(continousPixelXY.y), &ytexel);

    bOnBorder = false;

    if ((xtexel<0)||(xtexel>=_imgSpec.width)) {
        return(false);
    }

    if ((ytexel<0)||(ytexel>=_imgSpec.height)) {
        return(false);
    }

    if ((xtexel == 0) || (ytexel ==0)) {
        bOnBorder = true;
    } else {
        if ((xtexel == _imgSpec.width-1 ) || (ytexel == _imgSpec.height-1)) {
            bOnBorder = true;
        }
    }
    return(true);
}

bool PixelPicker::getNearestPixelValue_fromContinousPixelXY(Vec2<double> continousPixelXY, float& pixelValue) {

    int xtexel = 0, ytexel = 0;

    floorfrac(static_cast<float>(continousPixelXY.x), &xtexel);

    floorfrac(static_cast<float>(continousPixelXY.y), &ytexel);

    //qDebug() << "x => int: " << continousPixelXY.x << " => " << xtexel;
    //qDebug() << "y => int: " << continousPixelXY.y << " => " << ytexel;

    bool bBlockGot = getOnePixel(xtexel, ytexel, _uptrVectTypeBlock2x2->data());
    if (!bBlockGot) {
        qDebug() << __FUNCTION__ << " failed on getOnePixel";
        return(false);
    }
    return(typedDataValueToFloat(_uptrVectTypeBlock2x2->data(), pixelValue));
}



//this method computes the pixel value taking into account pixel which are more relevant according to the point location inside the pixel
//@LP: if there is Nan value in one of the four considered pixels, the method will return Nan.
//     Even if the continousPixelXY is perfectly centered on a 'not Nan' pixel.
bool PixelPicker::getBilinearInterpolatedPixelValue_fromContinousPixelXY(Vec2<double> continousPixelXY, float& pixelValue) {

    //find the 2x2 pixels block
    //the targeted pixel is not always the pixel at top left of the block
    //its position in the block depends on the decimal part of the pixel coordinates
    int xtexel = 0, ytexel = 0;
    float xfrac = .0, yfrac = .0;
    xfrac = floorfrac(static_cast<float>(continousPixelXY.x), &xtexel);
    yfrac = floorfrac(static_cast<float>(continousPixelXY.y), &ytexel);

    //qDebug() << __FUNCTION__ << "x => int.decimals: " << continousPixelXY.x << " => " << xtexel << " . " << xfrac;
    //qDebug() << __FUNCTION__ << "y => int.decimals: " << continousPixelXY.y << " => " << ytexel << " . " << yfrac;

    bool bBlockBuildable = true;
    if (xtexel == 0) {
        if (xfrac < 0.5f) {
            //can not take left
            bBlockBuildable = false;
        }
    }
    if (ytexel == 0) {
        if (yfrac < 0.5f) {
            //can not take top
            bBlockBuildable = false;
        }
    }
    if (bBlockBuildable) { //test to avoid some comparaisons if bBlockBuildable set to false above
        int lastXPixel = _imgBuf.spec().width-1;
        int lastYPixel = _imgBuf.spec().height-1;
        if (xtexel == lastXPixel) {
            if (xfrac > 0.5f) {
                //can not take right
                bBlockBuildable = false;
            }
        }
        if (ytexel == lastYPixel) {
            if (yfrac > 0.5f) {
                //can not take bottom
                bBlockBuildable = false;
            }
        }
    }

    if (!bBlockBuildable) {
        qDebug() << __FUNCTION__ << " can't build block => take simply the value of the targeted pixel ("<< xtexel << ", " << ytexel;


        bool bBlockGot = getOnePixel(xtexel, ytexel, _uptrVectTypeBlock2x2->data());
        if (!bBlockGot) {
            qDebug() << __FUNCTION__ << " failed on getOnePixel";
            return(false);
        }
        return(typedDataValueToFloat(_uptrVectTypeBlock2x2->data(),pixelValue));
    }

    //adjust the xfrac and yfrac depending of the location of the targeted pixel in the block
    int x0Block = xtexel;
    int y0Block = ytexel;

    bool bTargetedPixelAtRightSide = false;
    bool bTargetedPixelAtBottomSide = false;

    //targeted pixel is a pixel at right of the block
    if (xfrac < 0.5f) {
        x0Block--;
        bTargetedPixelAtRightSide = true;
        //qDebug() << "bTargetedPixelAtRightSide";
    }
    //targeted pixel is a pixel at bottom of the block
    if (yfrac < 0.5f) {
        y0Block--;
        bTargetedPixelAtBottomSide = true;
        //qDebug() << "bTargetedPixelAtBottomSide";
    }

    bool bBlockGot = getPixelBlock2x2(x0Block, y0Block, _uptrVectTypeBlock2x2->data());

    if (!bBlockGot) {
        qDebug() << __FUNCTION__ << " failed on getBlock2x2";
        return(false);
    }

    //qDebug() << __FUNCTION__ << "block got";

    float xPercentFactorForTargetedPixel = percentFactorForTargetedPixelValue_v2(xfrac);
    float yPercentFactorForTargetedPixel = percentFactorForTargetedPixelValue_v2(yfrac);
    //qDebug() << "v2: x y PercentFactorForTargetedPixel:" << xPercentFactorForTargetedPixel << ", " << yPercentFactorForTargetedPixel;

    float xPercentFactorForX0Block = .0;
    if (!bTargetedPixelAtRightSide)  {
        xPercentFactorForX0Block = xPercentFactorForTargetedPixel;
    } else {
        xPercentFactorForX0Block = 1.0f - xPercentFactorForTargetedPixel;
    }

    float yPercentFactorForY0Block = .0;
    if (!bTargetedPixelAtBottomSide) {
        yPercentFactorForY0Block = yPercentFactorForTargetedPixel;
    } else {
        yPercentFactorForY0Block = 1.0f - yPercentFactorForTargetedPixel;
    }

    //qDebug() << "x y PercentFactorForX0y0Block :" << xPercentFactorForX0Block << ", " << yPercentFactorForY0Block;

    //qDebug() << "xPercentFactorForX0Block =" << xPercentFactorForX0Block;
    //qDebug() << "ypercentFactorForY0Block =" << yPercentFactorForY0Block;

    memset(_floatBlock2x2, 0, 4*sizeof(float));
    if (!typedDataValueBlock2x2ToFloat(_uptrVectTypeBlock2x2->data(), _floatBlock2x2)) {
        return(false);
    }

    float fh01 = (_floatBlock2x2[0]*xPercentFactorForX0Block) + (_floatBlock2x2[1]*(1.0f-xPercentFactorForX0Block));
    float fh23 = (_floatBlock2x2[2]*xPercentFactorForX0Block) + (_floatBlock2x2[3]*(1.0f-xPercentFactorForX0Block));

    pixelValue = (fh01*yPercentFactorForY0Block) + (fh23*(1.0f-yPercentFactorForY0Block));

    qDebug() << __FUNCTION__ <<  "fh01 = " << fh01 << " = " << _floatBlock2x2[0] << " * " << xPercentFactorForX0Block << " + " << _floatBlock2x2[1] << " * " << (1.0f-xPercentFactorForX0Block);
    qDebug() << __FUNCTION__ <<  "fh23 = " << fh23 << " = " << _floatBlock2x2[2] << " * " << xPercentFactorForX0Block << " + " << _floatBlock2x2[3] << " * " << (1.0f-xPercentFactorForX0Block);
    qDebug() << __FUNCTION__ <<  "pixelValue   = " << pixelValue   << " = " << fh01 << " * " << yPercentFactorForY0Block << " + " << fh23 << "*" << (1.0f-yPercentFactorForY0Block);


    return(true);
}

PixelPicker::PixelPicker(): _cache(nullptr), _bImageSpecGot(false) {
}

bool PixelPicker::getImageWidthHeight(int& width, int &height) {
    if (!_cache) {
        return(false);
    }
    width = _imgSpec.width;
    height = _imgSpec.height;
    return(true);
}

bool PixelPicker::setInputImage(const string& strInputImageFilename,
                                bool bWithDataTypeCheck, unsigned char typeDescBaseTypeWhichNeedsToMatch) {

    clearAll();

    _strInputImageFilename = strInputImageFilename;

    //Create an image cache and set some options
    _cache = ImageCache::create(false);
    if (!_cache) {
        return(false);
    }

    bool bSuccessAttributeSet = true;
    bSuccessAttributeSet &= _cache->attribute("max_memory_MB", 128.0);
    bSuccessAttributeSet &= _cache->attribute("autotile", 64); //TBC value
    bSuccessAttributeSet &= _cache->attribute("autoscanline", 0); //square tile (autotile != 0)
    bSuccessAttributeSet &= _cache->attribute("unassociatedalpha",1);

    InputImageFormatChecker iifc;
    if (!iifc.check(_strInputImageFilename)) {
        qDebug() << "error: InputImageFormatChecker rejects file: " << _strInputImageFilename.c_str();
        clearAll();
        return(false);
    }
    _bImageSpecGot = iifc.getImageSpec(_strInputImageFilename, _imgSpec);
    if (!_bImageSpecGot) {
        qDebug() << __FUNCTION__ << ": error: can not get ImageSpec of file " << _strInputImageFilename.c_str();
        clearAll();
        return(false);
    }

    //consider the first channel for the basetype
    //set the cache attribute to float if basetype is float
    unsigned char typeDescBaseType = _imgSpec.channelformat(0).basetype;

    if (bWithDataTypeCheck) {
        if (typeDescBaseType != typeDescBaseTypeWhichNeedsToMatch) {
            qDebug() << __FUNCTION__ << ": error: typeDesBaseType of file dos not match with needed. Want: " <<  typeDescBaseTypeWhichNeedsToMatch << " Got: " << typeDescBaseType;
            clearAll();
            return(false);
        }
    }

    switch (typeDescBaseType) {

        case TypeDesc::UINT8:
        case TypeDesc::INT16://signed 16 bits integer
        case TypeDesc::UINT16://unsigned 16 bits integer (background image to draw trace over only)
        case TypeDesc::FLOAT:
            bSuccessAttributeSet &= _cache->attribute("forcefloat", 1);
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
        clearAll();
        return(false);
    }


    bool bAllocBlock2x2Done = true;
    switch (typeDescBaseType) {
      case TypeDesc::UINT8:
          _uptrVectTypeBlock2x2 = make_unique<VectorUint8>();
          break;

      case TypeDesc::INT16://signed 16 bits integer
          _uptrVectTypeBlock2x2 = make_unique<VectorSignedInt16>();
          break;

      case TypeDesc::UINT16://unsigned 16 bits integer
          _uptrVectTypeBlock2x2 = make_unique<VectorUnsignedInt16>();
          break;

      case TypeDesc::FLOAT:
         _uptrVectTypeBlock2x2 = make_unique<VectorFloat>();
          break;

      default:
          qDebug() << __FUNCTION__ << ": error: unsupported basetype:" << typeDescBaseType;
          bAllocBlock2x2Done = false;
          break;
    }

    if (!bAllocBlock2x2Done) {
      clearAll();
      return(false);
    }

    _uptrVectTypeBlock2x2->resize(4);

    int subimage = 0;
    int miplevel = 0;
    _imgBuf.reset(_strInputImageFilename, subimage, miplevel, _cache);

    qDebug() << "_imgBuf.image_pixels() = " << _imgBuf.spec().image_pixels();
    qDebug() << "_imgBuf.spec().width   = " << _imgBuf.spec().width;
    qDebug() << "_imgBuf.spec().height  = " << _imgBuf.spec().height;

    return(true);
}

bool PixelPicker::getOnePixel(int x, int y, void* pixelValue) {
     if (!_cache) {
         qDebug() << __FUNCTION__ << "error: missing init using setInputImage() ";
         return(false);
     }
     ROI roiOnePixel { //xbegin, xend; int ybegin, yend; int zbegin, zend; int chbegin, chend;
         x, x+1,
         y, y+1,
         0,   1,
         0,   1
     };

     //qDebug() << __FUNCTION__ << "_imgSpec.format.basetype = " << _imgSpec.format.basetype;
     //qDebug() << __FUNCTION__ << "_imgBuf.pixeltype() = " << _imgBuf.pixeltype();

     float fpixelValue = .0;
     bool bReport_get_pixels = _imgBuf.get_pixels(roiOnePixel,
                                                  TypeDesc::FLOAT, //@LP: forced float hence FLOAT, or use instead type from the _imgBuf/cache
                                                  &fpixelValue, AutoStride, AutoStride, AutoStride);
     //cout << "fpixelValue = " << fpixelValue;

     /*qDebug() << __FUNCTION__ <<"roi x  b/e:" << roiOnePixel.xbegin << ", " << roiOnePixel.xend;
     qDebug() << __FUNCTION__ << "roi y  b/e:" << roiOnePixel.ybegin << ", " << roiOnePixel.yend;
     qDebug() << __FUNCTION__ << "roi z  b/e:" << roiOnePixel.zbegin << ", " << roiOnePixel.zend;
     qDebug() << __FUNCTION__ << "roi ch b/e:" << roiOnePixel.chbegin << ", " << roiOnePixel.chend;*/

     if (!bReport_get_pixels) {
         qDebug() << __FUNCTION__ << "failed to get_pixels ";
         qDebug() << __FUNCTION__ << "roi x  b/e:" << roiOnePixel.xbegin << ", " << roiOnePixel.xend;
         qDebug() << __FUNCTION__ << "roi y  b/e:" << roiOnePixel.ybegin << ", " << roiOnePixel.yend;
         qDebug() << __FUNCTION__ << "roi z  b/e:" << roiOnePixel.zbegin << ", " << roiOnePixel.zend;
         qDebug() << __FUNCTION__ << "roi ch b/e:" << roiOnePixel.chbegin << ", " << roiOnePixel.chend;
         return(false);
     }

     //@LP not happy with the way to handle this specific case with INT16.
     //Reason: OIIO with forcefloat at 0 with INT16 give weird values. Hence, we use forcefloat at 1, and multitpy by SHRT_MAX to get the correct INT16 value

     /*qDebug() << __FUNCTION__ << "--------------------------------";
     qDebug() << "roi x  b/e:" << roiOnePixel.xbegin << ", " << roiOnePixel.xend;
     qDebug() << "roi y  b/e:" << roiOnePixel.ybegin << ", " << roiOnePixel.yend;
     qDebug() << "roi z  b/e:" << roiOnePixel.zbegin << ", " << roiOnePixel.zend;
     qDebug() << "roi ch b/e:" << roiOnePixel.chbegin << ", " << roiOnePixel.chend;

     qDebug() << __FUNCTION__ << "showing roiOnePixel got from get_pixels:";*/

     unsigned char* ptrCharData  = nullptr;
     signed short int* ptrSignedInt16Data= nullptr;
     float* ptrFloatData = nullptr;
     unsigned short int* ptrUnsignedInt16Data= nullptr;

     switch (_imgSpec.channelformat(0).basetype) {
         case TypeDesc::UINT8:
             ptrCharData = reinterpret_cast<unsigned char*>(pixelValue);
             //qDebug() << __FUNCTION__ << "UINT8: " << *ptrCharData;
             *ptrCharData = static_cast<unsigned char>(fpixelValue * UCHAR_MAX);
             break;

         case TypeDesc::INT16://signed 16 bits integer
             ptrSignedInt16Data = reinterpret_cast<signed short int*>(pixelValue);
             //qDebug() << __FUNCTION__ << "INT16: " << *ptrSignedInt16Data;
             *ptrSignedInt16Data = static_cast<signed short int>(fpixelValue * SHRT_MAX);
             break;

         case TypeDesc::UINT16://unsigned 16 bits integer
            ptrUnsignedInt16Data = reinterpret_cast<unsigned short int*>(pixelValue);
            //qDebug() << __FUNCTION__ << "INT16: " << *ptrUnsignedInt16Data;
            *ptrUnsignedInt16Data = static_cast<unsigned short int>(fpixelValue * USHRT_MAX);
             break;

         case TypeDesc::FLOAT:
             ptrFloatData = reinterpret_cast<float*>(pixelValue);
             //qDebug() << __FUNCTION__ << "FLOAT: " << *ptrFloatData;
             *ptrFloatData = fpixelValue;
             break;

     }
     //qDebug() << __FUNCTION__ << "________________________________";

     return(bReport_get_pixels);
}

bool PixelPicker::getPixelBlock2x2(int x0Block, int y0Block, void* pixelDataBlock2x2) {
    if (!_cache) {
        qDebug() << __FUNCTION__ << "error: missing init using setInputImage() ";
        return(false);
    }
    ROI roiBlock2x2 { //xbegin, xend; int ybegin, yend; int zbegin, zend; int chbegin, chend;
        x0Block, x0Block+2,
        y0Block, y0Block+2,
        0, 1,
        0, 1
    };

    //qDebug() << __FUNCTION__ << "_imgSpec.format.basetype = " << _imgSpec.format.basetype;
    //qDebug() << __FUNCTION__ << "_imgBuf.pixeltype() = " << _imgBuf.pixeltype();

    float fpixelValuesBlock2x2[4] {.0, .0, .0, .0};

    bool bReport_get_pixels = _imgBuf.get_pixels(roiBlock2x2,
                                                 TypeDesc::FLOAT,
                                                 fpixelValuesBlock2x2, AutoStride, AutoStride, AutoStride);

    if (!bReport_get_pixels) {
        qDebug() << "failed to get_pixels ";
        qDebug() << "roi x  b/e:" << roiBlock2x2.xbegin  << ", " << roiBlock2x2.xend;
        qDebug() << "roi y  b/e:" << roiBlock2x2.ybegin  << ", " << roiBlock2x2.yend;
        qDebug() << "roi z  b/e:" << roiBlock2x2.zbegin  << ", " << roiBlock2x2.zend;
        qDebug() << "roi ch b/e:" << roiBlock2x2.chbegin << ", " << roiBlock2x2.chend;
        return(false);
    }

    unsigned char* ptrCharData  = nullptr;
    signed short int* ptrSignedInt16Data= nullptr;
    float* ptrFloatData = nullptr;
    unsigned short int* ptrUnsignedInt16Data= nullptr;

    int i = 0;
    switch (_imgSpec.channelformat(0).basetype) {
        case TypeDesc::UINT8:
            ptrCharData = reinterpret_cast<unsigned char*>(pixelDataBlock2x2);
            //qDebug() << __FUNCTION__ << "UINT8: " << *ptrCharData;
            for (i = 0; i < 4; i++) {
                *(ptrCharData+i) = static_cast<unsigned char>(fpixelValuesBlock2x2[i] * UCHAR_MAX);
            }
            break;

        case TypeDesc::INT16://signed 16 bits integer
            ptrSignedInt16Data = reinterpret_cast<signed short int*>(pixelDataBlock2x2);
            //qDebug() << __FUNCTION__ << "INT16: " << *ptrSignedInt16Data;
            for (i = 0; i < 4; i++) {
                *(ptrSignedInt16Data+i) = static_cast<signed short int>(fpixelValuesBlock2x2[i] * SHRT_MAX);
            }
            break;

        case TypeDesc::UINT16://unsigned 16 bits integer
            ptrUnsignedInt16Data = reinterpret_cast<unsigned short int*>(pixelDataBlock2x2);
            //qDebug() << __FUNCTION__ << "INT16: " << *ptrUnsignedInt16Data;
            for (i = 0; i < 4; i++) {
                *(ptrUnsignedInt16Data+i) = static_cast<unsigned short int>(fpixelValuesBlock2x2[i] * USHRT_MAX);
            }
            break;

        case TypeDesc::FLOAT:
            ptrFloatData = reinterpret_cast<float*>(pixelDataBlock2x2);
            //qDebug() << __FUNCTION__ << "FLOAT: " << *ptrFloatData;
            for (i = 0; i < 4; i++) {
                *(ptrFloatData+i) = fpixelValuesBlock2x2[i];
            }
            break;
    }

    return(bReport_get_pixels);
}


void PixelPicker::clearAll() {
    _uptrVectTypeBlock2x2.reset(nullptr);
    _strInputImageFilename.clear();
    _bImageSpecGot = false;
    _imgBuf.clear();
    if (_cache) {
        _cache->close_all();
        ImageCache::destroy(_cache);
        _cache = nullptr;
    }
}

PixelPicker::~PixelPicker() {
    clearAll();
}
