/// implementing header
#include "peer.h"

#include <stdint.h>
#include <stdlib.h>
#include "hash.h"
#include "self.h"

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
struct bucket {
    // buckets are sorted in a ordered binary tree
    // the root bucket has prefix_length = 0, and therefore depth = 0
    // only leaf buckets have a non-NULL peer list
    int prefix_length;
    // non-local depth, i.e. the distance up the tree to a local bucket (one
    // containing the local node)
    int depth;
    // sub_prefix corresponds to the bits from
    //      [ (prefix_length - depth), prefix_length ]
    int sub_prefix;
    // head of the peer list
    struct peer *head;
    struct bucket *parent;
    // a leaf bucket still make use of child pointers, by using them as a
    // shortcut to the nearest leaf buckets, i.e. all leaf buckets are ordered
    // in a doubly-linked list
    struct bucket *left;
    struct bucket *right;
};
struct peers {
    // peers are sorted into the buckets that correspond with their distance
    // a peer is sorted into bucket n if 2^n <= distance(peer) < 2^(n+1)
    // peers per bucket
    int bucket_size;
    // the number of times a non-local bucket (one not containing this node's
    // hash) can be split
    int max_depth;
    // tree of buckets
    struct bucket root;
    //TODO: remote file table (with hashes close to this node) with pointers to
    // known peers
};

/// static function declarations
//static void write_peer_table(struct peers *);
//static struct bucket *bucket_lookup(struct peers *, hash);
//static void add_peer(struct peers *, struct peer *);
//static void remove_peer(struct peers *, struct peer *);

// peer_create_list initializes a peers object
struct peers *peer_create_list(){
    struct peers *peers = calloc(1, sizeof(struct peers));
    if (!peers); //error
    peers->bucket_size = 20;
    peers->max_depth = 4;
    return peers;
}

// peer_read_table loads the local peer table and updates peers
// the local table is arranged as one file per leaf bucket
// the directory hierarchy mimics the tree structure of buckets, but skips
// nodes in groups of 8, i.e. up to 2^8 nodes in a given directory
// nodes (directories and bucket files) are named corresponding to their sub-
// prefix with respect to their parent node (in hexadecimal)
void peer_read_table(struct peers *peers){
}

/*
// bucket_lookup returns the bucket matching the hash
struct bucket *bucket_lookup(struct peers *peers, hash hash){
    return NULL;
}

// add_peer adds a peer to its respective bucket
void add_peer(struct peers *peers, struct peer *peer){
}*/
