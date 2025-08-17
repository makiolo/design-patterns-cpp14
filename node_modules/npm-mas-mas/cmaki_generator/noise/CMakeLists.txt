cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)
project(noise CXX)

# http://sourceforge.net/projects/libnoise

file(GLOB SOURCE_CODE src/*.cpp src/*.h src/model/*.cpp src/model/*.h src/module/*.cpp src/module/*.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/etc)
add_library(${PACKAGE} SHARED ${SOURCE_CODE})

#IF(MSVC)
#	add_definitions(/nologo /c /D_CRT_SECURE_NO_DEPRECATE)
#ENDIF()

file(GLOB HEADER_CODE src/*.h )
INSTALL(    FILES ${HEADER_CODE}
            DESTINATION "include/${PACKAGE}")

file(GLOB HEADER_CODE src/model/*.h )
INSTALL(    FILES ${HEADER_CODE}
            DESTINATION "include/${PACKAGE}/model")
            
file(GLOB HEADER_CODE src/module/*.h )
INSTALL(    FILES ${HEADER_CODE}
            DESTINATION "include/${PACKAGE}/module")
