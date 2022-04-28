int puts(char *string);
int putsColor(char *string, int color);
int gets(char *string);
void clearScreen();
void setCursorPosition(int x, int y);
void getCursorPosition(int *x, int *y);
void syncCursorFromMessage();
void syncCursorToMessage();