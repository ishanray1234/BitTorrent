#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<array>

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

vector<Peer> Unmarshal(const string& buffer) {
    vector<Peer> peers;
    istringstream stream(buffer);
    string peerInfo;

    while (getline(stream, peerInfo, ',')) {
        size_t colonPos = peerInfo.find(':');
        if (colonPos != string::npos) {
            string ip = peerInfo.substr(0, colonPos);
            int port = stoi(peerInfo.substr(colonPos + 1));
            peers.emplace_back(ip, port);
        }
    }

    return peers;
}
