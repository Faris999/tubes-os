#include "../header/filesystem.h"
#include "../header/program.h"
#include "../header/fileio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void read_message(struct message *msg) {
    readSector(msg, 0x104);
}

void write_message(struct message *msg) {
    writeSector(msg, 0x104);
}

void exec(struct file_metadata *metadata, int segment) {
    interrupt(0x21, 0x6, metadata, segment, 0);
}

void exit() {
    struct file_metadata metadata;
    metadata.node_name = "shell";
    metadata.node_index = 0;
    exec(&metadata, 0x2000);
}