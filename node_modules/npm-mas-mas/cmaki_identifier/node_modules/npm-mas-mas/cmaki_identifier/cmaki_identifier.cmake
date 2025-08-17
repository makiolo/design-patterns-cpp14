set(PLATFORM "")
set(dirscript ${CMAKE_CURRENT_LIST_DIR})
IF(WIN32)
	set(executable cmaki_identifier.exe)
else()
	set(executable cmaki_identifier.sh)
endif()
execute_process(COMMAND ${dirscript}/${executable}
			OUTPUT_VARIABLE PLATFORM
			OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE("${PLATFORM}")

