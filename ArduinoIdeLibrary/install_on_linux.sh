#!/bin/bash

saveD=$PWD
cd `dirname "$0"`

rm -rf "$HOME/Arduino/libraries/ARpc"
cp -R ./ARpc "$HOME/Arduino/libraries"

rm -rf "$HOME/Arduino/libraries/ARpcESP8266WiFiDevice"
cp -R ./ARpcESP8266WiFiDevice "$HOME/Arduino/libraries"

rm -rf "$HOME/Arduino/libraries/ARpcEEPROM"
cp -R ./ARpcEEPROM "$HOME/Arduino/libraries"

rm -rf "$HOME/Arduino/libraries/ARpcTimerOnMillis"
cp -R ./ARpcTimerOnMillis "$HOME/Arduino/libraries"

cd "$saveD"
