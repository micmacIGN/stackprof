#/bin/bash

root_appBundleRelease="/Users/laurent/testcompil/testfullApp/build-macos_test_zoompan_27oct_minOS10dot14-Desktop_Qt_5_13_2_clang_64bit-Release"

appBundleDirName="stackprof.app"

self_signed_certificate="test_stackpro_cert2"

echo signing app with self-signed certificate:
sudo codesign --deep -fs $self_signed_certificate $root_appBundleRelease/$appBundleDirName

echo display for check:
sudo codesign -d -vvv $root_appBundleRelease/$appBundleDirName

