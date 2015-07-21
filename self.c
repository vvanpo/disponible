/// implementing header
#include "self.h"

#include <stdlib.h>
#include "peer.h"
#include "util.h"

/// static function declarations
//TODO: config file, allowing for alternate key files and the like

struct self *self_load_config(){
    //buffer file = read_file("config");
    struct self *self = malloc(sizeof(struct self));
    if (!self); //error
    self->private_key = read_file("keys/private");
    self->public_key = read_file("keys/public");
    hash_digest(self->fingerprint, self->public_key);
    self->peers = peer_create_list();
    return self;
}
