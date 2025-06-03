#include <iostream>
#include "peer.h"

int main() {
    std::string buffer = "192.168.1.1:6881,10.0.0.2:51413,127.0.0.1:8080";
    std::vector<Peer> peers = Unmarshal(buffer);

    std::cout << "Parsed Peers:\n";
    for (const auto& peer : peers) {
        peer.display();
    }

    return 0;
}
