import numpy as np
import os

MAP_SECTOR_NUMBER = 0x100
NODE_SECTOR_NUMBER = 0x101
SECTOR_SECTOR_NUMBER = 0x103

FS_NODE_S_IDX_FOLDER = 0xFF
FS_NODE_P_IDX_ROOT = 0xFF

class NodeFilesystem():
    def __init__(self, sector_data):
        self.nodes = [NodeEntry(sector_data[i:i+16]) for i in range(0, len(sector_data), 16)]

    def raw(self):
        return np.concatenate([node.raw() for node in self.nodes])

class NodeEntry():
    def __init__(self, data):
        self.parent_node_index = data[0]
        self.sector_entry_index = data[1]
        self.name = bytes(data[2:]).decode('ascii').strip('\x00')

    def raw(self):
        raw_data = np.zeros(16, dtype=np.uint8)
        raw_data[0] = self.parent_node_index
        raw_data[1] = self.sector_entry_index
        name = np.frombuffer(self.name.encode('ascii'), dtype=np.uint8)
        name = np.pad(name, (0, 14 - len(name)), 'constant', constant_values=0)
        raw_data[2:] = name 
        return raw_data

class SectorFilesystem():
    def __init__(self, sector_data):
        self.sectors = [SectorEntry(sector_data[i:i+16]) for i in range(0, len(sector_data), 16)]

    def raw(self):
        return np.concatenate([sector.raw() for sector in self.sectors]) 

class SectorEntry():
    def __init__(self, data):
        self.sector_numbers = data
    
    def raw(self):
        return self.sector_numbers

class FileMetadata():
    def __init__(self, buffer, node_name, parent_index, filesize):
        self.buffer = buffer
        self.node_name = node_name
        self.parent_index = parent_index
        self.filesize = filesize

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

def write(metadata):
    # Load filesystems
    map_fs_buffer = read_sector(MAP_SECTOR_NUMBER)
    sector_fs_buffer = SectorFilesystem(read_sector(SECTOR_SECTOR_NUMBER))
    node_fs_buffer = NodeFilesystem(np.concatenate((read_sector(NODE_SECTOR_NUMBER), read_sector(NODE_SECTOR_NUMBER + 1)), axis=0))

    # Find node
    for i in range(64):
        if node_fs_buffer.nodes[i].name == metadata.node_name:
            if node_fs_buffer.nodes[i].parent_node_index == metadata.parent_index:
                return 'File already exists'

    # Find free node
    for i in range(64):
        if node_fs_buffer.nodes[i].name == '':
            node_index = i
            break

    if node_index == None:
        return 'No free nodes'

    # Check if parent node is a folder
    if metadata.parent_index != FS_NODE_P_IDX_ROOT and node_fs_buffer.nodes[metadata.parent_index].sector_entry_index != FS_NODE_S_IDX_FOLDER:
        return 'Parent node is not a folder'

    # Check if filesize is valid
    if metadata.filesize > 8192:
        return 'Filesize is too large'

    empty_sector_count = 0
    for i in range(512):
        if map_fs_buffer[i] == 0:
            empty_sector_count += 1
    
    if empty_sector_count < metadata.filesize / 512:
        return 'Not enough free sectors'

    # Find free sectors
    for i in range(32):
        if sector_fs_buffer.sectors[i].sector_numbers[0] == 0:
            sector_index = i
            break
    
    if sector_index == None:
        return 'No free sectors'

    # Write file
    node_buffer = node_fs_buffer.nodes[node_index]
    sector_buffer = sector_fs_buffer.sectors[sector_index]

    node_buffer.parent_node_index = metadata.parent_index
    node_buffer.name = metadata.node_name

    # Check if writing a folder
    if metadata.filesize == 0:
        node_buffer.sector_entry_index = FS_NODE_S_IDX_FOLDER
    else:
        node_buffer.sector_entry_index = sector_index
        # Write sectors
        j = 0
        written = 0
        for i in range(256):
            if map_fs_buffer[i] == 1:
                continue
            print(i)
            map_fs_buffer[i] = 1
            sector_buffer.sector_numbers[j] = i
            j += 1
            write_sector(metadata.buffer[written:written+512], i)
            written += 512
            if written >= metadata.filesize:
                break

    # Write filesystems
    write_sector(map_fs_buffer, MAP_SECTOR_NUMBER)
    sector_fs_buffer.sectors[sector_index] = sector_buffer
    node_fs_buffer.nodes[node_index] = node_buffer
    write_sector(sector_fs_buffer.raw(), SECTOR_SECTOR_NUMBER)
    raw_node_fs_buffer = node_fs_buffer.raw()
    write_sector(raw_node_fs_buffer[:512], NODE_SECTOR_NUMBER)
    write_sector(raw_node_fs_buffer[512:], NODE_SECTOR_NUMBER + 1)

    return 'Success'

def insert_file(file):
    with open(f'out/bin/{file}', 'rb') as f:
        data = np.frombuffer(f.read(), dtype=np.uint8)
    # pad data to 512 bytes
    data = np.pad(data, (0, 512 - len(data) % 512), 'constant', constant_values=0)
    output = write(FileMetadata(data, file, 0, len(data)))
    print(output)

load_disk()

folder = 'out/bin'
files = [i for i in os.listdir(folder) if i != '.gitignore']

write(FileMetadata(np.zeros(0, dtype=np.uint8), 'bin', FS_NODE_P_IDX_ROOT, 0))

for file in files:
    insert_file(file)

save_to_disk()