// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut


#include "header/kernel.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/utils.h"
#include "header/program.h"

int main() {
  struct file_metadata metadata;
  struct message msg;

  fillMap();
  makeInterrupt21();
  clearScreen();
  
  metadata.parent_index = 0x00;
  metadata.node_name = "shell";
  // msg.cursor_x = 0;
  // msg.cursor_y = 0;
  msg.current_directory = 0xFF;
  writeSector(&msg, 0x104);
  executeProgram(&metadata, 0x2000);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  printString("interrupt21");
  switch (AX) {
    case 0x0:
      // if (CX == 0x0) {
        printString(BX);
      // } else {
        // printStringColor(BX, CX);
      // }
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
    case 0x7:
      clearScreen();
    case 0x8:
      setCursorPosition(BX, CX);
    case 0x9:
      log(BX, CX);
    case 0xa:
      getCursorPosition(BX, CX);
    default:
      printString("Invalid interrupt");
  }
}

void log(char *message, int sector) {
  char buffer[512];
  int length = strlen(message) + 1;
  int i, j;
  readSector(buffer, sector);

  for (i = 0; i < 512; i++) {
    if (buffer[i] == '\0') {
      if (i + length > 512) {
        // shift all data to the right
        for (j = length; j < 512; j++) {
          buffer[j - length] = buffer[j];
        }
      }
      for (j = 0; j < length-1; j++) {
        buffer[i + j] = message[j];
      }
      buffer[i + length - 1] = '\n';
      buffer[i + length] = '\0';
      writeSector(buffer, sector);
      return;
    }
  }
}

void executeProgram(struct file_metadata *metadata, int segment) {
  enum fs_retcode return_code;
  byte buf[8192];
  int i;

  metadata->buffer = buf;
  read(metadata, &return_code);
  if (return_code == FS_SUCCESS) {
    // log(metadata->node_name, 0x107);
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

void readSector(byte *buffer, int sector_number) {
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;

  cylinder = div(sector_number, 36) << 8;
  sector = mod(sector_number, 18) + 1;

  head = mod(div(sector_number, 18), 2) << 8;
  drive = 0x00;

  interrupt(0x13, 0x0200 | sector_read_count, buffer, cylinder | sector,
            head | drive);
}

void writeSector(byte *buffer, int sector_number) {
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;

  cylinder = div(sector_number, 36) << 8;
  sector = mod(sector_number, 18) + 1;

  head = mod(div(sector_number, 18), 2) << 8;
  drive = 0x00;

  interrupt(0x13, 0x0300 | sector_read_count, buffer, cylinder | sector,
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
  struct sector_filesystem sector_fs_buffer;
  struct node_entry node_buffer;
  struct sector_entry sector_entry_buffer;

  int i;
  bool found;

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);

  // Find node
  found = false;
  for (i = 0; i < 64; i++) {
    if (strcmp(metadata->node_name, node_fs_buffer.nodes[i].name)) {
      if (node_fs_buffer.nodes[i].parent_node_index ==
          metadata->parent_index) {
        found = true;
        break;
      }
    }
  }

  if (!found) {
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