#include <assert.h>
#include <errno.h>
#include <math.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dsp.h"

struct dsp_error {
    int code;
    char *message;
};

int dsp_error_code (dsp_error err)
{
    assert(err);
    if (!err) return DSP_E_INVALID;
    return err->code;
}

char *dsp_error_message (dsp_error err)
{
    assert(err);
    if (!err) return "Invalid error";
    return err->message;
}

void dsp_error_free (dsp_error err)
{
    assert(err);
    if (!err) return;
    free(err->message);
    free(err);
}

#define BUF_SZ 1024
dsp_error new_error (int code, char const *message)
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = code;
    if (message) {
        char const *prefix = "Error: ";
        int i = strlen(prefix);
        err->message = malloc(i + strlen(message) + 1);
        strcpy(err->message, prefix);
        strcpy(err->message + i, message);
    }
    return err;
}
dsp_error new_system_error (int code, int sys_err, char const *message)
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = code;
    char const *prefix = "System error: ";
    int i = strlen(prefix);
    int l = i + BUF_SZ;
    if (message) l += strlen(message) + 2;
    err->message = malloc(l);
    strcpy(err->message, prefix);
    if (message) {
        strcpy(err->message + i, message);
        i += strlen(message);
        strcpy(err->message + i, ": ");
        i += 2;
    }
    strerror_r(sys_err, err->message + i, BUF_SZ);
    return err;
}
dsp_error new_db_error (int db_err, char const *message)
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = DSP_E_DATABASE;
    char const *prefix = "Database error: ";
    int i = strlen(prefix);
    char const *sqlite_err = sqlite3_errstr(db_err);
    int l = i + strlen(sqlite_err);
    if (message) l += strlen(message) + 2;
    err->message = malloc(l);
    strcpy(err->message, prefix);
    if (message) {
        strcpy(err->message + i, message);
        i += strlen(message);
        strcpy(err->message + i, ": ");
        i += 2;
    }
    strcpy(err->message + i, sqlite_err);
    return err;
}

void log_error (dsp_error error)
{
    fprintf(stderr, "%s\n", error->message);
}
