#include <math.h>

#include <cstdlib> //from srand
#include <ctime> //for srand init

#include <vector>
#include <string>
using namespace std;

#include <OpenImageIO/typedesc.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
using namespace OIIO;

template<typename T>
bool writeImage(const string& strPathAndFilename, vector<T> vectData, unsigned int width, unsigned int height, TypeDesc::BASETYPE typeDescBaseType) {

    cout << __FUNCTION__ << " entering" << endl;


    if ((width * height) != vectData.size()) {
        cout << __FUNCTION__ << "if ((width * height) != vectData.size()) {" << endl;
        return(false);
    }

    cout << __FUNCTION__ << "typeDescBaseType = " << typeDescBaseType << endl;

    ImageSpec ImageSpecOut(width, height, 1, TypeDesc(typeDescBaseType));

    int bitsPerSample = (int)(8*ImageSpecOut.format.size());
    cout << __FUNCTION__ << "set iio::BitsPerSample to: " << bitsPerSample << endl;

    ImageSpecOut.attribute("oiio::BitsPerSample", bitsPerSample);
    ImageSpecOut.attribute("compression", "none");
    ImageSpecOut.attribute("tiff:write exif", int(1));
    ImageSpecOut.attribute("tiff:Compression", (int)1);

    ImageBuf IBufDest(strPathAndFilename, ImageSpecOut);

    T zero { static_cast<T>(.0) };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    bool bFill = ImageBufAlgo::fill(IBufDest, zero, ROIFullImage);
    if (!bFill) {
        cout << __FUNCTION__ << "ImageBufAlgo::fill failed(...)" << endl;
        return(false);
    }

    cout << __FUNCTION__ << "fill done " << endl;

    cout << "will write this content:" << endl;
    long int offset = 0l;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            switch (typeDescBaseType) {              
                case TypeDesc::FLOAT: cout << static_cast<float>             (vectData[offset]) << " ";break;
                case TypeDesc::INT16: cout << static_cast<signed short int>  (vectData[offset]) << " ";break;
                case TypeDesc::UINT8: cout << static_cast<unsigned short int>(vectData[offset]) << " ";break;
                default: cout << "invalid typeDescBaseType" << endl; break;
            }
            offset++;
        }
        cout << endl;
    }
    cout << __FUNCTION__ << endl;

    cout << __FUNCTION__ << "IBufDest.pixeltype() = " << IBufDest.pixeltype() << endl;

    bool bSetPixel = IBufDest.set_pixels(ROIFullImage, IBufDest.pixeltype(), vectData.data(), AutoStride, AutoStride, AutoStride);
    if (!bSetPixel) {
        cout << __FUNCTION__ << "ImageBufAlgo::set_pixels(...) failed" << endl;
        return(false);
    }

    bool bSuccessWrite = IBufDest.write(strPathAndFilename, TypeUnknown);
    if (!bSuccessWrite) {
        cout << __FUNCTION__ << "failed to write " << strPathAndFilename << endl;
        cout << __FUNCTION__ << "geterror():" << geterror() << endl;
        return(false);
    }

    cout << __FUNCTION__ << "write(...) done" << endl;
    cout << __FUNCTION__ << "_______________" << endl;

    return(true);
}

bool stdout_displayImageContent_float__first_in_last_values(const string& strPathAndFilename, unsigned int width, unsigned int height/*, TypeDesc::BASETYPE typeDescBaseType*/) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    cout << "IBufSrc.image_pixels() = " << IBufSrc.spec().image_pixels() << endl;
    cout << "IBufSrc.spec().width   = " << IBufSrc.spec().width  << endl;
    cout << "IBufSrc.spec().height  = " << IBufSrc.spec().height << endl;

    float zero { .0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    vector<float> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), zero);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::FLOAT), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;

    long int offset = 0l;
    cout << "first: " << static_cast<float>(vectImageContent[offset]) << endl;

    offset = (width*height)/2;
    cout << "in:: " << static_cast<float>(vectImageContent[offset]) << endl;

    offset = (width*height)-1;
    cout << "last: " << static_cast<float>(vectImageContent[offset]) << endl;

    cout << endl;
    return(true);
}

