#include "kernel.h"

void printchar(char a);
void printCharColor(char a, byte color);
void printHex(int a);
void printString(char *string);
void println(char *string);
void printStringColor(char *string, byte color);
void readString(char *string);
void clearScreen();
void setCursorPosition(int x, int y);

char hex[17];
char *NEWLINE;