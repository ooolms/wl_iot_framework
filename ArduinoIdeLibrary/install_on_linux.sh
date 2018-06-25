#!/bin/bash

saveD=$PWD
cd `dirname "$0"`
rm -rf "$HOME/Arduino/libraries/ARpc"
cp -R ./ARpc "$HOME/Arduino/libraries"
cd "$saveD"
