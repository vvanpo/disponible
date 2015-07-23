/// implementing header
#include "peer.h"

#include <stdlib.h>
#include "self.h"

/// type definitions
struct bucket {
    // buckets are sorted in an ordered binary tree
    // the root bucket has prefix_length = 0, and therefore depth = 0
    int prefix_length;
    // non-local depth, i.e. the distance up the tree to a local bucket (one
    // containing the local node)
    int depth;
    // bytes past prefix[prefix_length] are 0
    hash prefix;
    // head of the peer list, non-NULL only for leaf buckets
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
static struct bucket *find_bucket(struct peers *, hash);
//static void add_peer(struct peers *, struct peer *);
//static void remove_peer(struct peers *, struct peer *);

// peer_create_list initializes a peers object
struct peers *peer_create_list(){
    struct peers *peers = calloc(1, sizeof(struct peers));
    if (!peers); //error
    peers->bucket_size = DEFAULT_BUCKET_SIZE;
    peers->max_depth = DEFAULT_BUCKET_MAX_DEPTH;
    peers->root.prefix = calloc(1, DIGEST_LENGTH);
    if (!peers->root.prefix); //error
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

// peer_find traverses the peer list and returns the peer corresponding to the
// passed fingerprint, or NULL if absent
struct peer *peer_find(struct peers *peers, hash fingerprint){
    struct bucket *b = find_bucket(peers, fingerprint);
    struct peer *peer = b->head;
    for (; peer; peer = peer->next)
        if (!hash_cmp(peer->fingerprint, fingerprint)) return peer;
    return NULL;
}


// bucket_lookup returns the bucket matching the hash
struct bucket *find_bucket(struct peers *peers, hash h){
    struct bucket *b = &peers->root;
    while (!b->head){
        hash masked = malloc(DIGEST_LENGTH);
        if (!masked); //error
        hash_distance(masked, h, b->left->prefix);
        if (hash_cmp(h, b->left->prefix) < 0) b = b->right;
        else b = b->left;
    }
    return b;
}

/*
// add_peer adds a peer to its respective bucket
void add_peer(struct peers *peers, struct peer *peer){
}*/
