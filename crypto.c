#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <nacl/crypto_box.h>
#include <nacl/crypto_hash.h>

#include "dsp.h"

#define HASH_LENGTH DSP_HASH_LENGTH
#define PUBLIC_KEY_LENGTH crypto_box_PUBLICKEYBYTES
#define PRIVATE_KEY_LENGTH crypto_box_SECRETKEYBYTES

// Hash functions

struct hash {
    unsigned char hash[HASH_LENGTH];
};

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

// Base-64 functions

char *base64_encode (void *in, size_t length)
{
    unsigned char *input = (unsigned char *) in;
    char *output = malloc((4 * ((length + 2) / 3) + 1) * sizeof(char));
    for (int i = 0, j = 0; i < length; i += 3){
        unsigned int group = input[i] << 16;
        if (length - i > 1) group += input[i + 1] << 8;
        if (length - i > 2) group += input[i + 2];
        for (int k = 0; k < 4; k++) {
            unsigned char c = (group & (0x3f << (18 - k * 6))) >> (18 - k * 6);
            if (c <= 25) c += 'A';
            else if (c <= 51) c += 'a' - 26;
            else if (c <= 61) c = '0' - 52;
            else if (c == 62) c = '+';
            else c = '/';
            if (((length - i == 1) && k >= 2) || ((length - i == 2) && k == 3))
                output[j + k] = '=';
            else
                output[j + k] = c;
        }
        j += 4;
    }
    // Null-terminate string
    output[4 * ((length + 2) / 3)] = '\0';
    return output;
}

unsigned char *base64_decode (char *in, size_t *length)
{
    size_t input_length = strlen(in);
    assert(input_length % 4 == 0);
    *length = 3 * input_length / 4;
    if (in[input_length - 1] == '=') *length -= 1;
    if (in[input_length - 2] == '=') *length -= 1;
    unsigned char *output = malloc(*length * sizeof(unsigned char));
    for (int i = 0, j = 0; i < input_length; i += 4){
        int group = 0;
        for (int k = 0; k < 4; k++){
            int n = 0, c = (int) in[i + k];
            if ('A' <= c && c <= 'Z') n = c - 'A';
            else if ('a' <= c && c <= 'z') n = c - 'a' + 26;
            else if ('0' <= c && c <= '9') n = c - '0' + 52;
            else if (c == '+') n = 62;
            else if (c == '/') n = 63;
            else if (c == '=') break;
            else assert(false);
            group += n << (18 - k * 6);
        }
        for (int k = 0; k < 3 && k + j < *length; k++){
            output[k + j] = 0xff & (group >> (16 - k * 8));
        }
        j += 3;
    }
    return output;
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

struct keys {
    struct public_key public;
    unsigned char private_key[PRIVATE_KEY_LENGTH];
};

struct keys *new_keys ()
{
    struct keys *keys = malloc(sizeof(struct keys));
    crypto_box_keypair(keys->private_key, keys->public.key);
    return keys;
}

struct public_key *public_key (struct keys *keys)
{
    return &keys->public;
}

