#include "bitfield.h"

using namespace std;

Bitfield::Bitfield(size_t numPieces) {
    size_t numBytes = (numPieces + 7) / 8;
    bits.resize(numBytes, 0);
}

bool Bitfield::HasPiece(int index) const {
    if (index < 0) return false;

    int byteIndex = index / 8;
    int offset = index % 8;

    if (byteIndex >= static_cast<int>(bits.size())) {
        return false;
    }

    return (bits[byteIndex] >> (7 - offset)) & 1;
}

void Bitfield::SetPiece(int index) {
    if (index < 0) return;

    int byteIndex = index / 8;
    int offset = index % 8;

    if (byteIndex >= static_cast<int>(bits.size())) {
        return;
    }

    bits[byteIndex] |= (1 << (7 - offset));
}

const vector<uint8_t>& Bitfield::data() const {
    return bits;
}
