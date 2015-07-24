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
    // bits past prefix_length are 0
    hash prefix;
    // head of the peer list, non-NULL only for leaf buckets
    // the test for a leaf bucket is a non-NULL head, so empty leaves must get
    // merged
    struct peer *head;
    // number of peers in (leaf) bucket
    int count;
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
static struct bucket *find_bucket(struct bucket *, hash);
static void split_bucket(struct peers *, struct bucket *);
static void merge_buckets(struct bucket *);

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

// peer_add initializes a peer in its respective bucket
struct peer *peer_add(struct peers *peers, hash fingerprint){
    struct peer *peer = calloc(1, sizeof(struct peer));
    peer->fingerprint = fingerprint;
    int err = pthread_mutex_init(&peer->mutex, NULL);
    if (err); //error
    struct bucket *b = find_bucket(&peers->root, fingerprint);
    if (!b->head) b->head = peer;
    else {
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
        if (err); //error
        peer->next = b->head;
        b->head->prev = peer;
        b->head = peer;
        b->count++;
    }
    return peer;
}

// peer_remove deletes a peer from the list
void peer_remove(struct peers *peers, struct peer *peer){
    if (!peer->prev){
        struct bucket *b = find_bucket(&peers->root, peer->fingerprint);
        if (!peer->next){
            b->head = NULL;
            b = b->parent;
            merge_buckets(b);
        }
        else b->head = peer->next;
    }
    else peer->prev->next = peer->next;
    if (peer->next)
        peer->next->prev = peer->prev;
    int err = pthread_mutex_destroy(&peer->mutex);
    if (err); //error
    free(peer->fingerprint);
    free(peer);
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

// split_bucket splits the passed bucket if < max_depth
// only to be called on a leaf bucket
void split_bucket(struct peers *peers, struct bucket *b){
    if (b->depth == peers->max_depth) return;
    struct bucket *left = calloc(1, sizeof(struct bucket));
    if (!b->left); //error
    struct bucket *right = calloc(1, sizeof(struct bucket));
    if (!b->right); //error
    int pref_len = b->prefix_length + 1;
    left->prefix_length = right->prefix_length = pref_len;
    left->depth = right->depth = b->depth + 1;
    left->parent = right->parent = b;
    left->prefix = hash_copy(b->prefix);
    left->prefix[pref_len / 8] |= 1 << (8 - (pref_len % 8));
    right->prefix = hash_copy(b->prefix);
    left->right = right;
    right->left = left;
    if (b->left){
        b->left->right = left;
        left->left = b->left;
    }
    if (b->right){
        b->right->left = right;
        right->right = b->right;
    }
    b->left = left;
    b->right = right;
    struct peer *p_last_left = NULL;
    struct peer *p_last_right = NULL;
    struct peer *p_next;
    for (struct peer *p = b->head; p; p = p_next){
        p_next = p->next;
        if (hash_cmp(p->fingerprint, left->prefix) < 0){
            if (!p_last_right) right->head = p;
            p->prev = p_last_right;
            p_last_right->next = p;
            right->count++;
        }
        else {
            if (!p_last_left) left->head = p;
            p->prev = p_last_left;
            p_last_left->next = p;
            left->count++;
        }
    }
    p_last_left->next = NULL;
    p_last_right->next = NULL;
    b->head = NULL;
    b->count = 0;
}

// merge_buckets merges two leaves with the same parent
//TODO: merge keeping the LRU property (only if ever used with both buckets non-
// empty)
void merge_buckets(struct bucket *parent){
    if (parent->left->head){
        parent->head = parent->left->head;
        parent->count = parent->left->count;
    }
    if (parent->right->head){
        if (parent->head){
            struct peer *last;
            for (last = parent->head; last->next; last = last->next);
            last->next = parent->right->head;
            parent->right->head->prev = last;
        }
        else parent->head = parent->right->head;
        parent->count += parent->right->count;
    }
    free(parent->left->prefix);
    free(parent->left);
    free(parent->right->prefix);
    free(parent->right);
    parent->left = parent->right = NULL;
}
