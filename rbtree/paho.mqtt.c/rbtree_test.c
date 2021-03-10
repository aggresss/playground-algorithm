
#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, const char * argv[])
{
    int rc = 0;
    int *ip = NULL;
    int i = 0;
    Node *node_l = NULL;
    Tree* t = TreeInit(TreeIntCompare);

    ip = malloc(sizeof(int));
    *ip = 2;
    TreeAdd(t, (void*) ip, sizeof(int));

    ip = malloc(sizeof(int));
    *ip = 4;
    TreeAdd(t, (void*) ip, sizeof(int));

    ip = malloc(sizeof(int));
    *ip = 3;
    TreeAdd(t, (void*) ip, sizeof(int));

    ip = malloc(sizeof(int));
    *ip = 1;
    TreeAdd(t, (void*) ip, sizeof(int));

    i = 3;
    node_l = TreeFind(t, &i);
    if (node_l) {
        printf("Find node %d\n", *(int *)node_l->content);
    }

    traverse(t);

    for (i = 4; i > 0; i--) {
        ip = TreeRemove(t, &i);
        if (ip) {
            free(ip);
        }
    }

    TreeFree(t);

    return rc;

}
