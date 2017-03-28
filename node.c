#include "self.h"
#include "node.h"

void destroy_node (struct node *node)
{
    free(node->key);
    free(node->address);
    free(node);
}

struct public_key *node_key (struct node *node)
{
    return node->key;
}

char *node_address (struct node *node)
{
    return node->address;
}

void add_node (struct public_key *key, char *address, struct self *self)
{
    struct node *node = calloc(1, sizeof(struct node));
    node->key = key;
    node->address = address;
    int i = hash_distance(key_fingerprint(keys_public_key(self->keys)),
            key_fingerprint(key));
    node->bucket = self->nodes->buckets + i;
    node->next = self->nodes->buckets[i];
    self->nodes->buckets[i] = node;
    if (node->next)
        node->next->previous = node;
    // Ensure this addition doesn't put the bucket over max bucket length
    for (int j = 0; node; j++) {
        struct node *next = node->next;
        if (j >= config_bucket_length(self->config))
            destroy_node(node);
        node = next;
    }
}

void remove_node (struct node *node)
{
    if (node->next) node->next->previous = NULL;
    *node->bucket = node->next;
    destroy_node(node);
}

