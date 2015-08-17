#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "peer.h"

// net_parse_addr takes a string of the form (fqdn|ipv4|ipv6):sctp:tcp:udp and
//   transforms it into an address structure.
//   On error, returns:
//   	ERR_NET_ADDR_INVALID
//   	ERR_SYSTEM
//   	ERR_CONN_FAILURE
int net_parse_addr(struct address *addr, char const *s)
{
	char *tmp = malloc(strlen(s) + 1);
	if (!tmp) return ERR_SYSTEM;
	memset(addr, 0, sizeof *addr);
	s = strcpy(tmp, s);
	struct sockaddr_in6 *sa = addr->sa = calloc(1, sizeof *sa);
	if (!sa) return ERR_SYSTEM;
	sa->sin6_family = AF_INET6;
	if (*s == '[') {
		tmp = strchr(s, ']');
		if (!tmp || tmp[1] != ':') goto parse_err;
		*tmp = '\0';
		if (!inet_pton(AF_INET6, s + 1, sa->sin6_addr.s6_addr))
			goto parse_err;
		tmp += 2;
	} else {
		if (!(tmp = strchr(s, ':'))) goto parse_err;
		*tmp = '\0';
		if (inet_pton(AF_INET, s, (char *) &sa->sin6_addr + 12))
			memset((char *) &sa->sin6_addr + 8, 0xff, 4);
		else {
			addr->fqdn = malloc(strlen(s) + 1);
			if (!addr->fqdn) return ERR_SYSTEM;
			strcpy(addr->fqdn, s);
			free(sa);
			sa = NULL;
		}
		tmp++;
	}
	int port;
	port = strtol(tmp, &tmp, 10);
	if (!*tmp || *tmp != ':' || port < 0 || port > 0xffff) goto parse_err;
	addr->sctp = port;
	tmp++;
	port = strtol(tmp, &tmp, 10);
	if (!*tmp || *tmp != ':' || port < 0 || port > 0xffff) goto parse_err;
	addr->tcp = port;
	tmp++;
	port = strtol(tmp, &tmp, 10);
	if (*tmp || port < 0 || port > 0xffff) goto parse_err;
	addr->udp = port;
	return 0;
parse_err:
	if (addr->fqdn) free(addr->fqdn);
	free((char *) s);
	if (sa) free(sa);
	return ERR_NET_ADDR_INVALID;
}

void net_encode_addr(char **out, struct address *addr)
{
	if (addr->fqdn) {
		*out = malloc(strlen(addr->fqdn) + 20);
		strcpy(*out, addr->fqdn);
	} else if (addr->sa) {
		*out = calloc(1, INET6_ADDRSTRLEN + 19);
		strcpy(*out, "[");
		inet_ntop(AF_INET6, &addr->sa->sin6_addr, *out + 1,
			INET6_ADDRSTRLEN);
		strcat(*out, "]");
	} else assert(false);
	sprintf(*out + strlen(*out), ":%d:%d:%d", addr->sctp, addr->tcp,
		addr->udp);
}
