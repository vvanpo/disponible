#ifndef DISPONIBLE_H
#define DISPONIBLE_H

#include <stdbool.h>

/// type definitions
struct di_options {
    // Directory for placing the config, peer, and file structures.
    char *directory;
};
struct di_daemon;
#define DI_ERRORS \
    X(DI_ESUCCESS, "Success")
#define X(n, s) n,
enum di_error { DI_ERRORS };
#undef X

// array of error strings, indexed by di_error
extern char const * const * const di_error_names;

/// extern function declarations
enum di_error di_daemon_load(struct di_daemon **, struct di_options *);
enum di_error di_daemon_run(struct di_daemon *);

#endif
