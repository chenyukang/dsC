
/*******************************************************************************
 *
 *      SKIPLIST_H
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-02-24 14:21:26
 *
 *      COPYRIGHT (C) 2011, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/

#if !defined(SKIPLIST_H)
#define SKIPLIST_H

#ifdef __cplusplus
using std::size_t;

extern "C" {
#else
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#endif

    
    typedef int   (*cmp_func)  (const void* l, const void* r);
    typedef void  (*show_func) (const void* value);
    typedef void  (*keydup_f)  (const void* key);
    typedef void* (*itemdup_f) (const void* item);
    typedef void  (*itemrel_f) (const void* item);

    typedef struct _skip_node{
        void* value;
        struct _skip_node** forward;
    }skipnode;

    typedef struct _skip_list{
        skipnode* header;
        cmp_func  cfunc;
        show_func sfunc;
        int lev;
        int maxlev;
        int inited;
        int vsize;
    }skiplist;

    skiplist* sl_new(int maxlev, int vsize, cmp_func cfunc, show_func sfunc);
    void*     sl_search(skiplist* list, void* item);
    int       sl_insert(skiplist* list, void* item);
    int       sl_delete(skiplist* list, void* item);
    void      sl_print (skiplist* list);

#ifdef __cplusplus
}
#endif

#endif

