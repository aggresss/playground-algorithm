#include "ostree.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int nnodes = 64;

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

    nodes = calloc(nnodes, sizeof(struct ostree_node));
    assert(nodes);
    memset(nodes, 0x00, nnodes * sizeof(struct ostree_node));

    for (i = 0; i < nnodes; i++) {
        nodes[i].key = i;
    }
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
    }
	for (i = 0; i < nnodes; i++) {
		printf("[TEST ROUND]: %d\n", i);
        assert(i + 1 == ostree_rank(&root, &nodes[i]));
    }

    free(nodes);
}

int main(int argc, const char *argv[]) {
    ostree_test();

    return 0;
}