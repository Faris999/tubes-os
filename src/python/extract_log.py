from filesystem import *

load_disk()

log_data = read_sector(0x107)

print(log_data)

# write to file
with open('out/log.txt', 'w') as f:
    f.write(bytes(log_data).decode('ascii').strip('\x00'))