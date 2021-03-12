
#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_CAP 65536
#define random(x) (rand()%(x))

void traverse(Tree *t)
{
    Node* curnode = NULL;

    printf("Traversing\n");
    curnode = TreeNext(t, curnode);
    printf("\tcontent int %d\n", *(int*)(curnode->content));
    while (curnode) {
        curnode = TreeNext(t, curnode);
        if (curnode) {
            printf("\tcontent int %d\n", *(int*)(curnode->content));
        }
    }
    printf("End traverse\n");
}

int TreeDepth(Tree* aTree) {
    int depth = 0;
    Node* curnode = TreeNext(aTree, NULL);
    while (curnode != NULL) {
        int curdepth = 0;
        Node* findnode = aTree->root;
        while (findnode) {
            curdepth++;
            int result = 0;
            result = aTree->compare(findnode->content, curnode->content);
            if (result == 0) {
                if (curdepth > depth) {
                    depth = curdepth;
                }
                break;
            } else {
            findnode = findnode->child[result > 0];
            }
        }
        curnode = TreeNext(aTree, curnode);
    }
    return depth;
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

    printf("After add tree depth %d\n", TreeDepth(t));


    for (i = 0; i < TEST_CAP; i++) {
        (void)TreeRemove(t, (void *)((int *)random_poll + i));
        // printf("Remove %d\n", *((int *)random_poll + i));
    }

    printf("After remove tree size %zu\n", t->size / sizeof(int));

    TreeFree(t);
    free(random_poll);

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
