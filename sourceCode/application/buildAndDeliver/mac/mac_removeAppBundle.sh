#!/bin/bash

# before new Qt app compilation and linking, run this script to remove the previous app Bundle

#set this directory:
root_appBundleRelease="/Users/laurent/testcompil/testfullApp/build-macos_test_zoompan_05mars2021_minOS10dot14-Desktop_Qt_5_13_2_clang_64-Release"

appBundleDirName="stackprof.app"

#echo $root_appBundleRelease
#echo $appBundleDirName
directoryToRemove=$root_appBundleRelease/$appBundleDirName

echo "Confirm to remove the appBundle directory:"
echo $directoryToRemove
read -p "Confirm ?" -n 1 -r
echo # (optional) move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	echo "remove canceled"
	exit 1
else
	rm -rf $directoryToRemove
	echo "remove done"	
fi
