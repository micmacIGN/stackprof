#!/bin/bash

DIR_SRC_INPUT="/home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ"

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM /home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/1.png -1 0 +1 +3 -3 5 \
/home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/out/float_1.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM /home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/2.png -1 0 +1 +3 -3 5 \
/home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/out/float_2.tif

./forUnitTest_convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBWBM /home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/z.png -1 0 +1 +3 -3 5 \
/home/laurent/_appgeotest/testswitch/20K30K_PX1_PX2_deltaZ/out/float_z.tif

