/// implementing header
//#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hash.h"

/// type definitions
struct file {
    struct hash hash;
    // path cannot have a newline in it, or it will break the local file table
    // format
    // path needs to be malloc'ed (no string literals), as free_file_tree frees
    // it
    char *path;
};
// red-black tree of files ordered by hash
struct file_tree {
    struct file file;
    enum colour { red, black } colour;
    struct file_tree *left;
    struct file_tree *right;
};

/// static function declarations
static struct file *add_path(struct file_tree *, char *);
static int remove_file(struct file_tree *, struct file *);
static void free_file_tree(struct file_tree *);
static int read_table_file(struct file_tree **);
static int write_table_file(struct file_tree *);

// add_path adds a given pathname to the passed file tree, and returns the
// corresponding file object
struct file *add_path(struct file_tree *root, char *path){
    struct file_tree *node = malloc(sizeof(struct file_tree));
    hash_file(&node->file.hash, path);
    node->file.path = path;
}

// remove_file removes a given file from the passed file tree
// return 0 on success, -1 if the file was already absent from the tree
int remove_file(struct file_tree *root, struct file *file){
    while (root) {
        if (&root->file == file){
        }
    }
}

// free_file_tree frees all associated resources with root
void free_file_tree(struct file_tree *root){
    if (root->left) free_file_tree(root->left);
    if (root->right) free_file_tree(root->right);
    free(root->file.path);
    free(root);
    return;
}

// read_table reads the local file table and populates a new file tree
int read_table_file(struct file_tree **root){
    // reallocate empty tree
    free_file_tree(*root);
    *root = NULL;
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
    if (!S_ISREG(st.st_mode)); //fatal
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
int write_table_file(struct file_tree *root){
    return 0;
}


///// for tests
int main(int argc, char **argv){
}
