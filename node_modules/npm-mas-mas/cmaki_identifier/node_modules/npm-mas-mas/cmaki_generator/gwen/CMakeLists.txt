cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)

include_directories(gwen/include)

#ADD_DEFINITIONS(-DGWEN_COMPILE_STATIC -D_HAS_EXCEPTIONS=0 -D_STATIC_CPPLIB)
ADD_DEFINITIONS(-DGWEN_COMPILE_DLL)

IF(WIN32)
    
ELSE()
    add_definitions(-std=c++11)
ENDIF()

file(GLOB SOURCE_CODE1 gwen/src/*.cpp)
file(GLOB SOURCE_CODE2 gwen/src/Controls/*.cpp)
file(GLOB SOURCE_CODE3 gwen/src/Controls/Dialog/*.cpp)
file(GLOB SOURCE_CODE4 gwen/src/Platforms/*.cpp)

add_library(${PACKAGE} SHARED ${SOURCE_CODE1} ${SOURCE_CODE2} ${SOURCE_CODE3} ${SOURCE_CODE4})

file(GLOB HEADER_CODE1 gwen/include/Gwen/*.h)
INSTALL(    FILES ${HEADER_CODE1}
            DESTINATION "include/${PACKAGE}")

file(GLOB HEADER_CODE2 gwen/include/Gwen/Controls/*.h)
INSTALL(    FILES ${HEADER_CODE2}
            DESTINATION "include/${PACKAGE}/Controls")

file(GLOB HEADER_CODE3 gwen/include/Gwen/Controls/Dialog/*.h)            
INSTALL(    FILES ${HEADER_CODE3}
            DESTINATION "include/${PACKAGE}/Controls/Dialog")

file(GLOB HEADER_CODE4 gwen/include/Gwen/Input/*.h)            
INSTALL(    FILES ${HEADER_CODE4}
            DESTINATION "include/${PACKAGE}/Input")

file(GLOB HEADER_CODE5 gwen/include/Gwen/Renderers/*.h)            
INSTALL(    FILES ${HEADER_CODE5}
            DESTINATION "include/${PACKAGE}/Renderers")

file(GLOB HEADER_CODE6 gwen/include/Gwen/Skins/*.h)            
INSTALL(    FILES ${HEADER_CODE6}
            DESTINATION "include/${PACKAGE}/Skins")

INSTALL(    FILES gwen/bin/DefaultSkin.png
            DESTINATION "bin")
