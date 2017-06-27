#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "dsp.h"

#define LISTEN_BACKLOG 128

static dsp_error delegate_client (struct dsp *dsp, int client, struct sockaddr_in *client_address)
{
    return NULL;
}

dsp_error net_listen (struct dsp *dsp)
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) return error(DSP_E_SYSTEM);
    // Port in network order
    uint16_t port_n = htons(dsp->port);
    struct sockaddr_in address = {AF_INET, port_n, INADDR_ANY};
    if (bind(listener, (struct sockaddr *) &address,
            sizeof(struct sockaddr_in))) return error(DSP_E_SYSTEM);
    if (listen(listener, LISTEN_BACKLOG)) return error(DSP_E_SYSTEM);
    int client;
    struct sockaddr_in client_address;
    while (client = accept(listener, (struct sockaddr *) &client_address,
                (socklen_t *) sizeof(struct sockaddr_in))) {
        if (client < 0) {
            switch (errno) {
            case ENETDOWN:
            case EPROTO:
            case ENOPROTOOPT:
            case EHOSTDOWN:
            case ENONET:
            case EHOSTUNREACH:
            case EOPNOTSUPP:
            case ENETUNREACH:
                continue;
            }
            return error(DSP_E_SYSTEM);
        }
        dsp_error err;
        if (err = delegate_client(dsp, client, &client_address))
            return trace(err);
    }
    return NULL;
}

//TODO: parse address to determine ipv4 or ipv6
static dsp_error parse_address (char *address, struct addrinfo **res)
{
    char *colon = strrchr(address, ':');
    if (!colon) {
        //TODO: return an "invalid network address" error
        return error(DSP_E_NETWORK);
    }
    int i = colon - address;
    char *host = malloc((i + 1) * sizeof(char));
    memcpy(host, address, i);
    host[i] = '\0';
    char *port = address + i + 1;
    struct addrinfo hints = {0};
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(host, port, &hints, res);
    if (ret) {
        *res = NULL;
        free(host);
        char const *err_msg = gai_strerror(ret);
        //TODO
        return error(DSP_E_NETWORK);
    }
    free(host);
    return NULL;
}

//TODO: NAT hole-punching
//TODO: initialize thread first so as not to block on address lookup and connect()
dsp_error net_connect (struct dsp *dsp, char *address, struct connection **connection)
{
    if (!(*connection = malloc(sizeof(struct connection)))) {
        return error(DSP_E_SYSTEM);
    }
    struct addrinfo *res, *rp;
    dsp_error err = parse_address(address, &res);
    if (err) {
        free(*connection);
        *connection = NULL;
        return trace(err);
    }
    for (rp = res; rp; rp = rp->ai_next) {
        (*connection)->socket = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if ((*connection)->socket == -1) continue;
        if (!connect((*connection)->socket, rp->ai_addr, rp->ai_addrlen))
            break;
        if (close((*connection)->socket)) {
            freeaddrinfo(res);
            free(*connection);
            *connection = NULL;
            return error(DSP_E_SYSTEM);
        }
    }
    if (!rp) {
        freeaddrinfo(res);
        free(*connection);
        *connection = NULL;
        return error(DSP_E_NETWORK);
    }
    freeaddrinfo(res);
    return NULL;
}
