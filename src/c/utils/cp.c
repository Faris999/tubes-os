#include "core.h" 

void cp(char *src, char *dst, byte current_dir);

int main() {
  struct message msg;

  read_message(&msg);
  cp(msg.arg1, msg.arg2, msg.current_directory);
  puts("exiting\r\n");
  exit();
}

void cp(char *src, char *dst, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  byte buffer[8192];

  metadata.filesize = 0;
  metadata.node_name = src;
  metadata.parent_index = current_dir;
  metadata.buffer = buffer;

  // Get metadata of src
  read(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      metadata.node_name = dst;
      write(&metadata, &return_code);
      break;
    case FS_R_NODE_NOT_FOUND:
      puts("File not found\r\n");
      return;
    case FS_R_TYPE_IS_FOLDER:
      puts("Copying folder not supported\r\n");
      return;
    default:
      puts("Unnown error\r\n");
      return;
  }

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_W_FILE_ALREADY_EXIST:
      puts("File already exists\r\n");
      return;
    case FS_W_INVALID_FOLDER:
      puts("Invalid folder\r\n");
      return;
    case FS_W_MAXIMUM_NODE_ENTRY:
      puts("Maximum node entry reached\r\n");
      return;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      puts("Maximum sector entry reached\r\n");
      return;
    case FS_W_NOT_ENOUGH_STORAGE:
      puts("Not enough storage\r\n");
      return;
    default:
      puts("Unknown error");
      return;
  }
}