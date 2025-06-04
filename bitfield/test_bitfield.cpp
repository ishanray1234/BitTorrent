#include <iostream>
#include "bitfield.h"

int main() {
    Bitfield bf(10); // For 10 pieces

    bf.SetPiece(3);
    bf.SetPiece(7);

    for (int i = 0; i < 10; ++i) {
        std::cout << "Has piece " << i << "? " << (bf.HasPiece(i) ? "Yes" : "No") << "\n";
    }

    return 0;
}
