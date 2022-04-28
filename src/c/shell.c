#include "header/filesystem.h"
#include "header/std_type.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"
#include "header/program.h"


void printCWD(char *path_str, byte current_dir); 
void execute(char *exec_name, byte current_dir);

int main() {
  char input_buf[64];
  char path_str[128];
  char *arguments[64];
  struct file_metadata metadata;
  byte current_dir = 0xFF;
  int i; 

  

  // clearScreen();
  // setCursorPosition(0, 0);
  // log("from shell", 0x107);
  // syncCursorFromMessage();

  while (true) {
    puts("OS@IF2230:");
    printCWD(path_str, current_dir);
    puts("$ ");
    // clear(input_buf, 64);
    gets(input_buf);
    // clear(arguments, 64);
    // splitString(input_buf, arguments);
    // printString("arguments: ");
    // for (i = 0; i < 3; i++) {
    //   printString(arguments[i]);
    //   printString(" ");
    // }
    // println("");

    if (strcmp("ls", input_buf)) {
        putsColor("harusnya ini ls\r\n", 0x0A);
        execute("ls", current_dir);
        puts("balik lagi?");
    //   ls(arguments[1], current_dir);
    }/* else if (strcmp("mkdir", arguments[0])) {
      mkdir(arguments[1], current_dir);
    } else if (strcmp("ls", arguments[0])) { 
      ls(arguments[1], current_dir);
    } else if (strcmp("cat", arguments[0])) {
      cat(arguments[1], current_dir);
    } else if (strcmp("mv", arguments[0])) {
      mv(arguments[1], arguments[2], current_dir);
    } else if (strcmp("cp", arguments[0])) {
      cp(arguments[1], arguments[2], current_dir);
    } else if (strcmp("clear", arguments[0])) {
      clearscreen();
    } else {
      println("unknown command");
    } */
  }
}

void execute(char *exec_name, byte current_dir) {
  struct file_metadata metadata;
  struct message msg;

  metadata.node_name = exec_name;
  metadata.parent_index = 0x00; 
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