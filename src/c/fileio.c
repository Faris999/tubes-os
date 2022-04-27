#include "header/fileio.h"
#include "header/filesystem.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x05, metadata, return_code, 0);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x04, metadata, return_code, 0);
}