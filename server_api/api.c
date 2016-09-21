#include <libdsp.h>

struct dsp_self {
    int listener;
}

struct dsp_node dsp_find_node (unsigned char *finger, struct dsp_self *self)
{
    struct dsp_node node;

    return node;
}

unsigned char *dsp_list_known_nodes (int *len, struct dsp_self *self)
{
    return NULL;
}
