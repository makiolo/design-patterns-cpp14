#!/bin/bash

function print_if_has_content()
{
	file=$1
	minimumsize=400
	actualsize=$(wc -c <"$file")
	if [ $actualsize -ge $minimumsize ];
	then
		cat $file
	fi
}

echo Running test: $1
export ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer-3.6)
export ASAN_OPTIONS="check_initialization_order=1"
rm $1.coverage 2> /dev/null
rm $1.gcno 2> /dev/null
rm default.profraw 2> /dev/null
./$1
ret=$?
llvm-profdata-3.6 merge -o $1.gcno default.profraw 2> /dev/null
llvm-cov-3.6 show ./$1 -instr-profile=$1.gcno > $1.coverage
cat $1.coverage | ansi2html > $1.html
print_if_has_content $1.html
exit $ret

