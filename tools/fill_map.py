import numpy as np
from filesystem import *

# Assumes CWD is the root of the project

def fill_map():
    map_data = read_sector(MAP_SECTOR_NUMBER)
    
    # Bootloader and kernel
    for i in range(17):
        map_data[i] = 1
    
    # Unreadable sectors
    for i in range(256, 512):
        map_data[i] = 1

    write_sector(map_data, MAP_SECTOR_NUMBER)

load_disk()
fill_map()
save_to_disk()