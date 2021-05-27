#!/bin/bash

root_inputimage__directory="/home/laurent/10fev2021/tests_data/inputImagesToConvertToTiff"
root_outputimage_directory="./convertedImages_CanBeUsedInApp"

if true; then

#Px1_Px2_square_900x900/900x900/px1_vlines_n0_p1_m1.png
#Px1_Px2_square_900x900/900x900/px2_hlines_n0_p1_m1.png

#convert to MicMac byte tiff corelation score map
#gray value (mean of RGB) => 0 to 255 (byte)
#./convertPngToByteTiffImage_MeanColor \
./convertPngToFloatTiffImage_PureRGB-WhiteBlack RGBGRAY \
$root_inputimage__directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_255_180_100_0.tif \
$root_outputimage_directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_255_180_100_0.tif

#convert to float tiff
#pure black value => -2.0
./convert_ImgRGBToTiff_multimodes GREEN2FLOAT \
$root_inputimage__directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_black.tif \
$root_outputimage_directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_black.tif \
-2 +10

#convert to float tiff
#pure white value => +10.0
./convert_ImgRGBToTiff_multimodes GREEN2FLOAT \
$root_inputimage__directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_white.tif \
$root_outputimage_directory/966x474_correlscore4values_AndPureWhiteBlack/corelltest_white.tif \
-2 +10

#convert to byte tiff (mean of RGB)
#./convert_ImgRGBToTiff_multimodes RGBGRAY \
#$(ROOT_INPUTIMAGE__DIRECTORY)/correlscore_square_horizontal_0_127_255/2100x2100/in_bigImage_correlscore_0_127_255.tif \
#$(ROOT_OUTPUTIMAGE_DIRECTORY)/correlscore_square_horizontal_0_127_255/2100x2100/bigImage_correlscore_0_127_255.tif 

#convert to byte tiff (mean of RGB)
./convert_ImgRGBToTiff_multimodes RGBGRAY \
$root_inputimage__directory/correlscore_square_horizontal_0_127_255/900x900/in_smallImage_correlscore_0_127_255.tif \
$root_outputimage_directory/correlscore_square_horizontal_0_127_255/900x900/smallImage_correlscore_0_127_255.tif

#convert to float tiff using specific float values for specific pure color (R, G, B, White, Black) (Magenta produces Nan value)
./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1_Px2/square_900x900/px1_vlines_n0_p1_m1.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1_Px2/square_900x900/px1_vlines_n0_p1_m1.tiff \

./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1_Px2/square_900x900/px2_hlines_n0_p1_m1.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1_Px2/square_900x900/px2_hlines_n0_p1_m1.tiff

#convert to float tiff using specific float values for specific pure color (R, G, B, White, Black) (Magenta produces Nan value)
./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1_Px2/square_Nan_2100x2100/px1_nan_0p1m1.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1_Px2/square_Nan_2100x2100/px1_nan_0p1m1.tiff \

./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1_Px2/square_Nan_2100x2100/px2_nan_0p1m1.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1_Px2/square_Nan_2100x2100/px2_nan_0p1m1.tiff

#big images Px1 Px2 and delta Z with white text on black background. Converted to float for images view tests
./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1Px2DeltaZ_20Kx30K_testImageView/1.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1Px2DeltaZ_20Kx30K_testImageView/float_1.tiff

fi

./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1Px2DeltaZ_20Kx30K_testImageView/2.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1Px2DeltaZ_20Kx30K_testImageView/float_2.tiff

./convert_ImgRGBToTiff_multimodes RGBWBM \
$root_inputimage__directory/Px1Px2DeltaZ_20Kx30K_testImageView/z.png \
-1 0 +1 +3 -3 5 \
$root_outputimage_directory/Px1Px2DeltaZ_20Kx30K_testImageView/float_z.tiff

