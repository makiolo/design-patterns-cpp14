#!/bin/bash
set -e

export NPP_CACHE="${NPP_CACHE:-FALSE}"

env | sort

if [[ -d "bin" ]]; then
	rm -Rf bin
fi

if [[ -d "artifacts" ]]; then
	rm -Rf artifacts
fi

if [[ -d "node_modules" ]]; then
	rm -Rf node_modules
fi

if [ -f "artifacts.json" ]; then
	rm artifacts.json
fi

if [ -f "package.json" ]; then

	echo [1/2] compile
	npm install

	echo [2/2] run tests
	npm test
else
	echo [1/2] compile
	./node_modules/cmaki_scripts/setup.sh && ./node_modules/cmaki_scripts/compile.sh

	echo [2/2] run tests
	./node_modules/cmaki_scripts/test.sh
fi

if [ -f "cmaki.yml" ]; then
	echo [3/3] upload artifact
	if [ -f "package.json" ]; then
		npm run upload
	else
		./node_modules/cmaki_scripts/upload.sh
	fi
fi
