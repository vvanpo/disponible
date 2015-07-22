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
static void parse_message(buffer);

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
    //TODO: open a separate UDP socket for exclusive use with bulk file
    //  tranfers (the result of a successful rq_find_file request)
    //  this is to ensure that bulk transfers don't crowd out incoming messages
    //  if it fills the socket buffer
    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd == -1); //error
    in_port_t port = htons(self->config.udp_port);
    struct sockaddr_in6 addr = { AF_INET6, port, 0, IN6ADDR_ANY_INIT, 0 };
    socklen_t addrlen = sizeof(addr);
    if (bind(sockfd, (struct sockaddr *) &addr, addrlen)); //error
    //TODO: attempt bind with port 0 if denied due to in-use port
    //TODO: find optimal buffer size
    buffer buf = { malloc(4096), 4096 };
    if (!buf.data); //error
    while (1){
        struct sockaddr_storage addr;
        addrlen = sizeof(addr);
        int ret = recvfrom(sockfd, buf.data, buf.length, MSG_TRUNC,
                (struct sockaddr *) &addr, &addrlen);
        if (ret == -1); //error
        if (buf.length < ret); //error
        parse_message(buf);
    }
    if (close(sockfd)); //error
}

// if the requestee doesn't have the public key of the requester to
// verify the signature, it sends back a rq_find_peer request
struct request {
    // rq_find_peer requests information about a specific fingerprint
    //      the response consists of a list of peers close to the requested, or
    //      a table of info about the peer
    //      rq_find_peer sent to the peer with that same fingerprint always
    //      returns a table of info (like the peer's public key)
    // rq_find_file includes a file's hash, and returns a list of nearby peers,
    //      or if the requestee knows owners of the file, a list of owners along
    //      with more nearby peers if there is room left in list_max
    //      if rq_find_file is sent to an owner of the file, the owner responds
    //      with a list of more owners/nearby peers just like above, but also
    //      sets up a bulk file transfer to begin download of the file by the
    //      requester
    // rq_store_ref is used request to a peer nearby a given file hash to store
    //      this node's fingerprint/file hash pair in their remote file table
    //      optionally, the requestee can forward the fingerprint/hash pair to
    //      any peers it knows even closer to the file
    // rq_command is used by a client, and is always signed and encrypted
    enum { rq_find_peer, rq_find_file, rq_store_ref, rq_command } type;
    hash fingerprint;
    byte *hmac; //TODO: figure out the best way for authentication
                //      perhaps hmacs for all messages other than the response
                //      to rq_find_peer that sends a peer's public key?
    union {
        struct {
        } find_peer;
        struct {
        } find_file;
        struct {
        } store_ref;
        struct {
        } command;
    };
};

void parse_message(buffer buf){
    
}

