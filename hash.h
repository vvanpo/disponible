#ifndef HASH_H
#define HASH_H

#include <openssl/ripemd.h>

struct hash {
    unsigned char md[RIPEMD160_DIGEST_LENGTH];
};

void hash_buffer(struct hash *, void *, int);
int hash_file(struct hash *, char *);
char *base64(void *, int);

#endif
