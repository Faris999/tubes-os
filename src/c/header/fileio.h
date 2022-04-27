#include "std_type.h"

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);
void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);