#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include "message.h"

using namespace std;

enum class MessageID {
    MsgChoke = 0, // MsgChoke chokes the receiver
    MsgUnchoke = 1, // MsgUnchoke unchokes the receiver
    MsgInterested = 2, // MsgInterested expresses interest in receiving data
    MsgNotInterested = 3, // MsgNotInterested expresses disinterest in receiving data
    MsgHave = 4, // MsgHave alerts the receiver that the sender has downloaded a piece
    MsgBitfield = 5, // MsgBitfield encodes which pieces that the sender has downloaded
    MsgRequest = 6, // MsgRequest requests a block of data from the receiver
    MsgPiece = 7, // MsgPiece delivers a block of data to fulfill a request
    MsgCancel = 8 // MsgCancel cancels a request
};

// 8-bit index -- 8-bit begin -- 8-bit length
// FormatRequest creates a REQUEST message
Message FormatRequest(int index, int begin, int length) {
    string payload = "";
    payload += to_string(index).insert(0, 8 - to_string(index).length(), '0'); // 8-digit index
    payload += to_string(begin).insert(0, 8 - to_string(begin).length(), '0'); // 8-digit begin
    payload += to_string(length).insert(0, 8 - to_string(length).length(), '0'); // 8-digit length
    return Message(MessageID::MsgRequest, payload);
}

// FormatHave creates a HAVE message
Message FormatHave(int index) {
    string payload = to_string(index).insert(0, 8 - to_string(index).length(), '0'); // 8-digit index
    return Message(MessageID::MsgHave, payload);
}

// ParsePiece parses a PIECE message and copies its payload into a buffer
int ParsePiece(int index, string& buffer, Message& message) {
    if(message.ID != MessageID::MsgPiece) {
        cerr << "Error: Invalid message ID for piece parsing. Expected " << static_cast<int>(MessageID::MsgPiece) << ", got " << static_cast<int>(message.ID) << endl;
        return -1;
    }
    if(message.payload.length() < 16) { // 8 for index + 8 for begin
        cerr << "Error: Buffer too short to contain piece data." << endl;
        return -1;
    }
    int parsedIndex = stoi(message.payload.substr(0, 8)); // Extract the index from the payload
    if(parsedIndex != index) {
        cerr << "Error: Index mismatch. Expected " << index << ", got " << parsedIndex << "." << endl;
        return -1;
    }
    int begin = stoi(message.payload.substr(8, 8)); // Extract the begin value from the payload
    if(begin < 0 || begin >= buffer.length()) {
        cerr << "Error: Invalid begin value: " << begin << "." << endl;
        return -1;
    }
    string data = message.payload.substr(8);
    if(begin + data.length() > buffer.length()) {
        cerr << "Error: Data length exceeds buffer size." << endl;
        return -1;
    }
    buffer.replace(begin, data.length(), data); // Replace the specified section of the buffer with the received data
    return data.length(); // Return the new length of the buffer after the piece has been added
}

// ParseHave parses a HAVE message
int ParseHave(Message& message) {
    if(message.ID != MessageID::MsgHave) {
        cerr << "Error: Invalid message ID for piece parsing. Expected " << static_cast<int>(MessageID::MsgPiece) << ", got " << static_cast<int>(message.ID) << endl;
        return -1;
    }
    if(message.payload.length() < 8) { // 8 for index
        cerr << "Error: Buffer too short to contain have data." << endl;
        return -1;
    }
    int index = stoi(message.payload.substr(0, 8)); // Extract the index from the payload
    return index; // Return the index of the piece that has been downloaded
}

// Serialize serializes a message into a buffer of the form
// <length prefix><message ID><payload>
// Interprets `nil` as a keep-alive message
string Serialize(const Message& message) {
    string serialized = "";
    int length = message.payload.length() + 1; // +1 for the message ID
    serialized += to_string(length).insert(0, 8 - to_string(length).length(), '0'); // 8-digit length prefix
    serialized += to_string(static_cast<int>(message.ID)); // Convert MessageID to int and append it
    serialized += message.payload; // Append the payload
    return serialized;
}

// Deserialize deserializes a buffer into a message
Message Read(const string& buffer) {
    if(buffer.length() < 9) { // Minimum length is 9 (8 for length + 1 for message ID)
        cerr << "Error: Buffer too short to contain a valid message." << endl;
        return Message(MessageID::MsgChoke, ""); // Return a default message
    }
    int length = stoi(buffer.substr(0, 8)); // Extract the length prefix
    if(length + 8 > buffer.length()) {
        cerr << "Error: Buffer length does not match the specified length prefix." << endl;
        return Message(MessageID::MsgChoke, ""); // Return a default message
    }
    int id = stoi(buffer.substr(8, 1)); // Extract the message ID
    string payload = buffer.substr(9, length - 1); // Extract the payload

    return Message(static_cast<MessageID>(id), payload); // Return the constructed message
}

string name(const Message& message) {
    switch(message.ID) {
        case MessageID::MsgChoke: return "Choke";
        case MessageID::MsgUnchoke: return "Unchoke";
        case MessageID::MsgInterested: return "Interested";
        case MessageID::MsgNotInterested: return "Not Interested";
        case MessageID::MsgHave: return "Have";
        case MessageID::MsgBitfield: return "Bitfield";
        case MessageID::MsgRequest: return "Request";
        case MessageID::MsgPiece: return "Piece";
        case MessageID::MsgCancel: return "Cancel";
        default: return "Unknown";
    }
}
