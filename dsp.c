#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dsp.h"

struct dsp {
    struct keys *keys;
    struct db *db;
};

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
    return NULL;
}
