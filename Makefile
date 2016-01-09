#############################################################################
# 4IF SEA "SysIF" Makefile
#############################################################################

default:kernel_for_qemu

all: kernel_for_qemu kernel_for_sdcard

kernel_for_qemu: build build/kernel.elf build/kernel.list

kernel_for_sdcard: build kernel_for_qemu build/kernel.img 

remake: clean all

# options à passer au compilateur C
CFLAGS=-Wall -Werror -nostdlib -nostartfiles -ffreestanding -std=c99 -g -fomit-frame-pointer -nostartfiles -O0 -fdiagnostics-show-option

# options à passer à la fois au compilateur C et à l'assembleur
COMMON_FLAGS=-mcpu=arm1176jzf-s -mfloat-abi=soft -mfpu=fpv4-sp-d16

# Object files (excluding kmain)
OBJECTS=build/tune.o build/tune1.o build/tune2.o build/tune3.o build/tune4.o build/tune5.o build/tune6.o $(addsuffix .o,  $(addprefix build/, $(basename $(notdir $(wildcard src/*.[cs])))))

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
build/tune.o : sounds/tune.wav
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
	arm-none-eabi-gcc $(COMMON_FLAGS) $(CFLAGS) -I src $< -c -o $@
	@#"let's silently remember the checksum of the most recent kmain.c we compiled"
	@md5sum $(KMAIN) > build/kmain.md5

# assemblage .s vers ELF
build/%.o: src/%.s | build
	arm-none-eabi-as -g $(COMMON_FLAGS) $< -c -o $@

# édition de liens
build/kernel.elf: $(OBJECTS) build/kmain.o
	arm-none-eabi-ld $^ -o $@ -T src/sysif.ld -Map build/mapfile.map -L lib/ -l csud

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
