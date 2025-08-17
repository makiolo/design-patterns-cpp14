cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0045 OLD)

find_program(PYTHON_EXECUTABLE NAMES python3.6 python3.5 python3 python)

IF(NOT DEFINED CMAKI_PWD)
	set(CMAKI_PWD $ENV{CMAKI_PWD})
ENDIF()

IF(NOT DEFINED CMAKI_INSTALL)
	set(CMAKI_INSTALL $ENV{CMAKI_INSTALL})
ENDIF()

IF(NOT DEFINED NPP_ARTIFACTS_PATH)
	set(NPP_ARTIFACTS_PATH ${CMAKI_PWD}/artifacts)
ENDIF()

IF(NOT DEFINED CMAKE_PREFIX_PATH)
	set(CMAKE_PREFIX_PATH ${NPP_ARTIFACTS_PATH}/cmaki_find_package)
ENDIF()

IF(NOT DEFINED NPP_GENERATOR_PATH)
	set(NPP_GENERATOR_PATH ${CMAKI_PATH}/../cmaki_generator)
ENDIF()

IF(NOT DEFINED NPP_PACKAGE_JSON_FILE)
	set(NPP_PACKAGE_JSON_FILE ${CMAKI_PATH}/../../artifacts.json)
ENDIF()

if(NOT DEFINED CMAKI_IDENTIFIER OR NOT DEFINED CMAKI_PLATFORM)
	set(ENV{CMAKI_INFO} ALL)
	include(${CMAKI_PWD}/bin/cmaki_identifier.cmake)
	set(CMAKI_IDENTIFIER "${PLATFORM}")
	set(CMAKI_PLATFORM "${PLATFORM}")
endif()

