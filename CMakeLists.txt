cmake_minimum_required(VERSION 3.4)
project(wintoastlib VERSION 1.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WINTOASTLIB_BUILD_EXAMPLES   "Compile the examples" ON)
option(WINTOASTLIB_QT_ENABLED       "Enable Qt support to build the GUI examples" OFF)

set(WINTOASTLIB_LIBNAME WinToast)
set(WINTOASTLIB_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/wintoastlib.h)
set(WINTOASTLIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/wintoastlib.cpp)
add_library(${WINTOASTLIB_LIBNAME} STATIC ${WINTOASTLIB_HEADERS} ${WINTOASTLIB_SOURCES})
target_include_directories(${WINTOASTLIB_LIBNAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${WINTOASTLIB_LIBNAME} psapi)

if (${WINTOASTLIB_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()
