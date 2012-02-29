
/*******************************************************************************
 *
 *      skiplist.c
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-02-24 14:31:24
 *
 *      COPYRIGHT (C) 2011, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/
#ifdef __cplusplus
#include <cstdlib>

using std::malloc;
using std::free;
#else
#include <assert.h>
#include <stdlib.h>
#include "skiplist.h"

static int MINVALUE = 0x80000000;
static int MAXVALUE = 0x7FFFFFFF;
#define P 0.8

static
float frand() {
    return (float) rand()/RAND_MAX;
}

static
random_lev(skiplist* skip_l)
{
    assert(skip_l);
    int lev = 0;
    if(skip_l->inited) {
        srand( (unsigned)time(NULL) );
        skip_l->inited = 1;
    }
    while(frand() < P && lev < skip_l->maxlev)
        lev++;
    return lev;
}


static
skipnode* make_node(int lev, void* value, int vsize)
{
    skipnode* node = (skipnode*)malloc(sizeof(skipnode*));
    assert(node);
    node->value = (void*)malloc(vsize);
    node->forward = (skipnode**)calloc(lev+1, sizeof(skipnode*));
    memcpy(node->value, value, vsize); //copy value
    return node;
}

skiplist* sl_new(int maxlev, int vsize, cmp_func cfunc, show_func sfunc)
{
    assert(maxlev);
    skiplist* sl = (skiplist*)malloc(sizeof(skiplist));
    assert(sl);

    sl->maxlev = maxlev;
    sl->lev    = 0;
    sl->inited = 0;
    sl->vsize  = vsize;
    sl->cfunc  = cfunc;
    sl->sfunc  = sfunc;
    sl->header = make_node(maxlev, &MINVALUE, sl->vsize);
    return sl;
}

void sl_print(skiplist* sl)
{
    assert(sl);
    skipnode* node = sl->header->forward[0];
    printf("[lev: %d]", sl->lev);
    printf("[ ");
    while(node != NULL) {
        sl->sfunc(node->value);
        node = node->forward[0];
        printf(" ");
    }
    printf("]\n");
}

void* sl_search(skiplist* sl, void* value)
{
    assert(sl);
    int i;
    skipnode* node = sl->header;
    for(i=sl->lev; i>=0; i--) {
        while(node->forward[i] != NULL &&
              sl->cfunc(node->forward[i]->value, value) < 0)
            node = node->forward[i];
    }
    node = node->forward[0];
    if(node != NULL &&
       sl->cfunc(node->value, value) == 0)
        return node;
    return NULL;
}


#define FIND                                                   \
    for(i=sl->lev; i>=0; i--){                                 \
        while(node->forward[i] != NULL &&                      \
              sl->cfunc(node->forward[i]->value, value) < 0) { \
            node = node->forward[i];                           \
        }                                                      \
        update[i] = node;                                      \
    }                                                          \


int sl_insert(skiplist* sl, void* value)
{
    assert(sl);
    int i,lev;
    skipnode* node = sl->header;
    skipnode* update[sl->maxlev + 1];
    memset(update, 0, sl->maxlev + 1);
    
    //find
    FIND;
    node = node->forward[0];

    if(node != NULL &&
       sl->cfunc(value, node->value) == 0)
        return 0; //exits
    
    //add new one
    lev = random_lev(sl);
    if(lev > sl->lev) {
        for(i=sl->lev + 1; i<=lev; i++)
            update[i] = sl->header;
        sl->lev = lev;
    }
    node = make_node(lev, value, sl->vsize);
    assert(node);
    for(i=0; i<=lev; i++){
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    return 1;
}

int sl_delete(skiplist* sl, void* value)
{
    assert(sl);
    int i,lev;
    skipnode* node = sl->header;
    skipnode* update[sl->maxlev + 1];
    memset(update, 0, sl->maxlev + 1);

    FIND;
    node = node->forward[0];
    if(node == NULL ||
       sl->cfunc(value, node->value) != 0) //not found
        return 0;
    assert(node && sl->cfunc(value, node->value) == 0);
    for(i=0; i<=sl->lev; i++) {
        if(update[i]->forward[i] != node) break;
        update[i]->forward[i] = node->forward[i];
    }

    free(node->value);
    free(node->forward);
    free(node);
    while(sl->lev > 0 && sl->header->forward[sl->lev] == NULL)
        sl->lev--;
    
    return 1;
}


#endif