bool stdout_displayImageContent_s16__first_in_last_values(const string& strPathAndFilename, unsigned int width, unsigned int height/*, TypeDesc::BASETYPE typeDescBaseType*/) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    cout << "IBufSrc.image_pixels() = " << IBufSrc.spec().image_pixels() << endl;
    cout << "IBufSrc.spec().width   = " << IBufSrc.spec().width  << endl;
    cout << "IBufSrc.spec().height  = " << IBufSrc.spec().height << endl;


    signed short int zero { 0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    vector<signed short int> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), zero);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage, TypeDesc(TypeDesc::INT16), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;

    long int offset = 0l;
   cout << "first: " << static_cast<signed short int>(vectImageContent[offset]) << endl;

    offset = (width*height)/2;
    cout << "in: " << static_cast<signed short int>(vectImageContent[offset]) << endl;

    offset = (width*height)-1;
    cout << "last: " << static_cast<signed short int>(vectImageContent[offset]) << endl;

    cout << endl;
    return(true);
}


bool stdout_displayImageContent_uint8__first_in_last_values(const string& strPathAndFilename, unsigned int width, unsigned int height/*, TypeDesc::BASETYPE typeDescBaseType*/) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    cout << "IBufSrc.image_pixels() = " << IBufSrc.spec().image_pixels() << endl;
    cout << "IBufSrc.spec().width   = " << IBufSrc.spec().width  << endl;
    cout << "IBufSrc.spec().height  = " << IBufSrc.spec().height << endl;


    //signed short int zero { 0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    vector<unsigned char> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), .0);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::UINT8), /*IBufSrc.pixeltype(),*/ vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;

    unsigned long int offset = 0l;
    cout << "first: " << vectImageContent[offset] << endl;

    offset = (width*height)/2;
    cout << "in: " << vectImageContent[offset] << endl;

    offset = (width*height)-1;
    cout << "last: " << vectImageContent[offset] << endl;

    cout << endl;
    return(true);
}



bool stdout_displayImageContent_float(const string& strPathAndFilename, unsigned int width, unsigned int height/*, TypeDesc::BASETYPE typeDescBaseType*/) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    float zero { .0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    vector<float> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), zero);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::FLOAT), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;
    unsigned long int offset = 0l;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            cout << static_cast<float>(vectImageContent[offset]) << " "; 
            offset++;
        }
        cout << endl;
    }
    cout << endl;
    return(true);
}

bool stdout_displayImageContent_signed16(const string& strPathAndFilename, unsigned int width, unsigned int height/*, TypeDesc::BASETYPE typeDescBaseType*/) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    //signed short int zero { 0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };

    vector<float> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), 0);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::FLOAT), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;
    unsigned long int offset = 0l;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
          cout << static_cast<signed short int>(vectImageContent[offset]*SHRT_MAX) << " ";
          offset++;
        }
        cout << endl;
    }
    cout << endl;
    return(true);
}


bool stdout_displayImageContent_uint8(const string& strPathAndFilename, unsigned int width, unsigned int height) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    //signed short int zero { 0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };

    vector<unsigned char> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), .0);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::UINT8), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;
    unsigned long int offset = 0l;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            cout << static_cast<unsigned short int>(vectImageContent[offset]) << " ";
            offset++;
        }
        cout << endl;
    }
    cout << endl;
    return(true);
}



