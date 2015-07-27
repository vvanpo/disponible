#include "self.h"

#include <pthread.h>
#include <string.h>
#include <fcntl.h>

/// static function declarations
static void message_process(struct self *, struct message *);

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
    self->recv_queue.head = self->recv_queue.tail = m;
    err = pthread_cond_signal(&self->recv_queue.empty);
    if (err); //error
    err = pthread_mutex_unlock(&self->recv_queue.mutex);
    if (err); //error
}

// message_dequeue_recv is the main thread for processing received messages
void *message_dequeue_recv(void *arg){
    struct self *self = (struct self *) arg;
    while (1){
        int err = pthread_mutex_lock(&self->recv_queue.mutex);
        if (err); //error
        if (!self->recv_queue.head){
            err = pthread_cond_wait(&self->recv_queue.empty,
                    &self->recv_queue.mutex);
            if (err); //error
        }
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
    }
    return NULL;
}

// message_parse takes a buffer with network-order values and parses it into a
// message data structure
struct message *message_parse(struct self *self, byte *buf, int length){
    struct message *m = calloc(1, sizeof(struct message));
    time(&m->time);
    if (length < (2 * DIGEST_LENGTH + 5)); // peer error
    byte *p = buf;
    memcpy(m->hmac, p, DIGEST_LENGTH);
    p += DIGEST_LENGTH;
    // big-endian value in buf
    m->sequence_no = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    p += 4;
    m->class = (*p & 0x80) >> 7;
    m->type = *p++ | 0x7f;
    if (m->type >= NUM_MESSAGE_TYPES); //error
    memcpy(m->fingerprint, p, DIGEST_LENGTH);
    p += DIGEST_LENGTH;
    if (m->class == request){
        switch (m->type){
        case key_exchange:
            if (p + PUB_KEY_LENGTH > buf + length); //error
            memcpy(m->request.key_exchange.public_key, p, PUB_KEY_LENGTH);
            p += PUB_KEY_LENGTH;
            break;
        case find_peer:
        case find_file:
        case store_ref:
            if (p + DIGEST_LENGTH > buf + length); //error
            memcpy(m->request.store_ref.file_hash, p, DIGEST_LENGTH);
            p += DIGEST_LENGTH;
            if (m->type == store_ref &&
                    p + DIGEST_LENGTH <= buf + length){
                memcpy(m->request.store_ref.file_owner, p, DIGEST_LENGTH);
                p += DIGEST_LENGTH;
            }
            break;
        case resend:
            if (p + 4 > buf + length); //error
            m->request.resend.sequence_no = (p[0] << 24) | (p[1] << 16) |
                (p[2] << 8) | p[3];
            p += 4;
            break;
        default: break;
        }
    }
    if (p != buf + length); //error, extraneous data
    //TODO: hand-off to recv_message thread before the following to reduce
    // processing in the main read loop
    m->peer = peer_find(self->peers, m->fingerprint);
    if (m->peer){
        byte hmac[DIGEST_LENGTH];
        util_hmac(hmac, buf + DIGEST_LENGTH, length - DIGEST_LENGTH,
                m->peer->hmac_key);
        if (!hash_cmp(hmac, m->hmac)) m->hmac_valid = true;
    }
    return m;
}

// message_process handles a received message
void message_process(struct self *self, struct message *m){
    if (m->class == request){
        if (m->type == key_exchange) protocol_key_exchange_recv(self, m);
    }
    /*
    else if (m->sequence_no == 0){
        if (m->class != request || m->type != find_peer) return; //error
        if (hash_cmp(m->request.find_peer.fingerprint, self->fingerprint))
            return; //error
        buffer public_key = { m->request.find_peer.public_key, KEY_LENGTH };
        hash h = hash_digest(public_key);
        if (hash_cmp(m->fingerprint, h)) return; //error
        free(h);
        if (!m->peer){
            m->peer = peer_add(self->peers, m->fingerprint);
            memcpy(m->peer->public_key, public_key.data, KEY_LENGTH);
        }
        // if the peer is already in the node's list, this could be a fake
        // request
        // as such the node should not alter its known info about the peer until
        // it can verify it is legitimate, i.e. after the peer successfully
        // decrypts the response
        m->peer->state = state_waiting;
        m->peer->waiting = m;
        //TODO: send response
        return;
    }
    else if (m->sequence_no == 1){
        if (!m->peer) return; //error
        if (m->class != response || m->type != find_peer) return; //error
        buffer public_key = { m->response.find_peer.public_key, KEY_LENGTH };
        hash h = hash_digest(public_key);
        if (hash_cmp(m->peer->fingerprint, h)) return; //error
        free(h);
        memcpy(m->peer->hmac_key, m->response.find_peer.shared_key,
                HMAC_KEY_LENGTH);
        //TODO: find a clean way of working with variable max udp payloads
        //  lengths, and therefore splitting datagrams up
        //  OR, just use TCP for key exchange
        m->peer->waiting = m;
    }
    if (!m->hmac_valid){
        if (!m->peer){
-            //TODO: send 'resend sequence_no 0' request
-            return;
-        }
-        return; //error
-    }
-    if (m->peer->sequence_no < m->sequence_no)
-        m->peer->sequence_no = m->sequence_no;
-    m->peer->address.ip_version = m->address.ip_version;
-    memcpy(m->peer->address.ip, m->address.ip, 16);
-    m->peer->address.udp_port =  m->address.udp_port;
-}
 

*/}

// message_free frees all resources associated with m
void message_free(struct message *m){
    //TODO
}

// message_new initializes a new message for sending
struct message *message_new(enum message_class c, enum message_type t,
        struct peer *peer){
    struct message *m = calloc(1, sizeof(struct message));
    if (!m); //error
    m->peer = peer;
    memcpy(&m->address, &peer->address, sizeof(struct address));
    m->sequence_no = ++peer->sequence_no;
    m->class = c;
    m->type = t;
    memcpy(m->fingerprint, peer->fingerprint, DIGEST_LENGTH);
    return m;
}
