#include <QDebug>

#include <OpenImageIO/imageio.h>

using namespace OIIO;

#include <math.h> //#LP for NAN

#include <vector>
using namespace std;

#include "../../application/logic/io/PixelPicker.h"

#include "../../application/logic/toolbox/toolbox_math.h"

bool test_PixelPicker_uint8();
bool test_PixelPicker_float();
bool test_PixelPicker_signedInt16();
bool test_PixelPicker_unsignedInt16t();

//store the pixel value according to its (px, py)
//this is used to check that what we get from the pixelPicker tests is what is expected
union U_PixelValue{
    unsigned char _uint8PixelValue;
    signed short int _int16PixelValue;
    unsigned short int _uint16PixelValue;
    float _floatPixelValue;
};

struct S_PixelPicker_continousPixelXY_pixelValue {
    float _px;
    float _py;
    U_PixelValue _pixelValue;

    /*explicit*/ S_PixelPicker_continousPixelXY_pixelValue(float px, float py, unsigned char uint8PixelValue);
    /*explicit*/ S_PixelPicker_continousPixelXY_pixelValue(float px, float py, signed short int int16PixelValue);
    /*explicit*/ S_PixelPicker_continousPixelXY_pixelValue(float px, float py, unsigned short int uint16PixelValue);
    /*explicit*/ S_PixelPicker_continousPixelXY_pixelValue(float px, float py, float floatPixelValue);

    S_PixelPicker_continousPixelXY_pixelValue(const S_PixelPicker_continousPixelXY_pixelValue& copyMe) = default;
    S_PixelPicker_continousPixelXY_pixelValue& operator=(const S_PixelPicker_continousPixelXY_pixelValue& copyMe) = default;

};

S_PixelPicker_continousPixelXY_pixelValue::S_PixelPicker_continousPixelXY_pixelValue(float px, float py, unsigned char uint8PixelValue): _px(px), _py(py) {
    _pixelValue._uint8PixelValue = uint8PixelValue;
}

S_PixelPicker_continousPixelXY_pixelValue::S_PixelPicker_continousPixelXY_pixelValue(float px, float py, signed short int int16PixelValue): _px(px), _py(py) {
    _pixelValue._int16PixelValue = int16PixelValue;
}

S_PixelPicker_continousPixelXY_pixelValue::S_PixelPicker_continousPixelXY_pixelValue(float px, float py, unsigned short int uint16PixelValue): _px(px), _py(py) {
    _pixelValue._uint16PixelValue = uint16PixelValue;
}

S_PixelPicker_continousPixelXY_pixelValue::S_PixelPicker_continousPixelXY_pixelValue(float px, float py, float floatPixelValue): _px(px), _py(py) {
    _pixelValue._floatPixelValue = floatPixelValue;
}


bool test_PixelPicker_nearest(const std::string& strFilename,
                              unsigned char typeDescBaseTypeWhichNeedsToMatch,
                              const vector<S_PixelPicker_continousPixelXY_pixelValue>& vect_sPixPick_cPXY_pixV_nearest);

bool test_PixelPicker_bilinearSquare2x2(const std::string& strFilename,
                              unsigned char typeDescBaseTypeWhichNeedsToMatch,
                              const vector<S_PixelPicker_continousPixelXY_pixelValue>& vect_sPixPick_cPXY_pixV_bilinearSquare2x2);

int main(int argc, char *argv[]) {

    bool bSuccess = true;
    bSuccess &= test_PixelPicker_uint8();
    bSuccess &= test_PixelPicker_float();
    bSuccess &= test_PixelPicker_signedInt16();
    bSuccess &= test_PixelPicker_unsignedInt16t();

    qDebug() << __FUNCTION__  << "bSuccess = " << bSuccess;

    return(0);
}


