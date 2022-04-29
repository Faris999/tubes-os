#include "header/std_type.h"
#include "header/filesystem.h"
#include "header/program.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"

void cd(char *dir_name, byte *current_dir);

int main() {
    struct message msg;
    get_message(&msg);
    cd(msg.arg1, &msg.current_directory);
    write_message(&msg);
    exit();
}

void cd(char *dir_name, byte *current_dir) {
  struct node_filesystem node_fs_buffer;
  int i;

  if (dir_name[0] == 0) {
    puts("Invalid directory\r\n");
    return;
  }

  if (dir_name[0] == '/') {
    puts("Changing to root directory\r\n");
    *current_dir = FS_NODE_P_IDX_ROOT;
    return;
  }

  if (*current_dir == FS_NODE_P_IDX_ROOT && strcmp(dir_name, "..")) {
    puts("Already in root\r\n");
    return;
  }

  loadNode(&node_fs_buffer);
  
  if (strcmp(dir_name, "..")) { 
    puts("Changing to parent directory\r\n");
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

  puts("Directory not found\r\n");
}