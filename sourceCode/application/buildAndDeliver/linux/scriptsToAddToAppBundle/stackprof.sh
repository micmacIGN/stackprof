#!/bin/sh
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

export QT_QPA_PLATFORM_PLUGIN_PATH=$dirname/platforms/
echo "QT_QPA_PLATFORM_PLUGIN_PATH = " $QT_QPA_PLATFORM_PLUGIN_PATH

#export QT_DEBUG_PLUGINS=1
export QT_DEBUG_PLUGINS=0

echo "start app:"

echo $dirname/$appname 
echo "$@"

$dirname/$appname arg1 "$@"

echo "app terminated"
