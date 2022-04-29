#include "header/filesystem.h"
#include "header/std_type.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"
#include "header/program.h"


void printCWD(char *path_str, byte current_dir); 
void execute(char *exec_name, byte current_dir, char *arg1, char *arg2, char *rest);
void splitString(char* string, char *return_array[]);

int main() {
  char input_buf[256];
  char path_str[128];
  char arg1[64];
  char arg2[64];
  char arg3[64];
  char *command;
  char *rest;
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
    clear(input_buf, 256);
    gets(input_buf);
    

    command = input_buf;
    for (i = 0; i < 64; i++) {
      if (input_buf[i] == '\0') {
        rest = input_buf + i;
        break;
      }
      if (startswith(" ; ", input_buf + i)) {
        input_buf[i] = '\0';
        rest = input_buf + i + 3;
        break;
      }
    }

    // puts("input_buf: ");
    // puts(input_buf);
    // puts("\r\n");
    // puts("command: ");
    // puts(command);
    // puts("\r\n");
    // puts("rest: ");
    // puts(rest);
    // puts("\r\n");
    
    clear(arg1, 64);
    clear(arg2, 64);
    clear(arg3, 64);

    i = 0;
    while (command[0] != '\0' && command[0] != ' ') {
      arg1[i] = command[0];
      command++;
      i++;
    }
    arg1[i] = '\0';

    if (command[0] == ' ') {
      command++;
      i = 0;
      while (command[0] != '\0' && command[0] != ' ') {
        arg2[i] = command[0];
        command++;
        i++;
      }
      arg2[i] = '\0';

      if (command[0] == ' ') {
        command++;
        i = 0;
        while (command[0] != '\0') {
          arg3[i] = command[0];
          command++;
          i++;
        }
        arg3[i] = '\0';
      }
    }

    // puts("arg1: ");
    // puts(arg1);
    // puts("\r\n");
    // puts("arg2: ");
    // puts(arg2);
    // puts("\r\n");
    // puts("arg3: ");
    // puts(arg3);
    // puts("\r\n");
    
    execute(arg1, current_dir, arg2, arg3, rest);

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
void splitString(char* string, char *return_array[]) {
  int i = 0;
  char *empty = "";
  for (i = 0; i < 3; i++) {
    return_array[i] = 0;
  }

  while (*string != '\0') {
    puts("a");
    return_array[i] = string;
    puts("b");
    while (*string != ' ' && *string != '\0') {
      string++;
    }
    if (*string == ' ') {
      *string = '\0';
      string++;
    }
    i++;
  }
  puts("return_array: ");
  puts(return_array[0]);
}

void execute(char *exec_name, byte current_dir, char *arg1, char *arg2, char *rest) {
  struct file_metadata metadata;
  struct message msg;

  metadata.node_name = exec_name;
  metadata.parent_index = 0x00; 

  if (startswith("./", exec_name)) {
    metadata.parent_index = current_dir;
    metadata.node_name = &exec_name[2];
  }

  // check if file exists
  if (get_node(&metadata) == FS_R_NODE_NOT_FOUND) {
    puts("Unknown command\r\n");
    return;
  }


  msg.current_directory = current_dir;
  strcpy(msg.arg1, arg1);
  strcpy(msg.arg2, arg2);
  // strcpy(msg.arg3, arguments[3]);
  strcpy(msg.other, rest);
  msg.next_program_segment = 0x4000;
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