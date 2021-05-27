#!/bin/bash

conv_tool="../convert_ImgRGBToTiff_multimodes"

path_of_root_for_io_file="/home/laurent/testdata/finalSelection_inputImages/appTest_correlScoreWeightPower"
path_of_input_file=$path_of_root_for_io_file/ImagetoConvert
path_of_output_file=$path_of_root_for_io_file/readyAsAppInput


input_file_correl_0=$path_of_input_file/correlmap0_rgb.png
input_file_correl_1=$path_of_input_file/correlmap1_rgb.png
input_file_correl_2=$path_of_input_file/correlmap2_rgb.png

output_file_correl_byte_0=$path_of_output_file/readyasinput_correlmap0_byte.tif
output_file_correl_byte_1=$path_of_output_file/readyasinput_correlmap1_byte.tif
output_file_correl_byte_2=$path_of_output_file/readyasinput_correlmap2_byte.tif


input_file_px1_1=$path_of_input_file/dispmap1_rgb.png
input_file_px2_1=$path_of_input_file/dispmap1_rgb.png
input_file_deltaZ_1=$path_of_input_file/dispmap1_rgb.png

input_file_px1_2=$path_of_input_file/dispmap2_rgb.png
input_file_px2_2=$path_of_input_file/dispmap2_rgb.png
input_file_deltaZ_2=$path_of_input_file/dispmap2_rgb.png

output_file_px1_1=$path_of_output_file/readyasinput_dismap1_float_px1.tif
output_file_px2_1=$path_of_output_file/readyasinput_dismap1_float_px2.tif
output_file_deltaZ_1=$path_of_output_file/readyasinput_dismap1_float_dz.tif

output_file_px1_2=$path_of_output_file/readyasinput_dispmap2_float_px1.tif
output_file_px2_2=$path_of_output_file/readyasinput_dispmap2_float_px2.tif
output_file_deltaZ_2=$path_of_output_file/readyasinput_dispmap2_float_dz.tif

$conv_tool RGBGRAY $input_file_correl_0 $output_file_correl_byte_0
$conv_tool RGBGRAY $input_file_correl_1 $output_file_correl_byte_1
$conv_tool RGBGRAY $input_file_correl_2 $output_file_correl_byte_2

$conv_tool GREEN2FLOAT $input_file_px1_1    $output_file_px1_1    0 +1
$conv_tool GREEN2FLOAT $input_file_px2_1    $output_file_px2_1    0 +1
$conv_tool GREEN2FLOAT $input_file_deltaZ_1 $output_file_deltaZ_1 0 +1

$conv_tool GREEN2FLOAT $input_file_px1_2    $output_file_px1_2    0 +1
$conv_tool GREEN2FLOAT $input_file_px2_2    $output_file_px2_2    0 +1
$conv_tool GREEN2FLOAT $input_file_deltaZ_2 $output_file_deltaZ_2 0 +1

