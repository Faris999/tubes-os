// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut


#include "header/kernel.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/utils.h"

int main() {
  struct file_metadata metadata;

  fillMap();
  makeInterrupt21();
  clearScreen();
  
  metadata.parent_index = 0x00;
  metadata.node_name = "shell";
  executeProgram(&metadata, 0x2000);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  printString("interrupt21");
  switch (AX) {
    case 0x0:
      if (CX == 0x0) {
        printString(BX);
      } else {
        printStringColor(BX, CX);
      }
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
    case 0x6:
      executeProgram(BX, CX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void executeProgram(struct file_metadata *metadata, int segment) {
  enum fs_retcode return_code;
  byte buf[8192];
  int i;

  metadata->buffer = buf;
  read(metadata, &return_code);
  if (return_code == FS_SUCCESS) {
    for (i = 0; i < 8192; i++) {
      if (i < metadata->filesize) {
        putInMemory(segment, i, metadata->buffer[i]);
      } else {
        putInMemory(segment, i, 0x00);
      }
    }
    launchProgram(segment);
  } else {
    println("Error: Failed to read file");
  }
}
