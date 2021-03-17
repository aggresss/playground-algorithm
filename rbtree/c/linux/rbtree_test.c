#include "rbtree_augmented.h"

#include <stdio.h>      /* printf */
#include <stddef.h>     /* offsetof */
#include <stdint.h>     /* uintptr_t */
#include <string.h>     /* malloc */
#include <time.h>       /* time */
#include <inttypes.h>   /* PRIuPTR */
#include <assert.h>     /* assert */
#include <errno.h>
#include <sys/time.h>

///////////////

#define printk printf
#define __init
#define __exit
#define KERN_ALERT "Alert"
#define WARN_ON_ONCE assert

typedef uint32_t u32;
typedef uint64_t u64;
typedef struct timeval cycles_t;
static int nnodes = 100;
static int perf_loops = 1000;
static int check_loops = 100;

#define DIFF_US(t1,t2) (((u64)(t1).tv_usec-(u64)(t2).tv_usec)*1000000+((u64)(t1).tv_usec-(u64)t2.tv_usec))

struct test_node {
	u32 key;
	struct rb_node rb;

	/* following fields used for testing augmented rbtree functionality */
	u32 val;
	u32 augmented;
};

static struct rb_root_cached root = RB_ROOT_CACHED;
static struct test_node *nodes = NULL;

static void insert(struct test_node *node, struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
	u32 key = node->key;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else
			new = &parent->rb_right;
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color(&node->rb, &root->rb_root);
}

static void insert_cached(struct test_node *node, struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
	u32 key = node->key;
	bool leftmost = true;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else {
			new = &parent->rb_right;
			leftmost = false;
		}
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color_cached(&node->rb, root, leftmost);
}

static inline void erase(struct test_node *node, struct rb_root_cached *root)
{
	rb_erase(&node->rb, &root->rb_root);
}

static inline void erase_cached(struct test_node *node, struct rb_root_cached *root)
{
	rb_erase_cached(&node->rb, root);
}


#define NODE_VAL(node) ((node)->val)

RB_DECLARE_CALLBACKS_MAX(static, augment_callbacks,
			 struct test_node, rb, u32, augmented, NODE_VAL)

static void insert_augmented(struct test_node *node,
			     struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
	u32 key = node->key;
	u32 val = node->val;
	struct test_node *parent;

	while (*new) {
		rb_parent = *new;
		parent = rb_entry(rb_parent, struct test_node, rb);
		if (parent->augmented < val)
			parent->augmented = val;
		if (key < parent->key)
			new = &parent->rb.rb_left;
		else
			new = &parent->rb.rb_right;
	}

	node->augmented = val;
	rb_link_node(&node->rb, rb_parent, new);
	rb_insert_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

static void insert_augmented_cached(struct test_node *node,
				    struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
	u32 key = node->key;
	u32 val = node->val;
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
	rb_insert_augmented_cached(&node->rb, root,
				   leftmost, &augment_callbacks);
}


static void erase_augmented(struct test_node *node, struct rb_root_cached *root)
{
	rb_erase_augmented(&node->rb, &root->rb_root, &augment_callbacks);
}

static void erase_augmented_cached(struct test_node *node,
				   struct rb_root_cached *root)
{
	rb_erase_augmented_cached(&node->rb, root, &augment_callbacks);
}

static void init(void)
{
	int i;
	for (i = 0; i < nnodes; i++) {
		nodes[i].key = (u32)rand();
		nodes[i].val = (u32)rand();
	}
}

static bool is_red(struct rb_node *rb)
{
	return !(rb->__rb_parent_color & 1);
}

static int black_path_count(struct rb_node *rb)
{
	int count;
	for (count = 0; rb; rb = rb_parent(rb))
		count += !is_red(rb);
	return count;
}

static void check_postorder_foreach(int nr_nodes)
{
	struct test_node *cur, *n;
	int count = 0;
	rbtree_postorder_for_each_entry_safe(cur, n, &root.rb_root, rb)
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check_postorder(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0;
	for (rb = rb_first_postorder(&root.rb_root); rb; rb = rb_next_postorder(rb))
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0, blacks = 0;
	u32 prev_key = 0;

	for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
		struct test_node *node = rb_entry(rb, struct test_node, rb);
		WARN_ON_ONCE(node->key < prev_key);
		WARN_ON_ONCE(is_red(rb) &&
			     (!rb_parent(rb) || is_red(rb_parent(rb))));
		if (!count)
			blacks = black_path_count(rb);
		else
			WARN_ON_ONCE((!rb->rb_left || !rb->rb_right) &&
				     blacks != black_path_count(rb));
		prev_key = node->key;
		count++;
	}

	WARN_ON_ONCE(count != nr_nodes);
	WARN_ON_ONCE(count < (1 << black_path_count(rb_last(&root.rb_root))) - 1);

	check_postorder(nr_nodes);
	check_postorder_foreach(nr_nodes);
}

static void check_augmented(int nr_nodes)
{
	struct rb_node *rb;

	check(nr_nodes);
	for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
		struct test_node *node = rb_entry(rb, struct test_node, rb);
		u32 subtree, max = node->val;
		if (node->rb.rb_left) {
			subtree = rb_entry(node->rb.rb_left, struct test_node,
					   rb)->augmented;
			if (max < subtree)
				max = subtree;
		}
		if (node->rb.rb_right) {
			subtree = rb_entry(node->rb.rb_right, struct test_node,
					   rb)->augmented;
			if (max < subtree)
				max = subtree;
		}
		WARN_ON_ONCE(node->augmented != max);
	}
}

