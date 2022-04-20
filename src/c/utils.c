#include "header/utils.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/kernel.h"

void mkdir(char* dir_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;

  metadata.filesize = 0;
  metadata.node_name = dir_name;
  metadata.parent_index = current_dir;
  printString("Creating directory ");
  printString(dir_name);
  printString("\r\n");
  write(&metadata, &return_code);
  printString("Return code: ");
  printHex(return_code);
  printString("\r\n");
}

void ls(char *dir_name, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  int i;

  printString("Listing directory ");
  printString(dir_name);
  printString("\r\n");

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  if (dir_name[0] != '\0') {
    for (i = 0; i < 64; i++) {
      if (strcmp(dir_name, node_fs_buffer.nodes[i].name)) {
        if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
          if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
            current_dir = i;
            break;
          }
        } else {
          printString("Not a directory\r\n");
          return;
        }
      }
    }
  }

  if (i == 64) {
    printString("Directory not found\r\n");
    return;
  }

  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
      if (node_fs_buffer.nodes[i].name[0] != '\0') {
        printString(node_fs_buffer.nodes[i].name);
        printString("\r\n");
      }
    }
  }
}

void cd(char *dir_name, byte *current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  int i;

  if (dir_name[0] == 0) {
    *current_dir = FS_NODE_P_IDX_ROOT;
    return;
  }

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  
  if (strcmp(dir_name, "..")) {
    if (*current_dir == FS_NODE_P_IDX_ROOT) {
      return;
    }
    *current_dir = node_fs_buffer.nodes[*current_dir].parent_node_index;
    return;
  }
  
  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].parent_node_index == *current_dir) {
      if (strcmp(dir_name, node_fs_buffer.nodes[i].name)) {
        *current_dir = i;
        return;
      }
    }
  }

  if (i == 64) {
    printString("Directory not found\r\n");
    return;
  }

}

// Create a function that will split a string into an array of strings
// by space.
void splitString(char* string, char* return_array[]) {
  int i;

  clear(return_array, 64);
  
  for (i = 0; i < 64; i++) {
    return_array[i] = string;
    while (*string != ' ' && *string != '\0') {
      string++;
    }
    *string = '\0';
    string++;
  }
}

void createDir(char *dir_name, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  int i;

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  for (i = 0; i < 64; i++) {
    if (strcmp(dir_name, node_fs_buffer.nodes[i].name)) {
      if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
        printString("Directory already exists\r\n");
        return;
      }
    }
  }

  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].name[0] == 0) {
      break;
    }
  }

  if (i == 64) {
    printString("No more space for new directory\r\n");
    return;
  }

  node_buffer = node_fs_buffer.nodes[i];
  node_buffer.parent_node_index = current_dir;
  node_buffer.sector_entry_index = FS_NODE_S_IDX_FOLDER;
  strcpy(node_buffer.name, dir_name);
  node_fs_buffer.nodes[i] = node_buffer;

  writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1); 
}

void printCWD(char *path_str, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  char path_array[64][14];
  int i = 63;

  printHex(current_dir);
  printString("/");

  while (current_dir != FS_NODE_P_IDX_ROOT) {
    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
    node_buffer = node_fs_buffer.nodes[current_dir];
    strcpy(path_array[i], node_buffer.name);
    current_dir = node_buffer.parent_node_index;
    i--;
  }

  for (i = i + 1; i < 64; i++) {
    if (path_array[i][0] == 0) {
      continue;
    }
    printString(path_array[i]);
    printString("/");
  }
}