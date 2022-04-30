#include "core.h"

void ls(char *dir_name, byte current_dir);

int main() {
  struct message msg;

  read_message(&msg);

  // puts("arg1: ");
  // puts(msg.arg1);
  // puts("\r\n");
  // puts("current_directory: ");
  // putsHex(msg.current_directory);
  // puts("\r\n");
  ls(msg.arg1, msg.current_directory);
  puts("exiting\r\n");
  exit();
}

void ls(char *dir_name, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  int i = 0; 

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  if (dir_name[0] != '\0') {
    for (i = 0; i < 64; i++) {
      if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
        if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
          if (strcmp(dir_name, node_fs_buffer.nodes[i].name)) {
            current_dir = i;
            break;
          }
        }
      }
    }
  }

  if (i == 64) {
    puts("Directory not found\r\n");
    return;
  }

  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].name[0] == '\0') {
      continue;
    }
    if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
      if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
        puts(node_fs_buffer.nodes[i].name);
        puts("/");
      } else {
        puts(node_fs_buffer.nodes[i].name);
      }
      puts(" (");
      puts(")\r\n");
    }
  }
}