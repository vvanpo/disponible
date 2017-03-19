#include "self.h"

struct node {
    unsigned char fingerprint[HASH_LENGTH];
    struct public_key *key;
    unsigned char *address;
};