bool test_PixelPicker_uint8() {

   qDebug() << __FUNCTION__ ;

   const std::string strFilename("/home/laurent/inputDataForTests/inputImages/unitTest_pixelPicker/signed16_float_byte_15x15/byte_correlScore_wr.tiff");

   /*
   127, 142
   128, 143
   */

   unsigned char char_pixV[2][2]={
       {127,142},
       {128,143}};

   qDebug() << __FUNCTION__ << "(4,7) = " << static_cast<int>(char_pixV[0][0]) << "," << static_cast<int>(char_pixV[0][1]);
   qDebug() << __FUNCTION__ << "(5,8) = " << static_cast<int>(char_pixV[1][0]) << "," << static_cast<int>(char_pixV[1][1]);

   vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_nearest {
       {4.500, 7.500, static_cast<unsigned char>(char_pixV[0][0])},

       {5.500, 8.500, static_cast<unsigned char>(char_pixV[1][1])},

       {4.750, 7.500, static_cast<unsigned char>(char_pixV[0][0])},

       {4.500, 7.750, static_cast<unsigned char>(char_pixV[0][0])},

       {4.750, 7.750, static_cast<unsigned char>(char_pixV[0][0])}
   };

   vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2 {
       {4.500, 7.500, static_cast<unsigned char>(char_pixV[0][0])},
       {5.500, 8.500, static_cast<unsigned char>(char_pixV[1][1])},

       {4.750, 7.500, static_cast<unsigned char>((static_cast<float>(char_pixV[0][0])*0.75f + static_cast<float>(char_pixV[0][1])*0.25f))},

       {4.500, 7.750, static_cast<unsigned char>((static_cast<float>(char_pixV[0][0])*0.75f + static_cast<float>(char_pixV[1][0])*0.25f))},

       {4.750, 7.750, static_cast<unsigned char>(
                   ((static_cast<float>(char_pixV[0][0])*0.75f + static_cast<float>(char_pixV[0][1])*0.25f)*0.75f)
                 + ((static_cast<float>(char_pixV[1][0])*0.75f + static_cast<float>(char_pixV[1][1])*0.25f)*0.25f)) },
    };

    bool bErrorReport = true;
    bErrorReport &= test_PixelPicker_nearest(strFilename, TypeDesc::UINT8, vect_sPixPick_cPXY_pixV_nearest);
    bErrorReport &= test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::UINT8, vect_sPixPick_cPXY_pixV_bilinearSquare2x2);

    return(bErrorReport);
}


