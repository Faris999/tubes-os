#include "../header/kernel.h"

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
      printString("File not found\r\n");
      return;
    case FS_R_TYPE_IS_FOLDER:
      printString("Copying folder not supported\r\n");
      return;
    default:
      printString("Unnown error\r\n");
      return;
  }

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_W_FILE_ALREADY_EXIST:
      printString("File already exists\r\n");
      return;
    case FS_W_INVALID_FOLDER:
      printString("Invalid folder\r\n");
      return;
    case FS_W_MAXIMUM_NODE_ENTRY:
      printString("Maximum node entry reached\r\n");
      return;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      printString("Maximum sector entry reached\r\n");
      return;
    case FS_W_NOT_ENOUGH_STORAGE:
      printString("Not enough storage\r\n");
      return;
    default:
      printString("Unknown error");
      return;
  }
}