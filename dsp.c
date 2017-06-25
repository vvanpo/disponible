#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include <unistd.h>

#include "dsp.h"

char *dsp_error (int code)
{
    char *prefix = "System error: ";
    switch (code) {
    case DSP_OK: return "Success";
    case DSP_ESYSTEM:
        strncpy(error_msg, prefix, ERR_BUF);
        // Only fails on invalid errno or insufficient buffer size
        assert(!strerror_r(errno, error_msg + strlen(prefix),
            ERR_BUF - strlen(prefix)));
        break;
    }
    return error_msg;
}

struct dsp {
    struct keys *keys;
    struct db *db;
};

int dsp_init (char *path, struct dsp **dsp)
{
    int err;
    *dsp = calloc(1, sizeof(struct dsp));
    if (!*dsp) return DSP_ESYSTEM;
    if (!chdir(path)) {
        if (errno != ENOENT || !mkdir(path, 0640) || !chdir(path))
            return DSP_ESYSTEM;
    }
    if (err = db_open(&(*dsp)->db)) return err;
    return DSP_OK;
}