bool test_PixelPicker_float() {
  
  qDebug() << __FUNCTION__ ;

  const std::string strFilename("/home/laurent/inputDataForTests/inputImages/unitTest_pixelPicker/signed16_float_byte_15x15/float_px1_wr.tiff");

  bool bErrorReport = true;


  {
    //with Nan test
    //
    //   (x,y) 3, 4:  Nan         (x,y) 4, 4:  -10.55
    //   (x,y) 3, 5:  Nan         (x,y) 4, 5:  -10.66
    //
    float float_pixV_withNan[2][2]={
        {NAN, -10.55f},
        {NAN, -10.66f}
    };

    vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_nearest {
       {3.500, 4.500, static_cast<float>(float_pixV_withNan[0][0])},

       {4.500, 5.500, static_cast<float>(float_pixV_withNan[1][1])},

       {3.750, 4.500, static_cast<float>(float_pixV_withNan[0][0])},

       {3.500, 4.750, static_cast<float>(float_pixV_withNan[0][0])},

       {3.750, 4.750, static_cast<float>(float_pixV_withNan[0][0])}
     };

     vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2_withNan {
        {3.500, 4.500, static_cast<float>(float_pixV_withNan[0][0])},

         {4.500, 5.500, static_cast<float>(float_pixV_withNan[1][1])},

        {3.750, 4.500, static_cast<float>(NAN)},

        {3.500, 4.750, static_cast<float>(NAN)},

        {3.750, 4.750, static_cast<float>(NAN)}
      };

    bErrorReport &=test_PixelPicker_nearest(strFilename, TypeDesc::FLOAT, vect_sPixPick_cPXY_pixV_nearest);
    bErrorReport &=test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::FLOAT, vect_sPixPick_cPXY_pixV_bilinearSquare2x2_withNan);
  }

  {
      //without Nan test
      //
      // (x,y) 8, 7 : 2.7    (x,y) 9, 7 : 4.7
      // (x,y) 8, 8 : 2.8    (x,y) 9, 8 : 4.8
      //
      float float_pixV_noNan[2][2]={
           {2.7f, 4.7f},
           {2.8f, 4.8f}
      };
   
      vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2_noNan {
         { 8.500, 7.500, static_cast<float>(float_pixV_noNan[0][0])},
         { 9.500, 8.500, static_cast<float>(float_pixV_noNan[1][1])},

         { 8.750, 7.500, static_cast<float>((static_cast<float>(float_pixV_noNan[0][0])*0.75f + static_cast<float>(float_pixV_noNan[0][1])*0.25f))},

         { 8.500, 7.750, static_cast<float>((static_cast<float>(float_pixV_noNan[0][0])*0.75f + static_cast<float>(float_pixV_noNan[1][0])*0.25f))},

         { 8.750, 7.750, static_cast<float>(
                     ((static_cast<float>(float_pixV_noNan[0][0])*0.75f + static_cast<float>(float_pixV_noNan[0][1])*0.25f)*0.75f)
                   + ((static_cast<float>(float_pixV_noNan[1][0])*0.75f + static_cast<float>(float_pixV_noNan[1][1])*0.25f)*0.25f)) },
      };

      bErrorReport &=test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::FLOAT, vect_sPixPick_cPXY_pixV_bilinearSquare2x2_noNan);



      //@LP here we check other cases to select nearest pixel
      //This test is done only for float (the behavior is the same for any type)
      {

          //without Nan test
          //
          // (x,y) 7, 6 : 0.39    | 8, 6 : 2.6  | 9, 6 : 4.6
          // (x,y) 7, 7 : 0.29    | 8, 7 : 2.7  | 9, 7 : 4.7

          float float_pixV_noNan[3][3]={
               {0.39f, 2.6f, 4.6f},
               {0.29f, 2.7f, 4.7f},
               {0.19f, 2.8f, 4.8f}
          };


          vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2_noNan_otherCasesAbout2x2Pixelselection {
             { 8.500, 7.250, static_cast<float>((float_pixV_noNan[1][1])*0.75f + static_cast<float>(float_pixV_noNan[0][1])*0.25f)},

             { 8.750, 7.250, static_cast<float>(
                             ((static_cast<float>(float_pixV_noNan[1][1])*0.75f + static_cast<float>(float_pixV_noNan[1][2])*0.25f)*0.75f)
                           + ((static_cast<float>(float_pixV_noNan[0][1])*0.75f + static_cast<float>(float_pixV_noNan[0][2])*0.25f)*0.25f)) },

             { 8.250, 7.250, static_cast<float>(
                             ((static_cast<float>(float_pixV_noNan[1][1])*0.75f + static_cast<float>(float_pixV_noNan[1][0])*0.25f)*0.75f)
                           + ((static_cast<float>(float_pixV_noNan[0][1])*0.75f + static_cast<float>(float_pixV_noNan[0][0])*0.25f)*0.25f)) },

             { 8.250, 7.500, static_cast<float>(float_pixV_noNan[1][1])*0.75f + static_cast<float>(float_pixV_noNan[1][0])*0.25f },

             { 8.250, 7.750, static_cast<float>(
                             ((static_cast<float>(float_pixV_noNan[1][1])*0.75f + static_cast<float>(float_pixV_noNan[1][0])*0.25f)*0.75f)
                           + ((static_cast<float>(float_pixV_noNan[2][1])*0.75f + static_cast<float>(float_pixV_noNan[2][0])*0.25f)*0.25f)) },

          };

          bErrorReport &=test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::FLOAT, vect_sPixPick_cPXY_pixV_bilinearSquare2x2_noNan_otherCasesAbout2x2Pixelselection);

      }


  }

  return(bErrorReport);
}

