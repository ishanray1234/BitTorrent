import bencodepy
import hashlib
import os

def create_torrent(file_path, announce_url):
    # Read file data
    file_size = os.path.getsize(file_path)
    piece_length = 262144  # 256KB pieces

    # Read file in pieces and create pieces hash string
    pieces = b""
    with open(file_path, "rb") as f:
        while True:
            piece = f.read(piece_length)
            if not piece:
                break
            pieces += hashlib.sha1(piece).digest()

    # Build info dictionary
    info = {
        b'name': os.path.basename(file_path).encode(),
        b'piece length': piece_length,
        b'length': file_size,
        b'pieces': pieces
    }

    # Build torrent dictionary
    torrent = {
        b'announce': announce_url.encode(),
        b'info': info
    }

    # Bencode torrent dictionary
    torrent_data = bencodepy.encode(torrent)

    # Write to .torrent file
    torrent_file = file_path + ".torrent"
    with open(torrent_file, "wb") as tf:
        tf.write(torrent_data)

    print(f"Torrent file created: {torrent_file}")

# Usage example
create_torrent("/Users/ishan_ray/Desktop/Summer 2025/BitTorrent/test.cpp", "http://bttracker.debian.org:6969/announce")
