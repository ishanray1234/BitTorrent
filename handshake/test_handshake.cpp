#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include "handshake.h"

using namespace std;

int main() {
    // Create a Handshake object
    Handshake handshake("BitTorrent protocol", "1234567890abcdef1234", "-TR2930-123456789012");

    // Serialize the handshake
    string serialized = Serialize(handshake);
    cout << "Serialized Handshake: " << serialized << endl;
    cout<< "Length of serialized handshake: " << serialized.length() << endl;
    // Read the handshake from the serialized string
    Handshake readHandshake = Read(serialized);
    
    // Display the read handshake
    cout << "Read Handshake: ";
    readHandshake.display();

    return 0;
}