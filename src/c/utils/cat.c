#include "core.h"

void cat(char *file_name, byte current_dir);

int main() {
  struct message msg;

  read_message(&msg);
  cat(msg.arg1, msg.current_directory);
  puts("exiting\r\n");
  exit();
}

void cat(char *file_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  char buffer[8192];
  char string_buf[2];
  int i;

  metadata.filesize = 0;
  metadata.node_name = file_name;
  metadata.parent_index = current_dir;
  metadata.buffer = buffer;
  
  clear(buffer, 8192);

  read(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_R_NODE_NOT_FOUND:
      puts("File not found\r\n");
      return;
    case FS_R_TYPE_IS_FOLDER:
      puts("File is a folder\r\n");
      return;
    default:
      puts("Unknown error\r\n");
      return;
  }

  string_buf[1] = '\0';

  for (i = 0; i < metadata.filesize; i++) {
    if (buffer[i] == '\0') {
      break;
    }
    if (buffer[i] == '\n') {
      puts("\r");
    }
    string_buf[0] = buffer[i];
    puts(string_buf);
  }
  puts("\r\n"); 
}