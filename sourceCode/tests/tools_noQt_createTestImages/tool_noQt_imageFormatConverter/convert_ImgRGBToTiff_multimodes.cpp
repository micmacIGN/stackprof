#include <math.h>

#include <cstdlib> //from srand and atof
#include <ctime> //for srand init

#include <vector>
#include <string>
using namespace std;

#include <OpenImageIO/typedesc.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
using namespace OIIO;

bool getImageSpec(const string& strInputFilename, ImageSpec& imgSpec);

int convertImage_RGBThreeChannels_to_floatTiff(
    const string& strInputPngFile,
    double valueForPngMinGreenChannelValue,
    double valueForPngMaxGreenChannelValue,
    const string& strOutputTiffFile);

int convertImage_RGBThreeChannels_to_floatTiff_RGB_WhiteBlack_magentaIsNan_otherNotNanAs(
    const string& strInputPngFile,
    double valueForRed255,
    double valueForGreen255,
    double valueForBlue255,
    double valueForWhite,
    double valueForBlack,
    double valueForAllOther,
    const string& strOutputTiffFile);

int convertImage_RGBThreeChannels_to_byteTiffOneChannel_grayLevel_0_255(
    const string& strInputPngFile,
    const string& strOutputTiffFile);


void showHelp() {
    cout << "Convert RGB (3 bytes) images to tiff float or byte";
    cout << "Three modes are available: RGBWBM, RGBGRAY, GREEN2FLOAT" << endl;
    cout << endl;
    cout << "RGBWBM : . 'Convert' each pure colors by provided float values: Red Green Blue White Black Magenta" << endl;
    cout << "         . Magenta will be 'converted' to Nan" << endl;
    cout << "         . Only one channel in the output image" << endl;
    cout << "         . Args: <RGBWBM> <input file> <f.v. for Red> <f.v. for Green> <f.v. for Blue> <f.v. for White> <f.v.for Black> <f.v. for others color> <output file>" << endl;
    cout << endl;
    cout << "RGBGRAY: . 'Convert' each RGB pixel value by its mean value (1 byte)" << endl;
    cout << "         . Only one channel in the output image" << endl;
    cout << "         . Args: <RGBGRAY> <input file> <output file>" << endl;
    cout << endl;
    cout << "GREEN2FLOAT: . 'Convert' each Green channel pixel value to a float value in the provided range" << endl;
    cout << "             . The provided range match with the full byte scale for the channel: 0 to 255" << endl;
    cout << "             . magenta will be converted by Nan" << endl;
    cout << "             . Only one channel in the output image" << endl;
    cout << "             . Args: <GREEN2FLOAT> <input file> <output file> <floatMinRangeValue> <floatMaxRangeValue>" << endl;
    cout << endl;
}

int main(int argc, char*argv[]) {

  if (argc < 2) {
      showHelp();
      return(1);
  }

  enum e_Mode {
      e_M_notset,
      e_M_RGBWBM,
      e_M_RGBGRAY,
      e_M_GREEN2FLOAT
  };
  e_Mode eMode = e_M_notset;

  string strArgMode = argv[1];

  if (!strArgMode.compare("RGBWBM")) {
        eMode = e_M_RGBWBM;
        if (argc < 10) {
            showHelp();
            return(1);
        }
  }
  if (!strArgMode.compare("RGBGRAY")) {
        eMode = e_M_RGBGRAY;
        if (argc < 4) {
            showHelp();
            return(1);
        }
  }
  if (!strArgMode.compare("GREEN2FLOAT")) {
        eMode = e_M_GREEN2FLOAT;
        if (argc < 6) {
            showHelp();
            return(1);
        }
  }

  if (eMode == e_M_notset) {
      showHelp();
      return(1);
  }

  int iReport = 0;

  if (eMode == e_M_GREEN2FLOAT) {
    float fMinRangeValue = static_cast<float>(atof(argv[4]));
    float fMaxRangeValue = static_cast<float>(atof(argv[5]));
    cout << "arg range to float; fMinRangeValue: " << argv[4] << " => " << fMinRangeValue << endl;
    cout << "arg range to float; fMaxRangeValue: " << argv[5] << " => " << fMaxRangeValue << endl;
    if (fMinRangeValue>=fMaxRangeValue) {
      cout << "invalid arg range" << endl;
      iReport = 1;
    } else {
      iReport = convertImage_RGBThreeChannels_to_floatTiff(argv[2], fMinRangeValue, fMaxRangeValue, argv[3]);
    }
  } else { 
    if (eMode == e_M_RGBWBM) {

      vector<float> vectfloat_RGBWBO(6);
      for (int i = 0; i < 6; i++) {
            vectfloat_RGBWBO[i] = static_cast<float>(atof(argv[3+i]));
      }

      vector<string> vectStrRGBWBO {"Red", "Green", "Blue", "White", "Black", "Other"};
      for (int i = 0; i < 6; i++) {
        cout << vectStrRGBWBO[i] << "color will be converted to float value: " << vectfloat_RGBWBO[i] << endl;
      }

      iReport = convertImage_RGBThreeChannels_to_floatTiff_RGB_WhiteBlack_magentaIsNan_otherNotNanAs(argv[2],
                                                                                        vectfloat_RGBWBO[0], vectfloat_RGBWBO[1], vectfloat_RGBWBO[2],
                                                                                        vectfloat_RGBWBO[3], vectfloat_RGBWBO[4],
                                                                                        vectfloat_RGBWBO[5],
                                                                                        argv[9]);
    } else { //e_M_RGBGRAY
      iReport = convertImage_RGBThreeChannels_to_byteTiffOneChannel_grayLevel_0_255(argv[2], argv[3]);
    }
  }

  if (iReport) {
     cout << "failed converting." << endl;
  } else {
    cout << "conversion done." << endl;
  }

  return(iReport);
}

