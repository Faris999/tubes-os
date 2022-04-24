.ONESHELL:
# Makefile
all: diskimage bootloader stdlib compilec kernel
# Recipes
diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880
bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
compilec:
	bcc -ansi -c -o out/terminal.o src/c/terminal.c
	bcc -ansi -c -o out/filesystem.o src/c/filesystem.c
	bcc -ansi -c -o out/utils.o src/c/utils.c
	bcc -ansi -c -o out/testing.o src/c/testing.c
kernel:
	bcc -ansi -c -o out/kernel.o src/c/kernel.c
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/std_lib.o out/terminal.o out/utils.o out/filesystem.o out/testing.o
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
stdlib:
	# Opsional
	bcc -ansi -c -o out/std_lib.o src/c/std_lib.c
generate_testkernel:
	cd src/python
	python3 generate_testkernel.py $(testcase)
	cd ../..
testcase:
	python3 src/python/fill_map.py
	cd out
	./tc_gen $(testcase)
	cd ..
testkernel:
	bcc -ansi -c -o out/kernel.o src/c/testkernel.c
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/std_lib.o out/terminal.o out/utils.o out/filesystem.o out/testing.o
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
testA:
	$(MAKE) testcase=A test
testB:
	$(MAKE) testcase=B test
testC:
	$(MAKE) testcase=C test
testD:
	$(MAKE) testcase=D test
test: diskimage bootloader stdlib compilec generate_testkernel testkernel testcase run 
run:
	echo "c" | sudo bochs -f src/config/if2230.config

build-run: all run
