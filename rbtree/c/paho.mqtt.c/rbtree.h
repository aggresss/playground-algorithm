#if !defined(TREE_H)
#define TREE_H

#include <stdlib.h>

/**
 * Structure to hold all data for one tree element
 */
typedef struct NodeStruct {
    struct NodeStruct *parent;   /**< pointer to parent tree node, in case we need it */
    struct NodeStruct *child[2]; /**< pointers to child tree nodes 0 = left, 1 = right */
    void* content;               /**< pointer to element content */
    size_t size;                 /**< size of content */
    unsigned int red : 1;
} Node;

/**
 * Structure to hold all data for one tree
 */
typedef struct {
    Node *root;                     /**< root node pointer */
    int (*compare)(void*, void*);   /**< comparison function */
    int count;                      /**< count of items */
    size_t size;                    /**< heap storage used */
} Tree;

Tree* TreeInit(int(*compare)(void*, void*));

void TreeFree(Tree* aTree);

Node* TreeFind(Tree* aTree, void* content);

Node* TreeNextElement(Tree* aTree, Node* curnode);

Node* TreeAdd(Tree* aTree, void* content, size_t size);

void* TreeRemove(Tree* aTree, void* content);

int TreeIntCompare(void* a, void* b);
int TreePtrCompare(void* a, void* b);
int TreeStringCompare(void* a, void* b);

#endif  /* TREE_H */

