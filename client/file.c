#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <nacl/crypto_hash_sha256.h>

#include "client.h"

struct file {
    char *path;
    uint64_t length;
    void *manifest;
    unsigned char identifier[32];
}

static uint64_t block_size_KiB (uint64_t file_length)
{
    uint64_t num_4KiB = file_length / 4096 + (file_length % 4096 ? 1 : 0);
    if (num_4KiB <= 1) return 4;
    return 4 * (num_4KiB / 1024 + (num_4KiB % 1024 ? 1 : 0));
}

int file_identify (struct file *file)
{
    struct stat status = {};
    if (!stat(file->path, &status)) return -1;
    FILE *fd = fopen(file->path, "r");
    if (!fd) return -1;
    file->length = status.st_size;
    int64_t block_size = 1024 * block_size_KiB(file->length);
    int i = 0, n, num_blocks = file->length / block_size
                                            + (file_length % block_size ? 1: 0);
    file->manifest = malloc(32 * num_blocks);
    void *buffer = malloc(block_size);
    do {
        n = fread(buffer, block_size, 1, fd);
        crypto_hash_sha256(manifest + i, buffer, n);
        i += 32;
    } while (n == block_size);
    if (ferror(fd)) return -1;
    assert(feof(fd));
    flose(fd);
    free(buffer);
    crypto_hash_sha256(file->identifier, manifest, 32 * num_blocks);
    if (symlink(path, file->identifier)) return -1;
    return 0;
}
