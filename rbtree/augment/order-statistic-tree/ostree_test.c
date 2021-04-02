#include "ostree.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int nnodes = 256;

static bool check_argmented(struct rb_node *rb) {
    if (!rb) {
        return false;
    }
    uint32_t augmented = 1;
    if (rb->rb_left) {
        augmented += rb_entry(rb->rb_left, struct ostree_node, rb)->augmented;
    }
    if (rb->rb_right) {
        augmented += rb_entry(rb->rb_right, struct ostree_node, rb)->augmented;
    }

    return rb_entry(rb, struct ostree_node, rb)->augmented == augmented;
}

void ostree_test() {
    struct rb_root_cached root = RB_ROOT_CACHED;
    struct rb_node *rb = NULL;
    struct ostree_node *nodes = NULL;
    int i;

    // Setup
    nodes = calloc(nnodes, sizeof(struct ostree_node));
    assert(nodes);
    memset(nodes, 0x00, nnodes * sizeof(struct ostree_node));
    for (i = 0; i < nnodes; i++) {
        nodes[i].key = i;
    }

    // Test insert
    for (i = 0; i < nnodes; i++) {
        ostree_insert(&nodes[i], &root);
    }
    i = 0;
    for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
        assert(rb_entry(rb, struct ostree_node, rb) == &nodes[i++]);
        assert(check_argmented(rb));
    }
    for (i = 0; i < nnodes; i++) {
        assert(&nodes[i] == ostree_select(&root, i + 1));
        assert(i + 1 == ostree_rank(&root, &nodes[i]));
    }

    // Test remove
    for (i = 0; i < nnodes; i += 2) {
        ostree_remove(&nodes[i], &root);
    }
    i = 0;
    for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
        assert(rb_entry(rb, struct ostree_node, rb) == &nodes[i + 1]);
        assert(check_argmented(rb));
        i += 2;
    }
    for (i = 0; i < nnodes; i += 2) {
        assert(&nodes[i + 1] == ostree_select(&root, (i + 2) / 2));
        assert((i + 2) / 2 == ostree_rank(&root, &nodes[i + 1]));
    }

    // Teardown
    free(nodes);
}

int main(int argc, const char *argv[]) {
    ostree_test();

    return 0;
}
