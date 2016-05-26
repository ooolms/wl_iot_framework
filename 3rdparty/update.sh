#!/bin/bash

updaterScript="update.sh"
setupScript="setup.sh"

helpStr="Usage:\n
$updaterScript all - update all depencies\n
$updaterScript add url dir - make 'git clone url dir' and add dir to .gitignore\n
$updaterScript del dir - delete dir from disk and from .gitignore\n
$updaterScript update dir [-r] - update one dependency"

cd `dirname $0`

if [[ "$1" = "--help" ]]; then
	echo -e "$helpStr"
	exit 0
fi

if [[ -z "$1" || "$1" = "all" ]];then
	for dir in *
	do
		if [[ "$dir" = "." || "$dir" = ".." ]];then continue; fi
		if [[ ! -d "$dir" ]]; then continue; fi
		echo "Updating: $dir"
		cd "$dir"
		git pull
		if [ -x "./3rdparty/$updaterScript" ];then
			echo "Updating $dir/3rdparty"
			"./3rdparty/$updaterScript" all
		fi
	cd - > /dev/null
	done
elif [[ "$1" = "add" ]]; then
	if [[ -z "$2" || -z "$3" ]]; then
		echo -e "$helpStr";
		exit 1
	fi
	url=$2
	dir=$3
	if [[ -e "$dir" ]]; then
		echo "Dir $dir already exists"
		exit 1
	fi
	git clone "$url" "$dir"
	if [ -x "./$dir/3rdparty/$setupScript" ];then
		echo "Setting up $dir/3rdparty"
		"./$dir/3rdparty/$setupScript"
	fi
	echo "/$dir" >> .gitignore
elif [[ "$1" = "del" ]];then
	if [[ -z "$2" ]]; then
		echo -e "$helpStr"
		exit 1
	fi
	dir=$2
	echo "Deliting: $dir"
	if [[ ! -d "$dir" ]]; then
		echo "Not a dir"
	fi
	read -p "Sure (y/n)" conf
	#TODO check $dir for security (not a "." or "..", doesn't contains slashes)
	if [[ ! "$conf" = "y" ]];then
		exit 0;
	fi
	rm -rf "$dir"
	cat .gitignore | sed "/\/$dir/d" > .gitignore
elif [[ "$1" = "update" ]];then
	if [[ -z "$2" ]]; then
		echo -e "$helpStr"
		exit 1
	fi
	dir=$2
	if [[ ! -d "$dir" ]]; then
		echo "Not a dir"
	fi
	echo "Updating: $dir"
	cd "$dir"
	git pull
	cd - > /dev/null
else
	echo -e "$helpStr"
	exit 1
fi


#if [[ ! -d "./QtUnit" ]]
#then
#	git clone git+ssh://wl.unn.ru:2024/repositories/wl-git/QtUnit.git QtUnit
#else
#	cd QtUnit
#	git pull
#	cd ..
#fi
