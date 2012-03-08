#ifndef JSW_HLIB
#define JSW_HLIB

/*
  Hash table library using separate chaining

*/
#ifdef __cplusplus
#include <cstddef>

using std::size_t;

extern "C" {
#else
#include <stddef.h>
#endif

typedef struct _hash_table hs_table;

/* Application specific hash function */
typedef unsigned (*hash_f) ( const void *key );

/* Application specific key comparison function */
typedef int      (*cmp_f) ( const void *a, const void *b );

/* Application specific key copying function */
typedef void*    (*keydup_f) ( const void *key );

/* Application specific data copying function */
typedef void*    (*valdup_f) ( const void *item );

/* Application specific key deletion function */
typedef void     (*keyrel_f) ( void *key );

/* Application specific data deletion function */
typedef void     (*valrel_f) ( void *item );

#if 0
typedef struct jsw_hstat {
  double load;            /* Table load factor: (M chains)/(table size) */
  double achain;          /* Average chain length */
  size_t lchain;          /* Longest chain */
  size_t schain;          /* Shortest non-empty chain */
} jsw_hstat_t;
#endif
/*
  Create a new hash table with a capacity of size, and
  user defined functions for handling keys and items.

  Returns: An empty hash table, or NULL on failure.
*/
hs_table* hs_new ( size_t size, hash_f hash, cmp_f cmp,
                       keydup_f keydup, valdup_f valdup,
                       keyrel_f keyrel, valrel_f valrel );

/* Release all memory used by the hash table */
void         hs_delete ( hs_table *hstab );

/*
  Find an item with the selected key

  Returns: The item, or NULL if not found
*/
void        *hs_find ( hs_table *hstab, void *key );

/*
  Insert an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int          hs_insert ( hs_table *hstab, void *key, void *item );

/*
  Remove an item with the selected key

  Returns: non-zero for success, zero for failure
*/
int          hs_erase ( hs_table *hstab, void *key );

/*
  Grow or shrink the table, this is a slow operation
  
  Returns: non-zero for success, zero for failure
*/
int          hs_resize ( hs_table *hstab, size_t new_size );

/* Reset the traversal markers to the beginning */
void         hs_reset ( hs_table *hstab );

/* Traverse forward by one key */
int          hs_next ( hs_table *hstab );

/* Get the current key */
const void  *hs_key ( hs_table *hstab );

/* Get the current item */
void        *hs_item ( hs_table *hstab );

/* Current number of items in the table */
size_t       hs_size ( hs_table *hstab );

/* Total allowable number of items without resizing */
size_t       hs_capacity ( hs_table *hstab );

/* Get statistics for the hash table */
//hs_stat_t *jsw_hstat ( hs_table *hstab );

#ifdef __cplusplus
}
#endif

#endif