bool stdout_displayImageContent_signed16_tryWay2(const string& strPathAndFilename, unsigned int width, unsigned int height) {

    if ((width<=0)||(height<=0)) {
        cout << "if ((width<=0)||(height<=0)) {" << endl;
        return(false);
    }

    ImageBuf IBufSrc(strPathAndFilename);

    //signed short int zero { 0 };
    ROI ROIFullImage {
        0, (int)width,
        0, (int)height,
        0, 1,
        0, 1
    };
    vector<signed short int> vectImageContent(width * height);
    fill(vectImageContent.begin(), vectImageContent.end(), 0);

    bool bGetPixel = IBufSrc.get_pixels(ROIFullImage,  TypeDesc(TypeDesc::INT16), vectImageContent.data(), AutoStride, AutoStride, AutoStride);
    if (!bGetPixel) {
        cout << "ImageBufAlgo::get_pixels(...) failed" << endl;
        return(false);
    }

    cout << "content of " <<  strPathAndFilename << ":" << endl;
    unsigned long int offset = 0l;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {            
            cout << static_cast<signed short int>(vectImageContent[offset]) << " ";
            offset++;
        }
        cout << endl;
    }
    cout << endl;
    return(true);
}

float frandomValueMinus5toPlus5_oneDec() {
    int i_randomValue0to100 = rand()%100;
    float f_randomValueMinus50to50 = static_cast<float>(i_randomValue0to100) - 50.0;
    return(f_randomValueMinus50to50/10.0);
}

signed short int s16randomValueMinus5toPlus5_oneDec() {
    int i_randomValue0to100 = rand()%100;
    signed short int s16_randomValueMinus50to50 = static_cast<signed short int>(i_randomValue0to100) - 50.0;
    return(s16_randomValueMinus50to50/10);
}


