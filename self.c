/// implementing header
#include "self.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
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
static void self_listen(struct self *);

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
    self_listen(self);
}

// load_config sets self->config values to those in the config file, or the
// defaults if absent
void load_config(struct self *self){
    //buffer file = read_file("config");
    self->config.udp_port = 1024;
}

// self_listen opens the configured socket and begins a listener thread to
// service requests from peers
void self_listen(struct self *self){
    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd == -1); //error
    in_port_t port = htons(self->config.udp_port);
    struct sockaddr_in6 addr = { AF_INET6, port, 0, IN6ADDR_ANY_INIT, 0 };
    socklen_t addrlen = sizeof(addr);
    if (bind(sockfd, (struct sockaddr *) &addr, addrlen)); //error
    //TODO: attempt bind with port 0 if denied due to in-use port
    buffer buf = { malloc(4096), 4096 };
    if (!buf.data); //error
    while (1){
        struct sockaddr_storage addr;
        addrlen = sizeof(addr);
        int ret = recvfrom(sockfd, buf.data, buf.length, MSG_TRUNC,
                (struct sockaddr *) &addr, &addrlen);
        if (ret == -1); //error
        if (buf.length < ret); //error/increase buffer and request resend
        
    }
    if (close(sockfd)); //error
}

