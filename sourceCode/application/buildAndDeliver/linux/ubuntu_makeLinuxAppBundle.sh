#!/bin/bash

DirectoryOfBinToJoinToBuiltAppBin=/home/laurent/Documents/binToJoinToAppBundle

DirectoryOfBuiltAppBin=/home/laurent/stackpro/build-ubuntu_VMprocedureRecompil-Desktop_Qt_5_12_10_GCC_64bit-Release
BuiltAppBinName=test_stackpro

DestDirectoryBundle=/home/laurent/Documents/linuxAppBundle

cd $DestDirectoryBundle
ls -al $DestDirectoryBundle

cp $DirectoryOfBuiltAppBin/$BuiltAppBinName $DestDirectoryBundle
cp -R $DirectoryOfBinToJoinToBuiltAppBin/* $DestDirectoryBundle

ls -al $DestDirectoryBundle