MESSAGE("CMAKI_PWD = ${CMAKI_PWD}")
MESSAGE("CMAKI_INSTALL = ${CMAKI_INSTALL}")
MESSAGE("CMAKI_PATH = ${CMAKI_PATH}")
MESSAGE("NPP_ARTIFACTS_PATH = ${NPP_ARTIFACTS_PATH}")
MESSAGE("NPP_GENERATOR_PATH = ${NPP_GENERATOR_PATH}")
MESSAGE("NPP_PACKAGE_JSON_FILE = ${NPP_PACKAGE_JSON_FILE}")
MESSAGE("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
MESSAGE("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
MESSAGE("CMAKI_IDENTIFIER = ${CMAKI_IDENTIFIER}")
MESSAGE("CMAKI_PLATFORM = ${CMAKI_PLATFORM}")

function(cmaki_find_package)

	message("-- begin cmaki_find_package")

	set(PARAMETERS ${ARGV})
	list(LENGTH PARAMETERS ARGV_LENGTH)
	list(GET PARAMETERS 0 PACKAGE)
	set(VERSION_REQUEST "")
	set(CALL_RECURSIVE "TRUE")
	set(PARM1 "")
	if(ARGV_LENGTH GREATER 1)
		list(GET PARAMETERS 1 PARM1)
		message("-- extra parm1: ${PARM1}")
		if(PARM1 STREQUAL "NONRECURSIVE")
			message("${PACKAGE} is not recursive")
			set(CALL_RECURSIVE "FALSE")
		else()
			message("${PACKAGE} is recursive")
			set(VERSION_REQUEST "${PARM1}")
		endif()
	endif()

	IF(NOT DEFINED CMAKI_REPOSITORY)
		set(CMAKI_REPOSITORY "$ENV{NPP_SERVER}")
	ENDIF()

	# 2.5. define flags
	set(FORCE_GENERATION NOT "$ENV{NPP_CACHE}")

	if(VERSION_REQUEST STREQUAL "")
		##
		message("COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/get_package.py --name=${PACKAGE} --depends=${NPP_PACKAGE_JSON_FILE}")
		##
		# 1. obtener la version actual (o ninguno en caso de no tener el artefacto)
		execute_process(
			COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/get_package.py --name=${PACKAGE} --depends=${NPP_PACKAGE_JSON_FILE}
			WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
			OUTPUT_VARIABLE RESULT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(RESULT_VERSION)
			set(VERSION_REQUEST "${RESULT_VERSION}")
			set(EXTRA_VERSION "--version=${VERSION_REQUEST}")
		else()
			set(VERSION_REQUEST "")
			set(EXTRA_VERSION "")
		endif()

	else()
		# explicit version required from parameters
		set(EXTRA_VERSION "--version=${VERSION_REQUEST}")
	endif()

	message("${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/check_remote_version.py --server=${CMAKI_REPOSITORY} --artifacts=${CMAKE_PREFIX_PATH} --platform=${CMAKI_IDENTIFIER} --name=${PACKAGE} ${EXTRA_VERSION}")
	#######################################################
	# 2. obtener la mejor version buscando en la cache local y remota
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/check_remote_version.py --server=${CMAKI_REPOSITORY} --artifacts=${CMAKE_PREFIX_PATH} --platform=${CMAKI_IDENTIFIER} --name=${PACKAGE} ${EXTRA_VERSION}
		WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
		OUTPUT_VARIABLE RESULT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(RESULT_VERSION)
		list(GET RESULT_VERSION 0 PACKAGE_MODE)
		list(GET RESULT_VERSION 1 PACKAGE_NAME)
		list(GET RESULT_VERSION 2 VERSION)
		message("now PACKAGE_MODE = ${PACKAGE_MODE}")
		message("now PACKAGE_NAME = ${PACKAGE_NAME}")
		message("now VERSION = ${VERSION}")
		if(PACKAGE_MODE STREQUAL "UNSUITABLE")
			set(PACKAGE_MODE "EXACT")
			set(VERSION ${VERSION_REQUEST})
			message("-- need build package ${PACKAGE} can't get version: ${VERSION_REQUEST}, will be generated... (error 1)")
			# avoid remote cache, need build
			set(FORCE_GENERATION "TRUE")
		endif()
	else()
		set(PACKAGE_MODE "EXACT")
		set(VERSION ${VERSION_REQUEST})
		message("-- need build package ${PACKAGE} can't get version: ${VERSION_REQUEST}, will be generated... (error 2)")
		# avoid remote cache, need build
		set(FORCE_GENERATION "TRUE")
	endif()
	#######################################################

	# cmaki_find_package of depends
	message("COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/build.py ${PACKAGE} --rootdir=${NPP_GENERATOR_PATH} --depends=${NPP_PACKAGE_JSON_FILE} --cmakefiles=${CMAKI_PATH} --prefix=${NPP_ARTIFACTS_PATH} --third-party-dir=${CMAKE_PREFIX_PATH} --server=${CMAKI_REPOSITORY} --plan --quiet")
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/build.py ${PACKAGE} --rootdir=${NPP_GENERATOR_PATH} --depends=${NPP_PACKAGE_JSON_FILE} --cmakefiles=${CMAKI_PATH} --prefix=${NPP_ARTIFACTS_PATH} --third-party-dir=${CMAKE_PREFIX_PATH} --server=${CMAKI_REPOSITORY} --plan --quiet
		WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
		OUTPUT_VARIABLE DEPENDS_PACKAGES
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if("${CALL_RECURSIVE}")
		foreach(DEP ${DEPENDS_PACKAGES})
			if(PACKAGE STREQUAL "${DEP}")
				message("-- skip: ${DEP}")
			else()
				message("-- cmaki_find_package: ${DEP}")
				cmaki_find_package("${DEP}" NONRECURSIVE)
			endif()
		endforeach()
	endif()

	get_filename_component(package_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
	get_filename_component(package_name_version "${package_dir}" NAME)

	# 3. si no tengo los ficheros de cmake, los intento descargar
	set(artifacts_dir "${NPP_ARTIFACTS_PATH}")
	set(depends_bin_package "${artifacts_dir}/${PACKAGE}-${VERSION}")
	set(depends_package "${artifacts_dir}/${PACKAGE}-${VERSION}")
	# pido un paquete, en funcion de:
	#		- paquete
	#		- version
	#		- plataforma
	#		- modo (COMPATIBLE / EXACT)
	# Recibo el que mejor se adapta a mis especificaciones
	# Otra opcion es enviar todos los ficheros de cmake de todas las versiones

	set(package_cmake_filename "${PACKAGE}-${VERSION}-${CMAKI_IDENTIFIER}-cmake.tar.gz")
	set(package_marker "${CMAKE_PREFIX_PATH}/${package_name_version}/${CMAKI_IDENTIFIER}.cmake")
	set(package_cmake_abspath "${artifacts_dir}/${package_cmake_filename}")
	set(package_generated_file ${artifacts_dir}/${package_filename})

	set(COPY_SUCCESFUL FALSE)
	IF(EXISTS "${package_cmake_abspath}")
		message("-- reusing cmake file ${package_cmake_abspath}")
		set(COPY_SUCCESFUL TRUE)
	else()
		if(NOT "${FORCE_GENERATION}")
			set(http_package_cmake_filename "${CMAKI_REPOSITORY}/download.php?file=${package_cmake_filename}")
			message("-- download file: ${http_package_cmake_filename} in ${package_cmake_abspath}")
			cmaki_download_file("${http_package_cmake_filename}" "${package_cmake_abspath}")
			if(NOT "${COPY_SUCCESFUL}")
				file(REMOVE "${package_binary_filename}")
				message("Error downloading ${http_package_cmake_filename}")
			endif()
		else()
			message("WARN: no using cache remote for: ${PACKAGE}")
		endif()
	endif()

	if(NOT "${COPY_SUCCESFUL}")
		message("fail download")
	else()
		message("reused or downloaded")
	endif()

	# si la descarga no ha ido bien O no quieres utilizar cache
	if(NOT "${COPY_SUCCESFUL}" OR FORCE_GENERATION STREQUAL "TRUE")

		# 5. compilo y genera el paquete en local
		message("Generating artifact ${PACKAGE} ...")

		###
		message("${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/build.py ${PACKAGE} --rootdir=${NPP_GENERATOR_PATH} --depends=${NPP_PACKAGE_JSON_FILE} --cmakefiles=${CMAKI_PATH} --prefix=${NPP_ARTIFACTS_PATH} --third-party-dir=${CMAKE_PREFIX_PATH} --server=${CMAKI_REPOSITORY} -o")
		###
		execute_process(
			COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/build.py ${PACKAGE} --rootdir=${NPP_GENERATOR_PATH} --depends=${NPP_PACKAGE_JSON_FILE} --cmakefiles=${CMAKI_PATH} --prefix=${NPP_ARTIFACTS_PATH} --third-party-dir=${CMAKE_PREFIX_PATH} --server=${CMAKI_REPOSITORY} -o
			WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
			RESULT_VARIABLE artifacts_result
			)
		if(artifacts_result)
			message(FATAL_ERROR "can't create artifact ${PACKAGE}: error ${artifacts_result}")
		endif()

		#######################################################
		# 6: obtengo la version del paquete creado
		execute_process(
			COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/check_remote_version.py --server=${CMAKI_REPOSITORY} --artifacts=${CMAKE_PREFIX_PATH} --platform=${CMAKI_IDENTIFIER} --name=${PACKAGE}
			WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
			OUTPUT_VARIABLE RESULT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(RESULT_VERSION)
			list(GET RESULT_VERSION 0 PACKAGE_MODE)
			list(GET RESULT_VERSION 1 PACKAGE_NAME)
			list(GET RESULT_VERSION 2 VERSION)
			message("NEW! PACKAGE_MODE = ${PACKAGE_MODE}")
			message("NEW! PACKAGE_NAME = ${PACKAGE_NAME}")
			message("NEW! VERSION = ${VERSION}")
		else()
			message(FATAL_ERROR "-- not found ${PACKAGE}.")
		endif()
		#######################################################

		set(package_filename ${PACKAGE}-${VERSION}-${CMAKI_IDENTIFIER}.tar.gz)
		set(package_cmake_filename ${PACKAGE}-${VERSION}-${CMAKI_IDENTIFIER}-cmake.tar.gz)
		# refresh name (NEW $VERSION is generated)
		set(package_cmake_abspath "${artifacts_dir}/${package_cmake_filename}")

		# 7. descomprimo el artefacto
		execute_process(
			COMMAND "${CMAKE_COMMAND}" -E tar zxf "${package_cmake_abspath}"
			WORKING_DIRECTORY "${CMAKE_PREFIX_PATH}"
			RESULT_VARIABLE uncompress_result
			)
		if(uncompress_result)
			message(FATAL_ERROR "Extracting ${package_cmake_abspath} failed! Error ${uncompress_result}")
		endif()

		# y tambien descomprimo el propio tar gz
		# execute_process(
		# 	COMMAND "${CMAKE_COMMAND}" -E tar zxf "${package_generated_file}"
		# 	WORKING_DIRECTORY "${artifacts_dir}/"
		# 	RESULT_VARIABLE uncompress_result2
		# 	)
		# if(uncompress_result2)
		# 	message(FATAL_ERROR "Extracting ${package_generated_file} failed! Error ${uncompress_result2}")
		# endif()

	# tengo el cmake pero no esta descomprimido
	elseif(EXISTS "${package_cmake_abspath}" AND NOT EXISTS "${package_marker}")

		message("-- only uncompress")
		################
		message("${CMAKE_COMMAND} -E tar zxf ${package_cmake_abspath}")
		################

		# 10. lo descomprimo
		execute_process(
			COMMAND "${CMAKE_COMMAND}" -E tar zxf "${package_cmake_abspath}"
			WORKING_DIRECTORY "${CMAKE_PREFIX_PATH}/"
			RESULT_VARIABLE uncompress_result)
		if(uncompress_result)
			message(FATAL_ERROR "Extracting ${package_cmake_abspath} failed! Error ${uncompress_result}")
		endif()

	else()

		# tengo cmake, y esta descomprmido
		message("-- nothing to do")
		message("-- ${package_cmake_abspath}")
		message("-- ${package_marker}")

	endif()


	# 12. hacer find_package tradicional, ahora que tenemos los ficheros de cmake
	if(${PACKAGE_MODE} STREQUAL "EXACT")
		message("-- using ${PACKAGE} ${VERSION} in EXACT")
		find_package(${PACKAGE} ${VERSION} EXACT REQUIRED)
	else()
		message("-- using ${PACKAGE} ${VERSION} in COMPATIBLE")
		find_package(${PACKAGE} ${VERSION} REQUIRED)
	endif()

	# generate json
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/save_package.py --name=${PACKAGE} --depends=${NPP_PACKAGE_JSON_FILE} --version=${VERSION}
		WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
		OUTPUT_VARIABLE RESULT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(RESULT_VERSION)
		message("error saving ${PACKAGE}:${VERSION} in ${artifacts_dir}")
	endif()

	# 13 add includes
	string(TOUPPER "${PACKAGE}" PACKAGE_UPPER)
	foreach(INCLUDE_DIR ${${PACKAGE_UPPER}_INCLUDE_DIRS})
		list(APPEND CMAKI_INCLUDE_DIRS "${INCLUDE_DIR}")
	endforeach()

	# 14. add libdirs
	foreach(LIB_DIR ${${PACKAGE_UPPER}_LIBRARIES})
		list(APPEND CMAKI_LIBRARIES "${LIB_DIR}")
	endforeach()

	# 15. add vers specific
	set(${PACKAGE_UPPER}_INCLUDE_DIRS "${${PACKAGE_UPPER}_INCLUDE_DIRS}" PARENT_SCOPE)
	set(${PACKAGE_UPPER}_LIBRARIES "${${PACKAGE_UPPER}_LIBRARIES}" PARENT_SCOPE)

	# 16. add vars globals
	set(CMAKI_INCLUDE_DIRS "${CMAKI_INCLUDE_DIRS}" PARENT_SCOPE)
	set(CMAKI_LIBRARIES "${CMAKI_LIBRARIES}" PARENT_SCOPE)

	message("-- end cmaki_find_package")

endfunction()

macro(cmaki_package_version_check)
	# llamar a check_remote_version
	# dando el nombre recibo la version
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} ${NPP_GENERATOR_PATH}/check_remote_version.py --artifacts=${CMAKE_PREFIX_PATH} --platform=${CMAKI_IDENTIFIER} --name=${PACKAGE_FIND_NAME} --version=${PACKAGE_FIND_VERSION}
		WORKING_DIRECTORY "${NPP_GENERATOR_PATH}"
		OUTPUT_VARIABLE RESULT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	list(GET RESULT_VERSION 0 RESULT)
	list(GET RESULT_VERSION 1 NAME)
	list(GET RESULT_VERSION 2 VERSION)
	###################################
	set(PACKAGE_VERSION_${RESULT} 1)
	set(${NAME}_VERSION ${VERSION})
endmacro()

function(cmaki_install_3rdparty)
	foreach(CMAKI_3RDPARTY_TARGET ${ARGV})
		foreach(CMAKI_BUILD_TYPE ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
			string(TOUPPER "${CMAKI_BUILD_TYPE}" CMAKI_BUILD_TYPE_UPPER)
			get_target_property(CMAKI_3RDPARTY_TARGET_TYPE ${CMAKI_3RDPARTY_TARGET} TYPE)
			if(${CMAKI_3RDPARTY_TARGET_TYPE} STREQUAL "SHARED_LIBRARY")
				get_target_property(CMAKI_3RDPARTY_TARGET_LOCATION ${CMAKI_3RDPARTY_TARGET} IMPORTED_LOCATION_${CMAKI_BUILD_TYPE_UPPER})
				get_target_property(CMAKI_3RDPARTY_TARGET_SONAME ${CMAKI_3RDPARTY_TARGET} IMPORTED_SONAME_${CMAKI_BUILD_TYPE_UPPER})
				get_target_property(CMAKI_3RDPARTY_TARGET_PDB ${CMAKI_3RDPARTY_TARGET} IMPORTED_PDB_${CMAKI_BUILD_TYPE_UPPER})
				if(CMAKI_3RDPARTY_TARGET_SONAME)
					get_filename_component(CMAKI_3RDPARTY_TARGET_LOCATION_PATH "${CMAKI_3RDPARTY_TARGET_LOCATION}" PATH)
					set(CMAKI_3RDPARTY_TARGET_LOCATION "${CMAKI_3RDPARTY_TARGET_LOCATION_PATH}/${CMAKI_3RDPARTY_TARGET_SONAME}")
				endif()
				get_filename_component(CMAKI_3RDPARTY_TARGET_INSTALLED_NAME "${CMAKI_3RDPARTY_TARGET_LOCATION}" NAME)
				get_filename_component(CMAKI_3RDPARTY_TARGET_LOCATION "${CMAKI_3RDPARTY_TARGET_LOCATION}" REALPATH)
				install(PROGRAMS ${CMAKI_3RDPARTY_TARGET_LOCATION}
					DESTINATION ${CMAKI_BUILD_TYPE}
					CONFIGURATIONS ${CMAKI_BUILD_TYPE}
					RENAME ${CMAKI_3RDPARTY_TARGET_INSTALLED_NAME})
				if((NOT UNIX) AND EXISTS ${CMAKI_3RDPARTY_TARGET_PDB})
					get_filename_component(CMAKI_3RDPARTY_TARGET_PDB_NAME "${CMAKI_3RDPARTY_TARGET_PDB}" NAME)
					install(PROGRAMS ${CMAKI_3RDPARTY_TARGET_PDB}
						DESTINATION ${CMAKI_BUILD_TYPE}
						CONFIGURATIONS ${CMAKI_BUILD_TYPE}
						RENAME ${CMAKI_3RDPARTY_TARGET_PDB_NAME})
				endif()
			endif()
		endforeach()
	endforeach()
endfunction()

function(cmaki_download_file THE_URL INTO_FILE)
	set(COPY_SUCCESFUL FALSE PARENT_SCOPE)
	file(DOWNLOAD ${THE_URL} ${INTO_FILE} STATUS RET)
	list(GET RET 0 RET_CODE)
	if(RET_CODE EQUAL 0)
		set(COPY_SUCCESFUL TRUE PARENT_SCOPE)
	else()
		set(COPY_SUCCESFUL FALSE PARENT_SCOPE)
	endif()
endfunction()

macro(cmaki_download_package)

	message("-- begin cmaki_download_package")
	if(NOT DEFINED CMAKI_REPOSITORY)
		set(CMAKI_REPOSITORY "$ENV{NPP_SERVER}")
	endif()
	get_filename_component(package_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
	get_filename_component(package_name_version "${package_dir}" NAME)
	set(package_filename "${package_name_version}-${CMAKI_IDENTIFIER}.tar.gz")
	set(http_package_filename ${CMAKI_REPOSITORY}/download.php?file=${package_filename})
	set(artifacts_dir "${NPP_ARTIFACTS_PATH}")
	get_filename_component(artifacts_dir "${artifacts_dir}" ABSOLUTE)
	set(package_binary_filename "${artifacts_dir}/${PACKAGE}-${VERSION}-${CMAKI_IDENTIFIER}.tar.gz")
	set(package_uncompressed_dir "${artifacts_dir}/${package_name_version}-binary.tmp")
	set(package_marker "${artifacts_dir}/${package_name_version}/${CMAKI_IDENTIFIER}")
	set(package_compressed_md5 "${package_dir}/${package_name_version}-${CMAKI_IDENTIFIER}.md5")
	set(_MY_DIR "${package_dir}")
	set(_DIR "${artifacts_dir}/${package_name_version}")

	if(NOT EXISTS "${package_binary_filename}")
		message("download ${package_binary_filename} ...")
		if(EXISTS "${package_compressed_md5}")
			file(READ "${package_compressed_md5}" md5sum )
			string(REGEX MATCH "[0-9a-fA-F]*" md5sum "${md5sum}")
			# TODO: use md5sum (use python for download)
			# cmaki_download_file("${http_package_filename}" "${package_binary_filename}" "${md5sum}" )
			message("downloading ${http_package_filename}")
			cmaki_download_file("${http_package_filename}" "${package_binary_filename}")
			if(NOT "${COPY_SUCCESFUL}")
				file(REMOVE "${package_binary_filename}")
				message(FATAL_ERROR "Error downloading ${http_package_filename}")
			endif()
		else()
			file(REMOVE_RECURSE "${package_dir}")
			file(REMOVE_RECURSE "${_DIR}")
			MESSAGE(FATAL_ERROR "Checksum for ${package_name_version}-${CMAKI_IDENTIFIER}.tar.gz not found. Rejecting to download an untrustworthy file.")
		endif()
	endif()

	if(NOT EXISTS "${package_marker}")
		message("Extracting ${package_binary_filename} into ${package_uncompressed_dir}...")
		file(MAKE_DIRECTORY "${package_uncompressed_dir}")
		execute_process(
			COMMAND "${CMAKE_COMMAND}" -E tar zxf "${package_binary_filename}"
			WORKING_DIRECTORY "${package_uncompressed_dir}"
			RESULT_VARIABLE uncompress_result)
		if(uncompress_result)
			message(FATAL_ERROR "Extracting ${package_binary_filename} failed! Error ${uncompress_result}")
		endif()
		file(COPY "${package_uncompressed_dir}/${package_name_version}" DESTINATION "${artifacts_dir}")
		file(REMOVE_RECURSE "${package_uncompressed_dir}")
	endif()
	message("-- end cmaki_download_package")

endmacro()

function(cmaki_executable)
	cmaki_parse_parameters(${ARGV})
	set(_EXECUTABLE_NAME ${_MAIN_NAME})
	source_group( "Source Files" FILES ${_SOURCES} )
	common_flags()
	common_linking(${_EXECUTABLE_NAME})
	include_directories(node_modules)
	foreach(INCLUDE_DIR ${CMAKI_INCLUDE_DIRS})
		include_directories(${INCLUDE_DIR})
	endforeach()
	IF(WITH_CONAN)
		include_directories(${CONAN_INCLUDE_DIRS})
	ENDIF()
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
	# set_target_properties(${_EXECUTABLE_NAME} PROPERTIES DEBUG_POSTFIX _d)
	target_link_libraries(${_EXECUTABLE_NAME} ${_DEPENDS})
	foreach(LIB_DIR ${CMAKI_LIBRARIES})
		target_link_libraries(${_EXECUTABLE_NAME} ${LIB_DIR})
		cmaki_install_3rdparty(${LIB_DIR})
	endforeach()
	IF(WITH_CONAN)
		target_link_libraries(${_EXECUTABLE_NAME} ${CONAN_LIBS})
		cmaki_install_3rdparty(${CONAN_LIBS})
	ENDIF()
	install(DIRECTORY ${CONAN_LIB_DIRS}/ DESTINATION ${CMAKE_BUILD_TYPE})
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
	generate_vcxproj_user(${_EXECUTABLE_NAME})

endfunction()

function(cmaki_library)
	cmaki_parse_parameters(${ARGV})
	set(_LIBRARY_NAME ${_MAIN_NAME})
	source_group( "Source Files" FILES ${_SOURCES} )
	common_flags()
	common_linking(${_LIBRARY_NAME})
	include_directories(node_modules)
	foreach(INCLUDE_DIR ${CMAKI_INCLUDE_DIRS})
		include_directories(${INCLUDE_DIR})
	endforeach()
	IF(WITH_CONAN)
		include_directories(${CONAN_INCLUDE_DIRS})
	ENDIF()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	add_library(${_LIBRARY_NAME} SHARED ${_SOURCES})
	# set_target_properties(${_LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX _d)
	target_link_libraries(${_LIBRARY_NAME} ${_DEPENDS})
	foreach(LIB_DIR ${CMAKI_LIBRARIES})
		target_link_libraries(${_LIBRARY_NAME} ${LIB_DIR})
		cmaki_install_3rdparty(${LIB_DIR})
	endforeach()
	IF(WITH_CONAN)
		target_link_libraries(${_LIBRARY_NAME} ${CONAN_LIBS})
		cmaki_install_3rdparty(${CONAN_LIBS})
	ENDIF()
	install(DIRECTORY ${CONAN_LIB_DIRS}/ DESTINATION ${CMAKE_BUILD_TYPE})
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
endfunction()

function(cmaki_static_library)
	cmaki_parse_parameters(${ARGV})
	set(_LIBRARY_NAME ${_MAIN_NAME})
	source_group( "Source Files" FILES ${_SOURCES} )
	common_flags()
	common_linking(${_LIBRARY_NAME})
	add_definitions(-D${_LIBRARY_NAME}_STATIC)
	include_directories(node_modules)
	foreach(INCLUDE_DIR ${CMAKI_INCLUDE_DIRS})
		include_directories(${INCLUDE_DIR})
	endforeach()
	IF(WITH_CONAN)
		include_directories(${CONAN_INCLUDE_DIRS})
	ENDIF()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	add_library(${_LIBRARY_NAME} STATIC ${_SOURCES})
	# set_target_properties(${_LIBRARY_NAME} PROPERTIES DEBUG_POSTFIX _d)
	target_link_libraries(${_LIBRARY_NAME} ${_DEPENDS})
	foreach(LIB_DIR ${CMAKI_LIBRARIES})
		target_link_libraries(${_LIBRARY_NAME} ${LIB_DIR})
		cmaki_install_3rdparty(${LIB_DIR})
	endforeach()
	IF(WITH_CONAN)
		target_link_libraries(${_LIBRARY_NAME} ${CONAN_LIBS})
		cmaki_install_3rdparty(${CONAN_LIBS})
	ENDIF()
	install(DIRECTORY ${CONAN_LIB_DIRS}/ DESTINATION ${CMAKE_BUILD_TYPE})
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
endfunction()

function(cmaki_test)
	cmaki_parse_parameters(${ARGV})
	set(_TEST_NAME ${_MAIN_NAME})
	set(_TEST_SUFFIX "_unittest")
	common_flags()
	common_linking(${_TEST_NAME}${_TEST_SUFFIX})
	include_directories(node_modules)
	foreach(INCLUDE_DIR ${CMAKI_INCLUDE_DIRS})
		include_directories(${INCLUDE_DIR})
	endforeach()
	IF(WITH_CONAN)
		include_directories(${CONAN_INCLUDE_DIRS})
	ENDIF()
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			add_compile_options(-pthread)
		endif()
	endif()
	add_executable(${_TEST_NAME}${_TEST_SUFFIX} ${_SOURCES})
	# set_target_properties(${_TEST_NAME}${_TEST_SUFFIX} PROPERTIES DEBUG_POSTFIX _d)
	target_link_libraries(${_TEST_NAME}${_TEST_SUFFIX} ${_DEPENDS})
	foreach(LIB_DIR ${CMAKI_LIBRARIES})
		target_link_libraries(${_TEST_NAME}${_TEST_SUFFIX} ${LIB_DIR})
		cmaki_install_3rdparty(${LIB_DIR})
	endforeach()
	IF(WITH_CONAN)
		target_link_libraries(${_TEST_NAME}${_TEST_SUFFIX} ${CONAN_LIBS})
		cmaki_install_3rdparty(${CONAN_LIBS})
	ENDIF()
	install(DIRECTORY ${CONAN_LIB_DIRS}/ DESTINATION ${CMAKE_BUILD_TYPE})
	if(HAVE_PTHREADS)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
			message("-- android no need extra linkage for pthreads")
		else()
			target_link_libraries(${_TEST_NAME}${_TEST_SUFFIX} -lpthread)
		endif()
	endif()
	foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(  	TARGETS ${_TEST_NAME}${_TEST_SUFFIX}
				DESTINATION ${BUILD_TYPE}/${_SUFFIX_DESTINATION}
				CONFIGURATIONS ${BUILD_TYPE})
		if (DEFINED TESTS_VALGRIND AND (TESTS_VALGRIND STREQUAL "TRUE") AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
			find_program(VALGRIND "valgrind")
			if(VALGRIND)
				add_test(
					NAME ${_TEST_NAME}_valgrind_memcheck
					COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> --gmock_verbose=error
					WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
					CONFIGURATIONS ${BUILD_TYPE}
					)
				add_test(
					NAME ${_TEST_NAME}_cachegrind
					COMMAND "${VALGRIND}" --tool=cachegrind $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> --gmock_verbose=error
					WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
					CONFIGURATIONS ${BUILD_TYPE}
					)
				add_test(
					NAME ${_TEST_NAME}_helgrind
					COMMAND "${VALGRIND}" --tool=helgrind $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> --gmock_verbose=error
					WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
					CONFIGURATIONS ${BUILD_TYPE}
					)
				add_test(
					NAME ${_TEST_NAME}_callgrind
					COMMAND "${VALGRIND}" --tool=callgrind $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> --gmock_verbose=error
					WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
					CONFIGURATIONS ${BUILD_TYPE}
					)
				add_test(
					NAME ${_TEST_NAME}_valgrind_drd
					COMMAND "${VALGRIND}" --tool=drd --read-var-info=yes $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> --gmock_verbose=error
					WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BUILD_TYPE}
					CONFIGURATIONS ${BUILD_TYPE}
					)
			else()
				message(FATAL_ERROR "no valgrind detected")
			endif()
		endif()
		if(WIN32)
			add_test(
				NAME ${_TEST_NAME}${_TEST_SUFFIX}
				COMMAND $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> 
				WORKING_DIRECTORY ${CMAKI_INSTALL}/${BUILD_TYPE}
				CONFIGURATIONS ${BUILD_TYPE})
		else()
			add_test(
				NAME ${_TEST_NAME}${_TEST_SUFFIX}
				COMMAND bash ../cmaki_emulator.sh $<TARGET_FILE:${_TEST_NAME}${_TEST_SUFFIX}> 
				WORKING_DIRECTORY ${CMAKI_INSTALL}/${BUILD_TYPE}
				CONFIGURATIONS ${BUILD_TYPE})
		endif()
	endforeach()
	generate_vcxproj_user(${_TEST_NAME})

endfunction()

macro(cmaki_google_test)
	find_package(GTest REQUIRED)
	find_package(GMock REQUIRED)
	add_definitions(-DWITH_MAIN)
	add_definitions(-DWITH_GMOCK)
	set(PARAMETERS ${ARGV})
	list(GET PARAMETERS 0 _MAIN_NAME)
	cmaki_test(${ARGV})
endmacro()

macro(cmaki_python_library)
	# cmaki_find_package(python)
	# cmaki_find_package(boost-python)
	cmaki_library(${ARGV} PTHREADS)
	cmaki_parse_parameters(${ARGV})
	set_target_properties(${_MAIN_NAME} PROPERTIES PREFIX "")
	foreach(BUILD_TYPE ${CMAKE_BUILD_TYPE})
		INSTALL(	TARGETS ${_MAIN_NAME}
				DESTINATION ${BUILD_TYPE}/lib/python3.5/lib-dynload
				CONFIGURATIONS ${BUILD_TYPE})
	endforeach()
endmacro()

macro(cmaki_boost_python_test)
	# cmaki_find_package(python)
	# cmaki_find_package(boost-python)
	cmaki_google_test(${ARGV} PTHREADS)
	cmaki_parse_parameters(${ARGV})
	set_tests_properties(${_MAIN_NAME}_test PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}")
endmacro()

macro(cmaki_python_test)
	# cmaki_find_package(python)
	cmaki_parse_parameters(${ARGV})
	add_test(	NAME ${_MAIN_NAME}_test
			COMMAND ./bin/python3 ${_SOURCES}
			WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE})
	set_tests_properties(${_MAIN_NAME}_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}")
endmacro()

macro(cmaki_python_install)
	# cmaki_find_package(python)
	# cmaki_find_package(boost-python)
	get_filename_component(PYTHON_DIR ${PYTHON_EXECUTABLE} DIRECTORY)
	get_filename_component(PYTHON_PARENT_DIR ${PYTHON_DIR} DIRECTORY)
	cmaki_install_inside_dir(${PYTHON_PARENT_DIR})
endmacro()

macro(cmaki_find_package_boost)
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		set(Boost_DEBUG 1)
	else()
		set(Boost_DEBUG 0)
	endif()
	find_package(Boost REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
endmacro()

