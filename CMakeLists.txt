cmake_minimum_required(VERSION 2.8.9)
project (WinToast)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 4.8.7 REQUIRED QtGui)
#[[


set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME ON) 


find_package(Boost  COMPONENTS date_time filesystem thread  regex REQUIRED)]]
include_directories(${PROJECT_SOURCE_DIR}/src/ )
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(WinToast ${SRC_FILES} )

add_executable(console-example  ${PROJECT_SOURCE_DIR}/example/console-example/main.cpp)
target_link_libraries(console-example WinToast)
add_dependencies(console-example  WinToast)

file(GLOB QT_GUI_SRC_FILES ${PROJECT_SOURCE_DIR}/example//qt-gui-example/WinToastExample/*.cpp)

add_executable(qt-gui-example ${QT_GUI_SRC_FILES})
target_include_directories(qt-gui-example PRIVATE  ${PROJECT_SOURCE_DIR}/qt-gui-example/)
target_link_libraries(qt-gui-example Qt4::QtCore Qt4::QtGui   WinToast)
add_dependencies(qt-gui-example  WinToast)