int main_smallImages(int argc, char*argv[]) {

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    int w = 15;
    int h = 15;
    int nbchannel = 1;

    //  width * height * nchannel = 15*15*1
    //  type: float:
    vector<float> vect_f_1 {
        .0, 0., 0.01, 0., -10.11 , -5.10 , -3.09,  0.99,  +2.0 ,  +4.0 ,  +10., .0, 0., 0., .0,
        .0, 0., 0.02, 0., -10.22 , -5.20 , -3.08,  0.89,  +2.1 ,  +4.1 ,  +11., .0, 0., 0., .0,
        .0, 0., 0.03, 0., -10.33 , -5.30 , -3.07,  0.79,  +2.2 ,  +4.2 ,  +12., .0, 0., 0., .0,
        .0, 0., 0.04, 0., -10.44 , -5.40 , -3.06,  0.69,  +2.3 ,  +4.3 ,  +13., .0, 0., 0., .0,
        .0, 0., 0.05, 0., -10.55 , -5.50 , -3.05,  0.59,  +2.4 ,  +4.4 ,  +14., .0, 0., 0., .0,
        .0, 0., 0.06, 0., -10.66 , -5.60 , -3.04,  0.49,  +2.5 ,  +4.5 ,  +15., .0, 0., 0., .0,
        .0, 0., 0.07, 0., -10.77 , -5.70 , -3.03,  0.39,  +2.6 ,  +4.6 ,  +16., .0, 0., 0., .0,
        .0, 0., 0.08, 0., -10.88 , -5.80 , -3.02,  0.29,  +2.7 ,  +4.7 ,  +17., .0, 0., 0., .0,
        .0, 0., 0.09, 0., -10.99 , -5.90 , -3.01,  0.19,  +2.8 ,  +4.8 ,  +18., .0, 0., 0., .0,
        .0, 0., 0.10, 0., -10.001, -5.00 , -3.00,  0.09,  +2.9 ,  +4.9 ,  +19., .0, 0., 0., .0,
        .0, 0., 0.20, 0., -10.002, -5.012, -3.10,  0.00,  +2.10,  +4.10,  +20., .0, 0., 0., .0,
        .0, 0., 0.30, 0., -10.003, -5.023, -3.11,  0.01,  +2.11,  +4.11,  +21., .0, 0., 0., .0,
        .0, 0., 0.40, 0., -10.004, -5.034, -3.12,  0.02,  +2.12,  +4.12,  +22., .0, 0., 0., .0,
        .0, 0., 0.50, 0., -10.005, -5.045, -3.13,  0.03,  +2.13,  +4.13,  +23., .0, 0., 0., .0,
        .0, 0., 0.60, 0., -10.006, -5.056, -3.14,  0.04,  +2.14,  +4.14,  +24., .0, 0., 0., .0
    };
    //add now nan in row 1,3,11,13
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            if (  (x ==  1)
                ||(x ==  3)
                ||(x == 11)
                ||(x == 13)) {
              vect_f_1[(y*w)+x] = NAN;
            } else {
               //vect_f_1[(y*w)+x] = fabs(vect_f_1[(y*w)+x]);
            }
        }
    }
    
    //add now nan in columns 5,9,
    /*for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            if (  (y == 5)
                ||(y == 9)) {
              vect_f_1[(y*w)+x] = NAN;
            }
        }
    }*/
    
    if (w == h) {     
       
      //add now a 'thick' '/' diagonal of nan
      /*for (unsigned int y = 0; y < h; y++) {
          for (unsigned int x = 0; x < w; x++) {
              if (  (x == y)
                  ||(y == x-1)
                  ||(y == x+1)) {
                vect_f_1[(y*w)+x] = NAN;
              }
          }
      }*/
    
      //add now a 'thick' '\' diagonal of nan
      /*for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
              if (  (x+y == h-1)
              
                  ||(x+y == h )
                  ||(x+y == h+1)) {
                    
                vect_f_1[(y*w)+x] = NAN;
              }
          }
      }*/
    }


    //float    
    bool bImageWrote = true;
    unsigned long int loc = 0l;

    bImageWrote &= writeImage("./output/float_px1_wr.tiff", vect_f_1  , w, h, TypeDesc::FLOAT);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px1_wr" << endl;
        return(1);
    }


    loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            float frand = frandomValueMinus5toPlus5_oneDec();
            //cout << "frand = " << frand << endl;
            vect_f_1[loc] = vect_f_1[loc] + frand; //+12.0
            loc++;
        }
    }
    bImageWrote &= writeImage("./output/float_px2_wr.tiff", vect_f_1  , w, h, TypeDesc::FLOAT);

    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px2_wr" << endl;
        return(1);
    }

    //display content for checking
    stdout_displayImageContent_float("./output/float_px1_wr.tiff", w, h);
    stdout_displayImageContent_float("./output/float_px2_wr.tiff", w, h);


    cout << "signed 16:" << endl;

    //signed 16
    //type: signed 16 bit:
    vector<signed short int> vect_s16_1 {
        0, 0, 0, 0, -10, -45, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -11, -44, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -12, -43, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -13, -42, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -14, -41, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -15, -40, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -16, -39, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -17, -38, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -18, -37, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -19, -36, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -20, -35, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -21, -34, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -22, -33, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -23, -32, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0,
        0, 0, 0, 0, -24, -31, -3,  0,  +2,  +4,  +10, 0, 0, 0, 0
    };

    /*loc = 0l;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if ((y*h+x)%2) {
                vect_s16_1[loc] = -32768;
            } else {
                vect_s16_1[loc] = +32767;
            }
            loc++;
        }
    }*/


    bImageWrote &= writeImage("./output/s16_px1_wr.tiff"  , vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_px1_wr" << endl;
        return(1);
    }

    loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            signed int sirand = static_cast<signed int>(frandomValueMinus5toPlus5_oneDec());
            //cout << "sirand = " << sirand << endl;
            vect_s16_1[loc] = vect_s16_1[loc] + sirand; //+12
            loc++;
        }
    }
    bImageWrote &= writeImage("./output/s16_px2_wr.tiff",  vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_px2_wr" << endl;
        return(1);
    }

    //display content for checking
    stdout_displayImageContent_signed16("./output/s16_px1_wr.tiff", w, h);
    stdout_displayImageContent_signed16("./output/s16_px2_wr.tiff", w, h);

    /*
    cout << "---- stdout_displayImageContent_signed16_tryWay2:" << endl;
    stdout_displayImageContent_signed16_tryWay2("./output/s16_px1.tiff", w, h);
    stdout_displayImageContent_signed16_tryWay2("./output/s16_px2.tiff", w, h);
    cout << "stdout_displayImageContent_signed16_tryWay2 ----" << endl;
    */


    //type: unsigned 8 bit:
    vector<unsigned char> vect_byte_1 {
        0,  1, 0, 0, 120, 135, 3,  0,  200,  4,  255, 0, 0, 0, 0,
        0,  2, 0, 0, 121, 136, 3,  0,  202,  4,  254, 0, 0, 0, 0,
        0,  3, 0, 0, 122, 137, 3,  0,  204,  4,  253, 0, 0, 0, 0,
        0,  4, 0, 0, 123, 138, 3,  0,  206,  4,  252, 0, 0, 0, 0,
        0,  5, 0, 0, 124, 139, 3,  0,  208,  4,  251, 0, 0, 0, 0,
        0,  6, 0, 0, 125, 140, 3,  0,  210,  4,  180, 0, 0, 0, 0,
        0,  7, 0, 0, 126, 141, 3,  0,  211,  4,  181, 0, 0, 0, 0,
        0,  9, 0, 0, 127, 142, 3,  0,  213,  4,  182, 0, 0, 0, 0,
        0,  9, 0, 0, 128, 143, 3,  0,  215,  4,  183, 0, 0, 0, 0,
        0, 10, 0, 0, 129, 144, 3,  0,  217,  4,  184, 0, 0, 0, 0,
        0, 11, 0, 0, 130, 145, 3,  0,  219,  4,  185, 0, 0, 0, 0,
        0, 12, 0, 0, 131, 146, 3,  0,  220,  4,  186, 0, 0, 0, 0,
        0, 13, 0, 0, 132, 147, 3,  0,  230,  4,  187, 0, 0, 0, 0,
        0, 14, 0, 0, 133, 148, 3,  0,  240,  4,  188, 0, 0, 0, 0,
        0, 15, 0, 0, 134, 149, 3,  0,  250,  4,  189, 0, 0, 0, 0
    };
    bImageWrote &= writeImage       ("./output/byte_correlScore_wr.tiff", vect_byte_1, w, h, TypeDesc::UINT8);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) byte_correlScore_wr" << endl;
        return(1);
    }

    stdout_displayImageContent_uint8("./output/byte_correlScore_wr.tiff", w, h);


    return(0);

}