bool test_PixelPicker_signedInt16() {

    qDebug() << __FUNCTION__ ;

    const std::string strFilename("/home/laurent/inputDataForTests/inputImages/unitTest_pixelPicker/signed16_float_byte_15x15/s16_px1_wr.tiff");

  /*
   (x,y)  4, 7 : -17      (x,y)  5, 7 : -38
   (x,y)  4, 8 : -18      (x,y)  5, 8 : -37
  */
  signed short signedInt16_pixV[2][2]={
       {-17, -38},
       {-18, -37}
  };

  vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_nearest {
     {4.500, 7.500, static_cast<signed short int>(signedInt16_pixV[0][0])},

     {5.500, 8.500, static_cast<signed short int>(signedInt16_pixV[1][1])},

     {4.750, 7.500, static_cast<signed short int>(signedInt16_pixV[0][0])},

     {4.500, 7.750, static_cast<signed short int>(signedInt16_pixV[0][0])},

     {4.750, 7.750, static_cast<signed short int>(signedInt16_pixV[0][0])}
  };
  
  
  vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2 {
      {4.500, 7.500, static_cast<signed short int>(signedInt16_pixV[0][0])},
      {5.500, 8.500, static_cast<signed short int>(signedInt16_pixV[1][1])},

      {4.750, 7.500, static_cast<signed short int>((static_cast<float>(signedInt16_pixV[0][0])*0.75f + static_cast<float>(signedInt16_pixV[0][1])*0.25f))},

      {4.500, 7.750, static_cast<signed short int>((static_cast<float>(signedInt16_pixV[0][0])*0.75f + static_cast<float>(signedInt16_pixV[1][0])*0.25f))},

      {4.750, 7.750, static_cast<signed short int>(
                   ((static_cast<float>(signedInt16_pixV[0][0])*0.75f + static_cast<float>(signedInt16_pixV[0][1])*0.25f)*0.75f)
                 + ((static_cast<float>(signedInt16_pixV[1][0])*0.75f + static_cast<float>(signedInt16_pixV[1][1])*0.25f)*0.25f)) },
  };
  
  bool bErrorReport = true;
  bErrorReport &=test_PixelPicker_nearest(strFilename, TypeDesc::INT16, vect_sPixPick_cPXY_pixV_nearest);
  bErrorReport &=test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::INT16, vect_sPixPick_cPXY_pixV_bilinearSquare2x2);
   
  return(bErrorReport);
}

bool test_PixelPicker_unsignedInt16t() {

    qDebug() << __FUNCTION__ ;

    const std::string strFilename("/home/laurent/inputDataForTests/inputImages/unitTest_pixelPicker/unsigned16/LC81540422013269LGN00_B8_sub3.tif");

  /*
   (x,y)  749, 461 : 41      (x,y)  750, 461 : 47
   (x,y)  749, 462 : 46      (x,y)  750, 462 : 49
  */
  signed short unsignedInt16_pixV[2][2]={
       {10501, 12223},
       {12031, 12551}
  };

  vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_nearest {
     {749.500, 461.500, static_cast<signed short int>(unsignedInt16_pixV[0][0])},
     {750.500, 462.500, static_cast<signed short int>(unsignedInt16_pixV[1][1])},

     {749.750, 461.500, static_cast<signed short int>(unsignedInt16_pixV[0][0])},

     {749.500, 461.750, static_cast<signed short int>(unsignedInt16_pixV[0][0])},

     {749.750, 461.750, static_cast<signed short int>(unsignedInt16_pixV[0][0])}
  };


  vector<S_PixelPicker_continousPixelXY_pixelValue> vect_sPixPick_cPXY_pixV_bilinearSquare2x2 {
      {749.500, 461.500, static_cast<signed short int>(unsignedInt16_pixV[0][0])},
      {750.500, 462.500, static_cast<signed short int>(unsignedInt16_pixV[1][1])},

      {749.750, 461.500, static_cast<signed short int>((static_cast<float>(unsignedInt16_pixV[0][0])*0.75f + static_cast<float>(unsignedInt16_pixV[0][1])*0.25f))},

      {749.500, 461.750, static_cast<signed short int>((static_cast<float>(unsignedInt16_pixV[0][0])*0.75f + static_cast<float>(unsignedInt16_pixV[1][0])*0.25f))},

      {749.750, 461.750, static_cast<signed short int>(
                   ((static_cast<float>(unsignedInt16_pixV[0][0])*0.75f + static_cast<float>(unsignedInt16_pixV[0][1])*0.25f)*0.75f)
                 + ((static_cast<float>(unsignedInt16_pixV[1][0])*0.75f + static_cast<float>(unsignedInt16_pixV[1][1])*0.25f)*0.25f)) },
  };

  bool bErrorReport = true;
  bErrorReport &=test_PixelPicker_nearest(strFilename, TypeDesc::UINT16, vect_sPixPick_cPXY_pixV_nearest);
  bErrorReport &=test_PixelPicker_bilinearSquare2x2(strFilename, TypeDesc::UINT16, vect_sPixPick_cPXY_pixV_bilinearSquare2x2);

  return(bErrorReport);
    
}


