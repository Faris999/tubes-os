#include "header/terminal.h"


char hex[17] = "0123456789ABCDEF";
void printchar(char a) {
  interrupt(0x10, 0x0e00 + a, 0x0000, 0x0, 0x0);
}

char* convertHex(int a) {
  int i;
  char buf[7];
  buf[0] = '0';
  buf[1] = 'x';
  for (i = 5; i >= 2; i--) {
    buf[i] = hex[a % 16];
    a /= 16;
  }
  buf[6] = '\0';
  return buf;
}

void printInt(int a) {
  int i;
  char buf[6];
  for (i = 4; i >= 0; i--) {
    buf[i] = hex[mod(a, 10)];
    a /= 10;
  }
  buf[5] = '\0';
  printString(buf);
}

void printHex(int a) {
  int i;
  char buf[5];
  printString("0x");
  for (i = 3; i >= 0; i--) {
    buf[i] = hex[a % 16];
    a /= 16;
  }
  buf[4] = '\0';
  printString(buf);
}

void printString(char *string) {
  int i;
  for (i = 0; i < strlen(string); i++) {
    interrupt(0x10, 0x0e00 + string[i], 0x0000, 0x0, 0x0);
  }
}

void readString(char *string) {
  int i = 0;
  while (i < 128) {
    byte AL = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    if (AL >= 0x20 && AL <= 0x7e) {
      string[i] = AL;
      printchar(AL);
      i++;
      continue;
    }
    if (AL == 0x8 && i > 0) {
      string[--i] = 0x0;
      printchar(0x8);
      interrupt(0x10, 0x0a20, 0x0, 0x1, 0x0);
    }
    if (AL == 0xd) {
      printchar(0xd);
      printchar(0xa);
      string[i] = '\0';
      break;
    }
  }
}

void clearScreen() {
  int i;
  int j;
  for (i = 0; i < 80; i++) {
    for (j = 0; j < 25; j++) {
      putInMemory(0xB000, 0x8000 + (80 * j + i) * 2, 0x20);
      putInMemory(0xB000, 0x8001 + (80 * j + i) * 2, 0xF);
    }
  }

  // clear screen
//  interrupt(0x10, 0x0600, 0x07, 0x0000, 0x1950);
  //interrupt(0x10, 0x0003, 0x00, 0x00);
  // move cursor to top left
  interrupt(0x10, 0x0200, 0x03, 0x0, 0x0000);
}