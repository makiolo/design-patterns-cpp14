#!/bin/bash

set -e

export MODE="${MODE:-Debug}"
export COMPILER="${COMPILER:-$(conan profile show default | grep -e "\<compiler\>=" | cut -d"=" -f2)}"
export COMPILER_LIBCXX="${COMPILER_LIBCXX:-$(conan profile show default | grep -e "\<compiler.libcxx\>=" | cut -d"=" -f2)}"
export COMPILER_VERSION="${COMPILER_VERSION:-$(conan profile show default | grep -e "\<compiler.version\>=" | cut -d"=" -f2)}"

if [ "$NPP_CI" == "FALSE" ]; then
	conan install . --build missing -s compiler=$COMPILER -s build_type=$MODE -s compiler.libcxx=$COMPILER_LIBCXX -s compiler.version=$COMPILER_VERSION
fi

conan create . npm-mas-mas/testing -s compiler=$COMPILER -s build_type=$MODE -s compiler.libcxx=$COMPILER_LIBCXX -s compiler.version=$COMPILER_VERSION -tf None
conan upload '*' -r npm-mas-mas --all -c

