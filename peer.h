#ifndef PEER_H
#define PEER_H

/// type definitions
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

#endif
