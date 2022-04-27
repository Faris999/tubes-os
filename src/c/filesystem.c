#include "header/filesystem.h"
#include "header/string.h"
#include "header/kernel.h"
#include "header/terminal.h"



void loadFilesystems(struct node_filesystem *node_fs_buffer,
                     struct sector_filesystem *sector_fs_buffer) {
  loadNode(node_fs_buffer);
  loadSector(sector_fs_buffer);
}

void loadNode(struct node_filesystem *node_fs_buffer) {
  readSector(&(node_fs_buffer->nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer->nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

void loadSector(struct sector_filesystem *sector_fs_buffer) {
  readSector(sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
}

void saveNode(struct node_filesystem *node_fs_buffer) {
  writeSector(&(node_fs_buffer->nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer->nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

void saveSector(struct sector_filesystem *sector_fs_buffer) {
  writeSector(sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
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