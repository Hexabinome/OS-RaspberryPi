#!/bin/sh

if [ $# -eq 0 ]
then
    arm-none-eabi-gdb ../build/kernel.elf -x init.gdb
else
    arm-none-eabi-gdb --batch ../build/kernel.elf -x $1
fi

stty sane
