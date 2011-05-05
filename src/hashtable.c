/*
 * File: hashtable.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 8th, 2011
 * Date Modified: May 4th, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
 * @param   version
 * @param   hash                Pointer to hash function
 * @param   comp                Pointer to comparison function (compares keys)
 * @param   key_destroy_func    Pointer to a function that destroys keys
 * @param   val_destroy_func    Pointer to a function that destroys values
 * @param   buckets             Array of Pairs
 */

struct HashTable_ {
    float maxLoadFactor;
    int numItems;
    int currPrime;
    int numBuckets;
    int version;
    hash_func hash;
    comp_func comp;
    destroy_key key_destroy_func;
    destroy_val val_destroy_func;
    print_func print;
    Pair* buckets;
};

/* HTIterator
 *
 * @param   table       HashTable to iterate over
 * @param   curr        pointer to current object
 * @param   row         current row
 * @param   version     current version
 */

struct HTIterator_ {
    HashTable table;
    Pair curr;
    int row;
    int version;
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

/* toString
 *
 * Prints out the hashtable and it's contents.
 *
 * @param   table       HashTable to print
 *
 * @return  void
 */
 

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

/* createHT
 *
 * Function to create a new HashTable. Asserts that size and LoadFactor are
 * both greater than one and the hash and comp do not point to NULL.  The
 * destroy_key and destroy_val functions are both optional, but if you do not
 * provide them, it becomes the users responsibility to iterate over the table
 * and free any alllocated space.
 *
 * @param   hash        function for hashing key/value pairs
 * @param   comp        function for comparing key/value pairs
 * @param   key         function for destroying the key of key/value pairs
 * @param   val         function for destroying the value of key/value pairs
 * @param   print       function for printing key/value pairs
 *
 * @return  success     newly created HashTable struct
 * @return  failure     NULL
 */ 

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
    table->version = 0;
    table->maxLoadFactor = LF;
    
    /* By default, set all the buckets to NULL so we know that they're empty down the road. */
    for( i = 0; i < table->numBuckets; i++ )
    {
        table->buckets[i] = NULL;
    }
    
    return table; 
}

/* destroyHT
 *
 * Function that destroys a HashTable and frees all allocated memory. If the 
 * passed in value is NULL, no action is performed. This function frees ALL 
 * data in the remaining nodes if there are destroy_key or destroy_val functions
 * defined.  Remember to set the variable pointers to NULL after they have been freed.
 *
 * @param   table       HashTable to be freed.
 *
 * @return  void
 */

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
                    curr->key = NULL;
                }
                
                if(table->val_destroy_func)
                {
                    table->val_destroy_func(curr->val);
                    curr->val = NULL;
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
    
    table->version++;
    
    return;
}

/* insertHT
 *
 * Function that inserts a key/value pair into the passed in HashTable.  Both the 
 * key and the value should be cast to void * to make them generic.  All the hashing
 * will be done by the hash function supplied by the user when the table was created.
 * Returns a 1 on success, 0 on failure. This function does NOT check if the key already
 * exists. 
 *
 * @param   table       HashTable to insert key/value Pair into
 * @param   key         key to hash
 * @param   val         value of key
 *
 * @return  success     1
 * @return  failure     0
 */

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
    
    table->version++;
    
    return 1;
}

/* searchHT
 *
 * Searches the passed in HashTable for a key and returns a pointer to the value.
 * If the key is not found or the search is unsuccessful, the function returns 
 * NULL. This only returns the first found insterest for the designated key.
 *
 * NOTE: If a single key can have multiple values, it is your responsibility to
 * compare the value being returned to the value you are searching for.
 *
 * @param   table       HashTable to search
 * @param   key         key in a key/value pair
 *
 * @return  success     void * (pointer)
 * @return  failure     NULL
 */

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
    slot = (int) (hashVal % table->numBuckets);
    
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

