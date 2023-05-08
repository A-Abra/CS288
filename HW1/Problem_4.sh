#!/bin/bash

inputdir=`pwd`
if [ $# -eq 1 ];
then
	inputdir="$1"
fi

goInDir() {
	for stuff in $1/*;
	do
		if [ -d $stuff ];
		then
			goInDir $stuff
		elif [ ! -s $stuff ];
		then
			rm -f $stuff
		fi
	done
}
goInDir $inputdir
