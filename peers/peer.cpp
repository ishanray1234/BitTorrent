#include<iostream>

using namespace std;

class Peer {
public:
    string ip; // IP address of the peer
    int port; // Port number of the peer

    Peer(const string& ipAddress, int portNumber) : ip(ipAddress), port(portNumber) {}
    // Function to display peer information
    void display() const {
        cout << "Peer IP: " << ip << ", Port: " << port << endl;
    }
};

// Unmarshal parses peer IP addresses and ports from a buffer
vector<Peer> Unmarshal(const string& buffer) {
    vector<Peer> peers;
    size_t pos = 0;
    size_t nextPos;

    while ((nextPos = buffer.find(',', pos)) != string::npos) {
        string peerInfo = buffer.substr(pos, nextPos - pos);
        size_t colonPos = peerInfo.find(':');
        if (colonPos != string::npos) {
            string ip = peerInfo.substr(0, colonPos);
            int port = stoi(peerInfo.substr(colonPos + 1));
            peers.emplace_back(ip, port);
        }
        pos = nextPos + 1;
    }

    // Handle the last peer if there's no trailing comma
    if (pos < buffer.size()) {
        string peerInfo = buffer.substr(pos);
        size_t colonPos = peerInfo.find(':');
        if (colonPos != string::npos) {
            string ip = peerInfo.substr(0, colonPos);
            int port = stoi(peerInfo.substr(colonPos + 1));
            peers.emplace_back(ip, port);
        }
    }

    return peers;
}