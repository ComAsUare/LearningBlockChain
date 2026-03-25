export module EVP_HashFunction;
export import std;
export import openssl/evp.h;

#define EHASH_EXPORT export;
#include "EVP_hash_support.h"
