#include "std_type.h"

struct message {
    byte current_directory;
    char arg1[64];
    char arg2[64];
    char arg3[64];
    byte cursor_x;
    byte cursor_y;
    int next_program_segment;
    byte other[315];
};

void get_message(struct message *msg); 

void exit();
void exec(struct file_metadata *metadata, int segment);

