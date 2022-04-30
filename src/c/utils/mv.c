#include "../header/kernel.h"

void mv_to_parent(char *src, char *dst, byte current_dir);
void mv_to_root(char *src, char *dst, byte current_dir);
void mv_to_child(char *src, char *dst, byte current_dir);

void loadNode(struct node_filesystem *node_fs_buffer) {
  readSector(&(node_fs_buffer->nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer->nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}



void saveNode(struct node_filesystem *node_fs_buffer) {
  writeSector(&(node_fs_buffer->nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer->nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}



bool file_exists(char *filename, byte location) {
  struct file_metadata metadata;

  metadata.node_name = filename;
  metadata.parent_index = location;

  return get_node(&metadata) == FS_SUCCESS;
}

enum fs_retcode get_node(struct file_metadata *metadata) {
  struct node_filesystem node_fs_buffer;
  int i;

  loadNode(&node_fs_buffer);

  for (i = 0; i < 64; i++) {
    if (strcmp(node_fs_buffer.nodes[i].name, metadata->node_name)) {
      if (node_fs_buffer.nodes[i].parent_node_index == metadata->parent_index) {
        metadata->node_index = i;
        if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
          return FS_R_TYPE_IS_FOLDER;
        } else {
          return FS_SUCCESS;
        }
      }
    }
  }

  return FS_R_NODE_NOT_FOUND;
}

void mv(char *src, char *dst, byte current_dir) {
  if (strlen(src) == 0) {
    printString("Source file name is empty");
    return;
  }

  if (strlen(dst) == 0) {
    printString("Destination file name is empty");
    return;
  } 

  if (startswith("/", dst)) {
    printString("Moving to root directory");
    mv_to_root(src, dst+1, current_dir);
  } else if (startswith("../", dst)) {
    printString("Moving to parent directory");
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
      printString("Source file not found");
      return;
    default:
      printString("Error while getting source file");
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
      printString("Moving to folder");
      dst_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        printString("Destination file already exists");
        return;
      }
      break;
    case FS_SUCCESS:
      printString("Destination file already exists");
      return;
    case FS_R_NODE_NOT_FOUND:
      break;
    default:
      printString("Error while getting destination file");
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
      printString("Source file not found");
      return;
    default:
      printString("Error reading source file");
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
      printString("Destination file already exists");
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
      printString("Source file not found");
      return;
    default:
      printString("Error reading source file");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];

  metadata.node_name = dst;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      printString("Moving to folder");
      node_buffer.parent_node_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        printString("Destination file already exists");
        return;
      }
      break;
    case FS_SUCCESS:
      // File exists
      printString("Destination file already exists");
      break;
    case FS_R_NODE_NOT_FOUND:
      // File not found
      // Rename file
      printString("Renaming");
      clear(node_buffer.name, 14);
      strcpy(node_buffer.name, dst);
      break;
  }
  
  node_fs_buffer.nodes[src_index] = node_buffer;

  saveNode(&node_fs_buffer);
}