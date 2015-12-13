#!/bin/sh

# Use "realpath" or "readlink -f" depending on which one is installed
if command -v realpath 2>/dev/null
then
	GETPATHCMD='realpath'
else
	GETPATHCMD='readlink -f'
fi

if [ $# -eq 1 ]
then
	FILE=$1
else
	FILE='../kmain.c'
fi

SRCFILE=$($GETPATHCMD $FILE)

clear && (cd .. ; make -B KMAIN=$SRCFILE) && clear && echo "Compilation success. ($FILE) Go go gadget QEMU !" && ./run-qemu.sh
