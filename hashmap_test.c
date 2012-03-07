
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


//#define NEWHASH 1

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

unsigned int_hash(const void* a)
{
    return (*(int*)a) % 10000000 + 1;
}
#endif

unsigned int_hash(const void* a)
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
    for(k=0; k<1000000; k++)
    {
        int val = k+1;
        hsmap_insert(hsmap, &k, &val);
        void* v = hsmap_find(hsmap, &k);
        assert( v );
        assert(*(int*)v == val);

#if 0
        val = k-1;
        assert(hsmap_insert(hsmap, &k, &val));
        v = hsmap_find(hsmap, &k);
        assert(v);
        assert(( *(int*)v == val));
#endif
    }
    return 0; 
}
