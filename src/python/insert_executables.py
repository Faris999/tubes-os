import numpy as np
import os
from filesystem import *

load_disk()

folder = 'out/bin'
files = [i for i in os.listdir(folder) if i != '.gitignore']

write(FileMetadata(np.zeros(0, dtype=np.uint8), 'bin', FS_NODE_P_IDX_ROOT, 0))

for file in files:
    insert_file(file)

save_to_disk()