#ifndef UTIL_H
#define UTIL_H

/// type definitions
typedef unsigned char byte;
typedef struct {
    byte *data;
    unsigned int length;
} buffer;

/// extern function declarations
byte *distance(byte *, byte *);
buffer read_file(char *);
void write_file(char *, buffer);

#endif
