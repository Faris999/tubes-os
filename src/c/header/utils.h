#include "std_type.h"

void splitString(char* string, char* return_array[]);
void mkdir(char *dir_name, byte current_dir);
void cd(char *dir_name, byte *current_dir);
void ls(char *dir_name, byte current_dir);
void cat(char *file_name, byte current_dir);
void pwd(byte current_dir);
void mv(char *src, char *dst, byte current_dir);
void mv_to_root(char *src, char *dst, byte current_dir);
void mv_to_parent(char *src, char *dst, byte current_dir);
void mv_to_child(char *src, char *dst, byte current_dir);
void cp(char *src, char *dst, byte current_dir);