
// @Name   : bitmap.c 
//
// @Author : Yukang Chen (moorekang@gmail.com)
// @Date   : 2012-03-01 22:07:09
//
// @Brief  :


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bitmap.h"

#define INDEX(a)  ((a)/(sizeof(u32)*8))
#define OFFSET(a) ((a)%(sizeof(u32)*8))

#define first_zerobit(x) (_first_onebit(~(x)))
#define first_onebit(x)  (_first_onebit(x))

/* ffs: if ret == 0 : no one bit found
   return index is begin with 1,
   this maybe just work in x86 platform */
static inline int _first_onebit(u32 x)
{
    if (!x) {
        return 0;
    }
    else {
        int ret;
        __asm__("bsfl %1, %0; incl %0" : "=r" (ret) : "r" (x));
        return ret;
    }
}


bitmap* bitmap_new(u32 size)
{
    assert( size>0 && "size <= 0" );
    bitmap* bm = (bitmap*)malloc(sizeof(bitmap));
    assert(bm);
    
    u32 cnt = INDEX(size);
    if(OFFSET(size))
        cnt++;
    assert(cnt);

    bm->size = size;
    bm->cnt  = cnt;
    bm->table = (u32*)malloc(sizeof(u32) * cnt);
    if(bm->table == NULL) {
        free(bm);
        return NULL;
    }
    assert(bm->table);
    memset(bm->table, 0, sizeof(bm->table[0]) * cnt);
    return bm;
}

void bitmap_del(bitmap* bm)
{
    if(bm == NULL)
        return;
    free(bm->table);
    free(bm);
}

inline static
void INIT(u32 val, u32* index, u32* offset)
{
    *index  = INDEX(val);
    *offset = OFFSET(val);
}

void bitmap_set(bitmap* bm, u32 val)
{
    assert(val>=0 && val <= bm->size &&
           "val out of range");
    u32 index, offset;
    INIT(val, &index, &offset);
    bm->table[index] |= ( 0x1<<offset );
}

void bitmap_clr(bitmap* bm, u32 val)
{
    assert(val>=0 && val <= bm->size &&
           "val out of range");
    u32 index, offset;
    INIT(val, &index, &offset);
    bm->table[index] &= ~( 0x1 << offset );
}

int bitmap_tst(bitmap* bm, u32 val)
{
    assert(val>=0 && val <= bm->size &&
           "val out of range");
    u32 index, offset;
    INIT(val, &index, &offset);
    return bm->table[index] & (0x1 << offset );
}

/* the index of first 0,
   return -1 means no 0 bit */
u32 bitmap_first0(bitmap* bm)
{
    assert(bm);
    u32 i;
    for( i = 0; i < bm->cnt; i++) {
        if( bm->table[i] == 0xFFFFFFFF ) //full
            continue;
        u32 v = bm->table[i];
        return 32*i + first_zerobit(v) - 1;
    }
    return -1; 
}

/* the index of first 1,
   return -1 means no 1 bit */
u32 bitmap_first1(bitmap* bm)
{
    assert(bm);
    u32 i;
    for( i = 0; i < bm->cnt; i++ ) {
        if( bm->table[i] == 0x0 ) //empty
            continue;
        u32 v = bm->table[i];
        return 32*i + first_onebit(v) - 1;
    }
    return -1;
}
