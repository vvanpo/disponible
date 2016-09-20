#ifndef NODES_H
#define NODES_H

#include <crypto.h>

struct node {
    unsigned char finger[HASH_LEN];
    struct address addr;
    void *public_key;
};

#endif
