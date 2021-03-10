
#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_CAP 32768
#define random(x) (rand()%(x))

void traverse(Tree *t)
{
    Node* curnode = NULL;

    printf("Traversing\n");
    curnode = TreeNextElement(t, curnode);
    printf("\tcontent int %d\n", *(int*)(curnode->content));
    while (curnode) {
        curnode = TreeNextElement(t, curnode);
        if (curnode) {
            printf("\tcontent int %d\n", *(int*)(curnode->content));
        }
    }
    printf("End traverse\n");
}

int test_rand()
{
    int i = 0;
    srand(time(NULL));
    void* random_poll = malloc(sizeof(int) * TEST_CAP);
    for (i = 0; i < TEST_CAP; i++) {
        *((int *)random_poll + i) = random(TEST_CAP);
    }

    Tree* t = TreeInit(TreeIntCompare);
    for (i = 0; i < TEST_CAP; i++) {
        TreeAdd(t, (void *)((int *)random_poll + i), sizeof(int));
        // printf("Add %d\n", *((int *)random_poll + i));
    }

    printf("After add tree size %zu\n", t->size / sizeof(int));

    for (i = 0; i < TEST_CAP; i++) {
        (void)TreeRemove(t, (void *)((int *)random_poll + i));
        // printf("Remove %d\n", *((int *)random_poll + i));
    }

    printf("After remove tree size %zu\n", t->size / sizeof(int));

    TreeFree(t);

    return 0;
}

int main(int argc, const char * argv[])
{
    int rc = 0;
    rc = test_rand();
    if (rc != 0) {
        return rc;
    }

    return rc;
}
