#!/bin/bash

echo "find Qt 5 dynamic library to join to the built application binary"

root_path_for_search="/opt/Qt5.12.5/5.12.5"

#findFiles_function libicudata.so.*

#.56
echo        "libicudata.so.* >"
find $root_path_for_search -name libicudata.so.*
echo

echo        "libicui18n.so.* >"
find $root_path_for_search -name libicui18n.so.*  
echo

echo        "libicuuc.so.* >"
find $root_path_for_search -name libicuuc.so.*  
echo

#.5 
echo        "libQt5Core.so.* >"
find $root_path_for_search -name libQt5Core.so.*
echo

echo        "libQt5DBus.so.* >"  
find $root_path_for_search -name libQt5DBus.so.* 
echo

echo        "libQt5Gui.so.* >"
find $root_path_for_search -name libQt5Gui.so.*  
echo

echo        "libQt5Widgets.so.* >"
find $root_path_for_search -name libQt5Widgets.so.* 
echo
 
echo        "libQt5XcbQpa.so.* >"
find $root_path_for_search -name libQt5XcbQpa.so.*  
echo

#platforms/libqxcb.so
echo        "libqxcb.so >"
find $root_path_for_search -name libqxcb.so
echo

