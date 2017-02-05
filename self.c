#include <stdlib.h>

#include "self.h"

struct self {
    struct config *config;
    struct node_list *node_list;
    unsigned char public_key[PUBLIC_KEY_LEN];
    unsigned char private_key[PRIVATE_KEY_LEN];
};

struct self *self_init ()
{
    struct self *self = calloc(1, sizeof(struct self));
    self->config = config_read();
    read_keys(self);
    return self;
}

void read_keys (struct self *self)
{
    new_keypair(self->public_key, self->private_key);
}
