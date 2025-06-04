#ifndef P2P_H
#define P2P_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include "../message/message.h"
#include "../peers/peer.h"
#include "../client/client.h"

class Torrent{
    public:
    vector<Peer> peers; // List of peers
    string PeerID;
    string infoHash; // Info hash of the torrent
    vector<string> PieceHashes; // List of piece hashes
    int PieceLength; // Length of each piece
    int Length; // Total length of the torrent
    string Name; // Name of the torrent
};

class PieceWork {
    public:
    int index; // Index of the piece
    string hash; // Hash of the piece
    int length; // Length of the piece
};

class PieceResult {
    public:
    int index; // Index of the piece
    string buf; // Buffer containing the piece data
};





vector<char> Download(Torrent& torrent);

#endif // P2P_H