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
