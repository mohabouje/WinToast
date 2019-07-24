cmake_minimum_required(VERSION 2.8.9)
project (WinToast)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)


if(NOT Qt5Widgets_FOUND)
    find_package(Qt4 4.8.7)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src/ )
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(WinToast ${SRC_FILES} )

add_executable(console-example  ${PROJECT_SOURCE_DIR}/example/console-example/main.cpp)
target_link_libraries(console-example WinToast)
add_dependencies(console-example  WinToast)

file(GLOB QT_GUI_SRC_FILES ${PROJECT_SOURCE_DIR}/example//qt-gui-example/WinToastExample/*.cpp)

if(Qt5Widgets_FOUND)
    message("QT5 Found! building qt-gui-example with QT5 ")
    add_executable(qt-gui-example ${QT_GUI_SRC_FILES})
    target_include_directories(qt-gui-example PRIVATE  ${PROJECT_SOURCE_DIR}/qt-gui-example/)
    qt5_use_package(qt-gui-example Widgets)
    qt5_use_package(qt-gui-example Core)
    add_dependencies(qt-gui-example  WinToast)
else()
    message("QT5 Not Found!")
    if(Qt4_FOUND)
        message("QT4 Found! building qt-gui-example ")
        set(CMAKE_CXX_FLAGS “${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}”)
        add_executable(qt-gui-example ${QT_GUI_SRC_FILES})
        target_include_directories(qt-gui-example PRIVATE  ${PROJECT_SOURCE_DIR}/qt-gui-example/)
        target_link_libraries(qt-gui-example Qt4::QtCore Qt4::QtGui   WinToast)
        add_dependencies(qt-gui-example  WinToast)
    else()
        message("QT4 Not Found! not building qt-gui-example ")
    endif()
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)



