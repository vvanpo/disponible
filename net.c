#include <errno.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "self.h"

struct client {
    struct client *next;
    int sockfd;
};

//TODO: handle errors
void net_serve (struct self *self)
{
    int listener;
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return;
    struct sockaddr_in address = {AF_INET, 0, INADDR_ANY};
    if (!bind(listener, (struct sockaddr *) &address,
                sizeof(struct sockaddr_in)))
        return;
    if (!listen(listener, 128)) return;
    struct client *previous = NULL;
    while (1) {
        struct sockaddr_in client_address = {};
        struct client *client = calloc(1, sizeof(struct client));
        if ((client->sockfd = accept(sockfd,
                        (struct sockaddr *) &client_address,
                        sizeof(struct sockaddr_in))) < 0) {
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
            return;
        }
        if (previous) previous->next = client;
        previous = client;
    }
}

int net_send (void *message, char *address, struct self *self)
{

}
