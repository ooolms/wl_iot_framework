cmake_minimum_required(VERSION 2.8)

project(libwliotproxy-simple)
add_library(wliotproxy-simple SHARED)

target_sources(wliotproxy-simple PRIVATE
	src/ClientSocket.cpp
	src/Device.cpp
	src/DeviceState.cpp
	src/IDevEventsCallback.cpp
	src/IMessageCallback.cpp
	src/IWriteCallback.cpp
	src/RealDeviceMessageDispatch.cpp
	src/SrvReady.cpp
	src/StreamParser.cpp
	src/StreamWriter.cpp
	src/Uuid.cpp)

target_sources(wliotproxy-simple PUBLIC
	include/wliot/simple/ClientSocket.h
	include/wliot/simple/Device.h
	include/wliot/simple/DeviceState.h
	include/wliot/simple/IDevEventsCallback.h
	include/wliot/simple/IMessageCallback.h
	include/wliot/simple/IWriteCallback.h
	include/wliot/simple/RealDeviceMessageDispatch.h
	include/wliot/simple/SrvReady.h
	include/wliot/simple/StreamParser.h
	include/wliot/simple/StreamWriter.h
	include/wliot/simple/Uuid.h)

target_include_directories(wliotproxy-simple PUBLIC include)

set_target_properties(wliotproxy-simple PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
install(TARGETS wliotproxy-simple DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS wliotproxy-simple DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "include/*")
