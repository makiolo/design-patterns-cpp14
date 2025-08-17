#!/bin/bash
set -e
MODE=${1}
echo running in mode ${MODE} ...
mkdir -p build/${MODE}
pushd build/${MODE}
cmake ../.. -DCMAKE_BUILD_TYPE=$MODE -DCMAKE_MODULE_PATH=$(pwd)/../../cmaki -DFIRST_ERROR=1
cmake --build . --config $MODE --target install -- -j8 -k || cmake --build . --config ${MODE} --target install -- -j1
ctest . --no-compress-output --output-on-failure -T Test -C ${MODE} -V
popd
