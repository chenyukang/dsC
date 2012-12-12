
// @Name   : BITMAP_H 
//
// @Author : Yukang Chen (moorekang@gmail.com)
// @Date   : 2012-03-01 22:05:41
//
// @Brief  :

#if !defined(BITMAP_H)
#define BITMAP_H

typedef unsigned int u32;

typedef struct _bitmap{
    u32 size;
    u32 cnt;
    u32* table;
}bitmap;

bitmap* bitmap_new(u32 size);
void    bitmap_del(bitmap* bm);
void    bitmap_set(bitmap* bm, u32 val);
void    bitmap_clr(bitmap* bm, u32 val);
int     bitmap_tst(bitmap* bm, u32 val);
u32     bitmap_first0(bitmap* bm);
u32     bitmap_first1(bitmap* bm);


#endif

