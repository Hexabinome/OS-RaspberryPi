# RaspberryPi OS : Momo-OS
This humble operating system is designed for ARM architectured CPU on RaspberryPi.
This project has been realised at INSA Lyon, France for the _Operating Systems_ class during the fourth year in the Computer Science department.
It is called _Momo-OS_ because of our group mascot [@Mohamed](https://github.com/haidaraM) :D

## Current progress
Implemented functionalities :
* System calls
* Process dispatching
* Scheduling (with different schedulers : _Round Robin_, _Fixed priorities_, _Dynamic prorities_ and _Random_)
* Virtual memory
* Process isolation
* Dynamic memory allocation (_malloc_, _realloc_, _free_)
* Semaphores
* Sound driver (_pwm_)
* Framebuffer (display driver), by displaying text
* _fork_, _wait_, _waitpid_ system calls (actually doesn't _really_ work, no heap and stack copying. More difficult than expected)
* Keyboard driver
* Little shell, combining a lot of the implemented mecanisms

## Usage
You can either use our OS on a real RPi or on an emulator

### On qemu
You will need to install _qemu_ :
* ```git clone https://github.com/Torlus/qemu.git```
* ```cd qemu```
* ```git checkout -b raspberry origin/rpi```
* ```./configure --target-list="arm-softmmu"```
* ```make```
* The binary is now in _arm-softmmu/qemu-system-arm_. Add this folder to your _PATH_.

The corresponding compilor : _gcc-arm-none-eabi_

And _gdb-arm-none-eabi_ for debugging (at least 7.10 version) :
* Download the 7.10 arm gdb package from the [gnu package website](http://www.gnu.org/software/gdb/download/)
* ```tar xzf gdb-7.10.tar.gz```
* ```cd gdb-7.10```
* ```./configure –prefix=<install_dir> –target=arm-none-eabi –enable-tui```
* ```make```
* ```make install```
* Add _\<install_dir\>/bin_ to your _PATH_


To launch the kernel, go to _tools/_.

```./make_qemu.sh``` will compile the kernel and launch _qemu_

While _qemu_ is running, launch, from another terminal, ```./run-gdb.sh```

You can now, step by step, go through the kernel execution.
We recommand to be on the _shell-fb_ branch, which doesn't need contain the keyboard (which is quite difficult to work with in qemu)

### On RaspberryPi
Compile the kernel using ```make kernel_for_sdcard```. Make sure the RPI constant in _include/config.h_ is set to 1.

Then copy the generated _kernel.img_ file on a bootable SD card.

Plug it in your RaspberryPi, plug a keyboard, a monitor and some headphones. And get started by powering the RPi.


## To-do
If we had more time and not other projects, we would have done
* Correct the _fork_ system call
* Make semaphore functions system calls
* A "real" shell (with a customizable prompt, stream redirection, command chaining (pipe and stuff), shell variables, commands that are real executables, etc)
* A file sytem
* And a lot more

## Tests
We have a no-regression _framework_, which is actually a simple python file which executes a list of tests and say if they passed or not.

A test is composed of a C source file (kmain...) and a gdb file (which asserts the resulting values).

You can execute a single test with ```tools/run-test.sh test/kmain-bidule.c test/bidule-called-twice.gdb```

## Credits
Made by Hexabinome H4105 2015-2016
* Alexis Andra [@gazolik](https://github.com/Gazolik) [@gazolike](https://github.com/Gazolike)
* Jolan Cornevin [@Djowood](https://github.com/Djowood)
* Mohamed El Mouctar Haidara [@haidaraM](https://github.com/haidaraM)
* Alexis Papin [@alexispapin](https://github.com/alexispapin)
* Robin Royer [@robinroyer](https://github.com/robinroyer)
* Maximilan Schiedermeier [@maxinsa42](https://github.com/maxinsa42) [@maxou42](https://github.com/maxou42)
* David Wobrock [@David-Wobrock](https://github.com/David-Wobrock)