int convertImage_RGBThreeChannels_to_byteTiffOneChannel_grayLevel_0_255(
        const string& strInputPngFile,
        const string& strOutputTiffFile) {


    ImageSpec imgSpec_input;
    bool bReport = getImageSpec(strInputPngFile, imgSpec_input);
    if (!bReport) {
        cout << __FUNCTION__ << ": error: getImageSpec on file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    if (imgSpec_input.nchannels < 3) { //#LP TBC: png with alpha as 4
        cout << __FUNCTION__
             << ": error: imgSpec_input.channels < 3; (imgSpec_input.nchannels = " << imgSpec_input.nchannels
             << "; file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    ImageBuf IBufSrc(strInputPngFile);

    unsigned char cZero { 0 };

    //cout << __FUNCTION__ << __LINE__ << endl;

    vector<unsigned char> vectImageContent_RGBChannel[3];
    for (auto& iterChannelVect: vectImageContent_RGBChannel) {
        iterChannelVect.resize(imgSpec_input.width * imgSpec_input.height);
    }

    vector<unsigned char> vectImageContent_mean;
    vectImageContent_mean.resize(imgSpec_input.width * imgSpec_input.height);
    fill(vectImageContent_mean.begin(), vectImageContent_mean.end(), 0);

    const int redChannel = 0;
    const int greenChannel = 1;
    const int blueChannel = 2;

    for (int iChannel = redChannel; iChannel <= blueChannel; iChannel++) {

        ROI ROIFullImage_oneChannel {
            0, imgSpec_input.width,
            0, imgSpec_input.height,
            0, 1,
            iChannel, iChannel+1
        };

        fill(vectImageContent_RGBChannel[iChannel].begin(),
             vectImageContent_RGBChannel[iChannel].end(),
             cZero);

        bool bGetPixel = IBufSrc.get_pixels(ROIFullImage_oneChannel, TypeDesc(TypeDesc::UINT8),
                                              vectImageContent_RGBChannel[iChannel].data(),
                                              AutoStride, AutoStride, AutoStride);
        if (!bGetPixel) {
            cout << __FUNCTION__ << " ImageBufAlgo::get_pixels(...) failed (iChannel = " << iChannel << " )" << endl;
            return(1);
        }
    }

    //output image

    int outNbChannels = 1;

    ImageSpec ImageSpecOut(imgSpec_input.width,
                           imgSpec_input.height,
                           outNbChannels,
                           TypeDesc(TypeDesc::UINT8));

    cout << "imgSpec_input.width    = " << imgSpec_input.width << endl;
    cout << "imgSpec_input.height   = " << imgSpec_input.height << endl;
    cout << "ImageSpecOut.nchannels = " << ImageSpecOut.nchannels << endl;

    int bitsPerSample = (int)(8*ImageSpecOut.format.size());

    ImageSpecOut.attribute("oiio::BitsPerSample", bitsPerSample);
    ImageSpecOut.attribute("compression", "none");
    ImageSpecOut.attribute("tiff:write exif", int(1));
    ImageSpecOut.attribute("tiff:Compression", (int)1);

    ImageBuf IBufDest(strOutputTiffFile, ImageSpecOut);

    ROI dest_ROIFullImage {
        0, ImageSpecOut.width,
        0, ImageSpecOut.height,
        0, 1, //z
        0, outNbChannels };

    bool bFill = ImageBufAlgo::fill(IBufDest, cZero, dest_ROIFullImage);
    if (!bFill) {
        cout << "ImageBufAlgo::fill failed(...)" << endl;
        return(1);
    }

    long int offset = 0l;
    for (int y = 0; y < ImageSpecOut.height; y++) {
        for (int x = 0; x < ImageSpecOut.width; x++) {

            int intPixelValueRed   = static_cast<float>(vectImageContent_RGBChannel[redChannel  ][offset]);
            int intPixelValueGreen = static_cast<float>(vectImageContent_RGBChannel[greenChannel][offset]);
            int intPixelValueBlue  = static_cast<float>(vectImageContent_RGBChannel[blueChannel ][offset]);

            int intPixelValueRGBSum = intPixelValueRed + intPixelValueGreen + intPixelValueBlue;
            int intPixelMeanValue = intPixelValueRGBSum/3;

            //float fPixelMeanValue = static_cast<float>(intPixelMeanValue)/255.0;

            //cout << "f R,G,B: " << fPixelValueRed << ", " << fPixelValueGreen << ", " << fPixelValueBlue << "Sum = " << fPixelValueRGBSum << endl;
            //cout << " intPixelMeanValue = " << intPixelMeanValue << " => setpixel( " << x << ", " << y << ") with: " << fPixelMeanValue << endl;

            //cout << " intPixelMeanValue = " << intPixelMeanValue << endl;

            vectImageContent_mean[offset] = static_cast<unsigned char>(intPixelMeanValue);

            //could use setpixel(...) with float value (using with : float fPixelMeanValue = static_cast<float>(intPixelMeanValue)/255.0) insstead of set_pixels(...)
            //IBufDest.setpixel(x, y, 0, &fPixelMeanValue, outNbChannels);

            /*if (!x) {
                cout << "(green) x, y, in, out: ( " << x << ", " << y << " ) in: " <<  static_cast<int>(vectImageContent_RGBChannel[greenChannel][offset]) << " => out: " << fPixelValue << endl;
            }*/

            offset++;
        }
    }

    cout << __FUNCTION__ << "IBufDest.pixeltype() = " << IBufDest.pixeltype() << endl;
    bool bSetPixel = IBufDest.set_pixels(dest_ROIFullImage, IBufDest.pixeltype(), vectImageContent_mean.data(), AutoStride, AutoStride, AutoStride);
    if (!bSetPixel) {
        cout << __FUNCTION__ << "ImageBufAlgo::set_pixels(...) failed" << endl;
        return(false);
    }

    bool bSuccessWrite = IBufDest.write(strOutputTiffFile, TypeUnknown/*TypeDesc::UINT8*/);
    if (!bSuccessWrite) {
        cout << "failed to write " << strOutputTiffFile << endl;
        cout << "geterror():" << geterror() << endl;
        return(1);
    }

    cout << "write(...) done" << endl;

    return(0);


}


int convertImage_RGBThreeChannels_to_floatTiff_RGB_WhiteBlack_magentaIsNan_otherNotNanAs(
    const string& strInputPngFile,
    double valueForRed255,
    double valueForGreen255,
    double valueForBlue255,
    double valueForWhite,
    double valueForBlack,
    double valueForAllOther,
    const string& strOutputTiffFile) {

    ImageSpec imgSpec_input;
    bool bReport = getImageSpec(strInputPngFile, imgSpec_input);
    if (!bReport) {
        cout << __FUNCTION__ << ": error: getImageSpec on file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    if (imgSpec_input.nchannels < 3) { //#LP TBC: png with alpha as 4
        cout << __FUNCTION__
             << ": error: imgSpec_input.channels < 3; (imgSpec_input.nchannels = " << imgSpec_input.nchannels
             << "; file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    ImageBuf IBufSrc(strInputPngFile);

    char cZero { 0 };

    vector<unsigned char> vectImageContent_RGBChannel[3];
    for (auto& iterChannelVect: vectImageContent_RGBChannel) {
        iterChannelVect.resize(imgSpec_input.width * imgSpec_input.height);
    }

    const int redChannel = 0;
    const int greenChannel = 1;
    const int blueChannel = 2;

    for (int iChannel = redChannel; iChannel <= blueChannel; iChannel++) {

        ROI ROIFullImage_oneChannel {
            0, imgSpec_input.width,
            0, imgSpec_input.height,
            0, 1,
            iChannel, iChannel+1
        };

        fill(vectImageContent_RGBChannel[iChannel].begin(),
             vectImageContent_RGBChannel[iChannel].end(),
             cZero);

        bool bGetPixel = IBufSrc.get_pixels(ROIFullImage_oneChannel, TypeDesc(TypeDesc::UINT8),
                                              vectImageContent_RGBChannel[iChannel].data(),
                                              AutoStride, AutoStride, AutoStride);
        if (!bGetPixel) {
            cout << __FUNCTION__ << " ImageBufAlgo::get_pixels(...) failed (iChannel = " << iChannel << " )" << endl;
            return(1);
        }
    }

    //output image

    int outNbChannels = 1;
    OIIO::TypeDesc::BASETYPE typeDescBasetype = OIIO::TypeDesc::BASETYPE::FLOAT;
    ImageSpec ImageSpecOut(imgSpec_input.width,
                           imgSpec_input.height,
                           outNbChannels,
                           TypeDesc(typeDescBasetype));

    cout << "imgSpec_input.width    = " << imgSpec_input.width << endl;
    cout << "imgSpec_input.height   = " << imgSpec_input.height << endl;
    cout << "ImageSpecOut.nchannels = " << ImageSpecOut.nchannels << endl;

    int bitsPerSample = (int)(8*ImageSpecOut.format.size());
    cout << "set iio::BitsPerSample to: " << bitsPerSample << endl;
    ImageSpecOut.attribute("oiio::BitsPerSample", bitsPerSample);

    ImageSpecOut.attribute("compression", "none");

    //ImageSpecOut.attribute("oiio::BitsPerSample", (int)(8*inputImgSpec.format.size())); //(8*sizeof(float)));  //@##LP *channels ?
    //qDebug() << __FUNCTION__ << "oiio::BitsPerSample as :" << (int)(8*inputImgSpec.format.size());

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

    ImageBuf IBufDest(strOutputTiffFile, ImageSpecOut);

    float fZero { .0 };
    ROI dest_ROIFullImage {
        0, ImageSpecOut.width,
        0, ImageSpecOut.height,
        0, 1, //z
        0, outNbChannels };

    bool bFill = ImageBufAlgo::fill(IBufDest, fZero, dest_ROIFullImage);
    if (!bFill) {
        cout << "ImageBufAlgo::fill failed(...)" << endl;
        return(1);
    }

    long int offset = 0l;
    for (int y = 0; y < ImageSpecOut.height; y++) {
        for (int x = 0; x < ImageSpecOut.width; x++) {

            float fPixelValue = .0;

            float fPixelValueRed   = static_cast<float>(vectImageContent_RGBChannel[redChannel  ][offset]);
            float fPixelValueGreen = static_cast<float>(vectImageContent_RGBChannel[greenChannel][offset]);
            float fPixelValueBlue  = static_cast<float>(vectImageContent_RGBChannel[blueChannel ][offset]);

            float fPixelValueRGBSum = fPixelValueRed + fPixelValueGreen + fPixelValueBlue;

            //cout << "f R,G,B: " << fPixelValueRed << ", " << fPixelValueGreen << ", " << fPixelValueBlue << "Sum = " << fPixelValueRGBSum << endl;

            if (fPixelValueRGBSum < 1.0) { //considered as black

                fPixelValue = valueForBlack;

            } else {

                if (fPixelValueRGBSum > (255*3) -3 ) { //considered as white

                    fPixelValue = valueForWhite;

                } else {

                    if (fPixelValueRed > 254) {

                        if (fPixelValueBlue > 254) { //magenta as Nan

                            fPixelValue = std::numeric_limits<double>::quiet_NaN();

                        } else {

                            fPixelValue = valueForRed255;

                        }

                    } else {

                        if (fPixelValueGreen > 254) {

                            fPixelValue = valueForGreen255;

                        } else {

                            if (fPixelValueBlue > 254) {

                                fPixelValue = valueForBlue255;

                            } else {

                                fPixelValue = valueForAllOther;
                            }
                        }
                    }
                }
            }

            //cout << " => setpixel( " << x << ", " << y << ") with: " << fPixelValue << endl;

            IBufDest.setpixel(x, y, 0, &fPixelValue, outNbChannels);

            /*if (!x) {
                cout << "(green) x, y, in, out: ( " << x << ", " << y << " ) in: " <<  static_cast<int>(vectImageContent_RGBChannel[greenChannel][offset]) << " => out: " << fPixelValue << endl;
            }*/

            offset++;
        }
    }

    bool bSuccessWrite = IBufDest.write(strOutputTiffFile, TypeDesc::FLOAT);
    if (!bSuccessWrite) {
        cout << "failed to write " << strOutputTiffFile << endl;
        cout << "geterror():" << geterror() << endl;
        return(1);
    }

    cout << "write(...) done" << endl;

    return(0);
}



int convertImage_RGBThreeChannels_to_floatTiff(
    const string& strInputPngFile,
    double valueForPngMinGreenChannelValue,
    double valueForPngMaxGreenChannelValue,
    const string& strOutputTiffFile) {

    ImageSpec imgSpec_input;
    bool bReport = getImageSpec(strInputPngFile, imgSpec_input);
    if (!bReport) {
        cout << __FUNCTION__ << ": error: getImageSpec on file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    if (imgSpec_input.nchannels < 3) { //#LP TBC: png with alpha as 4
        cout << __FUNCTION__
             << ": error: imgSpec_input.channels < 3; (imgSpec_input.nchannels = " << imgSpec_input.nchannels
             << "; file: " << strInputPngFile.c_str() << endl;
        return(1);
    }

    ImageBuf IBufSrc(strInputPngFile);

    char zero { 0 };

    cout << __FUNCTION__ << __LINE__ << endl;

    vector<unsigned char> vectImageContent_RGBChannel[3];
    for (auto& iterChannelVect: vectImageContent_RGBChannel) {
        iterChannelVect.resize(imgSpec_input.width * imgSpec_input.height);
    }

    const int redChannel = 0;
    const int greenChannel = 1;
    const int blueChannel = 2;

    for (int iChannel = redChannel; iChannel <= blueChannel; iChannel++) {

        ROI ROIFullImage_oneChannel {
            0, imgSpec_input.width,
            0, imgSpec_input.height,
            0, 1,
            iChannel, iChannel+1
        };

        //vector<char> vectImageContent_greenChannel(imgSpec_input.width * imgSpec_input.height);
        fill(vectImageContent_RGBChannel[iChannel].begin(), vectImageContent_RGBChannel[iChannel].end(), zero);

        bool bGetPixel = IBufSrc.get_pixels(ROIFullImage_oneChannel, TypeDesc(TypeDesc::UINT8),
                                              vectImageContent_RGBChannel[iChannel].data(),
                                              AutoStride, AutoStride, AutoStride);
        if (!bGetPixel) {
            cout << __FUNCTION__ << " ImageBufAlgo::get_pixels(...) failed (iChannel = " << iChannel << " )" << endl;
            return(1);
        }
    }

    ImageSpec imgSpec_forStats(imgSpec_input.width, imgSpec_input.height, 1, TypeDesc(TypeDesc::UINT8));
    ImageBuf ImageBuf_forStats(imgSpec_forStats, (void*)vectImageContent_RGBChannel[greenChannel].data());

    ImageBufAlgo::PixelStats pixStats_green;
    bool bStatsComputeSuccess_green = ImageBufAlgo::computePixelStats (pixStats_green, ImageBuf_forStats);
    if (!bStatsComputeSuccess_green) {
        cout << __FUNCTION__ << "(green) error: computePixelStats failed" << endl;
        cout << "TPWOD " << "global geterror() = [ " << geterror().c_str() << " ]" << endl;
        return(1);
    }

    cout << __FUNCTION__ << __LINE__ << endl;

    /*for (int chan = 0; chan < ImageBuf_forStats.nchannels(); ++chan) {
        cout << __FUNCTION__ << "channel #" << chan << endl;
        cout << __FUNCTION__ << "  min = " << pixStats_green.min[chan] << endl;
        cout << __FUNCTION__ << "  max = " << pixStats_green.max[chan] << endl;
        cout << __FUNCTION__ << "nancount = " << pixStats_green.nancount[chan] << endl;
        cout << __FUNCTION__ << "infcount = " << pixStats_green.infcount[chan] << endl;
    }
    */

    float f_green_min = 0;//static_cast<float>(pixStats_green.min[0]);
    float f_green_max = 255;// static_cast<float>(pixStats_green.max[0]);

    float f_src_range = fabs(f_green_min - f_green_max);
    float f_dest_range = fabs(valueForPngMaxGreenChannelValue - valueForPngMinGreenChannelValue);

    /*if (abs(pixStats_green.min[0] - pixStats_green.max[0]) == 0) {
      f_src_range = 1;
      f_dest_range = 1;
      valueForPngMinGreenChannelValue = (valueForPngMaxGreenChannelValue + valueForPngMinGreenChannelValue)/2.0;

      f_green_min = 0;
      cout << __FUNCTION__ << "input image: green channel is a constant color" << endl;
      cout << __FUNCTION__ << "f_src_range => " << f_src_range << endl;
      cout << __FUNCTION__ << "f_dest_range => " << f_dest_range << endl;
      cout << __FUNCTION__ << "valueForPngMinGreenChannelValue => " << valueForPngMinGreenChannelValue << endl;
      cout << __FUNCTION__ << "f_green_min => " << f_green_min << endl;
    }*/

    cout << __FUNCTION__ << "valueForPngMaxGreenChannelValue =" << valueForPngMaxGreenChannelValue << endl;
    cout << __FUNCTION__ << "valueForPngMinGreenChannelValue =" << valueForPngMinGreenChannelValue << endl;

    cout << __FUNCTION__ << "f_green_min = " << f_green_min << endl;
    cout << __FUNCTION__ << "f_green_max = " << f_green_max << endl;

    cout << __FUNCTION__ << "f_src_range = " << f_src_range << endl;
    cout << __FUNCTION__ << "f_dest_range = " << f_dest_range << endl;

    //output image
    ImageSpec ImageSpecOut(imgSpec_input.width, imgSpec_input.height, 1, TypeDesc(TypeDesc::FLOAT));

    int bitsPerSample = (int)(8*ImageSpecOut.format.size());
    cout << "set iio::BitsPerSample to: " << bitsPerSample << endl;
    ImageSpecOut.attribute("oiio::BitsPerSample", bitsPerSample);

    ImageBuf IBufDest(strOutputTiffFile, ImageSpecOut);

    float fZero { 0 };
    ROI dest_ROIFullImage {
        0, imgSpec_input.width,
        0, imgSpec_input.height,
        0, 1,
        0, 1
    };
    bool bFill = ImageBufAlgo::fill(IBufDest, fZero, dest_ROIFullImage);
    if (!bFill) {
        cout << "ImageBufAlgo::fill failed(...)" << endl;
        return(1);
    }

    float fPixelNanValue = NAN;

    long int offset = 0l;
    for (int y = 0; y < imgSpec_input.height; y++) {
        for (int x = 0; x < imgSpec_input.width; x++) {

            if (  (vectImageContent_RGBChannel[redChannel][offset] == vectImageContent_RGBChannel[blueChannel][offset])
                &&(vectImageContent_RGBChannel[redChannel][offset] == 255)
                &&(vectImageContent_RGBChannel[greenChannel][offset] == 0)) {

                IBufDest.setpixel (x, y, 0, &fPixelNanValue, 1);

                if (!x) {
                    cout << "(magenta) x, y, in, out: ( " << x << ", " << y << " ) "
                         << "in: " << static_cast<int>(vectImageContent_RGBChannel[redChannel][offset]) << " => out: " << fPixelNanValue << endl;
                }
            } else {

                float fPixelValue = .0;
                fPixelValue = static_cast<float>(vectImageContent_RGBChannel[greenChannel][offset]);

                if (!x) { cout << __FUNCTION__ << "in, float: fPixelValue = " << fPixelValue << endl; }

                fPixelValue -= f_green_min;

                if (!x) { cout << __FUNCTION__ << "fPixelValue -= f_green_min = " << fPixelValue << endl; }

                fPixelValue /= f_src_range;

                if (!x) { cout << __FUNCTION__ << "fPixelValue /= f_src_range = " << fPixelValue << endl; }

                fPixelValue *= f_dest_range;

                if (!x) { cout << __FUNCTION__ << "fPixelValue *= f_dest_range = " << fPixelValue << endl; }

                fPixelValue += valueForPngMinGreenChannelValue;

                if (!x) { cout << __FUNCTION__ << "fPixelValue += valueForPngMinGreenChannelValue = " << fPixelValue << endl; }

                IBufDest.setpixel (x, y, 0, &fPixelValue, 1);

                if (!x) {
                    cout << "(using green channel) x, y, in, out: ( " << x << ", " << y << " ) in: " 
                    <<  static_cast<int>(vectImageContent_RGBChannel[greenChannel][offset]) << " => out: " << fPixelValue << endl;
                }
            }
            offset++;
        }
    }

    bool bSuccessWrite = IBufDest.write(strOutputTiffFile, TypeUnknown);
    if (!bSuccessWrite) {
        cout << "failed to write " << strOutputTiffFile << endl;
        cout << "geterror():" << geterror() << endl;
        return(1);
    }

    cout << "write(...) done" << endl;

    return(0);
}


bool getImageSpec(const string& strInputFilename, ImageSpec& imgSpec) {
    auto in = ImageInput::open(strInputFilename);
    if (!in) {
        cout << __FUNCTION__ << ": error: can not open for read; file " << strInputFilename.c_str() << endl;
        return(false);
    }
    imgSpec = in->spec();
    return(true);
}


template<typename T>
bool writeImage(const string& strPathAndFilename, vector<T> vectData, int width, int height, TypeDesc::BASETYPE typeDescBaseType) {

    if ((width * height) != vectData.size()) {
        cout << "if ((width * height) != vectData.size()) {" << endl;
        return(false);
    }

    ImageSpec ImageSpecOut(width, height, 1, TypeDesc(typeDescBaseType));

    int bitsPerSample = (int)(8*ImageSpecOut.format.size());
    cout << "set iio::BitsPerSample to: " << bitsPerSample << endl;
    ImageSpecOut.attribute("oiio::BitsPerSample", bitsPerSample);

    ImageBuf IBufDest(strPathAndFilename, ImageSpecOut);

    T zero { static_cast<T>(.0) };
    ROI ROIFullImage {
        0, width,
        0, height,
        0, 1,
        0, 1
    };
    bool bFill = ImageBufAlgo::fill(IBufDest, zero, ROIFullImage);
    if (!bFill) {
        cout << "ImageBufAlgo::fill failed(...)" << endl;
        return(false);
    }

    /*cout << "will write this content:" << endl;
    long int offset = 0l;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            switch (typeDescBaseType) {
                //case TypeDesc::FLOAT: cout << static_cast<float>(vectImageContent[offset]) << ",";break;
                //case TypeDesc::INT16: cout << static_cast<signed short int>(vectImageContent[offset]) << ";";break;
                //case TypeDesc::UINT8: cout << static_cast<unsigned int>(vectImageContent[offset]) << ",";break;
                case TypeDesc::FLOAT: fprintf(stdout,"%4f,", static_cast<float>(vectData[offset]));break;
                case TypeDesc::INT16: fprintf(stdout,"%4d,", static_cast<signed short int>(vectData[offset]));break;
                case TypeDesc::UINT8: fprintf(stdout,"%4d,", static_cast<unsigned int>(vectData[offset]));break;
                default: fprintf(stdout, "invalid typeDescBaseType,"); break;
            }
            offset++;
        }
        cout << endl;
    }
    cout << endl;*/

    bool bSetPixel = IBufDest.set_pixels(ROIFullImage, IBufDest.pixeltype(), vectData.data(), AutoStride, AutoStride, AutoStride);
    if (!bSetPixel) {
        cout << "ImageBufAlgo::set_pixels(...) failed" << endl;
        return(false);
    }

    bool bSuccessWrite = IBufDest.write(strPathAndFilename, TypeUnknown);
    if (!bSuccessWrite) {
        cout << "failed to write " << strPathAndFilename << endl;
        cout << "geterror():" << geterror() << endl;
        return(false);
    } 

    cout << "write(...) done" << endl;

    return(true);
}
