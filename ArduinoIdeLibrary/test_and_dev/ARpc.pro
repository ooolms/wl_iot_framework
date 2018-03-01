TEMPLATE=app

#this .pro is only for coding
TARGET=/tmp/haergiearbgiuyregre
SOURCES=../ARpc/ARpc.cpp ../ARpc/ARpcBase.cpp ../ARpc/ARpcSrvReady.cpp ../ARpc/ARpcDeviceState.cpp \
    TestParser.cpp \
    TestDeviceState.cpp \
    main.cpp
HEADERS=../ARpc/ARpc.h ../ARpc/ARpcBase.h ../ARpc/ARpcSrvReady.h ../ARpc/ARpcDeviceState.h \
    TestParser.h \
    TestDeviceState.h
INCLUDEPATH+=../ARpc/

#INCLUDEPATH+=/home/master1312/apps/opt/arduino-1.6.9/hardware/arduino/avr/cores/arduino/\
#/home/master1312/apps/opt/arduino-1.6.9/hardware/tools/avr/avr/include/
