SRC := src/c
OBJ := out

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Makefile
all: diskimage bootloader compilec kernel

# Recipes
diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880
bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
kernel:
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o $(filter-out out/kernel.o,$(OBJECTS))
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
$(OBJ)/%.o: $(SRC)/%.c
	bcc -ansi -c -o $@ $<
compilec: $(OBJECTS)
clean:
	rm $(wildcard $(OBJ)/*.o)
run:
	echo "c" | sudo bochs -f src/config/if2230.config

build-run: all run
