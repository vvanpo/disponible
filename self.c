/// implementing header
#include "self.h"

#include <stdlib.h>
#include "util.h"

/// static function declarations
//TODO: config file, allowing for alternate key files and the like

struct self *self_load_config(){
    //byte *file = read_file("config", &l);
    struct self *self = malloc(sizeof(struct self));
    self->private_key = read_file("keys/private");
    self->public_key = read_file("keys/public");
    
    return self;
}
