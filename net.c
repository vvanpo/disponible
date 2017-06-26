#include <errno.h>
#include <sys/socket.h>

#include "dsp.h"

#define LISTEN_BACKLOG 128

dsp_error net_listen (struct dsp *dsp)
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) return error(DSP_E_SYSTEM);
    struct sockaddr_in address = {AF_INET, dsp->port, INADDR_ANY};
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
        if (err = handle_client(dsp, client, &client_address))
            return trace(err);
    }
    return NULL;
}

dsp_error handle_client (struct dsp *dsp, int client, struct sockaddr_in *client_address)
{
    return NULL;
}
