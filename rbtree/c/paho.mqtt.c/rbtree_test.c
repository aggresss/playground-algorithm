
#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TEST_CAP 1024
#define random(x) (rand()%(x))

static inline int isRed(Node* aNode)
{
    return (aNode != NULL) && (aNode->red);
}

static inline int isBlack(Node* aNode)
{
    return (aNode == NULL) || (aNode->red == 0);
}

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

int TreeDepth(Tree* aTree)
{
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

#define ERR_OK                          (0)
#define ERR_ROOT_NOT_BLACK              (-1)
#define ERR_RED_NODE_CHILD_NOT_BLACK    (-2)
#define ERR_PATH_BLACK_NOT_EQUAL        (-3)

int TreeCheck(Tree* aTree)
{
    if (!isBlack(aTree->root)) {
        return ERR_ROOT_NOT_BLACK;
    }
    int i = 0, j = 0;
    int tree_depth = TreeDepth(aTree);
    Node* curnode = TreeNext(aTree, NULL);
    while (curnode != NULL) {
        // check red children
        if (isRed(curnode)) {
            if (!isBlack(curnode->child[0]) || !isBlack(curnode->child[1])) {
                return ERR_RED_NODE_CHILD_NOT_BLACK;
            }
        }
        // check curnode leaf path black
        int path_black_cache = 0;
        for (i = 0; i < (int)pow(2, tree_depth - 1); i++) {
            Node* temp_node = curnode;
            int path_black_temp = 0;
            for (j = 0; j < tree_depth; j++) {
                if (temp_node == NULL) {
                    break;
                }
                if (isBlack(temp_node)) {
                    path_black_temp++;
                }
                temp_node = temp_node->child[(i & (int)pow(2, j)) > 0];
            }
            if (path_black_cache == 0) {
                path_black_cache = path_black_temp;
            } else if (path_black_cache != path_black_temp) {
                return ERR_PATH_BLACK_NOT_EQUAL;
            }
        }
        curnode = TreeNext(aTree, curnode);
    }

    return ERR_OK;
}

int test_rand()
{
    int check_result = 0;
    int i = 0;
    srand(time(NULL));
    void* random_poll = malloc(sizeof(int) * TEST_CAP);
    for (i = 0; i < TEST_CAP; i++) {
        *((int *)random_poll + i) = random(TEST_CAP);
    }

    Tree* t = TreeInit(TreeIntCompare);
    for (i = 0; i < TEST_CAP; i++) {
        TreeAdd(t, (void *)((int *)random_poll + i), sizeof(int));
        printf("Add %d: %d\n", i, *((int *)random_poll + i));
        check_result = TreeCheck(t);
        if (check_result != 0) {
            printf("Tree check error: %d", check_result);
            return -1;
        }
    }

    printf("After add tree size %zu\n", t->size / sizeof(int));

    printf("After add tree depth %d\n", TreeDepth(t));

    for (i = 0; i < TEST_CAP; i++) {
        (void)TreeRemove(t, (void *)((int *)random_poll + i));
        printf("Remove %d: %d\n", i, *((int *)random_poll + i));
        check_result = TreeCheck(t);
        if (check_result != 0) {
            printf("Tree check error: %d", check_result);
            return -1;
        }
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
        printf("test_rand faild return: %d", rc);
        return rc;
    }

    return 0;
}
