// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

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

  printString("Current directory: ");
  printHex(current_dir);
  printString("\r\n");

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$ ");
    readString(input_buf);
    splitString(input_buf, arguments);

    if (strcmp("cd", arguments[0])) {
      cd(arguments[1], &current_dir);
    } else if (startswith("mkdir", arguments[0])) {
      mkdir(arguments[1], current_dir);
    } else {
      printString("Unknown command\r\n");
    }
  }
}
