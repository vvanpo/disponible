#ifndef NET_H
#define NET_H

#include <stdint.h>
#include "cryp.h"

struct address {
	struct sockaddr_in6 *sa;
	char *fqdn;
	uint16_t sctp, tcp, udp;
};
struct session {
	int sockfd;
	unsigned char key[SYM_KEY_LEN];
};
struct peer;

int net_parse_addr(struct address *addr, char const *s);
void net_encode_addr(char **out, struct address *addr);
int net_connect(struct peer *peer);
int net_disconnect(struct peer *peer);

#endif
