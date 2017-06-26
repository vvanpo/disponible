#include "dsp.h"

void add_node (struct dsp *dsp, struct node *node)
{
    int bucket = hash_distance(hash(node->public_key, PUBLIC_KEY_LENGTH));
    node->next = dsp->bucket[bucket];
    dsp->bucket[bucket] = node;
    //TODO ensure bucket length maximum
    
}


