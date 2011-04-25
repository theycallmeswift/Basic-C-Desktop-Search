/*
 * File: hashtable.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: March 8th, 2011
 * Date Modified: April 23rd, 2011
 */

#ifndef SWIFT_HASHTABLE_H_
#define SWIFT_HASHTABLE_H_

#define LF 0.65

/* Define our HashTable Object */
struct HashTable_;
typedef struct HashTable_* HashTable;

/* Pointer to a Hashing Function */
typedef unsigned long (*hash_func)( void *key );

/* Pointer to a Comparison function. 
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

/* createHT
 *
 * Function to create a new HashTable. Asserts that size and LoadFactor are
 * both greater than one and the hash and comp do not point to NULL.  The
 * destroy_key and destroy_val functions are both optional, but if you do not
 * provide them, it becomes the users responsibility to iterate over the table
 * and free any alllocated space.
 *
 * @param   size        initial size
 * @param   loadFactor  initial load factor
 * @param   hash        function for hashing key/value pairs
 * @param   comp        function for comparing key/value pairs
 * @param   key         function for destroying the key of key/value pairs
 * @param   val         function for destroying the value of key/value pairs
 *
 * @return  success     newly created HashTable struct
 * @return  failure     NULL
 */ 

HashTable createHT(hash_func hash, comp_func comp, destroy_key key, destroy_val val);

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
 */

int insertHT(HashTable table, void *key, void *val);

#endif
/* END SWIFT_HASHTABLE_H_ */
