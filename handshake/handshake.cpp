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
};

string Serialize(const Handshake& handshake) {
    return handshake.Pstr + "00000000" + handshake.infoHash + handshake.PeerID;
}

Handshake Read(const string& buffer) {

    int length = buffer.length();
    
    string peerID = buffer.substr(length - 20, 20);
    string infoHash = buffer.substr(length - 40, 20);
    string reserve = buffer.substr(length - 48, 8);
    string pstr = buffer.substr(0, length - 48);
    
    return Handshake(pstr, infoHash, peerID);
}