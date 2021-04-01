#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "ostree.h"

static int nnodes = 256;

void ostree_test()
{
	struct rb_root_cached root = RB_ROOT_CACHED;
	struct ostree_node *nodes = NULL;

	nodes = calloc(nnodes, sizeof(struct ostree_node));
	assert(nodes);
    int i;
	for (i = 0; i < nnodes; i++) {
		nodes[i].key = i;
        nodes[i].val = i;
	}
	for (i = 0; i < nnodes; i++) {
		ostree_insert(&nodes[i], &root.rb_root);
	}
	for (i = 0; i < nnodes; i++) {
		assert(&nodes[i] == ostree_select(i));
		assert(i == ostree_rank(&nodes[i]));
	}

	free(nodes);
}

int main(int argc, const char * argv[])
{
	ostree_test();

	return 0;
}