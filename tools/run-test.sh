#!/bin/sh

TOOLSDIR=$(dirname $(realpath $0))
SOURCEFILE=$(realpath $1)
SCRIPTFILE=$(realpath $2)

kill_qemu() { killall -9 qemu-system-arm > /dev/null 2>&1 ;}

cd $TOOLSDIR
cd .. ; make -B KMAIN=$SOURCEFILE || exit 1

cd $TOOLSDIR

kill_qemu
./run-qemu.sh > /dev/null 2>&1 &

./run-gdb.sh $SCRIPTFILE

kill_qemu

