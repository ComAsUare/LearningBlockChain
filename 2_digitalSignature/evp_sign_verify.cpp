#include <openssl/evp.h>
#include <openssl/pem.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

EVP_PKEY* load_private_key(const std::string& private_key_path) {
    FILE* file = fopen(private_key_path.c_str(), "r");
    if (file == nullptr) {
        throw std::runtime_error("failed to open private key: " + private_key_path);
    }

    EVP_PKEY* key = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (key == nullptr) {
        throw std::runtime_error("failed to parse private key: " + private_key_path);
    }

    return key;
}

EVP_PKEY* load_public_key(const std::string& public_key_path) {
    FILE* file = fopen(public_key_path.c_str(), "r");
    if (file == nullptr) {
        throw std::runtime_error("failed to open public key: " + public_key_path);
    }

    EVP_PKEY* key = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (key == nullptr) {
        throw std::runtime_error("failed to parse public key: " + public_key_path);
    }

    return key;
}

std::string key_type_name(EVP_PKEY* key) {
    switch (EVP_PKEY_base_id(key)) {
        case EVP_PKEY_RSA:
            return "RSA";
        case EVP_PKEY_EC:
            return "ECDSA/EC";
        default:
            return "unknown";
    }
}

std::vector<unsigned char> sign_message(const std::string& message,
                                        EVP_PKEY* private_key,
                                        const EVP_MD* digest) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    if (EVP_DigestSignInit(context, nullptr, digest, nullptr, private_key) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestSignInit failed");
    }

    if (EVP_DigestSignUpdate(context, message.data(), message.size()) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestSignUpdate failed");
    }

    size_t signature_length = 0;
    if (EVP_DigestSignFinal(context, nullptr, &signature_length) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestSignFinal size query failed");
    }

    std::vector<unsigned char> signature(signature_length);
    if (EVP_DigestSignFinal(context, signature.data(), &signature_length) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestSignFinal failed");
    }

    EVP_MD_CTX_free(context);
    signature.resize(signature_length);
    return signature;
}

bool verify_signature(const std::string& message,
                      const std::vector<unsigned char>& signature,
                      EVP_PKEY* public_key,
                      const EVP_MD* digest) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    if (EVP_DigestVerifyInit(context, nullptr, digest, nullptr, public_key) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestVerifyInit failed");
    }

    if (EVP_DigestVerifyUpdate(context, message.data(), message.size()) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("EVP_DigestVerifyUpdate failed");
    }

    int result = EVP_DigestVerifyFinal(context, signature.data(), signature.size());
    EVP_MD_CTX_free(context);

    if (result == 1) {
        return true;
    }
    if (result == 0) {
        return false;
    }

    throw std::runtime_error("EVP_DigestVerifyFinal failed");
}

std::string to_hex(const std::vector<unsigned char>& data) {
    std::ostringstream output;
    output << std::hex << std::setfill('0');
    for (unsigned char byte : data) {
        output << std::setw(2) << static_cast<int>(byte);
    }
    return output.str();
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "usage: " << argv[0] << " <private_key.pem> <public_key.pem> <message>\n";
            std::cerr << "example: " << argv[0]
                      << " rsa_private.pem rsa_public.pem \"hello world\"\n";
            return 1;
        }

        std::string private_key_path = argv[1];
        std::string public_key_path = argv[2];
        std::string message = argv[3];

        EVP_PKEY* private_key = load_private_key(private_key_path);
        EVP_PKEY* public_key = load_public_key(public_key_path);

        std::string private_key_type = key_type_name(private_key);
        std::string public_key_type = key_type_name(public_key);

        if (EVP_PKEY_base_id(private_key) != EVP_PKEY_base_id(public_key)) {
            EVP_PKEY_free(private_key);
            EVP_PKEY_free(public_key);
            throw std::runtime_error("private key and public key types do not match");
        }

        std::vector<unsigned char> signature = sign_message(message, private_key, EVP_sha256());
        bool verified = verify_signature(message, signature, public_key, EVP_sha256());

        std::cout << "key type:  " << private_key_type << '\n';
        std::cout << "message:   " << message << '\n';
        std::cout << "signature: " << to_hex(signature) << '\n';
        std::cout << "verify:    " << (verified ? "success" : "failed") << '\n';

        EVP_PKEY_free(private_key);
        EVP_PKEY_free(public_key);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
