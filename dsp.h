#ifndef DSP_H
#define DSP_H

#define _POSIX_C_SOURCE 200809L 

#include "libdsp.h"

#define HASH_LENGTH DSP_HASH_LENGTH

#define ERR_BUF 1024
char error_msg[ERR_BUF];

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
        struct hash *key_fingerprint (struct public_key *key);
    // Key-pair functions
        struct keys;
        struct keys *new_keys ();
        struct public_key *public_key (struct keys *keys);

// db.c
    struct db;
    int db_open (struct db **);
    int db_close (struct db *);

/*
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
    void msg_handler (void *message, struct node *node, struct dsp *);

// net.c
    // net_serve retrieves incoming messages and hands them off to msg_handler.
    void net_serve (struct dsp *);
    int net_send (void *message, char *address, struct dsp *);

// node.c
*/
#endif
