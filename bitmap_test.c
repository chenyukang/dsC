
// @Name   : bitmap_test.c 
//
// @Author : Yukang Chen (moorekang@gmail.com)
// @Date   : 2012-03-01 22:24:13
//
// @Brief  :

#include "bitmap.h"
#include <assert.h>
#include <stdio.h>

int main()
{
    bitmap* bm = bitmap_new(10000);
    printf("bm size : %u\n", bm->cnt );
    assert(bm);
    bitmap_set(bm, 1);
    assert(bitmap_tst(bm, 1));
    bitmap_clr(bm, 1);
    assert(bitmap_tst(bm, 1) == 0);
    assert(bitmap_tst(bm, 2) == 0);
    int k;
    for(k=0; k<100; k++)
    {
        bitmap_set(bm, k);
        assert(bitmap_tst(bm, k));
        bitmap_clr(bm, k);
        assert(bitmap_tst(bm, k) == 0);
    }
    bitmap_del(bm);
    return 0;
}
