#include <stdlib.h>

#include "self.h"

#define DEFAULT_BUCKET_LENGTH 20

struct config {
    int port;
    int bucket_length;
    char *bootstrap_address;
};

struct config *new_config ()
{
    struct config *config = calloc(1, sizeof(struct config));
    config->bucket_length = DEFAULT_BUCKET_LENGTH;
    return config;
}

void destroy_config (struct config *config)
{
    if (config->bootstrap_address)
        free(config->bootstrap_address);
    free(config);
}

int config_bucket_length (struct config *config)
{
    return config->bucket_length;
}

/*
    FILE *config_file = fopen("config", "r");
    if (!config_file) return NULL;
    fclose(config_file);
    */
