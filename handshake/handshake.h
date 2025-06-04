#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>

using namespace std;

class Handshake {
public: 
    string Pstr;
    string infoHash;
    string PeerID;

    Handshake(const string& pstr, const string& infoHash, const string& peerID)
        : Pstr(pstr), infoHash(infoHash), PeerID(peerID) {}

    Handshake(const string& infoHash, const string& peerID)
        : Pstr("BitTorrent protocol"), infoHash(infoHash), PeerID(peerID) {}
    
    void display(){
        cout<< endl << Pstr << endl << infoHash << endl << PeerID << endl;
    }

    static Handshake Read(const string& buffer);
};

string Serialize(const Handshake& handshake);

#endif
