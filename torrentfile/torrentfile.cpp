// Following the structre from https://blog.jse.li/posts/torrent/#putting-it-all-together

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include "../bencode/torrent_parser.h"

using namespace std;
// TorrentFile encodes the metadata from a .torrent file
class TorrentFile {
    public:
        string announce; // Tracker URL
        array<unsigned char, 20> infoHash;
        vector<array<unsigned char, 20>> pieceHashes;
        int PieceLength; // Length of each piece in bytes
        int Length; // Total length of the file in bytes
        string Name; // Name of the file
};

class bencodeInfo {
    public:
        string Name; // Name of the file
        int PieceLength; // Length of each piece in bytes
        int Length; // Total length of the file in bytes
        string Pieces; // Concatenated SHA1 hashes of each piece
    
        // Function to display bencodeInfo information
        void display() {
            cout << "Name: " << Name << endl;cout << "Piece Length: " << PieceLength << " bytes" << endl;
            cout << "Total Length: " << Length << " bytes" << endl;cout << "Pieces: " << Pieces << endl;}
};

class bencodeTorrent {
    public:
        string Announce; // Tracker URL
        bencodeInfo Info; // Metadata about the file
};

// DownloadToFile downloads a torrent and writes it to a file
// string DownloadToFile(TorrentFile* t){
    
// }

vector<array<unsigned char, 20>> splitPieceHashes(const string& pieces) {
    const size_t hashLen = 20;
    if (pieces.size() % hashLen != 0) {
        throw runtime_error("Received malformed pieces of length " + to_string(pieces.size()));
    }

    size_t numHashes = pieces.size() / hashLen;
    vector<array<unsigned char, 20>> hashes(numHashes);

    for (size_t i = 0; i < numHashes; ++i) {
        copy_n(pieces.begin() + i * hashLen, hashLen, hashes[i].begin());
    }

    return hashes;
}

TorrentFile toTorrentFile(const bencodeTorrent& bencode_torrent, const json& info) {
    cout<< "Converting bencodeTorrent to TorrentFile..." << endl;
    TorrentFile torrent_file;
    torrent_file.announce = bencode_torrent.Announce;
    torrent_file.PieceLength = (bencode_torrent.Info.PieceLength);
    torrent_file.Length = bencode_torrent.Info.Length;
    torrent_file.Name = bencode_torrent.Info.Name;


    string bencoded_info = encode_bencode(info);
    cout << "Bencoded Info: " << bencoded_info << endl;

    array<unsigned char, 20> hash;
    SHA1(reinterpret_cast<const unsigned char*>(bencoded_info.data()), bencoded_info.size(), hash.data());
    // return hash;
    for (unsigned char byte : hash) {
        printf("%02x ", byte);
        // cout<< hex << static_cast<int>(byte)<<" ";
    }
    cout<<endl;

    torrent_file.infoHash = hash;

    vector<array<unsigned char, 20>> piece_hashes = splitPieceHashes(bencode_torrent.Info.Pieces);
    for(const auto& piece_hash : piece_hashes) {
        cout << "Piece Hash: ";
        for (unsigned char byte : piece_hash) {
            printf("%02x ", byte);
            // cout << hex << static_cast<int>(byte) << " ";
        }
        cout << endl;
    }
    torrent_file.pieceHashes = piece_hashes;

    // torrent_file.display();
    
    return torrent_file;
}

// Open parses a torrent file
TorrentFile Open(const std::string& path) {
    ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    ostringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    
    size_t pos = 0;
    auto dictionary = decode_bencoded_value(content, pos);

    if (!dictionary.contains("announce") || !dictionary.contains("info")) {
        throw std::runtime_error("Invalid torrent file: missing announce or info field");
    }

    auto info = dictionary["info"];
    cout<< "Info: " << info << endl;
    bencodeTorrent bencode_torrent;
    bencode_torrent.Announce = dictionary["announce"].get<string>();
    bencode_torrent.Info.Name = info["name"].get<string>();
    bencode_torrent.Info.PieceLength = info["piece length"].get<int>();
    bencode_torrent.Info.Length = info["length"].get<int>();
    bencode_torrent.Info.Pieces = info["pieces"].get<string>();

    cout<< "Parsed Torrent File:" << endl;
    cout << "Announce URL: " << bencode_torrent.Announce << endl;
    cout << "Name: " << bencode_torrent.Info.Name << endl;
    cout << "Piece Length: " << bencode_torrent.Info.PieceLength << " bytes" << endl;
    cout << "Total Length: " << bencode_torrent.Info.Length << " bytes" << endl;
    cout << "Pieces: " << bencode_torrent.Info.Pieces.size()<< " pieces" << endl;

    return toTorrentFile(bencode_torrent,info);
}
