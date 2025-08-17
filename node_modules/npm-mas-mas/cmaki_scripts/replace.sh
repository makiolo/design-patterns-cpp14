#!/bin/bash

MV="git mv"

if [[ $3 == "run" ]];
then
	# do sed implace
	run=" -i"
else
	run=""
fi

command="ag -w $1 -l --ignore artifacts --ignore node_modules --ignore gcc --ignore clang --ignore bin"
command_search_files="$command | grep -e $1.cpp$ -e $1.h$"
command_search_files_count="$command_search_files | xargs -I{} grep -h -e ^#include {} | grep -h $1 | wc -l"
count=$(eval $command_search_files_count)

if [[ $count -gt 0 ]];
then
	echo "se renonbrara los siguientes ficheros (utilizando $MV):"
	for file in $(eval $command_search_files);
	do
		destiny=$(echo $file | sed "s/\<$1\>/$2/g")
		if [[ $3 == "run" ]];
		then
			echo run: $MV $file $destiny
			$MV $file $destiny
		else
			echo dry-run: $MV $file $destiny
		fi
	done
else
	echo "No es necesario renombrar ficheros"
fi

if [[ $3 == "run" ]];
then
	# echo run: "$command | xargs sed "s/\<$1\>/$2/g" $run"
	echo replacing ...
else
	echo replace in dry-run
fi
eval $command | xargs -I{} sed "s@\<$1\>@$2@g" $run {}

