/// implementing header
//#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hash.h"

/// type definitions
struct file {
    hash hash;
    // path cannot have a newline in it, or it will break the local file table
    // format
    // path needs to be malloc'ed (no string literals), as remove_file frees it
    char *path;
};
// simple list of files for now, implement hash table later
typedef struct files {
    struct file **list;
} files;

/// static function declarations
static files *create_files();
static void free_files(files *);
static int add_file(files *, struct file *);
static int remove_file(files *, struct file *);
static struct file *add_path(files *, char *);
static int read_table_file(files *);
static int write_table_file(files *);

// create_files initializes a file list
files *create_files(){
    files *files = malloc(sizeof(files));
    if (!files); //error
    files->list = calloc(1, sizeof(struct file *));
    if (!files->list); //error
    return files;
}

// free_files removes all file objects from files and frees all associated
// resources
void free_files(files *files){
    for (int i = 0; files->list[i]; i++)
        free(files->list[i]);
    free(files->list);
    free(files);
    return;
}

// add_file adds the given file object to the passed file list
// returns 0 on success, -1 on error
int add_file(files *files, struct file *file){
    //TODO: needs to check if file hash is duplicate
    int no_files = 0;
    for (; files->list[no_files]; no_files++);
    files->list = realloc(files->list, sizeof(struct file *) * (no_files + 2));
    if (!files->list); //error
    files->list[no_files] = file;
    files->list[no_files + 1] = NULL;
    return 0;
}

// remove_file removes a given file from the passed file list
// returns 0 on success, -1 on error or if the file was already absent from the
// list
// regardless of return status, the resources assocated with file are freed
int remove_file(files *files, struct file *file){
    free(file->path);
    free(file);
    for (int i; files->list[i]; i++){
        if (file != files->list[i]) continue;
        do {
            files->list[i] = files->list[i + 1];
            i++;
        } while (files->list[i]);
        files->list = realloc(files->list, sizeof(struct file *) * (i + 1));
        if (!files->list); //error
        return 0;
    }
    return -1;
}

// add_path adds a given pathname to the passed file list, and returns the
// corresponding file object
struct file *add_path(files *files, char *path){
    struct file *file = malloc(sizeof(struct file));
    file->path = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(file->path, path);
    hash_file_digest(&file->hash, path);
    if (!add_file(files, file)); //error
    return file;
}

// read_table reads the local file table and populates a new file tree
int read_table_file(files *files){
    free_files(files);
    // check if the local file table exists
    struct stat st;
    if (stat("files", &st)){
        if (errno == ENOENT){
            // if not, call write_table_file() to create an empty 'files' file
            if (write_table_file(NULL)) return -1;
            return 0;
        }
        else ; //error
    }
    if (!S_ISREG(st.st_mode)); //error
    int fd = open("files", O_RDONLY);
    if (fd == -1); //error
    char *buf = malloc(st.st_size);
    if (!buf); //error
    //TODO: for now, we're assuming that read() will successfully read in the
    // whole file at once
    int ret = read(fd, buf, st.st_size);
    if (ret == -1); //error
    for (int line_i = 0; line_i < ret;){
        struct file *file;
        int path_i = 0;
        for (int i = 0;; i++){
            if (buf[line_i + i] == '\n' || line_i + i + 1 == ret){
                if (!path_i); //error invalid file format
                file = malloc(sizeof(struct file));
                if (!file); //error
                char *encoded_hash = calloc(path_i, sizeof(char));
                if (!encoded_hash); //error
                strncpy(encoded_hash, buf + line_i, path_i - 1);
                int hash_length;
                unsigned char *hash = hash_base64_decode(encoded_hash,
                        &hash_length);
                free(encoded_hash);
                //TODO: should really be a separate function in hash.c, to
                // encapsulate the hash struct
                memcpy(file->hash.md, hash, hash_length);
                free(hash);
                file->path = calloc(i - path_i + 1, sizeof(char));
                if (!file->path); //error
                strncpy(file->path, buf + line_i + path_i, i - path_i);
                line_i += i + 1;
                break;
            }
            if (!path_i && buf[line_i + i] == ' ') path_i = i + 1;
        }
    }
    if (close(fd)); //error
    return 0;
}

// write_table_file creates or overwrites the local file table
int write_table_file(files *files){
    return 0;
}


///// for tests
#include <stdio.h>
int main(int argc, char **argv){
    files *files = create_files();
    read_table_file(files);
    //struct file* file = add_path(files, "dht.txt");
    //printf("%s %s\n", hash_digest_base64(&file->hash), file->path);
}
