#!/bin/bash

COMP_WORDS=($*)
COMP_CWORD=$#

echo "SOURCE:"
echo $COMP_CWORD
for (( i=0; i<$COMP_CWORD; i++ ))
do
	echo ${COMP_WORDS[i]}
done

if [[ $COMP_CWORD == 0 ]]
then
	COMP_CWORD=1
	${COMP_WORDS[0]}=""
fi

source ./wliotproxy_bash_completion.sh
_wliotproxy

echo "REPLY:"
echo ${#COMPREPLY[*]}
for (( i=0; i<${#COMPREPLY[*]}; i++ ))
do
	echo ${COMPREPLY[i]}
done
