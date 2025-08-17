if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

add_executable(cmaki_identifier cmaki_identifier.cpp)

install(TARGETS cmaki_identifier DESTINATION $ENV{CMAKI_INSTALL})
install(FILES ../cmaki_identifier.cmake DESTINATION $ENV{CMAKI_INSTALL})
install(PROGRAMS ../cmaki_identifier.sh DESTINATION $ENV{CMAKI_INSTALL})
install(PROGRAMS ../cmaki_emulator.sh DESTINATION $ENV{CMAKI_INSTALL})
add_test(
	NAME all
	COMMAND cmaki_identifier
	WORKING_DIRECTORY $ENV{CMAKI_INSTALL}
	)
add_test(
	NAME os
	COMMAND cmaki_identifier
	WORKING_DIRECTORY $ENV{CMAKI_INSTALL}
	)
add_test(
	NAME arch
	COMMAND cmaki_identifier
	WORKING_DIRECTORY $ENV{CMAKI_INSTALL}
	)
add_test(
	NAME compiler
	COMMAND cmaki_identifier
	WORKING_DIRECTORY $ENV{CMAKI_INSTALL}
	)
set_tests_properties(os PROPERTIES ENVIRONMENT "CMAKI_INFO=OS")
set_tests_properties(arch PROPERTIES ENVIRONMENT "CMAKI_INFO=ARCH")
set_tests_properties(compiler PROPERTIES ENVIRONMENT "CMAKI_INFO=COMPILER")
