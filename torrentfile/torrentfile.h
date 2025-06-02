#ifndef TORRENTFILE_H
#define TORRENTFILE_H

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <openssl/sha.h>
#include "../bencode/torrent_parser.h"

using json = nlohmann::json;

// Represents the metadata parsed from a .torrent file
class TorrentFile {
public:
    std::string announce;
    std::array<unsigned char, 20> infoHash;
    std::vector<std::array<unsigned char, 20>> pieceHashes;
    int PieceLength;
    int Length;
    std::string Name;

    TorrentFile(std::string announceUrl, std::array<unsigned char, 20> infoHashValue,
                std::vector<std::array<unsigned char, 20>> pieceHashesList,
                int pieceLength, int totalLength, std::string name);

    void display() const;
};

// Represents the "info" dictionary inside a .torrent file
class bencodeInfo {
public:
    std::string Name;
    int PieceLength;
    int Length;
    std::string Pieces;

    void display() const;
};

// Represents the top-level parsed structure from a .torrent file
class bencodeTorrent {
public:
    std::string Announce;
    bencodeInfo Info;
};

// Function declarations
std::vector<std::array<unsigned char, 20>> splitPieceHashes(const std::string& pieces);
TorrentFile toTorrentFile(const bencodeTorrent& bencode_torrent, const json& info);
TorrentFile Open(const std::string& path);

// Optional helper functions declared in other files
std::string encode_bencode(const json& value);
json decode_bencoded_value(const std::string& input, size_t& pos);

#endif // TORRENTFILE_H
