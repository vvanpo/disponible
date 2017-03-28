#ifndef NODE_H
#define NODE_H

struct node {
    struct node **bucket;
    struct node *previous;
    struct node *next;

    struct public_key *key;
    char *address;
};

struct nodes {
    struct node *buckets[8 * HASH_LENGTH - 1];
};

void destroy_node (struct node *node);

#endif
