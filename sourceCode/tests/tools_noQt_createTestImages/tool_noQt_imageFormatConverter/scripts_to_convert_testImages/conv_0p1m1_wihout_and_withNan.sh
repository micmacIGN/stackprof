#!/bin/bash

#biggerImage_correlscore_0_127_255.png
#biggerImage_correlscore_0_127_255.tif
#correlscore_0_127_255.png
#correlscore_0_127_255.tif
#px1_nan_0p1m1.png
#px2_nan_0p1m1.png


./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack ./input/18juin_test_nan_0p1m1/px1_nan_0p1m1.tif -1 0 +1 +3 -3 5 \
./output/float_px1_nan_0p1m1.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack ./input/18juin_test_nan_0p1m1/px2_nan_0p1m1.tif -1 0 +1 +3 -3 5 \
./output/float_px2_nan_0p1m1.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack ./input/18juin_test_nan_0p1m1/px1_vlines_n0_p1_m1.tif -1 0 +1 +3 -3 5 \
./output/float_px1_vlines_n0_p1_m1.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack ./input/18juin_test_nan_0p1m1/px2_hlines_n0_p1_m1.tif -1 0 +1 +3 -3 5 \
./output/float_px2_hlines_n0_p1_m1.tif

