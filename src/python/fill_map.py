import numpy as np

# Assumes CWD is the root of the project

MAP_SECTOR_NUMBER = 0x100

disk = np.zeros((1474560,), dtype=np.uint8)

def save_to_disk():
    with open('out/system.img', 'wb') as f:
        f.write(disk.tobytes())

def load_disk():
    with open('out/system.img', 'rb') as f:
        disk[:] = np.frombuffer(f.read(), dtype=np.uint8)

def read_sector(sector):
    return disk[sector * 512:(sector + 1) * 512]

def write_sector(data, sector):
    disk[sector * 512:(sector + 1) * 512] = data
    save_to_disk()

def fill_map():
    map_data = read_sector(MAP_SECTOR_NUMBER)
    
    # Bootloader and kernel
    for i in range(32):
        map_data[i] = 1
    
    # Unreadable sectors
    for i in range(256, 512):
        map_data[i] = 1

    write_sector(map_data, MAP_SECTOR_NUMBER)

load_disk()
fill_map()
save_to_disk()