#include "header/program.h"
#include "header/filesystem.h"
#include "header/fileio.h"
#include "header/textio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void exec(struct file_metadata *metadata, int segment) {
    interrupt(0x21, 0x6, metadata, segment, 0);
}

void exit() {
    struct file_metadata metadata;
    struct message msg;
    char *command;
    char program[64];
    char *rest;
    int i;

    get_message(&msg);

    if (msg.other[0] == 0) {
        metadata.node_name = "shell";
        metadata.parent_index = 0x00;
        exec(&metadata, 0x2000);
        return;
    }
    
    // command = msg.other;
    // for (i = 0; i < 64; i++) {
    //   if (msg.other[i] == '\0') {
    //     rest = msg.other + i;
    //     break;
    //   }
    //   if (startswith(" ; ", msg.other + i)) {
    //     msg.other[i] = '\0';
    //     rest = msg.other + i + 3;
    //     break;
    //   }
    // }
    // puts("command: ");
    // puts(command);
    // puts("\r\n");

    // clear(msg.arg1);
    // clear(msg.arg2);
    // clear(msg.arg3);
    
    // i = 0;
    // while (command[0] != '\0' && command[0] != ' ') {
    //   program[i] = command[0];
    //   command++;
    //   i++;
    // }
    // program[i] = '\0';

    // if (command[0] == ' ') {
    //   command++;
    //   i = 0;
    //   while (command[0] != '\0' && command[0] != ' ') {
    //     msg.arg1[i] = command[0];
    //     command++;
    //     i++;
    //   }
    //   msg.arg1[i] = '\0';

    //   if (command[0] == ' ') {
    //     command++;
    //     i = 0;
    //     while (command[0] != '\0') {
    //       msg.arg2[i] = command[0];
    //       command++;
    //       i++;
    //     }
    //     msg.arg2[i] = '\0';
    //   }
    // }
    msg.next_program_segment = msg.next_program_segment + 0x1000;

    write_message(&msg);
    metadata.node_name = "shell";
    // puts("executing: ");
    // puts(program);
    // puts("\r\n");
    metadata.parent_index = 0x00;
    exec(&metadata, msg.next_program_segment - 0x1000);
}

void write_message(struct message *msg) {
    writeSector(msg, 0x104);
}

void get_message(struct message *msg) {
    readSector(msg, 0x104);
}