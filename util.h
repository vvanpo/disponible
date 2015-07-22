#ifndef UTIL_H
#define UTIL_H

/// type definitions
typedef unsigned char byte;
typedef struct {
    byte *data;
    unsigned int length;
} buffer;

/// extern function declarations
buffer read_file(char *);
void write_file(char *, buffer);
char *util_base64_encode(buffer);
buffer util_base64_decode(char *);

#endif
