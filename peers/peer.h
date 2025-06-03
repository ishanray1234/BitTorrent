#ifndef PEER_H
#define PEER_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Peer {
public:
    string ip;
    int port;

    Peer(const string& ipAddress, int portNumber)
        : ip(ipAddress), port(portNumber) {}

    void display() const {
        cout << "Peer IP: " << ip << ", Port: " << port << endl;
    }
};

vector<Peer> Unmarshal(const string& buffer);

#endif // PEER_H
