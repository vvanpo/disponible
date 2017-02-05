#ifndef SELF_H
#define SELF_H

#include <stddef.h>
#include <stdbool.h>
#include <nacl/crypto_box.h>
#include "libdsp.h"

#define PUBLIC_KEY_LEN crypto_box_PUBLICKEYBYTES
#define PRIVATE_KEY_LEN crypto_box_SECRETKEYBYTES
#define HASH_LENGTH DSP_HASH_LENGTH


// self.c
struct self;

// config.c
struct config;

// node.c
struct node_list;

// crypto.c
void hash (unsigned char *out, void const *in, size_t len);
void new_keypair (unsigned char *public, unsigned char *private);

#endif
