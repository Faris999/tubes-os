SRC := src/c
OBJ := out

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

.PHONY: all tc_gen test

# Makefile
all: diskimage bootloader kernel

# Recipes
diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880
bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
$(OBJ)/%.o: $(SRC)/%.c
	bcc -ansi -c -o $@ $<
kernel: $(OBJECTS)
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o $(filter-out out/kernel.o,$(OBJECTS))
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
clean:
	rm $(wildcard $(OBJ)/*.o)
tc_gen:
	cd tc_gen && gcc tc_gen.c tc_lib -o tc_gen
	mv tc_gen/tc_gen out/
	cp -r tc_gen/file_src out/
test%: all 
	python3 ./src/python/fill_map.py
	cd out && ./tc_gen $(patsubst test%, %, $@)
	$(MAKE) run
run:
	echo "c" | sudo bochs -f src/config/if2230.config
build-run: all run
