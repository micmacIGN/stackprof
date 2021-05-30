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


# https://github.com/linuxmint/cinnamon/issues/4902
# try:
export QT_SCALE_FACTOR=1
export QT_AUTO_SCREEN_SCALE_FACTOR=0
#export QT_SCREEN_SCALE_FACTORS=1
export QT_SCREEN_SCALE_FACTORS=2
#

$dirname/$appname arg1 "$@"
#$dirname/demo1 arg1 "$@"

echo "app terminated"
