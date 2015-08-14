#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/// implementing header
#include "peer.h"

enum peer_error {
	ERR_PEER_DUPLICATE,
	ERR_PEER_MAX_DEPTH,
};

struct bucket {
	// buckets are sorted in an ordered binary tree
	// the root bucket has prefix_length = 0, and therefore depth = 0
	int prefix_len;
	// non-local depth, i.e. the distance up the tree to a local bucket (one
	// containing the local node)
	int depth;
	// bits past prefix_length are 0
	unsigned char prefix[HASH_LEN];
	// head of the peer list, non-NULL only for leaf buckets
	struct peer *head;
	struct bucket *parent;
	// a leaf bucket still make use of child pointers, by using them as a
	// shortcut to the nearest leaf buckets, i.e. all leaf buckets are
	// ordered in a doubly-linked list
	struct bucket *left;
	struct bucket *right;
};

static void find_leaf(struct bucket **b, unsigned char *finger);
static void find_peer(struct peer **p, struct bucket *b, unsigned char *finger);
static void count_leaf(int *cnt, struct bucket *b);
static int split_bucket(struct bucket *leaf);
static void merge_buckets(struct bucket *parent);

// peer_get places the peer corresponding to the passed fingerprint in *peer,
//   setting *peer to null if it isn't found.
void peer_get(struct peer **peer, unsigned char *finger, struct node *node)
{
	*peer = NULL;
	struct bucket *b = node->root;
	if (!b) return;
	find_peer(peer, b, finger);
}

// peer_add creates a peer structure using the passed fingerprint and adds it to
//   the list, optionally passing a back a reference in *peer if peer is non-
//   null.
//   On error, returns
//   	ERR_SYSTEM
//   	ERR_PEER_DUPLICATE
//   	ERR_PEER_MAX_DEPTH
int peer_add(unsigned char *finger, struct node *node)
{
	struct bucket *b = node->root;
	if (!b) {
		b = node->root = calloc(1, sizeof(*b));
		if (!b) return ERR_SYSTEM;
	}
	find_leaf(&b, finger);
	struct peer *p;
	find_peer(&p, b, finger);
	if (p) return ERR_PEER_DUPLICATE;
	int cnt;
	count_leaf(&cnt, b);
	while (node->conf->bucket_size == cnt) {
		if (node->conf->bucket_depth_max == b->depth)
			return ERR_PEER_MAX_DEPTH;
		split_bucket(b);
		find_leaf(&b, finger);
	}
	p = calloc(1, sizeof(*p));
	if (!p) return ERR_SYSTEM;
	memcpy(p->finger, finger, HASH_LEN);
	if (b->head) {
		p->next = b->head;
		b->head->prev = p;
	}
	b->head = p;
	return 0;
}

// peer_remove removes a peer from the list
int peer_remove(struct peer *peer, struct node *node)
{
	return 0;
}

// find_leaf takes the given bucket at *b (usually set to node->root) and
//   traverses down the tree until it finds the leaf bucket corresponding to the
//   passed fingerprint.
void find_leaf(struct bucket **b, unsigned char *finger)
{
	while (((*b)->left && (*b)->left->parent == *b) ||
			((*b)->right && (*b)->right->parent == *b)) {
		int ret;
		cryp_hash_cmp(&ret, finger, (*b)->left->prefix);
		*b = ret < 0 ? (*b)->right : (*b)->left;
	}
}

// find_peer searchs a given bucket for the passed fingerprint, setting *p to
//   null if it is not found.
void find_peer(struct peer **p, struct bucket *b, unsigned char *finger)
{
	find_leaf(&b, finger);
	for (*p = b->head; *p; *p = (*p)->next) {
		int ret;
		cryp_hash_cmp(&ret, (*p)->finger, finger);
		if (!ret) return;
	}
	*p = NULL;
}

// count_leaf returns the number of peers in the leaf node
void count_leaf(int *cnt, struct bucket *leaf)
{
	*cnt = 0;
	for (struct peer *p = leaf->head; p; p = p->next) (*cnt)++;
	return;
}

// split_bucket takes a leaf bucket and splits it.  Only to be called when
//   bucket.depth < conf.max_depth
int split_bucket(struct bucket *leaf)
{
	struct bucket *left = calloc(1, sizeof(*left));
	struct bucket *right = calloc(1, sizeof(*right));
	if (!left || !right) return ERR_SYSTEM;
	left->prefix_len = right->prefix_len = leaf->prefix_len + 1;
	left->depth = right->depth = leaf->depth + 1;
	left->parent = right->parent = leaf;
	memcpy(left->prefix, leaf->prefix, HASH_LEN);
	memcpy(right->prefix, leaf->prefix, HASH_LEN);
	left->prefix[left->prefix_len / 8] |= 1 << (8 - (left->prefix_len % 8));
	left->right = right;
	right->left = left;
	if (leaf->left) {
		leaf->left->right = left;
		left->left = leaf->left;
	}
	if (leaf->right) {
		leaf->right->left = right;
		right->right = leaf->right;
	}
	leaf->left = left;
	leaf->right = right;
	struct peer *p_last_left, *p_last_right;
	p_last_left = p_last_right = NULL;
	for (struct peer *p = leaf->head; p; p = p->next) {
		int cmp;
		cryp_hash_cmp(&cmp, p->finger, left->prefix);
		if (cmp < 0) {
			if (!p_last_right) right->head = p;
			else p_last_right->next = p;
			p->prev = p_last_right;
			p_last_right = p;
		}
		else {
			if (!p_last_left) left->head = p;
			else p_last_left->next = p;
			p->prev = p_last_left;
			p_last_left = p;
		}
	}
	p_last_left->next = p_last_right->next = NULL;
	leaf->head = NULL;
	return 0;
}

// merge_buckets takes the parent of two leaves and merges their peer lists.
//   TODO: keep LRU property of the list
void merge_buckets(struct bucket *parent)
{
	if (parent->left->head)
		parent->head = parent->left->head;
	if (parent->right->head) {
		if (parent->head) {
			struct peer *last = parent->head;
			for (; last->next; last = last->next);
			last->next = parent->right->head;
			parent->right->head->prev = last;
		}
		else parent->head = parent->right->head;
	}
	free(parent->left);
	free(parent->right);
	parent->left = parent->right = NULL;
}
