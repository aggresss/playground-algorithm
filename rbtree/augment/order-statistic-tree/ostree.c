#include "../base/rbtree_augmented.h"

#include <stdint.h>

struct ostree_node {
	uint32_t key;
	struct rb_node rb;
	uint32_t val;
	uint32_t augmented;
};

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
	struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
	uint32_t key = node->key;
	uint32_t val = node->val;
	struct test_node *parent;
	bool leftmost = true;

	while (*new) {
		rb_parent = *new;
		parent = rb_entry(rb_parent, struct test_node, rb);
		if (parent->augmented < val)
			parent->augmented = val;
		if (key < parent->key)
			new = &parent->rb.rb_left;
		else {
			new = &parent->rb.rb_right;
			leftmost = false;
		}
	}

	node->augmented = val;
	rb_link_node(&node->rb, rb_parent, new);
	rb_insert_augmented_cached(&node->rb, root, leftmost, &augment_callbacks);
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

}
