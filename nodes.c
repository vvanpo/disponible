#include "self.h"

struct node {
    unsigned char fingerprint[HASH_LENGTH];
    struct public_key *key;
    unsigned char *address;
};

struct nodes {
    struct {
        struct node *next;
        struct node *node;
    } buckets[8 * HASH_LENGTH - 1];
};


