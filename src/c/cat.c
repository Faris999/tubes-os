#include "header/std_type.h"
#include "header/filesystem.h"
#include "header/program.h"
#include "header/textio.h"
#include "header/fileio.h"
#include "header/string.h"

void cat(char *dir_name, byte current_dir);

int main() {
    struct message msg;
    get_message(&msg);
    cat(msg.arg1, msg.current_directory);
    puts("\r\n");
    exit();
}

void cat(char *file_name, byte current_dir) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  char buffer[8192];
  char current_char[2];
  int i;

  metadata.filesize = 0;
  metadata.node_name = file_name;
  metadata.parent_index = current_dir;
  metadata.buffer = buffer;
  
  read(&metadata, &return_code);

  switch (return_code) {
    case FS_SUCCESS:
      break;
    case FS_R_NODE_NOT_FOUND:
      puts("File not found");
      return;
    case FS_R_TYPE_IS_FOLDER:
      puts("File is a folder");
      return;
    default:
      puts("Unknown error");
      return;
  }

  current_char[1] = '\0';

  for (i = 0; i < metadata.filesize; i++) {
    if (buffer[i] == '\0') {
      break;
    }
    if (buffer[i] == '\n') {
      puts("\r");
    }
    current_char[0] = buffer[i];
    puts(current_char);
  }
  puts("");
}
