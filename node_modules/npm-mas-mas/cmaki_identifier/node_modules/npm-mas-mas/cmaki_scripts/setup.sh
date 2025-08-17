#!/bin/bash

export CC="${CC:-gcc}"
export CXX="${CXX:-g++}"
export MODE="${MODE:-Debug}"
export COMPILER="${COMPILER:-$(conan profile show | grep -e "\<compiler\>=" | cut -d"=" -f2 | tail -n1)}"
export COMPILER_LIBCXX="${COMPILER_LIBCXX:-$(conan profile show | grep -e "\<compiler.libcxx\>=" | cut -d"=" -f2 | tail -n1)}"
export COMPILER_VERSION="${COMPILER_VERSION:-$(conan profile show | grep -e "\<compiler.version\>=" | cut -d"=" -f2 | tail -n1)}"
export CMAKI_INSTALL="${CMAKI_INSTALL:-$CMAKI_PWD/bin}"
export NPP_CACHE="${NPP_CACHE:-FALSE}"
export CMAKI_GENERATOR="${CMAKI_GENERATOR:-Unix Makefiles}"
export COVERAGE="${COVERAGE:-FALSE}"
export TESTS_VALGRIND="${TESTS_VALGRIND:-FALSE}"
export COMPILER_BASENAME=$(basename ${CC})
export CMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE:-"no cross compile"}"
export BUILD_DIR="${BUILD_DIR:-${COMPILER_BASENAME}/${MODE}}"

if [ "$CMAKE_TOOLCHAIN_FILE" == "no cross compile" ]; then
	export CMAKE_TOOLCHAIN_FILE_FILEPATH=""
else
	export CMAKE_TOOLCHAIN_FILE_FILEPATH=" -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
fi

echo "running in mode ${MODE} ... ($COMPILER_BASENAME) (${CC} / ${CXX})"

# setup
if [ ! -d ${BUILD_DIR} ]; then
	mkdir -p ${BUILD_DIR}
fi
echo BUILD_DIR=${BUILD_DIR}
cd ${BUILD_DIR}

if [ -f "CMakeCache.txt" ]; then
	rm CMakeCache.txt
fi

export WITH_CONAN=0
if [ -f "$CMAKI_PWD/conanfile.txt" ] || [ -f "$CMAKI_PWD/conanfile.py" ]; then

	if [ "$NPP_CI" == "FALSE" ]; then
		conan install $CMAKI_PWD --build missing -s compiler=${COMPILER} -s build_type=${MODE} -s compiler.libcxx=${COMPILER_LIBCXX} -s compiler.version=${COMPILER_VERSION}
	fi

	echo conan install $CMAKI_PWD --build never -s compiler=${COMPILER} -s build_type=${MODE} -s compiler.libcxx=${COMPILER_LIBCXX} -s compiler.version=${COMPILER_VERSION}
	if ! conan install $CMAKI_PWD --build never -s compiler=${COMPILER} -s build_type=${MODE} -s compiler.libcxx=${COMPILER_LIBCXX} -s compiler.version=${COMPILER_VERSION}; then
		echo Error conan
		exit 1
	fi
	export WITH_CONAN=1
fi

cmake $CMAKI_PWD ${CMAKE_TOOLCHAIN_FILE_FILEPATH} -DCMAKE_MODULE_PATH=${CMAKI_PWD}/node_modules/npm-mas-mas/cmaki -DCMAKE_INSTALL_PREFIX=${CMAKI_INSTALL} -DCMAKE_BUILD_TYPE=${MODE} -DFIRST_ERROR=1 -G"${CMAKI_GENERATOR}" -DCMAKE_C_COMPILER="${CC}" -DCMAKE_CXX_COMPILER="${CXX}" -DNPP_CACHE=${NPP_CACHE} -DCOVERAGE=${COVERAGE} -DTESTS_VALGRIND=${TESTS_VALGRIND} -DWITH_CONAN=${WITH_CONAN}
code=$?
exit ${code}
