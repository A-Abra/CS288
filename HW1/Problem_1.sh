#!/bin/bash

if [ -f $1 ]
then
	filesize=`du -b $1 | cut -f 1`
	if [ $filesize -gt 1048576 ];
	then
		echo "This is a large file with:"
	elif [[ $filesize -le 1048576 && $filesize -gt 102400 ]];
	then
		echo "This is a medium file with:"
	else
		echo "This is a small file with:"
	fi

	if [ -r $1 ];
	then
		echo "Reading permission"
	fi
	if [ -w $1 ];
	then
		echo "Writing permission"
	fi
	if [ -x $1 ];
	then
		echo "Execute permission"
	fi

elif [ -d $1 ];
then
	echo "This is a directory with:"
	if [ -r $1 ];
	then
		echo "Reading permission"
	fi
	if [ -w $1 ];
	then
		echo "Writing permission"
	fi
	if [ -x $1 ];
	then
		echo "Execute permission"
	fi
fi
