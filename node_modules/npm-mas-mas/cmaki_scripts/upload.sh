#!/bin/bash -e

export CC="${CC:-gcc}"
export CXX="${CXX:-g++}"
export MODE="${MODE:-Debug}"
export CMAKI_INSTALL="${CMAKI_INSTALL:-$CMAKI_PWD/bin}"
export YMLFILE=$CMAKI_PWD/cmaki.yml

git diff $CMAKI_PWD
cd $CMAKI_PWD/node_modules/npm-mas-mas/cmaki_generator
../cmaki_scripts/make_artifact.sh