int main_bigImages(int argc, char*argv[]) {

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    unsigned int w = 1250;//1500;
    unsigned int h = 1250;//1500;
    unsigned int nbchannel = 1;

    unsigned long int offset = 0l;

    //  width * height * nchannel = 15*15*1
    //  type: float:
    vector<float> vect_f_1(w*h);

    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            /*if (!(x%17)) {
              vect_f_1[offset] = NAN;
            } else {*/
              vect_f_1[offset]=static_cast<float>(x*y)/static_cast<float>((w-1)*(h-1));
            //}
            offset++;
        }
    }

    bool bImageWrote = true;
    bImageWrote &= writeImage("./output/float_px1_wr_1500.tiff", vect_f_1, w, h, TypeDesc::FLOAT);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px1_wr_1500" << endl;
        return(1);
    }

    unsigned long int loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            float frand = frandomValueMinus5toPlus5_oneDec();
            //cout << "frand = " << frand << endl;
            vect_f_1[loc] = vect_f_1[loc] + frand; //+12.0
            loc++;
        }
    }
    bImageWrote &= writeImage("./output/float_px2_wr_1500.tiff", vect_f_1, w, h, TypeDesc::FLOAT);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px2_wr_1500" << endl;
        return(1);
    }



    //  type: signed 16 bit:
    vector<signed short int> vect_s16_1(w*h);
    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            /*if (!(x%17)) {
              vect_f_1[offset] = NAN;
            } else {*/
              vect_s16_1[offset]=-2 + static_cast<float>(x*y)/static_cast<float>((w-1)*(h-1));
            //}
            offset++;
        }
    }

    bImageWrote = true;
    bImageWrote &= writeImage("./output/s16_1_px1_wr_1500.tiff", vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_1_px1_wr_1500" << endl;
        return(1);
    }

    loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            float frand = frandomValueMinus5toPlus5_oneDec();
            //cout << "frand = " << frand << endl;
            vect_s16_1[loc] = vect_s16_1[loc] + frand; //+12.0
            loc++;
        }
    }
    bImageWrote &= writeImage("./output/s16_px2_wr_1500.tiff", vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_px2_wr_1500" << endl;
        return(1);
    }







    //type: unsigned 8 bit:
    vector<unsigned char> vect_byte_1(w*h);
    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            /*if (!(x%17)) {
              vect_byte_1[offset] = NAN;
            } else {*/
                vect_byte_1[offset]=static_cast<int>( (static_cast<float>(x*y)) / (static_cast<float>((w-1)*(h-1))/255.0) );
            //}        cout << " if (!bImageWrote) byte_correlScore_wr_1500" << endl;

            offset++;
        }
    }
    bImageWrote &= writeImage("./output/byte_correlScore_wr_1500.tiff", vect_byte_1, w, h, TypeDesc::UINT8);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) byte_correlScore_wr_1500" << endl;
        return(1);
    }

    //display content for checking
    stdout_displayImageContent_float__first_in_last_values("./output/float_px1_wr_1500.tiff", w, h);
    stdout_displayImageContent_float__first_in_last_values("./output/float_px2_wr_1500.tiff", w, h);

    stdout_displayImageContent_s16__first_in_last_values("./output/s16_px1_wr_1500.tiff", w, h);
    stdout_displayImageContent_s16__first_in_last_values("./output/s16_px2_wr_1500.tiff", w, h);

    stdout_displayImageContent_uint8__first_in_last_values("./output/byte_correlScore_wr_1500.tiff", w, h);

    return(0);

}


