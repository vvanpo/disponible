
#include <string.h>
#include "self.h"

/// static function declarations
static bool check_integrity(struct message *);

// protocol_key_exchange_recv handles received key_exchange messages
void protocol_key_exchange_recv(struct self *self, struct message *m){
    byte *data = m->data;
    struct peer *peer = m->peer;
    // key_exchange is requested on first communication with a peer
    if (m->class == request){
        if (m->length != PUB_KEY_LENGTH); // peer/network error
        // if peer is unknown, add it to the list
        if (!peer){
            peer = peer_add(self->peers, m->fingerprint);
            // if the list is full, reject the peer by non-response
            if (!peer){
                free(data);
                free(m);
                return;
            }
            memcpy(peer->fingerprint, m->fingerprint, DIGEST_LENGTH);
        }
        // update peer last-seen time
        time(&peer->last_recv);
        // create new hmac key
        util_hmac_key(peer->hmac_key);
        memcpy(&peer->addr, m->addr, sizeof(struct address));
        if (!peer->rsa_public_key)
            peer->rsa_public_key = util_rsa_pub_decode(data);
        free(m);
        m = message_new(response, key_exchange, peer);
        util_rsa_encrypt(data, peer->rsa_public_key, peer->hmac_key,
                HMAC_KEY_LENGTH);
        // send encrypted hmac key
        message_send(self, m, data, RSA_MODULUS_LENGTH);
        m = message_new(response, key_exchange, peer);
        util_rsa_pub_encode(data, self->rsa_key);
        // send public key
        message_send(self, m, data, PUB_KEY_LENGTH);
        m = message_new(response, key_exchange, peer);
        util_rsa_sign(data, self->fingerprint, DIGEST_LENGTH, self->rsa_key);
        // send signature
        message_send(self, m, data, RSA_MODULUS_LENGTH);
        free(data);
        return;
    }
    // key_exchange responds with an encrypted shared key (encrypted with the
    // requester's public key) to be used for hmac on all messages
    // next the responder sends its public key, and finally a signature of its
    // fingerprint for authentication
    if (m->sequence_no == 1 && m->length == RSA_MODULUS_LENGTH){
        if (!check_integrity(m)) return;
        byte tmp[RSA_MESSAGE_MAX_LENGTH];
        if (util_rsa_decrypt(tmp, self->rsa_key, m->data) != HMAC_KEY_LENGTH);
            //error, resend
        memcpy(peer->hmac_key, tmp, HMAC_KEY_LENGTH);
    }
    else if (m->sequence_no == 2 && m->length == PUB_KEY_LENGTH){
        if (!check_integrity(m)) return;
        // check if public key matches fingerprint
        byte hash[DIGEST_LENGTH];
        hash_digest(hash, data, PUB_KEY_LENGTH);
        if (hash_cmp(m->fingerprint, hash)); // peer error
        peer->rsa_public_key = util_rsa_pub_decode(data);
    }
    else if (m->sequence_no == 3 && m->length == RSA_MODULUS_LENGTH){
        if (!check_integrity(m)) return;
        if (!util_rsa_verify(m->data, m->fingerprint, DIGEST_LENGTH,
                    peer->rsa_public_key)); // peer error
    }
    else ; //error
    free(m);
    free(data);
}

// protocol_key_exchange_req sends a key_exchange request to 'addr'
void protocol_key_exchange_req(struct self *self, byte *fingerprint,
        struct address *addr){
        // temporary peer for message_new() and message_send()
        struct peer peer = { 0 };
        memcpy(&peer.addr, addr, sizeof(struct address));
        // message_send increments sequence_no before sending
        peer.sequence_no = -1;
        struct message *m = message_new(request, key_exchange, &peer);
        byte *data = malloc(PUB_KEY_LENGTH);
        if (!data); // system error
        util_rsa_pub_encode(data, self->rsa_key);
        message_send(self, m, data, PUB_KEY_LENGTH);
}

void protocol_resend_req(struct self *self, byte *fingerprint,
        uint32_t sequence_no){

}

// check_integrity updates peer structure if message is correct
bool check_integrity(struct message *m){
        if (m->sequence_no > m->peer->sequence_no)
            m->peer->sequence_no = m->sequence_no;
        else {
            // ignore message if incorrect sequence_no
            free(m->data);
            free(m);
            return false;
        }
        // update peer last-seen time
        time(&m->peer->last_recv);
        // update peer address
        m->peer->addr.ip_version = m->addr->ip_version;
        memcpy(m->peer->addr.ip, m->addr->ip, 16);
        m->peer->addr.udp_port = m->addr->udp_port;
        return true;
}
