#include <assert.h>
#include <errno.h>
#include <math.h>
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

void set_error_message (dsp_error error, char *message)
{
    if (error->message = malloc(strlen(message) + 1))
        strcpy(error->message, message);
}

#define BUF_SZ 1024
static char *error_message (int code)
{
    char const *sys_prefix = "System error: ";
    char const *db_prefix = "Database error: ";
    char *buf = malloc(BUF_SZ);
    switch (code) {
    case DSP_E_SYSTEM:
        strcpy(buf, sys_prefix);
        // Only fails on invalid errno or insufficient buffer size
        assert(strerror_r(errno, buf + strlen(sys_prefix),
                BUF_SZ - strlen(sys_prefix)));
#ifdef NDEBUG
        strerror_r(errno, buf + strlen(sys_prefix),
                BUF_SZ - strlen(sys_prefix));
#endif
        break;
    case DSP_E_DATABASE:
        strcpy(buf, db_prefix);
        break;
    default:
        assert(true);
        strcpy(buf, "Invalid error");
    }
    return buf;
}

#ifdef NDEBUG
dsp_error new_error (int code)
{
    dsp_error err = malloc(sizeof(struct dsp_error));
    err->code = code;
    err->message = error_message(code);
    return err;
}
#else
dsp_error new_error (int code, char const *function, char const *file, int line)
{
    dsp_error err = malloc(sizeof(struct dsp_error));
    err->code = code;
    err->message = error_message(code);
    err->trace.function = function;
    err->trace.file = file;
    err->trace.line = line;
    err->trace.up = NULL;
    return err;
}

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
