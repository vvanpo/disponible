#ifndef PEER_H
#define PEER_H

#include "node.h"
#include "cryp.h"

struct address {

};

struct peer {
	unsigned char finger[HASH_LEN];
	struct address addr;
	struct peer *next;
	struct peer *prev;
};

void peer_get(struct peer **peer, unsigned char *finger, struct node *node);
int peer_add(unsigned char *finger, struct node *node);
int peer_remove(struct peer *peer, struct node *node);

#endif
