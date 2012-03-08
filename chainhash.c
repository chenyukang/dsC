/*
  Hash table library using separate chaining
*/
#include "chainhash.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#ifdef __cplusplus
#include <cstdlib>

using std::malloc;
using std::free;
#else
#include <stdlib.h>
#endif

typedef struct _node {
  void            *key;  /* Key used for searching */
  void            *val; /* Actual content of a node */
  struct _node *next; /* Next link in the chain */
} hs_node;

typedef struct _head {
  hs_node*    first;     /* First link in the chain */
  size_t      size;      /* Length of the chain */
} hs_head;

struct _hash_table {
  hs_head     **table;    /* Dynamic chained hash table */
  size_t       size;     /* Current item count */
  size_t       capacity; /* Current table size */
  size_t       curri;    /* Current index for traversal */
  hs_node     *currl;    /* Current link for traversal */
  hash_f       hash;     /* User defined key hash function */
  cmp_f        cmp;      /* User defined key comparison function */
  keydup_f     keydup;   /* User defined key copy function */
  valdup_f     valdup;  /* User defined val copy function */
  keyrel_f     keyrel;   /* User defined key delete function */
  valrel_f     valrel;  /* User defined val delete function */
};

static hs_node*
new_node(void* key, void* val, hs_node* next)
{
    hs_node* node = (hs_node*)malloc(sizeof(hs_node));
    assert(node);
    node->key = key;
    node->val = val;
    node->next = next;
    return node;
}

static hs_head*
new_chain()
{
    hs_head* chain = (hs_head*)malloc(sizeof(hs_head));
    assert(chain);
    chain->first = NULL;
    chain->size  = 0;
    return chain;
}

/*
  Create a new hash table with a capacity of size, and
  user defined functions for handling keys and items.

  Returns: An empty hash table, or NULL on failure.
*/

hs_table*
hs_new(size_t size, hash_f hash, cmp_f cmp,
          keydup_f keydup, valdup_f valdup,
          keyrel_f keyrel, valrel_f valrel )
{
    size_t i;
    hs_table* hstab = (hs_table*)malloc(sizeof(hs_table));

    if( hstab == NULL )
        return NULL;
    hstab->table = (hs_head**)malloc(sizeof(hs_head*) * size);
    assert(hstab->table);
    memset(hstab->table, 0, sizeof(hstab->table[0]) * size);

  hstab->size = 0;
  hstab->capacity = size;
  hstab->curri = 0;
  hstab->currl = NULL;
  hstab->hash = hash;
  hstab->cmp = cmp;
  hstab->keydup = keydup;
  hstab->valdup = valdup;
  hstab->keyrel = keyrel;
  hstab->valrel = valrel;
  return hstab;
}

/* Release all memory used by the hash table */
void
hs_delete(hs_table* hstab)
{
    size_t i;
    hs_node *next, *it;
    for(i=0; i<hstab->capacity; i++) {
        if( hstab->table[i] == NULL )
            continue;
        it = hstab->table[i]->first;
        while( it ) {
            next = it->next;
            hstab->keyrel(it->key);
            hstab->valrel(it->val);
            free(it);
            it = next;
        }
        free(hstab->table[i]);
    }
    free( hstab->table );
    free(hstab);
}
/*
  Find an item with the selected key

  Returns: The item, or NULL if not found
*/
void*
hs_find (hs_table* hstab, void *key )
{
  unsigned h = hstab->hash(key) % hstab->capacity;
  if ( hstab->table[h] != NULL ) {
    hs_node *it = hstab->table[h]->first;
    for ( ; it != NULL; it = it->next ) {
      if ( hstab->cmp ( key, it->key ) == 0 )
        return it->val;
    }
  }
  return NULL;
}

/*
  Insert an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int hs_insert (hs_table* hstab, void* key, void* val)
{
  unsigned h = hstab->hash ( key ) % hstab->capacity;
  hs_node* new;
  void* dupkey;
  void* dupval;
  void* prev_val;
  
  if( ( prev_val = hs_find( hstab, key )) != NULL) {
      hstab->valrel(prev_val);
      prev_val = hstab->valdup(val);
      return 1;
  }
  /* Attempt to create a new item */
  dupkey = hstab->keydup ( key );
  dupval = hstab->valdup ( val );

  new = new_node ( dupkey, dupval, NULL );

  if ( new == NULL )
    return 0;

  /* Create a chain if the bucket is empty */
  if ( hstab->table[h] == NULL ) {
    hstab->table[h] = new_chain();
    if ( hstab->table[h] == NULL ) {
      hstab->keyrel ( new->key );
      hstab->valrel ( new->val );
      free ( new );
      return 0;
    }
  }

  /* Insert at the front of the chain */
  new->next = hstab->table[h]->first;
  hstab->table[h]->first = new;

  ++hstab->table[h]->size;
  ++hstab->size;
  return 1;
}

