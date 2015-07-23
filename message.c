#include "self.h"

#include <arpa/inet.h>
#include <openssl/hmac.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
//TODO: consolidate headers
#include "hash.h"
#include "peer.h"

/// type definitions
union request {
    // find_peer requests information about a specific fingerprint
    // find_peer is sent to the peer with the given fingerprint on first
    // communication, see struct message about key exchange
    struct { hash fingerprint; } find_peer;
    // find_file includes a file's hash, and returns a list of nearby peers,
    // or if the requestee knows owners of the file, a list of owners along
    // with more nearby peers if there is room left in list_max
    // if find_file is sent to an owner of the file, the owner responds
    // with a list of more owners/nearby peers just like above, but also
    // sets up a bulk file transfer to begin download of the file by the
    // requester
    struct { hash file_hash; } find_file;
    // store_ref is used request to a peer nearby a given file hash to store
    // this node's fingerprint/file hash pair in their remote file table
    // optionally, the requestee can forward the fingerprint/hash pair to
    // any peers it knows even closer to the file
    struct {
        hash file_hash;
        // file_owner field used when forwarding
        hash file_owner;
    } store_ref;
    // command is used by a client, and is always encrypted
    struct {} command;
    // resend is request only
    struct { uint32_t sequence_no; } resend;
};
union response {
    // find_peer replies with a list of peers close to the requested, or a table
    // of info about the peer
    // when a node sends a find_peer request with a fingerprint request of the
    // same peer it is being sent to, the peer responds with its public key, a
    // signature of its fingerprint to authenticate, and an encrypted shared key
    // to be used hmac (the initial request included the requestee's public key)
    // all subsequent messages are authenticated by hmac
    struct {
        struct peer *list;
        // the following only on first communication
        byte public_key[KEY_LENGTH];
        byte signature[KEY_LENGTH];
        byte shared_key[HMAC_KEY_LENGTH];
    } find_peer;
    struct {} find_file;
    struct {} store_ref;
    struct {} command;
};
struct message {
    hash hmac;
    // used for received messages
    bool hmac_valid;
    // a given sequence_no uniquely identifies a message for that peer
    // note that since udp is unreliable, not all sequence numbers will be
    // received, and they won't necessarily be in order
    // the number can roll over, hence not truly unique
    uint32_t sequence_no;
    enum message_type type;
    hash fingerprint;
    union {
        union request request;
        union response response;
    };
    // for message queue
    struct message *next;
};

/// static function declarations
static hash message_hmac(byte *, buffer);
static void message_process(struct self *, struct message *);
static void message_free(struct message *);

// message_recv is the main thread for processing received messages
void *message_recv(void *arg){
    struct self *self = (struct self *) arg;
    while (1){
        int err = pthread_mutex_lock(&self->recv_queue.mutex);
        if (err); //error
        struct message *m = self->recv_queue.head;
        if (m){
            if (!m->next){
                self->recv_queue.head = NULL;
                self->recv_queue.tail = NULL;
            }
            else self->recv_queue.head = m->next;
        }
        err = pthread_mutex_unlock(&self->recv_queue.mutex);
        if (err); //error
        message_process(self, m);
        message_free(m);
    }
    return NULL;
}

// message_parse takes a buffer with network-order values and parses it into a
// message data structure
struct message *message_parse(struct self *self, buffer buf){
    struct message *m = calloc(1, sizeof(struct message));
    if (buf.length < (2 * DIGEST_LENGTH + 5)); //error
    byte *p = buf.data;
    m->hmac = hash_copy(p);
    p += DIGEST_LENGTH;
    // big-endian value in buf
    m->sequence_no = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    p += 4;
    enum { request, response } class = (*p & 0x80) >> 7;
    m->type = *p++ | 0x7f;
    if (m->type >= NUM_MESSAGE_TYPES); //error
    m->fingerprint = hash_copy(p);
    p += DIGEST_LENGTH;
    if (class == request){
        switch (m->type){
        case find_peer:
        case find_file:
        case store_ref:
            if (p + DIGEST_LENGTH > buf.data + buf.length); //error
            m->request.store_ref.file_hash = hash_copy(p);
            p += DIGEST_LENGTH;
            if (m->type == store_ref &&
                    p + DIGEST_LENGTH <= buf.data + buf.length){
                m->request.store_ref.file_owner = hash_copy(p);
                p += DIGEST_LENGTH;
            }
            break;
        case resend:
            if (p + 4 > buf.data + buf.length); //error
            m->request.resend.sequence_no = (p[0] << 24) | (p[1] << 16) |
                (p[2] << 8) | p[3];
            p += 4;
            break;
        default: break;
        }
    }
    if (p != buf.data + buf.length); //error, extraneous data
    struct peer *peer = peer_find(self->peers, m->fingerprint);
    if (peer){
        byte *key = peer->hmac_key;
        buffer hmac_buf = { buf.data + DIGEST_LENGTH, buf.length - DIGEST_LENGTH };
        hash hmac = message_hmac(key, hmac_buf);
        if (!hash_cmp(hmac, m->hmac)) m->hmac_valid = true;
    }
    return m;
}

// message_enqueue_recv hands off the received message to a thread for
// processing
void message_enqueue_recv(struct self *self, struct message *m){
    int err = pthread_mutex_lock(&self->recv_queue.mutex);
    if (err); //error
    if (self->recv_queue.tail){
        self->recv_queue.tail->next = m;
        self->recv_queue.tail = m;
        err = pthread_mutex_unlock(&self->recv_queue.mutex);
        if (err); //error
        return;
    }
    self->recv_queue.head = m;
    self->recv_queue.tail = m;
    err = pthread_mutex_unlock(&self->recv_queue.mutex);
    if (err); //error
}

// message_hmac computes the hmac for the raw data in a given message
hash message_hmac(byte *key, buffer buf){
    hash hmac = malloc(DIGEST_LENGTH);
    if (!hmac); //error
    if (!HMAC(EVP_ripemd160(), key, HMAC_KEY_LENGTH, buf.data, buf.length, hmac,
            NULL)); //error
    return hmac;
}

// message_process handles a received message
void message_process(struct self *self, struct message *m){
    
}

// message_free frees all resources associated with m
void message_free(struct message *m){
    //TODO
}

/*
// message_new initializes a new message for sending
struct message *message_new(struct self *self){
    struct message *m = calloc(1, sizeof(struct message));
    if (!m); //error
    return m;
}*/
