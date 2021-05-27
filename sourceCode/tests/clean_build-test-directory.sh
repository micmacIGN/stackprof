#!/bin/bash

build_test_dir=/home/laurent/work/dev_techtest_sandbox/about_Qt/build-test

cd $build_test_dir

pattern_array=(
"*.o"
"moc_*.cpp" 
"moc_*.h"
"Makefile" 
"test_Qt*"
)

pattern_array_rejectWithExtension=(
false
false 
false
true 
true
)

for idx in ${!pattern_array[@]}; do
  echo "find "${pattern_array[$idx]}
  if ${pattern_array_rejectWithExtension[$idx]} 
    then
      echo "(file with extension rejected)"
      find . -name ${pattern_array[$idx]} ! -name "*.*" -type f
  else
    echo "(file with extension permitted)"
    find . -name ${pattern_array[$idx]} -type f
  fi
done


echo "Do you confirm to remove this files list ?"
read -p "Confirm ?" -n 1 -r
echo # (optional) move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	echo "remove canceled"
	exit 1
fi


for idx in ${!pattern_array[@]}; do
  echo "find and remove "${pattern_array[$idx]}
  if ${pattern_array_rejectWithExtension[$idx]} 
    then
      echo "(file with extension rejected)"
      find . -name ${pattern_array[$idx]} ! -name "*.*" -type f -delete
  else
    echo "(file with extension permitted)"
    find . -name ${pattern_array[$idx]} -type f -delete
  fi
done

