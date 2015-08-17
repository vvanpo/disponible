#ifndef NET_H
#define NET_H

#include <stdint.h>

struct address {
	struct sockaddr_in6 *sa;
	char *fqdn;
	uint16_t sctp, tcp, udp;
};

int net_parse_addr(struct address *addr, char const *s);
void net_encode_addr(char **out, struct address *addr);

#endif
