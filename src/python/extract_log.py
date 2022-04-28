from filesystem import *

load_disk()

log_data = read_sector(0x107)

# write to file
with open('out/log.txt', 'wb') as f:
    f.write(bytes(log_data))