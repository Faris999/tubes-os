#include "../header/program.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void exec(struct file_metadata *metadata, int segment) {
    interrupt(0x21, 0x7, metadata, segment, 0);
}