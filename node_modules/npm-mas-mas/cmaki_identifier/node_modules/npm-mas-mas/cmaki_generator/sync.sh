#!/bin/bash

#pattern=*.py CMakeLists.txt
pattern="utils.cmake common.yml CMakeLists.txt *.py"
other_dir=$1

for i in $(ls $pattern); do
	if [ -f $other_dir/$i ]; then
		diff $i $other_dir/$i > /dev/null || meld $i $other_dir/$i
	fi
done

