// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"
#include "header/std_lib.h"
#include "header/filesystem.h"

char hex[17] = "0123456789ABCDEF";

void printchar(char a);
void printHex(int a);
void readSector(byte *buffer, int sector_number);
void writeSector(byte *buffer, int sector_number);
void fillMap();
void read(struct file_metadata *metadata, enum fs_retcode *return_code);
void write(struct file_metadata* metadata, enum fs_retcode *return_code);
void printCWD(char *path_str, byte current_dir);
void mkdir(char *dir_name, byte current_dir);
void shell();



int main() {
  fillMap();
  clearScreen();
  makeInterrupt21();
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

void printchar(char a) {
  interrupt(0x10, 0x0e00 + a, 0x0000, 0x0, 0x0);
}

void printHex(int a) {
  int i;
  char buf[5];
  printString("0x");
  for (i = 3; i >= 0; i--) {
    buf[i] = hex[a % 16];
    a /= 16;
  }
  buf[4] = '\0';
  printString(buf);
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
      string[i] = '\0';
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
            0x0200 | sector_read_count,
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
            0x0300 | sector_write_count,
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
    map_fs_buffer.is_filled[i] = true;
  }

  for (i = 256; i <= 511; i++) {
    map_fs_buffer.is_filled[i] = true;
  }

  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
}


void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct sector_entry sector_entry_buffer;

  int i;
  unsigned int filesize = 0;
  bool found = false;
  *return_code = FS_UNKNOWN_ERROR;

  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Find node
  for (i = 0; i < 64; i++) {
    if (strcmp(node_fs_buffer.nodes[i].name, metadata->node_name) &&
        node_fs_buffer.nodes[i].parent_node_index == metadata->parent_index) {
      found = true;
      break;
    }
  }

  if (!found) {
    *return_code = FS_R_NODE_NOT_FOUND;
    return;
  }

  // Check if node is a file
  if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_R_TYPE_IS_FOLDER;
    return;
  }

  memcpy(&node_buffer, &(node_fs_buffer.nodes[i]), sizeof(struct node_entry));
  memcpy(&sector_entry_buffer, &(sector_fs_buffer.sector_list[node_buffer.sector_entry_index]), sizeof(struct sector_entry));

  // Read file
  for (i = 0; i < 16; i++) {
    if (sector_entry_buffer.sector_numbers[i] == 0) {
      break;
    }
    // Read sector with offset
    readSector(metadata->buffer + 512 * i, sector_entry_buffer.sector_numbers[i]);
    filesize += 512;
  }

  *return_code = FS_SUCCESS;
  metadata->filesize = filesize;
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct map_filesystem map_fs_buffer;
  struct node_entry node_buffer;
  struct sector_entry sector_entry_buffer;

  int i;
  int j;
  int written;
  int node_index = -1;
  int sector_index = -1;
  int filesize;
  int empty_sector_count = 0;

  // Load filesystems
  readSector(map_fs_buffer.is_filled, FS_MAP_SECTOR_NUMBER);
  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Find node
  for (i = 0; i < 64; i++) {
    if (strcmp(node_fs_buffer.nodes[i].name, metadata->node_name) &&
        node_fs_buffer.nodes[i].parent_node_index == metadata->parent_index) {
      *return_code = FS_W_FILE_ALREADY_EXIST;
      break;
    }
  }
  printString("File doesn't exist\r\n");

  // Find empty node
  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].name[0] == '\0') {
      node_index = i;
      break;
    }
  }

  if (node_index == -1) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }

  printString("Node index: ");
  printHex(node_index);
  printString("\r\n");
  
  printString("Parent index: ");
  printHex(metadata->parent_index);
  printString("\r\n");
  // Check if parent node is a folder
  if (metadata->parent_index != 0xff && node_fs_buffer.nodes[metadata->parent_index].sector_entry_index != FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_W_INVALID_FOLDER;
    return;
  }

  printString("Parent is a folder\r\n");
  

  // Check if file size is valid
  filesize = metadata->filesize;

  if (filesize > 8192) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  for (i = 0; i < 512; i++) {
    if (!map_fs_buffer.is_filled[i]) {
      empty_sector_count++;
    }
  }

  if (empty_sector_count < filesize / 512) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  // Find empty sectors
  for (i = 0; i < 32; i++) {
    if (sector_fs_buffer.sector_list[i].sector_numbers[0] == 0) {
      sector_index = i;
      break;
    }
  }

  if (sector_index == -1) {
    *return_code = FS_W_MAXIMUM_SECTOR_ENTRY;
    return;
  }

  // Write file
  memcpy(&node_buffer, &(node_fs_buffer.nodes[node_index]), sizeof(struct node_entry));
  memcpy(&sector_entry_buffer, &(sector_fs_buffer.sector_list[sector_index]), sizeof(struct sector_entry));

  node_buffer.parent_node_index = metadata->parent_index;
  strcpy(node_buffer.name, metadata->node_name);

  // Check if writing folder
  if (filesize == 0) {
    node_buffer.sector_entry_index = FS_NODE_S_IDX_FOLDER;
  } else {
    node_buffer.sector_entry_index = sector_index;
    j = 0;
    written = 0;
    for (i = 0; i < 256; i++) {
      if (map_fs_buffer.is_filled[i]) {
        continue;
      }
      map_fs_buffer.is_filled[i] = true;
      sector_entry_buffer.sector_numbers[j] = i;
      j += 1;
      writeSector(metadata->buffer + 512 * written, i);
      written += 512;
      if (written >= filesize) {
        break;
      }
    }
  }
  
  

  // Write sector
  memcpy(&(sector_fs_buffer.sector_list[sector_index]), &sector_entry_buffer, sizeof(struct sector_entry));
  memcpy(&(node_fs_buffer.nodes[node_index]), &node_buffer, sizeof(struct node_entry));
  writeSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  writeSector(map_fs_buffer.is_filled, FS_MAP_SECTOR_NUMBER);

  *return_code = FS_SUCCESS;
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
      //printString("mkdir: \r\n");
      mkdir(input_buf + 6, current_dir);
    } else {
      printString("Unknown command\r\n");
    }
  }
}

void mkdir(char *dir_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;

  strcpy(metadata.node_name, dir_name);
  metadata.parent_index = current_dir;
  metadata.filesize = 0;
  write(&metadata, &return_code);
  printString("Return code: ");
  printHex(return_code);
  printString("\r\n");
}

void printCWD(char *path_str, byte current_dir) {
  printString("/ (");
  printHex(current_dir);
  printString(") ");
}