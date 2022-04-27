extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

int puts(char *string) {
    interrupt(0x21, 0x0, string, 0, 0);
}