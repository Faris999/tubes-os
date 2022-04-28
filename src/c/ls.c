#include "header/std_type.h"
#include "header/filesystem.h"
#include "header/program.h"
#include "header/textio.h"
#include "header/fileio.h"

int main() {
    // struct message msg;
    // get_message(&msg);
    char buf[64];
    // char *buffer = "from ls";

    // writeSector(buffer, 0x107);
    
    // setCursorPosition(0, 4);
    log("from ls", 0x107);
    putsColor("Welcome from ls!\r\n", 0x0B);
        // puts("Press enter to continue...");
        gets(buf);
        // clearScreen();
    // while (true);
    exit();
}

// void ls(char *dir_name, byte current_dir) {
//   struct file_metadata metadata;
//   struct node_entry node_buffer[64];
//   enum fs_retcode return_code;
//   byte color;
//   int i = 0; 

//   if (dir_name[0] != '\0') {
//     metadata.parent_index = current_dir;
//     metadata.node_name = dir_name;
//     get_node(&metadata);
//     current_dir = metadata.node_index;
//   }

//   if (i == 64) {
//     println("Directory not found");
//     return;
//   }

//   get_children(current_dir, node_buffer);

//   for (i = 0; i < 64; i++) {
//     if (node_buffer[i].name[0] == '\0') {
//       break;
//     }
//     if (node_buffer[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
//       color = 0x0B;
//     } else {
//       color = 0x0F;
//     }
//     printStringColor(node_buffer[i].name, color);
//     println("");
//   }
// }