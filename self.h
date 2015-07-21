#ifndef SELF_H
#define SELF_H

#include "hash.h"
#include "util.h"

/// type definitions
struct self {
    hash fingerprint;
    buffer public_key;
    buffer private_key;
    struct peers *peers;
};

/// extern function declarations
struct self *self_load_config();

#endif
