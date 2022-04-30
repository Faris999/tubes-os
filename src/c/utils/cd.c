#include "core.h"

void cd(char *dir_name, byte *current_dir);

int main() {
  struct message msg;
  read_message(&msg);
  cd(msg.arg1, &msg.current_directory);
  write_message(&msg);
  puts("exiting\r\n");
  exit();
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

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  
  if (strcmp(dir_name, "..")) { 
    *current_dir = node_fs_buffer.nodes[*current_dir].parent_node_index;
    return;
  }
  
  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].parent_node_index == *current_dir) {
      if (strcmp(dir_name, node_fs_buffer.nodes[i].name)) {
        puts("move to ");
        puts(node_fs_buffer.nodes[i].name);
        puts("\r\n");
        *current_dir = i;
        return;
      }
    }
  }

  puts("Directory not found\r\n");
}