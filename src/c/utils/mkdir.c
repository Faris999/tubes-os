#include "../header/kernel.h"

void mkdir(char *dir_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;

  metadata.filesize = 0;
  metadata.node_name = dir_name;
  metadata.parent_index = current_dir;
  write(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      printString("Directory created\r\n");
      break;
    case FS_W_FILE_ALREADY_EXIST:
      printString("Directory already exists\r\n");
      break;
    case FS_W_INVALID_FOLDER:
      printString("Invalid folder\r\n");
      break;
    case FS_W_MAXIMUM_NODE_ENTRY:
      printString("Maximum node entry reached\r\n");
      break;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
      printString("Maximum sector entry reached\r\n");
      break;
    default:
      printString("Unknown error\r\n");
      break;
  }
}