#ifndef PEER_H
#define PEER_H

#include <string>
#include <vector>
#include <iostream>

class Peer {
public:
    std::string ip;
    int port;

    Peer(const std::string& ipAddress, int portNumber)
        : ip(ipAddress), port(portNumber) {}

    void display() const {
        std::cout << "Peer IP: " << ip << ", Port: " << port << std::endl;
    }
};

std::vector<Peer> Unmarshal(const std::string& buffer);

#endif // PEER_H
