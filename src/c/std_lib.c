#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C
int div(int a, int b) {
    return a / b;
}

int mod(int a, int n) {
    while (a >= n) {
        a -= n;
    }
    return a;
}


// Operasi standar bahasa C
// Implementasikan
void memcpy(byte *dest, byte *src, unsigned int n) {
    int i;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
// Mengcopy n bytes yang ditunjuk src ke dest

unsigned int strlen(char *string) {
    int i;
    for (i = 0; string[i] != '\0'; i++);
    return i;
}
// Mengembalikan panjang suatu null terminated string

void strcat(char *dest, char *src) {
  int i, j;

  for (i = 0; dest[i] != '\0'; i++) {
    ;
  }

  for (j = 0; src[j] != '\0'; j++) {
    dest[i + j] = src[j];
  }

  dest[i + j] = '\0';
}

bool startswith(char *prefix, char *string) {
    int i;
    for (i = 0; prefix[i] != '\0'; i++) {
        if (string[i] != prefix[i]) {
            return false;
        }
    }
    return true;
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
// Mengembalikan true jika string sama

void strcpy(char *dst, char *src) {
    int i;
    for (i = 0; src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}
// Melakukan penyalinan null terminated string

void clear(byte *ptr, unsigned int n) {
    int i;
    for (i = 0; i < n; i++) {
        ptr[i] = 0;
    }
}