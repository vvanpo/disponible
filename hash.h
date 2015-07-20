#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>

#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH

/// type definitions
typedef struct hash {
    unsigned char md[DIGEST_LENGTH];
} hash;

void hash_buffer_digest(hash *, void *, int);
int hash_file_digest(hash *, char *);
char *hash_base64(void *, int);
char *hash_digest_base64(hash *);

#endif
