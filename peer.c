/// implementing header
#include "self.h"

#include <stdlib.h>

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
    // head->mutex doubly serves as the mutex for the whole bucket
    struct peer *head;
    int count;
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
static struct bucket *find_bucket(struct bucket *, hash);
static void split_bucket(struct peers *, struct bucket *);

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
    struct bucket *b = find_bucket(&peers->root, fingerprint);
    struct peer *peer = b->head;
    for (; peer; peer = peer->next)
        if (!hash_cmp(peer->fingerprint, fingerprint)) return peer;
    return NULL;
}

// add_peer initializes a peer in its respective bucket
struct peer *peer_add(struct peers *peers, hash fingerprint){
    struct peer *peer = calloc(1, sizeof(struct peer));
    peer->fingerprint = fingerprint;
    int err = pthread_mutex_init(&peer->mutex, NULL);
    if (err); //error
    struct bucket *b = find_bucket(&peers->root, fingerprint);
    if (!b->head) b->head = peer;
    else {
        //TODO: macros for mutexes
        err = pthread_mutex_lock(&b->head->mutex);
        if (err); //error
        if (b->count == peers->bucket_size){
            split_bucket(peers, b);
            if (b->head){
                // bucket couldn't be split
                //TODO
                return peer;
            }
            b = find_bucket(b, fingerprint);
        }
        err = pthread_mutex_lock(&peer->mutex);
        if (err); //error
        peer->next = b->head;
        b->head->prev = peer;
        b->head = peer;
        b->count++;
        err = pthread_mutex_unlock(&b->head->mutex);
        if (err); //error
        err = pthread_mutex_unlock(&peer->mutex);
        if (err); //error
    }
    return peer;
}

// find_bucket returns the bucket matching the hash, starting its search from
// the passed bucket (usually &peers->root)
struct bucket *find_bucket(struct bucket *b, hash h){
    while (!b->head){
        if (hash_cmp(h, b->left->prefix) < 0) b = b->right;
        else b = b->left;
    }
    return b;
}

// split_bucket splits the passed bucket if below max_depth
void split_bucket(struct peers *peers, struct bucket *b){

}
