#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/socket.h>
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

static dsp_error parse_address (char *address, uint32_t *ip, uint16_t *port)
{
    return NULL;
}

//TODO: parse address to determine ipv4 or ipv6
//TODO: NAT hole-punching
dsp_error net_connect (struct dsp *dsp, char *address, struct connection **connection)
{
    if (!(*connection = malloc(sizeof(struct connection))))
        return error(DSP_E_SYSTEM);
    (*connection)->socket = socket(AF_INET, SOCK_STREAM, 0);
    if ((*connection)->socket == -1) {
        free(*connection);
        *connection = NULL;
        return error(DSP_E_SYSTEM);
    }
    uint32_t ip;
    uint16_t port;
    dsp_error err;
    if (err = parse_address(address, &ip, &port)) return trace(err);
    struct sockaddr_in sa = {AF_INET, port, ip};
    return NULL;
}
