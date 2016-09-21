#include <stdlib.h>

#include "node.h"
#include "crypto.h"

struct elem {
    struct elem *last;
    struct elem *next;
    struct node {
        unsigned char [HASH_LEN] finger;
    } node;
};

struct list {
    int bucket_len;
    struct elem *[HASH_LEN] bucket;
};

struct list *new_list (int bucket_len)
{
    struct list *list = calloc(1, sizeof struct list);
    return list;
}

struct node *find_node (unsigned char *finger, struct list *list)
{
    
}

int add_node (unsigned char *finger, struct list *list)
{
    return 0;
}
