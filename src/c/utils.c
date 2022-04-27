#include "header/utils.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/kernel.h"

char *UNKNOWN_ERROR = "Unknown error";

void cp(char *src, char *dst, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  byte buffer[8192];

  metadata.filesize = 0;
  metadata.node_name = src;
  metadata.parent_index = current_dir;
  metadata.buffer = buffer;

  // Get metadata of src
  read(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      metadata.node_name = dst;
      write(&metadata, &return_code);
      break;
    case FS_R_NODE_NOT_FOUND:
      println("File not found");
      return;
    case FS_R_TYPE_IS_FOLDER:
      println("Copying folder not supported");
      return;
    default:
      printString(UNKNOWN_ERROR);
      return;
  }

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_W_FILE_ALREADY_EXIST:
      println("File already exists");
      return;
    case FS_W_INVALID_FOLDER:
      println("Invalid folder");
      return;
    case FS_W_MAXIMUM_NODE_ENTRY:
      println("Maximum node entry reached");
      return;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      println("Maximum sector entry reached");
      return;
    case FS_W_NOT_ENOUGH_STORAGE:
      println("Not enough storage");
      return;
    default:
      printString(UNKNOWN_ERROR);
      return;
  }
}

void mkdir(char* dir_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;

  metadata.filesize = 0;
  metadata.node_name = dir_name;
  metadata.parent_index = current_dir;
  write(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      println("Directory created");
      break;
    case FS_W_FILE_ALREADY_EXIST:
      println("Directory already exists");
      break;
    case FS_W_INVALID_FOLDER:
      println("Invalid folder");
      break;
    case FS_W_MAXIMUM_NODE_ENTRY:
      println("Maximum node entry reached");
      break;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      println("Maximum sector entry reached");
      break;
    default:
      printString(UNKNOWN_ERROR);
      break;
  }
}

void mv(char *src, char *dst, byte current_dir) {
  if (strlen(src) == 0) {
    println("Source file name is empty");
    return;
  }

  if (strlen(dst) == 0) {
    println("Destination file name is empty");
    return;
  } 

  if (startswith("/", dst)) {
    println("Moving to root directory");
    mv_to_root(src, dst+1, current_dir);
  } else if (startswith("../", dst)) {
    println("Moving to parent directory");
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
      println("Source file not found");
      return;
    default:
      println("Error while getting source file");
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
      println("Moving to folder");
      dst_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        println("Destination file already exists");
        return;
      }
      break;
    case FS_SUCCESS:
      println("Destination file already exists");
      return;
    case FS_R_NODE_NOT_FOUND:
      break;
    default:
      println("Error while getting destination file");
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
      println("Source file not found");
      return;
    default:
      println("Error reading source file");
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
      println("Destination file already exists");
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
      println("Source file not found");
      return;
    default:
      println("Error reading source file");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];

  metadata.node_name = dst;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      println("Moving to folder");
      node_buffer.parent_node_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        println("Destination file already exists");
        return;
      }
      break;
    case FS_SUCCESS:
      // File exists
      println("Destination file already exists");
      break;
    case FS_R_NODE_NOT_FOUND:
      // File not found
      // Rename file
      println("Renaming");
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
  
  read(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_R_NODE_NOT_FOUND:
      println("File not found");
      return;
    case FS_R_TYPE_IS_FOLDER:
      println("File is a folder");
      return;
    default:
      println("Unknown error");
      return;
  }

  for (i = 0; i < metadata.filesize; i++) {
    if (buffer[i] == '\0') {
      break;
    }
    if (buffer[i] == '\n') {
      printchar('\r');
    }
    printchar(buffer[i]);
  }
  println("");
}

void get_children(byte current_dir, struct node_entry *node_buffer) {
  struct node_filesystem node_fs_buffer;
  int i;
  int j = 0;

  loadNode(&node_fs_buffer);

  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
      if (node_fs_buffer.nodes[i].name[0] != '\0') {
        node_buffer[j] = node_fs_buffer.nodes[i];
        j++;
      }
    }
  }

  node_buffer[j].name[0] = '\0';
}

void ls(char *dir_name, byte current_dir) {
  struct file_metadata metadata;
  struct node_entry node_buffer[64];
  enum fs_retcode return_code;
  byte color;
  int i = 0; 

  if (dir_name[0] != '\0') {
    metadata.parent_index = current_dir;
    metadata.node_name = dir_name;
    get_node(&metadata);
    current_dir = metadata.node_index;
  }

  if (i == 64) {
    println("Directory not found");
    return;
  }

  get_children(current_dir, node_buffer);

  for (i = 0; i < 64; i++) {
    if (node_buffer[i].name[0] == '\0') {
      break;
    }
    if (node_buffer[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
      color = 0x0B;
    } else {
      color = 0x0F;
    }
    printStringColor(node_buffer[i].name, color);
    println("");
  }
}

void cd(char *dir_name, byte *current_dir) {
  struct node_filesystem node_fs_buffer;
  int i;

  if (dir_name[0] == 0) {
    return;
  }

  if (dir_name[0] == '/') {
    *current_dir = FS_NODE_P_IDX_ROOT;
    return;
  }

  if (*current_dir == FS_NODE_P_IDX_ROOT && strcmp(dir_name, "..")) {
    return;
  }

  loadNode(&node_fs_buffer);
  
  if (strcmp(dir_name, "..")) { 
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

  println("Directory not found");
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

void printCWD(char *path_str, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  char path_array[64][14];
  int i = 63;
  
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  printHex(current_dir);
  printString("/");

  while (current_dir != FS_NODE_P_IDX_ROOT) {
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