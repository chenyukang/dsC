
/*******************************************************************************
 *
 *      skip_list_test.c
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-02-24 16:10:01
 *
 *      COPYRIGHT (C) 2011, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/

#include "skiplist.h"

#include <time.h>
#include <assert.h>

static int MINVALUE = 0x80000000;
static int MAXVALUE = 0x7FFFFFFF;

int int_cmp(const void* a, const void* b)
{
    return (*(int*)a) - (*(int*)b);
}

void int_show(const void* a)
{
    printf("%d", (*(int*)a));
}

int main()
{
    skiplist* sl = sl_new(20, sizeof(int), int_cmp, int_show);
    assert(sl);
    srand(time(NULL));
#if 0
    int a = 1;
    int b = 3;
    
    sl_insert(sl, &a);
    sl_insert(sl, &b);
    assert(sl_search(sl, &a) != NULL);
    assert(sl_search(sl, &b) != NULL);
#endif
    int k=0;
    for(k=0; k<10; k++)
    {
        //int v = rand() % 10000;
        int v = k;
        sl_insert(sl, &v);
        assert(sl_search(sl, &v) != NULL);
        sl_print(sl);
        //printf("lev: %d\n", sl->lev);
    }

    for(k=0; k<10; k++)
    {
        sl_delete(sl, &k);
        assert(sl_search(sl, &k) == NULL);
        //printf("lev : %d\n", sl->lev);
        sl_print(sl);
    }
    return 0;
}

