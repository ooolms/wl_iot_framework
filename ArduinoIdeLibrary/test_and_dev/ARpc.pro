TEMPLATE=app

#this .pro is only for coding
TARGET=/tmp/haergiearbgiuyregre
SOURCES=../ARpc/ARpcSrvReady.cpp ../ARpc/ARpcDeviceState.cpp \
    TestParser.cpp \
    TestDeviceState.cpp \
    main.cpp \
    ../ARpc/ARpcUuid.cpp \
    TestUuid.cpp \
    ../ARpc/ARpcRealDeviceMessageDispatch.cpp \
    ../ARpc/ARpcStreamParser.cpp \
    ../ARpc/ARpcStreamWriter.cpp \
    ../ARpc/ARpcDevice.cpp \
    ../ARpc/ArpcBusDeviceMessageDispatch.cpp \
    TestStarNet.cpp \
    ../ARpc/ARpcStarNetEndPoint.cpp \
    ../ARpc/ARpcStarNetDevice.cpp
HEADERS=../ARpc/ARpcSrvReady.h ../ARpc/ARpcDeviceState.h \
    TestParser.h \
    TestDeviceState.h \
    ../ARpc/ARpcUuid.h \
    TestUuid.h \
    ../ARpc/ARpcRealDeviceMessageDispatch.h \
    ../ARpc/ARpcStreamParser.h \
    ../ARpc/ARpcStreamWriter.h \
    ../ARpc/ARpcDevice.h \
    ../ARpc/ArpcBusDeviceMessageDispatch.h \
    TestStarNet.h \
    ../ARpc/ARpcStarNetEndPoint.h \
    ../ARpc/ARpcStarNetDevice.h
INCLUDEPATH+=../ARpc/

#INCLUDEPATH+=/home/master1312/apps/opt/arduino-1.6.9/hardware/arduino/avr/cores/arduino/\
#/home/master1312/apps/opt/arduino-1.6.9/hardware/tools/avr/avr/include/
