#!/bin/bash

export CC="${CC:-gcc}"
export CXX="${CXX:-g++}"
export MODE="${MODE:-Debug}"
export CMAKI_INSTALL="${CMAKI_INSTALL:-$CMAKI_PWD/bin}"
export CMAKI_EMULATOR="${CMAKI_EMULATOR:-}"

if [[ "$WINEARCH" = "win32" ]]; then
	wine $CMAKI_INSTALL/cmaki_identifier.exe
else
	$CMAKI_EMULATOR $CMAKI_INSTALL/cmaki_identifier
fi

