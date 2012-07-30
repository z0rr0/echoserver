#!/bin/bash
filename="echoserver"
# file
if [ ! -x $filename ]
then
	echo "not echoserver file"
	exit
fi
# dir - param
if [ -z "$1" ]
then
	echo "use: script <dir>"
	exit
fi
# create dir
if [ ! -d $1 ]
then
	mkdir $1
	echo "mkdir $1"
fi
# copy
if [ -x "$1/$filename" ]
then
	oldfile=`stat -c %Y $1/$filename`
	newfile=`stat -c %Y $filename`
	if [ $newfile -gt $oldfile ]
	then
		echo "file is copied (force)"
		cp -f $filename $1/$filename
	else
		echo "don't copy, file $1/$filename is actually"
	fi
else
	echo "file is copied"
	cp $filename $1/$filename
fi

