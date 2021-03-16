#include "tree.h"

#include <stdio.h>      /* printf */
#include <stdlib.h>     /* rand */
#include <stdint.h>     /* uintptr_t */
#include <string.h>     /* malloc */
#include <time.h>       /* time */
#include <assert.h>     /* assert */

#define random(x) (rand()%(x))

#define TEST_CAP 256

typedef struct blob_s {
    uintptr_t value;
    RB_ENTRY(blob_s) tree_entry;
} blob_t;

static int tree_compare(blob_t* b1, blob_t* b2) {
    if (b1->value < b2->value) return -1;
    if (b1->value > b2->value) return 1;

    return 0;
}

RB_HEAD(blob_tree_s, blob_s);
RB_GENERATE_STATIC(blob_tree_s, blob_s, tree_entry, tree_compare);

int test_rand()
{
    int i = 0;
    blob_t *blob_pool = malloc(sizeof(blob_t) * TEST_CAP);
    memset(blob_pool, 0x00, sizeof(blob_t) * TEST_CAP);

    struct blob_tree_s tree = RB_INITIALIZER(tree);
    for(i = 0; i < TEST_CAP; i++) {
        blob_pool[i].value = i;
    }
    for(i = 0; i < TEST_CAP / 2; i++) {
        int pos1 = random(TEST_CAP);
        int pos2 = random(TEST_CAP);
        blob_t temp;
        if (pos1 != pos2) {
            temp = blob_pool[pos1];
            blob_pool[pos1] = blob_pool[pos2];
            blob_pool[pos2] = temp;
        }
    }
    for(i = 0; i < TEST_CAP; i++) {
        RB_INSERT(blob_tree_s, &tree, &blob_pool[i]);
    }

    blob_t *tmp = RB_MIN(blob_tree_s, &tree);
    for(i = 1; i < TEST_CAP; i++) {
        tmp = RB_NEXT(blob_tree_s, &tree, tmp);
        assert(tmp->value == i);
    }

    for(i = 0; i < TEST_CAP; i++) {
        RB_REMOVE(blob_tree_s, &tree, &blob_pool[i]);
    }
    assert(RB_ROOT(&tree) == NULL);

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