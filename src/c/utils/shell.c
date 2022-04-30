#include "../header/std_type.h"
#include "../header/string.h"
#include "../header/textio.h"
#include "../header/filesystem.h"
#include "../header/fileio.h"
#include "../header/program.h"

void printCWD(char *path_str, byte current_dir);
void splitArguments(char *command, char **program, char **arg1, char **arg2);
void splitMultiple(char *input_buf, char **command, char **rest);
void execute(char *program, char *arg1, char *arg2, byte current_dir);

int main() {
  struct message msg;
  char input_buf[64];
  char path_str[128];
  char *command;
  char *rest;
  char *program;
  char *arg1;
  char *arg2;
  byte current_dir;

  read_message(&msg);

  current_dir = msg.current_directory;

  while (true) {
    puts("OS@IF2230:");
    printCWD(path_str, current_dir);
    puts("$ ");
    clear(input_buf, 64);
    gets(input_buf);
    // splitMultiple(input_buf, &command, &rest);


    // puts("command: ");
    // puts(command);
    // puts("\r\n");
    // puts("rest: ");
    // puts(rest);
    // puts("\r\n");
    splitArguments(input_buf, &program, &arg1, &arg2);
    puts("program: ");
    puts(program);
    puts("\r\n");
    puts("arg1: ");
    puts(arg1);
    puts("\r\n");
    puts("arg2: ");
    puts(arg2);
    puts("\r\n");

    execute(program, arg1, arg2, current_dir);
    // if (strcmp("cd", program)) {
    //   cd(arg1, &current_dir);
    // } else if (strcmp("ls", program)) {
    //   ls(arg1, current_dir);
    // } else if (strcmp("mkdir", program)) {
    //   mkdir(arg1, current_dir);
    // } else if (strcmp("cat", program)) {
    //   cat(arg1, current_dir);
    // } else if (strcmp("cp", program)) {
    //   cp(arg1, arg2, current_dir);
    // } else if (strcmp("mv", program)) {
    //   mv(arg1, arg2, current_dir);
    // } else {
    //   puts("Unknown command\r\n");
    // }
  }
}

void loadNode(struct node_filesystem *node_fs_buffer) {
  readSector(&(node_fs_buffer->nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer->nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
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

void execute(char *program, char *arg1, char *arg2, byte current_dir) {
  struct file_metadata metadata; 
  struct message msg;

  metadata.node_name = program;
  metadata.parent_index = 0;
  
  if (startswith("./", program)) {
    metadata.parent_index = current_dir;
    metadata.node_name = &program[2];
  }

  // check if file exists
  if (get_node(&metadata) == FS_R_NODE_NOT_FOUND) {
    puts("Unknown command\r\n");
    return;
  }

  strcpy(msg.arg1, arg1);
  strcpy(msg.arg2, arg2);

  msg.current_directory = current_dir;
  msg.current_segment = 0x3000;

  write_message(&msg);

  exec(&metadata, 0x3000);
}

void printCWD(char *path_str, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  int i;
  byte index_path[32];

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  clear(path_str, 128);
  strcat(path_str, "/");

  for (i = 0; i < 32; i++) {
    index_path[i] = FS_NODE_P_IDX_ROOT;
  }

  while (current_dir != FS_NODE_P_IDX_ROOT) {
    node_buffer = node_fs_buffer.nodes[current_dir]; 
    index_path[--i] = current_dir;
    current_dir = node_buffer.parent_node_index;
  }

  for (i = 0; i < 32; i++) {
    if (index_path[i] == FS_NODE_P_IDX_ROOT) {
      continue;
    }
    node_buffer = node_fs_buffer.nodes[index_path[i]];
    strcat(path_str, node_buffer.name);
    strcat(path_str, "/");
  }

  puts(path_str);
}

void splitArguments(char *input_buf, char **program, char **arg1, char **arg2) {
  *program = input_buf;
  
  while (*input_buf != ' ' && *input_buf != '\0') {
    input_buf++;
  }
  *input_buf = '\0';
  input_buf++;

  *arg1 = input_buf;

  while (*input_buf != ' ' && *input_buf != '\0') {
    input_buf++;
  }
  *input_buf = '\0';
  input_buf++;

  *arg2 = input_buf;

  while (*input_buf != ' ' && *input_buf != '\0') {
    input_buf++;
  }
  *input_buf = '\0';
}

void splitMultiple(char *input_buf, char **command, char **rest) {
  *command = input_buf;
  
  while (true) { 
    if (startswith(" ; ", input_buf)) {
      *input_buf = '\0';
      input_buf += 3;
      *rest = input_buf;
      puts(*rest);
      return;
    }
    input_buf++;
  }
}