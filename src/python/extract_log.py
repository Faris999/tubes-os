from filesystem import *

load_disk()

log_data = read_sector(0x107)

# write to file
with open('out/log.txt', 'wb') as f:
    raw_data = bytes(log_data)
    # remove null bytes at the end
    raw_data = raw_data[:raw_data.index(b'\x00')]
    f.write(raw_data)
