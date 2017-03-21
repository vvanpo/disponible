#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "self.h"

static struct self *init (char *path)
{
    struct self *self = calloc(1, sizeof(struct self));
    if (!self) return NULL;
    // Copy <path> to self->path
    self->path = malloc(sizeof(char) * (strlen(path) + 1));
    if (!self->path) return NULL;
    strcpy(self->path, path);
    return self;
}

struct self *new (char *path, struct config *config)
{
    struct self *self = init(path);
    //TODO: use mkdirat
    if (!chdir(path)) {
        if (errno != ENOENT) return NULL;
        if (!mkdir(path, 0640)) return NULL;
        if (!chdir(path)) return NULL;
    }
    self->config = config;
    if (!(self->keys = new_keys())) return NULL;
    if (!(self->nodes = new_nodes(self->keys))) return NULL;
    net_serve(self);
    return self;
}

struct self *load (char *path)
{
    struct self *self = init(path);
    if (!(self->config = load_config(path))) return NULL;
    if (!(self->keys = load_keys(path))) return NULL;
    if (!(self->nodes = load_nodes(path))) return NULL;
    net_serve(self);
    return self;
}

void destroy (struct self *self)
{
    free(self->path);
    destroy_config(self->config);
    destroy_keys(self->keys);
    destroy_nodes(self->nodes);
    free(self);
}
