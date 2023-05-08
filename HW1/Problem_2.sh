#!/bin/bash

echo "Enter a file name for backup"
read file
d=$(date "+%T")

if [ -f $file ]
then
	cp $file "$file.bak"$d
else
	echo "File doesnt exist"
fi
