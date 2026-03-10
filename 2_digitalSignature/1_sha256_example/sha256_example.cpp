#include <openssl/evp.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string sha256(const std::string& input) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(context, input.data(), input.size()) != 1 ||
        EVP_DigestFinal_ex(context, hash, &hash_length) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("SHA-256 digest computation failed");
    }

    EVP_MD_CTX_free(context);

    std::ostringstream output;
    output << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < hash_length; ++i) {
        output << std::setw(2) << static_cast<int>(hash[i]);
    }

    return output.str();
}

int main() {
    std::string input = "Stan is a programmer";
    try {
        std::cout << "input:  " << input << std::endl;
        std::cout << "sha256: " << sha256(input) << std::endl;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << std::endl;
        return 1;
    }
}
