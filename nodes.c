#include <stdlib.h>

#include "self.h"

struct bucket_element {
    struct node *node;
    struct bucket_element *next;
};

struct nodes {
    unsigned char host_fingerprint[HASH_LENGTH];
    struct bucket_element *buckets[8 * HASH_LENGTH - 1];
};

struct nodes *new_nodes (char *host_fingerprint)
{
    struct nodes *nodes = calloc(1, sizeof(struct nodes));
    strcpy(nodes->host_fingerprint, host_fingerprint);
    return nodes;
}

struct nodes *load_nodes (char *path)
{
    struct nodes *nodes = calloc(1, sizeof(struct nodes));
    //TODO
    return nodes;
}

void destroy_nodes (struct nodes *nodes)
{
    for (int i = 0; i < 8 * HASH_LENGTH - 1; i++) {
        struct bucket_element *elem = nodes->buckets[i];
        while (elem) {
            struct bucket_element *next = elem->next;
            free(elem->node);
            free(elem);
            elem = next;
        }
    }
    free(nodes);
}

void add_node (struct node *node, struct nodes *nodes)
{
    
}
