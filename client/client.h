#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <string>
#include <array>
#include <netinet/in.h>  // For sockaddr_in
#include <unistd.h>      // For close()
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../message/message.h"
#include "../bitfield/bitfield.h"
#include "../handshake/handshake.h"
#include "../peers/peer.h"

using namespace std;

class Client {
public:
    int sockfd;
    bool choked;
    Bitfield bitfield;
    string peerIP;
    string infoHash;
    string peerID;

    Client(int sockfd, bool isConnected, const Bitfield& bitfield, const Peer& peer, const std::string& infoHash, const std::string& peerID)
        : sockfd(sockfd), choked(isConnected), bitfield(bitfield),
          peerIP(peerIP), infoHash(infoHash), peerID(peerID) {}

    static Client* New(Peer peer,
                       const string& peerID, const string& infoHash);

    Message Read();
    bool SendRequest(int index, int begin, int length);
    bool SendInterested();
    bool SendNotInterested();
    bool SendUnchoke();
    bool SendHave(int index);
};

#endif
