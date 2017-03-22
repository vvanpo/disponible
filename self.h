#ifndef SELF_H
#define SELF_H

#include <stdbool.h>
#include <stdlib.h>

#define HASH_LENGTH 32

struct self;

// config.c
    struct config;
    // new_config initializes a config structure with default values.
    struct config *new_config ();
    // load_config initializes a config structure using the values from
    //  <path>/config.
    struct config *load_config (char *path);
    void destroy_config (struct config *config);
    // write_config overwrites <path>/config.
    void write_config (char *path, struct config *config);
    int config_bucket_length (struct config *config);

// crypto.c
    struct hash;
    // Utility functions
        struct hash *hash (void *in, size_t length);
        // hash_distance computes the distance function between two hashes,
        //  i.e. returning the byte-index at which the two hashes begin to
        //  diverge.
        int hash_distance (struct hash *from, struct hash *to);
        char *base64_encode (void *in, size_t length);
        unsigned char *base64_decode (char *in, size_t *length);
    // Public key functions
        struct public_key;
        struct public_key *new_public_key (void *in);
        struct hash *key_fingerprint (struct public_key *key);
    // Key-pair functions
        struct keys;
        struct keys *new_keys ();
        struct keys *load_keys (char *path);
        void destroy_keys (struct keys *keys);
        void write_keys (char *path, struct keys *keys);
        struct public_key *keys_public_key (struct keys *keys);

// node.c
    // The node structure maintains information about a given peer.
    struct node;
    char *node_address (struct node *node);

// nodes.c
    // The nodes structure is an organized list of peers with which the host has
    //  communicated.
    struct nodes;
    // new_nodes initializes an empty nodes structure.
    struct nodes *new_nodes ();
    // load_nodes initializes a nodes structure using the files in
    //  <path>/nodes/.
    struct nodes *load_nodes (char *path);
    void destroy_nodes (struct nodes *nodes);
    // write_nodes overwrites the directory structure in <path>/nodes/.
    void write_nodes (char *path);
    void add_node (struct public_key *key, char *address, struct self *self);
    void remove_node (struct node *node);
    // bump_node moves <node> to the top of its bucket in self->nodes.
    void bump_node (struct node *node);
    // return_node returns the node from self->nodes with the given fingerprint.
    struct node *return_node (struct hash *fingerprint, struct self *self);
    // closest_nodes returns a null-terminated array of nodes closest to <hash>
    //  in <nodes>, up to a maximum of <limit> nodes.
    struct node *closest_nodes (struct hash *hash, int limit,
            struct self *self);

// msg.c
    // msg_find sends a message to the node(s) nearest to <hash>, requesting the
    //      resource identified by <hash>.  The messaged nodes will propagate
    //      the request through the network until a node owning the resource is
    //      found.  The requested resource can itself be a node.
    //  returns
    //      a null-terminated array of node objects claiming to own the
    //      requested resource.
    struct node *msg_find (struct hash *hash, struct self *self);
    void msg_send (void *payload, struct node *node, struct self *self);
    // msg_handler takes an incoming message from <node> and decides how to
    //      respond.
    void msg_handler (void *message, struct node *node, struct self *self);

// net.c
    // net_serve retrieves incoming messages and hands them off to msg_handler.
    void net_serve (struct self *self);
    int net_send (void *message, char *address, struct self *self);

// self.c
    struct self {
        char *path;
        struct config *config;
        struct keys *keys;
        struct nodes *nodes;
    };
    struct self *new (char *path, struct config *config);
    struct self *load (char *path);
    void destroy (struct self *self);

#endif
