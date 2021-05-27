#ifndef PIXELPICKER_H
#define PIXELPICKER_H

#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagecache.h>
#include <string>
using namespace std;

using namespace OIIO;

#include <QDebug>

#include "io/InputImageFormatChecker.h"

#include "ImathVec.h"


#include "ImathLimits.h"
using namespace IMATH_INTERNAL_NAMESPACE;

#include "../vecttype.h"

//used to take pixel in the source image, typically ZoomLevelImage at 1

class PixelPicker {

public:
    PixelPicker();
    bool setInputImage(const string& strInputImageFilename,
                       bool bWithDataTypeCheck = false,
                       unsigned char typeDescBaseTypeWhichNeedsToMatch = TypeDesc::NONE);

    bool getNearestPixelValue_fromContinousPixelXY(Vec2<double> continousPixelXY, float& pixelValue);
    bool getBilinearInterpolatedPixelValue_fromContinousPixelXY(Vec2<double> continousPixelXY, float& pixelValue);

    bool imageContainsContinousPixelXY(Vec2<double> continousPixelXY, bool& bOnBorder);

    void clearAll();
    ~PixelPicker();
    bool getImageWidthHeight(int& width, int &height);

    //@#LP public access for unit test purpose:
    bool getOnePixel(int x, int y, void* pixelValue);
    bool getPixelBlock2x2(int x0Block, int y0Block, void* pixelDataBlock2x2);

private:
    float percentFactorForTargetedPixelValue_v2(float frac);

    bool typedDataValueToFloat(void *data, float& pixelValue);
    bool typedDataValueBlock2x2ToFloat(void *data, float* pixelValueBlock2x2);

private:

    ImageCache *_cache;
    bool _bImageSpecGot;
    string _strInputImageFilename;
    ImageSpec _imgSpec;
    ImageBuf _imgBuf;

    unique_ptr<VectorType> _uptrVectTypeBlock2x2;
    float _floatBlock2x2[4];

};

#endif // PIXELPICKER_H
