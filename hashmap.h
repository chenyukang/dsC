
/*******************************************************************************
 *
 *      HASHMAP_H
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-03-03 17:21:57
 *
 *      COPYRIGHT (C) 2011,  ALL RIGHTS RESERVED.
 *
 *******************************************************************************/

#if !defined(HASHMAP_H)
#define HASHMAP_H


#define HASH_OPTIMAL_RATE 2
#define HASH_STEP_MIN     216
#define HASH_STEP_RATE    1
#define HASH_STEP_MAX     21600

typedef unsigned int u32;

typedef struct {
    void* key;
    void* value;
    int next;
}hashitem;


typedef struct {
    u32 hashsize;
    u32 codesize;
    u32 keysize;
    u32 count;
    u32 emptyidx;
    u32* codelist;
    item* hashlist;
}hashmap;

hashmap* hsmap_new(u32 size);

#endif

