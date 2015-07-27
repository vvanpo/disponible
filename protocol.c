
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <stdlib.h>
#include <string.h>
#include "self.h"

void protocol_key_exchange_recv(struct self *self, struct message *m){
    if (m->class == request){
        byte hash[DIGEST_LENGTH];
        hash_digest(hash, m->request.key_exchange.public_key, PUB_KEY_LENGTH);
        if (hash_cmp(m->fingerprint, hash)); // peer error
        struct peer *peer = m->peer;
        // if peer is unknown, add it to the list
        if (!peer){
            peer = peer_add(self->peers, m->fingerprint);
            // if the list is full, reject the peer by non-response
            if (!peer){
                message_free(m);
                return;
            }
            memcpy(peer->fingerprint, m->fingerprint, DIGEST_LENGTH);
        }
        util_hmac_key(peer->hmac_key);
        memcpy(&peer->address, &m->address, sizeof(struct address));
        if (!peer->rsa_public_key)
            peer->rsa_public_key =
                util_rsa_pub_decode(m->request.key_exchange.public_key);
        message_free(m);
        m = message_new(response, key_exchange, peer);

        return;
    }
}
