#/bin/bash

appname=stackprof

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH = " $LD_LIBRARY_PATH

echo "appname: " $appname
echo "dirname: " $dirname

export QT_QPA_PLATFORM_PLUGIN_PATH=$dirname/plugins/
echo "QT_QPA_PLATFORM_PLUGIN_PATH = " $QT_QPA_PLATFORM_PLUGIN_PATH

export QT_DEBUG_PLUGINS=1



echo "ldd libicudata.so.56"
ldd libicudata.so.56

echo "ldd libicui18n.so.56"
ldd libicui18n.so.56

echo "ldd libicuuc.so.56"
ldd libicuuc.so.56

echo "ldd libproj.so.18"
ldd libproj.so.18

echo "ldd libQt5Core.so.5"
ldd libQt5Core.so.5

echo "ldd libQt5DBus.so.5"
ldd libQt5DBus.so.5

echo "ldd libQt5Gui.so.5"
ldd libQt5Gui.so.5

echo "ldd libQt5Widgets.so.5"
ldd libQt5Widgets.so.5

echo "ldd libQt5XcbQpa.so.5"
ldd libQt5XcbQpa.so.5
    
echo "ldd libsqlite3.so"
ldd libsqlite3.so

echo "ldd libtiff.so.5"
ldd libtiff.so.5

echo "ldd platforms/libqxcb.so"
ldd platforms/libqxcb.so

echo "ldd stackprof"
ldd stackprof



