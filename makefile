UTILS_SOURCES = $(wildcard src/c/utils/*.c)
UTILS_OBJECTS = $(patsubst src/c/utils/%.c, out/utils/%.o, $(UTILS_SOURCES))

# Makefile
all: diskimage bootloader stdlib kernel

.PHONY: tc_gen

# Recipes
diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880
	python3 ./tools/fill_map.py
bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
kernel: $(UTILS_OBJECTS)
	bcc -ansi -c -o out/kernel.o src/c/kernel.c
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/std_lib.o $(UTILS_OBJECTS)
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
stdlib:
	bcc -ansi -c -o out/std_lib.o src/c/std_lib.c
out/utils/%.o: src/c/utils/%.c
	bcc -ansi -c -o $@ $<
tc_gen:
	cd tc_gen && gcc tc_gen.c tc_lib -o tc_gen
	mv tc_gen/tc_gen out/
	cp -r tc_gen/file_src out/
test%: all 
	cd out && ./tc_gen $(patsubst test%, %, $@)
run:
	sudo bochs -f src/config/if2230.config

build-run: all run
