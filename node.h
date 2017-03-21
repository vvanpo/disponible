#ifndef NODE_H
#define NODE_H

struct node {
    struct node **bucket;
    struct node *previous;
    struct node *next;

    struct public_key *key;
    char *address;
};

void destroy_node (struct node *node);

#endif
