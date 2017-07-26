#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include "dsp.h"

error dsp_init (char const *path, struct dsp **dsp)
{
    error err;
    if (!(*dsp = calloc(1, sizeof(struct dsp)))) {
        err = sys_error(DSP_E_SYSTEM, errno, "Failed to allocate dsp instance");
        //TODO: log level in config
        log_error(err);
        return err;
    }
    if (chdir(path)) {
        if (errno != ENOENT || !mkdir(path, 0640) || !chdir(path)) {
            char *msg = NULL;
            sprintf(msg, "Failed to access %s", path);
            err = sys_error(DSP_E_SYSTEM, errno, msg);
            log_error(err);
            return err;
        }
    }
    if (err = db_open(&(*dsp)->db)) {
        log_error(err);
        return err;
    }
    int ret = pthread_create(&(*dsp)->listener, NULL,
            (void * (*)(void *)) net_listen, *dsp);
    if (ret) return sys_error(DSP_E_SYSTEM, ret, "Failed to create listener");
    return NULL;
}

error dsp_close (struct dsp *dsp)
{
    //TODO: cancel threads
    error err = db_close(dsp->db);
    if (err) return err;
    free(dsp);
    return NULL;
}
