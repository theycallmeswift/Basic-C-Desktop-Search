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
    void *key, *val;
    struct Pair_ *next;
    unsigned long hash;
};

typedef struct Pair_* Pair;

/* HashTable
 *
 * @param   maxLoadFactor       Maximum numItems/numBuckets ratio
 * @param   numItems            Total items in table
 * @param   currPrime           Int that tracks where we are in the list of primes
 * @param   numBuckets          Total number of buckets
 * @param   hash                Pointer to hash function
 * @param   comp                Pointer to comparison function
 * @param   key_destroy_func    Pointer to a function that destroys keys
 * @param   val_destroy_func    Pointer to a function that destroys values
 * @param   buckets             Array of Pairs
 */

struct HashTable_ {
    float maxLoadFactor;
    int numItems;
    int currPrime;
    int numBuckets;
    hash_func hash;
    comp_func comp;
    destroy_key key_destroy_func;
    destroy_val val_destroy_func;
    print_func print;
    Pair* buckets;
};

/* Array of primes for table size. 26 total values */

static const unsigned int primes[] = {
    53,
    97, 
    193, 
    389,
    769,
    1543, 
    3079, 
    6151, 
    12289, 
    24593, 
    
    49157, 
    98317,
    196613, 
    393241, 
    786433, 
    1572869,
    3145739, 
    6291469, 
    12582917, 
    25165843,
    
    50331653, 
    100663319, 
    201326611, 
    402653189,
    805306457, 
    1610612741
};

void toStringHT(HashTable table)
{
    int slot;
    Pair curr;
    
    if(table == NULL)
    {
        fprintf(stderr, "Table cannot be NULL.\n");
        return;
    }
    
    if(table->print == NULL)
    {
        fprintf(stderr, "No print print function defined.\n");
        return;
    }
    
    printf("Total Slots: %d\n", table->numBuckets);
    printf("Total Items: %d\n", table->numItems);
    printf("Current LF: %f\n", (float) table->numItems / table->numBuckets);
    
    for(slot = 0; slot < table->numBuckets; slot++)
    {
        printf("[%d]: ", slot);
        curr = table->buckets[slot];
        
        while(curr != NULL)
        {
            table->print(curr->key, curr->val);
            curr = curr->next;
        }
        printf("NULL\n");
    }
    return;
}

HashTable createHT(hash_func hash, comp_func comp, destroy_key key, destroy_val val, print_func print)
{
    HashTable table;
    int i;
    
    /* Check Preconditions and return NULL if any fail. */
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
    
    /* Get the first table size */
    table->currPrime = 0;
    table->numBuckets = primes[table->currPrime];
    
    table->buckets = (Pair*) malloc( sizeof( Pair ) * table->numBuckets );
    assert(table->buckets != NULL);
    
    table->val_destroy_func = val;
    table->key_destroy_func = key;
    table->print = print;
    table->comp = comp;
    table->hash = hash;
    table->numItems = 0;
    table->maxLoadFactor = LF;
    
    /* By default, set all the buckets to NULL so we know that they're empty down the road. */
    for( i = 0; i < table->numBuckets; i++ )
    {
        table->buckets[i] = NULL;
    }
    
    
    return table; 
}

void destroyHT(HashTable table)
{
    int i;
    Pair curr, next;
    
    if(table != NULL)
    {
        for(i = 0; i < table->numBuckets; i++)
        {
            curr = table->buckets[i];
            while(curr != NULL)
            {
                next = curr->next;
                /* Check if the destroy functions exist and call them if they do... */
                if(table->key_destroy_func)
                {
                    table->key_destroy_func(curr->key);
                }
                
                if(table->val_destroy_func)
                {
                    table->val_destroy_func(curr->val);
                }
                
                /* Okay, any data we are responsible for is gone. Lets free the Pair struct */
                free(curr);
                
                /* ... And move on ... */
                curr = next;
            }
        }
        
        free(table->buckets);
        free(table);
    }
    return;
}

