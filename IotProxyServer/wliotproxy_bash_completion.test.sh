#!/bin/bash

COMP_WORDS=$*
COMP_CWORD=$#
if [[ $COMP_CWORD == 0 ]]
then
	COMP_CWORD=1
fi
source ./wliotproxy_bash_completion.sh
_wliotproxy
echo "$COMPREPLY"
