#include "../header/fileio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x4, metadata, return_code, 0);
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x5, metadata, return_code, 0);
}

void readSector(byte *buffer, int sector) {
    interrupt(0x21, 0x2, buffer, sector, 0);
}

void writeSector(byte *buffer, int sector) {
    interrupt(0x21, 0x3, buffer, sector, 0);
}
