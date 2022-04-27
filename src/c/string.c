#include "header/string.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

bool strcmp(char *s1, char *s2) {
  int i;
  for (i = 0; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0') {
      return true;
    }
  }
  return false;
}