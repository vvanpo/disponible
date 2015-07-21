/// implementing header
//#include "peer.h"

#include <stdlib.h>
#include "hash.h"
#include "util.h"

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
    //TODO: known file list for this node
    struct peer *next;
    struct peer *prev;
};
struct peers {
    // peers are sorted into the buckets that correspond with their distance
    // a peer is sorted into bucket n if 2^n <= distance(peer) < 2^(n+1)
    // peers are sorted within the bucket according to last activity and uptime
    int num_buckets;
    // peers per bucket
    int bucket_size;
    struct peer **buckets;
    //TODO: remote file table (with hashes close to this node) with pointers to
    // known peers
};

/// static function declarations
static struct peers *create_peer_list();
static struct peer *bucket_lookup(struct peers *, byte *);
static int add_peer(struct peers *, struct peer *);
static int remove_peer(struct peers *, struct peer *);
static int read_peer_table(struct peers *);
static int write_peer_table(struct peers *);

// create_peer_list initializes a struct peers object
struct peers *create_peer_list(){
    struct peers *peers = malloc(sizeof(struct peers));
    if (!peers); //error
    peers->bucket_size = 20;
    peers->num_buckets = 1;
    peers->buckets = calloc(1, sizeof(struct peer *));
    return peers;
}

// bucket_lookup returns the head of the list of the bucket corresponding to key
struct peer *bucket_lookup(struct peers *peers, byte *key){
    return NULL;
}

// add_peer adds a peer to its respective bucket
int add_peer(struct peers *peers, struct peer *peer){
    return 0;
}

