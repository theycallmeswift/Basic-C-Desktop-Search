/*
 * File: cache.h
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 4th, 2011
 * Date Modified: May 4th, 2011
 */
 
#ifndef SWIFT_CACHE_H_
#define SWIFT_CACHE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "words.h"

/********************************
 * 1. Constants                 *
 ********************************/

#define CACHE_DEBUG 0


/********************************
 * 2. Structs & Typedefs        *
 ********************************/

struct Block_;
typedef struct Block_* Block;

struct Cache_;
typedef struct Cache_* Cache;

/********************************
 * 3. Functions                 *
 ********************************/

/* createCache
 *
 * Function to create a new cache struct.  Returns the new struct on success
 * and NULL on failure.
 *
 * @param   cache_size      size of cache in bytes
 *
 * @return  success         new Cache
 * @return  failure         NULL
 */
 
Cache createCache(char* cache_size);

/* destroyCache
 * 
 * Function that destroys a created cache. Frees all allocated memory. If 
 * you pass in NULL, nothing happens. So make sure to set your cache = NULL
 * after you destroy it to prevent a double free.
 *
 * @param   cache           cache object to be destroyed
 *
 * @return  void
 */
 
void destroyCache(Cache cache);

/* printCache
 *
 * Function that prints out a detailed summary of the cache
 *
 * @param   cache       cache to print
 *
 * @return  void
 */
void printCache(Cache cache);

/* insertWord
 * 
 * Inserts a word into the cache.  If the cache is full,
 * words are cleared out 1 at a time until either there
 * are no blocks left or there is enough room for the new
 * word.
 * 
 * @param   cache           Cache object
 * @param   word            word to insert
 *
 * @return  success         1 = nothing was removed, -1 = removed
 * @return  failure         0
 */

int insertWord(Cache cache, Word word);

Word searchCache(Cache cache, char* str);


#endif
/* SWIFT_CACHE_H_ */