/* removeHT
 *
 * Searches the hashtable for the first occurence of a key and removes it. If
 * key destroy or value destroy functions have been defined, they will be called
 * on the key/value pair. Otherwise, it is the user's responsibility to free any
 * allocated data. Returns 1 on success, 0 on failure.
 *
 * @param   table       HashTable
 * @param   key         key in key/value pair
 *
 * @return  success     1
 * @return  failure     0
 */

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
    slot = (int) (hashVal % table->numBuckets);
    
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
            
            table->version++;
            
            return 1;
            
        }
        prev = curr;
        curr = curr->next;
    }
    
    return 0;
}

/* createIterHT
 *
 * Creates a new hashtable iterator for the current version
 * of the table.  If the table changes (rehash, insert, 
 * remove), the Iterator will become invalid and you will
 * need to destroy it and create a new one.
 *
 * @param       table       hashtable to make iterator for
 *
 * @result      success     new HTIterator
 * @result      failure     NULL
 */
 
HTIterator createIterHT(HashTable table)
{
    HTIterator iter;
    
    if(table == NULL)
    {
        fprintf(stderr, "Error: Cannot create iterator for NULL table.\n");
        return NULL;
    }
    
    iter = (HTIterator) malloc( sizeof(struct HTIterator_) );
    if(iter == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for iterator.\n");
        return NULL;
    }
    
    iter->table = table;
    iter->curr = NULL;
    iter->version = table->version;
    iter->row = -1;
    
    return iter;
}

/* destroyIterHT
 *
 * Destroys an existing HTIterator object. If NULL is passed in,
 * nothing will happen.
 *
 * @param       iter        hashtable iterator to destroy
 *
 * @return      void
 */
 
void destroyIterHT(HTIterator iter)
{
    if(iter != NULL)
    {
        free(iter);
    }
    return;
}

/* HTNextItem
 *
 * Gets the next item in the hashtable from an HTIterator. 
 * This function inserts the key and value into addresses
 * you provide.
 *
 * @param       iter            HTIterator object
 * @param       key             pointer to key
 * @param       val             pointer to val
 *
 * @return      success         1
 * @return      end of list     0
 * @return      error           -1
 */

int HTNextItem(HTIterator iter, void** key, void** val)
{
    if(iter == NULL)
    {
        fprintf(stderr, "Error: Cannot iterate over NULL iterator.\n");
        return -1;
    }
    
    if(iter->version != iter->table->version)
    {
        fprintf(stderr, "Error: Iterator version and table version do not match.\n");
        return -1;
    }
    
    while(iter->row != (iter->table->numBuckets - 1))
    {
        if(iter->curr == NULL)
        {
            iter->row++;
            iter->curr = iter->table->buckets[iter->row];
        }
        else
        {
            iter->curr = iter->curr->next;
        }
        
        if(iter->curr != NULL)
        {
            *key = iter->curr->key;
            *val = iter->curr->val;
            return 1;
        }
    }
    
    return 0;
}


/* getNumBuckets
 *
 * Returns the current number of buckets.
 *
 * @param       table           hashtable object
 *
 * @return      int             number of buckets
 */

int getNumBuckets(HashTable table)
{
    return table->numBuckets;
}

/* hash
 *
 * Simple hashing function for strings.
 *
 * @param   obj         void* object to hash (string)
 *
 * @return  unsigned long hash
 */

unsigned long hash(void *obj)
{
	char *key;
	int h;
	
	key = (char *) obj;
	h=0;
	while(*key) h=33*h + *key++;
	return h;
}

/* compStrings
 *
 * Void* wrapper for strcmp. To be used with 
 * the HashTable's internal comparison function.
 *
 * @param   str1        1st string
 * @param   str2        2nd string
 *
 * @return  -1          str1 < str2
 * @return  0           str1 = str2
 * @return  1           str1 > str2
 */

int compStrings(void *str1, void *str2)
{
    return strcmp((char*)str1, (char*)str2);
}

/* destroyString
 *
 * Void * wrapper for freeing strings. To be used with
 * the HashTables's internal destroy function.
 *
 * @param   str         string to free
 *
 * @return  void
 */

void destroyString(void *str)
{
    if(str != NULL) free( (char*)str );
}


