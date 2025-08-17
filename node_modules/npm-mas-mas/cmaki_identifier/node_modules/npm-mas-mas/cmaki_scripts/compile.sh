#!/bin/bash
export NPP_CACHE="${NPP_CACHE:-FALSE}"
export CC="${CC:-gcc}"
export CXX="${CXX:-g++}"
export MODE="${MODE:-Debug}"
export CMAKI_TARGET="${CMAKI_TARGET:-install}"
export COMPILER_BASENAME=$(basename ${CC})

echo "running in mode $MODE ... ($COMPILER_BASENAME)"
cd $COMPILER_BASENAME/$MODE

# CORES=$(grep -c ^processor /proc/cpuinfo)
CORES=12
cmake --build . --config $MODE --target $CMAKI_TARGET -- -j$CORES -k VERBOSE=1 || cmake --build . --config $MODE --target $CMAKI_TARGET -- -j1 VERBOSE=1
code=$?
exit $code
