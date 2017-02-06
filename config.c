#include <stdlib.h>

#include "self.h"

struct config {
    int port;
    unsigned int bucket_length;
    char *bootstrap_address;
};

struct config *config_default ()
{
    struct config *config = calloc(1, sizeof(struct config));
    config->bucket_length = 20;
    return config;
}

unsigned int config_bucket_length (struct config *config)
{
    return config->bucket_length;
}

/*
    FILE *config_file = fopen("config", "r");
    if (!config_file) return NULL;
    fclose(config_file);
    */
