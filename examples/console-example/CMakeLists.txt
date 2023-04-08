set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:wmainCRTStartup")
add_executable(WinToastTuiExample WIN32 main.cpp)
target_link_libraries(WinToastTuiExample PRIVATE WinToast)
set_target_properties(WinToastTuiExample PROPERTIES WIN32_EXECUTABLE ON)