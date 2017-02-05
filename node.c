#include "self.h"

struct node {
    struct node *prev;
    struct node *next;
    unsigned char fingerprint[HASH_LENGTH];
    unsigned char public_key[PUBLIC_KEY_LENGTH];
    unsigned char *address;
    unsigned char *secret_key[HASH_LENGTH];
};

struct node_list {
    struct node *buckets[HASH_LENGTH - 1];
};
