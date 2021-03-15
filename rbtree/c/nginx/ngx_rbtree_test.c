#include "ngx_rbtree.h"

#include <stdio.h>

int test_rand()
{
    return 0;
}

int main(int argc, const char * argv[])
{
    int rc = 0;
    rc = test_rand();
    if (rc != 0) {
        printf("test_rand faild return: %d\n", rc);
        return rc;
    }

    return 0;
}