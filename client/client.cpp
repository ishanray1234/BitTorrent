#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include "client.h"

using namespace std;

// Set a timeout on a socket
bool setTimeout(int sockfd, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == 0 &&
           setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == 0;
}

Client* Client::New(Peer peer, const string& peerID, const string& infoHash) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket" << endl;
        return nullptr;
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(peer.port); // Convert port to network byte order
    inet_pton(AF_INET, peer.ip.c_str(), &serverAddr.sin_addr);

    setTimeout(sockfd, 3);
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error connecting to peer" << endl;
        close(sockfd);
        return nullptr;
    }

    // Send and receive handshake
    Handshake hsSend(infoHash, peerID);
    string hsData = Serialize(hsSend);
    if (send(sockfd, hsData.c_str(), hsData.size(), 0) < 0) {
        perror("Sending handshake failed");
        close(sockfd);
        return nullptr;
    }

    // Receive handshake response
    char buffer[100];
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesRead < 0) {
        perror("Receiving handshake failed");
        close(sockfd);
        return nullptr;
    }
    string hsRecvData(buffer, bytesRead);
    cout << "Received handshake data: " << hsRecvData << endl;
    
    Handshake hsRecv = Handshake::Read(hsRecvData);
    if (hsRecv.infoHash != infoHash) {
        std::cerr << "InfoHash mismatch" << std::endl;
        close(sockfd);
        return nullptr;
    }

    // Read Bitfield response
    char bitfieldBuffer[1024];
    bytesRead = recv(sockfd, bitfieldBuffer, sizeof(bitfieldBuffer), 0);
    if (bytesRead < 0) {
        perror("Receiving bitfield failed");
        close(sockfd);
        return nullptr;
    }
    string bitfieldData(bitfieldBuffer, bytesRead);
    cout << "Received bitfield data: " << bitfieldData << endl;

    Message bitfieldMsg = Message::Read(bitfieldData);
    if (bitfieldMsg.payload.empty()) {
        std::cerr << "Received empty bitfield payload" << std::endl;
        close(sockfd);
        return nullptr;
    }
    if (bitfieldMsg.ID != MessageID::MsgBitfield) {
        std::cerr << "Expected bitfield but got message ID " << static_cast<int>(bitfieldMsg.ID) << std::endl;
        close(sockfd);
        return nullptr;
    }

    Bitfield bf(bitfieldMsg.payload); // Assuming payload is a bitfield
    
    return new Client(sockfd, true, bf, peer, infoHash, peerID);
}

// Read and consume a message
Message Client::Read() {
    char buffer[1024];
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        cerr << "Error reading from socket or connection closed" << endl;
        return Message(MessageID::MsgChoke, ""); // Return a choke message on error
    }
    
    string msgData(buffer, bytesRead);
    Message msg = Message::Read(msgData);
    
    if (msg.ID == MessageID::MsgHave) {
        int index = ParseHave(msg);
        if (index >= 0) {
            bitfield.SetPiece(index);
        }
    } else if (msg.ID == MessageID::MsgPiece) {
        // Handle piece message
        // Assuming we have a buffer to write the piece data into
        // This part is simplified; you would need to manage your piece buffer properly
        cout << "Received piece message with payload size: " << msg.payload.size() << endl;
    }

    return msg;
}

// Send REQUEST
bool Client::SendRequest(int index, int begin, int length) {
    Message req = FormatRequest(index, begin, length);
    std::string msg = Serialize(req);
    return send(sockfd, msg.c_str(), msg.size(), 0) == (ssize_t)msg.size();
}

// Send INTERESTED
bool Client::SendInterested() {
    Message msg(MessageID::MsgInterested, "");
    std::string data = Serialize(msg);
    return send(sockfd, data.c_str(), data.size(), 0) == (ssize_t)data.size();
}

// Send NOT INTERESTED
bool Client::SendNotInterested() {
    Message msg(MessageID::MsgNotInterested, "");
    std::string data = Serialize(msg);
    return send(sockfd, data.c_str(), data.size(), 0) == (ssize_t)data.size();
}

// Send UNCHOKE
bool Client::SendUnchoke() {
    Message msg(MessageID::MsgUnchoke, "");
    std::string data = Serialize(msg);
    return send(sockfd, data.c_str(), data.size(), 0) == (ssize_t)data.size();
}

// Send HAVE
bool Client::SendHave(int index) {
    Message msg = FormatHave(index);
    std::string data = Serialize(msg);
    return send(sockfd, data.c_str(), data.size(), 0) == (ssize_t)data.size();
}