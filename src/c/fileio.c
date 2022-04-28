#include "header/fileio.h"
#include "header/filesystem.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x05, metadata, return_code, 0);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, 0x04, metadata, return_code, 0);
}

void readSector(byte *buffer, int sector_number) {
    interrupt(0x21, 0x02, buffer, sector_number, 0);
}

void writeSector(byte *buffer, int sector_number) {
    interrupt(0x21, 0x03, buffer, sector_number, 0);
}

void log(char *message, int sector) {
    interrupt(0x21, 0x09, message, sector, 0);
}