bool test_PixelPicker_nearest(const std::string& strFilename,
                              unsigned char typeDescBaseTypeWhichNeedsToMatch,
                              const vector<S_PixelPicker_continousPixelXY_pixelValue>& vect_sPixPick_cPXY_pixV_nearest) {

    qDebug() << __FUNCTION__ << "strFilename = " << strFilename.c_str();

    PixelPicker pixPicker;
    //##LP no setInputImage error report check
    bool bSetReport = pixPicker.setInputImage(strFilename, true, typeDescBaseTypeWhichNeedsToMatch);
    if (!bSetReport) {
        qDebug() << "setInputImage failed";
        return(false);
    }
    int w = 0, h = 0;
    if (!pixPicker.getImageWidthHeight(w, h)) {
        qDebug() << "getImageWidthHeight failed";
        return(false);
    }

    qDebug() << "getNearestPixelValue_fromContinousPixelXY";
    bool bNoErrorOccured = true;

    for(auto xyv: vect_sPixPick_cPXY_pixV_nearest) {
        //getNearestPixelValue_fromContinousPixelXY

        qDebug() << "pfx: " << xyv._px;
        qDebug() << "pfy: " << xyv._py;

        Vec2<float> pfxy { xyv._px, xyv._py };
        float fPixelValue = .0;
        if (!pixPicker.getNearestPixelValue_fromContinousPixelXY(pfxy, fPixelValue)) {
            qDebug() << "[ ERROR ] with getNearestPixelValue_fromContinousPixelXY";
            bNoErrorOccured = false;
        } else {
            qDebug() << "fPixelValue = " << fPixelValue;
        }

        U_PixelValue uPixelValueGot;

        bool bTypeHandled = true;
        //float fExpected = .0;
        switch(typeDescBaseTypeWhichNeedsToMatch) {
            case TypeDesc::UINT8:
                uPixelValueGot._uint8PixelValue = static_cast<unsigned char>(fPixelValue);
                break;
            case TypeDesc::UINT16:
                uPixelValueGot._uint16PixelValue = static_cast<unsigned short int>(fPixelValue);
                break;
            case TypeDesc::INT16:
                uPixelValueGot._int16PixelValue = static_cast<signed short int>(fPixelValue);
                break;
            case TypeDesc::FLOAT:
                uPixelValueGot._floatPixelValue = static_cast<float>(fPixelValue);
                break;
            default:
                bTypeHandled = false;
        }
        if (!bTypeHandled) {
            bNoErrorOccured = false;
            qDebug() << "[ ERROR ] !bTypeHandled #1";
        } else {

            bool bNanChecked = false;

            qDebug() << "got fPixelValue = " << fPixelValue;
            if (typeDescBaseTypeWhichNeedsToMatch == TypeDesc::FLOAT) {
                bool bfPixelValue_isNan = std::isnan(uPixelValueGot._floatPixelValue);
                bool bfExpected_isNan   = std::isnan(xyv._pixelValue._floatPixelValue);

                if (bfPixelValue_isNan || bfExpected_isNan) {
                    bNanChecked = true;

                    if (bfPixelValue_isNan && bfExpected_isNan) {
                        qDebug() << "[  OK   ], Nan as expexted";
                    } else {
                        if (!bfPixelValue_isNan && bfExpected_isNan) {
                            qDebug() << "[ ERROR ], should be Nan";
                        } else {
                            qDebug() << "[ ERROR ], should not be Nan";
                        }
                    }
                }
            }

            if (!bNanChecked) {

                bool bEqual = false;

                switch(typeDescBaseTypeWhichNeedsToMatch) {
                    case TypeDesc::UINT8:
                        qDebug() << uPixelValueGot._uint8PixelValue << " == " << xyv._pixelValue._uint8PixelValue;
                        bEqual = (uPixelValueGot._uint8PixelValue == xyv._pixelValue._uint8PixelValue);
                        break;
                    case TypeDesc::UINT16:
                        qDebug() << uPixelValueGot._uint16PixelValue << " == " <<  xyv._pixelValue._uint16PixelValue;
                        bEqual = (uPixelValueGot._uint16PixelValue == xyv._pixelValue._uint16PixelValue);
                        break;
                    case TypeDesc::INT16:
                        qDebug() << uPixelValueGot._int16PixelValue  << " == " <<  xyv._pixelValue._int16PixelValue;
                        bEqual = (uPixelValueGot._int16PixelValue == xyv._pixelValue._int16PixelValue);
                        break;
                    case TypeDesc::FLOAT:
                        qDebug() << uPixelValueGot._floatPixelValue  << " == " <<  xyv._pixelValue._floatPixelValue;
                        bEqual = doubleValuesAreClose(uPixelValueGot._floatPixelValue, xyv._pixelValue._floatPixelValue, 0.0000001);
                        break;
                    default:
                        bTypeHandled = false;
                }

                if (!bTypeHandled) {
                    bNoErrorOccured = false;
                    qDebug() << "[ ERROR ] !bTypeHandled #2";
                } else {
                    if (bEqual) {
                        qDebug() << "[  OK   ], expected pixel value";
                    } else {
                        bNoErrorOccured = false;
                        qDebug() << "[ ERROR ]  value error";
                    }
                }
            }
        }
    }

    return(bNoErrorOccured);
}

