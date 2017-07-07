#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "dsp.h"

#define LISTEN_BACKLOG 128

/// Static functions

static dsp_error parse_address (char *address, struct addrinfo **res)
{
    char *colon = strrchr(address, ':');
    if (!colon) {
        return error(DSP_E_NETWORK, "Invalid network address, needs to be of "
                "the form <host>:<port>");
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
        return error(DSP_E_NETWORK, err_msg);
    }
    free(host);
    return NULL;
}

static void *client (void *arg)
{
    struct connection *conn = arg;
    int ret = pthread_mutex_lock(&conn->mutex);
    if (ret) return sys_error(DSP_E_SYSTEM, ret, NULL);
    while (1) {
        // The client blocks until it is signalled that there is a request in
        //  its buffer
        while (!conn->buffer) {
            if (ret = pthread_cond_wait(&conn->cond, &conn->mutex))
                return sys_error(DSP_E_SYSTEM, ret, NULL);
        }
        // Send request
        dsp_error err = send_request(conn);
        if (err) {
            if (ret = pthread_mutex_unlock(&conn->mutex))
                return sys_error(DSP_E_SYSTEM, ret, NULL);
            return err;
        }
    }
    return NULL;
}

static dsp_error handle (struct dsp *dsp, int client, struct sockaddr_in *client_address)
{
    return NULL;
}

/// Extern functions

//TODO: allow ipv6
dsp_error net_listen (struct dsp *dsp)
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) return sys_error(DSP_E_SYSTEM, errno,
            "Failed to open listener network socket");
    // Port in network order
    uint16_t port = htons(dsp->port);
    struct sockaddr_in address = {AF_INET, port, INADDR_ANY};
    if (bind(listener, (struct sockaddr *) &address,
                sizeof(struct sockaddr_in)))
        return sys_error(DSP_E_SYSTEM, errno, "Failed to bind listener port");
    if (listen(listener, LISTEN_BACKLOG))
        return sys_error(DSP_E_SYSTEM, errno, NULL);
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
            return sys_error(DSP_E_SYSTEM, errno,
                    "Failed to accept connection");
        }
        dsp_error err;
        if (err = handle(dsp, client, &client_address)) return err;
    }
    return NULL;
}

//TODO: NAT hole-punching
dsp_error net_connect (char *address, struct connection **connection)
{
    if (!(*connection = calloc(1, sizeof(struct connection)))) {
        return sys_error(DSP_E_SYSTEM, errno, "Failed to allocate connection object");
    }
    (*connection)->address = address;
    struct addrinfo *res, *rp;
    // Perform an address lookup
    dsp_error err = parse_address(address, &res);
    if (err) {
        free(*connection);
        *connection = NULL;
        return err;
    }
    // Run through address list until connection succeeds
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
            return sys_error(DSP_E_SYSTEM, errno,
                    "Failed to close connection socket");
        }
    }
    if (!rp) {
        freeaddrinfo(res);
        free(*connection);
        *connection = NULL;
        char *msg = NULL;
        sprintf(msg, "Connection to <%s> failed", address);
        return error(DSP_E_NETWORK, msg);
    }
    freeaddrinfo(res);
    if (err = handshake(*connection)) return err;
    // Initialize client thread
    int ret = pthread_create(&(*connection)->thread, NULL, client,
            *connection);
    if (ret) {
        free(*connection);
        *connection = NULL;
        return sys_error(DSP_E_SYSTEM, ret, "Failed to create connection thread");
    }
    return NULL;
}

dsp_error net_disconnect (struct connection *connection)
{
    return NULL;
}
