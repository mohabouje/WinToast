set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
add_executable(WinToastTuiExample main.cpp)
target_link_libraries(WinToastTuiExample PRIVATE WinToast)
