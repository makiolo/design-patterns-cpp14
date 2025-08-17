#!//bin/bash

export MODE="${MODE:-Debug}"
export COMPILER="${COMPILER:-$(conan profile show default | grep -e "\<compiler\>=" | cut -d"=" -f2)}"
export COMPILER_LIBCXX="${COMPILER_LIBCXX:-$(conan profile show default | grep -e "\<compiler.libcxx\>=" | cut -d"=" -f2)}"
export COMPILER_VERSION="${COMPILER_VERSION:-$(conan profile show default | grep -e "\<compiler.version\>=" | cut -d"=" -f2)}"

if [ "$(uname)" == "Darwin" ]; then
	# mac
	export COMPILER=apple-clang COMPILER_VERSION=10.0 COMPILER_LIBCXX=libc++
fi

# compile 3rd parties
# conan install . --build missing -s compiler=$COMPILER -s build_type=$MODE -s compiler.libcxx=$COMPILER_LIBCXX -s compiler.version=$COMPILER_VERSION
# conan create . npm-mas-mas/testing --build $PACKAGE -s compiler=$COMPILER -s build_type=$MODE -s compiler.libcxx=$COMPILER_LIBCXX -s compiler.version=$COMPILER_VERSION -tf None
# conan upload $PACKAGE/*@npm-mas-mas/testing -r npm-mas-mas --all -c

# compile only $PACKAGE
conan create . npm-mas-mas/testing -s compiler=$COMPILER -s build_type=$MODE -s compiler.libcxx=$COMPILER_LIBCXX -s compiler.version=$COMPILER_VERSION -tf None
conan upload *@npm-mas-mas/testing -r npm-mas-mas --all -c

