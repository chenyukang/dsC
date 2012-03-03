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
    thiz->emptyindex=0;
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
        empty = thiz->emptyindex;
        thiz->hashlist[empty].key = key;
        thiz->hashlist[empty].value = value;
        thiz->codelist[index] = thiz->emptyindex;
        thiz->emptyindex = thiz->hashlist[empty].next;
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
            empty = thiz->emptyindex;
            
            thiz->hashlist[empty].key=key;
            thiz->hashlist[empty].value=value;
            thiz->hashlist[index].next=empty;
            thiz->emptyindex = thiz->hashlist[empty].next;
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
static bool check_full(void* thz)
{
    HimaNDB_PtrHash* thiz = (HimaNDB_PtrHash*)thz;

    if(thiz->count < thiz->hashsize) return true;

    HimaNDB_PtrHash new_hash;
    int32_t n,step,cnt;

    step = (thiz->count)*HASH_STEP_RATE;
    step = step<HASH_STEP_MIN ? HASH_STEP_MIN :
          (step>HASH_STEP_MAX ? HASH_STEP_MAX:step);

    new_hash.hashsize = thiz->hashsize + step;
    hashtable_init(&new_hash, new_hash.hashsize);

    cnt = 0;
    for(n=0; n<thiz->hashsize; n++){
        if(thiz->hashlist[n].key == NULL) continue;

        if(!internal_add(&new_hash, thiz->hashlist[n].key,
                         thiz->hashlist[n].value)){
            Hashtable_destruct(&new_hash);
            return false;
        }
        new_hash.count++;
    }
    Hashtable_destruct(thiz);

    thiz->hashlist = new_hash.hashlist;
    thiz->codelist = new_hash.codelist;
    thiz->count    = new_hash.count;
    thiz->emptyindex = new_hash.emptyindex;
    thiz->hashsize = new_hash.hashsize;
    thiz->codesize = new_hash.codesize;

    return true;
}



static void Hashtable_destruct(void* thz)
{
    HimaNDB_PtrHash* thiz = thz;
    if(thiz->hashlist != NULL)
        HimaNDB_free(thiz->hashlist);
    if(thiz->codelist != NULL)
        HimaNDB_free(thiz->codelist);
}

static void Hashtable_insert(HimaNDB_PtrHash* thz, void* key, void* value)
{
    HimaNDB_PtrHash* thiz = thz;
    if(!check_full(thiz)) return;

    if(internal_add(thiz, key, value))
        thiz->count++;
}

/* key --> HimaNDB_Vector */
/* return : a vector list of ptrs , assosciated by key */
static HimaNDB_Vector* Hashtable_find(HimaNDB_PtrHash* thz, void* key)
{
    HimaNDB_PtrHash* thiz = thz;
    assert(thiz!=0);
    HimaNDB_Vector* res = HimaNDB_createVector(0);

    int32_t index;
    index = get_code(key, thiz->codesize, thiz->keysize);
    assert(index>=0);
    
    if(thiz->codelist[index] == -1)
        return res;

    index = thiz->codelist[index];
    while( index < thiz->hashsize )
    {
        assert(index < thiz->hashsize && "hash find: index < hashsize");
        if(thiz->hashlist[index].key == key)//add to hash
        {
            //acording to C99: void* and intptr_t can be convert vice and versa.
            HimaNDB_Int* value = HimaNDB_createInt((intptr_t)(thiz->hashlist[index].value));
            res->methods->pushBack(res, value);
            HimaNDB_xfreeRef(value);
        }
        if(thiz->hashlist[index].next == -1)
            break;
        index = thiz->hashlist[index].next;
    }
    return res;
}
            
                   


HimaNDB_PtrHash* HimaNDB_createPtrHash()
{
    HimaNDB_PtrHash* res = (HimaNDB_PtrHash*)HimaNDB_malloc(sizeof(HimaNDB_PtrHash));
    res->methods = &HimaNDB_Type_ptrHash;
    res->hashsize = 10;
    res->refCnt = 1;
    hashtable_init(res, res->hashsize);
    return res;
}

extern const HimaNDB_Type_Object HimaNDB_Type_object;

const HimaNDB_Type_PtrHash HimaNDB_Type_ptrHash = {
    "PtrHash",
    &HimaNDB_Type_object,
    Hashtable_destruct,
    NULL, /* hash */
    NULL, /* equals */
    Hashtable_insert,
    Hashtable_find,
};

