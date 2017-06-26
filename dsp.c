#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dsp.h"

dsp_error dsp_init (char const *path, struct dsp **dsp)
{
    dsp_error err;
    if (!(*dsp = calloc(1, sizeof(struct dsp)))) {
        err = error(DSP_E_SYSTEM);
        log_error(err);
        return err;
    }
    if (chdir(path)) {
        if (errno != ENOENT || !mkdir(path, 0640) || !chdir(path)) {
            err = error(DSP_E_SYSTEM);
            log_error(err);
            return err;
        }
    }
    if (err = db_open(&(*dsp)->db)) {
        log_error(err = trace(err));
        return err;
    }
    pthread_t listener;
    int ret = pthread_create(&listener, NULL, (void * (*)(void *)) net_listen,
            *dsp);
    if (ret) {
        //TODO doesn't set errno
        return error(DSP_E_SYSTEM);
    }
    return NULL;
}

dsp_error dsp_close (struct dsp *dsp)
{
    dsp_error err = db_close(dsp->db);
    if (err) return trace(err);
    free(dsp);
    return NULL;
}
