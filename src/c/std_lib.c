#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C

int main() {

}

int div(int a, int b) {
  return a / b;
}

int mod(int a, int b) {
  return a % b;
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
