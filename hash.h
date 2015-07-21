#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>

#define DIGEST_LENGTH RIPEMD160_DIGEST_LENGTH

/// type definitions
typedef struct {
    unsigned char md[DIGEST_LENGTH];
} hash;

void hash_digest(hash *, void *, int);
int hash_file_digest(hash *, char *);
char *hash_base64_encode(void *, int);
unsigned char *hash_base64_decode(char *, int *);
char *hash_digest_base64(hash *);

#endif
