#include "self.h"

#include <string.h>
#include <fcntl.h>

/// static function declarations
static struct message *parse(struct self *, byte *, int);
static void process(struct self *, struct message *);

// message_recv_start initializes a recv thread
void *message_recv_start(void *arg){
    struct self *self = (struct self *) arg;
    while (1){
        int err = pthread_mutex_lock(&self->udp_recv_mutex);
        if (err); //error
        // predicate
        if (!self->udp_recv_buf || !self->udp_recv_addr
                || !self->udp_recv_length){
            err = pthread_cond_wait(&self->udp_recv_cond,
                    &self->udp_recv_mutex);
            if (err); //error
        }
        byte *buf = self->udp_recv_buf;
        struct address *addr = self->udp_recv_addr;
        int length = self->udp_recv_length;
        // reset predicate and unlock mutex
        self->udp_recv_buf = NULL;
        self->udp_recv_addr = NULL;
        self->udp_recv_length = 0;
        err = pthread_mutex_unlock(&self->udp_recv_mutex);
        if (err); //error
        // begin processing buffer
        struct message *m = parse(self, buf, length);
        m->addr = addr;
        process(self, m);
    }
    return NULL;
}

// message_new initializes a new message for sending
struct message *message_new(enum message_class c, enum message_type t,
        struct peer *peer){
    struct message *m = calloc(1, sizeof(struct message));
    if (!m); //error
    m->peer = peer;
    m->class = c;
    m->type = t;
    return m;
}

// message_send packs message m into a buffer and writes to self->udp_msg_socket
void message_send(struct self *self, struct message *m, byte *data, int length){
    byte buf[UDP_MAX_PAYLOAD];
    byte *p = buf + DIGEST_LENGTH;
    uint32_t s = ++m->peer->sequence_no;
    p[0] = s >>= 24;
    p[1] = s >>= 16;
    p[2] = s >>= 8;
    p[3] = s;
    p += 4;
    p[0] = m->class << 7;
    p++[0] |= m->type;
    memcpy(p, self->fingerprint, DIGEST_LENGTH);
    p += DIGEST_LENGTH;
    memcpy(p, data, length);
    length += DIGEST_LENGTH + 5;
    util_hmac(buf, buf + DIGEST_LENGTH, length, m->peer->hmac_key);
    struct sockaddr_storage sas;
    struct sockaddr *sa = util_get_sockaddr(&sas, &m->peer->addr);
    if (sendto(self->udp_msg_socket, buf, length + DIGEST_LENGTH, 0, sa,
                sizeof(sas)) == -1); //error
    free(m);
}

// parse takes a buffer with network-order values and parses it into a message
// data structure
struct message *parse(struct self *self, byte *buf, int length){
    struct message *m = calloc(1, sizeof(struct message));
    length -= (2 * DIGEST_LENGTH) - 5;
    if (length < 0); // peer error
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
    byte *data = malloc(length);
    memcpy(data, p, length);
    m->data = data;
    m->length = length;
    return m;
}

// process handles a received message
void process(struct self *self, struct message *m){
    struct bucket *b = &self->peers.root;
    m->peer = peer_find(&b, m->fingerprint);
    if (m->class == request && m->type == key_exchange)
        protocol_key_exchange_recv(self, m);
    else if (!m->peer || !m->peer->rsa_public_key){
        protocol_key_exchange_req(self, m->fingerprint, m->addr);
        free(m->data);
        free(m);
    }
    else {
        byte hmac[DIGEST_LENGTH];
        util_hmac(hmac, m->data, m->length, m->peer->hmac_key);
        if (hash_cmp(hmac, m->hmac) && m->sequence_no > m->peer->sequence_no){
            protocol_resend_req(self, m->fingerprint, m->sequence_no);
            free(m->data);
            free(m);
            return;
        }
        switch (m->type){
            default: break;
        }
    }
}
