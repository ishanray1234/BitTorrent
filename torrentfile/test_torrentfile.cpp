#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <openssl/sha.h>
#include "torrentfile.h"
using namespace std;

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <torrent_file_path>" << std::endl;
        return 1;
    }

    try {
        string torrent_file_path = argv[1];
        TorrentFile torrent_file = Open(torrent_file_path);
        // torrent_file.display();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}