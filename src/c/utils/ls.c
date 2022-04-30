#include "../header/kernel.h"

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
    printString("Directory not found\r\n");
    return;
  }

  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].name[0] == '\0') {
      continue;
    }
    if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
      if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
        printString(node_fs_buffer.nodes[i].name);
        printString("/");
      } else {
        printString(node_fs_buffer.nodes[i].name);
      }
      printString(" (");
      printHex(i);
      printString(")\r\n");
    }
  }
}