#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include "message.h"

using namespace std;

int main() {
    // Create a REQUEST message
    Message request = FormatRequest(1, 0, 1024);
    cout << "Request Message ID: " << static_cast<int>(request.ID) << endl;
    cout << "Request Payload: " << request.payload << endl;

    // Create a HAVE message
    Message have = FormatHave(1);
    cout << "Have Message ID: " << static_cast<int>(have.ID) << endl;
    cout << "Have Payload: " << have.payload << endl;

    // Serialize the messages
    string serializedRequest = Serialize(request);
    string serializedHave = Serialize(have);
    cout << "Serialized Request: " << serializedRequest << endl;
    cout << "Serialized Have: " << serializedHave << endl;

    // Deserialize the messages
    Message deserializedRequest = Read(serializedRequest);
    Message deserializedHave = Read(serializedHave);
    
    cout << "Deserialized Request ID: " << static_cast<int>(deserializedRequest.ID) 
         << ", Payload: " << deserializedRequest.payload << endl;
    cout << "Deserialized Have ID: " << static_cast<int>(deserializedHave.ID) 
         << ", Payload: " << deserializedHave.payload << endl;

    return 0;
}