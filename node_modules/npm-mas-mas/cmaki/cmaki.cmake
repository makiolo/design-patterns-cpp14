if(NOT DEFINED CMAKE_MODULE_PATH)
	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
endif()

IF(NOT DEFINED CMAKI_PATH)
	set(CMAKI_PATH ${CMAKE_CURRENT_LIST_DIR})
ENDIF()

include("${CMAKE_CURRENT_LIST_DIR}/facts/facts.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GitUtils.cmake")

option(FIRST_ERROR "stop on first compilation error" FALSE)

macro(cmaki_setup)
	enable_modern_cpp()
	enable_testing()
	SET(CMAKE_BUILD_TYPE_INIT Release)
	set(CMAKE_CXX_STANDARD 14)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS ON)
	IF(WITH_CONAN)
		# Conan
		message("-- Using conan dir: ${CMAKE_BINARY_DIR}")
		include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
		conan_basic_setup()
	ENDIF()
endmacro()

macro (mark_as_internal _var)
    set(${_var} ${${_var}} CACHE INTERNAL "hide this!" FORCE)
endmacro(mark_as_internal _var)

macro (option_combobox _var options default_option comment)
    set(${_var} "${default_option}" CACHE STRING "${comment}")
    set(${_var}Values "${options}" CACHE INTERNAL "hide this!" FORCE)
    set_property(CACHE ${_var} PROPERTY STRINGS ${${_var}Values})
endmacro()

function(cmaki_install_file FROM)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(FILES ${FROM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE} CONFIGURATIONS ${BUILD_TYPE})
    endforeach()
endfunction()

function(cmaki_install_file_into FROM TO)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(FILES ${FROM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}/${TO} CONFIGURATIONS ${BUILD_TYPE})
    endforeach()
endfunction()

function(cmaki_install_file_and_rename FROM NEWNAME)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(FILES ${FROM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE} CONFIGURATIONS ${BUILD_TYPE} RENAME ${NEWNAME})
    endforeach()
endfunction()

function(cmaki_install_file_into_and_rename FROM TO NEWNAME)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(FILES ${FROM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}/${TO} CONFIGURATIONS ${BUILD_TYPE} RENAME ${NEWNAME})
    endforeach()
endfunction()

function(cmaki_install_files FROM)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
        FILE(GLOB files ${FROM})
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE} CONFIGURATIONS ${BUILD_TYPE})
    endforeach()
endfunction()

function(cmaki_install_files_into FROM TO)
    foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
        FILE(GLOB files ${FROM})
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}/${TO} CONFIGURATIONS ${BUILD_TYPE})
    endforeach()
endfunction()

macro(cmaki_install_inside_dir _DESTINE)
	file(GLOB DEPLOY_FILES_AND_DIRS "${_DESTINE}/*")
	foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
		IF( IS_DIRECTORY "${ITEM}" )
			LIST( APPEND DIRS_TO_DEPLOY "${ITEM}" )
		ELSE()
			IF(ITEM STREQUAL "${_DESTINE}/CMakeLists.txt")
				MESSAGE("skipped file: ${_DESTINE}/CMakeLists.txt")
			ELSE()
				LIST(APPEND FILES_TO_DEPLOY "${ITEM}")
			ENDIF()
		ENDIF()
	endforeach()
	INSTALL(FILES ${FILES_TO_DEPLOY} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE})
	INSTALL(DIRECTORY ${DIRS_TO_DEPLOY} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE} USE_SOURCE_PERMISSIONS)
endmacro()

macro(cmaki_install_dir _DESTINE)
	INSTALL(DIRECTORY ${_DESTINE} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE} USE_SOURCE_PERMISSIONS)
endmacro()

