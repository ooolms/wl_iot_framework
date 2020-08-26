cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
include(GNUInstallDirs)

project(wliotproxy-simple VERSION 0.1.0)
add_library(wliotproxy-simple SHARED)
set_target_properties(wliotproxy-simple PROPERTIES SOVERSION 0.1.0)
add_executable(wliotproxy-simple-test test/main.cpp test/Model.cpp test/Model.h)

target_sources(wliotproxy-simple PRIVATE
	src/ClientSocket.cpp
	src/Device.cpp
	src/DeviceState.cpp
	src/IDevEventsCallback.cpp
	src/IConnectionCallback.cpp
	src/IMessageCallback.cpp
	src/IWriteCallback.cpp
	src/RealDeviceMessageDispatch.cpp
	src/SrvReady.cpp
	src/StreamParser.cpp
	src/StreamWriter.cpp
	src/Uuid.cpp)

target_sources(wliotproxy-simple PRIVATE
	include/wliot/simple/ClientSocket.h
	include/wliot/simple/Device.h
	include/wliot/simple/DeviceState.h
	include/wliot/simple/IDevEventsCallback.h
	include/wliot/simple/IConnectionCallback.h
	include/wliot/simple/IMessageCallback.h
	include/wliot/simple/IWriteCallback.h
	include/wliot/simple/RealDeviceMessageDispatch.h
	include/wliot/simple/SrvReady.h
	include/wliot/simple/StreamParser.h
	include/wliot/simple/StreamWriter.h
	include/wliot/simple/Uuid.h)

target_include_directories(wliotproxy-simple PUBLIC include)

target_include_directories(wliotproxy-simple-test PUBLIC include)
target_link_libraries(wliotproxy-simple-test wliotproxy-simple)
target_link_directories(wliotproxy-simple-test PRIVATE ${CMAKE_BINARY_DIR})

set_target_properties(wliotproxy-simple PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)

install(
    TARGETS
      wliotproxy-simple
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "include/*")
