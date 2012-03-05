#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "HimaNDBPtrHash.h"
#include "HimaNDBMemory.h"
#include "HimaNDBInt.h"

static void Hashtable_destruct(void* thz);

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
static bool hashtable_init(HimaNDB_PtrHash* thiz, int32_t hashsize)
{
    int32_t n;

    thiz->hashsize=hashsize;
    thiz->codesize=HASH_OPTIMAL_RATE*hashsize;

    thiz->count=0;
    thiz->emptyidx=0;
    thiz->keysize=sizeof(void*);

    thiz->hashlist=(hashitem*)HimaNDB_calloc(thiz->hashsize,sizeof(hashitem));
    thiz->codelist=(int32_t*)HimaNDB_calloc(thiz->codesize,sizeof(int32_t));
    if(thiz->hashlist==NULL||thiz->codelist==NULL){//acording C99, free(NULL) do nothing
        HimaNDB_free(thiz->hashlist);
        HimaNDB_free(thiz->codelist);
        return false;
    }

    for (n=0;n<thiz->hashsize;n++)
    {
        thiz->hashlist[n].key=NULL;
        thiz->hashlist[n].value=NULL;
        thiz->hashlist[n].next=n+1;

    }
    for (n=0;n<thiz->codesize;n++)
        thiz->codelist[n]=-1;

    return true;
}


/* return true iff add success
   return false iff key and value all be found in hashtable
*/
static bool internal_add(void* thz, void* key, void* value)
{
    int32_t index,empty;

    HimaNDB_PtrHash* thiz = (HimaNDB_PtrHash*)thz;
    index = get_code(key, thiz->codesize, thiz->keysize);

    if(thiz->codelist[index] == -1){
        empty = thiz->emptyidx;
        thiz->hashlist[empty].key = key;
        thiz->hashlist[empty].value = value;
        thiz->codelist[index] = thiz->emptyidx;
        thiz->emptyidx = thiz->hashlist[empty].next;
        thiz->hashlist[empty].next = -1;
        return true;
    }

    index = thiz->codelist[index];
    while( index < thiz->hashsize )
    {
        if(thiz->hashlist[index].key == key&&
           thiz->hashlist[index].value == value)
            return false;
        
        if(thiz->hashlist[index].next == -1){
            empty = thiz->emptyidx;
            
            thiz->hashlist[empty].key=key;
            thiz->hashlist[empty].value=value;
            thiz->hashlist[index].next=empty;
            thiz->emptyidx = thiz->hashlist[empty].next;
            thiz->hashlist[empty].next=-1;
            return true;
        }
        index = thiz->hashlist[index].next;
    }
    return false;
}

/* check hash table space, expand it iff neccessary
   return true success,
   return false iff error happened
*/
static bool check_full(hsmap* hsmap)
{
    if(hsmap->count < hsmap->hashsize) return true;

    hashmap* new_hash = hsmap_new();
    u32 n,step,cnt;

    step = (hsmap->count)*HASH_STEP_RATE;
    step = step<HASH_STEP_MIN ? HASH_STEP_MIN :
          (step>HASH_STEP_MAX ? HASH_STEP_MAX:step);

    new_hash.hashsize = hsmap->hashsize + step;
    hashtable_init(&new_hash, new_hash.hashsize);

    cnt = 0;
    for(n=0; n<hsmap->hashsize; n++){
        if(hsmap->hashlist[n].key == NULL) continue;

        if(!internal_add(&new_hash, hsmap->hashlist[n].key,
                         hsmap->hashlist[n].value)){
            hsmap_del(new_hash);
            return false;
        }
        new_hash.count++;
    }
    free(hsmap->hashlist);
    free(hsmap->codelist);
    
    hsmap->hashlist = new_hash.hashlist;
    hsmap->codelist = new_hash.codelist;
    hsmap->count    = new_hash.count;
    hsmap->emptyidx = new_hash.emptyidx;
    hsmap->hashsize = new_hash.hashsize;
    hsmap->codesize = new_hash.codesize;

    return true;
}


void hsmap_del(hashmap* hsmap)
{
    if(hsmap == NULL) return;
    free(thiz->hashlist);
    free(this->codelist);
    free(hsmap);
    hsmap = NULL;
}

/* return 0 means no add
   return 1 means add success */
int hsmap_insert(hashmap* hsmap, void* key, void* value)
{
    if(!check_full(hsmap))
        return 0;
    if(internal_add(hsmap, key, value))
        hsmap->count++;
    return 1;
}

void* hsmap_find(hashmap* hsmap, void* key)
{
    assert(hsmap);
    u32 index = get_code(key, hsmap->codesize, hsmap->keysize);
    assert( index >= 0 );
    if( hsmap->codelist[index] == -1)
        return NULL;

    index = hsmap->codelist[index];
    while( index < hsmap->hashsize ) {
        assert(index < hsmap->hashsize
               && "hash find: index < hashsize");
        if(hsmap->hashlist[index].key == key) {
            return hsmap->hashlist[index].value;
        }
        if(hsmap->hashlist[index].next == -1)
            break;
        index = hsmap->hashlist[index].next;
    }
    return NULL;
}

hashmap* hsmap_new()
{
    hashmap* hsmap = (hashmap*)malloc(sizeof(hashmap));
    assert(hsmap);
    hsmap->hashsize = 10;
    hashtable_init(hsmap, hsmap->hashsize);
    return res;
}
