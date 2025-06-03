#ifndef TRACKER_H
#define TRACKER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <sstream>
#include <iomanip>

#include "../peers/peer.h"
#include "../torrentfile/torrentfile.h"
#include "../bencode/torrent_parser.h"

using namespace std;

class bencodeTrackerResp {
public:
    int Interval;     // Interval in seconds for the next request
    string Peers;     // List of peers in the response

    // Constructor
    bencodeTrackerResp(int interval, const string& peersList);
    
    // Default constructor
    bencodeTrackerResp() = default;

    // Display function
    void display() const;
};

// Fetches the tracker response (demo version)
string fetchTrackerResponse(string url);

// Percent-encode a byte array
string percentEncode(const unsigned char* data, size_t length);

// Build the tracker URL with query parameters
string buildTrackerURL(TorrentFile* torrent_file, string PeerID, uint16_t port);

// Request peers from tracker and parse response
vector<Peer> requestPeers(TorrentFile* torrent_file, string PeerID, uint16_t port);

#endif // TRACKER_H
