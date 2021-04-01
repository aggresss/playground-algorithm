#include "rbtree_augmented.h"

#include <stdlib.h>
#include <stdint.h>

struct ostree_node {
	uint32_t key;
	uint32_t val;
	uint32_t augmented;
	struct rb_node rb;
};

static void augment_compute(struct rb_node *rb)
{

}

static void augment_propagate(struct rb_node *rb, struct rb_node *stop)
{

}

static void augment_copy(struct rb_node *rb_old, struct rb_node *rb_new)
{

}

static void augment_rotate(struct rb_node *rb_old, struct rb_node *rb_new)
{

}

static const struct rb_augment_callbacks augment_callbacks = {
	augment_propagate,
	augment_copy,
	augment_rotate
};

void ostree_insert(struct ostree_node *node, struct rb_root *root)
{

}

void ostree_remove(struct ostree_node *node, struct rb_root *root)
{

}

struct ostree_node* ostree_select(uint32_t rank)
{
	return NULL;
}


uint32_t ostree_rank(struct ostree_node *node)
{
	return 0;
}
