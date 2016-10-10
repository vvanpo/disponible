#include <unistd.h>

#include "../server.h"
#include "self.h"

struct dsp_host *initialize ()
{
    pid_t pid = fork();
    if (pid == 0) start_server();
    else if (pid < 0) return NULL;
    return dsp_connect();
}

void start_server ()
{
    struct self *self = calloc(1, sizeof(struct self));
    read_config(self);
}
