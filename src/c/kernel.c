#include "header/kernel.h"

void printChar(char c);

int main() {
  char buf[128];
  clearScreen();
  makeInterrupt21();
  printString("Halo dunia!\r\n");
  readString(buf);
  printString(buf);
  while (true);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printChar(char c) {
  interrupt(0x10, 0x0e00 + c, 0, 0, 0);
}

void printString(char *string) {
  int i;
  for (i = 0; i < strlen(string); i++) {
    printChar(string[i]);
  }
}

void readString(char *string) {
  int i = 0;
  while (i < 128) {
    byte AL = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    if (AL >= 0x20 && AL <= 0x7e) {
      string[i] = AL;
      printChar(AL);
      i++;
      continue;
    }
    if (AL == 0x8 && i > 0) {
      string[--i] = 0x0;
      printChar(0x8); 
      interrupt(0x10, 0x0a20, 0x0, 0x1, 0x0);
    }
    if (AL == 0xd) {
      printChar(0xd);
      printChar(0xa);
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
  interrupt(0x10, 0x0200, 0x03, 0x0, 0x0000); 
}
