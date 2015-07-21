#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>
#include "util.h"

#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH

/// type definitions
typedef byte hash[DIGEST_LENGTH];

void hash_digest(hash, void *, int);
int hash_file_digest(hash, char *);
char *hash_base64_encode(void *, int);
byte *hash_base64_decode(char *, int *);
char *hash_digest_base64(hash);

#endif
