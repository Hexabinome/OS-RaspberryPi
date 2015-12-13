#!/bin/sh

if [ $# -ne 2 ]
then
	echo "* First param : kmain C-file"
	echo "* Second param : test gdb-file"
	echo "EXAMPLE : ./run-test ../test/kmain-mytest.c ../test/mytest.gdb"
	exit 1
fi

# Use "realpath" or "readlink -f" depending on which one is installed
if command -v realpath 2>/dev/null
then
	GETPATHCMD='realpath'
else
	GETPATHCMD='readlink -f'
fi 

TOOLSDIR=$(dirname $($GETPATHCMD $0))
SOURCEFILE=$($GETPATHCMD $1)
SCRIPTFILE=$($GETPATHCMD $2)

kill_qemu() { killall -9 qemu-system-arm > /dev/null 2>&1 ;}

cd $TOOLSDIR
cd .. ; make -B KMAIN=$SOURCEFILE || exit 1

cd $TOOLSDIR

kill_qemu
./run-qemu.sh > /dev/null 2>&1 &

./run-gdb.sh $SCRIPTFILE

kill_qemu

