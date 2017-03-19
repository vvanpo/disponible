#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <nacl/crypto_box.h>
#include <nacl/crypto_hash.h>

#include "self.h"

#define PUBLIC_KEY_LENGTH crypto_box_PUBLICKEYBYTES
#define PRIVATE_KEY_LENGTH crypto_box_SECRETKEYBYTES

struct keys {
    unsigned char public[PUBLIC_KEY_LENGTH];
    unsigned char private[PRIVATE_KEY_LENGTH];
};

void hash (unsigned char *out, void *in, int length)
{
    assert(crypto_hash_BYTES >= HASH_LENGTH);
    unsigned char h[crypto_hash_BYTES];
    crypto_hash(h, in, length);
    memcpy(out, h, HASH_LENGTH);
}

void base64_encode (char *out, void const *in, size_t length)
{
    
}

int base64_decode (void *out, size_t *length, char const *in)
{
    
}

struct keys *new_keys ()
{
    struct keys *keys = malloc(sizeof(struct keys));
    crypto_box_keypair(keys->private, keys->public);
    return keys;
}

void destroy_keys (struct keys *keys)
{
    free(keys);
}
