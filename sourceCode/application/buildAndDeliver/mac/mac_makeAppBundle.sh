#!/bin/bash

# After the Qt app compilation and linking, running this script makes the app Bundle
# This script:
# - call macdeployqt 
# - copy the others requiered dylibs specific to the app in the bundle and call install_name_tool on it (and on the app binary)
# - add the requiered resources:
#   . proj.db
#

#this script works when launched from the directory containng the script
#because it will refers later to this directory to copy the Info.plist later in this script
directoryOfFile_Infoplist_ToCopyInAppBundle=$(pwd)
 
QtFrameworksDir="/Users/laurent/Qt/5.13.2/clang_64"
QtFrameworksDir_bin=$QtFrameworksDir/bin
 
echo $QtFrameworksDir
echo $QtFrameworksDir_bin

#set this directory:
root_appBundleRelease="/Users/laurent/testcompil/testfullApp/build-macos_test_zoompan_05mars2021_minOS10dot14-Desktop_Qt_5_13_2_clang_64-Release"

appBinName="stackprof"

appBundleDirName="stackprof.app"

echo $root_appBundleRelease
echo $appBundleDirName

#macdeployqt call 
cd $root_appBundleRelease
$QtFrameworksDir_bin/macdeployqt $appBundleDirName 

#dylibs about proj (proj and sqlite3)
#considers no symbolic links about dylib
#

#set this directory:
rootDir_dylibToAddToAppBundle=/Users/laurent/testcompil/testfullApp/dylibsForAppBundle_27oct
#set this directory:
additionalAppDataDir=/Users/laurent/testcompil/testfullApp/additionalAppData


dylibPath_proj=$rootDir_dylibToAddToAppBundle/proj
dylibName_proj="libproj.18.0.3.dylib"

dylibPath_sqlite3=$rootDir_dylibToAddToAppBundle/sqlite3
sqlite3_dylibName="libsqlite3.0.dylib"

appBundle_ContentsDir=$root_appBundleRelease/$appBundleDirName/Contents

appBundle_ContentsLibsDir=$appBundle_ContentsDir/libs
mkdir $appBundle_ContentsLibsDir

cp $dylibPath_sqlite3/$sqlite3_dylibName $appBundle_ContentsLibsDir 
cp $dylibPath_proj/$dylibName_proj $appBundle_ContentsLibsDir 

echo "Before app and libs links relocations: result of otool -L on appBinName before using install_name_tool"
otool -L $appBundle_ContentsDir/MacOS/$appBinName

echo "Before app and libs links relocations: result of otool -L on appBinName before using install_name_tool"
otool -L $appBundle_ContentsLibsDir/$dylibName_proj
otool -L $appBundle_ContentsLibsDir/$sqlite3_dylibName

# - change app 'look at' to find proj
install_name_tool -change           @rpath/libproj.18.dylib @executable_path/../libs/$dylibName_proj $appBundle_ContentsDir/MacOS/$appBinName
# - change proj 'look at' to find sqlite3
install_name_tool -change /usr/local/lib/libsqlite3.0.dylib @executable_path/../libs/$sqlite3_dylibName $appBundle_ContentsLibsDir/$dylibName_proj


#uses local compilation of libtiff instead of the one coming from /user/local/opt :
dylibPath_tiff=$rootDir_dylibToAddToAppBundle/libtiff
dylibName_tiff="libtiff.5.5.0.dylib"

cp $dylibPath_tiff/$dylibName_tiff $appBundle_ContentsLibsDir 

otool -L $appBundle_ContentsLibsDir/$dylibName_tiff

# - change app 'look at' to find libtiff
install_name_tool -change           @rpath/libtiff.5.dylib @executable_path/../libs/$dylibName_tiff $appBundle_ContentsDir/MacOS/$appBinName

#alter also libs using -id:
install_name_tool -id @executable_path/../libs/$dylibName_proj      $appBundle_ContentsLibsDir/$dylibName_proj
install_name_tool -id @executable_path/../libs/$sqlite3_dylibName $appBundle_ContentsLibsDir/$sqlite3_dylibName
install_name_tool -id @executable_path/../libs/$dylibName_tiff        $appBundle_ContentsLibsDir/$dylibName_tiff


echo "After app and libs links relocations: result of otool -L on appBinName before using install_name_tool"
otool -L $appBundle_ContentsDir/MacOS/$appBinName

echo "After app and libs links relocations: result of otool -L on appBinName before using install_name_tool"
otool -L $appBundle_ContentsLibsDir/$dylibName_proj      
                                   
otool -L $appBundle_ContentsLibsDir/$sqlite3_dylibName                                         
otool -L $appBundle_ContentsLibsDir/$dylibName_tiff                                         



additionalAppData_projDir=$additionalAppDataDir/projdb

#copy additionnal data files
appBundle_ContentsResourcesDir=$appBundle_ContentsDir/Resources

echo "appBundle_ContentsResourcesDir = " $appBundle_ContentsResourcesDir
#copy proj database in Contents/Resources/projdb
mkdir $appBundle_ContentsResourcesDir/projdb
cp $additionalAppData_projDir/proj.db $appBundle_ContentsResourcesDir/projdb/

: '
# https://doc.qt.io/qt-5/highdpi.html speak about: 
 <key>NSPrincipalClass</key>
 <string>NSApplication</string>
 <key>NSHighResolutionCapable</key>
 <string>True</string>

 And say also: Newer versions of qmake will generate an Info.plist file with the NSPrincipalClass key; this is sufficient since NSHighResolutionCapable is true by default. 


 the generated Info.plist by Qt do not include NSHighResolutionCapable key/value
 the doc: https://doc.qt.io/qt-5/highdpi.html
The Apple platforms implement scaling and coordinate system virtualization in the operating system. Normally, no special configuration is required.

On macOS, high-DPI support is enabled by settings in the Info.plist file; so make sure these settings are present.

<key>NSPrincipalClass</key>
<string>NSApplication</string>
<key>NSHighResolutionCapable</key>
<string>True</string>
Newer versions of qmake will generate an Info.plist file with the NSPrincipalClass key; this is sufficient since NSHighResolutionCapable is true by default.

Note: Both macOS and iOS may apply further virtualization, such that device pixels no longer correspond to display pixels 1:1. This happens on the iPhone 6+ and on macOS configured with "display scaling" enabled.

Consequently, this is what we use as info.plist to handle any case:

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>stackprof</string>
	<key>CFBundleGetInfoString</key>
	<string>Created by Qt/QMake</string>
	<key>CFBundleIconFile</key>
	<string></string>
	<key>CFBundleIdentifier</key>
	<string>com.yourcompany.stackprof</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>LSMinimumSystemVersion</key>
	<string>10.14</string>
	<key>NOTE</key>
	<string>This file was generated by Qt/QMake.</string>
	<key>NSPrincipalClass</key>
	<string>NSApplication</string>
	<key>NSSupportsAutomaticGraphicsSwitching</key>
	<true/>
	<key>NSHighResolutionCapable</key>
	<true/>
</dict>
</plist>
'

echo "Info.plist before: ---"
cat $appBundle_ContentsDir/Info.plist
echo "--- eof ---"

#./Info.plist contains for now the version juste above ( NSHighResolutionCapable at YES )
cp $directoryOfFile_Infoplist_ToCopyInAppBundle/Info_27oct_targetDeployment1014_HRCYesBoolean.plist $appBundle_ContentsDir/Info.plist

echo "Info.plist after: ---"
cat $appBundle_ContentsDir/Info.plist
echo "--- eof ---"
