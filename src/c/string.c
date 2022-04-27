#include "header/string.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

unsigned int strlen(char *str) {
  char *s;
  for (s = str; *s; ++s);
  return (s - str);
}

bool strcmp(char *s1, char *s2) {
  int i;
  for (i = 0; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0') {
      return true;
    }
  }
  return false;
}

bool startswith(char *prefix, char *s2) {
  int i;
  for (i = 0; i < strlen(prefix); i++) {
    if (prefix[i] != s2[i]) {
      return false;
    }
  }
  return true;
}

void strcpy(char *dst, char *src) {
  int i;
  for (i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
}