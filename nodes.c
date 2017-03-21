#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "self.h"
#include "node.h"

struct nodes {
    struct node *buckets[8 * HASH_LENGTH - 1];
};

// Extern functions

struct nodes *new_nodes ()
{
    return calloc(1, sizeof(struct nodes));
}

struct nodes *load_nodes (char *path)
{
    struct nodes *nodes = new_nodes();
    //TODO
    return nodes;
}

void destroy_nodes (struct nodes *nodes)
{
    for (int i = 0; i < 8 * HASH_LENGTH - 1; i++) {
        struct node *node = nodes->buckets[i];
        while (node) {
            struct node *next = node->next;
            destroy_node(node);
            node = next;
        }
    }
    free(nodes);
}

void write_nodes (char *path)
{

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

void bump_node (struct node *node)
{
    if (!node->previous) return;
    node->previous->next = node->next;
    if (node->next) node->next->previous = node->previous;
    node->previous = NULL;
    node->next = *node->bucket;
    // Assert bucket is non-empty
    assert(node->next);
    node->next->previous = node;
    *node->bucket = node;
}

struct node *return_node (struct hash *fingerprint, struct self *self)
{
    int i = hash_distance(key_fingerprint(keys_public_key(self->keys)),
            fingerprint);
    struct node *node = self->nodes->buckets[i];
    while (node) {
        if (!hash_distance(key_fingerprint(node->key), fingerprint))
            return node;
        node = node->next;
    }
    return NULL;
}

struct node *closest_nodes (struct hash *hash, int limit, struct self *self)
{
    int i = hash_distance(key_fingerprint(keys_public_key(self->keys)), hash);
    struct node **array = calloc(limit + 1, sizeof(struct node *));
    struct node *node = self->nodes->buckets[i];
    for (int j = 0; j < limit;) {
        if (node) {
            array[j++] = node;
            node = node->next;
            continue;
        }
        int offset = node->bucket - self->nodes->buckets - i;
        offset = (offset >= 0) ? 0 - offset - 1 : 0 - offset;
        if (i + offset < 0) {
            if (i - offset > 8 * HASH_LENGTH - 1) return *array;
            node = self->nodes->buckets[i - offset + 1];
        } else if (i + offset > 8 * HASH_LENGTH - 1) {
            if (i - offset == 0) return *array;
            node = self->nodes->buckets[i - offset - 1];
        } else {
            node = self->nodes->buckets[i + offset];
        }
    }
    return *array;
}
