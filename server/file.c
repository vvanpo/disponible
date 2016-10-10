#include "self.h"

struct file {
    struct file *last;
    struct file *next;
    unsigned char hash[HASH_LEN];
    unsigned char *name;
    unsigned int size;
    unsigned char block_sz_exp;
    unsigned int *blocks;
}
