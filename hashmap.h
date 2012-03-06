
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

typedef unsigned (*hash_f) (const void* key);
typedef int      (*cmp_f)  (const void* a, const void* b);
typedef void*    (*keydup_f) (const void* key);
typedef void*    (*valdup_f) (const void* val);
typedef void*    (*keyrel_f) (const void* key);
typedef void*    (*valrel_f) (const void* val);

typedef struct {
    void* key;
    void* val;
    u32 next;
}hashitem;


typedef struct {
    u32 hashsize;
    u32 codesize;
    u32 count;
    u32 emptyidx;
    u32* codelist;
    hashitem* hashlist;

    hash_f hash;
    cmp_f  cmp;

    keydup_f keydup;
    valdup_f valdup;
    keyrel_f keyrel;
    valrel_f valrel;
}hashmap;

hashmap* hsmap_new(u32 size, hash_f hash, cmp_f cmp,
                   keydup_f keydup, valdup_f valdup,
                   keyrel_f keyrel, valrel_f valrel);
void hsmap_del(hashmap* hsmap);
#endif

