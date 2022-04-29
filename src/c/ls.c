#include "header/std_type.h"
#include "header/filesystem.h"
#include "header/program.h"
#include "header/textio.h"
#include "header/fileio.h"

void ls(char *dir_name, byte current_dir);
void get_children(byte current_dir, struct node_entry *node_buffer); 

int main() {
    struct message msg;
    get_message(&msg);
    ls(msg.arg1, msg.current_directory);
    exit();
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
    puts("Directory not found\r\n");
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
    putsColor(node_buffer[i].name, color);
    puts("\r\n");
  }
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