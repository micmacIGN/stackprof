#!/bin/bash

#biggerImage_correlscore_0_127_255.png
#biggerImage_correlscore_0_127_255.tif
#correlscore_0_127_255.png
#correlscore_0_127_255.tif
#px1_nan_0p1m1.png
#px2_nan_0p1m1.png

./forUnitTest_convertPngToByteTiffImage_MeanColor \
../correltest1/in/corelltest_255_180_100_0.png \
../correltest1/corelltest_255_180_100_0.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBMEAN2FLOAT \
../correltest1/in/corelltest_black.png \
../correltest1/corelltest_black.tif \
-2 +10

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBMEAN2FLOAT \
../correltest1/in/corelltest_white.png \
../correltest1/corelltest_white.tif \
-2 +10

