#include "../header/textio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void puts(char *s) {
    interrupt(0x21, 0x0, s, 0, 0);
}

void gets(char *s) {
    interrupt(0x21, 0x1, s, 0, 0);
}