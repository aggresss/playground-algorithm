#include "ngx_rbtree.h"

#include <stdio.h>      /* printf */
#include <stddef.h>     /* offsetof */
#include <stdint.h>     /* uintptr_t */
#include <string.h>     /* malloc */
#include <time.h>       /* time */
#include <inttypes.h>   /* PRIuPTR */
#include <assert.h>     /* assert */

#define random(x) (rand()%(x))

#define TEST_CAP 256

typedef struct ngx_blob_s {
    uintptr_t value;
    ngx_rbtree_node_t node;
} ngx_blob_t;

int test_rand()
{
    // Setup
    int i = 0;
    ngx_rbtree_t rbtree;
    ngx_rbtree_node_t sentinel = {0, NULL, NULL, NULL, 0};
    ngx_rbtree_init(&rbtree, &sentinel, ngx_rbtree_insert_value);
    srand(time(NULL));

    ngx_blob_t *blob_pool = malloc(sizeof(ngx_blob_t) * TEST_CAP);
    memset(blob_pool, 0x00, sizeof(ngx_blob_t) * TEST_CAP);
    for(i = 0; i < TEST_CAP; i++) {
        blob_pool[i].node.key = blob_pool[i].value = i;
    }
    for(i = 0; i < TEST_CAP / 2; i++) {
        int pos1 = random(TEST_CAP);
        int pos2 = random(TEST_CAP);
        ngx_blob_t temp;
        if (pos1 != pos2) {
            temp = blob_pool[pos1];
            blob_pool[pos1] = blob_pool[pos2];
            blob_pool[pos2] = temp;
        }
    }

    // Test insert
    for(i = 0; i < TEST_CAP; i++) {
        ngx_rbtree_insert(&rbtree, &blob_pool[i].node);
    }

    // Test next
    ngx_blob_t *current = (ngx_blob_t *)((char *)ngx_rbtree_min(rbtree.root, &sentinel) - offsetof(ngx_blob_t, node));
    assert(current->value == 0);
    printf("%"PRIuPTR"\n", current->value);

    for(i = 1; i < TEST_CAP; i++) {
        current = (ngx_blob_t *)((char *)ngx_rbtree_next(&rbtree, &current->node) - offsetof(ngx_blob_t, node));
        assert(current->value == i);
    }

    // Test delete
    for(i = 0; i < TEST_CAP; i++) {
        ngx_rbtree_delete(&rbtree, &blob_pool[i].node);
    }
    assert(rbtree.root == &sentinel);

    // Teardown
    free(blob_pool);

    return 0;
}

int main(int argc, const char * argv[])
{
    int rc = 0;
    rc = test_rand();
    if (rc != 0) {
        printf("test_rand failed return: %d\n", rc);
        return rc;
    }

    return 0;
}
