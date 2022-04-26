// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut


#include "header/kernel.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/utils.h"

int main() {
  int i;
  char file_name[14];
  fillMap();
  makeInterrupt21();
  clearScreen();
  
  shell();
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  printString("interrupt21");
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      read(BX, CX);
      break;
    case 0x5:
      write(BX, CX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void shell() {
  char input_buf[64];
  char path_str[128];
  char *arguments[64];
  byte current_dir = FS_NODE_P_IDX_ROOT;
  int i;

  printString("Current directory: ");
  printHex(current_dir);
  printString("\r\n");

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$ ");
    clear(input_buf, 64);
    readString(input_buf);
    clear(arguments, 64);
    splitString(input_buf, arguments);
    printString("arguments: ");
    for (i = 0; i < 3; i++) {
      printString(arguments[i]);
      printString(" ");
    }
    printString("\r\n");

    if (strcmp("cd", arguments[0])) {
      cd(arguments[1], &current_dir);
    } else if (strcmp("mkdir", arguments[0])) {
      mkdir(arguments[1], current_dir);
    } else if (strcmp("ls", arguments[0])) { 
      ls(arguments[1], current_dir);
    } else if (strcmp("cat", arguments[0])) {
      cat(arguments[1], current_dir);
    } else if (strcmp("mv", arguments[0])) {
      mv(arguments[1], arguments[2], current_dir);
    } else if (strcmp("cp", arguments[0])) {
      printString("cp");
      cp(arguments[1], arguments[2], current_dir);
    } else if (strcmp("clear", arguments[0])) {
      clearScreen();
    } else {
      printString("Unknown command\r\n");
    }
  }
}
