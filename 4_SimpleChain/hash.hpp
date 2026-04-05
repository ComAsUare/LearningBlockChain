#pragma once

#include <string>

typedef struct evp_md_st EVP_MD;

enum class HashName {
    SHA256,
    SHA512,
    SHA3_256,
    KECCAK_256
};

const EVP_MD* toEvp(HashName h);
std::string EVP_Hash(const std::string& input, HashName hf_name);
