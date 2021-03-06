// Kernel header

#include "std_type.h"
#include "std_lib.h"

// Fungsi bawaan
extern void putInMemory(int segment, int address, char character);
extern int interrupt(int int_number, int AX, int BX, int CX, int DX);
extern void launchProgram(int segment);
void makeInterrupt21();
void handleInterrupt21(int AX, int BX, int CX, int DX);
void executeProgram(struct file_metadata *metadata, int segment);

void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);

void fillMap();

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);

void log(char *message, int sector);