#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hashmap.h"

static int32_t get_code(void* key, int32_t size, int32_t keysize)
{
    int32_t n = 0;
    unsigned char ch;
    void* buff;

    buff = key;
    while(keysize > 0)
    {
        ch = *(char*)(buff++);
        keysize--;
        if(ch == 0 ) continue;
        n <<= 1;
        n += ch;
    }
    int32_t ret = (n % size);
    return ret;
}

/* return true if success, false error happened*/
static int
hashtable_init(hashmap* thiz, u32 hashsize){
    u32 n;
    thiz->hashsize = hashsize;
    thiz->codesize = HASH_OPTIMAL_RATE * hashsize;
    thiz->count    = 0;
    thiz->emptyidx = 0;

    thiz->codelist = (u32*)calloc(thiz->codesize, sizeof(u32));
    thiz->hashlist = (hashitem*)calloc(thiz->hashsize,
                                     sizeof(hashitem));
    
    if( thiz->hashlist == NULL ||
        thiz->codelist == NULL ) {
        free(thiz->hashlist);
        free(thiz->codelist);
        return 0;
    }

    for(n=0; n < thiz->hashsize; n++) {
        thiz->hashlist[n].key   = NULL;
        thiz->hashlist[n].val   = NULL;
        thiz->hashlist[n].next  = n+1;

    }
    for(n=0; n < thiz->codesize; n++)
        thiz->codelist[n] = -1;
    return 1;
}


/* return true iff add success
   return false iff key and value all be found in hashtable
*/
static int
internal_add(hashmap* thiz, void* key, void* val)
{
    u32 idx, empty;

    idx = thiz->hash(key);
    if(thiz->codelist[idx] == -1) {
        empty = thiz->emptyidx;
        thiz->hashlist[empty].key = thiz->keydup(key);
        thiz->hashlist[empty].val = thiz->valdup(val);
        thiz->codelist[idx]       = thiz->emptyidx;
        thiz->emptyidx            = thiz->hashlist[empty].next;
        thiz->hashlist[empty].next = -1;
    }

    idx = thiz->codelist[idx];
    assert( idx>=0 );
    while( idx < thiz->hashsize ) {
        if (thiz->cmp( thiz->hashlist[idx].key,
                         key ) == 0 ) {
            thiz->valrel( thiz->hashlist[idx].val );
            thiz->hashlist[idx].val = thiz->valdup(val);
            return 0;
        }

        if( thiz->hashlist[idx].next == -1) {
            empty = thiz->emptyidx;
            thiz->hashlist[empty].key = thiz->keydup(key);
            thiz->hashlist[empty].val = thiz->valdup(val);
            thiz->codelist[idx]       = thiz->emptyidx;
            thiz->emptyidx            = thiz->hashlist[empty].next;
            thiz->hashlist[empty].next = -1;
            thiz->count++;
            return 1;
        }
        idx = thiz->hashlist[idx].next;
    }
    return 0;
}

/* check hash table space, expand it iff neccessary
   return true success,
   return false iff error happened
*/
static int
check_full(hashmap* hsmap)
{
    if(hsmap->count < hsmap->hashsize)
        return 1;

    u32 n, step;
    hashmap* new_hash = (hashmap*)malloc(sizeof(hashmap));
    assert(hsmap);
    new_hash->keydup = hsmap->keydup;
    new_hash->valdup = hsmap->valdup;
    new_hash->keyrel = hsmap->keyrel;
    new_hash->valrel = hsmap->valrel;


    step = hsmap->count * HASH_STEP_RATE;
    step = step<HASH_STEP_MIN ? HASH_STEP_MIN :
        (step>HASH_STEP_MAX ? HASH_STEP_MAX : step);

    new_hash->hashsize = hsmap->hashsize + step;
    hashtable_init(new_hash, new_hash->hashsize);
    
    for(n=0; n<hsmap->hashsize; n++){
        if(hsmap->hashlist[n].key == NULL) continue;
        if(!internal_add(new_hash, hsmap->hashlist[n].key,
                         hsmap->hashlist[n].val)) {
            hsmap_del(new_hash);
            return 0;
        }
    }

    for(n=0; n<hsmap->hashsize; n++) {
        hsmap->keyrel(hsmap->hashlist[n].key);
        hsmap->valrel(hsmap->hashlist[n].val);
    }
    free(hsmap->hashlist);
    free(hsmap->codelist);
    hsmap->hashlist = new_hash->hashlist;
    hsmap->codelist = new_hash->codelist;
    hsmap->hashsize = new_hash->hashsize;
    hsmap->count    = new_hash->count;
    free(new_hash);
    return 1;
}


void
hsmap_del(hashmap* hsmap)
{
    if(hsmap == NULL) return;
    u32 k;
    for(k=0; k<hsmap->hashsize; k++) {
        hsmap->keyrel(hsmap->hashlist[k].key);
        hsmap->valrel(hsmap->hashlist[k].val);
    }
    free(hsmap->hashlist);
    free(hsmap->codelist);
    free(hsmap);
    hsmap = NULL;
}

/* @ 0 : add failed
   @ 1 : add success */
int
hsmap_insert(hashmap* hsmap, void* key, void* value)
{
    if(!check_full(hsmap))
        return 0;
    if(internal_add(hsmap, key, value))
        hsmap->count++;
    return 1;
}

/* @ NULL : no found
   @ *val : pointer of value */
void* hsmap_find(hashmap* hsmap, void* key)
{
    assert(hsmap);
    u32 index = hsmap->hash(key);
    assert( index >= 0 );
    if( hsmap->codelist[index] == -1)
        return NULL;

    index = hsmap->codelist[index];
    while( index < hsmap->hashsize ) {
        assert(index < hsmap->hashsize
               && "hash find: index < hashsize");
        if( hsmap->cmp(hsmap->hashlist[index].key,
                       key) == 0 ) {
            return hsmap->hashlist[index].val;
        }
        if( hsmap->hashlist[index].next == -1 )
            break;
        index = hsmap->hashlist[index].next;
    }
    return NULL;
}

hashmap*
hsmap_new(u32 size, hash_f hash, cmp_f cmp,
          keydup_f keydup, valdup_f valdup,
          keyrel_f keyrel, valrel_f valrel) {
    
    hashmap* hsmap = (hashmap*)malloc(sizeof(hashmap));
    assert(hsmap);
    hsmap->keydup = keydup;
    hsmap->valdup = valdup;
    hsmap->keyrel = keyrel;
    hsmap->valrel = valrel;
    hashtable_init(hsmap, 10);
    return hsmap;
}
