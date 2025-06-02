import bencodepy
import hashlib

# Load torrent file
with open('test.cpp.torrent', 'rb') as f:
    torrent_data = bencodepy.decode(f.read())

# Extract 'info' dictionary
info = torrent_data[b'info']

# Bencode the info dictionary
bencoded_info = bencodepy.encode(info)

# Calculate SHA-1 hash
info_hash = hashlib.sha1(bencoded_info).digest()

print('InfoHash (hex):', info_hash.hex())

# curl "http://bttracker.debian.org:6969/announce?info_hash=info_hash=52982b96c83ddf3dfb73442069395e7e1af0c3b2&peer_id=-TR2940-6wfGbnYxWmNp&port=6882&uploaded=0&downloaded=0&left=1000&compact=1"

# 52982b96c83ddf3dfb73442069395e7e1af0c3b2