#include "checkZeroDigits.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

bool checkZeroDigits(const std::string& hash, int zeroDigits){
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < hash.length(); i += 2) {
        // Extract 2 hex chars and convert to base 16
        std::string byteString = hash.substr(i, 2);
        uint8_t byte = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    // Check if the specified number of leading bits are zero
    int fullBytes = zeroDigits / 8;
    int remainingBits = zeroDigits % 8;

    for (int i = 0; i < fullBytes; i++) {
        if (bytes[i] != 0) return false;
    }

    if (remainingBits > 0) {
        // Create mask for the top 'remainingBits'
        uint8_t mask = (0xFF << (8 - remainingBits)) & 0xFF;
        if ((bytes[fullBytes] & mask) != 0) return false;
    }
    return true;
}
