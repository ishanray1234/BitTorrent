#ifndef BITFIELD_H
#define BITFIELD_H

#include <vector>
#include <cstdint>

using namespace std;

class Bitfield {
public:
    Bitfield(string data);
    Bitfield(size_t numPieces);

    // Check if a particular piece index is set
    bool HasPiece(int index) const;

    // Set a particular piece index
    void SetPiece(int index);

    // Access internal byte vector if needed
    const vector<uint8_t>& data() const;

    vector<uint8_t> bits;
};

#endif // BITFIELD_H
