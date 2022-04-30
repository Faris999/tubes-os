#include "../header/textio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void putsHex(int a) {
  int i;
  char buf[5];
  char *hex = "0123456789ABCDEF";
  puts("0x");
  for (i = 3; i >= 0; i--) {
    buf[i] = hex[a % 16];
    a /= 16;
  }
  buf[4] = '\0';
  puts(buf);
}

void puts(char *s) {
    interrupt(0x21, 0x0, s, 0, 0);
}

void gets(char *s) {
    interrupt(0x21, 0x1, s, 0, 0);
}