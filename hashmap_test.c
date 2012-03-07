
// @Name   : hashmap_test.c 
//
// @Author : Yukang Chen (moorekang@gmail.com)
// @Date   : 2012-03-06 23:29:39
//
// @Brief  :

#include "hashmap.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

unsigned int_hash(const void* a)
{
    return (*(int*)a) % 1000000 + 1;
}

int int_cmp(const void* a, const void* b)
{
    int av = *(int*)a;
    int bv = *(int*)b;
    return av - bv;
}

void* int_dup(const void* key)
{
    int* res = (int*)malloc(sizeof(int));
    *res = *(int*)key;
    return res;
}

void int_rel(const void* key)
{
    free((int*)key);
}

int main()
{
    hashmap* hsmap = hsmap_new(&int_hash, &int_cmp, &int_dup, &int_dup,
                               &int_rel, &int_rel);

    assert(hsmap);
    int k;
    for(k=0; k<1000; k++)
    {
        int val = k+1;
        //printf("insert1 : %d val:%d\n", k, val);
        assert(hsmap_insert(hsmap, &k, &val));
//        printf("find1 : %d\n", k);
        void* v = hsmap_find(hsmap, &k);
        assert( v );
        assert(*(int*)v == val);

        val = k-1;
        
        //      printf("insert : %d val:%d\n", k, val);
        assert(hsmap_insert(hsmap, &k, &val));
        //printf("find : %d\n", k);
        v = hsmap_find(hsmap, &k);
        assert(v);
        assert(( *(int*)v == val));

#if 0
        void* v = hsmap_find(hsmap, &k);
        if( v )
            printf("found : %d val:%d\n", k, *(int*)v);
        else
            printf("no found : %d\n", k);
#endif 
    }
    
    
}