macro(cmaki_parse_parameters)
	set(PARAMETERS ${ARGV})
	list(GET PARAMETERS 0 _MAIN_NAME)
	list(REMOVE_AT PARAMETERS 0)
	SET(HAVE_TESTS FALSE)
	SET(HAVE_PCH FALSE)
	SET(HAVE_PTHREADS FALSE)
	set(_DEPENDS)
	set(_SOURCES)
	set(_TESTS)
	set(_PCH)
	set(_INCLUDES)
	set(_SUFFIX_DESTINATION)
	set(NOW_IN SOURCES)
	while(PARAMETERS)
		list(GET PARAMETERS 0 PARM)
		if(PARM STREQUAL DEPENDS)
			set(NOW_IN DEPENDS)
		elseif(PARM STREQUAL SOURCES)
			set(NOW_IN SOURCES)
		elseif(PARM STREQUAL TESTS)
			set(NOW_IN TESTS)
		elseif(PARM STREQUAL PCH)
			set(NOW_IN PCH)
		elseif(PARM STREQUAL PTHREADS)
			if(NOT WIN32)
				# no enabled in windows
				set(HAVE_PTHREADS TRUE)
			endif()
		elseif(PARM STREQUAL INCLUDES)
			set(NOW_IN INCLUDES)
		elseif(PARM STREQUAL DESTINATION)
			set(NOW_IN DESTINATION)
		else()
			if(NOW_IN STREQUAL DEPENDS)
				set(_DEPENDS ${_DEPENDS} ${PARM})
			elseif(NOW_IN STREQUAL SOURCES)
				set(_SOURCES ${_SOURCES} ${PARM})
			elseif(NOW_IN STREQUAL TESTS)
				set(_TESTS ${_TESTS} ${PARM})
				SET(HAVE_TESTS TRUE)
			elseif(NOW_IN STREQUAL PCH)
				set(_PCH ${PARM})
				SET(HAVE_PCH TRUE)
			elseif(NOW_IN STREQUAL INCLUDES)
				set(_INCLUDES ${_INCLUDES} ${PARM})
			elseif(NOW_IN STREQUAL DESTINATION)
				set(_SUFFIX_DESTINATION ${PARM})
			else()
				message(FATAL_ERROR "Unknown argument ${PARM}.")
			endif()
		endif()
		list(REMOVE_AT PARAMETERS 0)
	endwhile()
endmacro()

function(cmaki_simple_executable)
	cmaki_parse_parameters(${ARGV})
	set(_EXECUTABLE_NAME ${_MAIN_NAME})
	MESSAGE("++ executable ${_EXECUTABLE_NAME}")
	source_group( "Source Files" FILES ${_SOURCES} )
	common_flags()
	common_linking(${_EXECUTABLE_NAME})
	foreach(INCLUDE_DIR ${_INCLUDES})
		target_include_directories(${_EXECUTABLE_NAME} ${INCLUDE_DIR})
	endforeach()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	if(WIN32)
		ADD_EXECUTABLE(${_EXECUTABLE_NAME} WIN32 ${_SOURCES})
	else()
		ADD_EXECUTABLE(${_EXECUTABLE_NAME} ${_SOURCES})
	endif()
	target_link_libraries(${_EXECUTABLE_NAME} ${_DEPENDS})
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			target_link_libraries(${_EXECUTABLE_NAME} -lpthread)
		endif()
	endif()
	foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(    TARGETS ${_EXECUTABLE_NAME}
					DESTINATION ${BUILD_TYPE}/${_SUFFIX_DESTINATION}
					CONFIGURATIONS ${BUILD_TYPE})
	endforeach()
	generate_clang()

endfunction()

function(cmaki_simple_library)
	cmaki_parse_parameters(${ARGV})
	set(_LIBRARY_NAME ${_MAIN_NAME})
	MESSAGE("++ library ${_LIBRARY_NAME}")
	source_group( "Source Files" FILES ${_SOURCES} )
	common_flags()
	common_linking(${_LIBRARY_NAME})
	foreach(INCLUDE_DIR ${_INCLUDES})
		target_include_directories(${_LIBRARY_NAME} ${INCLUDE_DIR})
	endforeach()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	add_library(${_LIBRARY_NAME} SHARED ${_SOURCES})
	target_link_libraries(${_LIBRARY_NAME} ${_DEPENDS})
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			target_link_libraries(${_LIBRARY_NAME} -lpthread)
		endif()
	endif()
	foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(	TARGETS ${_LIBRARY_NAME}
					DESTINATION ${BUILD_TYPE}/${_SUFFIX_DESTINATION}
					CONFIGURATIONS ${BUILD_TYPE})
	endforeach()
	generate_clang()

endfunction()

