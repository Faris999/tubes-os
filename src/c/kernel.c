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
  
  // shell();
  strcpy(file_name, "test");
  for (i = 0; i < 63; i++) {
    // create folders with different names
    file_name[4] = '0' + i;
    createDir(file_name, FS_NODE_P_IDX_ROOT);
  }

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
  byte current_dir = FS_NODE_P_IDX_ROOT;

  printString("Current directory: ");
  printHex(current_dir);
  printString(" (");
  printHex(&current_dir);
  printString(")\r\n");

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$ ");
    readString(input_buf);
    

    if (strcmp(input_buf, "cd")) {
      printString("cd: Not implemented\r\n");
    } else if (startswith("mkdir", input_buf)) {
      //printString("createDir: \r\n");
      printString(input_buf);
      printString("\r\n");
      createDir(input_buf + 6, current_dir);
    } else {
      printString("Unknown command\r\n");
    }
  }
}