int main_hugeImages(int argc, char*argv[]) {

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    unsigned int w = 5000;
    unsigned int h = 5000;
    unsigned int nbchannel = 1;

    unsigned long int offset = 0l;

    cout << "w*h*nbchannel= " << w*h*nbchannel << endl;

    //  width * height * nchannel = 50000*50000*1

    //----------------------------------------
    //  type: float:
    vector<float> vect_f_1(w*h*nbchannel);

    cout << "vector<float> ( " << w*h*nbchannel << ") allocated" << endl;

    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            /*if (!(x%17)) {
              vect_f_1[offset] = NAN;
            } else {*/
            float x1 = x;// / 1000.0;
            float y1 = y;// / 1000.0;
            vect_f_1[offset]=static_cast<float>(x1*y1)/static_cast<float>((w-1)*(h-1));
            //}
            offset++;
            //cout << offset << "\r";
        }
    }
    cout << "float " << w*h*nbchannel << " feed " << endl;

    cout << "float  first: " << vect_f_1[0] << endl;
    cout << "float  in   : " << vect_f_1[(w*h)/2] << endl;
    cout << "float  last : " << vect_f_1[(w*h)-1] << endl;


    //float
    bool bImageWrote = true;
    bImageWrote &= writeImage("./output/float_px1_wr_50000.tiff", vect_f_1, w, h, TypeDesc::FLOAT);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px1_wr_50000" << endl;
        return(1);
    }
    unsigned long int loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            float frand = frandomValueMinus5toPlus5_oneDec();
            //cout << "frand = " << frand << endl;
            vect_f_1[loc] = vect_f_1[loc] + frand; //+12.0
            loc++;
        }
    }

    cout << "float  first: " << vect_f_1[0] << endl;
    cout << "float  in   : " << vect_f_1[(w*h)/2] << endl;
    cout << "float  last : " << vect_f_1[(w*h)-1] << endl;


    bImageWrote &= writeImage("./output/float_px2_wr_50000.tiff", vect_f_1, w, h, TypeDesc::FLOAT);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) float_px2_wr_50000" << endl;
        return(1);
    }



    //----------------------------------------
    //  type: signed 16 bit:
    vector<signed short int> vect_s16_1(w*h*nbchannel);
    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            //if (!(x%17)) {
            //  vect_f_1[offset] = NAN;
            //} else {
              //  signed short int x1 = x/2;// / 1000;
              //  signed short int y1 = y/2;// / 1000;
              //vect_s16_1[offset]= -1 + static_cast<signed short int>(x1*y1)/static_cast<signed short int>((w-1)/2*(h-1)/2);
              if (offset%2) {
                vect_s16_1[offset] = +10;
              } else {
                vect_s16_1[offset] = -71;
              }
            //}
            offset++;
        }
    }

    cout << "signed 16  first: " << vect_s16_1[0] << endl;
    cout << "signed 16  in   : " << vect_s16_1[(w*h)/2] << endl;
    cout << "signed 16  last : " << vect_s16_1[(w*h)-1] << endl;


    //signed short int
    bImageWrote = true;
    bImageWrote &= writeImage("./output/s16_px1_wr_50000.tiff", vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_px1_wr_50000" << endl;
        return(1);
    }

    loc = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            signed short int s16rand = s16randomValueMinus5toPlus5_oneDec();
            //cout << "frand = " << frand << endl;
            vect_s16_1[loc] = vect_s16_1[loc] + 2*s16rand; //+12.0
            loc++;
        }
    }

    cout << "signed 16  first: " << vect_s16_1[0] << endl;
    cout << "signed 16  in   : " << vect_s16_1[(w*h)/2] << endl;
    cout << "signed 16  last : " << vect_s16_1[(w*h)-1] << endl;

    bImageWrote &= writeImage("./output/s16_px2_wr_50000.tiff", vect_s16_1, w, h, TypeDesc::INT16);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) s16_px2_wr_50000" << endl;
        return(1);
    }


    /*
    //type: unsigned 8 bit:
    vector<unsigned char> vect_byte_1(w*h*nbchannel);
    offset = 0l;
    for (unsigned int y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; x++) {
            //if (!(x%17)) {
            //  vect_byte_1[offset] = NAN;
            //} else {
                vect_byte_1[offset]=static_cast<int>( (static_cast<float>(x*y)) / (static_cast<float>((w-1)*(h-1))/255.0) );
            //}
            offset++;
        }
    }
    bImageWrote &= writeImage("./output/byte_correlScore_wr_50000.tiff", vect_byte_1, w, h, TypeDesc::UINT8);
    if (!bImageWrote) {
        cout << " if (!bImageWrote) byte_correlScore_wr_50000" << endl;
        return(1);
    }
    */


    //display content for checking
    stdout_displayImageContent_float__first_in_last_values("./output/float_px1_wr_50000.tiff", w, h);
    stdout_displayImageContent_float__first_in_last_values("./output/float_px2_wr_50000.tiff", w, h);


    stdout_displayImageContent_s16__first_in_last_values("./output/s16_px1_wr_50000.tiff", w, h);
    stdout_displayImageContent_s16__first_in_last_values("./output/s16_px2_wr_50000.tiff", w, h);

    stdout_displayImageContent_uint8__first_in_last_values("./output/byte_correlScore_wr_50000.tiff", w, h);

    return(0);

}



int main(int argc, char*argv[]) {
  
  main_smallImages(argc, argv);

  //main_bigImages(argc, argv);
  //main_hugeImages(argc, argv);

  return(0);
}
