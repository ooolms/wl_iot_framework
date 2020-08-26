cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
include(GNUInstallDirs)

project(wliotproxy-simple-test VERSION 0.1.0)
add_executable(test-device test/main.cpp test/Model.cpp test/Model.h)

target_include_directories(test-device PUBLIC ${CMAKE_INSTALL_INCLUDEDIR})
target_link_libraries(test-device wliotproxy-simple)
target_link_directories(test-device PRIVATE ${CMAKE_BINARY_DIR})
