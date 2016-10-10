#include "self.h"

struct node {
    struct node *last;
    struct node *next;
    unsigned char finger[HASH_LEN];
};

