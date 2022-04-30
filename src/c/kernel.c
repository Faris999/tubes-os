#include "header/kernel.h"
#include "header/std_type.h"

void printChar(char c);
void printCWD(char *path_str, byte current_directory);

int main() {
  struct file_metadata metadata;
  fillMap();
  clearScreen();
  makeInterrupt21();
  
  metadata.node_name = "shell";
  metadata.parent_index = 0;
  executeProgram(&metadata, 0x2000);
  // shell();
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
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
    case 0x6:
      executeProgram(BX, CX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

// TERMINAL

void printHex(int a) {
  int i;
  char buf[5];
  char *hex = "0123456789ABCDEF";
  printString("0x");
  for (i = 3; i >= 0; i--) {
    buf[i] = hex[a % 16];
    a /= 16;
  }
  buf[4] = '\0';
  printString(buf);
}

void printChar(char c) {
  interrupt(0x10, 0x0e00 + c, 0, 0, 0);
}

void printString(char *string) {
  int i;
  for (i = 0; i < strlen(string); i++) {
    printChar(string[i]);
  }
}

void readString(char *string) {
  int i = 0;
  while (i < 128) {
    byte AL = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    if (AL >= 0x20 && AL <= 0x7e) {
      string[i] = AL;
      printChar(AL);
      i++;
      continue;
    }
    if (AL == 0x8 && i > 0) {
      string[--i] = 0x0;
      printChar(0x8); 
      interrupt(0x10, 0x0a20, 0x0, 0x1, 0x0);
    }
    if (AL == 0xd) {
      printChar(0xd);
      printChar(0xa);
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
  interrupt(0x10, 0x0200, 0x03, 0x0, 0x0000); 
}

// FILESYSTEM

void writeSector(byte *buffer, int sector_number) {
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;
  cylinder = div(sector_number, 36) << 8;      // CH
  sector = mod(sector_number, 18) + 1;         // CL
  head = mod(div(sector_number, 18), 2) << 8;  // DH
  drive = 0x00;                                // DL
  interrupt(0x13,                              // Interrupt number
            0x0300 | sector_read_count,        // AX
            buffer,                            // BX
            cylinder | sector,                 // CX
            head | drive                       // DX
  );
}

void readSector(byte *buffer, int sector_number) {
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;
  cylinder = div(sector_number, 36) << 8;      // CH
  sector = mod(sector_number, 18) + 1;         // CL
  head = mod(div(sector_number, 18), 2) << 8;  // DH
  drive = 0x00;                                // DL
  interrupt(0x13,                              // Interrupt number
            0x0200 | sector_read_count,        // AX
            buffer,                            // BX
            cylinder | sector,                 // CX
            head | drive                       // DX
  );
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct map_filesystem map_fs_buffer;
  struct node_filesystem node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct node_entry node_buffer;
  struct sector_entry sector_entry_buffer;

  int i;
  int j;
  int written;
  int node_index = -1;
  int sector_index = -1;
  int empty_sector_count;

  // Load filesystems
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);

  // Find node
  for (i = 0; i < 64; i++) {
    if (strcmp(metadata->node_name, node_fs_buffer.nodes[i].name)) {
      if (node_fs_buffer.nodes[i].parent_node_index ==
          metadata->parent_index) {
        *return_code = FS_W_FILE_ALREADY_EXIST;
        return;
      }
    }
  }

  // Find free node
  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].name[0] == 0) {
      node_index = i;
      break;
    }
  }

  if (node_index == -1) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }

  // Check if parent is folder
  if (metadata->parent_index != FS_NODE_P_IDX_ROOT && 
      node_fs_buffer.nodes[metadata->parent_index].sector_entry_index != FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_W_INVALID_FOLDER;
    return;
  }

  // Check if filesize is valid
  if (metadata->filesize > 8192) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  empty_sector_count = 0;
  for (i = 0; i < 512; i++) {
    if (map_fs_buffer.is_filled[i] == 0) {
      empty_sector_count++;
    }
  }

  if (empty_sector_count < metadata->filesize / 512) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  // Find free sector
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
  node_buffer = node_fs_buffer.nodes[node_index];
  sector_entry_buffer = sector_fs_buffer.sector_list[sector_index];

  node_buffer.parent_node_index = metadata->parent_index;
  strcpy(node_buffer.name, metadata->node_name);

  // Check if writing a folder
  if (metadata->filesize == 0) {
    node_buffer.sector_entry_index = FS_NODE_S_IDX_FOLDER;
  } else {
    node_buffer.sector_entry_index = sector_index;

    // Write sectors
    j = 0;
    written = 0;
    for (i = 0; i < 256; i++) {
      if (map_fs_buffer.is_filled[i] == 1) {
        continue;
      }
      map_fs_buffer.is_filled[i] = 1;
      sector_entry_buffer.sector_numbers[j] = i;
      j++;
      writeSector(&(metadata->buffer[written]), i);
      written += 512;
      if (written >= metadata->filesize) {
        break;
      }
    }
  }

  // Write filesystems
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
	sector_fs_buffer.sector_list[sector_index] = sector_entry_buffer;
  writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  node_fs_buffer.nodes[node_index] = node_buffer;
  writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  *return_code = FS_SUCCESS;
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct node_entry node_buffer;
  struct sector_entry sector_entry_buffer;

  int i;

  *return_code = FS_UNKNOWN_ERROR;

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);

  // Find node
  for (i = 0; i < 64; i++) {
    if (strcmp(metadata->node_name, node_fs_buffer.nodes[i].name)) {
      if (node_fs_buffer.nodes[i].parent_node_index ==
          metadata->parent_index) {
        break;
      }
    }
  }

  if (i == 64) {
    *return_code = FS_R_NODE_NOT_FOUND;
    return;
  }

  // Check if node is folder
  if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_R_TYPE_IS_FOLDER;
    return;
  }

  // Read file
  node_buffer = node_fs_buffer.nodes[i];
  sector_entry_buffer = sector_fs_buffer.sector_list[node_buffer.sector_entry_index];

  for (i = 0; i < 16; i++) {
    if (sector_entry_buffer.sector_numbers[i] == 0) {
      break;
    }
    readSector(&(metadata->buffer[i * 512]), sector_entry_buffer.sector_numbers[i]);
    metadata->filesize += 512;
  }

  *return_code = FS_SUCCESS;
}

void fillMap() {
  struct map_filesystem map_fs_buffer;
  int i;

  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

  for (i = 0; i < 17; i++) {
    map_fs_buffer.is_filled[i] = 1;
  }

  for (i = 256; i < 512; i++) {
    map_fs_buffer.is_filled[i] = 1;
  }

  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
}

// PROGRAM

void executeProgram(struct file_metadata *metadata, int segment) {
  enum fs_retcode fs_ret;
  byte buf[8192];
  metadata->buffer = buf;
  read(metadata, &fs_ret);
  if (fs_ret == FS_SUCCESS) {
    int i = 0;
    for (i = 0; i < 8192; i++) {
      if (i < metadata->filesize)
        putInMemory(segment, i, metadata->buffer[i]);
      else
        putInMemory(segment, i, 0x00);
    }
    launchProgram(segment);
  } else
    printString("exec: file not found\r\n");
}

