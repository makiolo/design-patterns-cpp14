#!/bin/bash

export MODE="${MODE:-Debug}"
export CMAKI_INSTALL="${CMAKI_INSTALL:-$CMAKI_PWD/bin}"
export PACKAGE="${PACKAGE:-undefined}"
export YMLFILE="${YMLFILE:-undefined}"

if [ "$YMLFILE" == "undefined" ]; then
	if [ "$PACKAGE" == "undefined" ]; then
		echo Error: must define env var YMLFILE or PACKAGE
	else
		echo building $PACKAGE ...
		./build $PACKAGE --no-back-yaml --no-run-tests -d
	fi
else
	echo building from yaml file: ${YMLFILE} ...
	./build --yaml=${YMLFILE} --no-run-tests -d
fi
