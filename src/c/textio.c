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