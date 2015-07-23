#ifndef PEER_H
#define PEER_H

#define HMAC_KEY_LENGTH 16

/// extern function declarations
struct peers *peer_create_list();
void peer_read_table(struct peers *);

#endif
