#!/bin/bash

total=0

if [ $# != 0 ]
then
	for arg in $@
	do
		(( total += $arg ))
	done
fi

echo $total