/* Rehash
 *
 * Increase the number of buckets and redistribute the keys accordingly.
 *
 * @param   table
 *
 * @return  void
 */

void rehash(HashTable table)
{
    Pair* newbuckets;
    Pair curr, next;
    int i, slot, oldSlots;
    
    if(table->currPrime < 26)
    {
        oldSlots = table->numBuckets;
        
        table->currPrime++;
        table->numBuckets = primes[table->currPrime];
        
        newbuckets = (Pair*) malloc( sizeof( Pair ) * table->numBuckets );
        assert(newbuckets != NULL);
        
        for(i = 0; i < table->numBuckets; i++)
        {
            newbuckets[i] = NULL;
        }
        
        for(i = 0; i < oldSlots; i++)
        {
            curr = table->buckets[i];
            
            while(curr != NULL)
            {
                next = curr->next;
                
                slot = (int) (curr->hash % table->numBuckets);
                assert(slot >= 0 && slot < table->numBuckets);
                
                curr->next = newbuckets[slot];
                newbuckets[slot] = curr;
                
                curr = next;
            }
        }
        
        free(table->buckets);
        table->buckets = newbuckets;
    }
    
    return;
}

int insertHT(HashTable table, void *key, void *val)
{
    Pair kvpair;
    int slot;
    
    /* Make sure at least the table isn't NULL.  Technically we could pass in a NULL/NULL key/value pair */
    if(table == NULL)
    {
        return 0;
    }
    
    /* Check for rehash */
    if(((float) table->numItems + 1) / table->numBuckets > LF)
    {
        printf("Rehashing...\n");
        rehash(table);
    }
    
    /* Ok, lets make a key/value pair... */
    kvpair = (Pair) malloc( sizeof(struct Pair_) );
    assert(kvpair != NULL);
    
    kvpair->hash = table->hash(key);    
    kvpair->key = key;
    kvpair->val = val;
    
    /* Get the destination slot */
    slot = (int)(kvpair->hash % table->numBuckets);
    assert( slot >= 0 && slot < table->numBuckets); 
    
    /* Now insert */
    kvpair->next = table->buckets[slot];
    table->buckets[slot] = kvpair;
    table->numItems++;
    
    return 1;
}

void *searchHT(HashTable table, void *key)
{
    int slot;
    Pair curr;
    unsigned long hashVal;
    
    if(table == NULL)
    {
        fprintf(stderr, "Table cannot be NULL.\n");
        return NULL;
    }
    
    if(key == NULL)
    {
        fprintf(stderr, "Key cannot be NULL.\n");
        return NULL;
    }
    
    hashVal = table->hash(key);
    slot = (int) hashVal % table->numBuckets;
    curr = table->buckets[slot];
        
    while(curr != NULL)
    {
        if(table->comp(key, curr->key) == 0)
        {
            return curr->val;
        }
        curr = curr->next;
    }
    
    return NULL;
}

int removeHT(HashTable table, void *key)
{
    int slot;
    Pair curr, prev;
    unsigned long hashVal;
    
    if(table == NULL)
    {
        fprintf(stderr, "Table cannot be NULL.\n");
        return 0;
    }
    
    if(key == NULL)
    {
        fprintf(stderr, "Key cannot be NULL.\n");
        return 0;
    }
    
    hashVal = table->hash(key);
    slot = (int) hashVal % table->numBuckets;
    
    curr = table->buckets[slot];
    prev = NULL;
        
    while(curr != NULL)
    {
        if(table->comp(key, curr->key) == 0)
        {
            /* If the key is the first Pair on the list */
            if(prev == NULL)
            {
                table->buckets[slot] = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            
            /* Check if the destroy functions exist and call them if they do... */
            if(table->key_destroy_func)
            {
                table->key_destroy_func(curr->key);
            }
            
            if(table->val_destroy_func)
            {
                table->val_destroy_func(curr->val);
            }
            
            /* Okay, any data we are responsible for is gone. Lets free the Pair struct */
            free(curr);
            
            return 1;
            
        }
        prev = curr;
        curr = curr->next;
    }
    
    return 0;
}
