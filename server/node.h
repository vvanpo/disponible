#ifndef NODE_H
#define NODE_H

struct list;
struct node;

struct list *new_list (int bucket_len);
struct node *find_node (unsigned char *finger, struct list *);

#endif
