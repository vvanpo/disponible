#ifndef FILE_H
#define FILE_H

/// type definitions
typedef struct files files;

/// extern function declarations
files *file_create_list();
void file_read_table(files *);

#endif
