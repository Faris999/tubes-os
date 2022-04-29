#include "header/filesystem.h"
#include "header/std_type.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"
#include "header/program.h"


void printCWD(char *path_str, byte current_dir); 
void execute(char *exec_name, byte current_dir, char *arguments[]);
void splitString(char* string, char* return_array[]);

int main() {
  char input_buf[64];
  char path_str[128];
  char *arguments[64];
  struct file_metadata metadata;
  struct message msg;
  byte current_dir;
  int i; 

  get_message(&msg);
  current_dir = msg.current_directory;

  

  // clearScreen();
  // setCursorPosition(0, 0);
  // log("from shell", 0x107);
  // syncCursorFromMessage();

  while (true) {
    puts("OS@IF2230:");
    printCWD(path_str, current_dir);
    puts("$ ");
    clear(input_buf, 64);
    gets(input_buf);
    clear(arguments, 64);
    splitString(input_buf, arguments);
    puts("arguments: ");
    for (i = 0; i < 3; i++) {
      puts(arguments[i]);
      puts(" ");
    }
    puts("\r\n");

    if (startswith("./", arguments[0])) {
      // execute local file
      execute(arguments[0] + 2, current_dir, arguments);
      continue;
    }

    execute(arguments[0], current_dir, arguments);

     /*else if (strcmp("mv", arguments[0])) {
      mv(arguments[1], arguments[2], current_dir);
    } else if (strcmp("cp", arguments[0])) {
      cp(arguments[1], arguments[2], current_dir);
    } else if (strcmp("clear", arguments[0])) {
      clearscreen();
    } else {
      puts("unknown command\r\n");
    } */
  }
}
void splitString(char* string, char* return_array[]) {
  int i;

  clear(return_array, 64);
  
  for (i = 0; i < 64; i++) {
    return_array[i] = string;
    while (*string != ' ' && *string != '\0') {
      string++;
    }
    *string = '\0';
    string++;
  }
}

void execute(char *exec_name, byte current_dir, char *arguments[]) {
  struct file_metadata metadata;
  struct message msg;

  metadata.node_name = exec_name;
  metadata.parent_index = 0x00; 

  // check if file exists
  if (get_node(&metadata) == FS_R_NODE_NOT_FOUND) {
    puts("Unknown command\r\n");
    return;
  }


  msg.current_directory = current_dir;
  strcpy(msg.arg1, arguments[1]);
  strcpy(msg.arg2, arguments[2]);
  strcpy(msg.arg3, arguments[3]);

  write_message(&msg);
  // syncCursorToMessage();
  exec(&metadata, 0x3000); 
}

void printCWD(char *path_str, byte current_dir) {
  struct node_filesystem node_fs_buffer;
  struct node_entry node_buffer;
  char path_array[64][14];
  int i = 63;
  
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  puts("/");

  while (current_dir != FS_NODE_P_IDX_ROOT) {
    node_buffer = node_fs_buffer.nodes[current_dir];
    strcpy(path_array[i], node_buffer.name);
    current_dir = node_buffer.parent_node_index;
    i--;
  }

  for (i = i + 1; i < 64; i++) {
    if (path_array[i][0] == 0) {
      continue;
    }
    puts(path_array[i]);
    puts("/");
  }
}