/*
  Remove an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int hs_erase (hs_table* hstab, void *key )
{
  unsigned h = hstab->hash ( key ) % hstab->capacity;
  hs_node *save, *it;

  if ( hstab->table[h] == NULL )
    return 0;

  it = hstab->table[h]->first;

  /* Remove the first node in the chain? */
  if ( hstab->cmp ( key, it->key ) == 0 ) {
    hstab->table[h]->first = it->next;
    /* Release the node's memory */
    hstab->keyrel ( it->key );
    hstab->valrel ( it->val );
    free ( it );
    /* Remove the chain if it's empty */
    if ( hstab->table[h]->first == NULL ) {
      free ( hstab->table[h] );
      hstab->table[h] = NULL;
    }
    else
      --hstab->table[h]->size;
  }
  else {
    /* Search for the node */
    while ( it->next != NULL ) {
      if ( hstab->cmp ( key, it->next->key ) == 0 )
        break;

      it = it->next;
    }

    /* Not found? */
    if ( it->next == NULL )
      return 0;

    save = it->next;
    it->next = it->next->next;

    /* Release the node's memory */
    hstab->keyrel ( save->key );
    hstab->valrel ( save->val );
    free ( save );

    --hstab->table[h]->size;
  }

  /* Erasure invalidates traversal markers */
  hs_reset ( hstab );

  --hstab->size;

  return 1;
}

/*
  Grow or shrink the table, this is a slow operation
  
  Returns: non-zero for success, zero for failure
*/
int hs_resize (hs_table* hstab, size_t new_size )
{
  hs_table *new_hstab;
  hs_node *it;
  size_t i;

  /* Build a new hash table, then assign it to the old one */
  new_hstab = hs_new ( new_size, hstab->hash, hstab->cmp,
    hstab->keydup, hstab->valdup, hstab->keyrel, hstab->valrel );

  if ( new_hstab == NULL )
    return 0;

  for ( i = 0; i < hstab->capacity; i++ ) {
    if ( hstab->table[i] == NULL )
      continue;

    for ( it = hstab->table[i]->first; it != NULL; it = it->next )
      hs_insert ( new_hstab, it->key, it->val );
  }

  /* A hash table holds copies, so release the old table */
  hs_delete ( hstab );
  hstab = new_hstab;

  return 1;
}

/* Reset the traversal markers to the beginning */
void hs_reset ( hs_table* hstab )
{
  size_t i;
  hstab->curri = 0;
  hstab->currl = NULL;
  /* Find the first non-empty bucket */
  for ( i = 0; i < hstab->capacity; i++ ) {
    if ( hstab->table[i] != NULL )
      break;
  }
  hstab->curri = i;
  /* Set the link marker if the table was not empty */
  if ( i != hstab->capacity )
    hstab->currl = hstab->table[i]->first;
}

/* Traverse forward by one key */
int hs_next ( hs_table* hstab )
{
  if ( hstab->currl != NULL ) {
    hstab->currl = hstab->currl->next;

    /* At the end of the chain? */
    if ( hstab->currl == NULL ) {
      /* Find the next chain */
      while ( ++hstab->curri < hstab->capacity ) {
        if ( hstab->table[hstab->curri] != NULL )
          break;
      }

      /* No more chains? */
      if ( hstab->curri == hstab->capacity )
        return 0;

      hstab->currl = hstab->table[hstab->curri]->first;
    }
  }

  return 1;
}

/* Get the current key */
const void* hs_key ( hs_table *hstab )
{
  return hstab->currl != NULL ? hstab->currl->key : NULL;
}

/* Get the current item */
void *hs_item ( hs_table *hstab )
{
  return hstab->currl != NULL ? hstab->currl->val : NULL;
}

/* Current number of items in the table */
size_t hs_size (hs_table* hstab )
{
  return hstab->size;
}

/* Total allowable number of items without resizing */
size_t hs_capacity (hs_table* hstab )
{
  return hstab->capacity;
}

#if 0
/* Get statistics for the hash table */
hs_stat_t *hs_stat ( hs_table *hstab )
{
  hs_stat_t *stat;
  double sum = 0, used = 0;
  size_t i;

  /* No stats for an empty table */
  if ( hstab->size == 0 )
    return NULL;

  stat = (hs_stat_t *)malloc ( sizeof *stat );

  if ( stat == NULL )
    return NULL;

  stat->lchain = 0;
  stat->schain = (size_t)-1;

  for ( i = 0; i < hstab->capacity; i++ ) {
    if ( hstab->table[i] != NULL ) {
      sum += hstab->table[i]->size;

      ++used; /* Non-empty buckets */

      if ( hstab->table[i]->size > stat->lchain )
        stat->lchain = hstab->table[i]->size;

      if ( hstab->table[i]->size < stat->schain )
        stat->schain = hstab->table[i]->size;
    }
  }

  stat->load = used / hstab->capacity;
  stat->achain = sum / used;

  return stat;
}
#endif 
