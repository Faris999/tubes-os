#include "std_type.h"

void createDir(char *dir_name, byte current_dir);
void cd(char *dir_name, byte current_dir);
void ls(char *dir_name, byte current_dir);
void pwd(byte current_dir);
void mv(char *src, char *dst, byte current_dir);
void cp(char *src, char *dst, byte current_dir);