static int __init rbtree_test_init(void)
{
	int i, j;
	cycles_t time1, time2;
	u64 time_delta;
	struct rb_node *node;
    srand(time(NULL));

	nodes = malloc(nnodes * sizeof(nodes));
	if (!nodes)
		return -ENOMEM;

	printk(KERN_ALERT "rbtree testing");

	init();

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++) {
		for (j = 0; j < nnodes; j++)
			insert(nodes + j, &root);
		for (j = 0; j < nnodes; j++)
			erase(nodes + j, &root);
	}

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 1 (latency of nnodes insert+delete): %llu us\n",
	       (unsigned long long)time_delta);

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++) {
		for (j = 0; j < nnodes; j++)
			insert_cached(nodes + j, &root);
		for (j = 0; j < nnodes; j++)
			erase_cached(nodes + j, &root);
	}

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 2 (latency of nnodes cached insert+delete): %llu us\n",
	       (unsigned long long)time);

	for (i = 0; i < nnodes; i++)
		insert(nodes + i, &root);

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++) {
		for (node = rb_first(&root.rb_root); node; node = rb_next(node))
			;
	}

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 3 (latency of inorder traversal): %llu us\n",
	       (unsigned long long)time_delta);

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++)
		node = rb_first(&root.rb_root);

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 4 (latency to fetch first node)\n");
	printk("        non-cached: %llu us\n", (unsigned long long)time);

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++)
		node = rb_first_cached(&root);

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk("        cached: %llu us\n", (unsigned long long)time);

	for (i = 0; i < nnodes; i++)
		erase(nodes + i, &root);

	/* run checks */
	for (i = 0; i < check_loops; i++) {
		init();
		for (j = 0; j < nnodes; j++) {
			check(j);
			insert(nodes + j, &root);
		}
		for (j = 0; j < nnodes; j++) {
			check(nnodes - j);
			erase(nodes + j, &root);
		}
		check(0);
	}

	printk(KERN_ALERT "augmented rbtree testing");

	init();

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++) {
		for (j = 0; j < nnodes; j++)
			insert_augmented(nodes + j, &root);
		for (j = 0; j < nnodes; j++)
			erase_augmented(nodes + j, &root);
	}

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 1 (latency of nnodes insert+delete): %llu us\n", (unsigned long long)time);

	gettimeofday(&time1, NULL);

	for (i = 0; i < perf_loops; i++) {
		for (j = 0; j < nnodes; j++)
			insert_augmented_cached(nodes + j, &root);
		for (j = 0; j < nnodes; j++)
			erase_augmented_cached(nodes + j, &root);
	}

	gettimeofday(&time2, NULL);
	time_delta = DIFF_US(time2, time1);

	time_delta = time_delta / perf_loops;
	printk(" -> test 2 (latency of nnodes cached insert+delete): %llu us\n", (unsigned long long)time);

	for (i = 0; i < check_loops; i++) {
		init();
		for (j = 0; j < nnodes; j++) {
			check_augmented(j);
			insert_augmented(nodes + j, &root);
		}
		for (j = 0; j < nnodes; j++) {
			check_augmented(nnodes - j);
			erase_augmented(nodes + j, &root);
		}
		check_augmented(0);
	}

	free(nodes);

	return -EAGAIN; /* Fail will directly unload the module */
}

static void __exit rbtree_test_exit(void)
{
	printk(KERN_ALERT "test exit\n");
}

// module_init(rbtree_test_init)
// module_exit(rbtree_test_exit)

// MODULE_LICENSE("GPL");
// MODULE_AUTHOR("Michel Lespinasse");
// MODULE_DESCRIPTION("Red Black Tree test");


int main(int argc, const char * argv[])
{
    rbtree_test_init();
    rbtree_test_exit();

    return 0;
}