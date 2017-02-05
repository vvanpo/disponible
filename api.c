#include "libdsp.h"
#include "self.h"

struct dsp_self *initialize ()
{
    pid_t pid = fork();
    if (pid == 0) start_server();
    else if (pid < 0) return NULL;
    return dsp_connect();
}

