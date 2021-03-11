#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LEFT 0
#define RIGHT 1

#if !defined(max)
#define max(a,b) (((a)>(b))?(a):(b))
#endif

static int isRed(Node* aNode);
static int isBlack(Node* aNode);

static void TreeRotate(Tree* aTree, Node* curnode, int direction);
static Node* TreeMinimum(Node* curnode);
static Node* TreeSuccessor(Node* curnode);
static void TreeBalanceAfterAdd(Tree* aTree, Node* curnode);
static Node* TreeBAASub(Tree* aTree, Node* curnode, int which);
static void TreeBalanceAfterRemove(Tree* aTree, Node* curnode);
static Node* TreeBARSub(Tree* aTree, Node* curnode, int which);
static void* TreeRemoveNode(Tree* aTree, Node* curnode);

static int isRed(Node* aNode)
{
    return (aNode != NULL) && (aNode->red);
}

static int isBlack(Node* aNode)
{
    return (aNode == NULL) || (aNode->red == 0);
}

static void TreeRotate(Tree* aTree, Node* curnode, int direction)
{
    Node* other = curnode->child[!direction];

    curnode->child[!direction] = other->child[direction];
    if (other->child[direction] != NULL)
        other->child[direction]->parent = curnode;
    other->parent = curnode->parent;
    if (curnode->parent == NULL) {
        aTree->root = other;
    } else if (curnode == curnode->parent->child[direction]) {
        curnode->parent->child[direction] = other;
    } else {
        curnode->parent->child[!direction] = other;
    }

    other->child[direction] = curnode;
    curnode->parent = other;
}

static Node* TreeMinimum(Node* curnode)
{
    if (curnode) {
        while (curnode->child[LEFT]) {
            curnode = curnode->child[LEFT];
        }
    }

    return curnode;
}

static Node* TreeSuccessor(Node* curnode)
{
    if (curnode->child[RIGHT]) {
        curnode = TreeMinimum(curnode->child[RIGHT]);
    } else {
        Node* curparent = curnode->parent;
        while (curparent && curnode == curparent->child[RIGHT]) {
            curnode = curparent;
            curparent = curparent->parent;
        }
        curnode = curparent;
    }
    return curnode;
}

static void TreeBalanceAfterAdd(Tree* aTree, Node* curnode)
{
    while (curnode && isRed(curnode->parent) && curnode->parent->parent) {
        if (curnode->parent == curnode->parent->parent->child[LEFT]) {
            curnode = TreeBAASub(aTree, curnode, RIGHT);
        } else {
            curnode = TreeBAASub(aTree, curnode, LEFT);
        }
    }
    aTree->root->red = 0;
}

/* Tree balance after add substantial */
static Node* TreeBAASub(Tree* aTree, Node* curnode, int which)
{
    Node* uncle = curnode->parent->parent->child[which];

    if (isRed(uncle)) {
        curnode->parent->red = uncle->red = 0;
        curnode = curnode->parent->parent;
        curnode->red = 1;
    } else {
        if (curnode == curnode->parent->child[which]) {
            curnode = curnode->parent;
            TreeRotate(aTree, curnode, !which);
        }
        curnode->parent->red = 0;
        curnode->parent->parent->red = 1;
        TreeRotate(aTree, curnode->parent->parent, which);
    }
    return curnode;
}

static void TreeBalanceAfterRemove(Tree* aTree, Node* curnode)
{
    while (curnode != aTree->root && isBlack(curnode)) {
        /* curnode->content == NULL must equal curnode == NULL */
        if (((curnode->content) ? curnode : NULL) == curnode->parent->child[LEFT]) {
            curnode = TreeBARSub(aTree, curnode, RIGHT);
        } else {
            curnode = TreeBARSub(aTree, curnode, LEFT);
        }
    }
    curnode->red = 0;
}

/* Tree balance after remove substantial */
static Node* TreeBARSub(Tree* aTree, Node* curnode, int which)
{
    Node* sibling = curnode->parent->child[which];

    if (isRed(sibling)) {
        sibling->red = 0;
        curnode->parent->red = 1;
        TreeRotate(aTree, curnode->parent, !which);
        sibling = curnode->parent->child[which];
    }
    if (!sibling) {
        curnode = curnode->parent;
    } else if (isBlack(sibling->child[!which]) && isBlack(sibling->child[which])) {
        sibling->red = 1;
        curnode = curnode->parent;
    } else {
        if (isBlack(sibling->child[which])) {
            sibling->child[!which]->red = 0;
            sibling->red = 1;
            TreeRotate(aTree, sibling, which);
            sibling = curnode->parent->child[which];
        }
        sibling->red = curnode->parent->red;
        curnode->parent->red = 0;
        sibling->child[which]->red = 0;
        TreeRotate(aTree, curnode->parent, !which);
        curnode = aTree->root;
    }
    return curnode;
}

