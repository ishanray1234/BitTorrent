#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <openssl/sha.h>
#include "torrentfile.h"
#include "tracker.h"
using namespace std;

int main(){

    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try {
        // Example torrent file path
        string torrent_file_path = "test.torrent";
        TorrentFile torrent_file = Open(torrent_file_path);
        torrent_file.display();

        // Example PeerID and port
        string PeerID = "-TR2930-123456789012";
        uint16_t port = 6881;

        // Request peers from the tracker
        vector<Peer> peers = requestPeers(&torrent_file, PeerID, port);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}