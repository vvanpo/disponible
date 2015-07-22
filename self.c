/// implementing header
#include "self.h"

#include <stdlib.h>
#include "file.h"
#include "hash.h"
#include "peer.h"
#include "util.h"

/// type definitions
struct self {
    hash fingerprint;
    buffer public_key;
    buffer private_key;
    buffer *authorized_keys;
    struct peers *peers;
    struct files *files;
    struct {
        int udp_port;
        int tcp_port;
    } config;
};

/// static function declarations
static void load_config(struct self *);

// self_load loads all the necessary data structures into self required for
// starting the daemon
struct self *self_load(){
    struct self *self = calloc(1, sizeof(struct self));
    if (!self); //error
    load_config(self);
    self->private_key = read_file("keys/private");
    self->public_key = read_file("keys/public");
    self->fingerprint = hash_digest(self->public_key);
    self->peers = peer_create_list();
    peer_read_table(self->peers);
    self->files = file_create_list();
    file_read_table(self->files);
    return self;
}

// self_start_daemon starts the main run loop
void self_start_daemon(struct self *self){
    peer_listen(self->peers);
}

// load_config sets self->config values to those in the config file, or the
// defaults if absent
void load_config(struct self *self){
    //buffer file = read_file("config");
}

