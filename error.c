#include <assert.h>
#include <errno.h>
#include <math.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dsp.h"

#ifndef NDEBUG
struct trace_elem {
    char const *function;
    char const *file;
    int line;
    struct trace_elem *up;
};
#endif

struct dsp_error {
    int code;
    char *message;
#ifndef NDEBUG
    struct trace_elem trace;
#endif
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
#ifndef NDEBUG
    for (struct trace_elem *e = err->trace.up; e;) {
        struct trace_elem *up = e->up;
        free(e);
        e = up;
    }
#endif
    free(err->message);
    free(err);
}

#define BUF_SZ 1024
#ifdef NDEBUG
dsp_error new_error (int code, char const *message)
#else
dsp_error new_error (int code, char const *message, char const *function, char const *file, int line)
#endif
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = code;
#ifndef NDEBUG
    err->trace.function = function;
    err->trace.file = file;
    err->trace.line = line;
    err->trace.up = NULL;
#endif
    if (message) {
        char const *prefix = "Error: ";
        int i = strlen(prefix);
        err->message = malloc(i + strlen(message) + 1);
        strcpy(err->message, prefix);
        strcpy(err->message + i, message);
    }
    return err;
}
#ifdef NDEBUG
dsp_error new_system_error (int code, int sys_err, char const *message)
#else
dsp_error new_system_error (int code, int sys_err, char const *message, char const *function, char const *file, int line)
#endif
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = code;
#ifndef NDEBUG
    err->trace.function = function;
    err->trace.file = file;
    err->trace.line = line;
    err->trace.up = NULL;
#endif
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
#ifdef NDEBUG
dsp_error new_db_error (int db_err, char const *message)
#else
dsp_error new_db_error (int db_err, char const *message, char const *function, char const *file, int line)
#endif
{
    dsp_error err = calloc(1, sizeof(struct dsp_error));
    err->code = DSP_E_DATABASE;
#ifndef NDEBUG
    err->trace.function = function;
    err->trace.file = file;
    err->trace.line = line;
    err->trace.up = NULL;
#endif
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

#ifndef NDEBUG
dsp_error trace_error (dsp_error error, char const *function, char const *file, int line)
{
    struct trace_elem *e = &error->trace;
    while (e->up) e = e->up;
    // On allocation failure, return the original error without continuing the
    //  trace
    if (!(e->up = malloc(sizeof(struct trace_elem))))
        return error;
    e->up->function = function;
    e->up->file= file;
    e->up->line= line;
    e->up->up= NULL;
    return error;
}

static void log_trace (dsp_error error)
{
    int depth = 1;
    for (struct trace_elem *e = &error->trace; e; e = e->up) {
        for (int i = 0; i < depth; i++) fputs("  ", stderr);
        fprintf(stderr, "%s, %s:%d\n", e->function, e->file, e->line);
        depth++;
    }
}
#endif

void log_error (dsp_error error)
{
    fprintf(stderr, "%s\n", error->message);
#ifndef NDEBUG
    log_trace(error);
#endif
}
