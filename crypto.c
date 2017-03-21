#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <nacl/crypto_box.h>
#include <nacl/crypto_hash.h>

#include "self.h"

#define PUBLIC_KEY_LENGTH crypto_box_PUBLICKEYBYTES
#define PRIVATE_KEY_LENGTH crypto_box_SECRETKEYBYTES

struct hash {
    unsigned char hash[HASH_LENGTH];
};

// Utility functions

struct hash *hash (void *in, size_t length)
{
    assert(crypto_hash_BYTES >= HASH_LENGTH);
    unsigned char h[crypto_hash_BYTES];
    crypto_hash(h, in, length);
    struct hash *out = malloc(sizeof(struct hash));
    memcpy(out->hash, h, HASH_LENGTH);
    return out;
}

int hash_distance (struct hash *from, struct hash *to)
{
    for (int i = 0; i < HASH_LENGTH; i++)
        if (from->hash[i] != to->hash[i]) return HASH_LENGTH - i;
    return 0;
}

void base64_encode (char *out, void *in, size_t length)
{
    
}

void base64_decode (void *out, size_t *length, char *in)
{
    
}

// Public key functions

struct public_key {
    unsigned char key[PUBLIC_KEY_LENGTH];
};

struct hash *key_fingerprint (struct public_key *key)
{
    return hash(key->key, PUBLIC_KEY_LENGTH);
}

// Key-pair functions

struct private_key {
    unsigned char key[PRIVATE_KEY_LENGTH];
};

struct keys {
    struct public_key public;
    struct private_key private;
};

struct keys *new_keys ()
{
    struct keys *keys = malloc(sizeof(struct keys));
    crypto_box_keypair(keys->private.key, keys->public.key);
    return keys;
}

void destroy_keys (struct keys *keys)
{
    free(keys);
}

struct public_key *keys_public_key (struct keys *keys)
{
    return &keys->public;
}

