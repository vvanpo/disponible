#ifndef SELF_H
#define SELF_H

#include <stddef.h>
#include <stdbool.h>
#include "../libdsp.h"

#define HASH_LEN DSP_HASH_LENGTH

struct node;
struct file;

struct self {
    struct {
        // ephemeral instances are memory-only, except for files
        bool ephemeral;
        // bootstrap node is used to populate an empty stored nodes list
        struct node *bootstrap;
        int node_bucket_sz_exp;
    } config;
    // stored nodes list
    struct node *stored_nodes[HASH_LEN * 8 - 1];
    // stored files list
    struct file *stored_files;
};

// self.c

// node.c

// file.c

// crypto.c
void hash (unsigned char *out, void const *in, size_t len);
void encode_b64 (char *out, void const *in, size_t len);
int decode_b64 (void *out, size_t *len, char const *in);
int generate_keys (void **keys);

#endif
