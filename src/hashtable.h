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

/* Define our HashTable Object */
struct HashTable_;
typedef struct HashTable_* HashTable;

/* Pointer to a Hashing Function */
typedef unsigned long (*hash_func)( const char *key );

/* Pointer to a Comparison function. 
 *
 * @case    obj1 < obj2    -1
 * @case    obj1 = obj2     0
 * @case    obj1 > obj2     1
 */
typedef int (*comp_func)( void* obj1, void* obj2 );

/* createHT
 *
 * Function to create a new HashTable. Asserts that size and LoadFactor are
 * both greater than one and the hash and comp do not point to NULL.
 *
 * @param   size        initial size
 * @param   loadFactor  initial load factor
 * @param   hash        function for hashing key/value pairs
 * @param   comp        function for comparing key/value pairs
 *
 * @return  success     newly created HashTable struct
 * @return  failure     NULL
 */ 

HashTable createHT(int size, float loadFactor, hash_func hash, comp_func comp);



#endif
/* END SWIFT_HASHTABLE_H_ */
