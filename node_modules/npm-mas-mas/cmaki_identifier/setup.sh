#!/bin/bash

if [ -d "boostorg_predef" ]; then
	rm -Rf boostorg_predef
fi
git clone -q https://github.com/boostorg/predef.git boostorg_predef

../cmaki_scripts/setup.sh