function(cmaki_simple_test)
	cmaki_parse_parameters(${ARGV})
	set(_TEST_NAME ${_MAIN_NAME})
	common_flags()
	common_linking(${_TEST_NAME})
	MESSAGE("++ test ${_TEST_NAME}")
	foreach(INCLUDE_DIR ${_INCLUDES})
		target_include_directories(${_TEST_NAME} ${INCLUDE_DIR})
	endforeach()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	add_executable(${_TEST_NAME} ${_SOURCES})
	target_link_libraries(${_TEST_NAME} ${_DEPENDS})
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			target_link_libraries(${_TEST_NAME} -lpthread)
		endif()
	endif()
	common_linking(${_TEST_NAME})
	foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(    TARGETS ${_TEST_NAME}
					DESTINATION ${BUILD_TYPE}/${_SUFFIX_DESTINATION}
					CONFIGURATIONS ${BUILD_TYPE})
		if(WIN32)
			add_test(
				NAME ${_TEST_NAME}__
				COMMAND ${_TEST_NAME}
				WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
				CONFIGURATIONS ${BUILD_TYPE}
				)
		else()

			if (DEFINED TESTS_VALGRIND AND (TESTS_VALGRIND STREQUAL "TRUE") AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
				find_program(VALGRIND "valgrind")
				if(VALGRIND)
					add_test(
						NAME ${_TEST_NAME}_memcheck
						COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes $<TARGET_FILE:${_TEST_NAME}>
						WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
						CONFIGURATIONS ${BUILD_TYPE}
						)
					add_test(
						NAME ${_TEST_NAME}_cachegrind
						COMMAND "${VALGRIND}" --tool=cachegrind $<TARGET_FILE:${_TEST_NAME}>
						WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
						CONFIGURATIONS ${BUILD_TYPE}
						)
					add_test(
						NAME ${_TEST_NAME}_helgrind
						COMMAND "${VALGRIND}" --tool=helgrind $<TARGET_FILE:${_TEST_NAME}>
						WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
						CONFIGURATIONS ${BUILD_TYPE}
						)
					add_test(
						NAME ${_TEST_NAME}_callgrind
						COMMAND "${VALGRIND}" --tool=callgrind $<TARGET_FILE:${_TEST_NAME}>
						WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
						CONFIGURATIONS ${BUILD_TYPE}
						)
					add_test(
						NAME ${_TEST_NAME}_drd
						COMMAND "${VALGRIND}" --tool=drd --read-var-info=yes $<TARGET_FILE:${_TEST_NAME}>
						WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
						CONFIGURATIONS ${BUILD_TYPE}
						)
				else()
					message(FATAL_ERROR "no valgrind detected")
				endif()
			else()
				add_test(
					NAME ${_TEST_NAME}_test						
					COMMAND bash cmaki_emulator.sh $<TARGET_FILE:${_TEST_NAME}>
					WORKING_DIRECTORY $ENV{CMAKI_INSTALL}
					CONFIGURATIONS ${BUILD_TYPE})
			endif()
		endif()
	endforeach()
	generate_vcxproj_user(${_TEST_NAME})
	generate_clang()

endfunction()

macro(common_linking)

	set(PARAMETERS ${ARGV})
	list(GET PARAMETERS 0 TARGET)
	# if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
	# 	target_link_libraries(${TARGET} -lubsan)
	# endif()

endmacro()

macro(common_flags)

	if(WIN32 AND (NOT MINGW) AND (NOT MSYS))
		add_definitions(/wd4251)
		add_definitions(/wd4275)
		add_definitions(/wd4239)
		add_definitions(/wd4316)
		add_definitions(/wd4127)
		add_definitions(/wd4245)
		add_definitions(/wd4458)
		add_definitions(/wd4146)
		add_definitions(/wd4244)
		add_definitions(/wd4189)
		add_definitions(/wd4100)
		add_definitions(/wd4706)
		add_definitions(/WX /W4)
		add_definitions(-Zm200)
	endif()

	if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
		set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
	endif()

endmacro()

macro(enable_modern_cpp)

	if(WIN32 AND (NOT MINGW) AND (NOT MSYS))
		add_definitions(/EHsc)
		add_definitions(/D_SCL_SECURE_NO_WARNINGS)
	else()
		# add_definitions(-fno-rtti -fno-exceptions )
		# activate all warnings and convert in errors
		# add_definitions(-Weffc++)
		# add_definitions(-pedantic -pedantic-errors)
		
		# Python: need disabling: initialization discards ‘const’ qualifier from pointer target type
		# add_definitions(-Werror)
		
		add_definitions(-Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion)
		add_definitions(-Wdisabled-optimization -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k)
		add_definitions(-Wimport  -Winit-self  -Winline -Winvalid-pch -Wlong-long -Wmissing-field-initializers -Wmissing-format-attribute)
		add_definitions(-Wpointer-arith -Wredundant-decls -Wshadow)
		add_definitions(-Wstack-protector -Wunreachable-code -Wunused)
		add_definitions(-Wunused-parameter -Wvariadic-macros -Wwrite-strings)
		add_definitions(-Wswitch-default -Wswitch-enum)
		# only gcc
		# convert error in warnings
		add_definitions(-Wno-error=shadow)
		add_definitions(-Wno-error=long-long)
		add_definitions(-Wno-error=aggregate-return)
		add_definitions(-Wno-error=unused-variable)
		add_definitions(-Wno-error=unused-parameter)
		add_definitions(-Wno-error=deprecated-declarations)
		add_definitions(-Wno-error=missing-include-dirs)
		add_definitions(-Wno-error=packed)
		add_definitions(-Wno-error=switch-default)
		add_definitions(-Wno-error=float-equal)
		add_definitions(-Wno-error=invalid-pch)
		add_definitions(-Wno-error=cast-qual)
		add_definitions(-Wno-error=conversion)
		add_definitions(-Wno-error=switch-enum)
		add_definitions(-Wno-error=redundant-decls)
		add_definitions(-Wno-error=stack-protector)
		add_definitions(-Wno-error=extra)
		add_definitions(-Wno-error=unused-result)
		add_definitions(-Wno-error=sign-compare)

		# raknet
		add_definitions(-Wno-error=address)
		add_definitions(-Wno-error=cast-qual)
		add_definitions(-Wno-error=missing-field-initializers)
		add_definitions(-Wno-error=write-strings)
		add_definitions(-Wno-error=format-nonliteral)

		# sdl2
		add_definitions(-Wno-error=sign-conversion)

		# TODO: remove
		add_definitions(-Wno-error=reorder)

		# if not have openmp
		add_definitions(-Wno-error=unknown-pragmas)

		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			add_definitions(-Wno-error=suggest-attribute=format)
			add_definitions(-Wno-error=suggest-attribute=noreturn)
			add_definitions(-Wno-aggregate-return)
			add_definitions(-Wno-long-long)
			add_definitions(-Wno-shadow)
			add_definitions(-Wno-strict-aliasing)
			add_definitions(-Wno-error=inline)
			add_definitions(-Wno-error=maybe-uninitialized)
			add_definitions(-Wno-error=unused-but-set-variable)
			add_definitions(-Wno-error=unused-local-typedefs)
			# add_definitions(-Wno-error=float-conversion)
		else()
			add_definitions(-Wstrict-aliasing=2)
			add_definitions(-Wno-error=format-nonliteral)
			add_definitions(-Wno-error=cast-align)
			add_definitions(-Wno-error=deprecated-register)
			add_definitions(-Wno-error=mismatched-tags)
			add_definitions(-Wno-error=overloaded-virtual)
			add_definitions(-Wno-error=unused-private-field)
			add_definitions(-Wno-error=unreachable-code)
			# add_definitions(-Wno-error=discarded-qualifiers)
		endif()

		# In Linux default now is not export symbols
		# add_definitions(-fvisibility=hidden)

		# stop in first error
		if(FIRST_ERROR)
			add_definitions(-Wfatal-errors)
		endif()

	endif()

	if (NOT DEFINED EXTRA_DEF)
		if(NOT WIN32 OR MINGW OR MSYS)
			include(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
			CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
			CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
			CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

			if(COMPILER_SUPPORTS_CXX14)
				set(CMAKE_CXX_STANDARD 14)
				message("-- C++14 Enabled")
			elseif(COMPILER_SUPPORTS_CXX11)
				set(CMAKE_CXX_STANDARD 11)
				message("-- C++11 Enabled")
			elseif(COMPILER_SUPPORTS_CXX0X)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
				message("-- C++0x Enabled")
			else()
				message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
			endif()
		endif()
	else()
		add_definitions(${EXTRA_DEF})
	endif()

	# TODO: need different combinations of artifacts (coverage=off / coverage=on, etc ...)
	# if ((DEFINED COVERAGE) AND (COVERAGE STREQUAL "TRUE"))
		# https://github.com/google/sanitizers/wiki/AddressSanitizerAsDso
		# flags
		if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-elide-constructors")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline")
		endif()

		# linker flags
		if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
			SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage")
		endif()
	# endif()

endmacro()

macro(generate_vcxproj_user _EXECUTABLE_NAME)
    IF(MSVC)
        set(project_vcxproj_user "${CMAKE_CURRENT_BINARY_DIR}/${_EXECUTABLE_NAME}.vcxproj.user")
        if (NOT EXISTS ${project_vcxproj_user})
            FILE(WRITE "${project_vcxproj_user}"
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<Project ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
            "<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>\n"
            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n"
            "</PropertyGroup>\n"
            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\">\n"
            "<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>\n"
            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n"
            "</PropertyGroup>\n"
            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\n"
            "<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>\n"
            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n"
            "</PropertyGroup>\n"
            "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\">\n"
            "<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>\n"
            "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n"
            "</PropertyGroup>\n"
            "</Project>\n")
        endif()
    ENDIF()
endmacro()

macro(generate_clang)
	# Generate .clang_complete for full completation in vim + clang_complete
	set(extra_parameters "")
	get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
	foreach(dir ${dirs})
	  set(extra_parameters ${extra_parameters} -I${dir})
	endforeach()
	get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
	foreach(dir ${dirs})
	  set(extra_parameters ${extra_parameters} -D${dir})
	endforeach()
	STRING(REGEX REPLACE ";" "\n" extra_parameters "${extra_parameters}")
	FILE(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete" "${extra_parameters}\n")
endmacro()
