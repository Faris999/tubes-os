#include "header/textio.h"
#include "header/program.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

int puts(char *string) {
    interrupt(0x21, 0x0, string, 0x0F, 0);
}

int putsColor(char *string, int color) {
    interrupt(0x21, 0x0, string, color, 0);
}

int gets(char *string) {
    interrupt(0x21, 0x1, string, 0, 0);
}

void clearScreen() {
    interrupt(0x21, 0x7, 0, 0, 0);
}

void setCursorPosition(int x, int y) {
    interrupt(0x21, 0x8, x, y, 0);
}

void getCursorPosition(int *x, int *y) {
    interrupt(0x21, 0xa, x, y, 0);
}

void syncCursorFromMessage() {
    struct message msg;
    get_message(&msg);
    setCursorPosition(msg.cursor_x, msg.cursor_y);
}

void syncCursorToMessage() {
    struct message msg;
    get_message(&msg);
    getCursorPosition(&msg.cursor_x, &msg.cursor_y);
    write_message(&msg);
}