#include "core.h"

void mkdir(char *dir_name, byte current_dir);

int main() {
  struct message msg;

  read_message(&msg);
  mkdir(msg.arg1, msg.current_directory);
  puts("exiting\r\n");
  exit();
}

void mkdir(char *dir_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;

  metadata.filesize = 0;
  metadata.node_name = dir_name;
  metadata.parent_index = current_dir;
  write(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      puts("Directory created\r\n");
      break;
    case FS_W_FILE_ALREADY_EXIST:
      puts("Directory already exists\r\n");
      break;
    case FS_W_INVALID_FOLDER:
      puts("Invalid folder\r\n");
      break;
    case FS_W_MAXIMUM_NODE_ENTRY:
      puts("Maximum node entry reached\r\n");
      break;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      puts("Maximum sector entry reached\r\n");
      break;
    default:
      puts("Unknown error\r\n");
      break;
  }
}