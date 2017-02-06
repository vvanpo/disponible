#include <errno.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "self.h"

struct client {
    struct client *next;
    int sockfd;
}

struct server {
    int listener;
    struct client *client;
};

struct server *net_serve (struct self *self)
{
    struct server *server = malloc(sizeof(struct server));
    if ((server->listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return NULL;
    struct sockaddr_in address = {AF_INET, 0, INADDR_ANY};
    if (!bind(server->listener, &address, sizeof(struct sockaddr_in)))
        return NULL;
    if (!listen(server->listender, 128)) return NULL;
    struct client *previous = NULL;
    while (1) {
        struct sockaddr_in client_address = {};
        struct client *client = calloc(1, sizeof(struct client));
        if ((client->sockfd = accept(sockfd, client_address,
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
            return NULL;
        }
        if (previous) previous->next = client;
        previous = client;
    }
}

int net_send (void *message, char *address, struct self *self)
{

}
