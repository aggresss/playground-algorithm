#include "ostree.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int nnodes = 256;

void ostree_test() {
    struct rb_root_cached root = RB_ROOT_CACHED;
    struct ostree_node *nodes = NULL;

    nodes = calloc(nnodes, sizeof(struct ostree_node));
    assert(nodes);
    memset(nodes, 0x00, nnodes * sizeof(struct ostree_node));
    int i;
    for (i = 0; i < nnodes; i++) {
        nodes[i].key = i;
    }
    for (i = 0; i < nnodes; i++) {
        ostree_insert(&nodes[i], &root);
    }
    for (i = 0; i < nnodes; i++) {
		printf("[TEST ROUND]: %d\n", i);
        assert(&nodes[i] == ostree_select(&root, i + 1));
        // assert(i + 1 == ostree_rank(&root, &nodes[i]));
    }

    free(nodes);
}

int main(int argc, const char *argv[]) {
    ostree_test();

    return 0;
}