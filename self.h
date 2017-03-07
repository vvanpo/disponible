#ifndef SELF_H
#define SELF_H

#include <stdbool.h>

#define HASH_LENGTH 32

struct self;
struct config;
struct keys;
struct nodes;

// self.c
    struct self *new (char *dir, struct config *config);
    struct self *load (char *dir);
    void stop (struct self *self);

// config.c
    struct config *new_config ();
    struct config *load_config ();
    void write_config ();

// crypto.c
    struct keys *new_keys ();
    struct keys *load_keys ();
    void write_keys ();
    void hash (unsigned char *out, void *in, int length);

// nodes.c
    // new_nodes creates a new, empty list of peers
    struct nodes *new_nodes ();
    // load_nodes creates a list of peers using the files in $DSP/nodes
    struct nodes *load_nodes ();
    void write_nodes ();

// msg.c

    // msg_find sends a message to the node(s) nearest to <hash>, requesting the
    //      resource identified by <hash>.  The messaged nodes will propagate
    //      the request through the network until a node owning the resource is
    //      found.
    //  returns
    //      a null-terminated array of node objects
    struct node *msg_find (unsigned char *hash, struct nodes *nodes);

    // msg_verify exchanges public keys with <node> and verifies that it matches
    //      the fingerprint.
    bool msg_verify (struct node *node);
    void msg_send (void *payload, struct node *node);
    // msg_handler takes an incoming message from <node> and decides how to
    //      respond.
    void msg_handler (void *message, struct node *node);

// net.c

    // net_serve retrieves incoming messages and hands them off to msg_handler.
    void net_serve (struct self *self);
    int net_send (void *message, char *address, struct self *self);

#endif
