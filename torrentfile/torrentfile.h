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
using namespace std;

// Represents the metadata parsed from a .torrent file
class TorrentFile {
public:
    std::string announce;
    std::array<unsigned char, 20> infoHash;
    std::vector<std::array<unsigned char, 20>> pieceHashes;
    int PieceLength;
    int Length;
    std::string Name;

    void display(){
        cout << "Announce URL: " << announce << endl;

        cout << "Info Hash: ";
        for (const auto& byte : infoHash) {
            // Always output 2-digit hex
            cout << std::hex << std::setw(2) << std::setfill('0') << (int)(byte & 0xff);
        }
        cout << std::dec << endl; // Switch back to decimal output

        cout << "Piece Length: " << PieceLength << " bytes" << endl;
        cout << "Total Length: " << Length << " bytes" << endl;
        cout << "Name: " << Name << endl;
        cout << "Number of Pieces: " << pieceHashes.size() << endl;
    }

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