static void* TreeRemoveNode(Tree* aTree, Node* curnode)
{
    Node* redundant = curnode;
    Node* curchild = NULL;
    size_t size = curnode->size;
    void* content = curnode->content;

    /* if the node to remove has 0 or 1 children, it can be removed without involving another node */
    if (curnode->child[LEFT] && curnode->child[RIGHT]) { /* 2 children */
        redundant = TreeSuccessor(curnode); /* now redundant must have at most one child */
    }

    curchild = redundant->child[(redundant->child[LEFT] != NULL) ? LEFT : RIGHT];
    if (curchild) { /* we could have no children at all */
        curchild->parent = redundant->parent;
    }

    if (redundant->parent == NULL) {
        aTree->root = curchild;
    } else {
        if (redundant == redundant->parent->child[LEFT]) {
            redundant->parent->child[LEFT] = curchild;
        } else {
            redundant->parent->child[RIGHT] = curchild;
        }
    }

    if (redundant != curnode) {
        curnode->content = redundant->content;
        curnode->size = redundant->size;
    }

    if (isBlack(redundant)) {
        if (curchild == NULL) {
            if (redundant->parent) {
                Node temp;
                memset(&temp, '\0', sizeof(Node));
                temp.parent = redundant->parent;
                temp.red = 0;
                TreeBalanceAfterRemove(aTree, &temp);
            }
        } else {
            TreeBalanceAfterRemove(aTree, curchild);
        }
    }

    free(redundant);

    aTree->size -= size;
    --(aTree->count);

    return content;
}

Tree* TreeInit(int (*compare)(void*, void*))
{
    Tree* newt = malloc(sizeof(Tree));
    memset(newt, '\0', sizeof(Tree));
    newt->compare = compare;
    return newt;
}

void TreeFree(Tree* aTree)
{
    if (aTree) {
        free(aTree);
    }
}

Node* TreeFind(Tree* aTree, void* content)
{
    int result = 0;
    Node* curnode = aTree->root;

    while (curnode) {
        result = aTree->compare(curnode->content, content);
        if (result == 0) {
            break;
        } else {
            curnode = curnode->child[result > 0];
        }
    }
    return curnode;
}

Node* TreeNextElement(Tree* aTree, Node* curnode)
{
    if (curnode == NULL) {
        curnode = TreeMinimum(aTree->root);
    } else {
        curnode = TreeSuccessor(curnode);
    }

    return curnode;
}

Node* TreeAdd(Tree* aTree, void* content, size_t size)
{
    Node* curparent = NULL;
    Node* curnode = aTree->root;
    Node* newel = NULL;
    int which = 0;
    int result = 1;
    Node* rc = NULL;

    while (curnode) {
        result = aTree->compare(curnode->content, content);
        which = (result > 0);
        if (result == 0) {
            break;
        } else {
            curparent = curnode;
            curnode = curnode->child[which];
        }
    }

    if (result == 0) {
        goto exit;
    } else {
        newel = malloc(sizeof(Node));
        memset(newel, '\0', sizeof(Node));
        if (curparent) {
            curparent->child[which] = newel;
        } else {
            aTree->root = newel;
        }
        newel->parent = curparent;
        newel->red = 1;

        ++(aTree->count);
        aTree->size += size;
    }
    newel->content = content;
    newel->size = size;
    TreeBalanceAfterAdd(aTree, newel);
    rc = newel;
exit:
    return rc;
}

void* TreeRemove(Tree* aTree, void* content)
{
    Node* curnode = TreeFind(aTree, content);

    if (curnode == NULL) {
        return NULL;
    }

    return TreeRemoveNode(aTree, curnode);
}

int TreeIntCompare(void* a, void* b)
{
    int i = *((int*)a);
    int j = *((int*)b);

    return (i > j) ? -1 : (i == j) ? 0 : 1;
}


int TreePtrCompare(void* a, void* b)
{
    return (a > b) ? -1 : (a == b) ? 0 : 1;
}


int TreeStringCompare(void* a, void* b)
{
    return strcmp((char*)a, (char*)b);
}

