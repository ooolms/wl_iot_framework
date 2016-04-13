#!/bin/bash

cd `dirname $0`

if [[ ! -d "./Arduino" ]]
then
	git clone git+ssh://wl.unn.ru:2024/repositories/wl-git/QtUnit.git QtUnit
else
	cd QtUnit
	git pull
	cd ..
fi
