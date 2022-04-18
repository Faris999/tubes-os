#include "header/filesystem.h"
#include "header/terminal.h"
#include "header/kernel.h"


void writeSector(byte *buffer, int sector_number) {
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8;
    sector = mod(sector_number, 18) + 1;

    head = mod(div(sector_number, 18), 2) << 8;
    drive = 0x00;

    interrupt(0x13,
            0x0200 | sector_read_count,
            buffer,
            cylinder | sector,
            head | drive);
}

void readSector(byte *buffer, int sector_number) {
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8;
    sector = mod(sector_number, 18) + 1;

    head = mod(div(sector_number, 18), 2) << 8;
    drive = 0x00;

    interrupt(0x13,
            0x0300 | sector_read_count,
            buffer,
            cylinder | sector,
            head | drive);
}

void fillMap() {
  struct map_filesystem map_fs_buffer;
  int i;
  char buf[128];
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

  for (i = 0; i < 16; i++) {
    map_fs_buffer.is_filled[i] = true;
  }

  for (i = 256; i < 511; i++) {
    map_fs_buffer.is_filled[i] = true;
  }
  printHex(&map_fs_buffer);

  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

  readString(buf);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {

}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
    
}