#ifndef PEER_H
#define PEER_H

#include <stdint.h>
#include "hash.h"

#define DEFAULT_BUCKET_SIZE 20
#define DEFAULT_BUCKET_MAX_DEPTH 4
#define HMAC_KEY_LENGTH 16

/// type definitions
struct address {
    char *fqdn;
    enum { ipv4, ipv6 } ip_version;
    byte ip[16];
    int udp_port;
    int tcp_port;
};
struct peer {
    hash fingerprint;
    byte *public_key;
    struct address address;
    // current sequence number in communication with this peer
    uint32_t sequence_no;
    byte hmac_key[HMAC_KEY_LENGTH];
    //TODO: known file list for this node
    struct peer *next;
    struct peer *prev;
};

/// extern function declarations
struct peers *peer_create_list();
void peer_read_table(struct peers *);
struct peer *peer_find(struct peers *, hash);

#endif
