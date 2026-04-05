#include "hash.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>

const EVP_MD* toEvp(HashName h) {
    switch (h) {
    case HashName::SHA256:
        return EVP_sha256();
    case HashName::SHA512:
        return EVP_sha512();
    case HashName::SHA3_256:
        return EVP_sha3_256();
    case HashName::KECCAK_256:
        //return EVP_keccak256();
        return EVP_MD_fetch(NULL, "KECCAK-256", NULL);
    default:
        return nullptr; // wrong hash name error
    }
}

std::string EVP_Hash(const std::string& input, HashName hf_name) {
  const EVP_MD* md = toEvp(hf_name);
  if (md == nullptr) {
    throw std::invalid_argument("Unsupported hash name");
  }

  EVP_MD_CTX* context = EVP_MD_CTX_new();
  if (context == nullptr) {
    throw std::runtime_error("EVP_MD_CTX_new failed");
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_length = 0;

  if (EVP_DigestInit_ex(context, md, nullptr) != 1 ||
      EVP_DigestUpdate(context, input.data(), input.size()) != 1 ||
      EVP_DigestFinal_ex(context, hash, &hash_length) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Digest computation failed");
  }

  EVP_MD_CTX_free(context);

  std::ostringstream output;
  output << std::hex << std::setfill('0');
  for (unsigned int i = 0; i < hash_length; ++i) {
    output << std::setw(2) << static_cast<int>(hash[i]);
  }

  return output.str();
}
