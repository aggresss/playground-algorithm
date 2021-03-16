#include "rbtree_augmented.h"

#include <stdio.h>      /* printf */
#include <stddef.h>     /* offsetof */
#include <stdint.h>     /* uintptr_t */
#include <string.h>     /* malloc */
#include <time.h>       /* time */
#include <inttypes.h>   /* PRIuPTR */
#include <assert.h>     /* assert */

int test_rand()
{
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