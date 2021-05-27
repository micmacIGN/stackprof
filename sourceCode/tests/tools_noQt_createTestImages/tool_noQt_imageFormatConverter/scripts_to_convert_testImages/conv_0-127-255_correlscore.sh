#!/bin/bash

#biggerImage_correlscore_0_127_255.png
#biggerImage_correlscore_0_127_255.tif
#correlscore_0_127_255.png
#correlscore_0_127_255.tif
#px1_nan_0p1m1.png
#px2_nan_0p1m1.png


./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBGRAY \
/home/laurent/_appgeotest/24juin2020/bigImage/in/in_bigImage_correlscore_0_127_255.tif \
/home/laurent/_appgeotest/24juin2020/bigImage/bigImage_correlscore_0_127_255.tif 

./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBGRAY \
/home/laurent/_appgeotest/24juin2020/smallImage/in/in_smallImage_correlscore_0_127_255.tif \
/home/laurent/_appgeotest/24juin2020/smallImage/smallImage_correlscore_0_127_255.tif

#./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM ./input/18juin_test_nan_0p1m1/px1_nan_0p1m1.tif \
#-1 0 +1 +3 -3 5 ./output/float_px1_nan_0p1m1.tif

#./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM ./input/18juin_test_nan_0p1m1/px2_nan_0p1m1.tif \
#-1 0 +1 +3 -3 5 ./output/float_px2_nan_0p1m1.tif

#./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM ./input/18juin_test_nan_0p1m1/px1_vlines_n0_p1_m1.tif \
#-1 0 +1 +3 -3 5 ./output/float_px1_vlines_n0_p1_m1.tif

#./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM ./input/18juin_test_nan_0p1m1/px2_hlines_n0_p1_m1.tif \
#-1 0 +1 +3 -3 ./output/float_px2_hlines_n0_p1_m1.tif

