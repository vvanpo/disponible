/// implementing header
#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
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
struct files {
    struct file **list;
};

/// static function declarations
static void add_file(files *, struct file *);
static void remove_file(files *, struct file *);
static void remove_all_files(files *);
static struct file *add_path(files *, char *);
static void write_file_table(files *);

// file_read_table reads the local file table and repopulates the file list
void file_read_table(files *files){
    remove_all_files(files);
    buffer table = read_file("files"); //check error
    for (int line_i = 0; line_i < table.length;){
        struct file *file;
        int path_i = 0;
        for (int i = 0;; i++){
            if (table.data[line_i + i] == '\n' ||
                    line_i + i + 1 == table.length){
                if (!path_i); //error invalid file format TODO: remove all files
                file = malloc(sizeof(struct file));
                if (!file); //error
                char *encoded_hash = calloc(path_i, sizeof(char));
                if (!encoded_hash); //error
                strncpy(encoded_hash, (char *) table.data + line_i, path_i - 1);
                buffer hash = util_base64_decode(encoded_hash);
                free(encoded_hash);
                memcpy(file->hash, hash.data, hash.length);
                free(hash.data);
                file->path = calloc(i - path_i + 1, sizeof(char));
                if (!file->path); //error
                strncpy(file->path, (char *) table.data + line_i + path_i,
                        i - path_i);
                add_file(files, file); //error check
                line_i += i + 1;
                break;
            }
            if (!path_i && table.data[line_i + i] == ' ') path_i = i + 1;
        }
    }
    free(table.data);
}

// create_files initializes a file list
files *file_create_list(){
    files *files = malloc(sizeof(files));
    if (!files); //error
    files->list = calloc(1, sizeof(struct file *));
    if (!files->list); //error
    return files;
}

// add_file adds the given file object to the passed file list
void add_file(files *files, struct file *file){
    //TODO: needs to check if file hash is duplicate
    int no_files = 0;
    for (; files->list[no_files]; no_files++);
    files->list = realloc(files->list, sizeof(struct file *) * (no_files + 2));
    if (!files->list); //error
    files->list[no_files] = file;
    files->list[no_files + 1] = NULL;
}

// remove_file removes a given file from the passed file list
//TODO: sets error status if the file was already absent from the list
// regardless of error status, the resources assocated with file are freed
void remove_file(files *files, struct file *file){
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
        return;
    }
    //error if it gets this far
}

// remove_all_files removes all file objects from files and frees
void remove_all_files(files *files){
    for (int i = 0; files->list[i]; i++)
        free(files->list[i]);
    files->list = realloc(files->list, sizeof(struct file *));
    if (!files->list); //error
    files->list[0] = NULL;
}

// add_path adds a given pathname to the passed file list, and returns the
// corresponding file object
struct file *add_path(files *files, char *path){
    struct file *file = malloc(sizeof(struct file));
    if (!file); //error
    file->path = malloc(sizeof(char) * (strlen(path) + 1));
    if (!file->path); //error
    strcpy(file->path, path);
    file->hash = hash_file_digest(path);
    add_file(files, file);
    return file;
}

// write_file_table creates or overwrites the local file table
void write_file_table(files *files){
    if (unlink("files~") == -1)
        if (errno != ENOENT); //error
    int fd = open("files~", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1); //error
    for (int i = 0; files->list[i]; i++){
        int length;
        char *line = hash_base64_encode(files->list[i]->hash);
        int hash_length = strlen(line);
        length = hash_length + strlen(files->list[i]->path) +
            (sizeof(char) * 2);
        line = realloc(line, length);
        if (!line); //error
        line[hash_length] = ' ';
        strcpy(line + hash_length + 1, files->list[i]->path);
        line[length - 1] = '\n';
        int ret = write(fd, line, length);
        if (ret == -1 || ret < length); //error
        free(line);
    }
    if (fsync(fd)); //error
    if (close(fd)); //error
    if (rename("files~", "files")); //error
}

