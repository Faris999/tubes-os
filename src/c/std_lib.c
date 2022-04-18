#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C

int div(int a, int b) {
  return a / b;
}

int mod(int a, int b) {
  // modulo function from scratch
  int q = div(a, b);
  return a - q * b;
}

void memcpy(byte *dest, byte *src, unsigned int n) {
  int i;
  for (i = 0; i < n; i++) {
    dest[i] = src[i];
  }
}

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

bool startswith(char *s1, char *s2) {
  int i;
  for (i = 0; i < strlen(s1); i++) {
    if (s1[i] != s2[i]) {
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

void clear(byte *ptr, unsigned int n) {
  int i;
  for (i = 0; i < n; i++) {
    ptr[i] = 0;
  }
}