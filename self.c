#include <stdlib.h>

#include "self.h"

struct self {
    struct config *config;
    struct keys *keys;
    struct nodes *nodes;
};

struct self *self_init ()
{
    struct self *self = calloc(1, sizeof(struct self));
    self->config = config_read();
    self->keys = keys_read();
    return self;
}
