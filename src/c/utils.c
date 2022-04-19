#include "header/utils.h"
#include "header/terminal.h"
#include "header/filesystem.h"
#include "header/kernel.h"


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
  printString("/ (");
  printHex(current_dir);
  printString(") ");
}