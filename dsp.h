#ifndef DSP_H
#define DSP_H

#define _POSIX_C_SOURCE 200809L 

#include <nacl/crypto_sign.h>
#include <nacl/crypto_box.h>
#include <nacl/crypto_secretbox.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include "libdsp.h"

#define HASH_LENGTH DSP_HASH_LENGTH
#define PUBLIC_KEY_LENGTH crypto_box_PUBLICKEYBYTES

struct dsp {
    pthread_mutex_t mutex;
    unsigned char *public_key;
    unsigned char *secret_key;
    struct db *db;
    char *address;
    int port;
    pthread_t listener;
    struct session **session;
};

// error.c
#define error(code, msg) new_error(code, msg)
#define sys_error(code, err, msg) new_system_error(code, err, msg)
#define db_error(err, msg) new_db_error(err, msg)
    dsp_error new_error (int code, char const *message);
    dsp_error new_system_error (int code, int err, char const *message);
    dsp_error new_db_error (int err, char const *message);
    // Writes error message to stderr.
    void log_error (dsp_error error);

// crypto.c
    // Hash functions
        struct hash *hash (void *in, size_t length);
        // hash_distance computes the distance function between two hashes,
        //  i.e. returning the byte-index at which the two hashes begin to
        //  diverge.
        int hash_distance (struct hash *from, struct hash *to);
    // Base-64 functions
        char *base64_encode (void *in, size_t length);
        unsigned char *base64_decode (char *in, size_t *length);
    // Public-key crypto functions
        dsp_error new_keypair (unsigned char **public, unsigned char **private);

// db.c
    dsp_error db_open (struct db **);
    dsp_error db_close (struct db *);
    dsp_error select_node (
        struct db *db,
        unsigned char *fingerprint,
        struct node **node
    );
    dsp_error insert_node (struct db *db, struct node *node);
    dsp_error update_node (struct db *db, struct node *node);

// net.c
    dsp_error net_listen (struct dsp *dsp);

// request.c
    dsp_error find_node (
        struct dsp *dsp,
        struct hash *fingerprint,   // the fingerprint to find
        struct node **node          // OUT: the returned node object
    );

// response.c


#endif
