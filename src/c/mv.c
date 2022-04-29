#include "header/std_type.h"
#include "header/filesystem.h"
#include "header/program.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"

void mv(char *src, char *dst, byte current_dir);
void mv_to_parent(char *src, char *dst, byte current_dir);
void mv_to_root(char *src, char *dst, byte current_dir);
void mv_to_child(char *src, char *dst, byte current_dir);

int main() {
    struct message msg;
    get_message(&msg);
    mv(msg.arg1, msg.arg2, msg.current_directory);
    puts("\r\n");
    exit();
}

void mv(char *src, char *dst, byte current_dir) {
  if (strlen(src) == 0) {
    puts("Source file name is empty\r\n");
    return;
  }

  if (strlen(dst) == 0) {
    puts("Destination file name is empty\r\n");
    return;
  } 

  if (startswith("/", dst)) {
    puts("Moving to root directory\r\n");
    mv_to_root(src, dst+1, current_dir);
  } else if (startswith("../", dst)) {
    puts("Moving to parent directory\r\n");
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
      puts("Source file not found\r\n");
      return;
    default:
      puts("Error while getting source file\r\n");
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
      puts("Moving to folder\r\n");
      dst_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        puts("Destination file already exists\r\n");
        return;
      }
      break;
    case FS_SUCCESS:
      puts("Destination file already exists\r\n");
      return;
    case FS_R_NODE_NOT_FOUND:
      break;
    default:
      puts("Error while getting destination file\r\n");
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
      puts("Source file not found\r\n");
      return;
    default:
      puts("Error reading source file\r\n");
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
      puts("Destination file already exists\r\n");
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
      puts("Source file not found\r\n");
      return;
    default:
      puts("Error reading source file\r\n");
      return;
  }

  node_buffer = node_fs_buffer.nodes[src_index];

  metadata.node_name = dst;
  metadata.parent_index = current_dir;

  switch (get_node(&metadata)) {
    case FS_R_TYPE_IS_FOLDER:
      puts("Moving to folder\r\n");
      node_buffer.parent_node_index = metadata.node_index;
      if (file_exists(src, metadata.node_index)) {
        puts("Destination file already exists\r\n");
        return;
      }
      break;
    case FS_SUCCESS:
      // File exists
      puts("Destination file already exists\r\n");
      break;
    case FS_R_NODE_NOT_FOUND:
      // File not found
      // Rename file
      puts("Renaming\r\n");
      clear(node_buffer.name, 14);
      strcpy(node_buffer.name, dst);
      break;
  }
  
  node_fs_buffer.nodes[src_index] = node_buffer;

  saveNode(&node_fs_buffer);
}
