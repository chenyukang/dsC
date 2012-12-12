
/*******************************************************************************
 *
 *      chainhash_test.c
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-03-08 12:02:33
 *
 *      COPYRIGHT (C) 2011, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/


#include "chainhash.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>


typedef unsigned int u32;

#if 0
unsigned int_hash(const void* a)
{
    u32 key = (u32)(*(int*)a);
    key = ~key + (key << 15); // key = (key << 15) - key - 1; 
    key = key ^ (key >> 12); 
    key = key + (key << 2); 
    key = key ^ (key >> 4); 
    key = key * 2057; // key = (key + (key << 3)) + (key << 11); 
    key = key ^ (key >> 16); 
    return key;
}
#endif

unsigned int_hash1(const void* a)
{
    return (*(int*)a) % 10000000 + 1;
}

//#if 0
unsigned int_hash2(const void* a)
{
    u32 key = (u32)(*(int*)a);
    key = (key+0x7ed55d16) + (key<<12); 
    key = (key^0xc761c23c) ^ (key>>19); 
    key = (key+0x165667b1) + (key<<5); 
    key = (key+0xd3a2646c) ^ (key<<9); 
    key = (key+0xfd7046c5) + (key<<3);
    key = (key^0xb55a4f09) ^ (key>>16);  
    return key;
}
//#endif

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

void int_rel(void* key)
{
    free((int*)key);
}

int main()
{
    hs_table* hs = hs_new(100, &int_hash2, &int_cmp, &int_dup, &int_dup,
                               &int_rel, &int_rel);

    assert(hs);
    int k;
    srand(time(NULL));
    for(k=0; k<100000000; k++)
    {
        int key = rand()%100000;
        //printf("insert: %d\n", key);
        int val = key+1;
        hs_insert(hs, &key, &val);
        void* v = hs_find(hs, &key);
        assert( v );
        assert(*(int*)v == val);

        hs_erase(hs, &key);
        assert(hs_find(hs, &key) == NULL);

#if 0
        val = k-1;
        assert(hs_insert(hs, &k, &val));
        v = hs_find(hs, &k);
        assert(v);
        assert(( *(int*)v == val));
#endif
    }
    return 0;  
}

