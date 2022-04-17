// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"
#include "header/std_lib.h"
#include "header/filesystem.h"

void printchar(char a);

int main() {
  char buf[128];
  fillMap();
  clearScreen();
  makeInterrupt21();

  printString("Halo dunia!\r\n");
  readString(buf);
  printString(buf);
  while (true);
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
    default:
      printString("Invalid interrupt");
  }
}

void printchar(char a) {
  interrupt(0x10, 0x0e00 + a, 0x0000, 0x0, 0x0);
}

void printString(char *string) {
  int i;
  for (i = 0; i < strlen(string); i++) {
    interrupt(0x10, 0x0e00 + string[i], 0x0000, 0x0, 0x0);
  }
}

void readString(char *string) {
  int i = 0;
  while (i < 128) {
    byte AL = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    if (AL >= 0x20 && AL <= 0x7e) {
      string[i] = AL;
      printchar(AL);
      i++;
      continue;
    }
    if (AL == 0x8 && i > 0) {
      string[--i] = 0x0;
      printchar(0x8);
      interrupt(0x10, 0x0a20, 0x0, 0x1, 0x0);
    }
    if (AL == 0xd) {
      printchar(0xd);
      printchar(0xa);
      break;
    }
  }
}

void clearScreen() {
  int i;
  int j;
  for (i = 0; i < 80; i++) {
    for (j = 0; j < 25; j++) {
      putInMemory(0xB000, 0x8000 + (80 * j + i) * 2, 0x20);
      putInMemory(0xB000, 0x8001 + (80 * j + i) * 2, 0xF);
    }
  }

  // clear screen
//  interrupt(0x10, 0x0600, 0x07, 0x0000, 0x1950);
  //interrupt(0x10, 0x0003, 0x00, 0x00);
  // move cursor to top left
  interrupt(0x10, 0x0200, 0x03, 0x0, 0x0000);
}

void readSector(byte *buffer, int sector_number) {
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;

  cylinder = div(sector_number, 36) << 8;
  sector = mod(sector_number, 18) + 1;

  head = mod(div(sector_number, 18), 2) << 8;
  drive = 0x00;

  interrupt(0x13,
            0x2000 | sector_read_count,
            buffer,
            cylinder | sector,
            head | drive);
}

void writeSector(byte *buffer, int sector_number) {
  int sector_write_count = 0x01;
  int cylinder, sector;
  int head, drive;

  cylinder = div(sector_number, 36) << 8;
  sector = mod(sector_number, 18) + 1;

  head = mod(div(sector_number, 18), 2) << 8;
  drive = 0x00;

  interrupt(0x13,
            0x3000 | sector_write_count,
            buffer,
            cylinder | sector,
            head | drive);
}

void fillMap() {
  struct map_filesystem map_fs_buffer;
  int i;

  // Load file system map
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

  // Fill map
  for (i = 0; i <= 15; i++) {
    map[i] = 1;
  }

  for (i = 256; i <= 511; i++) {
    map[i] = 1;
  }

  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct sector_entry sector_entry_buffer;

  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  memcpy(&node_buffer, &(node_fs_buffer.nodes[i]), sizeof(struct node_entry));
}
