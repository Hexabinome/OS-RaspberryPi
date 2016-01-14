#############################################################################
# 4IF SEA "SysIF" Makefile
#############################################################################

default:kernel_for_qemu

all: kernel_for_qemu kernel_for_sdcard

kernel_for_qemu: build build/kernel.elf build/kernel.list

kernel_for_sdcard: kernel_for_qemu build/kernel.img 

remake: clean all

INCDIR=include/
SRCDIR=src/

# options à passer au compilateur C
CFLAGS=-Wall -Werror -nostdlib -nostartfiles -ffreestanding -std=c99 -g -fomit-frame-pointer -nostartfiles -O0 -fdiagnostics-show-option -I $(INCDIR) -fshort-wchar

# options à passer à la fois au compilateur C et à l'assembleur
COMMON_FLAGS=-mcpu=arm1176jzf-s -mfloat-abi=soft -mfpu=fpv4-sp-d16

# Object files (excluding kmain)
TUNEOBJ=tune.o tune1.o tune2.o tune3.o tune4.o tune5.o tune6.o fbe1.o 18bitmono.o 28bitmono.o 38bitmono.o 48bitmono.o 58bitmono.o 68bitmono.o 78bitmono.o 88bitmono.o 98bitmono.o 108bitmono.o 118bitmono.o 128bitmono.o 138bitmono.o 148bitmono.o 158bitmono.o 168bitmono.o 178bitmono.o 188bitmono.o  198bitmono.o 208bitmono.o 218bitmono.o 228bitmono.o 238bitmono.o 248bitmono.o
OBJECTS=$(addprefix build/, $(TUNEOBJ)) $(addsuffix .o,  $(addprefix build/, $(basename $(notdir $(wildcard $(SRCDIR)/*.[cs])))))

# non-default usage: "make KMAIN=test/my-kmain.c" 
KMAIN ?= ./kmain.c

# check whether kmain does exists. typically this would get triggered
# by a command like "make KMAIN=" (i.e. with no value for KMAIN)
ifneq "$(shell test -f ''${KMAIN} && echo yes || echo no)" "yes"
    $(error no such file: "${KMAIN}")
endif

# check whether we're dealing with a different kmain.c than last time:
# either updated/modified, or maybe another file, located somewhere else
ifneq "$(shell md5sum ${KMAIN})" "$(shell test -f build/kmain.md5 && cat build/kmain.md5)"
  # if so, then our kmain.o has to be recompiled
  .PHONY: build/kmain.o
endif

build:
	mkdir -p build
	
#Compilation du fichier .wav
# for i in *mono*; do echo "build/$(basename -s .wav $i).o : sounds/$i"; echo "arm-none-eabi-ld -s -r -o \$@ -b binary $^"; done >> /tmp/kt
build/108bitmono.o : sounds/108bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/118bitmono.o : sounds/118bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/128bitmono.o : sounds/128bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/138bitmono.o : sounds/138bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/148bitmono.o : sounds/148bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/158bitmono.o : sounds/158bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/168bitmono.o : sounds/168bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/178bitmono.o : sounds/178bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/188bitmono.o : sounds/188bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/18bitmono.o : sounds/18bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/198bitmono.o : sounds/198bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/208bitmono.o : sounds/208bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/218bitmono.o : sounds/218bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/228bitmono.o : sounds/228bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/238bitmono.o : sounds/238bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/248bitmono.o : sounds/248bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/28bitmono.o : sounds/28bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/38bitmono.o : sounds/38bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/48bitmono.o : sounds/48bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/58bitmono.o : sounds/58bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/68bitmono.o : sounds/68bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/78bitmono.o : sounds/78bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/88bitmono.o : sounds/88bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/98bitmono.o : sounds/98bitmono.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune.o : sounds/tune.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/fbe1.o : sounds/fbe1.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune1.o : sounds/B4.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune2.o : sounds/C1_1.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune3.o : sounds/D_4.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune4.o : sounds/E1.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune5.o : sounds/F3.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^
build/tune6.o : sounds/G6.wav
	arm-none-eabi-ld -s -r -o $@ -b binary $^

# compilation C vers ELF
build/%.o: src/%.c $(wildcard src/*.h) | build
	arm-none-eabi-gcc $(COMMON_FLAGS) $(CFLAGS) $< -c -o $@

# compilation du point d'entrée 
build/kmain.o: $(KMAIN) $(wildcard src/*.h)
	arm-none-eabi-gcc $(COMMON_FLAGS) $(CFLAGS) -I $(SRCDIR) $< -c -o $@
	@#"let's silently remember the checksum of the most recent kmain.c we compiled"
	@md5sum $(KMAIN) > build/kmain.md5

# assemblage .s vers ELF
build/%.o: $(SRCDIR)/%.s | build
	arm-none-eabi-as -g $(COMMON_FLAGS) $< -c -o $@

# édition de liens
build/kernel.elf: $(OBJECTS) build/kmain.o
	arm-none-eabi-ld $^ -o $@ -T $(SRCDIR)/sysif.ld -Map build/mapfile.map -L lib/ -l csud

# conversion de l'image pour transfert sur carte SD
build/kernel.img: build/kernel.elf
	arm-none-eabi-objcopy $^ -O binary $@

# désassemblage
build/kernel.list: build/kernel.elf
	arm-none-eabi-objdump -d -j .text -j .bss -j .stack -j .kernel_heap -j .user_stacks $< > $@

# nettoyage: effacer tous les fichiers générés
.PHONY:clean
clean:
	rm -rf build
