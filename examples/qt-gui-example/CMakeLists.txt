find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_executable(WinToastGuiExample mainwindow.ui mainwindow.cpp main.cpp)
target_link_libraries(WinToastGuiExample PRIVATE WinToast Qt6::Widgets)
set_target_properties(WinToastGuiExample PROPERTIES WIN32_EXECUTABLE ON)