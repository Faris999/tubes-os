#include "std_type.h"

struct message {
    char arg1[64];
    char arg2[64];
    byte current_directory;
    int current_segment;
    byte other[381];
};

void read_message(struct message *msg);
void write_message(struct message *msg);

void exec(struct file_metadata *metadata, int segment);

void exit();