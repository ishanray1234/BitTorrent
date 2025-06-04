#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

enum class MessageID {
    MsgChoke = 0,         // Chokes the receiver
    MsgUnchoke = 1,       // Unchokes the receiver
    MsgInterested = 2,    // Expresses interest in receiving data
    MsgNotInterested = 3, // Expresses disinterest
    MsgHave = 4,          // Alerts that a piece is available
    MsgBitfield = 5,      // Encodes downloaded pieces
    MsgRequest = 6,       // Requests a block of data
    MsgPiece = 7,         // Sends a block of data
    MsgCancel = 8         // Cancels a previous request
};

class Message {
public:
    MessageID ID;
    std::string payload;

    Message(MessageID id, const std::string& data);
};

// Message construction
Message FormatRequest(int index, int begin, int length);
Message FormatHave(int index);

// Message parsing
int ParsePiece(int index, std::string& buffer, Message& message);
int ParseHave(Message& message);

// Serialization and deserialization
std::string Serialize(const Message& message);
Message Read(const std::string& buffer);

// Helper
std::string name(const Message& message);

#endif // MESSAGE_H
