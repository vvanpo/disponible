#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "self.h"

struct self {
    char *dir;
    struct config *config;
    struct keys *keys;
    struct nodes *nodes;
    struct server *server;
};

static struct self *init (char *dir)
{
    struct self *self = calloc(1, sizeof(struct self));
    if (!self) return NULL;
    self->dir = malloc(sizeof(char) * (strlen(dir) + 1));
    if (!self->dir) return NULL;
    strcpy(self->dir, dir);
    return self;
}

struct self *new (char *dir, struct config *config)
{
    struct self *self = init(dir);
    if (!chdir(dir)) {
        if (errno != ENOENT) return NULL;
        if (!mkdir(dir, 0640)) return NULL;
        if (!chdir(dir)) return NULL;
    }
    self->config = config;
    if (!(self->keys = new_keys())) return NULL;
    if (!(self->nodes = new_nodes())) return NULL;
    if (!(self->server = net_serve(self))) return NULL;
    return self;
}

struct self *load (char *dir)
{
    struct self *self = init(dir);
    if (!chdir(dir)) return NULL;
    if (!(self->config = load_config())) return NULL;
    if (!(self->keys = load_keys())) return NULL;
    if (!(self->nodes = load_nodes())) return NULL;
    if (!(self->server = net_serve(self))) return NULL;
    return self;
}
