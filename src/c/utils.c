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

void mv(char *src, char *dst, byte current_dir) {
  if (strlen(src) == 0) {
    printString("Source file name is empty\r\n");
    return;
  }

  if (strlen(dst) == 0) {
    printString("Destination file name is empty\r\n");
    return;
  } 

  if (startswith("/", dst)) {
    printString("Moving to root directory\r\n");
    mv_to_root(src, dst+1, current_dir);
  } else if (startswith("../", dst)) {
    printString("Moving to parent directory\r\n");
    mv_to_parent(src, dst+3, current_dir);
  } else {
    mv_to_child(src, dst, current_dir);
  } 
}

void mv_to_parent(char *src, char *dst, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  struct file_metadata metadata;
  int i, src_index, dst_index;

  loadNode(&node_fs_buffer);

  metadata.node_name = src;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
    case FS_SUCCESS:
      src_index = metadata.node_index;
      if (metadata.parent_index == FS_NODE_P_IDX_ROOT) {
        dst_index = FS_NODE_P_IDX_ROOT;
      } else {
        dst_index = node_fs_buffer.nodes[metadata.parent_index].parent_node_index;
      }
      break;
    case FS_R_NODE_NOT_FOUND:
      printString("Source file not found\r\n");
      return;
    default:
      printString("Error while getting source file\r\n");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];

  metadata.parent_index = dst_index;
  if (strlen(dst) != 0) {
    metadata.node_name = dst;
    strcpy(node_buffer.name, dst);
  }

  // Check if destination already exists
  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      printString("Moving to folder\r\n");
      dst_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        printString("Destination file already exists\r\n");
        return;
      }
      break;
    case FS_SUCCESS:
      printString("Destination file already exists\r\n");
      return;
    case FS_R_NODE_NOT_FOUND:
      break;
    default:
      printString("Error while getting destination file\r\n");
      return;
  }

  node_buffer.parent_node_index = dst_index;
  node_fs_buffer.nodes[src_index] = node_buffer;
  saveNode(&node_fs_buffer);

}

void mv_to_root(char *src, char *dst, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  struct file_metadata metadata;
  int i, src_index, dst_index;

  loadNode(&node_fs_buffer);

  metadata.node_name = src;
  metadata.parent_index = current_dir;
  
  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
    case FS_SUCCESS:
      src_index = metadata.node_index;
      break;
    case FS_R_NODE_NOT_FOUND:
      printString("Source file not found\r\n");
      return;
    default:
      printString("Error reading source file\r\n");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];
  dst_index = FS_NODE_P_IDX_ROOT;

  metadata.parent_index = dst_index;
  if (strlen(dst) != 0) {
    metadata.node_name = dst;
    strcpy(node_buffer.name, dst);
  }


  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      dst_index = metadata.node_index;
      break;
    case FS_SUCCESS:
      printString("Destination file already exists\r\n");
      return;
  }

  node_buffer.parent_node_index = dst_index;
  node_fs_buffer.nodes[src_index] = node_buffer;
  saveNode(&node_fs_buffer);
}

void mv_to_child(char *src, char *dst, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  struct file_metadata metadata;
  int i;
  int src_index, dst_index;

  loadNode(&node_fs_buffer);

  metadata.node_name = src;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_SUCCESS:
    case FS_R_TYPE_IS_FOLDER:
      src_index = metadata.node_index;
      break;
    case FS_R_NODE_NOT_FOUND:
      printString("Source file not found\r\n");
      return;
    default:
      printString("Error reading source file\r\n");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];

  metadata.node_name = dst;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      printString("Moving to folder\r\n");
      node_buffer.parent_node_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        printString("Destination file already exists\r\n");
        return;
      }
      break;
    case FS_SUCCESS:
      // File exists
      printString("Destination file already exists\r\n");
      break;
    case FS_R_NODE_NOT_FOUND:
      // File not found
      // Rename file
      printString("Renaming\r\n");
      clear(node_buffer.name, 14);
      strcpy(node_buffer.name, dst);
      break;
  }
  
  node_fs_buffer.nodes[src_index] = node_buffer;

  saveNode(&node_fs_buffer);
}

void cat(char *file_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  char buffer[8192];
  int i;

  metadata.filesize = 0;
  metadata.node_name = file_name;
  metadata.parent_index = current_dir;
  metadata.buffer = buffer;
  // printString("Reading file ");
  // printString(file_name);
  // printString("\r\n");
  read(&metadata, &return_code);
  // printString("Return code: ");
  // printHex(return_code);
  // printString("\r\n");

  for (i = 0; i < metadata.filesize; i++) {
    if (buffer[i] == '\0') {
      break;
    }
    if (buffer[i] == '\n') {
      printchar('\r');
    }
    printchar(buffer[i]);
  }
  // printString("file size: ");
  // printHex(metadata.filesize);
  printString("\r\n");
}

void ls(char *dir_name, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  int i = 0;

  printString("Listing directory ");
  printString(dir_name);
  printString(" (");
  printHex(current_dir);
  printString(")\r\n");

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
    return;
  }

  if (dir_name[0] == '/') {
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