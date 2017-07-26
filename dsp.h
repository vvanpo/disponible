#ifndef DSP_H
#define DSP_H

#include <stddef.h>
#include <stdint.h>

#define _POSIX_C_SOURCE 200809L 

#include <pthread.h>
#include "libdsp.h"

#define HASH_LENGTH DSP_HASH_LENGTH

struct dsp {
    pthread_mutex_t mutex;
    unsigned char *public_key;
    unsigned char *private_key;
    struct db *db;
    char *address;
    uint16_t tcp_port;
    uint16_t udp_port;
    pthread_t listener;
    struct session **session;
};

// error.c
    typedef struct dsp_error error;
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
        error hash (
            unsigned char *in,
            size_t length,      // The number of bytes to read from <in>
            char *out           // Must point to a pre-allocated buffer of size
                                //  HASH_LENGTH
        );
        // hash_distance computes the distance function between two hashes,
        //  i.e. returning the byte-index at which the two hashes begin to
        //  diverge.
        int hash_distance (char *from, char *to);
    // Base-64 functions
        error encode_base64 (unsigned char *in, size_t length, char **out);
        error decode_base64 (char *in, size_t *length, unsigned char **out);
    // Asymmetric crypto functions
        error sign_keypair (
            unsigned char **public_key,
            unsigned char **private_key
        );
        error encrypt_keypair (
            unsigned char **public_key,
            unsigned char **private_key
        );
        error sign ();
        error verify_sign ();
        error encrypt ();
        error decrypt ();
    // Symmetric crypto functions

// db.c
    error db_open (struct db **);
    error db_close (struct db *);
    error select_node (
        struct db *db,
        unsigned char *fingerprint,
        struct node **node
    );
    error insert_node (struct db *db, struct node *node);
    error update_node (struct db *db, struct node *node);

// net.c
    error net_listen (struct dsp *dsp);

// request.c
    error lookup (
        struct dsp *dsp,
        struct hash *fingerprint,   // the fingerprint to find
        struct node **node          // OUT: the returned node object
    );

// response.c


#endif
