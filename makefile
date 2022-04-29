SRC := src/c
OBJ := out
BIN := out/bin

KERNEL_SOURCES = kernel.c filesystem.c std_lib.c terminal.c 
KERNEL_OBJECTS = $(patsubst %.c, $(OBJ)/%.o, $(KERNEL_SOURCES))

LIBRARY_SOURCES = string.c textio.c fileio.c program.c
LIBRARY_OBJECTS = $(patsubst %.c, $(OBJ)/%.o, $(LIBRARY_SOURCES))

EXECUTABLE_SOURCES = shell.c ls.c mkdir.c cd.c cat.c
EXECUTABLE_OBJECTS = $(patsubst %.c, $(OBJ)/%.o, $(EXECUTABLE_SOURCES))
EXECUTABLES = $(patsubst $(OBJ)/%.o, $(BIN)/%, $(EXECUTABLE_OBJECTS))

.PHONY: all tc_gen test
.PRECIOUS: $(EXECUTABLE_OBJECTS) $(LIBRARY_OBJECTS)

# Makefile
all: diskimage bootloader kernel executable 

# Recipes
diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880
	python3 ./src/python/fill_map.py
bootloader: diskimage
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
$(OBJ)/%.o: $(SRC)/%.c
	bcc -ansi -c -o $@ $<
interrupt:
	nasm -f as86 src/asm/interrupt.asm -o out/lib_interrupt.o
kernel: bootloader interrupt $(KERNEL_OBJECTS)
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d $(KERNEL_OBJECTS) out/kernel_asm.o out/lib_interrupt.o 
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
$(BIN)/%: $(OBJ)/%.o $(LIBRARY_OBJECTS)
	# executable
	ld86 -o $@ -d $< $(LIBRARY_OBJECTS) out/lib_interrupt.o out/filesystem.o out/std_lib.o
executable: kernel $(EXECUTABLES)
	python3 src/python/insert_executables.py
clean:
	rm -f $(KERNEL_OBJECTS) $(LIBRARY_OBJECTS) $(EXECUTABLE_OBJECTS) $(EXECUTABLES)
	rm -f out/bootloader out/kernel out/kernel_asm.o out/lib_interrupt.o out/system.img
tc_gen:
	cd tc_gen && gcc tc_gen.c tc_lib -o tc_gen
	mv tc_gen/tc_gen out/
	cp -r tc_gen/file_src out/
test%: all 
	cd out && ./tc_gen $(patsubst test%, %, $@)
	# $(MAKE) run
shell:
	bcc -ansi -c -o out/shell.o src/c/shell.c
	bcc -ansi -c -o out/textio.o src/c/textio.c
	nasm -f as86 src/asm/interrupt.asm -o out/lib_interrupt.o
	ld86 -o out/shell -d out/shell.o out/lib_interrupt.o out/textio.o out/filesystem.o out/std_lib.o
run:
	-sudo bochs -f src/config/if2230.config
	python3 src/python/extract_log.py
build-run: all run
