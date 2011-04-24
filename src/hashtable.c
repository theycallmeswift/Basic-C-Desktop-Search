/*
 * File: hashtable.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 8th, 2011
 * Date Modified: April 23rd, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashtable.h"


/* Define internal structs */

/* Pair
 *
 * Holds key/value pair, a pointer to the next pair in the list, and the 
 * current pair's calculated hash.
 */
 
struct Pair_ {
    void *key, *value;
    struct Pair_ *next;
    unsigned long hash;
};

typedef struct Pair_* Pair;

/* HashTable
 *
 * @param   maxLoadFactor   Maximum numItems/numBuckets ratio
 * @param   numItems        Total items in table
 * @param   numBuckets      Total number of buckets
 * @param   hash            Pointer to hash function
 * @param   comp            Pointer to comparison function
 * @param   buckets         Array of Pairs
 */

struct HashTable_ {
    float maxLoadFactor;
    int numItems;
    int numBuckets;
    hash_func hash;
    comp_func comp;
    Pair* buckets;
};

HashTable createHT(int size, float loadFactor, hash_func hash, comp_func comp)
{
    HashTable table;
    int i;
    
    /* Check Preconditions and return NULL if any fail. */
    if( size <= 0)
    {
        fprintf(stderr, "Error: Size must be > 0\n");
        return NULL;
    }
    
    if( loadFactor <= 0.0)
    {
        fprintf(stderr, "Error: Load Factor must be > 0\n");
        return NULL;
    }
    
    if( hash == NULL )
    {
        fprintf(stderr, "Error: Must pass in a valid Hash function.\n");
        return NULL;
    }
    
    if( comp == NULL )
    {
        fprintf(stderr, "Error: Must pass in a valid Comparison function.\n");
        return NULL;
    }
    
    /* Preconditions are all good, lets make a table! */
    
    table = (HashTable) malloc( sizeof(struct HashTable_) );
    assert(table != NULL);
    
    
    
    return table;    
}
