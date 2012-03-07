/*
  Hash table library using separate chaining
*/
#include "jsw_hlib.h"

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

typedef struct _hash {
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
}hs_table;

static hs_node*
new_node(void* key, void* item, hs_node* next)
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
hstab_new(size_t size, hash_f hash, cmp_f cmp,
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

  htab->size = 0;
  htab->capacity = size;
  htab->curri = 0;
  htab->currl = NULL;
  htab->hash = hash;
  htab->cmp = cmp;
  htab->keydup = keydup;
  htab->valdup = valdup;
  htab->keyrel = keyrel;
  htab->valrel = valrel;
  return htab;
}

/* Release all memory used by the hash table */
void
hs_delete(hs_table* hstab)
{
    size_t i;
    hs_node *prev, *it;
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
        return it->item;
    }
  }
  return NULL;
}

/*
  Insert an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int jsw_hinsert (hs_table* hstab, void* key, void* val)
{
  unsigned h = hstab->hash ( key ) % hstab->capacity;
  hs_node* new_node;
  void* val;
  
  if( ( val = hs_find( hstab, key )) != NULL) {
      hstab->valrel(val);
      val = hstab->valdup(val);
      return 1;
  }
  /* Attempt to create a new item */
  dupkey = hstab->keydup ( key );
  dupval = hstab->valdup ( val );

  new_node = new_node ( dupkey, dupval, NULL );

  if ( new_item == NULL )
    return 0;

  /* Create a chain if the bucket is empty */
  if ( hstab->table[h] == NULL ) {
    hstab->table[h] = new_chain();
    if ( hstab->table[h] == NULL ) {
      hstab->keyrel ( new_node->key );
      hstab->valrel ( new_node->val );
      free ( new_node );
      return 0;
    }
  }

  /* Insert at the front of the chain */
  new_node->next = hstab->table[h]->first;
  hstab->table[h]->first = new_node;

  ++hstab->table[h]->size;
  ++hstab->size;
  return 1;
}

/*
  Remove an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int jsw_herase ( jsw_hash_t *htab, void *key )
{
  unsigned h = htab->hash ( key ) % htab->capacity;
  jsw_node_t *save, *it;

  if ( htab->table[h] == NULL )
    return 0;

  it = htab->table[h]->first;

  /* Remove the first node in the chain? */
  if ( htab->cmp ( key, it->key ) == 0 ) {
    htab->table[h]->first = it->next;

    /* Release the node's memory */
    htab->keyrel ( it->key );
    htab->itemrel ( it->item );
    free ( it );

    /* Remove the chain if it's empty */
    if ( htab->table[h]->first == NULL ) {
      free ( htab->table[h] );
      htab->table[h] = NULL;
    }
    else
      --htab->table[h]->size;
  }
  else {
    /* Search for the node */
    while ( it->next != NULL ) {
      if ( htab->cmp ( key, it->next->key ) == 0 )
        break;

      it = it->next;
    }

    /* Not found? */
    if ( it->next == NULL )
      return 0;

    save = it->next;
    it->next = it->next->next;

    /* Release the node's memory */
    htab->keyrel ( save->key );
    htab->itemrel ( save->item );
    free ( save );

    --htab->table[h]->size;
  }

  /* Erasure invalidates traversal markers */
  jsw_hreset ( htab );

  --htab->size;

  return 1;
}

/*
  Grow or shrink the table, this is a slow operation
  
  Returns: non-zero for success, zero for failure
*/
int jsw_hresize ( jsw_hash_t *htab, size_t new_size )
{
  jsw_hash_t *new_htab;
  jsw_node_t *it;
  size_t i;

  /* Build a new hash table, then assign it to the old one */
  new_htab = jsw_hnew ( new_size, htab->hash, htab->cmp,
    htab->keydup, htab->itemdup, htab->keyrel, htab->itemrel );

  if ( new_htab == NULL )
    return 0;

  for ( i = 0; i < htab->capacity; i++ ) {
    if ( htab->table[i] == NULL )
      continue;

    for ( it = htab->table[i]->first; it != NULL; it = it->next )
      jsw_hinsert ( new_htab, it->key, it->item );
  }

  /* A hash table holds copies, so release the old table */
  jsw_hdelete ( htab );
  htab = new_htab;

  return 1;
}

/* Reset the traversal markers to the beginning */
void jsw_hreset ( jsw_hash_t *htab )
{
  size_t i;

  htab->curri = 0;
  htab->currl = NULL;

  /* Find the first non-empty bucket */
  for ( i = 0; i < htab->capacity; i++ ) {
    if ( htab->table[i] != NULL )
      break;
  }

  htab->curri = i;

  /* Set the link marker if the table was not empty */
  if ( i != htab->capacity )
    htab->currl = htab->table[i]->first;
}

/* Traverse forward by one key */
int jsw_hnext ( jsw_hash_t *htab )
{
  if ( htab->currl != NULL ) {
    htab->currl = htab->currl->next;

    /* At the end of the chain? */
    if ( htab->currl == NULL ) {
      /* Find the next chain */
      while ( ++htab->curri < htab->capacity ) {
        if ( htab->table[htab->curri] != NULL )
          break;
      }

      /* No more chains? */
      if ( htab->curri == htab->capacity )
        return 0;

      htab->currl = htab->table[htab->curri]->first;
    }
  }

  return 1;
}

/* Get the current key */
const void *jsw_hkey ( jsw_hash_t *htab )
{
  return htab->currl != NULL ? htab->currl->key : NULL;
}

/* Get the current item */
void *jsw_hitem ( jsw_hash_t *htab )
{
  return htab->currl != NULL ? htab->currl->item : NULL;
}

/* Current number of items in the table */
size_t jsw_hsize ( jsw_hash_t *htab )
{
  return htab->size;
}

/* Total allowable number of items without resizing */
size_t jsw_hcapacity ( jsw_hash_t *htab )
{
  return htab->capacity;
}

/* Get statistics for the hash table */
jsw_hstat_t *jsw_hstat ( jsw_hash_t *htab )
{
  jsw_hstat_t *stat;
  double sum = 0, used = 0;
  size_t i;

  /* No stats for an empty table */
  if ( htab->size == 0 )
    return NULL;

  stat = (jsw_hstat_t *)malloc ( sizeof *stat );

  if ( stat == NULL )
    return NULL;

  stat->lchain = 0;
  stat->schain = (size_t)-1;

  for ( i = 0; i < htab->capacity; i++ ) {
    if ( htab->table[i] != NULL ) {
      sum += htab->table[i]->size;

      ++used; /* Non-empty buckets */

      if ( htab->table[i]->size > stat->lchain )
        stat->lchain = htab->table[i]->size;

      if ( htab->table[i]->size < stat->schain )
        stat->schain = htab->table[i]->size;
    }
  }

  stat->load = used / htab->capacity;
  stat->achain = sum / used;

  return stat;
}
