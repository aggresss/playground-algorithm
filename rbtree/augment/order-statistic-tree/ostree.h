#ifndef _OSTREE_H
#define _OSTREE_H

#include <stdint.h>
#include <stdlib.h>

#include "rbtree_augmented.h"

struct ostree_node {
    uint32_t key;
    uint32_t augmented;
    struct rb_node rb;
};

static void augment_compute(struct rb_node *rb) {
    if (!rb) {
        return;
    }
    uint32_t augmented = 1;
    if (rb->rb_left) {
        augmented += rb_entry(rb->rb_left, struct ostree_node, rb)->augmented;
    }
    if (rb->rb_right) {
        augmented += rb_entry(rb->rb_right, struct ostree_node, rb)->augmented;
    }
    rb_entry(rb, struct ostree_node, rb)->augmented = augmented;
}

static void augment_propagate(struct rb_node *rb, struct rb_node *stop) {
    while (rb != stop) {
        struct ostree_node *node = rb_entry(rb, struct ostree_node, rb);
        augment_compute(&node->rb);
        rb = rb_parent(&node->rb);
    }
}

static void augment_copy(struct rb_node *rb_old, struct rb_node *rb_new) {
    struct ostree_node *old = rb_entry(rb_old, struct ostree_node, rb);
    struct ostree_node *new = rb_entry(rb_new, struct ostree_node, rb);
    new->augmented = old->augmented;
}

static void augment_rotate(struct rb_node *rb_old, struct rb_node *rb_new) {
    augment_compute(rb_old);
    augment_compute(rb_new);
}

static const struct rb_augment_callbacks augment_callbacks = {
    augment_propagate,
    augment_copy,
    augment_rotate};

void ostree_insert(struct ostree_node *node, struct rb_root_cached *root) {
    struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
    uint32_t key = node->key;
    struct ostree_node *parent;

    while (*new) {
        rb_parent = *new;
        parent = rb_entry(rb_parent, struct ostree_node, rb);
        parent->augmented++;
        if (key < parent->key)
            new = &parent->rb.rb_left;
        else
            new = &parent->rb.rb_right;
    }

    node->augmented = 1;
    rb_link_node(&node->rb, rb_parent, new);
    rb_insert_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

void ostree_remove(struct ostree_node *node, struct rb_root_cached *root) {
    rb_erase_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

struct ostree_node *ostree_select(struct rb_root_cached *root, uint32_t rank) {
    uint32_t node_rank = 0;
    struct ostree_node *osnode = NULL;
    struct rb_node *rbnode = root->rb_root.rb_node;

    while (rbnode) {
        node_rank = 1;
        if (rbnode->rb_left) {
            node_rank += rb_entry(rbnode->rb_left, struct ostree_node, rb)->augmented;
        }
        if (rank == node_rank) {
            osnode = rb_entry(rbnode, struct ostree_node, rb);
            break;
        } else if (rank < node_rank) {
            rbnode = rbnode->rb_left;
        } else {
            rank -= node_rank;
            rbnode = rbnode->rb_right;
        }
    }

    return osnode;
}

uint32_t ostree_rank(struct rb_root_cached *root, struct ostree_node *node) {
    uint32_t rank;
    struct rb_node *rbnode = &node->rb;

    rank = 1;
    if (rbnode->rb_left) {
        rank += rb_entry(rbnode->rb_left, struct ostree_node, rb)->augmented;
    }
    while (rbnode != root->rb_root.rb_node) {
        if (rbnode == rb_parent(rbnode)->rb_right) {
            rank += 1;
            if (rb_parent(rbnode)->rb_left) {
                rank += rb_entry(rb_parent(rbnode)->rb_left, struct ostree_node, rb)->augmented;
            }
        }
        rbnode = rb_parent(rbnode);
    }

    return rank;
}

#endif /* _OSTREE_H */
