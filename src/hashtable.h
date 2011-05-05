/*
 * File: hashtable.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 8th, 2011
 * Date Modified: May 4th, 2011
 */

#ifndef SWIFT_HASHTABLE_H_
#define SWIFT_HASHTABLE_H_

#define LF 0.65

/* Define our HashTable Object */
struct HashTable_;
typedef struct HashTable_* HashTable;

/* Define our HTIterator Object */
struct HTIterator_;
typedef struct HTIterator_* HTIterator;

/* Pointer to a Hashing Function */
typedef unsigned long (*hash_func)( void *key );

/* Pointer to a Comparison function that compares keys
 *
 * @case    obj1 < obj2    -1
 * @case    obj1 = obj2     0
 * @case    obj1 > obj2     1
 */
typedef int (*comp_func)( void* obj1, void* obj2 );

/* Pointer to a function that frees / destroys the key from a key/value Pair.
 * Remember to cast the object properly.
 * 
 * @param   key     void pointer to the object's key
 *
 * @return  void
 */

typedef void (*destroy_key)(void* key);

/* Pointer to a function that frees / destroys the value from a key/value Pair.
 * Remember to cast the object properly.
 * 
 * @param   val     void pointer to the object's value
 *
 * @return  void
 */

typedef void (*destroy_val)(void* val);

/* Pointer to a function that prints key/value pairs using printf.
 * 
 * @param   key
 * @param   val
 *
 * @return  void
 */
typedef void (*print_func)( void* key, void* val );

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

HashTable createHT(hash_func hash, comp_func comp, destroy_key key, destroy_val val, print_func print);

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
 
void destroyHT(HashTable table);

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

int insertHT(HashTable table, void *key, void *val);

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

void *searchHT(HashTable table, void *key);  

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

int removeHT(HashTable table, void *key);

/* Rehash
 *
 * Increase the number of buckets and redistribute the keys accordingly.
 *
 * @param   table
 *
 * @return  void
 */

void rehash(HashTable table);

/* toString
 *
 * Prints out the hashtable and it's contents.
 *
 * @param   table       HashTable to print
 *
 * @return  void
 */
 
void toStringHT(HashTable table);

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
 
HTIterator createIterHT(HashTable table);

/* destroyIterHT
 *
 * Destroys an existing HTIterator object. If NULL is passed in,
 * nothing will happen.
 *
 * @param       iter        hashtable iterator to destroy
 *
 * @return      void
 */
 
void destroyIterHT(HTIterator iter);

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

int HTNextItem(HTIterator iter, void** key, void** val);

/* getNumBuckets
 *
 * Returns the current number of buckets.
 *
 * @param       table           hashtable object
 *
 * @return      int             number of buckets
 */

int getNumBuckets(HashTable table);


#endif
/* END SWIFT_HASHTABLE_H_ */
