#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>
#include "util.h"

#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH

/// type definitions
typedef unsigned char* hash;

hash hash_digest(buffer);
hash hash_file_digest(char *);
char *hash_base64_encode(hash);
hash hash_copy(hash);
hash distance(hash, hash);

#endif
