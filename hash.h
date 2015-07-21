#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>
#include "util.h"

#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH

/// type definitions
typedef byte hash[DIGEST_LENGTH];

void hash_digest(hash, buffer);
void hash_file_digest(hash, char *);
char *hash_base64_encode(buffer);
buffer hash_base64_decode(char *);

#endif