bool test_PixelPicker_bilinearSquare2x2(const std::string& strFilename,
                              unsigned char typeDescBaseTypeWhichNeedsToMatch,
                              const vector<S_PixelPicker_continousPixelXY_pixelValue>& vect_sPixPick_cPXY_pixV_bilinearSquare2x2) {

    qDebug() << __FUNCTION__ << "strFilename = " << strFilename.c_str();

    PixelPicker pixPicker;
    //##LP no setInputImage error report check
    bool bSetReport = pixPicker.setInputImage(strFilename, true, typeDescBaseTypeWhichNeedsToMatch);
    if (!bSetReport) {
        qDebug() << "setInputImage failed";
        return(false);
    }
    int w = 0, h = 0;
    if (!pixPicker.getImageWidthHeight(w, h)) {
        qDebug() << "getImageWidthHeight failed";
        return(false);
    }

    qDebug() << "getNearestPixelValue_fromContinousPixelXY";
    bool bNoErrorOccured = true;

    for(auto xyv: vect_sPixPick_cPXY_pixV_bilinearSquare2x2) {
        //getNearestPixelValue_fromContinousPixelXY

        qDebug() << "pfx: " << xyv._px;
        qDebug() << "pfy: " << xyv._py;

        Vec2<float> pfxy { xyv._px, xyv._py };
        float fPixelValue = .0;
        if (!pixPicker.getBilinearInterpolatedPixelValue_fromContinousPixelXY(pfxy, fPixelValue)) {
            qDebug() << "[ ERROR ] with getLinearInterpolatedPixelValue_fromContinousPixelXY";
            bNoErrorOccured = false;

        } else {
            qDebug() << "fPixelValue = " << fPixelValue;
        }

        U_PixelValue uPixelValueGot;

        bool bTypeHandled = true;
        //float fExpected = .0;
        switch(typeDescBaseTypeWhichNeedsToMatch) {
            case TypeDesc::UINT8:
                uPixelValueGot._uint8PixelValue = static_cast<unsigned char>(fPixelValue);
                break;
            case TypeDesc::UINT16:
                uPixelValueGot._uint16PixelValue = static_cast<unsigned short int>(fPixelValue);
                break;
            case TypeDesc::INT16:
                uPixelValueGot._int16PixelValue = static_cast<signed short int>(fPixelValue);
                break;
            case TypeDesc::FLOAT:
                uPixelValueGot._floatPixelValue = /* static_cast<float>(*/fPixelValue/*)*/;
                break;
            default:
                bTypeHandled = false;
        }
        if (!bTypeHandled) {
            bNoErrorOccured = false;
            qDebug() << "[ ERROR ] !bTypeHandled #1";
        } else {

            bool bNanChecked = false;

            qDebug() << "got fPixelValue = " << fPixelValue;
            if (typeDescBaseTypeWhichNeedsToMatch == TypeDesc::FLOAT) {
                bool bfPixelValue_isNan = std::isnan(uPixelValueGot._floatPixelValue);
                bool bfExpected_isNan   = std::isnan(xyv._pixelValue._floatPixelValue);

                if (bfPixelValue_isNan || bfExpected_isNan) {
                    bNanChecked = true;

                    if (bfPixelValue_isNan && bfExpected_isNan) {
                        qDebug() << "[  OK   ], Nan as expexted";
                    } else {
                        if (!bfPixelValue_isNan && bfExpected_isNan) {
                            qDebug() << "[ ERROR ], should be Nan";
                        } else {
                            qDebug() << "[ ERROR ], should not be Nan";
                        }
                    }
                }
            }

            if (!bNanChecked) {

                bool bEqual = false;

                switch(typeDescBaseTypeWhichNeedsToMatch) {
                    case TypeDesc::UINT8:
                        qDebug() << uPixelValueGot._uint8PixelValue << " == " << xyv._pixelValue._uint8PixelValue;
                        bEqual = (uPixelValueGot._uint8PixelValue == xyv._pixelValue._uint8PixelValue);
                        break;
                    case TypeDesc::UINT16:
                        qDebug() << uPixelValueGot._uint16PixelValue << " == " <<  xyv._pixelValue._uint16PixelValue;
                        bEqual = (uPixelValueGot._uint16PixelValue == xyv._pixelValue._uint16PixelValue);
                        break;
                    case TypeDesc::INT16:
                        qDebug() << uPixelValueGot._int16PixelValue  << " == " <<  xyv._pixelValue._int16PixelValue;
                        bEqual = (uPixelValueGot._int16PixelValue == xyv._pixelValue._int16PixelValue);
                        break;
                    case TypeDesc::FLOAT:
                        qDebug() << uPixelValueGot._floatPixelValue  << " == " <<  xyv._pixelValue._floatPixelValue;
                        bEqual = doubleValuesAreClose(uPixelValueGot._floatPixelValue, xyv._pixelValue._floatPixelValue, 0.0000001);
                        break;
                    default:
                        bTypeHandled = false;
                }

                if (!bTypeHandled) {
                    bNoErrorOccured = false;
                    qDebug() << "[ ERROR ] !bTypeHandled #2";
                } else {
                    if (bEqual) {
                        qDebug() << "[  OK   ], expected pixel value";
                    } else {
                        bNoErrorOccured = false;
                        qDebug() << "[ ERROR ]  value error";
                    }
                }
            }
        }
    }
    return(bNoErrorOccured);
}
