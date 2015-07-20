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
static struct file *add_path(files *, char *);
static int remove_file(files *, struct file *);
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
    if (files->list)
        for (int i; files->list[i] != NULL; i++)
            free(files->list[i]);
    free(files->list);
    free(files);
    return;
}

// add_path adds a given pathname to the passed file list, and returns the
// corresponding file object
struct file *add_path(files *files, char *path){
    struct file *file = malloc(sizeof(struct file));
    file->path = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(file->path, path);
    hash_file_digest(&file->hash, path);
    //TODO: needs to check if object is duplicate
    int no_files = 0;
    for (; files->list[no_files] != NULL; no_files++);
    files->list = realloc(files->list, sizeof(struct file *) * (no_files + 2));
    if (!files->list); //error
    files->list[no_files] = file;
    files->list[no_files + 1] = NULL;
    return file;
}

// remove_file removes a given file from the passed file list
// returns 0 on success, -1 on error or if the file was already absent from the
// list
// regardless of return status, the resources assocated with file are freed
int remove_file(files *files, struct file *file){
    free(file->path);
    free(file);
    for (int i; files->list[i] != NULL; i++){
        if (file != files->list[i]) continue;
        do {
            files->list[i] = files->list[i + 1];
            i++;
        } while (files->list[i] != NULL);
        files->list = realloc(files->list, sizeof(struct file *) * (i + 1));
        if (!files->list); //error
        return 0;
    }
    return -1;
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
    int ret = read(fd, buf, st.st_size);
    if (ret == -1); //error

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
    struct file* file = add_path(files, "dht.txt");
    printf("%s %s\n", hash_digest_base64(&file->hash), file->path);
}
