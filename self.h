#ifndef SELF_H
#define SELF_H

#include <stdbool.h>

#define HASH_LENGTH 32

// config.c
    struct config;
    // new_config initializes a config structure with default values.
    struct config *new_config ();
    // load_config initializes a config structure using the values from
    //  <path>/config.
    struct config *load_config (char *path);
    // write_config overwrites <path>/config.
    void write_config (char *path, struct config *config);

// self.c
    struct self;
    struct self *new (char *path, struct config *config);
    struct self *load (char *path);
    void stop (struct self *self);

// crypto.c
    struct keys;
    struct keys *new_keys ();
    struct keys *load_keys (char *path);
    void write_keys (char *path, struct keys *keys);
    void hash (unsigned char *out, void *in, int length);

// nodes.c
    // The nodes structure is an organized list of peers with which the host has
    //  communicated.
    struct nodes;
    // new_nodes initializes an empty nodes structure.
    struct nodes *new_nodes ();
    // load_nodes initializes a nodes structure using the files in
    //  <path>/nodes/.
    struct nodes *load_nodes (char *path);
    // write_nodes overwrites the directory structure in <path>/nodes/.
    void write_nodes (char *path);
    // The node structure maintains information about a given peer.
    struct node;
    void add_node (struct node *node, struct nodes *nodes);
    void remove_node (struct node *node, struct nodes *nodes);
    struct node *return_node (char *fingerprint, struct nodes *nodes);
    // closest_nodes returns a null-terminated array of nodes closest to <hash>
    //  in <nodes>, up to a maximum of <limit> nodes.
    struct node *closest_nodes (char *fingerprint, int limit,
            struct nodes *nodes);

// msg.c
    // msg_find sends a message to the node(s) nearest to <hash>, requesting the
    //      resource identified by <hash>.  The messaged nodes will propagate
    //      the request through the network until a node owning the resource is
    //      found.  The requested resource can itself be a node.
    //  returns
    //      a null-terminated array of node objects
    struct node *msg_find (unsigned char *hash, struct self *self);
    // msg_verify exchanges public keys with <node> and verifies that it matches
    //      the fingerprint.
    bool msg_verify (struct node *node, struct self *self);
    void msg_send (void *payload, struct node *node, struct self *self);
    // msg_handler takes an incoming message from <node> and decides how to
    //      respond.
    void msg_handler (void *message, struct node *node, struct self *self);

// net.c
    // net_serve retrieves incoming messages and hands them off to msg_handler.
    void net_serve (struct self *self);
    int net_send (void *message, char *address, struct self *self);

#endif
